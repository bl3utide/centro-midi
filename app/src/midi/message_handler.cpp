#include "common.hpp"

namespace CentroMidi
{
namespace MessageHandler
{

// private
// ------------ common ------------
const Byte SYSEX_FIRST          = 0xF0;
const Byte SYSEX_LAST           = 0xF7;
const Byte SYSEX_SECOND_UNRT    = 0x7E;
const Byte MIDI_NOTE_OFF        = 0x80;
const Byte MIDI_NOTE_ON         = 0x90;
const Byte MIDI_CC              = 0xB0;
const Byte MIDI_PC              = 0xC0;
const Byte MAX_CH_OFFSET        = 0x0F;

const ByteVec getBankSelectMsbMessage(int ch, int value)
{
    const Byte order_byte = MIDI_CC + static_cast<Byte>(ch);

    ByteVec bs;
    bs.clear();
    bs.push_back(order_byte);
    bs.push_back(0x00);
    bs.push_back(static_cast<Byte>(value / 128));
    return bs;
}

const ByteVec getBankSelectLsbMessage(int ch, int value)
{
    const Byte order_byte = MIDI_CC + static_cast<Byte>(ch);

    ByteVec bs;
    bs.clear();
    bs.push_back(order_byte);
    bs.push_back(0x20);
    bs.push_back(static_cast<Byte>(value % 128));
    return bs;
}

const ByteVec getProgChangeMessage(int ch, int value)
{
    const Byte order_byte = MIDI_PC + static_cast<Byte>(ch);

    ByteVec pc;
    pc.clear();
    pc.push_back(order_byte);
    pc.push_back(static_cast<Byte>(value));
    return pc;
}

const ByteVec getAllSoundOffMessage(int ch)
{
    const Byte order_byte = MIDI_CC + static_cast<Byte>(ch);

    ByteVec aso;
    aso.clear();
    aso.push_back(order_byte);
    aso.push_back(static_cast<Byte>(0x78));
    return aso;
}

bool isNoteOff(const ByteVec& mb) noexcept
{
    return MIDI_NOTE_OFF <= mb[0] && mb[0] <= MIDI_NOTE_OFF + MAX_CH_OFFSET;
}

bool isNoteOn(const ByteVec& mb) noexcept
{
    return MIDI_NOTE_ON <= mb[0] && mb[0] <= MIDI_NOTE_ON + MAX_CH_OFFSET;
}

#ifdef _DEBUG
const std::string getMessageDesc(const ByteVec& data)
{
    std::stringstream ss;

    if (data.empty())
    {
        ss << "Empty Message";
    }
    else if (MIDI_NOTE_OFF <= data[0] && data[0] <= MIDI_NOTE_ON + MAX_CH_OFFSET)
    {
        if (data[0] < MIDI_NOTE_ON) ss << "Note Off";
        else ss << "Note On";

        ss << " <" << static_cast<int>(data[1]) << "> Vel(" << static_cast<int>(data[2]) << ")";
    }
    else if (MIDI_CC <= data[0] && data[0] <= 0xBF)
    {
        if (data[1] == 0x00)      ss << "Bank Select MSB: " << static_cast<int>(data[2]);
        else if (data[1] == 0x01) ss << "Modulation: " << static_cast<int>(data[2]);
        else if (data[1] == 0x20) ss << "Bank Select LSB: " << static_cast<int>(data[2]);
        else if (data[1] == 0x40) ss << "Damper Pedal: " << static_cast<int>(data[2]);
        else if (data[1] == 0x78) ss << "All Sound Off";
        else if (data[1] == 0x79) ss << "Reset All Controllers";
        else if (data[1] == 0x7A)
        {
            ss << "Local Control";
            if (data[2] == 0)        ss << " Off";
            else if (data[2] == 127) ss << " On";
            else                     ss << " (unknown 3rd byte)";
        }
        else if (data[1] == 0x7B)
        {
            ss << "All Notes Off";
        }
        else
        {
            ss << "Control Change (" << static_cast<int>(data[1]) << "): "
                << static_cast<int>(data[2]);
        }
    }
    else if (MIDI_PC <= data[0] && data[0] <= MIDI_PC + MAX_CH_OFFSET)
    {
        ss << "Program Change (" << static_cast<int>(data[1]) << ")";
    }
    else
    {
        ss << "?";
    }

    return ss.str();
}
#endif

} // MessageHandler
} // CentroMidi