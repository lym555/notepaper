/*****************************************************************************
* | File      	:   EPD_2IN13_test.c
* | Author      :   Waveshare team
* | Function    :   2.9inch e-paper test demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-06-11
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "EPD_2in3_display.h"
#include "EPD_2in13.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "mqtt_queue.h"
#include "mqtt_task.h"
#include "ImageData.h"
#include "cJSON.h"

static portTickType xLastWakeTime;

static const uint8_t TASKBARH_HIGH = 16;
static const char *TAG = "EPD_2IN13_TEST";
char *data = "";

TaskHandle_t xTimeTask = NULL;
TaskHandle_t xContentTask = NULL;
//Create a new image cache
UBYTE *BlackImage;
/* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
UWORD Imagesize = 0;

void time_Task(void *pvParameters)
{
    sFONT Font = Font16;
    uint8_t content_len = 5; // 时间显示占5个字
    uint8_t start_x = (Paint.Width - Font.Width * content_len) / 2;
    uint8_t start_y = 0;
    xLastWakeTime = xTaskGetTickCount(); //获取计数
    EPD_2IN13_Init(EPD_2IN13_PART);      //局部刷新
    Paint_SelectImage(BlackImage);       //选择图像存储空间
    PAINT_TIME sPaint_time;
    sPaint_time.Hour = 12;
    sPaint_time.Min = 34;
    while (1)
    {
        EPD_2IN13_Init(EPD_2IN13_PART); //局部刷新
        sPaint_time.Min = sPaint_time.Min + 1;
        if (sPaint_time.Min == 60)
        {
            sPaint_time.Hour = sPaint_time.Hour + 1;
            sPaint_time.Min = 0;
            if (sPaint_time.Hour == 24)
            {
                sPaint_time.Hour = 0;
                sPaint_time.Min = 0;
            }
        }
        Paint_ClearWindows(start_x, start_y, start_x + Font.Width * content_len, start_y + Font.Height, WHITE);
        Paint_DrawTime(start_x, start_y, &sPaint_time, &Font, WHITE, BLACK);
        EPD_2IN13_Display(BlackImage);
        vTaskDelayUntil(&xLastWakeTime, 60 * 1000 / portTICK_RATE_MS);
    }
}

void content_Task(void *pvParameters)
{
    struct MQTTMessage *pxRxedMessage = NULL; //结构体类型的指针变量
    pxRxedMessage = &xMessage;                //指针指向AMessage结构体
    while (1)
    {
        if (xQueue == NULL)
        {
            ESP_LOGE(TAG, "消息队列未初始化");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        if (xQueueReceive(xQueue, (void *)pxRxedMessage, (TickType_t)10))
        {
            /*订阅成功说明Wi-Fi连接没问题，更新Wi-Fi图标*/
            if (pxRxedMessage->data_type == QUEUE_TYPE_CONNECT_SUCCESS)
            {
                Paint_ClearWindows(250 - 16, 0, 16, 16, WHITE);
                Paint_DrawBitMapFree(gImage_connect, 250 - 16, 0, 16, 16);
                EPD_2IN13_Display(BlackImage);
                printf("wifi connect success\r\n");
                continue;
            }
            if (pxRxedMessage->data_type == QUEUE_TYPE_JSON)
            {
                if (strcmp(pxRxedMessage->topic, SUB_TOPIC_REFRESH) == 0)
                {
                    EPD_display_refresh(pxRxedMessage->data, pxRxedMessage->data_len);
                }
                if (strcmp(pxRxedMessage->topic, SUB_TOPIC_DATA) == 0)
                {
                    EPD_display_text(pxRxedMessage->data, pxRxedMessage->data_len);
                }
            }
        }
    }
}
void EPD_display_refresh(char *json_str, uint16_t str_len)
{
    sFONT Font = Font16;
    cJSON *root = NULL;
    cJSON *item = NULL;
    root = cJSON_ParseWithLength(json_str, str_len);
    item = cJSON_GetObjectItem(root, "data");
    uint8_t type = cJSON_GetObjectItem(root, "type")->valueint;
    uint8_t x = cJSON_GetObjectItem(item, "x")->valueint;
    uint8_t y = cJSON_GetObjectItem(item, "y")->valueint;
    uint8_t w = cJSON_GetObjectItem(item, "w")->valueint;
    uint8_t h = cJSON_GetObjectItem(item, "h")->valueint;
    printf("type = %d; x=%d;y=%d;w=%d;h=%d\r\n", type, x, y, w, h);

    x = x * Font.Width;                       //起点坐标x
    x = (x >= Paint.Width) ? Paint.Width : x; //起点坐标x不能大于屏幕宽度

    y = y * Font.Height + TASKBARH_HIGH;        //起点坐标y，注意状态栏
    y = (y >= Paint.Height) ? Paint.Height : y; //起点坐标y不能大于屏幕高度

    uint8_t x1 = w * Font.Width + x; // 终点坐标x1
    x1 = (x1 >= Paint.Width) ? Paint.Width : x1;

    uint8_t y1 = h * Font.Height + y; // 终点左右y1
    y1 = (y1 >= Paint.Height) ? Paint.Height : y1;

    printf("type = %d; x=%d;y=%d;x1=%d;y1=%d\r\n", type, x, y, x1, y1);

    if (type == EPD_2IN13_PART)
    {
        EPD_2IN13_Init(EPD_2IN13_PART);
        printf("EPD_2IN13_PART\r\n");
    }

    else if (type == EPD_2IN13_FULL)
    {
        EPD_2IN13_Init(EPD_2IN13_FULL);
        printf("EPD_2IN13_FULL\r\n");
    }
    Paint_ClearWindows(x, y, x1, y1, WHITE); //清除画布
    EPD_2IN13_Display(BlackImage);           //显示清除后的画布
    if (root != NULL)
        cJSON_Delete(root); //删除最外层即可
}

void EPD_display_text(char *json_str, uint16_t str_len)
{

    cJSON *root = NULL;
    cJSON *item = NULL;
    root = cJSON_ParseWithLength(json_str, str_len);
    item = cJSON_GetObjectItem(root, "data");
    // uint8_t type = cJSON_GetObjectItem(root, "type")->valueint;

    uint8_t item_len = cJSON_GetArraySize(item);
    if (item_len > 0)
    {
        item = item->child;
        for (uint8_t i = 0; i < item_len; i++)
        {
            uint8_t x = cJSON_GetObjectItem(item, "x")->valueint;
            uint8_t y = cJSON_GetObjectItem(item, "y")->valueint;
            char *text = cJSON_GetObjectItem(item, "text")->valuestring;
            printf("x=%d; y=%d; text=%s\r\n", x, y, text);
            show_text(x, y, text);
            item = item->next;
        }
    }
}

void show_text(uint8_t x, uint8_t y, char *text)
{
    sFONT Font = Font16;
    // char *show_text = NULL;
    uint8_t text_len = strlen(text);

    /*计算每行最多显示字的个数,和可以显示多少列，因为下标从0开始，所以-1*/
    uint8_t x_max = (Paint.Width / Font.Width) - 1;
    uint8_t y_max = ((Paint.Height - TASKBARH_HIGH) / Font.Height) - 1;

    /*判断起点字符是否超出显示*/
    uint8_t x_start = (x > x_max) ? x_max : x;
    uint8_t y_start = (y > y_max) ? y_max : y;
    /*判断是否可以完整显示*/
    int8_t show_max_len = x_max - (x + text_len);
    if (show_max_len < 0)
        text_len = x_max - x_start + 1;         
    char *show_text = (char *)malloc((text_len + 1) * sizeof(char));
    strncpy(show_text, text, text_len);
    show_text[text_len] = '\0';     //结束字符
    printf("show_text = %s;len = %d\r\n", show_text, strlen(show_text));

    /*计算起点像素点*/
    x_start = x_start * Font.Width;
    y_start = y_start * Font.Height + TASKBARH_HIGH;

    printf("x_max=%d, y_max=%d, x_start=%d; y_start=%d;text=%s;text_len = %d\r\n", x_max, y_max, x_start, y_start, text, text_len);
    EPD_2IN13_Init(EPD_2IN13_PART);
    Paint_ClearWindows(x_start, y_start, x_start + Font.Width * text_len, y_start + Font.Height, WHITE);
    Paint_DrawString_EN(x_start, y_start, show_text, &Font, WHITE, BLACK);
    EPD_2IN13_Display(BlackImage);
    free(show_text);//释放内存
}


void EPD_init(void)
{
    Imagesize = ((EPD_2IN13_WIDTH % 8 == 0) ? (EPD_2IN13_WIDTH / 8) : (EPD_2IN13_WIDTH / 8 + 1)) * EPD_2IN13_HEIGHT;
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
    {
        ESP_LOGE(TAG, "Failed to apply for black memory...\r\n");
    }

    DEV_Module_Init();
    EPD_2IN13_Init(EPD_2IN13_FULL);
    EPD_2IN13_Clear();
    DEV_Delay_ms(500);
    Paint_NewImage(BlackImage, EPD_2IN13_WIDTH, EPD_2IN13_HEIGHT, 270, WHITE);
    Paint_Clear(WHITE);
    Paint_DrawBitMapFree(gImage_disconnect, 250 - 16, 0, 16, 16);
}

void EPD_start(void)
{
    xTaskCreate(time_Task, "PaperTimeTask", 4096 * 2, (void *)0, 5, &xTimeTask);
    configASSERT(xTimeTask);
    if (NULL == xTimeTask)
    {
        ESP_LOGE(TAG, "Paper Time Task Create Error\r\n");
    }

    xTaskCreate(content_Task, "PaperContentTask", 4096 * 8, (void *)0, 5, &xContentTask);
    configASSERT(xContentTask);
    if (NULL == xContentTask)
    {
        ESP_LOGE(TAG, "Paper Content Task Create Error\r\n");
    }
}