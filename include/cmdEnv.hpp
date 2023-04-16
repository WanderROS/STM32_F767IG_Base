#pragma once
#include "commander.hpp"

class CmdEnv : public Commander
{
public:
    CmdEnv(Commander &commander) {}
    CmdEnv()
    {
        setCmd("env");
        setDesc("display env info.");
        setExample("{\"cmd\":\"env\"}");
    }
    ~CmdEnv()
    {
    }
    void cmdProcess(DynamicJsonDocument doc)
    {
        cout << "cmd Process" << endl;
    }
};