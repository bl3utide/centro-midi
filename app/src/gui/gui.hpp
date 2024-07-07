#pragma once

#include "state.hpp"

namespace CentroMidi
{
namespace Gui
{

void initialize();
void finalize() noexcept;
void drawGui();
void showMessageBox(Uint32 flags, const char* title, const char* message) noexcept;

// sub modules
void drawOperationPanel();
#ifdef _DEBUG
namespace Debug
{
void drawDebugMenuBar(const ImVec2 viewport_pos);
void drawDebugWindows(const int window_w, const int window_h, const State current_state);
} // Debug
#endif

} // Gui
} // CentroMidi
