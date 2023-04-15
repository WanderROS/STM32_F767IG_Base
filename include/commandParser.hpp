#pragma once

#include "main.h"
#include "ArduinoJson.h"
#include "commander.hpp"
#include <iostream>
#include <string>
#include <map>
#include "cmdDisplay.hpp"
#include "cmdEnv.hpp"
using namespace std;
extern "C"
{
    extern uint8_t ucDebugRecvBuffer[];
    extern uint16_t ulDebugRecvSize;
    extern uint8_t ucDebugRecvReady;
}

class CommandParser
{
public:
    CommandParser()
    {
        CmdDisplay cmdDisplay;
        CmdEnv cmdEnv;
        registerCmd(cmdDisplay);
        registerCmd(cmdEnv);
    }
    void registerCmd(Commander cmd)
    {
        cmdMaps[cmd.getCmd()] = cmd;
    }
    void displayCmds()
    {
        cout << "/***********************************************/" << endl;
        cout << "*                                              *" << endl;
        cout << "*             JSON 命令行工具 1.0              *" << endl;
        cout << "*             Author: WanderROS                *" << endl;
        cout << "*                                              *" << endl;
        cout << "/***********************************************/" << endl;
        cout << "Commands: " << endl;
        for (auto iter = cmdMaps.begin(); iter != cmdMaps.end(); ++iter)
        {
            cout << "  " << iter->first << ", " << iter->second.getDesc() << "  order: "<< iter->second.getExample() << endl;
        }
        cout << "/***********************************************/" << endl;
    }
    void commandProcess()
    {
        if (ucDebugRecvReady == TRUE)
        {
            ucDebugRecvReady = FALSE;
            DynamicJsonDocument doc(1024);
            // Deserialize the JSON document
            DeserializationError error = deserializeJson(doc, ucDebugRecvBuffer);
            if (error)
            {
                displayCmds();
            }
            else
            {
                if (strcmp(doc["command"], "ls") == 0)
                {
                    cout << doc << endl;
                }
                else
                {
                    cout << doc["command"] << endl;
                }
            }
        }
    }

private:
    map<string, Commander> cmdMaps;
};