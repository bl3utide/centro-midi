#pragma once
#include "midi/message_handler.hpp"

namespace CentroMidi
{
namespace Connector
{

void receiveInputDeviceMessageCallback(double delta_time, MessageHandler::Bytes* message, void* user_data);

} // Connector
} // CentroMidi
