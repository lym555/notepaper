#ifndef _MQTT_QUEUE_H_
#define _MQTT_QUEUE_H_
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define QUEUE_LEN 5 /* 队列的长度，最大可包含多少个消息 */

#define QUEUE_TYPE_CONNECT_SUCCESS 0x00 //表示传输MQTT连接成功
#define QUEUE_TYPE_TEXT            0x01 //表示队列传输类型为文本
struct MQTTMessage
{
    uint8_t data_type;  // 0表示联网成功
    uint16_t data_id;
    uint16_t data_len;
    uint16_t data_len_last;
    char *data;
} xMessage;

extern QueueHandle_t xQueue;
#endif