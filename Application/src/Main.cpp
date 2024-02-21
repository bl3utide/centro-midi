#include "Common.hpp"
#include "Error.hpp"
#include "Main.hpp"
#include "Image.hpp"
#include "State.hpp"
#include "gui/Gui.hpp"
#include "midi/Connector.hpp"
#include "util/StringUtil.hpp"
#ifdef _DEBUG
#include "Logger.hpp"
#endif

namespace CentroMidi
{

// private
const char* APP_NAME = DEF_APP_NAME;
const char* APP_VERSION = DEF_APP_VERSION;
const std::string APP_COPYRIGHT = format("Copyright (C) %d %s", DEF_APP_DEV_YR, DEF_APP_DEV_BY);
const char* APP_TITLE = DEF_APP_TITLE;
#ifdef _DEBUG
const std::string DEBUG_FILE_NAME = format("%s.debug.log", APP_NAME);
#endif

void initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        throw std::runtime_error("SDL_Init error");
    }

    try
    {
        Gui::initialize(APP_TITLE);
        Image::initialize();
        Connector::initialize();
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

std::string getAppVersion() noexcept
{
    return APP_VERSION;
}

std::string getAppCopyright() noexcept
{
    return APP_COPYRIGHT;
}

std::string getAppTitle() noexcept
{
    return APP_TITLE;
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