
#include <stdio.h>
#include "mqtt_task.h"

#include "EPD_2in3_display.h"
#include "ntp.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

void app_main(void)
{
    GPIO_Init();
    SPI_Init();
    EPD_init();

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());     //http 需要

    EPD_start();
    
    ntp_start();
    mqtt_app_start();

    while (1)
    {
        printf("Main ...\r\n");
        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}
