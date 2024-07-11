﻿#include "common.hpp"
#include "config/cv.hpp"

namespace CentroMidi
{
namespace Config
{

const std::unordered_map<Cv::Type, std::string> Cv::TYPE_STR
{
    { Type::String, "string" },
    { Type::Int, "int" },
    { Type::Bool, "bool" },
};

} // Config
} // CentroMidi
