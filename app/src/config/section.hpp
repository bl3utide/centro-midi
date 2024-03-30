#pragma once

namespace CentroMidi
{
namespace Config
{

enum class Section : int
{
    Device,
    _COUNT_,
};

enum class Key : int
{
    // [Device]
    InputDevice,
    OutputDevice,
    ToChannel,
    ForceAdjustMidiCh,

    _COUNT_,
};

extern const char* SECTION_NAMES[static_cast<int>(Section::_COUNT_)];
extern const char* KEY_NAMES[static_cast<int>(Key::_COUNT_)];

} // Config
} // CentroMidi
