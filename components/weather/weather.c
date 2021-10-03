#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include "cJSON.h"
#include "EPD_2in3_display.h"
#include "EPD_2in13.h"
#include "GUI_Paint.h"
#include "ImageData.h"

#define WEB_SERVER "devapi.qweather.com"
#define WEB_PORT "80"
#define WEB_URL "https://devapi.qweather.com/v7/weather/now?location=117.282488,31.775297&key=9e54b1e3d00e4f36b813065e30b0eec7&gzip=n&lang=en"
static const char *TAG = "example";

static const char REQUEST[] = "GET " WEB_URL " HTTP/1.1\r\n"
                              "Host: " WEB_SERVER "\r\n"
                              "User-Agent: esp-idf/1.0 esp32\r\n"
                              "\r\n";

extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[] asm("_binary_server_root_cert_pem_end");

static void EPD_display_weather(char *json_str)
{

    cJSON *root = NULL;
    cJSON *item = NULL;
    root = cJSON_Parse(json_str);
    char* code = cJSON_GetObjectItem(root, "code")->valuestring;
    if (strcmp("200",code) == 0)
    {
        item = cJSON_GetObjectItem(root, "now");
        char* temp = cJSON_GetObjectItem(item, "temp")->valuestring;           //温度

        char* icon = cJSON_GetObjectItem(item, "icon")->valuestring;          //图标
        char* text = cJSON_GetObjectItem(item, "text")->valuestring;          //天气
        
        char *windDir = cJSON_GetObjectItem(item, "windDir")->valuestring;    //风向
        char* windScale = cJSON_GetObjectItem(item, "windScale")->valuestring; //风力等级
        char* windSpeed = cJSON_GetObjectItem(item, "windSpeed")->valuestring; //风速
        char* humidity = cJSON_GetObjectItem(item, "humidity")->valuestring;   //湿度
        char* precip = cJSON_GetObjectItem(item, "precip")->valuestring;       //降雨量
        char* pressure = cJSON_GetObjectItem(item, "pressure")->valuestring;  //大气压
        printf("%s\r\n", cJSON_Print(item));
        printf("temp=%s;icon=%s;code=%s\r\n",temp,icon,code);
        sFONT Font = Font20;
        EPD_2IN13_Init(EPD_2IN13_PART);
        
        char show_temp[32]="temp:";
        strcat(show_temp,temp);
        Paint_DrawString_EN(0, Font.Height, show_temp, &Font, WHITE, BLACK);

        char show_humidity[32]="humidity:";
        strcat(show_humidity,humidity);
        Paint_DrawString_EN(0, Font.Height*2, show_humidity, &Font, WHITE, BLACK);

        // char show_weather[32]="weather:";
        // strcat(show_weather,text);
        // Paint_DrawString_EN(0, Font.Height, show_weather, &Font, WHITE, BLACK);


    }
    else
    {
        Paint_DrawString_EN(0, 16, "weather error", &Font16, WHITE, BLACK);
    }

    // Paint_DrawNum(0,16*2,temp, &Font16, WHITE, BLACK);
    // Paint_DrawNum(0,16*3,123, &Font16, WHITE, BLACK);

    // EPD_2IN13_Init(EPD_2IN13_PART);
    // Paint_ClearWindows(0, 16, 16, 16, WHITE);
    // Paint_DrawString_EN(0, 16, "show_text", &Font16, WHITE, BLACK);
    EPD_2IN13_Display(BlackImage);
    cJSON_Delete(root);
}

static void https_get_request(esp_tls_cfg_t cfg)
{
    char buf[1024];
    int ret, len;

    struct esp_tls *tls = esp_tls_conn_http_new(WEB_URL, &cfg);

    if (tls != NULL)
    {
        ESP_LOGI(TAG, "Connection established...");
    }
    else
    {
        ESP_LOGE(TAG, "Connection failed...");
        goto exit;
    }

    size_t written_bytes = 0;
    do
    {
        ret = esp_tls_conn_write(tls,
                                 REQUEST + written_bytes,
                                 sizeof(REQUEST) - written_bytes);
        if (ret >= 0)
        {
            ESP_LOGI(TAG, "%d bytes written", ret);
            written_bytes += ret;
        }
        else if (ret != ESP_TLS_ERR_SSL_WANT_READ && ret != ESP_TLS_ERR_SSL_WANT_WRITE)
        {
            ESP_LOGE(TAG, "esp_tls_conn_write  returned: [0x%02X](%s)", ret, esp_err_to_name(ret));
            goto exit;
        }
    } while (written_bytes < sizeof(REQUEST));

    ESP_LOGI(TAG, "Reading HTTP response...");

    uint16_t count_recv = 0;
    do
    {
        len = sizeof(buf) - 1;
        bzero(buf, sizeof(buf));
        ret = esp_tls_conn_read(tls, (char *)buf, len);

        if (ret == ESP_TLS_ERR_SSL_WANT_WRITE || ret == ESP_TLS_ERR_SSL_WANT_READ)
        {
            continue;
        }

        if (ret < 0)
        {
            ESP_LOGE(TAG, "esp_tls_conn_read  returned [-0x%02X](%s)", -ret, esp_err_to_name(ret));
            break;
        }

        if (ret == 0)
        {
            ESP_LOGI(TAG, "connection closed");
            break;
        }

        len = ret;
        ESP_LOGD(TAG, "%d bytes read", len);
        /* Print response directly to stdout as it is read */
        for (int i = 0; i < len; i++)
        {
            if ((buf[i] == '{') && (count_recv == 0))
                count_recv = i;
            if (count_recv > 0)
            {
                buf[(i - count_recv)] = buf[i];
            }
        }
        buf[len - count_recv] = '\0';
        ESP_LOGI(TAG, "ret=%.*s", len, buf);

        EPD_display_weather(buf);

    } while (1);

exit:
    esp_tls_conn_delete(tls);
    // for (int countdown = 10; countdown >= 0; countdown--) {
    //     ESP_LOGI(TAG, "%d...", countdown);
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
}

static void https_get_request_using_crt_bundle(void)
{
    ESP_LOGI(TAG, "https_request using crt bundle");
    esp_tls_cfg_t cfg = {
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    https_get_request(cfg);
}

static void https_request_task(void *pvparameters)
{
    ESP_LOGI(TAG, "Start https_request example");

    https_get_request_using_crt_bundle();
    // https_get_request_using_crt_bundle();

    ESP_LOGI(TAG, "Finish https_request example");
    vTaskDelete(NULL);
}

void weather_start(void)
{
    xTaskCreate(&https_request_task, "https_get_task", 8192 * 2, NULL, 5, NULL);
}