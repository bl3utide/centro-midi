#include "common.hpp"
#include "state.hpp"
#include "config/config.hpp"
#include "midi/connector.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace CentroMidi
{

// public
#ifdef _DEBUG
const char* STATE_STR[static_cast<int>(State::_COUNT_)] =
{
    "InitInternalData",
    "ApplyConfig",
    "Idle",
    "SendBankProgChange",
    "PrepareToExit",
    "None"
};
#endif

// private
State _state = State::InitInternalData;
State _next_state = State::None;    // the next state that change in the next loop

bool processForCurrentState()
{
    switch (_state)
    {
    case State::InitInternalData:
        Connector::resetAllConnections();
        setNextState(State::ApplyConfig);
        break;
    case State::ApplyConfig:
        Config::load();
        Connector::applyConfig();
        setNextState(State::Idle);
        break;
    case State::Idle:
        Connector::sendOneTaskMessage();
        break;
    case State::SendBankProgChange:
        Connector::sendBankSelectMsb();
        Connector::sendBankSelectLsb();
        Connector::sendProgChange();
        setNextState(State::Idle);
        break;
    case State::PrepareToExit:
        Connector::updateConfig();
        return false;
        break;
    default:
        break;
    }

    return true;
}

State getState() noexcept
{
    return _state;
}

State getNextState() noexcept
{
    return _next_state;
}

void setNextState(State state, const bool force_mod) noexcept
{
    if (state == State::None)
        return;

    if (_next_state == State::None || force_mod)
    {
        _next_state = state;
#ifdef _DEBUG
        LDEBUG << "setNextState: [" << static_cast<int>(_next_state) << "]"
            << STATE_STR[static_cast<int>(_next_state)]
            << " (current: " << STATE_STR[static_cast<int>(_state)] << ")";
#endif
    }
    else
    {
#ifdef _DEBUG
        LDEBUG << "*** called multiple times in one loop ***";
        LDEBUG << " -> current_state: " << STATE_STR[static_cast<int>(_state)];
        LDEBUG << " -> next_state:    " << STATE_STR[static_cast<int>(_next_state)];
        LDEBUG << " -> arg:           " << STATE_STR[static_cast<int>(state)];
#endif
    }
}

void transitionState() noexcept
{
    _state = _next_state;
    _next_state = State::None;
#ifdef _DEBUG
    LDEBUG << "State changed to [" << static_cast<int>(_state) << "]"
        << STATE_STR[static_cast<int>(_state)];
#endif
}

} // CentroMidi
