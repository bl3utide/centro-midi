#include "common.hpp"
#include "config/writer.hpp"

namespace CentroMidi
{
namespace Config
{
namespace Writer
{

template<>
void cvToStructure<std::string>(Cv<std::string>& cv, mINI::INIStructure& is) noexcept
{
    is[cv.section_name][cv.key_name] = cv.cv();
}

template<>
void cvToStructure<int>(Cv<int>& cv, mINI::INIStructure& is) noexcept
{
    is[cv.section_name][cv.key_name] = format("%d", cv.cv());
}

template<>
void cvToStructure<bool>(Cv<bool>& cv, mINI::INIStructure& is) noexcept
{
    is[cv.section_name][cv.key_name] = cv.cv() ? "1" : "0";
}

} // Writer
} // Config
} // CentroMidi
