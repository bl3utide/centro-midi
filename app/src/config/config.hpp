#pragma once
#include "config/section.hpp"

namespace CentroMidi
{
namespace Config
{

template<typename T>
T getConfigValue(const Key key);
template<>
std::string getConfigValue(const Key key);
template<>
int getConfigValue(const Key key);
template<>
bool getConfigValue(const Key key);

void load(const std::string& ini_file_name) noexcept;
void save(const std::string& ini_file_name) noexcept;
void initialize();

} // Config
} // CentroMidi
