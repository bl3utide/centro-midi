﻿/*
    Centro MIDI
    Copyright (C) 2024 bl3utide <bl3utide@gmail.com>
    1.0.1
*/
#pragma once

namespace CentroMidi
{

#define DEF_APP_NAME        "CentroMidi"
#define DEF_APP_VERSION     "1.0"
#define DEF_APP_DEV_BY      "bl3utide"
#define DEF_APP_DEV_YR      2024
#define DEF_APP_TITLE       "Centro MIDI"

std::string getAppVersion() noexcept;
std::string getAppCopyright() noexcept;
std::string getAppTitle() noexcept;

} // CentroMidi