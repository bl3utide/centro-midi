#include "common.hpp"
#ifdef _DEBUG
#include "midi/midi_common.hpp"
#include "midi/callback_debug.hpp"
#include "midi/connector_debug.hpp"
#include "midi/message_handler.hpp"

namespace CentroMidi
{
namespace Connector
{
namespace Debug
{

// public
std::vector<ProcessedMidiMessage> history;
int history_selected_index = -1;
ProcessedMidiMessage history_selected;
std::mutex history_mutex;

// private
const size_t MAX_SIZE_DISPLAY_HISTORY = 200;

void addProcessedHistory(const bool transmitted, const std::string& device_name, const ByteVec& data)
{
    auto now = std::chrono::system_clock::now();
    auto now_as_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream now_ss;
    now_ss << std::put_time(std::localtime(&now_as_time_t), "%F %T")
        << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
    std::string timestamp = now_ss.str();

    auto description = MessageHandler::getMessageDesc(data);

    std::unique_lock lock(history_mutex);
    history.emplace_back(ProcessedMidiMessage(timestamp, transmitted, device_name, description, data));

    if (history.size() > MAX_SIZE_DISPLAY_HISTORY)
    {
        history.erase(history.begin());
        --history_selected_index;
    }
}

} // Debug
} // Connector
} // CentroMidi
#endif