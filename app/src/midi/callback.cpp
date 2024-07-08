#include "common.hpp"
#include "error.hpp"
#include "logger.hpp"
#include "midi/callback.hpp"
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
    if (MessageHandler::isNoteOff(*message) || MessageHandler::isNoteOn(*message))
    {
        if (Connector::force_adjust_midi_channel)
        {
            const int ch = getTransmitMidiChannel();
            ByteVec channel_adj_message;
            if (MessageHandler::isNoteOff(*message))
            {
                channel_adj_message = {
                    static_cast<Byte>(0x80 + ch),
                    message->at(1),
                    message->at(2)
                };
            }
            else
            {
                channel_adj_message = {
                    static_cast<Byte>(0x90 + ch),
                    message->at(1),
                    message->at(2)
                };
            }

            try
            {
                output.sendMessage(&channel_adj_message);
            }
            catch (RtMidiError& error)
            {
                Logger::debug(StringUtil::format("MIDI error: %s", error.getMessage().c_str()));
                setAppError("MIDI error when sending message from input to output");
            }
        }
        else
        {
            output.sendMessage(message);
        }
    }
}

} // Callback
} // Connector
} // CentroMidi