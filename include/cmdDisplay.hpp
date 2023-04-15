#pragma once
#include "commander.hpp"

class CmdDisplay : public Commander
{
public:
    CmdDisplay()  {
        setCmd("display");
        setDesc("display sys info.");
        setExample("{\"cmd\":\"display\"}");
    }
    ~CmdDisplay(){
        
    }
    void cmdProcess(DynamicJsonDocument doc)
    {
    }
};
