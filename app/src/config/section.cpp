#include "common.hpp"
#include "config/section.hpp"

namespace CentroMidi
{
namespace Config
{

const char* SECTION_NAMES[static_cast<int>(Section::_COUNT_)]
{
    "device"
};

const char* KEY_NAMES[static_cast<int>(Key::_COUNT_)]
{
    // [device]
    "input_device",
    "output_device",
    "to_channel",
    "force_adjust_midi_ch",
};

} // Config
} // CentroMidi
