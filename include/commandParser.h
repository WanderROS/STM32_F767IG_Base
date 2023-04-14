#ifndef __COMMAND_PARSER_H__
#define __COMMAND_PARSER_H__

#include "main.h"
#include "ArduinoJson.h"
#include <iostream>
using namespace std;
extern "C"{
    extern uint8_t ucDebugRecvBuffer[];
    extern uint16_t ulDebugRecvSize;
    extern uint8_t ucDebugRecvReady;
}
void commandProcess();

#endif