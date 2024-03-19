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

extern const char* SECTIONS[static_cast<int>(Section::_COUNT_)];

} // Config
} // CentroMidi
