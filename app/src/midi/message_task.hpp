#pragma once

namespace CentroMidi
{
namespace MessageTask
{

Bytes lastTask();
size_t taskSize() noexcept;
#ifdef _DEBUG
size_t largestTaskSizeEver() noexcept;
#endif

} // MessageTask
} // CentroMidi
