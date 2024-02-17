/*
    ImGui Project
    v0.2.0
*/
/*
    (Project Name)
    Copyright (C) 20xx bl3utide <bl3utide@gmail.com>
*/
#pragma once

// TODO change app namespace
namespace ImGuiProject
{

#define DEF_APP_NAME        "ImGuiProject"      // TODO fix app name
#define DEF_APP_VERSION     "1.0"
#define DEF_APP_DEV_BY      "bl3utide"
#define DEF_APP_DEV_YR      2024                // TODO fix dev year
#define DEF_APP_TITLE       "ImGui Project"     // TODO fix app title

std::string getAppVersion();
std::string getAppCopyright();
std::string getAppTitle();

} // ImGuiApp
