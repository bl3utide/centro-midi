#pragma once

#include "State.hpp"

namespace CentroMidi
{
namespace Gui
{

enum class Font : int
{
    Title,
    Version,
    OptionItem,
    OptionItemBold,
    Section,
    Text,
    TextBold,
    Debug,
    DebugProcHead,
    DebugProcHex,
    _COUNT_,
};

void initialize(const char* app_title);
void finalize() noexcept;
void drawGui();

// sub modules
void drawOperationPanel();
#ifdef _DEBUG
void drawDebugMenuBar(const ImVec2 viewport_pos);
void drawDebugWindows(const int window_w, const int window_h, const State current_state);
bool isChildBgAlt() noexcept;
#endif

} // Gui
} // CentroMidi
