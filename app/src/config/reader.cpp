#include "common.hpp"
#include "config/reader.hpp"

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
void setToCv<std::string>(Cv<std::string>& cv, const std::string& val_str) noexcept
{
    cv.set(val_str);
}

template <>
void setToCv<int>(Cv<int>& cv, const std::string& val_str) noexcept
{
    if (std::regex_match(val_str, std::regex("[(-|+)|][0-9]*")))
        cv.set(std::stoi(val_str));
    else
        cv.setDefault();
}

template <>
void setToCv<bool>(Cv<bool>& cv, const std::string& val_str) noexcept
{
    if (val_str == "1")
        cv.set(true);
    else
        cv.set(false);
}

template<typename T>
void structureValueToCv(mINI::INIStructure& is, Cv<T>& cv) noexcept
{
    if (is.get(cv.section_name).has(cv.key_name))
    {
        const std::string& src_val = is.get(cv.section_name).get(cv.key_name);

        setToCv<T>(cv, src_val);
#ifdef _DEBUG
        LOGD << "Loaded config value [" << cv.section_name << "]" << cv.key_name << ": " << cv.cv();
#endif
    }
    else
    {
        cv.setDefault();
#ifdef _DEBUG
        LOGD << "Loaded DEFAULT config value [" << cv.section_name << "]" << cv.key_name << ": " << cv.cv();
#endif
    }
}
template void structureValueToCv(mINI::INIStructure&, Cv<std::string>&) noexcept;
template void structureValueToCv(mINI::INIStructure&, Cv<int>&) noexcept;
template void structureValueToCv(mINI::INIStructure&, Cv<bool>&) noexcept;

} // Reader
} // Config
} // CentroMidi
