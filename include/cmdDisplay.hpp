#pragma once
#include "commander.hpp"

extern "C"{
    extern uint32_t SystemCoreClock;
}
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
        cout<< "CPU Freq: " <<  SystemCoreClock/1000000<<" MHz."<<endl;
    }
};
