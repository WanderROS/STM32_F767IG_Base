#pragma once
#include "commander.hpp"

class CmdFile : public Commander
{
public:
    CmdFile(Commander &commander) {}
    CmdFile()
    {
        setCmd("file");
        setDesc("file operation");
        setExample("{\"cmd\":\"file\"}");
    }
    ~CmdFile()
    {
    }
    void cmdProcess(DynamicJsonDocument doc)
    {
        cout << "file Process" << endl;
    }
};