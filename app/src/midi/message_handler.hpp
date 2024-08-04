#pragma once

namespace CentroMidi
{
namespace MessageHandler
{

const ByteVec getBankSelectMsbMessage(int ch, int value);
const ByteVec getBankSelectLsbMessage(int ch, int value);
const ByteVec getProgChangeMessage(int ch, int value);
const ByteVec getAllSoundOffMessage(int ch);
bool isNoteOff(const ByteVec& mb) noexcept;
bool isNoteOn(const ByteVec& mb) noexcept;
#ifdef _DEBUG
const std::string getMessageDesc(const ByteVec& data);
#endif

} // MessageHandler
} // CentroMidi
