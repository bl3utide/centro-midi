#pragma once

#include "MessageHandler.hpp"

namespace CentroMidi
{
namespace MessageTask
{

MessageHandler::Bytes lastTask();
size_t taskSize() noexcept;
#ifdef _DEBUG
size_t largestTaskSizeEver() noexcept;
#endif

} // MessageTask
} // CentroMidi
