#pragma once

namespace CentroMidi
{
namespace MessageHandler
{

Bytes getBankSelectMsbMessage(const int ch, const int value);
Bytes getBankSelectLsbMessage(const int ch, const int value);
Bytes getProgChangeMessage(const int ch, const int value);
Bytes getAllSoundOffMessage(const int ch);
bool isNoteOff(const Bytes& mb) noexcept;
bool isNoteOn(const Bytes& mb) noexcept;
#ifdef _DEBUG
std::string getMessageDesc(const Bytes& data);
#endif

} // MessageHandler
} // CentroMidi
