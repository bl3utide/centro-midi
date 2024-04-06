#pragma once

namespace CentroMidi
{
namespace Connector
{
namespace Callback
{

void receiveInputDeviceMessage(double delta_time, ByteVec* message, void* user_data);

} // Callback
} // Connector
} // CentroMidi
