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
const std::unordered_map<State, const char*> STATE_STR
{
    { State::InitInternalData,      "InitInternalData" },
    { State::ApplyConfig,           "ApplyConfig"} ,
    { State::Idle,                  "Idle" },
    { State::SendBankProgChange,    "SendBankProgChange" },
    { State::PrepareToExit,         "PrepareToExit" },
    { State::None,                  "None" },
};
#endif

// private
State state_;
State next_state_;  // the next state that change in the next loop

void initState() noexcept
{
    state_ = State::InitInternalData;
    next_state_ = State::None;
}

bool processForCurrentState()
{
    switch (state_)
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
    return state_;
}

State getNextState() noexcept
{
    return next_state_;
}

void setNextState(State state, bool force_mod) noexcept
{
    if (state == State::None) return;

    if (next_state_ == State::None || force_mod)
    {
        next_state_ = state;
#ifdef _DEBUG
        LDEBUG << "setNextState: [" << static_cast<int>(next_state_) << "]"
            << STATE_STR.at(next_state_)
            << " (current: " << STATE_STR.at(state_) << ")";
#endif
    }
    else
    {
#ifdef _DEBUG
        LDEBUG << "*** called multiple times in one loop ***";
        LDEBUG << " -> current_state: " << STATE_STR.at(state_);
        LDEBUG << " -> next_state:    " << STATE_STR.at(next_state_);
        LDEBUG << " -> arg:           " << STATE_STR.at(state);
#endif
    }
}

void transitionState() noexcept
{
    state_ = next_state_;
    next_state_ = State::None;
#ifdef _DEBUG
    LDEBUG << "State changed to [" << static_cast<int>(state_) << "]"
        << STATE_STR.at(state_);
#endif
}

} // CentroMidi
