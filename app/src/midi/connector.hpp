#pragma once

namespace CentroMidi
{
namespace Connector
{

class Connection
{
public:
    Connection()
    {
        resetPortInfo();
    }

    virtual ~Connection()
    {
    }

    virtual void initialize() = 0;
    virtual void finalize() noexcept = 0;

    void open(const int port_index, const std::string& port_name)
    {
        close();

        try
        {
            _port_index = port_index;
            _port_name = port_name;
            _rtmidi->openPort(port_index);
            setLastConnectedPortIndex(port_index);
        }
        catch (RtMidiError&)
        {
            setLastFailedPortIndex(port_index);
            throw;
        }
    }

    virtual void close() noexcept
    {
        _rtmidi->closePort();
    }

    bool isPortOpen() const
    {
        return _rtmidi->isPortOpen();
    }

    unsigned int getPortCount()
    {
        return _rtmidi->getPortCount();
    }

    std::string getPortName(unsigned int port_index)
    {
        return _rtmidi->getPortName(port_index);
    }

    int getPortIndex() const noexcept
    {
        return _port_index;
    }

    const std::string& getPortName() const noexcept
    {
        return _port_name;
    }

    void resetPortInfo() noexcept
    {
        _port_index = -1;
        _port_name = "";
        _last_connected_port_index = -1;
        _last_failed_port_index = -1;
    }

    int getLastConnectedPortIndex() const noexcept { return _last_connected_port_index; }

    int getLastFailedPortIndex() const noexcept { return _last_failed_port_index; }

protected:
    void setLastConnectedPortIndex(const int port_index) noexcept
    {
        _last_connected_port_index = port_index;
        _last_failed_port_index = -1;
    }

    void setLastFailedPortIndex(const int port_index) noexcept
    {
        _last_failed_port_index = port_index;
        _last_connected_port_index = -1;
    }

    RtMidi* _rtmidi;
    int _port_index;
    std::string _port_name;
    int _last_connected_port_index;
    int _last_failed_port_index;
};

class InputConnection : public Connection
{
public:
    InputConnection() : Connection()
    {
    }

    void initialize() override
    {
        _rtmidi = new RtMidiIn();
    }

    void finalize() noexcept override
    {
        if (_rtmidi != nullptr)
        {
            close();
            delete dynamic_cast<RtMidiIn*>(_rtmidi);
            _rtmidi = nullptr;
        }
    }

    void close() noexcept override
    {
        dynamic_cast<RtMidiIn*>(_rtmidi)->cancelCallback();
        Connection::close();
    }

    void setCallback(RtMidiIn::RtMidiCallback callback, void* userData = (void*)0)
    {
        dynamic_cast<RtMidiIn*>(_rtmidi)->setCallback(callback, userData);
    }

    void ignoreTypes(bool midiSysex, bool midiTime, bool midiSense)
    {
        dynamic_cast<RtMidiIn*>(_rtmidi)->ignoreTypes(midiSysex, midiTime, midiSense);
    }
};

class OutputConnection : public Connection
{
public:
    OutputConnection() : Connection()
    {
    }

    void initialize() override
    {
        _rtmidi = new RtMidiOut();
    }

    void finalize() noexcept override
    {
        if (_rtmidi != nullptr)
        {
            close();
            delete dynamic_cast<RtMidiOut*>(_rtmidi);
            _rtmidi = nullptr;
        }
    }

    void sendMessage(const ByteVec* message)
    {
        dynamic_cast<RtMidiOut*>(_rtmidi)->sendMessage(message);
    }
};

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
