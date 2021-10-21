#ifndef _EPD_QUEUE_H_
#define _EPD_QUEUE_H_
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define QUEUE_LEN 5 /* 队列的长度，最大可包含多少个消息 */

#define QUEUE_TYPE_CONNECT_SUCCESS  0x00 //
#define QUEUE_TYPE_MQTT             0x01 //表示队列传输类型为MQTT
#define QUEUE_TYPE_WEATHER_NOW      0x02  
#define QUEUE_TYPE_WEATHER_3D       0x03

typedef struct EPD_Message
{
    uint8_t data_type;  
    uint8_t data_id;
    uint16_t data_len;
    char *data;
    uint16_t topic_len;
    char *topic;
} epdMessage;



extern QueueHandle_t epdQueue;
#endif