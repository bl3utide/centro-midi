#ifdef _DEBUG
#pragma once
#include "midi/message_handler.hpp"

namespace CentroMidi
{
namespace Connector
{
namespace Debug
{

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

void addProcessedHistory(const bool transmitted, const std::string& device_name, const MessageHandler::Bytes& data);

} // Debug
} // Connector
} // CentroMidi
#endif
