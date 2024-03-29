﻿#include "common.hpp"
#include "midi/message_handler.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace CentroMidi
{
namespace MessageHandler
{

ByteVec getBankSelectMsbMessage(const int ch, const int value)
{
    Byte order_byte = 0xB0 + static_cast<Byte>(ch);

    ByteVec bs;
    bs.clear();
    bs.push_back(order_byte);
    bs.push_back(0x00);
    bs.push_back(static_cast<Byte>(value / 128));
    return bs;
}

ByteVec getBankSelectLsbMessage(const int ch, const int value)
{
    Byte order_byte = 0xB0 + static_cast<Byte>(ch);

    ByteVec bs;
    bs.clear();
    bs.push_back(order_byte);
    bs.push_back(0x20);
    bs.push_back(static_cast<Byte>(value % 128));
    return bs;
}

ByteVec getProgChangeMessage(const int ch, const int value)
{
    Byte order_byte = 0xC0 + static_cast<Byte>(ch);

    ByteVec pc;
    pc.clear();
    pc.push_back(order_byte);
    pc.push_back(static_cast<Byte>(value));
    return pc;
}

ByteVec getAllSoundOffMessage(const int ch)
{
    Byte order_byte = 0xB0 + static_cast<Byte>(ch);

    ByteVec aso;
    aso.clear();
    aso.push_back(order_byte);
    aso.push_back(static_cast<Byte>(0x78));
    return aso;
}

bool isNoteOff(const ByteVec& mb) noexcept
{
    return 0x80 <= mb[0] && mb[0] <= 0x8F;
}

bool isNoteOn(const ByteVec& mb) noexcept
{
    return 0x90 <= mb[0] && mb[0] <= 0x9F;
}

#ifdef _DEBUG
std::string getMessageDesc(const ByteVec& data)
{
    std::stringstream ss;

    if (data.empty())
    {
        ss << "Empty Message";
    }
    else if (0x80 <= data[0] && data[0] <= 0x9F)
    {
        if (data[0] < 0x90)
            ss << "Note Off";
        else
            ss << "Note On";

        ss << " <" << static_cast<int>(data[1]) << "> Vel(" << static_cast<int>(data[2]) << ")";
    }
    else if (0xB0 <= data[0] && data[0] <= 0xBF)
    {
        if (data[1] == 0x00)
            ss << "Bank Select MSB: " << static_cast<int>(data[2]);
        else if (data[1] == 0x20)
            ss << "Bank Select LSB: " << static_cast<int>(data[2]);
        else if (data[1] == 0x78)
            ss << "All Sound Off";
        else if (data[1] == 0x79)
            ss << "Reset All Controllers";
        else if (data[1] == 0x7A)
        {
            ss << "Local Control";
            if (data[2] == 0)
                ss << " Off";
            else if (data[2] == 127)
                ss << " On";
            else
                ss << " (unknown 3rd byte)";
        }
        else if (data[1] == 0x7B)
            ss << "All Notes Off";
        else
            ss << "Control Change (" << static_cast<int>(data[1]) << "): "
                << static_cast<int>(data[2]);
    }
    else if (0xC0 <= data[0] && data[0] <= 0xCF)
        ss << "Program Change (" << static_cast<int>(data[1]) << ")";
    else
    {
        ss << "?";
    }

    return ss.str();
}
#endif

} // MessageHandler
} // CentroMidi