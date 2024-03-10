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
std::list<ProcessedMidiMessage> processed_history;
int history_selected_index = -1;
ProcessedMidiMessage selected_processed_message;

// private
size_t _processed_history_max_size = 100;

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

    processed_history.push_front(ProcessedMidiMessage(timestamp, transmitted, device_name, description, data));

    if (processed_history.size() > _processed_history_max_size)
    {
        processed_history.resize(_processed_history_max_size);
    }

    if (history_selected_index != -1) ++history_selected_index;
}

} // Debug
} // Connector
} // CentroMidi
#endif