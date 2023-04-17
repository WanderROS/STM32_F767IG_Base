#pragma once
#include "commander.hpp"
#include "deviceOrderProcess.hpp"
#include <map>
extern DeviceOrder deviceOrder;
class CmdDevice : public Commander
{
public:
    CmdDevice(Commander &commander)
    {
        init();
    }
    CmdDevice()
    {
        setCmd("device");
        setDesc("display/set device info.");
        setExample("{\"cmd\":\"device\",\"op\":\"help\"}");
        init();
    }
    ~CmdDevice()
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
        cout << "DeviceOps: " << endl;
        for (auto iter = funcDescMap.begin(); iter != funcDescMap.end(); ++iter)
        {
            cout << "  " << iter->first << ", " << iter->second << endl;
        }
        cout << "/***********************************************/" << endl;
    }
    void saveEnv2File(DynamicJsonDocument doc)
    {
        deviceOrder.saveSystemConfig2File();
    }
    void setDeviceValues(DynamicJsonDocument doc)
    {
        if (!doc["boolCheatA0"].isNull())
        {
            if (doc["boolCheatA0"] == true)
            {
                deviceOrder.setBoolCheatA0(true);
            }
            else
            {
                deviceOrder.setBoolCheatA0(false);
            }
        }
        if (!doc["projectNo"].isNull())
        {
            if (doc["projectNo"] > 0)
            {
                deviceOrder.setProjectNo(doc["projectNo"]);
            }
        }
        if (!doc["boolCheatSN"].isNull())
        {
            if (doc["boolCheatSN"] == true)
            {
                deviceOrder.setBoolCheatSN(true);
            }
        }
        if (!doc["sn"].isNull())
        {
            const char *_sn = doc["sn"];
            string sn = _sn;
            deviceOrder.setSN(sn);
        }
        cout << ">> Device 设置完成." << endl;
        deviceOrder.showDeviceConfig();
    }
    typedef void((CmdDevice::*Fun_ptr))(DynamicJsonDocument doc);
    /**
     * 注入操作
     */
    void init()
    {
        funcMap.insert(make_pair("save", &CmdDevice::saveEnv2File));
        funcMap.insert(make_pair("set", &CmdDevice::setDeviceValues));
        funcMap.insert(make_pair("help", &CmdDevice::displayCmds));
        funcDescMap.insert(make_pair("save", "保存设备变量,example: {\"cmd\":\"device\",\"op\":\"save\"}"));
        funcDescMap.insert(make_pair("set", "设置设备变量,example: {\"cmd\":\"device\",\"op\":\"set\",\"boolCheatA0\":true,\"projectNo\":13104,\"sn\":\"0000DB99138104887734179988880003\",\"boolCheatSN\":true}"));
        funcDescMap.insert(make_pair("help", "帮助,example: {\"cmd\":\"device\",\"op\":\"help\"}"));
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