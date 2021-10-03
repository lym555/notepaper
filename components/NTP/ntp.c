#include <stdio.h>
#include "ntp.h"
#include "EPD_2in3_display.h"
/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include <string.h>
#include "cJSON.h"
#include "timestamp.h"
#include "math.h"

TaskHandle_t timeHandle=NULL;
void update_time(char *json_str);

/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "api.m.taobao.com"
#define WEB_PORT "80"
#define WEB_PATH "/rest/api3.do?api=mtop.common.getTimestamp"

static const char *TAG = "example";

static const char *REQUEST = "GET " WEB_PATH " HTTP/1.0\r\n"
    "Host: "WEB_SERVER":"WEB_PORT"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";

static void http_get_task(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[1024];

    while(1) {
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Code to print the resolved IP.

           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... allocated socket");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "... connected");
        freeaddrinfo(res);

        if (write(s, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(TAG, "... socket send failed");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... socket send success");

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... set socket receiving timeout success");

        bzero(recv_buf, sizeof(recv_buf));
        uint16_t count_recv = 0;
        /* Read HTTP response */
        do {
            
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                if((recv_buf[i] == '{') && (count_recv == 0))
                    count_recv = i;
                if (count_recv > 0)
                {
                    recv_buf[(i-count_recv)] = recv_buf[i];                 
                    // putchar(recv_buf[i-count_recv]);
                }
                
            }
            recv_buf[r-count_recv] = '\0';

        } while(r > 0);
       
        // ESP_LOGI(TAG, "Json data = %s\r\n",recv_buf);

        update_time(recv_buf);
        
        
        ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
        close(s);
        
        vTaskDelete(timeHandle);
    }
}
uint8_t strcup(char *dst, char *src, uint16_t cp_start, uint16_t cp_size)
{
    char *p = dst;
    char *q = src+cp_start;
    uint16_t size = strlen(src);
    if ((cp_start+cp_size) > size)
        return 0;

    while (cp_size--)
    {
        *(p++)=*(q++);
    }
    *(p++) = '\0';

    return 1;

}

void update_time(char *json_str)
{
    cJSON *root = NULL;
    cJSON *item = NULL;
    root = cJSON_Parse(json_str);
    item = cJSON_GetObjectItem(root, "data");

    char* time_sta = cJSON_GetObjectItem(item, "t")->valuestring;
    *(time_sta+10) = '\0';

    uint64_t time_u64 = atol(time_sta);
    // printf("ti = %llu\r\n", time_u64);


    timestamp_t ts={time_u64,0,0};;
    char buf[40];
    timestamp_format(buf, sizeof(buf), &ts);
    // printf("time = %s\r\n", buf);
    char time_s[2];
    strcup(time_s,buf,11,2);
    sPaint_time.Hour = atoi(time_s)+8;
    strcup(time_s,buf,14,2);
    sPaint_time.Min = atoi(time_s);

    NPT_FLAG = 1;   //网络对时标志
    cJSON_Delete(root); //删除最外层即可
}

void ntp_start(void)
{
    xTaskCreate(&http_get_task, "http_get_task", 4096*2, NULL, 5, &timeHandle);
}

