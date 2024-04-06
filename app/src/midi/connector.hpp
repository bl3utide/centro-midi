#pragma once
#include "midi/connection.hpp"

namespace CentroMidi
{
namespace Connector
{

extern InputConnection input;
extern OutputConnection output;
extern std::vector<std::string> in_name_list;
extern std::vector<std::string> out_name_list;
extern bool force_adjust_midi_channel;
extern int display_midi_channel;
extern int display_bank;
extern int display_program_change;

void initialize();
void finalize() noexcept;
void applyConfig();
void updateConfig() noexcept;
void resetAllConnections();
void openInputPort(const int port_index, const std::string& port_name);
void openOutputPort(const int port_index, const std::string& port_name);
void sendBankSelectMsb();
void sendBankSelectLsb();
void sendProgChange();
void sendAllSoundOff();
void sendOneTaskMessage();
void updateTransmitMidiChannel() noexcept;
void updateTransmitBank() noexcept;
void updateTransmitProgramChange() noexcept;
int getTransmitMidiChannel() noexcept;
int getMinTransmitMidiChannel() noexcept;
int getMaxTransmitMidiChannel() noexcept;
int getMinTransmitBank() noexcept;
int getMaxTransmitBank() noexcept;
int getMinTransmitProgramChange() noexcept;
int getMaxTransmitProgramChange() noexcept;
bool isBothDevicesConnected() noexcept;
void setBothDevicesConnected(const bool connected);

} // Connector
} // CentroMidi
