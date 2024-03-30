#include "common.hpp"
#include "annotation.hpp"
#include "error.hpp"
#include "state.hpp"
#include "config/config.hpp"
#include "midi/callback.hpp"
#include "midi/connector.hpp"
#include "midi/message_handler.hpp"
#include "midi/message_task.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#include "midi/connector_debug.hpp"
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

// private
int _transmit_midi_channel;     // 0 to 15;         midi ch actually sent
int _transmit_bank;             // 0 to 16 ^ 2 - 1; bank actually sent
int _transmit_program_change;   // 0 to 127;        pc actually sent
const int MIN_TRANSMIT_MIDI_CHANNEL = 0;
const int MAX_TRANSMIT_MIDI_CHANNEL = 15;
const int MIN_TRANSMIT_BANK = 0;
const int MAX_TRANSMIT_BANK = 128 * 128 - 1;
const int MIN_TRANSMIT_PROGRAM_CHANGE = 0;
const int MAX_TRANSMIT_PROGRAM_CHANGE = 127;
bool _is_both_devices_connected;

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

void applyConfig()
{
    // Input Device
    const std::string cv_input_device_name = Config::getConfigValue<std::string>(Config::Key::InputDevice);
    const auto in_res = std::find(in_name_list.cbegin(), in_name_list.cend(), cv_input_device_name);
    if (in_res != in_name_list.cend())
    {   // found
        const int index = static_cast<int>(std::distance(in_name_list.cbegin(), in_res));
        conn.input_port_index = index;
        conn.input_port_name = in_name_list[index];
        checkOpenInputPort();
    }

    // Output Device
    const std::string cv_output_device_name = Config::getConfigValue<std::string>(Config::Key::OutputDevice);
    const auto out_res = std::find(out_name_list.cbegin(), out_name_list.cend(), cv_output_device_name);
    if (out_res != out_name_list.cend())
    {   // found
        const int index = static_cast<int>(std::distance(out_name_list.cbegin(), out_res));
        conn.output_port_index = index;
        conn.output_port_name = out_name_list[index];
        checkOpenOutputPort();
    }

    // To Channel
    display_midi_channel = Config::getConfigValue<int>(Config::Key::ToChannel);
    updateTransmitMidiChannel();

    // Force Adjust MIDI Channel
    force_adjust_midi_channel = Config::getConfigValue<bool>(Config::Key::ForceAdjustMidiCh);
}

void updateConfig() noexcept
{
    const std::string in_device_name = conn.last_in_connected_port_index != -1
        ? conn.input_port_name
        : "";
    const std::string out_device_name = conn.last_out_connected_port_index != -1
        ? conn.output_port_name
        : "";
    Config::setConfigValue(Config::Key::InputDevice, in_device_name);
    Config::setConfigValue(Config::Key::OutputDevice, out_device_name);
    Config::setConfigValue(Config::Key::ToChannel, display_midi_channel);
    Config::setConfigValue(Config::Key::ForceAdjustMidiCh, force_adjust_midi_channel);
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
        setAppError(StringUtil::format("MIDI error: %s", error.getMessage().c_str()));
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
        setAppError(StringUtil::format("MIDI error: %s", error.getMessage().c_str()));
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
    ByteVec bank_select_msb =
        MessageHandler::getBankSelectMsbMessage(_transmit_midi_channel, _transmit_bank);

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
    Debug::addProcessedHistory(true, conn.output_port_name, bank_select_msb);
#endif
}

void sendBankSelectLsb()
{
    ByteVec bank_select_lsb =
        MessageHandler::getBankSelectLsbMessage(_transmit_midi_channel, _transmit_bank);

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
    Debug::addProcessedHistory(true, conn.output_port_name, bank_select_lsb);
#endif
}

void sendProgChange()
{
    ByteVec prog_change =
        MessageHandler::getProgChangeMessage(_transmit_midi_channel, _transmit_program_change);

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
    Debug::addProcessedHistory(true, conn.output_port_name, prog_change);
#endif
}

void sendAllSoundOff()
{
    ByteVec all_sound_off = MessageHandler::getAllSoundOffMessage(_transmit_midi_channel);

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
    Debug::addProcessedHistory(true, conn.output_port_name, all_sound_off);
#endif
}

void sendOneTaskMessage()
{
    if (MessageTask::taskSize() > 0)
    {
        ByteVec message = MessageTask::lastTask();
        conn.output->sendMessage(&message);
#ifdef _DEBUG
        Debug::addProcessedHistory(true, conn.output_port_name, message);
#endif
    }
}

void updateTransmitMidiChannel() noexcept
{
    _transmit_midi_channel = display_midi_channel - 1;
}

void updateTransmitBank() noexcept
{
    _transmit_bank = display_bank - 1;
}

void updateTransmitProgramChange() noexcept
{
    _transmit_program_change = display_program_change - 1;
}

int getTransmitMidiChannel() noexcept { return _transmit_midi_channel; }

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
        Annotation::clearText();
    }
    else
    {
        _is_both_devices_connected = false;
        Annotation::setText(Annotation::Message::DisconnectedAlert, Annotation::Type::Alert);
    }
}

} // Connector
} // CentroMidi