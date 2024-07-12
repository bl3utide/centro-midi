#include "common.hpp"
#include "annotation.hpp"
#include "error.hpp"
#include "logger.hpp"
#include "state.hpp"
#include "config/config.hpp"
#include "midi/callback.hpp"
#include "midi/connector.hpp"
#include "midi/message_handler.hpp"
#include "midi/message_task.hpp"
#ifdef _DEBUG
#include "midi/connector_debug.hpp"
#endif

namespace CentroMidi
{
namespace Connector
{

// public
InputConnection input;
OutputConnection output;
std::vector<std::string> in_name_list;
std::vector<std::string> out_name_list;
bool force_adjust_midi_channel;         // send note off/on messages
int display_midi_channel;               // transmit_midi_channel + 1
int display_bank;                       // transmit_bank + 1
int display_program_change;             // transmit_program_change + 1

// private
bool is_both_devices_connected_;
int transmit_midi_channel_;     // 0 to 15;         midi ch actually sent
int transmit_bank_;             // 0 to 16 ^ 2 - 1; bank actually sent
int transmit_program_change_;   // 0 to 127;        pc actually sent
const int MIN_TRANSMIT_MIDI_CHANNEL = 0;
const int MAX_TRANSMIT_MIDI_CHANNEL = 15;
const int MIN_TRANSMIT_BANK = 0;
const int MAX_TRANSMIT_BANK = 128 * 128 - 1;
const int MIN_TRANSMIT_PROGRAM_CHANGE = 0;
const int MAX_TRANSMIT_PROGRAM_CHANGE = 127;

void fetchDeviceList()
{
    // MIDI IN Port
    in_name_list.clear();
    const auto in_port_count = input.getPortCount();
    for (unsigned int i = 0; i < in_port_count; ++i)
    {
        try
        {
            in_name_list.push_back(input.getPortName(i));
            in_name_list[i] = in_name_list[i].substr(0, in_name_list[i].find_last_of(" "));
        }
        catch (RtMidiError& error)
        {
            throw ContinuableException(
                StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
                ERROR_WHEN_FETCH_DEV_LIST,
                ERROR_CAUSE_GET_INDEV_NAME
            );
        }
    }

    // MIDI OUT Port
    out_name_list.clear();
    const auto out_port_count = output.getPortCount();
    for (unsigned int i = 0; i < out_port_count; ++i)
    {
        try
        {
            out_name_list.push_back(output.getPortName(i));
            out_name_list[i] = out_name_list[i].substr(0, out_name_list[i].find_last_of(" "));
        }
        catch (RtMidiError& error)
        {
            throw ContinuableException(
                StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
                ERROR_WHEN_FETCH_DEV_LIST,
                ERROR_CAUSE_GET_OUTDEV_NAME
            );
        }
    }
}

void initialize()
{
    input.initialize();
    output.initialize();
    updateTransmitMidiChannel();
    updateTransmitBank();
    updateTransmitProgramChange();

    force_adjust_midi_channel = true;
    display_midi_channel = 1;
    display_bank = 1;
    display_program_change = 1;
    is_both_devices_connected_ = false;

#ifdef _DEBUG
    Debug::history_selected_index = -1;
#endif
}

void finalize() noexcept
{
    input.finalize();
    output.finalize();
}

void applyConfig()
{
    // Input Device
    const auto cv_input_device_name = Config::getConfigValue<std::string>(Config::Key::InputDevice);
    const auto in_res = std::find(in_name_list.cbegin(), in_name_list.cend(), cv_input_device_name);
    if (in_res != in_name_list.cend())
    {   // found
        const int index = static_cast<int>(std::distance(in_name_list.cbegin(), in_res));
        try
        {
            openInputPort(index, in_name_list[index]);
        }
        catch (ContinuableException& ce)
        {
            Logger::debug(ce);
        }
    }

    // Output Device
    const auto cv_output_device_name = Config::getConfigValue<std::string>(Config::Key::OutputDevice);
    const auto out_res = std::find(out_name_list.cbegin(), out_name_list.cend(), cv_output_device_name);
    if (out_res != out_name_list.cend())
    {   // found
        const auto index = static_cast<int>(std::distance(out_name_list.cbegin(), out_res));
        try
        {
            openOutputPort(index, out_name_list[index]);
        }
        catch (ContinuableException& ce)
        {
            Logger::debug(ce);
        }
    }

    // To Channel
    display_midi_channel = Config::getConfigValue<int>(Config::Key::ToChannel);
    updateTransmitMidiChannel();

    // Force Adjust MIDI Channel
    force_adjust_midi_channel = Config::getConfigValue<bool>(Config::Key::ForceAdjustMidiCh);
}

void updateConfig() noexcept
{
    Config::setConfigValue(Config::Key::InputDevice, input.getPortName());
    Config::setConfigValue(Config::Key::OutputDevice, output.getPortName());
    Config::setConfigValue(Config::Key::ToChannel, display_midi_channel);
    Config::setConfigValue(Config::Key::ForceAdjustMidiCh, force_adjust_midi_channel);
}

void resetAllConnections()
{
    input.close();
    output.close();
    fetchDeviceList();
    setBothDevicesConnected(false);
}

void openInputPort(int port_index, const std::string& port_name)
{
    try
    {
        input.open(port_index, port_name);
    }
    catch (RtMidiError& error)
    {
        setBothDevicesConnected(false);
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_OPEN_DEV, ERROR_CAUSE_OPEN_DEV_IN);
    }

    // receive message in callback function
    input.setCallback(Callback::receiveInputDeviceMessage);
    input.ignoreTypes(false, false, false);

    if (output.isPortOpen())
        setBothDevicesConnected(true);
}

void openOutputPort(int port_index, const std::string& port_name)
{
    try
    {
        output.open(port_index, port_name);
    }
    catch (RtMidiError& error)
    {
        setBothDevicesConnected(false);
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_OPEN_DEV, ERROR_CAUSE_OPEN_DEV_OUT);
    }

    if (input.isPortOpen())
        setBothDevicesConnected(true);
}

void sendBankSelectMsb()
{
    ByteVec bank_select_msb =
        MessageHandler::getBankSelectMsbMessage(transmit_midi_channel_, transmit_bank_);

    try
    {
        output.sendMessage(bank_select_msb);
    }
    catch (RtMidiError& error)
    {
        setBothDevicesConnected(false);
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_SEND_MESSAGE,
            ERROR_CAUSE_MSB,
            State::Idle
        );
    }
#ifdef _DEBUG
    Debug::addProcessedHistory(true, output.getPortName(), bank_select_msb);
#endif
}

void sendBankSelectLsb()
{
    const auto bank_select_lsb =
        MessageHandler::getBankSelectLsbMessage(transmit_midi_channel_, transmit_bank_);

    try
    {
        output.sendMessage(bank_select_lsb);
    }
    catch (RtMidiError& error)
    {
        setBothDevicesConnected(false);
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_SEND_MESSAGE,
            ERROR_CAUSE_LSB,
            State::Idle
        );
    }
#ifdef _DEBUG
    Debug::addProcessedHistory(true, output.getPortName(), bank_select_lsb);
#endif
}

void sendProgChange()
{
    const auto prog_change =
        MessageHandler::getProgChangeMessage(transmit_midi_channel_, transmit_program_change_);

    try
    {
        output.sendMessage(prog_change);
        setNextState(State::Idle);
    }
    catch (RtMidiError& error)
    {
        setBothDevicesConnected(false);
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_SEND_MESSAGE,
            ERROR_CAUSE_PROG_CHANGE,
            State::Idle
        );
    }
#ifdef _DEBUG
    Debug::addProcessedHistory(true, output.getPortName(), prog_change);
#endif
}

void sendAllSoundOff()
{
    const auto all_sound_off = MessageHandler::getAllSoundOffMessage(transmit_midi_channel_);

    try
    {
        output.sendMessage(all_sound_off);
    }
    catch (RtMidiError& error)
    {
        throw ContinuableException(
            StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
            ERROR_WHEN_SEND_MESSAGE,
            ERROR_CAUSE_ALL_SND_OFF
        );
    }
#ifdef _DEBUG
    Debug::addProcessedHistory(true, output.getPortName(), all_sound_off);
#endif
}

void sendOneTaskMessage()
{
    if (MessageTask::taskSize() > 0)
    {
        const auto message = MessageTask::lastTask();

        try
        {
            output.sendMessage(message);
        }
        catch (RtMidiError& error)
        {
            throw ContinuableException(
                StringUtil::format("MIDI error: %s", error.getMessage().c_str()).c_str(),
                ERROR_WHEN_SEND_MESSAGE,
                ERROR_CAUSE_ONE_TASK
            );
        }

#ifdef _DEBUG
        Debug::addProcessedHistory(true, output.getPortName(), message);
#endif
    }
}

void updateTransmitMidiChannel() noexcept
{
    transmit_midi_channel_ = display_midi_channel - 1;
}

void updateTransmitBank() noexcept
{
    transmit_bank_ = display_bank - 1;
}

void updateTransmitProgramChange() noexcept
{
    transmit_program_change_ = display_program_change - 1;
}

int getTransmitMidiChannel() noexcept { return transmit_midi_channel_; }

int getMinTransmitMidiChannel() noexcept { return MIN_TRANSMIT_MIDI_CHANNEL; }
int getMaxTransmitMidiChannel() noexcept { return MAX_TRANSMIT_MIDI_CHANNEL; }
int getMinTransmitBank() noexcept { return MIN_TRANSMIT_BANK; }
int getMaxTransmitBank() noexcept { return MAX_TRANSMIT_BANK; }
int getMinTransmitProgramChange() noexcept { return MIN_TRANSMIT_PROGRAM_CHANGE; }
int getMaxTransmitProgramChange() noexcept { return MAX_TRANSMIT_PROGRAM_CHANGE; }

bool isBothDevicesConnected() noexcept
{
    return is_both_devices_connected_;
}

void setBothDevicesConnected(bool connected)
{
    if (connected)
    {
        is_both_devices_connected_ = true;
        Annotation::clearText();
    }
    else
    {
        is_both_devices_connected_ = false;
        Annotation::setText(Annotation::Message::DisconnectedAlert, Annotation::Type::Alert);
    }
}

} // Connector
} // CentroMidi