#pragma once
#include "config/cv.hpp"

namespace CentroMidi
{
namespace Config
{
namespace Writer
{

void cvToIni(Cv& cv, mINI::INIStructure& is) noexcept;

} // Writer
} // Config
} // CentroMidi
