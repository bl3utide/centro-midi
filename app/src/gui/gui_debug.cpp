#include "common.hpp"
#ifdef _DEBUG
#include "error.hpp"
#include "config/config.hpp"
#include "config/cv.hpp"
#include "config/section.hpp"
#include "gui/gui.hpp"
#include "gui/gui_color.hpp"
#include "gui/gui_font.hpp"
#include "midi/connector.hpp"
#include "midi/connector_debug.hpp"
#include "midi/message_task.hpp"
#include "logger.hpp"

namespace CentroMidi
{
namespace Gui
{
namespace Debug
{

// private
bool _show_debug_menu_bar = true;
bool _show_demo_window = false;
bool _show_debug_window = true;
bool _show_processed_message_window = false;
//int _selected_debug_log_index = -1;
//Logger::DisplayFormattedDebugLog _selected_debug_log;

void drawDebugMenuBar(const ImVec2 viewport_pos)
{
    GuiUtil::PushFont((int)FontDebug::Text);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.35f, 0.35f, 0.35f, 0.65f));
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(ImVec2(center.x - 80.0f, viewport_pos.y), ImGuiCond_Always);
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
        GuiUtil::MouseCursorToHand();
        ImGui::PopStyleVar();

        if (_show_debug_menu_bar)
        {
            ImGui::Checkbox("demo", &_show_demo_window);
            GuiUtil::MouseCursorToHand();
            ImGui::SameLine();
            ImGui::Checkbox("debug", &_show_debug_window);
            GuiUtil::MouseCursorToHand();
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
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.111f, 0.178f, 0.301f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.111f, 0.178f, 0.301f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.111f, 0.178f, 0.301f, 1.0f));
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
        ImGui::Text("%-24s: %-4s [%2d]%s", "input port",
            Connector::input.isPortOpen() ? "open" : "-",
            Connector::input.getPortIndex(),
            Connector::input.getPortName().c_str());
        ImGui::Text("%-24s: %-4s [%2d]%s", "output port",
            Connector::output.isPortOpen() ? "open" : "-",
            Connector::output.getPortIndex(),
            Connector::output.getPortName().c_str());
        ImGui::Text("%-24s", "last connected port index");
        ImGui::Text(" %-23s: %d", "input", Connector::input.getLastConnectedPortIndex());
        ImGui::Text(" %-23s: %d", "output", Connector::output.getLastConnectedPortIndex());
        ImGui::Text("%-24s", "last failed port index");
        ImGui::Text(" %-23s: %d", "input", Connector::input.getLastFailedPortIndex());
        ImGui::Text(" %-23s: %d", "output", Connector::output.getLastFailedPortIndex());
        ImGui::Text("%-24s: %s", "force midi channel", Connector::force_adjust_midi_channel ? "Yes" : "No");
        ImGui::Text("%-24s: %d", "transmit midi channel", Connector::display_midi_channel - 1);
        ImGui::Text("%-24s: %d", "transmit bank", Connector::display_bank - 1);
        ImGui::Text("%-24s: %d", "transmit program change", Connector::display_program_change - 1);

        ImGui::Separator(); //--------------------------------------------------

        ImGui::Text("%-24s: %d(max %d)", "task size", MessageTask::taskSize(), MessageTask::largestTaskSizeEver());

        ImGui::EndTabItem();
    }
}

void drawDebugTabItemConfig()
{
    if (ImGui::BeginTabItem("Config"))
    {
        ImGui::BeginChild("config_value_list", ImVec2(600, 300));
        {
            auto drawParamsRow = [](const Config::Key key)
            {
                const Config::Cv cv = Config::getCv(key);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", cv.section_name().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", cv.key_name().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", cv.type_str().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", cv.cv().c_str());
            };

            ImGui::Text("Loaded config values");

            if (ImGui::BeginTable("config_values", 4, ImGuiTableFlags_Borders
                | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
            {
                ImGui::TableSetupColumn("Section", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                ImGui::TableHeadersRow();

                for (int key_i = 0; key_i < static_cast<int>(Config::Key::_COUNT_); ++key_i)
                {
                    drawParamsRow(static_cast<Config::Key>(key_i));
                }

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();

        ImGui::EndTabItem();
    }
}

void drawDebugTabItemTransReceiveLog()
{
    namespace cd = Connector::Debug;

    if (ImGui::BeginTabItem("Transmitted/Received Log"))
    {
        std::unique_lock lock(cd::history_mutex);

        ImGui::BeginChild("processed_list", ImVec2(800, 500), false);
        {
            int selected_index = 0;
            std::vector<cd::ProcessedMidiMessage> ph_copy = cd::history;
            for (auto iter = ph_copy.cbegin(); iter != ph_copy.cend(); ++iter)
            {
                const bool is_selected = selected_index == cd::history_selected_index;
                ImGui::PushStyleColor(ImGuiCol_Text, iter->transmitted ? DEBUG_UI_COLOR_TEXT_TRANSMIT : DEBUG_UI_COLOR_TEXT_RECEIVE);
                if (ImGui::Selectable(iter->list_title.c_str(), is_selected))
                {
                    cd::history_selected_index = selected_index;
                    cd::history_selected  = *iter;
                    _show_processed_message_window = true;
                }
                GuiUtil::MouseCursorToHand();
                ImGui::PopStyleColor();
                ++selected_index;
            }

            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
        if (ImGui::IsItemHovered())
        {
            auto f = []()
            {
                auto idx = cd::history_selected_index;
                cd::history_selected = cd::ProcessedMidiMessage(cd::history[idx]);
            };

            if (GuiUtil::IsCustomKeyPressed(GuiUtil::ImGuiCustomKey::Up, true) && cd::history_selected_index > 0)
            {
                --cd::history_selected_index;
                f();
            }
            else if (GuiUtil::IsCustomKeyPressed(GuiUtil::ImGuiCustomKey::Down, true) && cd::history_selected_index < cd::history.size() - 1)
            {
                ++cd::history_selected_index;
                f();
            }
        }

        ImGui::EndTabItem();
    }
}

void drawProcessedWindow()
{
    Connector::Debug::ProcessedMidiMessage* message = &Connector::Debug::history_selected;

    ImGui::Begin("processed_detail", &_show_processed_message_window,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar);
    {
        GuiUtil::PushFont((int)FontDebug::ProcHead);
        if (message->transmitted)
            GuiUtil::TextColoredU32(DEBUG_UI_COLOR_TEXT_TRANSMIT, "%s", "Transmitted");
        else
            GuiUtil::TextColoredU32(DEBUG_UI_COLOR_TEXT_RECEIVE, "%s", "Received");
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
        GuiUtil::MouseCursorToHand();

        ImGui::Text(message->timestamp.c_str());
        ImGui::Text("%-12s: %s %s", "Device",
            message->device_name.c_str(), message->transmitted ? "[OUT]" : "[IN]");
        ImGui::Text("%-12s: %s", "Description", message->description.c_str());
        ImGui::Text("%-12s: %d", "Size", message->data.size());

        ImGui::Separator();

        auto hex_space = 30.0f;

        GuiUtil::PushFont((int)FontDebug::ProcHex);
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
        std::unique_lock lock(Logger::dlog_mutex);

        if (Logger::dlog.size() > 0)
        {
            ImGui::Text("%d logs", Logger::dlog.back().log_id + 1);
        }
        else
        {
            ImGui::Text("no logs");
        }

        ImGui::BeginChild("logger_list", ImVec2(800, 430), false, 0);
        {
            int selected_index = 0;
            std::vector<Logger::DisplayFormattedDebugLog> dlog_copy = Logger::dlog;
            for (auto iter = dlog_copy.cbegin(); iter != dlog_copy.cend(); ++iter)
            {
                const bool is_selected = selected_index == Logger::dlog_selected_index;
                if (ImGui::Selectable(StringUtil::format("%05d %s", iter->log_id, iter->text.c_str()).c_str(), is_selected))
                {
                    Logger::dlog_selected_index = selected_index;
                    Logger::dlog_selected = *iter;
                }
                GuiUtil::MouseCursorToHand();
                ++selected_index;
            }

            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
        if (ImGui::IsItemHovered())
        {
            auto f = []()
            {
                auto idx = Logger::dlog_selected_index;
                Logger::dlog_selected = Logger::DisplayFormattedDebugLog(Logger::dlog[idx]);
            };

            if (GuiUtil::IsCustomKeyPressed(GuiUtil::ImGuiCustomKey::Up, true) && Logger::dlog_selected_index > 0)
            {
                --Logger::dlog_selected_index;
                f();
            }
            else if (GuiUtil::IsCustomKeyPressed(GuiUtil::ImGuiCustomKey::Down, true) && Logger::dlog_selected_index < Logger::dlog.size() - 1)
            {
                ++Logger::dlog_selected_index;
                f();
            }
        }

        ImGui::Separator();

        if (Logger::dlog_selected_index != -1)
        {
            ImGui::BeginChild("logger_detail", ImVec2(800, 70), false, 0);
            {
                ImGui::Text("Log ID %d [%s]", Logger::dlog_selected.log_id, Logger::dlog_selected.category.c_str());
                ImGui::Text("%s", Logger::dlog_selected.timestamp.c_str());
                ImGui::Text("%s (LINE %s)", Logger::dlog_selected.function.c_str(), Logger::dlog_selected.line.c_str());
                ImGui::Text("%s", Logger::dlog_selected.text.c_str());
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

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
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
            drawDebugTabItemConfig();
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
    GuiUtil::PushFont((int)FontDebug::Text);

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

} // Debug
} // Gui
} // CentroMidi
#endif
