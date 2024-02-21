#include "Common.hpp"
#ifdef _DEBUG
#include "Error.hpp"
#include "gui/Gui.hpp"
#include "gui/GuiColor.hpp"
#include "gui/GuiUtil.hpp"
#include "midi/Connector.hpp"
#include "midi/MessageTask.hpp"
#include "Logger.hpp"

namespace CentroMidi
{
namespace Gui
{

// private
bool _show_debug_menu_bar = true;
bool _show_demo_window = false;
bool _show_debug_window = false;
bool _use_alternative_child_bg = false;
bool _show_processed_message_window = false;
int _selected_debug_log_index = -1;
Logger::Log _selected_debug_log;

void drawDebugMenuBar(const ImVec2 viewport_pos)
{
    ImGui::PushFont((int)Font::Debug);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.35f, 0.35f, 0.35f, 0.65f));
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(ImVec2(center.x - 140.0f, viewport_pos.y), ImGuiCond_Always);
    ImGui::Begin("debug_control", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoTitleBar);
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 3.0f));
        if (_show_debug_menu_bar)
        {
            if (ImGui::Button("_")) _show_debug_menu_bar = false;
        }
        else
        {
            if (ImGui::Button(">")) _show_debug_menu_bar = true;
        }
        ImGui::MouseCursorToHand();
        ImGui::PopStyleVar();

        if (_show_debug_menu_bar)
        {
            ImGui::Checkbox("demo", &_show_demo_window);
            ImGui::MouseCursorToHand();
            ImGui::SameLine();
            ImGui::Checkbox("debug", &_show_debug_window);
            ImGui::MouseCursorToHand();
            ImGui::SameLine();
            ImGui::Checkbox("alt_child_bg", &_use_alternative_child_bg);
            ImGui::MouseCursorToHand();
        }
    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopFont();
}

void pushDebugStyles() noexcept
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.282f, 0.282f, 0.282f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.034f, 0.035f, 0.086f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.034f, 0.035f, 0.086f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.034f, 0.035f, 0.086f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.111f, 0.178f, 0.301f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.069f, 0.111f, 0.188f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.069f, 0.111f, 0.188f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.09f, 0.09f, 0.12f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.24f, 0.24f, 0.28f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.24f, 0.24f, 0.28f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.275f, 0.275f, 0.275f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.380f, 0.380f, 0.380f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.427f, 0.427f, 0.427f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.275f, 0.275f, 0.275f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.965f, 0.965f, 0.965f, 1.0f));
}

void popDebugStyles() noexcept
{
    ImGui::PopStyleColor(17);
    ImGui::PopStyleVar();
}

void drawDebugTabItemGeneral()
{
    if (ImGui::BeginTabItem("General"))
    {
        ImGui::Text("%-24s: %s", "is both devices connected", Connector::isBothDevicesConnected() ? "Yes" : "No");
        ImGui::Text("%-24s: %-4s (%d)%s / %-4s (%d)%s", "synth conn in/out",
            Connector::conn.input->isPortOpen() ? "open" : "-",
            Connector::conn.input_port_index,
            Connector::conn.input_port_name.c_str(),
            Connector::conn.output->isPortOpen() ? "open" : "-",
            Connector::conn.output_port_index,
            Connector::conn.output_port_name.c_str());
        ImGui::Text("%-24s: %d", "in connected port index", Connector::conn.last_in_connected_port_index);
        ImGui::Text("%-24s: %d", "in failed port index", Connector::conn.last_in_failed_port_index);
        ImGui::Text("%-24s: %d", "out connected port index", Connector::conn.last_out_connected_port_index);
        ImGui::Text("%-24s: %d", "out failed port index", Connector::conn.last_out_failed_port_index);
        ImGui::Text("%-24s: %s", "force midi channel", Connector::force_adjust_midi_channel ? "Yes" : "No");
        ImGui::Text("%-24s: %d", "transmit midi channel", Connector::display_midi_channel - 1);
        ImGui::Text("%-24s: %d", "transmit bank", Connector::display_bank - 1);
        ImGui::Text("%-24s: %d", "transmit program change", Connector::display_program_change - 1);

        ImGui::Separator(); //--------------------------------------------------

        ImGui::Text("%-24s: %d(max %d)", "task size", MessageTask::taskSize(), MessageTask::largestTaskSizeEver());

        ImGui::EndTabItem();
    }
}

void drawDebugTabItemTransReceiveLog()
{
    if (ImGui::BeginTabItem("Transmitted/Received Log"))
    {
        ImGui::BeginChild("processed_list", ImVec2(600, 500), false);
        {
            int selected_index = 0;
            std::list<Connector::ProcessedMidiMessage> ph_copy = Connector::processed_history;
            for (auto iter = ph_copy.begin(); iter != ph_copy.end(); ++iter)
            {
                bool is_selected = selected_index == Connector::history_selected_index;
                ImGui::PushStyleColor(ImGuiCol_Text, iter->transmitted ? DEBUG_UI_COLOR_TEXT_TRANSMIT : DEBUG_UI_COLOR_TEXT_RECEIVE);
                if (ImGui::Selectable(iter->list_title.c_str(), is_selected))
                {
                    Connector::history_selected_index = selected_index;

                    Connector::selected_processed_message =
                        Connector::ProcessedMidiMessage(
                            iter->timestamp,
                            iter->transmitted,
                            iter->device_name,
                            iter->description,
                            iter->data);
                    _show_processed_message_window = true;
                }
                ImGui::MouseCursorToHand();
                ImGui::PopStyleColor();
                ++selected_index;
            }
        }
        ImGui::EndChild();

        ImGui::EndTabItem();
    }
}

void drawProcessedWindow()
{
    Connector::ProcessedMidiMessage* message = &Connector::selected_processed_message;

    ImGui::Begin("processed_detail", &_show_processed_message_window,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar);
    {
        ImGui::PushFont((int)Font::DebugProcHead);
        if (message->transmitted)
            ImGui::TextColoredU32(DEBUG_UI_COLOR_TEXT_TRANSMIT, "%s", "Transmitted");
        else
            ImGui::TextColoredU32(DEBUG_UI_COLOR_TEXT_RECEIVE, "%s", "Received");
        ImGui::PopFont();

        ImGui::SameLine(300.0f);
        if (ImGui::Button("Copy to clipboard"))
        {
            std::stringstream cb;
            for (int i = 0; i < message->data.size(); ++i)
            {
                cb << std::uppercase << std::hex
                    << std::setfill('0') << std::setw(2)
                    << static_cast<int>(message->data[i]);

                if (i != message->data.size() - 1)
                    cb << " ";
            }
            ImGui::LogToClipboard();
            ImGui::LogText(cb.str().c_str());
            ImGui::LogFinish();
        }
        ImGui::MouseCursorToHand();

        ImGui::Text(message->timestamp.c_str());
        ImGui::Text("%-12s: %s %s", "Device",
            message->device_name.c_str(), message->transmitted ? "[OUT]" : "[IN]");
        ImGui::Text("%-12s: %s", "Description", message->description.c_str());
        ImGui::Text("%-12s: %d", "Size", message->data.size());

        ImGui::Separator();

        auto hex_space = 30.0f;

        ImGui::PushFont((int)Font::DebugProcHex);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f));
        ImGui::Dummy(ImVec2(0.0f, 0.0f));
        ImGui::SameLine(100);
        ImGui::BeginChild("processed_detail_header", ImVec2(340.0f, 18.0f));
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            for (int i = 0; i < 10; ++i)
            {
                ImGui::Text("%02X", i);
                ImGui::SameLine(hex_space * (i + 1));
            }
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::BeginChild("processed_detail_content", ImVec2(400.0f, 360.0f));
        {
            auto size = message->data.size();
            auto max_row_idx = size / 10;
            auto hex_indent = 94.0f;
            for (auto row_i = 0; row_i <= max_row_idx; ++row_i)
            {
                ImGui::Text("%8d:", row_i * 10);
                ImGui::SameLine(hex_indent);
                for (auto col_i = 0; col_i < 10; ++col_i)
                {
                    auto current_index = row_i * 10 + col_i;
                    if (current_index < size)
                    {
                        ImGui::Text("%02X", message->data[current_index]);
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::Text("Index: %d, Data: %02X(%d)", current_index, message->data[current_index], message->data[current_index]);
                            ImGui::EndTooltip();
                        }
                        if (col_i != 9) ImGui::SameLine(hex_space * (col_i + 1) + hex_indent);
                    }
                    else
                        break;
                }
            }
        }
        ImGui::EndChild();
        ImGui::PopFont();
    }
    ImGui::End();
}

void drawDebugTabItemLogger()
{
    if (ImGui::BeginTabItem("Logger"))
    {
        ImGui::Text("%d logs", Logger::logs.size());

        ImGui::BeginChild("logger_list", ImVec2(800, 430), false, 0);
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 0.0f));
            auto debug_log = Logger::logs;
            for (auto iter = debug_log.begin(); iter != debug_log.end(); ++iter)
            {
                bool is_selected = _selected_debug_log_index == iter->log_id;
                if (ImGui::Selectable(format("%05d %s", iter->log_id, iter->text.c_str()).c_str(), is_selected))
                {
                    _selected_debug_log = *iter;
                    _selected_debug_log_index = iter->log_id;
                }
                ImGui::MouseCursorToHand();
            }

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        ImGui::Separator();

        if (_selected_debug_log_index != -1)
        {
            ImGui::BeginChild("logger_detail", ImVec2(800, 70), false, 0);
            {
                ImGui::Text("Log ID %d [%s]", _selected_debug_log.log_id, _selected_debug_log.category.c_str());
                ImGui::Text("%s", _selected_debug_log.timestamp.c_str());
                ImGui::Text("%s (LINE %s)", _selected_debug_log.function.c_str(), _selected_debug_log.line.c_str());
                ImGui::Text("%s", _selected_debug_log.text.c_str());
            }
            ImGui::EndChild();
        }

        ImGui::EndTabItem();
    }
}

void drawDebugWindow(bool* open, const int window_w, const int window_h,
    const State current_state)
{
    pushDebugStyles();

    ImGui::Begin("debug", open,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar);
    {
        ImGui::Text("%dx%d", window_w, window_h);
        ImGui::SameLine();
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::Separator(); //--------------------------------------------------

        ImGui::Text("%-24s: %d / %d", "has_error / showing?", has_error ? 1 : 0, showing_error_message ? 1 : 0);

        if (0 <= static_cast<int>(current_state)
            && static_cast<int>(current_state) <= static_cast<int>(State::None))
        {
            ImGui::Text("%-24s: [%d]%s", "state",
                current_state, STATE_STR[static_cast<int>(current_state)]);
        }

        State next_state = getNextState();
        if (0 <= static_cast<int>(next_state)
            && static_cast<int>(next_state) <= static_cast<int>(State::None))
        {
            ImGui::Text("%-24s: [%d]%s", "next state",
                next_state, STATE_STR[static_cast<int>(next_state)]);
        }

        if (ImGui::BeginTabBar("DebugTab", ImGuiTabBarFlags_None))
        {
            drawDebugTabItemGeneral();
            drawDebugTabItemTransReceiveLog();
            drawDebugTabItemLogger();
            ImGui::EndTabBar();
        }
    }
    ImGui::End();

    if (_show_processed_message_window)
    {
        drawProcessedWindow();
    }

    popDebugStyles();
}

void drawDebugWindows(const int window_w, const int window_h, const State current_state)
{
    ImGui::PushFont((int)Font::Debug);

    if (_show_demo_window)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::ShowDemoWindow(&_show_demo_window);
        ImGui::PopStyleVar();
    }

    if (_show_debug_window)
        drawDebugWindow(&_show_debug_window, window_w, window_h, current_state);

    ImGui::PopFont();
}

bool isChildBgAlt() noexcept
{
    return _use_alternative_child_bg;
}

} // Gui
} // CentroMidi
#endif
