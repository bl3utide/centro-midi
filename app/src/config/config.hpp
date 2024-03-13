#pragma once

namespace CentroMidi
{
namespace Config
{

void initialize(const std::string& ini_file_name);
void finalize() noexcept;

} // Config
} // CentroMidi
