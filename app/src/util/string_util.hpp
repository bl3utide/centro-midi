﻿#pragma once

namespace StringUtil
{

// Hint: '%%' escapes '%'
template <typename... Args>
std::string format(const std::string& fmt, Args ... args)
{
    size_t len = std::snprintf(nullptr, 0, fmt.c_str(), args ...);
    std::vector<char> buf(len + 1);
    std::snprintf(&buf[0], len + 1, fmt.c_str(), args ...);
    return std::string(&buf[0], &buf[0] + len);
}

inline bool isInteger(const std::string& str)
{
    return std::regex_match(str, std::regex("[(-|+)|][0-9]*"));
}

} // StringUtil
