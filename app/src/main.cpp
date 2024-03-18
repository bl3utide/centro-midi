#include "common.hpp"
#include "error.hpp"
#include "main.hpp"
#include "image.hpp"
#include "state.hpp"
#include "config/config.hpp"
#include "gui/gui.hpp"
#include "midi/connector.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace CentroMidi
{

// private
const std::string APP_NAME = DEF_APP_NAME;
const std::string APP_VERSION = DEF_APP_VERSION;
const std::string APP_COPYRIGHT = format("Copyright (C) %d %s", DEF_APP_DEV_YR, DEF_APP_DEV_BY);
const std::string APP_TITLE = DEF_APP_TITLE;
const std::string CONFIG_FILE_NAME = format("%s.ini", APP_NAME.c_str());
#ifdef _DEBUG
const std::string DEBUG_FILE_NAME = format("%s.debug.log", APP_NAME.c_str());
#endif

void initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        throw std::runtime_error("SDL_Init error");
    }

    try
    {
        Gui::initialize(APP_TITLE, APP_VERSION, APP_COPYRIGHT);
        Image::initialize();
        Connector::initialize();
        Config::load(CONFIG_FILE_NAME);
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        throw error;
    }
}

void finalize() noexcept
{
    Config::save();
    Connector::finalize();
    Image::finalize();
    Gui::finalize();

    SDL_Quit();
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
                running = false;
        }

        try
        {
            switch (getState())
            {
                case State::InitInternalData:
                    Connector::resetAllConnections();
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
                default:
                    break;
            }
        }
        catch (RtMidiError& error)
        {
#ifdef _DEBUG
            LOGD << error.getMessage();
#endif
            setAppError(format("MIDI error: %s", error.getMessage().c_str()));
        }
        catch (std::exception& error)
        {
#ifdef _DEBUG
            LOGD << error.what();
#endif
            setAppError(format("General error: %s", error.what()));
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
                setAppError(format("Gui error: %s", error.what()));
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
#ifdef _DEBUG
    static plog::DebugLogAppender<plog::LogFormatter> debugLogAppender;
    plog::init<plog::LogFormatter>(plog::debug, CentroMidi::DEBUG_FILE_NAME.c_str()).addAppender(&debugLogAppender);
    LOGD << "<beginning of application>";
#endif
    try
    {
        CentroMidi::initialize();
    }
    catch (std::exception& e)
    {
#ifdef _DEBUG
        LOGD << e.what();
#endif
        printf("%s", e.what());
        CentroMidi::finalize();
        exit(EXIT_FAILURE);
    }

    CentroMidi::loop();
    CentroMidi::finalize();

#ifdef _DEBUG
    LOGD << "<end of application>";
#endif
    return 0;
}