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
#include "EPD_Test.h"
#include "EPD_2in13.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "mqtt_queue.h"
#include "ImageData.h"
#include "cJSON.h"

static portTickType xLastWakeTime;

static const uint8_t TASKBARH_HIGH = 16;
static const char * TAG = "EPD_2IN13_TEST";
char * data = "";

TaskHandle_t xTimeTask=NULL;
TaskHandle_t xContentTask=NULL;
//Create a new image cache
UBYTE *BlackImage;
/* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
UWORD Imagesize = 0;

void time_Task(void * pvParameters)
{  	
    sFONT Font = Font16;
    uint8_t content_len = 5;        // 时间显示占5个字
    uint8_t start_x = (EPD_2IN13_HEIGHT - Font12.Width*content_len)/2;
    uint8_t start_y = 0;
    xLastWakeTime = xTaskGetTickCount();//获取计数
    EPD_2IN13_Init(EPD_2IN13_PART);     //局部刷新
    Paint_SelectImage(BlackImage);      //选择图像存储空间
    PAINT_TIME sPaint_time;
    sPaint_time.Hour = 12;
    sPaint_time.Min = 34;
    while (1)
     {
        EPD_2IN13_Init(EPD_2IN13_PART);     //局部刷新
        sPaint_time.Min= sPaint_time.Min + 1;
        if (sPaint_time.Min == 60) {
            sPaint_time.Hour =  sPaint_time.Hour + 1;
            sPaint_time.Min = 0;
            if (sPaint_time.Hour == 24) {
                sPaint_time.Hour = 0;
                sPaint_time.Min = 0;
            }
        }
        Paint_ClearWindows(start_x, start_y, start_x + Font.Width * content_len, start_y + Font.Height, WHITE);
        Paint_DrawTime(start_x, start_y, &sPaint_time, &Font, WHITE, BLACK);
        EPD_2IN13_Display(BlackImage);
        vTaskDelayUntil(&xLastWakeTime, 60*1000/portTICK_RATE_MS);
    }
}

void content_Task(void * pvParameters)
{
    struct MQTTMessage *pxRxedMessage= NULL;//结构体类型的指针变量
    pxRxedMessage = & xMessage;          //指针指向AMessage结构体
    while (1)
     {
        if( xQueue == NULL)
        {
            ESP_LOGE(TAG,"消息队列未初始化");
            vTaskDelay(1000/portTICK_PERIOD_MS);
            continue;
        }
        if( xQueueReceive( xQueue, ( void * )pxRxedMessage, ( TickType_t ) 10 ) )
        {
            /*订阅成功说明Wi-Fi连接没问题，更新Wi-Fi图标*/
            if(pxRxedMessage->data_type == QUEUE_TYPE_CONNECT_SUCCESS)
            {
                Paint_ClearWindows(250-16, 0,  16, 16, WHITE);
                Paint_DrawBitMapFree(gImage_connect,250-16,0,16,16);
                EPD_2IN13_Display(BlackImage);
                printf("wifi connect success\r\n");
                continue;
            }
            if(pxRxedMessage->data_type == QUEUE_TYPE_JSON)
            {
                cJSON *json = cJSON_ParseWithLength(pxRxedMessage->data, pxRxedMessage->data_len);
                ped_display_text(json);
                cJSON_Delete(json);
            }
        }
     }
}

void ped_display_text(cJSON *json)
{
    sFONT Font = Font16;

    /*状态栏占16位，所以y需要从16位像素开始*/
    uint8_t refersh =cJSON_GetObjectItem(json,"refresh")->valueint; 
    uint8_t x_start =cJSON_GetObjectItem(json,"x")->valueint; 
    uint8_t y_start =cJSON_GetObjectItem(json,"y")->valueint ;
    uint8_t clear_row = cJSON_GetObjectItem(json,"clear_row")->valueint ;
    char *text =cJSON_GetObjectItem(json,"text")->valuestring; 
    uint8_t text_len = strlen(text);
    /*计算每行最多显示字的个数,和可以显示多少列，因为下标从0开始，所以-1*/
    uint8_t x_max = (Paint.Width/Font.Width)-1;   
    uint8_t y_max = ((Paint.Height -TASKBARH_HIGH)/Font.Height)-1; 
    
    printf("y_max = %d\r\n",y_max);

    x_start = (x_start>x_max)?x_max:x_start;
    y_start = (y_start>y_max)?y_max:y_start;   

    x_start *= Font.Width;
    y_start = y_start * Font.Height  + TASKBARH_HIGH; 

    // printf("refersh = %d; x=%d; y=%d;text=%s;text_len = %d\r\n",refersh,x_start,y_start,text,text_len);    
    if(refersh==EPD_2IN13_PART){
        EPD_2IN13_Init(EPD_2IN13_PART);
        if (clear_row==1) Paint_ClearWindows(x_start, y_start, x_start+Font.Width*text_len, y_start + Font.Height, WHITE);
        else Paint_ClearWindows(x_start, y_start, Paint.Width, y_start + Font.Height, WHITE); //清除一整行

    }

    if(refersh==EPD_2IN13_FULL)
    {  
        EPD_2IN13_Init(EPD_2IN13_FULL);
        Paint_ClearWindows(0, TASKBARH_HIGH, Paint.Width, Paint.Height, WHITE);
    } 
    Paint_DrawString_EN(x_start, y_start, text, &Font, WHITE, BLACK);
    EPD_2IN13_Display(BlackImage);
}


void ped_app_start(void)
{
    Imagesize = ((EPD_2IN13_WIDTH % 8 == 0)? (EPD_2IN13_WIDTH / 8 ): (EPD_2IN13_WIDTH / 8 + 1)) * EPD_2IN13_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        ESP_LOGE(TAG,"Failed to apply for black memory...\r\n");
    }

    DEV_Module_Init();
    EPD_2IN13_Init(EPD_2IN13_FULL);
    EPD_2IN13_Clear();
    DEV_Delay_ms(500);

    Paint_NewImage(BlackImage, EPD_2IN13_WIDTH, EPD_2IN13_HEIGHT, 270, WHITE);
    Paint_Clear(WHITE); 

    Paint_DrawBitMapFree(gImage_disconnect,250-16,0,16,16);

    xTaskCreate(time_Task, "PaperTimeTask", 4096*2, (void *) 0, 2, &xTimeTask);
    configASSERT( xTimeTask );
    if(NULL == xTimeTask)
    {
        ESP_LOGE(TAG,"Paper Time Task Create Error\r\n");
    }

    xTaskCreate(content_Task, "PaperContentTask", 4096*8, (void *) 0, 2, &xContentTask);
    configASSERT( xContentTask );
    if(NULL == xContentTask)
    {
        ESP_LOGE(TAG,"Paper Content Task Create Error\r\n");
    }
}