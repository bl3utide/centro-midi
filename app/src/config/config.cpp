#include "common.hpp"
#include "config/config.hpp"
#include "config/cv.hpp"
#include "config/reader.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace CentroMidi
{
namespace Config
{

Cv<std::string> in_dev_name { Section::Device,  "input_device",         "",     "",     "" };
Cv<std::string> out_dev_name{ Section::Device,  "output_device",        "",     "",     "" };
Cv<int>         to_ch       { Section::Device,  "to_channel",           1,      16,     1 };
Cv<bool>        is_force_adj{ Section::Device,  "force_adjust_midi_ch", true,   true,   true };

void load(const std::string& ini_file_name) noexcept
{
    mINI::INIStructure read_is;
    mINI::INIFile file = mINI::INIFile(ini_file_name);

    if (file.read(read_is))
    {
#ifdef _DEBUG
        LOGD << "Load config from existing ini file";
#endif
        // ini-file already exists
        Import::structureValueToCv(read_is, in_dev_name);
        Import::structureValueToCv(read_is, out_dev_name);
        Import::structureValueToCv(read_is, to_ch);
        Import::structureValueToCv(read_is, is_force_adj);
    }
#ifdef _DEBUG
    else
    {
        LOGD << "Ini file does not exists";
    }
#endif
}

// TODO move to config/export.hpp/cpp
template<typename T>
void CvToStructure(mINI::INIStructure& is, Cv<T>& cv) noexcept
{}

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
// config/export.hpp/cpp

void save(const std::string& ini_file_name) noexcept
{
    mINI::INIStructure is;
    mINI::INIFile file = mINI::INIFile(ini_file_name);

    CvToStructure(is, in_dev_name);
    CvToStructure(is, out_dev_name);
    CvToStructure(is, to_ch);
    CvToStructure(is, is_force_adj);

    if (!file.write(is, true))
    {
#ifdef _DEBUG
        LOGD << "Failed to write config file";
#endif
    }
}

} // Config
} // CentroMidi
