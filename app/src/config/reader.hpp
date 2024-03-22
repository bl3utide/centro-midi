#pragma once
#include "config/cv.hpp"

namespace CentroMidi
{
namespace Config
{
namespace Reader
{

template<typename T>
void structureValueToCv(mINI::INIStructure& is, Cv<T>& cv) noexcept;

} // Reader
} // Config
} // CentroMidi
