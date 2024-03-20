#include "common.hpp"
#include "config/section.hpp"

namespace CentroMidi
{
namespace Config
{

const char* SECTIONS[static_cast<int>(Section::_COUNT_)]
{
    "device"
};

const char* KEYS[static_cast<int>(Key::_COUNT_)]
{
    // [device]
    "input_device",
    "output_device",
    "to_channel",
    "force_adjust_midi_ch",
};

} // Config
} // CentroMidi
