﻿#include "common.hpp"
#include "error.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace CentroMidi
{

// public
bool has_error = false;
std::string error_message;
bool showing_error_message = false;

void setAppError(const std::string& message) noexcept
{
    if (!has_error)
    {
        error_message = message;
        has_error = true;
    }
}

} // CentroMidi
