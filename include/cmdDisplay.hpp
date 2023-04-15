#pragma once
#include "commander.hpp"

class CmdDisplay : public Commander
{
public:
    CmdDisplay(Commander &commander) {}
    CmdDisplay()
    {
        setCmd("display");
        setDesc("display sys info.");
        setExample("{\"cmd\":\"display\"}");
    }
    ~CmdDisplay()
    {
    }
    void cmdProcess(DynamicJsonDocument doc)
    {
        cout << "cmd Display." << endl;
    }
};
