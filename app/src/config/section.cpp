#include "common.hpp"
#include "config/section.hpp"

namespace CentroMidi
{
namespace Config
{

const std::unordered_map<Section, const char*> SECTION_NAMES
{
    { Section::Device,  "device" },
};

const std::unordered_map<Key, const char*> KEY_NAMES
{
    // [device]
    { Key::InputDevice,         "input_device" },
    { Key::OutputDevice,        "output_device" },
    { Key::ToChannel,           "to_channel" },
    { Key::ForceAdjustMidiCh,   "force_adjust_midi_ch" },
};

} // Config
} // CentroMidi
