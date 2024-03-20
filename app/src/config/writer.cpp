#include "common.hpp"
#include "config/writer.hpp"

namespace CentroMidi
{
namespace Config
{
namespace Writer
{

template<>
void CvToStructure<std::string>(mINI::INIStructure& is, Cv<std::string>& cv) noexcept
{
    is[cv.section_name][cv.key_name] = cv.cv();
}

template<>
void CvToStructure<int>(mINI::INIStructure& is, Cv<int>& cv) noexcept
{
    is[cv.section_name][cv.key_name] = format("%d", cv.cv());
}

template<>
void CvToStructure<bool>(mINI::INIStructure& is, Cv<bool>& cv) noexcept
{
    is[cv.section_name][cv.key_name] = cv.cv() ? "1" : "0";
}

} // Writer
} // Config
} // CentroMidi
