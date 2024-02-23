#include "Common.hpp"
#include "Error.hpp"
#ifdef _DEBUG
#include "Logger.hpp"
#endif

namespace CentroMidi
{

// public
bool has_error = false;
std::string error_message;
bool showing_error_message = false;

void setAppError(const std::string& message) noexcept
{
    error_message = message;
    has_error = true;
}

} // CentroMidi
