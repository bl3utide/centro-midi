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

extern const std::unordered_map<Section, const char*> SECTION_NAMES;
extern const std::unordered_map<Key, const char*> KEY_NAMES;

} // Config
} // CentroMidi
