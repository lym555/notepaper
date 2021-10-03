#ifndef _MQTT_TASK_H_
#define _MQTT_TASK_H_
// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/queue.h"

// #define QUEUE_LEN 5 /* 队列的长度，最大可包含多少个消息 */


// struct MQTTMessage
// {
//  uint16_t ucMessageID;
//  char *ucData;
// } xMessage;

// extern QueueHandle_t xQueue;
extern const char *SUB_TOPIC_DATA;
extern const char *SUB_TOPIC_REFRESH;
void mqtt_start(void);

#endif