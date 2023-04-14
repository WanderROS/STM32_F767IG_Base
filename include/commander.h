#ifndef __COMMANDER_H__
#define __COMMANDER_H__
#include <iostream>
using namespace std;
#include "ArduinoJson.h"
class Commander
{
public:
    Commander()
    {
        cmd = "null";
        desc = "null";
    }
    Commander(string _cmd, string _desc)
    {
        cmd = _cmd;
        desc = _desc;
    }
    ~Commander(){

    }
    virtual void cmdProcess(DynamicJsonDocument doc){};
    virtual void cmdHelp(){};
    string getCmd()
    {
        return cmd;
    }
    string getDesc()
    {
        return desc;
    }

private:
    string cmd;
    string desc;
};

#endif