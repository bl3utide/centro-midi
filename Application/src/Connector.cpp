#include "Common.hpp"
#include "Annotation.hpp"
#include "Connector.hpp"
#include "Error.hpp"
#include "MessageTask.hpp"
#include "State.hpp"
#ifdef _DEBUG
#include "Logger.hpp"
#endif

namespace CentroMidi
{
namespace Connector
{

// public
MidiConnection conn;
std::vector<std::string> in_name_list;
std::vector<std::string> out_name_list;
bool force_adjust_midi_channel = true;  // send note off/on messages
int display_midi_channel = 1;           // transmit_midi_channel + 1
int display_bank = 1;                   // transmit_bank + 1
int display_program_change = 1;         // transmit_program_change + 1
#ifdef _DEBUG
std::list<ProcessedMidiMessage> processed_history;
int history_selected_index = -1;
ProcessedMidiMessage selected_processed_message;
#endif

// private
int transmit_midi_channel;      // 0 to 15;         midi ch actually sent
int transmit_bank;              // 0 to 16 ^ 2 - 1; bank actually sent
int transmit_program_change;    // 0 to 127;        pc actually sent
SDL_TimerID _waiting_timer;
const int MIN_TRANSMIT_MIDI_CHANNEL = 0;
const int MAX_TRANSMIT_MIDI_CHANNEL = 15;
const int MIN_TRANSMIT_BANK = 0;
const int MAX_TRANSMIT_BANK = 128 * 128 - 1;
const int MIN_TRANSMIT_PROGRAM_CHANGE = 0;
const int MAX_TRANSMIT_PROGRAM_CHANGE = 127;
#ifdef _DEBUG
size_t _processed_history_max_size = 100;
#endif
bool _is_both_devices_connected;

#ifdef _DEBUG
void addProcessedHistory(const bool transmitted, const std::string& device_name, const MessageHandler::Bytes& data)
{
    auto now = std::chrono::system_clock::now();
    auto now_as_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream now_ss;
    now_ss << std::put_time(std::localtime(&now_as_time_t), "%F %T")
        << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
    std::string timestamp = now_ss.str();

    auto description = MessageHandler::getMessageDesc(data);

    processed_history.push_front(ProcessedMidiMessage(timestamp, transmitted, device_name, description, data));

    if (processed_history.size() > _processed_history_max_size)
    {
        processed_history.resize(_processed_history_max_size);
    }

    if (history_selected_index != -1) ++history_selected_index;
}
#endif

void fetchDeviceList()
{
    // MIDI IN Port
    in_name_list.clear();
    unsigned int inPortNum = conn.input->getPortCount();
    for (unsigned int i = 0; i < inPortNum; ++i)
    {
        try
        {
            in_name_list.push_back(conn.input->getPortName(i));
            in_name_list[i] = in_name_list[i].substr(0, in_name_list[i].find_last_of(" "));
        }
        catch (RtMidiError& error)
        {
#ifdef _DEBUG
            LOGD << error.getMessage();
#endif
            throw error;
        }
    }

    // MIDI OUT Port
    out_name_list.clear();
    unsigned int outPortNum = conn.output->getPortCount();
    for (unsigned int i = 0; i < outPortNum; ++i)
    {
        try
        {
            out_name_list.push_back(conn.output->getPortName(i));
            out_name_list[i] = out_name_list[i].substr(0, out_name_list[i].find_last_of(" "));
        }
        catch (RtMidiError& error)
        {
#ifdef _DEBUG
            LOGD << error.getMessage();
#endif
            throw error;
        }
    }
}

/*******************************************************************************
    Received message from input device callback
*******************************************************************************/
void receiveInputDeviceMessageCallback(double delta_time, MessageHandler::Bytes* message, void* user_data)
{
    if (MessageHandler::isNoteOff(*message) || MessageHandler::isNoteOn(*message))
    {
        if (Connector::force_adjust_midi_channel)
        {
            const int ch = transmit_midi_channel;
            MessageHandler::Bytes channel_adj_message;
            if (MessageHandler::isNoteOff(*message))
            {
                channel_adj_message = {
                    static_cast<unsigned char>(0x80 + ch),
                    message->at(1),
                    message->at(2)
                };
            }
            else
            {
                channel_adj_message = {
                    static_cast<unsigned char>(0x90 + ch),
                    message->at(1),
                    message->at(2)
                };
            }
            conn.output->sendMessage(&channel_adj_message);
        }
        else
        {
            conn.output->sendMessage(message);
        }
    }
}

void initialize()
{
    conn.initialize();
    updateTransmitMidiChannel();
    updateTransmitBank();
    updateTransmitProgramChange();
}

void finalize() noexcept
{
    conn.finalize();
}

void resetAllConnections()
{
    conn.closePorts();
    conn.resetPortInfo();
    fetchDeviceList();
    setBothDevicesConnected(false);
}

void checkOpenInputPort()
{
    conn.input->cancelCallback();

    try
    {
        conn.openInPort();
        conn.last_in_connected_port_index = conn.input_port_index;
        conn.last_in_failed_port_index = -1;
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(format("MIDI error: %s", error.getMessage().c_str()));
        setBothDevicesConnected(false);
        conn.last_in_failed_port_index = conn.input_port_index;
        conn.last_in_connected_port_index = -1;
        return;
    }

    // receive message in callback function
    conn.input->setCallback(receiveInputDeviceMessageCallback);
    conn.input->ignoreTypes(false, false, false);

    if (conn.output_port_index != -1)
        setBothDevicesConnected(true);
}

void checkOpenOutputPort()
{
    try
    {
        conn.openOutPort();
        conn.last_out_connected_port_index = conn.output_port_index;
        conn.last_out_failed_port_index = -1;
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(format("MIDI error: %s", error.getMessage().c_str()));
        setBothDevicesConnected(false);
        conn.last_out_failed_port_index = conn.output_port_index;
        conn.last_out_connected_port_index = -1;
        return;
    }

    if (conn.input_port_index != -1)
        setBothDevicesConnected(true);
}

void sendBankSelectMsb()
{
    MessageHandler::Bytes bank_select_msb =
        MessageHandler::getBankSelectMsbMessage(transmit_midi_channel, transmit_bank);

    try
    {
        conn.output->sendMessage(&bank_select_msb);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(error.getMessage().c_str());
        setNextState(State::Idle);
        return;
    }
#ifdef _DEBUG
    addProcessedHistory(true, conn.output_port_name, bank_select_msb);
#endif
}

void sendBankSelectLsb()
{
    MessageHandler::Bytes bank_select_lsb =
        MessageHandler::getBankSelectLsbMessage(transmit_midi_channel, transmit_bank);

    try
    {
        conn.output->sendMessage(&bank_select_lsb);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(error.getMessage().c_str());
        setNextState(State::Idle);
        return;
    }
#ifdef _DEBUG
    addProcessedHistory(true, conn.output_port_name, bank_select_lsb);
#endif
}

void sendProgChange()
{
    MessageHandler::Bytes prog_change =
        MessageHandler::getProgChangeMessage(transmit_midi_channel, transmit_program_change);

    try
    {
        conn.output->sendMessage(&prog_change);
        setNextState(State::Idle);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(error.getMessage().c_str());
        setNextState(State::Idle);
        return;
    }
#ifdef _DEBUG
    addProcessedHistory(true, conn.output_port_name, prog_change);
#endif
}

void sendAllSoundOff()
{
    MessageHandler::Bytes all_sound_off = MessageHandler::getAllSoundOffMessage(transmit_midi_channel);

    try
    {
        conn.output->sendMessage(&all_sound_off);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        setAppError(error.getMessage().c_str());
        return;
    }
#ifdef _DEBUG
    addProcessedHistory(true, conn.output_port_name, all_sound_off);
#endif
}

void sendOneTaskMessage()
{
    if (MessageTask::taskSize() > 0)
    {
        MessageHandler::Bytes message = MessageTask::lastTask();
        conn.output->sendMessage(&message);
#ifdef _DEBUG
        addProcessedHistory(true, conn.output_port_name, message);
#endif
    }
}

void updateTransmitMidiChannel() noexcept
{
    transmit_midi_channel = display_midi_channel - 1;
}

void updateTransmitBank() noexcept
{
    transmit_bank = display_bank - 1;
}

void updateTransmitProgramChange() noexcept
{
    transmit_program_change = display_program_change - 1;
}

int getMinTransmitMidiChannel() noexcept { return MIN_TRANSMIT_MIDI_CHANNEL; }
int getMaxTransmitMidiChannel() noexcept { return MAX_TRANSMIT_MIDI_CHANNEL; }
int getMinTransmitBank() noexcept { return MIN_TRANSMIT_BANK; }
int getMaxTransmitBank() noexcept { return MAX_TRANSMIT_BANK; }
int getMinTransmitProgramChange() noexcept { return MIN_TRANSMIT_PROGRAM_CHANGE; }
int getMaxTransmitProgramChange() noexcept { return MAX_TRANSMIT_PROGRAM_CHANGE; }

bool isBothDevicesConnected() noexcept
{
    return _is_both_devices_connected;
}

void setBothDevicesConnected(const bool connected)
{
    if (connected)
    {
        _is_both_devices_connected = true;
        Annotation::clearAnnotation();
    }
    else
    {
        _is_both_devices_connected = false;
        Annotation::setAnnotation(Annotation::Message::DisconnectedAlert, Annotation::Type::Alert);
    }
}

} // Connector
} // CentroMidi