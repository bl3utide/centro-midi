#pragma once
#include "config/cv.hpp"

namespace CentroMidi
{
namespace Config
{
namespace Writer
{

template<typename T>
void setToStructure(Cv<T>& cv, mINI::INIStructure& is) noexcept
{
}

template<>
void setToStructure<std::string>(Cv<std::string>& cv, mINI::INIStructure& is) noexcept;

template<>
void setToStructure<int>(Cv<int>& cv, mINI::INIStructure& is) noexcept;

template<>
void setToStructure<bool>(Cv<bool>& cv, mINI::INIStructure& is) noexcept;

template<typename T>
void cvToStructure(Cv<T>& cv, mINI::INIStructure& is) noexcept
{
    setToStructure<T>(cv, is);
#ifdef _DEBUG
    LOGD << "writed config value [" << cv.section_name << "]" << cv.key_name << ": " << cv.cv();
#endif
}

} // Writer
} // Config
} // CentroMidi
