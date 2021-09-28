
#include <stdio.h>
#include "mqtt_task.h"

#include "DEV_Config.h"
#include "EPD_Test.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"




void app_main(void)
{
    GPIO_Init();
    SPI_Init();
    ped_app_start();
    mqtt_app_start();
    
    while (1)
    {
        printf("Main ...\r\n");
        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}