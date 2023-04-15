#pragma once

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
        example = "null";
    }
    Commander(string _cmd, string _desc)
    {
        cmd = _cmd;
        desc = _desc;
    }
    ~Commander(){

    }
    virtual void cmdProcess(DynamicJsonDocument doc){
        cout<<"调用了基类的处理函数"<<endl;
    };
    virtual void cmdHelp(){};
    string getCmd()
    {
        return cmd;
    }
    string getDesc()
    {
        return desc;
    }
    void setCmd(string _cmd)
    {
         cmd = _cmd;
    }
    void setDesc(string _desc)
    {
        desc = _desc;
    }
    string getExample()
    {
        return example;
    }
    void setExample(string _example)
    {
         example = _example;
    }
private:
    string cmd;
    string desc;
    string example;
};