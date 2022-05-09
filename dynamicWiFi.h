/*
 * control_wifi.h
 *
 * 
 * 
 *  Created on: 22 oct. 2018
 *  Author: Diego Carro Fernandez
 */
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include <esp_err.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/event_groups.h"
#include <string.h>
#include "esp_wifi.h"
#include "tcpip_adapter.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "esp_log.h"

#ifndef CONTROL_WIFI_H_
#define CONTROL_WIFI_H_

void wifi_api_init(void);   //Initialize this API and the wifi module
void wifi_save_new_config(char*, char*);    //It changes the connection settings and reboot the wifi module. New settigns are saved in wifi module in a non-volatile way.

char* wifi_get_ip(void);    //Gets the IP of the ESP32 in "xxx.xxx.xxx.xxx" string format
void wifi_wait_connected(void); //Wait for wifi to being connected
bool wifi_query_connected(void);    //Returns if the connection was stablished

#endif /* CONTROL_WIFI_H_ */
