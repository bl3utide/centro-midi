﻿/*
    Centro MIDI
    Copyright (C) 2024 bl3utide <bl3utide@gmail.com>
    1.2.0
*/
#include "common.hpp"
#include "error.hpp"
#include "image.hpp"
#include "state.hpp"
#include "config/config.hpp"
#include "gui/gui.hpp"
#include "midi/connector.hpp"
#include "logger.hpp"

namespace CentroMidi
{

void initialize()
{
    Logger::initialize();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        throw std::runtime_error("SDL_Init error");
    }
    Logger::debug("<beginning of application>");

    try
    {
        Gui::initialize();
        Image::initialize();
        Connector::initialize();
        Config::initialize();
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        throw;
    }
}

void finalize() noexcept
{
    Config::save();
    Connector::finalize();
    Image::finalize();
    Gui::finalize();

    SDL_Quit();

    Logger::debug("<end of application>");
}

void loop()
{
    SDL_Event event;
    bool running = true;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                setNextState(State::PrepareToExit);
        }

        try
        {
            running = processForCurrentState();
        }
        catch (RtMidiError& error)
        {
#ifdef _DEBUG
            LOGD << error.getMessage();
#endif
            setAppError(StringUtil::format("MIDI error: %s", error.getMessage().c_str()));
        }
        catch (std::exception& error)
        {
#ifdef _DEBUG
            LOGD << error.what();
#endif
            setAppError(StringUtil::format("General error: %s", error.what()));
        }

        if (getNextState() == State::None)
        {
            try
            {
                Gui::drawGui();
            }
            catch (std::exception& error)
            {
#ifdef _DEBUG
                LOGD << error.what();
#endif
                setAppError(StringUtil::format("Gui error: %s", error.what()));
            }
        }
        else
        {
            transitionState();
        }

        SDL_Delay(5);
    }
}

} // CentroMidi

int main(int, char**)
{
    try
    {
        CentroMidi::initialize();
        CentroMidi::loop();
    }
    catch (std::runtime_error&)
    {
        CentroMidi::finalize();
        exit(EXIT_FAILURE);
    }

    CentroMidi::finalize();

    return 0;
}