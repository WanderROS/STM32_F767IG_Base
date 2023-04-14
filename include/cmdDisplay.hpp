#ifndef __CMD_DISPLAY_HPP__
#define __CMD_DISPLAY_HPP__
#include "commander.h"

class CmdDisplay : public Commander
{
public:
    CmdDisplay() : cmd("display"), desc("display sys info.") {}
    ~CmdDisplay(){
        
    }
    void cmdProcess(DynamicJsonDocument doc)
    {
    }
    void cmdHelp()
    {
        cout<<desc<<endl;
    }

private:
    string cmd;
    string desc;
};

#endif