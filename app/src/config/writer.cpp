#include "common.hpp"
#include "config/writer.hpp"

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
void setToStructure<std::string>(Cv<std::string>& cv, mINI::INIStructure& is) noexcept
{
    is[cv.section_name][cv.key_name] = cv.cv();
}

template<>
void setToStructure<int>(Cv<int>& cv, mINI::INIStructure& is) noexcept
{
    is[cv.section_name][cv.key_name] = format("%d", cv.cv());
}

template<>
void setToStructure<bool>(Cv<bool>& cv, mINI::INIStructure& is) noexcept
{
    is[cv.section_name][cv.key_name] = cv.cv() ? "1" : "0";
}

template<typename T>
void cvToStructure(Cv<T>& cv, mINI::INIStructure& is) noexcept
{
    setToStructure<T>(cv, is);
#ifdef _DEBUG
    LOGD << "Writed config value [" << cv.section_name << "]" << cv.key_name << ": " << cv.cv();
#endif
}
template void cvToStructure(Cv<std::string>&, mINI::INIStructure&) noexcept;
template void cvToStructure(Cv<int>&, mINI::INIStructure&) noexcept;
template void cvToStructure(Cv<bool>&, mINI::INIStructure&) noexcept;

} // Writer
} // Config
} // CentroMidi
