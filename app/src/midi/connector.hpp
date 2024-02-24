#pragma once

#include "midi/message_handler.hpp"
#include "util/string_util.hpp"

namespace CentroMidi
{
namespace Connector
{

struct MidiConnection
{
    RtMidiIn* input = nullptr;
    RtMidiOut* output = nullptr;
    int input_port_index = -1;
    int output_port_index = -1;
    std::string input_port_name;
    std::string output_port_name;
    int last_in_connected_port_index = -1;
    int last_in_failed_port_index = -1;
    int last_out_connected_port_index = -1;
    int last_out_failed_port_index = -1;

    void initialize()
    {
        input = new RtMidiIn();
        output = new RtMidiOut();
    }

    void finalize()
    {
        input->cancelCallback();
        input->closePort();
        if (input != nullptr)
        {
            delete input;
            input = nullptr;
        }

        output->closePort();
        if (output != nullptr)
        {
            delete output;
            output = nullptr;
        }
    }

    void openInPort()
    {
        input->cancelCallback();
        input->closePort();
        input->openPort(input_port_index);
    }

    void openOutPort()
    {
        output->closePort();
        output->openPort(output_port_index);
    }

    void closePorts()
    {
        input->cancelCallback();
        input->closePort();
        output->closePort();
    }

    void resetPortInfo()
    {
        input_port_index = -1;
        input_port_name = "";
        output_port_index = -1;
        output_port_name = "";
        last_in_connected_port_index = -1;
        last_in_failed_port_index = -1;
        last_out_connected_port_index = -1;
        last_out_failed_port_index = -1;
    }

    void operator=(const MidiConnection& conn)
    {
        input_port_index = conn.input_port_index;
        input_port_name = conn.input_port_name;
        output_port_index = conn.output_port_index;
        output_port_name = conn.output_port_name;
    }
};

extern MidiConnection conn;
extern std::vector<std::string> in_name_list;
extern std::vector<std::string> out_name_list;
extern bool force_adjust_midi_channel;
extern int display_midi_channel;
extern int display_bank;
extern int display_program_change;

#ifdef _DEBUG
struct ProcessedMidiMessage
{
    std::string timestamp;
    bool transmitted;               // true: transmitted, false: received
    std::string device_name;
    std::string description;
    MessageHandler::Bytes data;
    std::string list_title;

    ProcessedMidiMessage()
    {
        timestamp = "";
        transmitted = true;
        device_name = "";
        description = "";
        data = MessageHandler::Bytes();
        list_title = "";
    }

    ProcessedMidiMessage(
        const std::string& ts,
        const bool t,
        const std::string& d_name,
        const std::string& desc,
        const MessageHandler::Bytes d)
    {
        timestamp = ts;
        transmitted = t;
        device_name = d_name;
        description = desc;
        data.clear();
        data = d;

        list_title = format("%s %s %s",
            transmitted ? "T" : "R",
            timestamp.c_str(),
            description.c_str());
    }
};
extern std::list<ProcessedMidiMessage> processed_history;
extern int history_selected_index;
extern ProcessedMidiMessage selected_processed_message;
#endif

void initialize();
void finalize() noexcept;
void resetAllConnections();
void checkOpenInputPort();
void checkOpenOutputPort();
void sendBankSelectMsb();
void sendBankSelectLsb();
void sendProgChange();
void sendAllSoundOff();
void sendOneTaskMessage();
void updateTransmitMidiChannel() noexcept;
void updateTransmitBank() noexcept;
void updateTransmitProgramChange() noexcept;
int getMinTransmitMidiChannel() noexcept;
int getMaxTransmitMidiChannel() noexcept;
int getMinTransmitBank() noexcept;
int getMaxTransmitBank() noexcept;
int getMinTransmitProgramChange() noexcept;
int getMaxTransmitProgramChange() noexcept;
bool isBothDevicesConnected() noexcept;
void setBothDevicesConnected(const bool connected);

} // Connector
} // CentroMidi
