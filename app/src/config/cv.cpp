﻿#include "common.hpp"
#include "cv.hpp"

namespace CentroMidi
{
namespace Config
{

const std::unordered_map<Cv::Type, std::string> Cv::_type_str
{
    {Type::String, "string"},
    {Type::Int, "int"},
    {Type::Bool, "bool"}
};

} // Config
} // CentroMidi
