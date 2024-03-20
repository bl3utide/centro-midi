#pragma once
#include "config/cv.hpp"

namespace CentroMidi
{
namespace Config
{
namespace Writer
{

template<typename T>
void CvToStructure(mINI::INIStructure& is, Cv<T>& cv) noexcept
{}

template<>
void CvToStructure<std::string>(mINI::INIStructure& is, Cv<std::string>& cv) noexcept;

template<>
void CvToStructure<int>(mINI::INIStructure& is, Cv<int>& cv) noexcept;

template<>
void CvToStructure<bool>(mINI::INIStructure& is, Cv<bool>& cv) noexcept;

} // Writer
} // Config
} // CentroMidi
