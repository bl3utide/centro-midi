#pragma once
#include "config/cv.hpp"

namespace CentroMidi
{
namespace Config
{
namespace Reader
{

template<typename T>
void setToCv(Cv<T>& cv, const std::string& val_str) noexcept
{
}

template <>
void setToCv<std::string>(Cv<std::string>& cv, const std::string& val_str) noexcept;

template <>
void setToCv<int>(Cv<int>& cv, const std::string& val_str) noexcept;

template <>
void setToCv<bool>(Cv<bool>& cv, const std::string& val_str) noexcept;

template<typename T>
void structureValueToCv(mINI::INIStructure& is, Cv<T>& cv) noexcept
{
    if (is.get(cv.section_name).has(cv.key_name))
    {
        const std::string& src_val = is.get(cv.section_name).get(cv.key_name);

        setToCv<T>(cv, src_val);
#ifdef _DEBUG
        LOGD << "config value [" << cv.section_name << "]" << cv.key_name << ": " << cv.cv();
#endif
    }
    else
    {
        cv.setDefault();
#ifdef _DEBUG
        LOGD << "config value [" << cv.section_name << "]" << cv.key_name << " is set to default value: " << cv.cv();
#endif
    }
}

} // Reader
} // Config
} // CentroMidi
