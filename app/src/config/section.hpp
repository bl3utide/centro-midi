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

extern const char* SECTIONS[static_cast<int>(Section::_COUNT_)];
extern const char* KEYS[static_cast<int>(Key::_COUNT_)];

} // Config
} // CentroMidi
