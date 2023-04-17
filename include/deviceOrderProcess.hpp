#pragma once
#include "systemConfig.hpp"
#include <string>
/**
 * 设备指令处理
 */

/* 系统配置类 */
extern SystemConfig systemConfig;

extern "C"
{
#include "main.h"
#include "bsp/bsp_device_usart.h"
#include "bsp/bsp_wifi_usart.h"
    extern uint8_t ucDeviceRecvBuffer[];
    extern uint16_t ulDeviceRecvSize;
    extern uint8_t ucDeviceRecvReady;
}

class DeviceOrder
{
public:
    void coreProcess()
    {
        if (ucDeviceRecvReady == TRUE)
        {
            injectOrder(ucDeviceRecvBuffer, ulDeviceRecvSize);
            if (systemConfig.getBoolDeviceOutEcho())
            {
                printf(">> 设备输出: ");
            }

            for (int i = 0; i < ulDeviceRecvSize; ++i)
            {
                if (systemConfig.getBoolDeviceOutEcho())
                {
                    printf("%02x ", ucDeviceRecvBuffer[i]);
                }

                HAL_UART_Transmit(&UartWiFiHandle, (uint8_t *)(ucDeviceRecvBuffer + i), 1, 1000);
            }
            if (systemConfig.getBoolDeviceOutEcho())
            {
                printf("\n");
            }
            ucDeviceRecvReady = FALSE;
        }
    }

    void showDeviceConfig()
    {
        cout << "/***********************************************/" << endl;
        cout << "                   设备环境变量                   " << endl;
        cout << "/***********************************************/" << endl;
        cout << ">> 是否欺骗A0  boolCheatA0:     " << boolCheatA0 << endl;
        cout << ">> 设备项目号 projectNo:    " << projectNo << endl;
        cout << endl;
    }

    void setSN(string _sn)
    {
        sn = _sn;
    }
    string getSN()
    {
        return sn;
    }
    void setBoolCheatA0(bool val)
    {
        boolCheatA0 = val;
    }

    bool getBoolCheatA0()
    {
        return boolCheatA0;
    }
    void setBoolCheatSN(bool val)
    {
        boolCheatSN = val;
    }

    bool getBoolCheatSN()
    {
        return boolCheatSN;
    }
    void setProjectNo(uint16_t a0)
    {
        projectNo = a0;
    }
    uint16_t getProjectNo()
    {
        return projectNo;
    }
    void saveSystemConfig2File()
    {
        DynamicJsonDocument doc = serialOut();
        string content;
        serializeJson(doc, content);
        res_flash = f_mount(&fs, QSPIPath, 1);
        if (res_flash == FR_OK)
        {
            string filename = defaultConfigFile;
            if (filename.compare("null") == 0)
            {
                cout << ">> 请检查文件名是否正确, 输入内容:"
                     << endl
                     << doc
                     << endl;
                return;
            }
            else
            {
                res_flash = f_open(&fnew, filename.c_str(), FA_CREATE_ALWAYS | FA_WRITE);
                if (res_flash == FR_OK)
                {
                    cout << ">> 打开/创建文件:  "
                         << filename
                         << " 成功."
                         << "即将写入内容:"
                         << endl
                         << content
                         << endl;
                    /* 将指定存储区内容写入到文件内 */
                    res_flash = f_write(&fnew, content.c_str(), content.size(), &fnum);
                    if (res_flash == FR_OK)
                    {
                        cout << ">> 内容写入成功，写入字节数: "
                             << fnum
                             << " ."
                             << endl;
                    }
                    else
                    {
                        cout << ">> 内容写入失败，返回: "
                             << res_flash
                             << " ."
                             << endl;
                    }
                    /* 不再读写，关闭文件 */
                    f_close(&fnew);
                    /* 不再使用文件系统，取消挂载文件系统 */
                    f_mount(NULL, QSPIPath, 1);
                }
            }
        }
        else
        {
            cout << ">> 当前 Flash 文件系统挂载失败,写入文件失败." << endl;
            return;
        }
    }
    void readFile2Config()
    {
        res_flash = f_mount(&fs, QSPIPath, 1);
        if (res_flash == FR_OK)
        {

            // 打开文件，读取的方式，文件不存在打开会失败
            res_flash = f_open(&fnew, defaultConfigFile.c_str(), FA_OPEN_EXISTING | FA_READ);
            if (res_flash == FR_OK)
            {
                long length = 1024;
                BYTE *ReadBuffer = (BYTE *)malloc(length);
                cout << ">> 打开文件 "
                     << defaultConfigFile
                     << " 成功."
                     << "即将读取文件长度: "
                     << length
                     << " ."
                     << endl;
                res_flash = f_read(&fnew, ReadBuffer, length, &fnum);
                if (res_flash == FR_OK)
                {
                    cout << ">>文件 "
                         << defaultConfigFile
                         << " 读取成功,读取到 "
                         << fnum
                         << " 字节数据."
                         << endl;
                    cout << ">> 文件内容: "
                         << endl;
                    for (uint16_t i = 0; i < fnum; i++)
                    {
                        cout << ReadBuffer[i];
                    }
                    cout << endl;
                    ReadBuffer[fnum] = '\0';
                    DynamicJsonDocument doc(1024);
                    DeserializationError error = deserializeJson(doc, ReadBuffer);
                    if (error)
                    {
                        cout << "读取的配置文件 " << defaultConfigFile << " 内容格式非 Json 格式." << endl;
                        return;
                    }
                    else
                    {
                        deSerial(doc);
                    }
                }
                else
                {
                    cout << ">>文件 "
                         << defaultConfigFile
                         << " 读取失败.返回: "
                         << res_flash
                         << " ."
                         << endl;
                }
                free(ReadBuffer);
                ReadBuffer = NULL;
            }
            else
            {
                cout << ">> 打开文件失败,请检查文件 "
                     << defaultConfigFile
                     << " 是否存在."
                     << endl
                     << endl;
            }
            /* 不再读写，关闭文件 */
            f_close(&fnew);
            /* 不再使用文件系统，取消挂载文件系统 */
            f_mount(NULL, QSPIPath, 1);
        }
        else
        {
            cout << ">> 当前 Flash 文件系统挂载失败,读取文件失败" << endl;
            return;
        }
    }

private:
    void processA0(uint8_t *buffer, int len)
    {
        if (boolCheatA0)
        {
            buffer[12] = projectNo & 0xFF;
            buffer[13] = projectNo / 255;
        }
        buffer[len - 1] = orderCheckSum(buffer, len);
    }
    void process04(uint8_t *buffer, int len)
    {
        if (boolCheatA0)
        {
            buffer[31] = projectNo & 0xFF;
            buffer[32] = projectNo / 255;
        }

        buffer[len - 1] = orderCheckSum(buffer, len);
    }
    void process03(uint8_t *buffer, int len)
    {
        if (boolCheatA0)
        {
            buffer[31] = projectNo & 0xFF;
            buffer[32] = projectNo / 255;
        }
        buffer[len - 1] = orderCheckSum(buffer, len);
    }
    void processSN(uint8_t *buffer, int len)
    {

        if (boolCheatSN)
        {
            len = 43;
            for (int i = 10; i < len - 1; i++)
            {
                buffer[i] = sn.at(i - 10);
            }
        }
        buffer[len - 1] = orderCheckSum(buffer, len);
    }
    // 注入内容
    void injectOrder(uint8_t *buffer, int len)
    {
        // 设备指令小于 10 没必要注入
        if (len >= 10)
        {
            // 指令
            switch ((unsigned char)buffer[9])
            {
            case 0xA0:
                processA0(buffer, len);
                break;
            case 0x04:
                process04(buffer, len);
                break;
            case 0x03:
                process03(buffer, len);
                break;
            case 0x07:
                processSN(buffer, len);
                break;
            default:
                break;
            }
        }
    }
    /**
     * CRC 校验
     */
    unsigned char orderCheckSum(uint8_t *buffer, int len)
    {
        unsigned char *p = buffer;
        unsigned char checksum = 0;
        p++;
        for (int i = 0; i < len - 2; i++)
        {
            checksum += *p;
            p++;
        }
        checksum = ~checksum + 1;
        return checksum;
    }

    /* 序列化 */
    DynamicJsonDocument serialOut()
    {
        DynamicJsonDocument doc(1024);
        doc["projectNo"] = projectNo;
        doc["boolCheatA0"] = boolCheatA0;
        doc["boolCheatSN"] = boolCheatSN;
        return doc;
    }
    /* 反序列化 */
    void deSerial(DynamicJsonDocument doc)
    {
        if (!doc["projectNo"].isNull())
        {
            projectNo = doc["projectNo"];
        }
        if (!doc["boolCheatA0"].isNull())
        {
            boolCheatA0 = doc["boolCheatA0"];
        }
        if (!doc["boolCheatSN"].isNull())
        {
            boolCheatSN = doc["boolCheatSN"];
        }
        if (!doc["sn"].isNull())
        {
            const char * _sn = doc["sn"];
            sn = _sn;
        }
        cout << ">> 反序列化成功." << endl;
    }
    // 项目号
    uint16_t projectNo = 13105;
    // 欺骗项目号,项目号四个品类都是一样的处理逻辑
    bool boolCheatA0 = false;
    // 欺骗 SN
    bool boolCheatSN = true;
    string sn = "0000DB99138104887734179988880003";
    /* 配置保存文件名 */
    string defaultConfigFile = "device.txt";
};