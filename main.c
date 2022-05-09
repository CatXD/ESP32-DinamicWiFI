/*
 * main.c
 *
 * This is an example use of the library.
 * 
 * 
 *  Created on: 09 may 2020
 *  Author: Diego Carro Fernandez
 */


#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "driver/gpio.h"
#include <string.h>

#include "dynamicWiFi.h"


void app_main()
{

    wifi_api_init();

    vTaskDelay(1000/portTICK_RATE_MS);

    wifi_save_new_config("newSSID","newPassword");   //It can be used again with another settings.

    while (1)
    {
    	bool wifiConnected = wifi_query_connected();

    	vTaskDelay(50 / portTICK_RATE_MS);
    }
}

