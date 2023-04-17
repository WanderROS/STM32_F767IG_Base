#pragma once
#include "commander.hpp"
#include "systemConfig.hpp"
#include "deviceOrderProcess.hpp"

extern "C"
{
    extern uint32_t SystemCoreClock;
}
/* 系统配置类 */
extern SystemConfig systemConfig;
extern DeviceOrder deviceOrder;
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
        cout << "CPU Freq: " << SystemCoreClock / 1000000 << " MHz." << endl;
        systemConfig.showSystemConfig();
        deviceOrder.showDeviceConfig();
    }
};
