﻿#ifdef _DEBUG
#pragma once

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
    ByteVec data;
    std::string list_title;

    explicit ProcessedMidiMessage()
        : timestamp(""), transmitted(true), device_name(""), description("")
        , data(ByteVec()), list_title("")
    {
    }

    explicit ProcessedMidiMessage(
        const std::string& ts,
        bool t,
        const std::string& d_name,
        const std::string& desc,
        const ByteVec& d)
        : timestamp(ts), transmitted(t), device_name(d_name)
        , description(desc)
    {
        data.clear();
        data = d;

        list_title = std::format("{0} {1} {2}",
            transmitted ? "T" : "R",
            timestamp,
            description);
    }
};
extern std::vector<ProcessedMidiMessage> history;
extern int history_selected_index;
extern ProcessedMidiMessage history_selected;
extern std::mutex history_mutex;

void addProcessedHistory(bool transmitted, const std::string& device_name, const ByteVec& data);

} // Debug
} // Connector
} // CentroMidi
#endif
