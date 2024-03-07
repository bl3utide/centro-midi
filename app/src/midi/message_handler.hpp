#pragma once

namespace CentroMidi
{
namespace MessageHandler
{

ByteVec getBankSelectMsbMessage(const int ch, const int value);
ByteVec getBankSelectLsbMessage(const int ch, const int value);
ByteVec getProgChangeMessage(const int ch, const int value);
ByteVec getAllSoundOffMessage(const int ch);
bool isNoteOff(const ByteVec& mb) noexcept;
bool isNoteOn(const ByteVec& mb) noexcept;
#ifdef _DEBUG
std::string getMessageDesc(const ByteVec& data);
#endif

} // MessageHandler
} // CentroMidi
