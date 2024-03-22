#pragma once
#include "config/cv.hpp"

namespace CentroMidi
{
namespace Config
{
namespace Writer
{

template<typename T>
void cvToStructure(Cv<T>& cv, mINI::INIStructure& is) noexcept;

} // Writer
} // Config
} // CentroMidi
