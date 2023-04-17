#pragma once
#include <iostream>
using namespace std;
#include "ArduinoJson.h"
// C 库依赖
extern "C"
{
#include "../lib/FATFS/ff.h"
#include "../lib/FATFS/drivers/fatfs_flash_qspi.h"
#include "../lib/FATFS/ff_gen_drv.h"

    /**
     ******************************************************************************
     *                              定义变量
     ******************************************************************************
     */
    extern char QSPIPath[4];  /* QSPI flash逻辑驱动器路径 */
    extern FATFS fs;          /* FatFs文件系统对象 */
    extern FIL fnew;          /* 文件对象 */
    extern FRESULT res_flash; /* 文件操作结果 */
    extern UINT fnum;         /* 文件成功读写数量 */
    extern FATFS flash_fs;
}

/**
 * 系统配置类
 */
class SystemConfig
{
public:
    SystemConfig()
    {
    }
    void showSystemConfig()
    {
        cout << "/***********************************************/" << endl;
        cout << "                   系统环境变量                   " << endl;
        cout << "/***********************************************/" << endl;
        cout << ">> 调试串口回显  boolDebugEcho:     " << boolDebugEcho << endl;
        cout << ">> WiFi 输入显示 boolWiFiInEcho:    " << boolWiFiInEcho << endl;
        cout << ">> WiFi 输出显示 boolWiFiOutEcho:   " << boolWiFiOutEcho << endl;
        cout << ">> 设备 输入显示 boolDeviceInEcho:  " << boolDeviceInEcho << endl;
        cout << ">> 设备 输出显示 boolDeviceOutEcho: " << boolDeviceOutEcho << endl;
        cout << endl;
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
    /* 序列化 */
    DynamicJsonDocument serialOut()
    {
        DynamicJsonDocument doc(1024);
        doc["boolDebugEcho"] = boolDebugEcho;
        doc["boolWiFiInEcho"] = boolWiFiInEcho;
        doc["boolWiFiOutEcho"] = boolWiFiOutEcho;
        doc["boolDeviceInEcho"] = boolDeviceInEcho;
        doc["boolDeviceOutEcho"] = boolDeviceOutEcho;
        return doc;
    }
    /* 反序列化 */
    void deSerial(DynamicJsonDocument doc)
    {
        if (!doc["boolDebugEcho"].isNull())
        {
            boolDebugEcho = doc["boolDebugEcho"];
        }
        if (!doc["boolWiFiInEcho"].isNull())
        {
            boolWiFiInEcho = doc["boolWiFiInEcho"];
        }
        if (!doc["boolWiFiOutEcho"].isNull())
        {
            boolWiFiOutEcho = doc["boolWiFiOutEcho"];
        }
        if (!doc["boolDeviceInEcho"].isNull())
        {
            boolDeviceInEcho = doc["boolDeviceInEcho"];
        }
        if (!doc["boolDeviceOutEcho"].isNull())
        {
            boolDeviceOutEcho = doc["boolDeviceOutEcho"];
        }
        cout << ">> 反序列化成功." << endl;
    }

    bool getBoolDebugEcho()
    {
        return boolDebugEcho;
    }
    bool getBoolDeviceOutEcho()
    {
        return boolDeviceOutEcho;
    }
    bool getBoolWiFiOutEcho()
    {
        return boolWiFiOutEcho;
    }

    void setBoolDebugEcho(bool val)
    {
        boolDebugEcho = val;
    }
    void setBoolDeviceOutEcho(bool val)
    {
        boolDeviceOutEcho = val;
    }
    void setBoolWiFiOutEcho(bool val)
    {
        boolWiFiOutEcho = val;
    }

private:
    /* 调试串口输入回显开关 */
    bool boolDebugEcho = true;
    /* Wi-Fi 输入显示开关 */
    bool boolWiFiInEcho = false;
    /* Wi-Fi 输出显示开关 */
    bool boolWiFiOutEcho = false;
    /* Device 输入显示开关 */
    bool boolDeviceInEcho = false;
    /* Device 输出显示开关 */
    bool boolDeviceOutEcho = false;
    /* 配置保存文件名 */
    string defaultConfigFile = "config.txt";
};