#include "common.hpp"
#include "config/config.hpp"
#include "config/cv.hpp"
#include "config/reader.hpp"
#include "config/writer.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace CentroMidi
{
namespace Config
{

// private
std::unordered_map<Key, Cv> _data;

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
        for (int key_i = 0; key_i < static_cast<int>(Key::_COUNT_); ++key_i)
        {
            Key key = static_cast<Key>(key_i);
            Reader::iniValueToCv(read_is, _data.at(key));
        }
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
    mINI::INIStructure write_is;
    mINI::INIFile file = mINI::INIFile(ini_file_name);

    for (int key_i = 0; key_i < static_cast<int>(Key::_COUNT_); ++key_i)
    {
        Key key = static_cast<Key>(key_i);
        Writer::cvToIni(_data.at(key), write_is);
    }

    if (!file.write(write_is, true))
    {
#ifdef _DEBUG
        LOGD << "Failed to write config file";
#endif
    }
}

template<typename T>
T getConfigValue(const Key key)
{
    throw new std::runtime_error("Unexpected type of Cv");
}
template std::string getConfigValue(const Key);
template int getConfigValue(const Key);
template bool getConfigValue(const Key);

template<>
std::string getConfigValue(const Key key)
{
    Cv& cv = _data.at(key);

    if (cv.type() != Cv::Type::String)
        throw new std::runtime_error(StringUtil::format("Config key '%s' is not string", cv.key_name));

    return cv.cv();
}

template<>
int getConfigValue(const Key key)
{
    Cv& cv = _data.at(key);

    if (cv.type() != Cv::Type::Int)
        throw new std::runtime_error(StringUtil::format("Config key '%s' is not int", cv.key_name));

    return std::stoi(cv.cv());
}

template<>
bool getConfigValue(const Key key)
{
    Cv& cv = _data.at(key);

    if (cv.type() != Cv::Type::String)
        throw new std::runtime_error(StringUtil::format("Config key '%s' is not bool", cv.key_name));

    return cv.cv() == "1";
}

void initialize()
{
    // [Device]
    _data.insert({ Key::InputDevice,        Cv(Section::Device, Key::InputDevice, std::string()) });
    _data.insert({ Key::OutputDevice,       Cv(Section::Device, Key::OutputDevice, std::string()) });
    _data.insert({ Key::ToChannel,          Cv(Section::Device, Key::ToChannel, 1, 16, 1) });
    _data.insert({ Key::ForceAdjustMidiCh,  Cv(Section::Device, Key::ForceAdjustMidiCh, true) });
}

} // Config
} // CentroMidi
