#include "commandParser.h"

void commandProcess(){
        if (ucDebugRecvReady == TRUE)
        {
            ucDebugRecvReady = FALSE;
            DynamicJsonDocument doc(1024);
            // Deserialize the JSON document
            DeserializationError error = deserializeJson(doc, ucDebugRecvBuffer);
            if (error)
            {
                printf("解析异常,JSON 格式异常\n");
            }
            else
            {
                if(strcmp(doc["command"],"ls")==0){
                    cout<<doc<<endl;
                }else{
                    cout<<doc["command"]<<endl;
                }
            }
        }
}