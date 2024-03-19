#include "common.hpp"
#include "config/config.hpp"
#include "config/cv.hpp"
#include "config/import.hpp"
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
        setToStructure<std::string>(read_is, in_dev_name);
        setToStructure<std::string>(read_is, out_dev_name);
        setToStructure<int>(read_is, to_ch);
        setToStructure<bool>(read_is, is_force_adj);
    }
#ifdef _DEBUG
    else
    {
        LOGD << "Ini file does not exists";
    }
#endif
}

void save(const std::string& ini_file_name) noexcept
{
    mINI::INIStructure is;
    mINI::INIFile file = mINI::INIFile(ini_file_name);

    // TODO impl write function
    is[in_dev_name.section_name][in_dev_name.key_name] = in_dev_name.cv();
    is[out_dev_name.section_name][out_dev_name.key_name] = out_dev_name.cv();
    is[to_ch.section_name][to_ch.key_name] = format("%d", to_ch.cv());
    is[is_force_adj.section_name][is_force_adj.key_name] = is_force_adj.cv() ? "1" : "0";

    if (!file.write(is, true))
    {
#ifdef _DEBUG
        LOGD << "Failed to write config file";
#endif
    }
}

} // Config
} // CentroMidi
