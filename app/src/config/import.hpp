#pragma once
#include "config/cv.hpp"

namespace CentroMidi
{
namespace Config
{

template<typename T>
void setValue(Cv<T>& cv, const std::string& val_str)
{}

template <>
void setValue<std::string>(Cv<std::string>& cv, const std::string& val_str);

template <>
void setValue<int>(Cv<int>& cv, const std::string& val_str);

template <>
void setValue<bool>(Cv<bool>& cv, const std::string& val_str);

template<typename T>
void setToStructure(Cv<T>& cv)
{
    if (_is.get(cv.section_name).has(cv.key_name))
    {
        const std::string& src_val = _is.get(cv.section_name).get(cv.key_name);

        setValue<T>(cv, src_val);
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

} // Config
} // CentroMidi
