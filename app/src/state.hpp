﻿#pragma once

namespace CentroMidi
{

enum class State : int
{
    InitInternalData,
    ApplyConfig,
    Idle,
    SendBankProgChange,
    PrepareToExit,
    None,
    _COUNT_
};

#ifdef _DEBUG
extern const char* STATE_STR[static_cast<int>(State::_COUNT_)];
#endif

State getState() noexcept;
State getNextState() noexcept;
void setNextState(State state) noexcept;
void transitionState() noexcept;

} // CentroMidi
