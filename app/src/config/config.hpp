#pragma once
#include "config/section.hpp"

namespace CentroMidi
{
namespace Config
{

void load(const std::string& ini_file_name) noexcept;
void save(const std::string& ini_file_name) noexcept;
void initialize();

} // Config
} // CentroMidi
