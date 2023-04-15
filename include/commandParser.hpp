#pragma once

#include "main.h"
#include "ArduinoJson.h"
#include "commander.hpp"
#include <iostream>
#include <string>
#include <map>
#include "cmdDisplay.hpp"
#include "cmdEnv.hpp"
#include "cmdFile.hpp"
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
        CmdDisplay *cmdDisplay = new CmdDisplay();
        CmdEnv *cmdEnv = new CmdEnv();
        CmdFile *cmdFile = new CmdFile();
        registerCmd(cmdDisplay);
        registerCmd(cmdEnv);
        registerCmd(cmdFile);
    }
    void registerCmd(Commander *cmd)
    {
        cmdMaps[cmd->getCmd()] = cmd;
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
            Commander *cmd = iter->second;
            cout << "  " << iter->first << ", " << cmd->getDesc() << "  order: " << cmd->getExample() << endl;
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
                string cmd = doc["cmd"];
                // 命令未处理
                bool cmdProcessFlag = false;
                if (cmd.compare("null") != 0)
                {
                    for (auto iter = cmdMaps.begin(); iter != cmdMaps.end(); ++iter)
                    {
                        if (iter->first.compare(cmd) == 0)
                        {
                            // 命令处理了
                            cmdProcessFlag = true;
                            iter->second->cmdProcess(doc);
                        }
                    }
                    if (cmdProcessFlag == false)
                    {
                        displayCmds();
                        cout << endl
                             << "-----<请确认是否有该命令.>--------"
                             << endl
                             << endl
                             << endl;
                        cout << "--- 原始输入--- => " << doc << endl;
                    }
                }
                else
                {
                    displayCmds();
                    cout << endl
                         << "-----<请确认是否符合命令解析 json 格式.>--------"
                         << endl
                         << endl
                         << endl;
                }
            }
        }
    }
    ~CommandParser()
    {
        for (auto iter = cmdMaps.begin(); iter != cmdMaps.end(); ++iter)
        {
            Commander *cmd = iter->second;
            delete cmd;
        }
    }

private:
    map<string, Commander *> cmdMaps;
};