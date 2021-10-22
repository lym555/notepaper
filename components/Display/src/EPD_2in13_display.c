#include "EPD_2in3_display.h"
#include "EPD_2in13.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "epd_queue.h"
#include "mqtt_task.h"
#include "ImageData.h"
#include "cJSON.h"
#include "coap_client.h"
#include "tool.h"

// void time_init_display(void);

/**>时间显示的坐标和长度*/
uint8_t NPT_FLAG = 0;

static const char *TAG = "EPD_2IN13_TEST";
char *data = "";


TaskHandle_t xTimeTask = NULL;
TaskHandle_t xContentTask = NULL;
QueueHandle_t epdQueue = NULL;
//Create a new image cache
UBYTE *BlackImage;
/* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
UWORD Imagesize = 0;

static void paint_degrees_icon(uint8_t x, uint8_t y, sFONT *Font)
{
    Paint_DrawCircle(x, y + 4, 2, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawString_EN(x + 2, y, "C", Font, WHITE, BLACK);
}
static void paint_wins(void)
{
    /**>绘制网格*/
    Paint_DrawLine(1, TASKBARH_HIGH + 1, Paint.Width, TASKBARH_HIGH + 1, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);  //横线
    Paint_DrawLine(wind_width, TASKBARH_HIGH, wind_width, Paint.Height, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);   //竖线
    Paint_DrawLine(wind_width, wind_high - 1, Paint.Width, wind_high - 1, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID); //横线
}

void EPD_update_ntp(void)
{
    Paint_ClearWindows(0, 0, TIME_FONT_WIDTH * TIME_LEN, TIME_FONT_HEIGHT, WHITE);
    Paint_DrawTime_Month(0, 0, &sPaint_time, &Font16, WHITE, BLACK);

    Paint_ClearWindows(TIME_X, TIME_Y, TIME_X + TIME_FONT_WIDTH * TIME_LEN, TIME_Y + TIME_FONT_HEIGHT, WHITE);
    Paint_DrawTime(TIME_X, TIME_Y, &sPaint_time, &Font16, WHITE, BLACK);
}

void EPD_display_weather_now()
{
    printf("weather_now.code = %s\r\n", weather_now.code);
    printf("weather_now.text = %s\r\n", weather_now.text);
    uint8_t x_start = 10;
    uint8_t y_start = 32;
    // /**>当前温度*/
    // Paint_ClearWindows(x_start, y_start,x_start+Font24.Width * strlen(weather_now.temp),y_start+Font24.Height,WHITE);
    Paint_DrawString_EN(x_start, y_start, weather_now.temp, &Font24, WHITE, BLACK);
    x_start += Font24.Width * strlen(weather_now.temp) + INTERVAL;
    paint_degrees_icon(x_start, y_start, &Font24);

    /**>当前天气*/
    x_start = 10;
    y_start += Font24.Height + INTERVAL;
    Paint_DrawString_EN(x_start, y_start, weather_now.text, &Font12, WHITE, BLACK);

    /**>湿度*/
    y_start = Paint.Height - Font16.Height;
    Paint_DrawBitMapFree(gImage_humidity_16, x_start, y_start, 16, 16);
    x_start += 16;

    strcat(weather_now.humidity, "%");
    Paint_DrawString_EN(x_start, y_start + 2, weather_now.humidity, &Font12, WHITE, BLACK);
    /**>温度上限*/
    x_start = wind_width / 3 * 2;
    Paint_DrawBitMapFree(gImage_upper_limit_16, x_start, y_start, 16, 16);
    x_start += 16 + INTERVAL;
    Paint_DrawString_EN(x_start, y_start + 2, weather_3d.d1.tempMax, &Font12, WHITE, BLACK);

    /**>温度下限*/
    x_start = wind_width / 3 + 8;
    Paint_DrawBitMapFree(gImage_lower_limit_16, x_start, y_start, 16, 16);
    x_start += 16 + INTERVAL;
    Paint_DrawString_EN(x_start, y_start + 2, weather_3d.d1.tempMin, &Font12, WHITE, BLACK);

    /**>当前天气图标*/
    x_start = wind_width - 48 - 8;
    y_start = 24;
    uint16_t icon_id = atoi(weather_now.icon);
    // Paint_DrawBitMapFree(get_weather_icon_48(icon_id), x_start, y_start, 48, 48); //天气图标
    /**-----------------------------------------**/
    Paint_DrawBitMapFree(get_weather_icon_48(icon_id), x_start, y_start, 48, 48); //天气图标
    /**-----------------------------------------**/
}

void EPD_display_weather_3d()
{
    printf("weather_3d.d1.fxDate = %s ", weather_3d.d1.fxDate);
    printf("weather_3d.d1.tempMin = %s ", weather_3d.d1.tempMin);
    printf("weather_3d.d1.tempMax = %s ", weather_3d.d1.tempMax);
    printf("weather_3d.d1.iconDay = %s ", weather_3d.d1.iconDay);
    printf("weather_3d.d1.textDay = %s ", weather_3d.d1.textDay);

    uint8_t x_2d_fx = wind_width;
    uint8_t y_2d = 16 + INTERVAL;
    char str_fx[5];
    strcup(str_fx, weather_3d.d2.fxDate, 5, 5);
    uint8_t x_start = wind_width - 48 - 8;
    uint8_t y_start = 24;

    Paint_DrawString_EN(x_2d_fx, y_2d, str_fx, &Font12, WHITE, BLACK);

    /**>温度下限*/
    x_start = wind_width + INTERVAL * 2;
    y_start = wind_high - 16 - INTERVAL;
    Paint_DrawBitMapFree(gImage_lower_limit_16, x_start, y_start, 16, 16);

    x_start += 16 + INTERVAL;
    Paint_DrawString_EN(x_start, y_start + 2, weather_3d.d2.tempMin, &Font12, WHITE, BLACK);

    /**>温度上限*/
    x_start = Paint.Width - 16 - Font12.Width * 2 - INTERVAL * 2;
    Paint_DrawBitMapFree(gImage_upper_limit_16, x_start, y_start, 16, 16);
    x_start += 16 + INTERVAL;
    Paint_DrawString_EN(x_start, y_start + 2, weather_3d.d2.tempMax, &Font12, WHITE, BLACK);

    uint8_t x_3d = 160;
    uint8_t y_3d = wind_high + INTERVAL;

    // char str_fx[5];
    strcup(str_fx, weather_3d.d3.fxDate, 5, 5);
    Paint_DrawString_EN(x_3d, y_3d, str_fx, &Font12, WHITE, BLACK);

    /**>温度上限*/
    x_start = wind_width + INTERVAL * 2;
    y_start = Paint.Height - 16;
    Paint_DrawBitMapFree(gImage_lower_limit_16, x_start, y_start, 16, 16);
    x_start += 16 + INTERVAL;
    Paint_DrawString_EN(x_start, y_start + 2, weather_3d.d3.tempMin, &Font12, WHITE, BLACK);

    /**>温度下限*/
    x_start = Paint.Width - 16 - Font12.Width * 2 - INTERVAL * 2;
    Paint_DrawBitMapFree(gImage_upper_limit_16, x_start, y_start, 16, 16);
    x_start += 16 + INTERVAL;
    Paint_DrawString_EN(x_start, y_start + 2, weather_3d.d3.tempMax, &Font12, WHITE, BLACK);

    x_2d_fx = Paint.Width - 32 - INTERVAL;
    y_2d = 16;
    Paint_DrawBitMapFree(get_weather_icon_32(atoi(weather_3d.d2.iconDay)), x_2d_fx, y_2d, 32, 32); //天气图标

    x_3d = Paint.Width - 32 - INTERVAL;
    y_3d = wind_high;
    /**-----------------------------------------**/
    Paint_DrawBitMapFree(get_weather_icon_32(atoi(weather_3d.d3.iconDay)), x_3d, y_3d, 32, 32); //天气图标
    /**-----------------------------------------**/
}

void time_Task(void *pvParameters)
{
    static portTickType xLastWakeTime;
    // uint8_t TIME_X = EPD_2IN13_HEIGHT - TASKBARH_IMAGE * 2 - TIME_FONT_WIDTH * TIME_LEN - INTERVAL * 2;
    // uint8_t TIME_Y = 0;
    sFONT Font = Font16;

    // uint8_t start_x = (Paint.Width - Font.Width * content_len) / 2;
    xLastWakeTime = xTaskGetTickCount(); //获取计数
    EPD_2IN13_Init(EPD_2IN13_PART);      //局部刷新
    Paint_SelectImage(BlackImage);       //选择图像存储空间
    while (1)
    {
        // if (NPT_FLAG == 1)
        // {
        //     ESP_LOGD(TAG, "---------更新网络时间------");
        //     // EPD_2IN13_Init(EPD_2IN13_FULL); //全局刷新
        //     // EPD_2IN13_Init(EPD_2IN13_PART); //局部刷新

        //     /*绘制月份*/
        //     Paint_ClearWindows(0, 0, TIME_FONT_WIDTH * TIME_LEN, TIME_FONT_HEIGHT, WHITE);
        //     Paint_DrawTime_Month(0, 0, &sPaint_time, &Font, WHITE, BLACK);

        //     Paint_ClearWindows(TIME_X, TIME_Y, TIME_X + TIME_FONT_WIDTH * TIME_LEN, TIME_Y + TIME_FONT_HEIGHT, WHITE);
        //     Paint_DrawTime(TIME_X, TIME_Y, &sPaint_time, &Font, WHITE, BLACK);
        //     EPD_2IN13_Display(BlackImage);
        //     NPT_FLAG = 0;
        // }
        sPaint_time.Sec = sPaint_time.Sec + 1;
        if (sPaint_time.Sec == 60)
        {
            sPaint_time.Min = sPaint_time.Min + 1;
            sPaint_time.Sec = 0;
            if (sPaint_time.Min == 60)
            {
                sPaint_time.Hour = sPaint_time.Hour + 1;
                sPaint_time.Min = 0;
                if (sPaint_time.Hour == 24)
                {
                    sPaint_time.Hour = 0;
                    sPaint_time.Min = 0;
                    sPaint_time.Sec = 0;
                }
            }
        }
        if (sPaint_time.Sec == 0)
        {
            EPD_2IN13_Init(EPD_2IN13_PART);
            Paint_ClearWindows(TIME_X, TIME_Y, TIME_X + Font.Width * TIME_LEN, TIME_Y + Font.Height, WHITE);
            Paint_DrawTime(TIME_X, TIME_Y, &sPaint_time, &Font, WHITE, BLACK);
            EPD_2IN13_Display(BlackImage);
        }
        vTaskDelayUntil(&xLastWakeTime, 1 * 1000 / portTICK_RATE_MS);
    }
}

void content_Task(void *pvParameters)
{
    epdMessage *pxRxedMessage = NULL; //结构体类型的指针变量
    pxRxedMessage = (epdMessage *)malloc(sizeof(epdMessage));
    // pxRxedMessage = &xMessage;                //指针指向AMessage结构体
    while (1)
    {
        if (epdQueue == NULL)
        {
            ESP_LOGE(TAG, "消息队列未初始化");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        if (xQueueReceive(epdQueue, (void *)pxRxedMessage, (TickType_t)10))
        {
            if (pxRxedMessage->data_type == QUEUE_TYPE_MQTT)
            {
                ESP_LOGI(TAG, "QUEUE_TYPE_MQTT:Topic:%s\r\n", pxRxedMessage->topic);
                if (strcmp(pxRxedMessage->topic, SUB_TOPIC_REFRESH) == 0)
                {
                    EPD_display_refresh(pxRxedMessage->data, pxRxedMessage->data_len);
                }
                if (strcmp(pxRxedMessage->topic, SUB_TOPIC_DATA) == 0)
                {
                    printf("MQTT:data:=%s\r\n", pxRxedMessage->data);
                    EPD_display_text(pxRxedMessage->data, pxRxedMessage->data_len);
                }
            }
            else if (pxRxedMessage->data_type == QUEUE_TYPE_WEATHER_NOW)
            {
                printf("更新当前天气");
            }
            else if (pxRxedMessage->data_type == QUEUE_TYPE_WEATHER_3D)
            {
                printf("更新未来三天天气");
            }
        }

        if (weather_now.flag == 1 && weather_3d.flag == 1 && NPT_FLAG == 1)
        {
            ESP_LOGI(TAG, "-----update weather--------\r\n");
            weather_now.flag = 0;
            weather_3d.flag = 0;
            NPT_FLAG = 0;
            EPD_display_weather_now();
            EPD_display_weather_3d();
            EPD_update_ntp();
            paint_wins();
            EPD_2IN13_Display(BlackImage);
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

// const unsigned char *get_weather_icon_48(uint16_t id)
// {
//     switch (id)
//     {
//     case 100:
//         return gImage_clear_48; //晴天
//     case 101:
//     case 102:
//     case 103:
//     case 154:
//         return gImage_cloudy_48; //多云
//     case 104:
//         return gImage_overcast_48; //阴天

//     case 305:
//         return gImage_light_rain_48; //小雨

//     case 306:
//         return gImage_moderate_rain_48; //中雨

//     case 307:
//         return gImage_heavy_rain_48; //大雨

//     default:
//         return gImage_clear_48;
//     }
// }
// const unsigned char *get_weather_icon_32(uint16_t id)
// {
//     switch (id)
//     {
//     case 100:
//         return gImage_clear_32; //晴天
//     case 101:
//     case 102:
//     case 103:
//     case 154:
//         return gImage_cloudy_32; //多云
//     case 104:
//         return gImage_overcast_32; //阴天

//     case 305:
//         return gImage_light_rain_32; //小雨

//     case 306:
//         return gImage_moderate_rain_32; //中雨

//     case 307:
//         return gImage_heavy_rain_32; //大雨

//     default:
//         return gImage_clear_32;
//     }
// }

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
    show_text[text_len] = '\0'; //结束字符
    printf("show_text = %s;len = %d\r\n", show_text, strlen(show_text));

    /*计算起点像素点*/
    x_start = x_start * Font.Width;
    y_start = y_start * Font.Height + TASKBARH_HIGH;

    printf("x_max=%d, y_max=%d, x_start=%d; y_start=%d;text=%s;text_len = %d\r\n", x_max, y_max, x_start, y_start, text, text_len);
    EPD_2IN13_Init(EPD_2IN13_PART);
    Paint_ClearWindows(x_start, y_start, x_start + Font.Width * text_len, y_start + Font.Height, WHITE);
    Paint_DrawString_EN(x_start, y_start, show_text, &Font, WHITE, BLACK);
    EPD_2IN13_Display(BlackImage);
    free(show_text); //释放内存
}

void time_init_display(void)
{
    // uint8_t TIME_X = EPD_2IN13_HEIGHT - TASKBARH_IMAGE * 2 - TIME_FONT_WIDTH * TIME_LEN - INTERVAL * 2;
    // uint8_t TIME_Y = 0;
    sFONT Font = Font16;
    sPaint_time.Hour = 00;
    sPaint_time.Min = 00;
    Paint_SelectImage(BlackImage);                                     //选择图像存储空间
    Paint_DrawTime(TIME_X, TIME_Y, &sPaint_time, &Font, WHITE, BLACK); // 时间
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

    // time_init_display();
    Paint_DrawBitMapFree(gImage_battery_half, Paint.Width - 18, 0, 16, 16);
    Paint_DrawBitMapFree(gImage_disconnect, Paint.Width - 18 * 2, 0, 16, 16); // Wi-Fi

    EPD_2IN13_Display(BlackImage);
}

/*Wi-Fi连接没问题，更新Wi-Fi图标*/
static void up_wifi_connected()
{
    Paint_ClearWindows(Paint.Width - 18 * 2, 0, 16, 16, WHITE);
    Paint_DrawBitMapFree(gImage_connect, Paint.Width - 18 * 2, 0, 16, 16);
    // EPD_2IN13_Display(BlackImage);
    printf("wifi connect success\r\n");
}
void EPD_start(void)
{
    up_wifi_connected();

    epdQueue = xQueueCreate(QUEUE_LEN, sizeof(struct MQTTMessage *));
    if (NULL == epdQueue)
    {
        ESP_LOGE(TAG, "创建Queue消息队列失败\r\n");
    }

    xTaskCreate(time_Task, "PaperTimeTask", 4096 * 2, (void *)0, 5, &xTimeTask);
    configASSERT(xTimeTask);
    if (NULL == xTimeTask)
    {
        ESP_LOGE(TAG, "Paper Time Task Create Error\r\n");
    }

    xTaskCreate(content_Task, "PaperContentTask", 1024 * 32, (void *)0, 5, &xContentTask);
    configASSERT(xContentTask);
    if (NULL == xContentTask)
    {
        ESP_LOGE(TAG, "Paper Content Task Create Error\r\n");
    }
}