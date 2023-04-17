#pragma once
#include "commander.hpp"
#include "systemConfig.hpp"
#include <map>
/* 系统配置类 */
extern SystemConfig systemConfig;

class CmdEnv : public Commander
{
public:
    CmdEnv(Commander &commander)
    {
        init();
    }
    CmdEnv()
    {
        setCmd("env");
        setDesc("display/set env info.");
        setExample("{\"cmd\":\"env\",\"op\":\"help\"}");
        init();
    }
    ~CmdEnv()
    {
    }
    void cmdProcess(DynamicJsonDocument doc)
    {
        string op = doc["op"];
        if (op.compare("null") == 0 || funcMap.count(op) == 0)
        {
            displayCmds(doc);
        }
        else
        {
            RunFunc(op, doc);
        }
    }

private:
    /**
     * 文件操作帮助命令
     */
    void displayCmds(DynamicJsonDocument doc)
    {
        cout << "/***********************************************/" << endl;
        cout << "EnvOps: " << endl;
        for (auto iter = funcDescMap.begin(); iter != funcDescMap.end(); ++iter)
        {
            cout << "  " << iter->first << ", " << iter->second << endl;
        }
        cout << "/***********************************************/" << endl;
    }
    void saveEnv2File(DynamicJsonDocument doc)
    {
        systemConfig.saveSystemConfig2File();
    }
    void setEnvValues(DynamicJsonDocument doc)
    {
        if (!doc["boolDebugEcho"].isNull())
        {
            if (doc["boolDebugEcho"] == true)
            {
                systemConfig.setBoolDebugEcho(true);
            }
            else
            {
                systemConfig.setBoolDebugEcho(false);
            }
        }
        if (!doc["boolWiFiOutEcho"].isNull())
        {
            if (doc["boolWiFiOutEcho"] == true)
            {
                systemConfig.setBoolWiFiOutEcho(true);
            }
            else
            {
                systemConfig.setBoolWiFiOutEcho(false);
            }
        }
        if (!doc["boolDeviceOutEcho"].isNull())
        {
            if (doc["boolDeviceOutEcho"] == true)
            {
                systemConfig.setBoolDeviceOutEcho(true);
            }
            else
            {
                systemConfig.setBoolDeviceOutEcho(false);
            }
        }
        cout << ">> Env 设置完成." << endl;
        systemConfig.showSystemConfig();
    }
    typedef void((CmdEnv::*Fun_ptr))(DynamicJsonDocument doc);
    /**
     * 注入操作
     */
    void init()
    {
        funcMap.insert(make_pair("save", &CmdEnv::saveEnv2File));
        funcMap.insert(make_pair("set", &CmdEnv::setEnvValues));
        funcMap.insert(make_pair("help", &CmdEnv::displayCmds));
        funcDescMap.insert(make_pair("save", "保存系统变量,example: {\"cmd\":\"env\",\"op\":\"save\"}"));
        funcDescMap.insert(make_pair("set", "设置系统变量,example: {\"cmd\":\"env\",\"op\":\"set\",\"boolDebugEcho\":true,\"boolWiFiOutEcho\":false,\"boolDeviceOutEcho\":false}"));
        funcDescMap.insert(make_pair("help", "帮助,example: {\"cmd\":\"env\",\"op\":\"help\"}"));
    }
    // 传入函数名，执行对应的函数
    void RunFunc(string funcName, DynamicJsonDocument doc)
    {
        if (funcMap.count(funcName))
        {
            (this->*funcMap[funcName])(doc);
        }
    }
    map<string, Fun_ptr> funcMap;
    map<string, string> funcDescMap;
};