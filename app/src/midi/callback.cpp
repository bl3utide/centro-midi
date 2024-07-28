#include "common.hpp"
#include "error.hpp"
#include "logger.hpp"
#include "midi/connector.hpp"
#include "midi/message_handler.hpp"

namespace CentroMidi
{
namespace Connector
{
namespace Callback
{

/*******************************************************************************
    Received message from input device callback
*******************************************************************************/
void receiveInputDeviceMessage(double delta_time, ByteVec* message, void* user_data)
{
    if (isBothDevicesConnected() &&
        MessageHandler::isNoteOff(*message) || MessageHandler::isNoteOn(*message))
    {
        ByteVec send_message;
        if (Connector::force_adjust_midi_channel)
        {
            const auto ch = getTransmitMidiChannel();
            if (MessageHandler::isNoteOff(*message))
            {
                send_message =
                {
                    static_cast<Byte>(0x80 + ch),
                    message->at(1),
                    message->at(2)
                };
            }
            else
            {
                send_message =
                {
                    static_cast<Byte>(0x90 + ch),
                    message->at(1),
                    message->at(2)
                };
            }
        }
        else
        {
            send_message = *message;
        }

        try
        {
            output.sendMessage(send_message);
        }
        catch (RtMidiError& error)
        {
            Logger::debug(StringUtil::format("MIDI error: %s", error.getMessage().c_str()));
            setAppError("MIDI error when sending message from input to output");
        }
    }
}

} // Callback
} // Connector
} // CentroMidi