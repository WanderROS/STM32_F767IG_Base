#pragma once
#include "commander.hpp"
#include <map>
using namespace std;
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
    char QSPIPath[4];            /* QSPI flash逻辑驱动器路径 */
    FATFS fs;                    /* FatFs文件系统对象 */
    FIL fnew;                    /* 文件对象 */
    FRESULT res_flash;           /* 文件操作结果 */
    UINT fnum;                   /* 文件成功读写数量 */
    extern FATFS flash_fs;
    extern Diskio_drvTypeDef QSPI_Driver;
}

class CmdFile : public Commander
{
public:
    CmdFile(Commander &commander)
    {
        init();
    }
    CmdFile()
    {
        setCmd("file");
        setDesc("file operation");
        setExample("{\"cmd\":\"file\",\"op\":\"mkfs\"}");
        init();
    }
    ~CmdFile()
    {
    }
    void cmdProcess(DynamicJsonDocument doc)
    {
        string op = doc["op"];
        RunFunc(op, doc);
    }

private:
    void listFiles(DynamicJsonDocument doc)
    {
        DIR dir;
        static FILINFO fno;
        res_flash = f_mount(&fs, QSPIPath, 1);
        if (res_flash == FR_OK)
        {
            // 查看根目录下有哪些文件并通过串口输出
            res_flash = f_opendir(&dir, "");
            if (res_flash == FR_OK)
            {
                printf("打开根目录成功！\n");
            }
            for (int i = 0;; i++)
            {
                res_flash = f_readdir(&dir, &fno); // 读取目录项，索引会自动移到下一项
                if (res_flash != FR_OK || fno.fname[0] == 0)
                {
                    break; // 说明没有文件
                }
                if (fno.fname[0] == '.')
                {
                    continue;
                }
                if (fno.fattrib & AM_DIR) // 判断是文件还是子目录
                {
                    printf("(0x%02d)目录", fno.fattrib);
                }
                else
                {
                    printf("(0x%02d)文件", fno.fattrib);
                }
                printf("名称为：%s\r\n", (char *)fno.fname);
            }
            /*
             *
             * 不再使用文件系统，取消挂载文件系统
             */
            f_mount(NULL, "0:", 1);
        }
        else
        {
            cout << ">> 当前 Flash 文件系统挂载失败,写入文件失败" << endl;
            return;
        }
    }
    /**
     * 读文件
     */
    void readFile(DynamicJsonDocument doc)
    {
        res_flash = f_mount(&fs, QSPIPath, 1);
        if (res_flash == FR_OK)
        {
            string filename = doc["filename"];
            if (filename.compare("null") == 0)
            {
                cout << ">> 请检查文件名是否正确" << endl;
                return;
            }
            else
            {
                res_flash = f_open(&fnew, filename.c_str(), FA_OPEN_EXISTING | FA_READ);
                
                if (res_flash == FR_OK)
                {
                    BYTE *ReadBuffer = (BYTE *)malloc(1024);
                    printf("》打开文件成功。\r\n");
                    res_flash = f_read(&fnew, ReadBuffer, 1024, &fnum);
                    if (res_flash == FR_OK)
                    {
                        printf("》文件读取成功,读到字节数据：%d\r\n", fnum);
                        printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);
                    }
                    else
                    {
                        printf("！！文件读取失败：(%d)\n", res_flash);
                    }
                     free(ReadBuffer);
                }
                else
                {
                    printf("！！打开文件失败。\r\n");
                }
                /* 不再读写，关闭文件 */
                f_close(&fnew);
                /* 不再使用文件系统，取消挂载文件系统 */
                f_mount(NULL, QSPIPath, 1);
            }
        }
        else
        {
            cout << ">> 当前 Flash 文件系统挂载失败,读取文件失败" << endl;
            return;
        }
    }
    /**
     * 写文件
     */
    void writeFile(DynamicJsonDocument doc)
    {
        res_flash = f_mount(&fs, QSPIPath, 1);
        if (res_flash == FR_OK)
        {
            string filename = doc["filename"];
            string content = doc["content"];
            if (filename.compare("null") == 0)
            {
                cout << ">> 请检查文件名是否正确" << endl;
                return;
            }
            else
            {
                res_flash = f_open(&fnew, filename.c_str(), FA_CREATE_ALWAYS | FA_WRITE);
                if (res_flash == FR_OK)
                {
                    printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
                    /* 将指定存储区内容写入到文件内 */
                    res_flash = f_write(&fnew, content.c_str(), content.size(), &fnum);
                    if (res_flash == FR_OK)
                    {
                        printf("》文件写入成功，写入字节数据：%d\n", fnum);
                    }
                    else
                    {
                        printf("！！文件写入失败：(%d)\n", res_flash);
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
            cout << ">> 当前 Flash 文件系统挂载失败,写入文件失败" << endl;
            return;
        }
    }
    /**
     *
     *  格式化文件系统
     *
     */
    void mkfs(DynamicJsonDocument doc)
    {
        cout << ">> 即将格式化 Flash ..., 三秒内可复位取消格式化操作!" << endl;
        HAL_Delay(3000);
        res_flash = f_mount(&fs, QSPIPath, 1);
        if (res_flash == FR_OK)
        {
            cout << ">> 当前 Flash 存在文件系统，格式化完文件将全部丢失!!!" << endl;
        }
        else
        {
            cout << ">> 当前 Flash 不存在文件系统,正在格式化" << endl;
        }
        /* 格式化 */
        res_flash = f_mkfs(QSPIPath, 0, 0);
        if (res_flash == FR_OK)
        {
            cout << ">> 格式化 Flash 成功, 即将进行验证挂载 ... ..." << endl;
            /* 格式化后，先取消挂载 */
            res_flash = f_mount(NULL, QSPIPath, 1);
            /* 重新挂载	*/
            res_flash = f_mount(&fs, QSPIPath, 1);
            if (res_flash == FR_OK)
            {
                cout << ">> Flash 文件系统挂载成功, 即将进行卸载 ... ..." << endl;
                res_flash = f_mount(NULL, QSPIPath, 1);
                if (res_flash == FR_OK)
                {
                    cout << ">> Flash 文件系统卸载成功, 格式化 Flash 成功."
                         << endl
                         << endl;
                    return;
                }
            }
        }
        else
        {
            cout << res_flash << endl;
            cout << ">> Flash 格式化文件系统失败,请检查 Flash 硬件是否正常!!!"
                 << endl
                 << endl;
        }
    }
    typedef void((CmdFile::*Fun_ptr))(DynamicJsonDocument doc);
    /**
     * 注入操作
     */
    void init()
    {
        funcMap.insert(make_pair("mkfs", &CmdFile::mkfs));
        funcMap.insert(make_pair("write", &CmdFile::writeFile));
        funcMap.insert(make_pair("read", &CmdFile::readFile));
        funcMap.insert(make_pair("list", &CmdFile::listFiles));
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
};