﻿#pragma once

namespace CentroMidi
{
namespace Connector
{

class Connection
{
public:
    Connection();
    virtual ~Connection();
    virtual void initialize() = 0;
    virtual void finalize() noexcept = 0;
    void open(const int port_index, const std::string& port_name);
    virtual void close() noexcept;
    bool isPortOpen() const;
    unsigned int getPortCount();
    std::string getPortName(unsigned int port_index);
    int getPortIndex() const noexcept;
    const std::string& getPortName() const noexcept;
    int getLastConnectedPortIndex() const noexcept;
    int getLastFailedPortIndex() const noexcept;

protected:
    void setLastConnectedPortIndex(const int port_index) noexcept;
    void setLastFailedPortIndex(const int port_index) noexcept;
    RtMidi* rtmidi;

private:
    void resetPortInfo() noexcept;
    int _port_index;
    std::string _port_name;
    int _last_connected_port_index;
    int _last_failed_port_index;
};

class InputConnection : public Connection
{
public:
    InputConnection();
    void initialize() override;
    void finalize() noexcept override;
    void close() noexcept override;
    void setCallback(RtMidiIn::RtMidiCallback callback, void* userData = (void*)0);
    void cancelCallback();
    void ignoreTypes(bool midiSysex, bool midiTime, bool midiSense);
};

class OutputConnection : public Connection
{
public:
    OutputConnection();
    void initialize() override;
    void finalize() noexcept override;
    void sendMessage(const ByteVec* message);
};

} // Connector
} // CentroMidi
