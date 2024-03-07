#include "common.hpp"
#include "midi/callback.hpp"
#include "midi/connector.hpp"
#include "midi/message_handler.hpp"

namespace CentroMidi
{
namespace Connector
{

/*******************************************************************************
    Received message from input device callback
*******************************************************************************/
void receiveInputDeviceMessageCallback(double delta_time, ByteVec* message, void* user_data)
{
    if (MessageHandler::isNoteOff(*message) || MessageHandler::isNoteOn(*message))
    {
        if (Connector::force_adjust_midi_channel)
        {
            //const int ch = transmit_midi_channel;
            const int ch = getTransmitMidiChannel();
            ByteVec channel_adj_message;
            if (MessageHandler::isNoteOff(*message))
            {
                channel_adj_message = {
                    static_cast<unsigned char>(0x80 + ch),
                    message->at(1),
                    message->at(2)
                };
            }
            else
            {
                channel_adj_message = {
                    static_cast<unsigned char>(0x90 + ch),
                    message->at(1),
                    message->at(2)
                };
            }
            conn.output->sendMessage(&channel_adj_message);
        }
        else
        {
            conn.output->sendMessage(message);
        }
    }
}

} // Connector
} // CentroMidi