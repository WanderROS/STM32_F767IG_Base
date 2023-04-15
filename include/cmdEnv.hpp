#pragma once
#include "commander.hpp"

class CmdEnv : public Commander
{
public:
    CmdEnv()  {
        setCmd("env");
        setDesc("display env info.");
        setExample("{\"cmd\":\"env\"}");
    }
    ~CmdEnv(){
        
    }
    void cmdProcess(DynamicJsonDocument doc)
    {
    }
};