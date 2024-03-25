#pragma once
#include "config/section.hpp"

namespace CentroMidi
{
namespace Config
{

template<typename T> T getConfigValue(const Key key);
template<> std::string getConfigValue(const Key key);
template<> int getConfigValue(const Key key);
template<> bool getConfigValue(const Key key);

template<typename T> void setConfigValue(const Key key, T value);
template<> void setConfigValue(const Key key, std::string value);
template<> void setConfigValue(const Key key, int value);
template<> void setConfigValue(const Key key, bool value);

const std::string& getConfigSectionName(const Key key);
const std::string& getConfigKeyName(const Key key);
const char* getConfigTypeName(const Key key);
const std::string& getConfigValueStr(const Key key);

void load(const std::string& ini_file_name) noexcept;
void save(const std::string& ini_file_name) noexcept;
void initialize();

} // Config
} // CentroMidi
