#pragma once
#include "config/cv.hpp"

namespace CentroMidi
{
namespace Config
{
namespace Writer
{

template<typename T>
void cvToStructure(Cv<T>& cv, mINI::INIStructure& is) noexcept
{
}

template<>
void cvToStructure<std::string>(Cv<std::string>& cv, mINI::INIStructure& is) noexcept;

template<>
void cvToStructure<int>(Cv<int>& cv, mINI::INIStructure& is) noexcept;

template<>
void cvToStructure<bool>(Cv<bool>& cv, mINI::INIStructure& is) noexcept;

} // Writer
} // Config
} // CentroMidi
