﻿#include "common.hpp"
#include "logger.hpp"
#include "config/writer.hpp"

namespace CentroMidi
{
namespace Config
{
namespace Writer
{

void cvToIni(const Cv& cv, mINI::INIStructure& is) noexcept
{
    is[cv.section_name()][cv.key_name()] = cv.cv();
    Logger::debug(std::format("Writed config value [{0}]{1}: {2}",
        cv.section_name(),
        cv.key_name(),
        cv.cv()));
}

} // Writer
} // Config
} // CentroMidi
