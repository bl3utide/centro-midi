#include "common.hpp"
#include "image.hpp"
#include "gui/gui.hpp"
#include "gui/gui_color.hpp"
#include "gui/gui_font.hpp"
#include "midi/connector.hpp"

namespace CentroMidi
{
namespace Gui
{

const float CONNECTION_LABEL_WIDTH = 80.0f;
const float CONNECTION_CONTROL_WIDTH = 240.0f;

void drawSameLine()
{
    ImGui::SameLine(0.0f, 2.0f);
}

void drawOperationGroupConnections()
{
    GuiUtil::PushFont((int)Font::TextBold);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

    GuiUtil::BeginGroupPanel("Connections", ImVec2(300.0f, 200.0f));
    {
        auto label_width = CONNECTION_LABEL_WIDTH;
        auto control_width = CONNECTION_CONTROL_WIDTH;
        ImGuiIO& io = ImGui::GetIO();

        GuiUtil::PushFont((int)Font::Text);

        // Input Device Combobox
        {
            const int last_connected_port_index = Connector::input.getLastConnectedPortIndex();
            const int last_failed_port_index = Connector::input.getLastFailedPortIndex();
            const auto dev_color =
                last_connected_port_index != -1 ? UI_COLOR_TEXT_DEV_CONNECTED
                : last_failed_port_index != -1 ? UI_COLOR_TEXT_DEV_FAILED
                : UI_COLOR_TEXT_BASE;

            ImGui::PushStyleColor(ImGuiCol_Text, dev_color);
            if (GuiUtil::ImGuiLeftLabel(ImGui::BeginCombo, 0, "Input from", false,
                label_width, control_width, Connector::input.getPortName().c_str(),
                (int)ImGuiComboFlags_NoArrowButton))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 8.0f));
                for (int n = 0; n < Connector::in_name_list.size(); ++n)
                {
                    const bool is_selected = n == Connector::input.getPortIndex();
                    const auto dev_item_color =
                        last_connected_port_index == n ? UI_COLOR_TEXT_DEV_CONNECTED
                        : last_failed_port_index == n ? UI_COLOR_TEXT_DEV_FAILED
                        : UI_COLOR_TEXT_BASE;
                    ImGui::PushStyleColor(ImGuiCol_Text, dev_item_color);
                    if (ImGui::Selectable(Connector::in_name_list[n].c_str(),
                        is_selected))
                    {
                        Connector::openInputPort(n, Connector::in_name_list[n]);
                    }
                    GuiUtil::MouseCursorToHand();
                    ImGui::PopStyleColor();
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::PopStyleVar();
                ImGui::EndCombo();
            }
            GuiUtil::MouseCursorToHand();
            ImGui::PopStyleColor();
        }

        drawSameLine();
        if (ImGui::ImageButton((void*)(intptr_t)Image::getTextureId(Image::Texture::Reload), ImVec2(16.0f, 16.0f)))
        {
            Connector::resetAllConnections();
        }
        GuiUtil::MouseCursorToHand();

        // Output Dev
        {
            const int last_connected_port_index = Connector::output.getLastConnectedPortIndex();
            const int last_failed_port_index = Connector::output.getLastConnectedPortIndex();
            const auto dev_color =
                last_connected_port_index != -1 ? UI_COLOR_TEXT_DEV_CONNECTED
                : last_failed_port_index != -1 ? UI_COLOR_TEXT_DEV_FAILED
                : UI_COLOR_TEXT_BASE;

            ImGui::PushStyleColor(ImGuiCol_Text, dev_color);
            if (GuiUtil::ImGuiLeftLabel(ImGui::BeginCombo, 0, "Output to", false,
                label_width, control_width, Connector::output.getPortName().c_str(),
                (int)ImGuiComboFlags_NoArrowButton))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 8.0f));
                for (int n = 0; n < Connector::out_name_list.size(); ++n)
                {
                    const bool is_selected = n == Connector::output.getPortIndex();
                    const auto dev_item_color =
                        last_connected_port_index == n ? UI_COLOR_TEXT_DEV_CONNECTED
                        : last_failed_port_index == n ? UI_COLOR_TEXT_DEV_FAILED
                        : UI_COLOR_TEXT_BASE;
                    ImGui::PushStyleColor(ImGuiCol_Text, dev_item_color);
                    if (ImGui::Selectable(Connector::out_name_list[n].c_str(),
                        is_selected))
                    {
                        Connector::openOutputPort(n, Connector::out_name_list[n]);
                    }
                    GuiUtil::MouseCursorToHand();
                    ImGui::PopStyleColor();
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::PopStyleVar();
                ImGui::EndCombo();
            }
            GuiUtil::MouseCursorToHand();
            ImGui::PopStyleColor();
        }

        if (GuiUtil::ImGuiLeftLabel(ImGui::InputInt, 0, "to Channel", false,
            label_width, 22.0f, &Connector::display_midi_channel, NULL, NULL,
            (int)ImGuiInputTextFlags_AutoSelectAll))
        {
            if (Connector::display_midi_channel < Connector::getMinTransmitMidiChannel() + 1)
                Connector::display_midi_channel = Connector::getMinTransmitMidiChannel() + 1;
            else if (Connector::display_midi_channel > Connector::getMaxTransmitMidiChannel() + 1)
                Connector::display_midi_channel = Connector::getMaxTransmitMidiChannel() + 1;
            Connector::updateTransmitMidiChannel();
        }
        GuiUtil::MouseCursorToHand();
        if (ImGui::IsItemHovered())
        {
            if ((io.MouseWheel > 0 || GuiUtil::IsCustomKeyPressed(GuiUtil::ImGuiCustomKey::Up, true)) &&        // ++
                Connector::display_midi_channel < Connector::getMaxTransmitMidiChannel() + 1)
                ++Connector::display_midi_channel;
            else if ((io.MouseWheel < 0 || GuiUtil::IsCustomKeyPressed(GuiUtil::ImGuiCustomKey::Down, true)) &&   // --
                Connector::display_midi_channel > Connector::getMinTransmitMidiChannel() + 1)
                --Connector::display_midi_channel;
            Connector::updateTransmitMidiChannel();
        }

        GuiUtil::ImGuiLeftLabel(ImGui::Checkbox, 0, "Force Adjust MIDI Ch", false,
            120.0f, 80.0f, &Connector::force_adjust_midi_channel);
        GuiUtil::MouseCursorToHand();

        ImGui::PopFont();
    }
    GuiUtil::EndGroupPanel();

    ImGui::PopStyleVar();
    ImGui::PopFont();
}

void drawBankSelect()
{
    ImGuiIO& io = ImGui::GetIO();

    if (GuiUtil::ImGuiLeftLabel(ImGui::InputInt, 0, "Bank Select", false,
        80.0f, 42.0f, &Connector::display_bank, NULL, NULL,
        (int)ImGuiInputTextFlags_AutoSelectAll))
    {
        if (Connector::display_bank < Connector::getMinTransmitBank() + 1)
            Connector::display_bank = Connector::getMinTransmitBank() + 1;
        else if (Connector::display_bank > Connector::getMaxTransmitBank() + 1)
            Connector::display_bank = Connector::getMaxTransmitBank() + 1;
        Connector::updateTransmitBank();
        setNextState(State::SendBankProgChange);
    }
    GuiUtil::MouseCursorToHand();
    if (ImGui::IsItemHovered())
    {
        if ((io.MouseWheel > 0 || GuiUtil::IsCustomKeyPressed(GuiUtil::ImGuiCustomKey::Up, true)) &&        // ++
            Connector::display_bank < Connector::getMaxTransmitBank() + 1)
        {
            ++Connector::display_bank;
            Connector::updateTransmitBank();
            setNextState(State::SendBankProgChange);
        }
        else if ((io.MouseWheel < 0 || GuiUtil::IsCustomKeyPressed(GuiUtil::ImGuiCustomKey::Down, true)) && // --
            Connector::display_bank > Connector::getMinTransmitBank() + 1)
        {
            --Connector::display_bank;
            Connector::updateTransmitBank();
            setNextState(State::SendBankProgChange);
        }
    }
}

void drawProgramChangePanel()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Text("Program Change");
    ImGui::Indent(8.0f);

    const int PC_PER_ROW = 16;
    char buf[4];
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    for (int pc_row_i = 0; pc_row_i < Connector::getMaxTransmitProgramChange() / PC_PER_ROW + 1; ++pc_row_i)
    {
        for (int pc_i = 1; pc_i <= PC_PER_ROW; ++pc_i)
        {
            int target_pc = pc_row_i * PC_PER_ROW + pc_i;
            sprintf(buf, "%d", target_pc);
            ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
            if (ImGui::Selectable(buf, target_pc == Connector::display_program_change, 0, ImVec2(20.0f, 16.0f)))
            {
                Connector::display_program_change = target_pc;
                Connector::updateTransmitProgramChange();
                setNextState(State::SendBankProgChange);
            }
            ImGui::PopStyleVar();

            ImVec2 p0 = ImGui::GetItemRectMin();
            ImVec2 p1 = ImGui::GetItemRectMax();

            GuiUtil::MouseCursorToHand();
            if (ImGui::IsItemHovered())
            {
                if ((io.MouseWheel > 0 || GuiUtil::IsCustomKeyPressed(GuiUtil::ImGuiCustomKey::Right, true)) &&         // ++
                    Connector::display_program_change < Connector::getMaxTransmitProgramChange() + 1)
                {
                    ++Connector::display_program_change;
                    Connector::updateTransmitProgramChange();
                    setNextState(State::SendBankProgChange);
                }
                else if ((io.MouseWheel < 0 || GuiUtil::IsCustomKeyPressed(GuiUtil::ImGuiCustomKey::Left, true)) &&     // --
                    Connector::display_program_change > Connector::getMinTransmitProgramChange() + 1)
                {
                    --Connector::display_program_change;
                    Connector::updateTransmitProgramChange();
                    setNextState(State::SendBankProgChange);
                }
                else if (GuiUtil::IsCustomKeyPressed(GuiUtil::ImGuiCustomKey::Down, true) &&        // +16
                    Connector::display_program_change < Connector::getMaxTransmitProgramChange() - 14)
                {
                    Connector::display_program_change += 16;
                    Connector::updateTransmitProgramChange();
                    setNextState(State::SendBankProgChange);
                }
                else if (GuiUtil::IsCustomKeyPressed(GuiUtil::ImGuiCustomKey::Up, true) &&          // -16
                    Connector::display_program_change > Connector::getMinTransmitProgramChange() + 16)
                {
                    Connector::display_program_change -= 16;
                    Connector::updateTransmitProgramChange();
                    setNextState(State::SendBankProgChange);
                }
            }

            if (target_pc == Connector::display_program_change)
                draw_list->AddRect(p0, p1, IM_COL32(200, 200, 200, 255));

            if (pc_i != PC_PER_ROW)
                ImGui::SameLine();
        }
    }

    ImGui::Unindent(8.0f);
}

void drawOperationGroupControls()
{
    GuiUtil::PushFont((int)Font::TextBold);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));

    GuiUtil::BeginGroupPanel("Controls", ImVec2(532.0f, 300.0f));
    {
        GuiUtil::PushFont((int)Font::Text);

        drawBankSelect();
        drawProgramChangePanel();

        ImGui::PopFont();
    }
    GuiUtil::EndGroupPanel();

    ImGui::PopStyleVar();
    ImGui::PopFont();
}

void drawOperationPanel()
{
    drawOperationGroupConnections();
    const bool is_output_device_connected = Connector::output.isPortOpen();
    if (!is_output_device_connected) ImGui::BeginDisabled();
    drawOperationGroupControls();
    if (!is_output_device_connected) ImGui::EndDisabled();
}

} // Gui
} // CentroMidi
