#pragma once
#include "config/cv.hpp"

namespace CentroMidi
{
namespace Config
{
namespace Import
{

template<typename T>
void setToCv(Cv<T>& cv, const std::string& val_str)
{
}

template <>
void setToCv<std::string>(Cv<std::string>& cv, const std::string& val_str);

template <>
void setToCv<int>(Cv<int>& cv, const std::string& val_str);

template <>
void setToCv<bool>(Cv<bool>& cv, const std::string& val_str);

template<typename T>
void readValueFromStructure(mINI::INIStructure& is, Cv<T>& cv)
{
    if (is.get(cv.section_name).has(cv.key_name))
    {
        const std::string& src_val = is.get(cv.section_name).get(cv.key_name);

        setToCv<T>(cv, src_val);
#ifdef _DEBUG
        LOGD << "[" << cv.section_name << "]" << cv.key_name << " is set to value: " << cv.cv();
#endif
    }
    else
    {
        cv.setDefault();
#ifdef _DEBUG
        LOGD << "[" << cv.section_name << "]" << cv.key_name << " is set to default value: " << cv.cv();
#endif
    }
}

} // Import
} // Config
} // CentroMidi
