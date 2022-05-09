/*
 * control_wifi.c
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

#include "dynamicWiFi.h"

#define BIT_WIFI_STOPPED 0x1
#define BIT_WIFI_CONNECTED 0x2

static const char *TAG = "modulo wifi";

//wifi_country_t configuracion_regional = {.cc=WIFI_COUNTRY_EU, .schan=1, .nchan=13, .policy=WIFI_COUNTRY_POLICY_AUTO};

static wifi_config_t wifi_config;
static char wifi_ip[16]="000.000.000.000";

static bool wifi_flag_connected;
static bool wifi_flag_connecting;

static EventGroupHandle_t wifi_event_group;

static esp_err_t wifi_event_handler (void*, system_event_t*);

char* wifi_get_ip()
{
	if (wifi_flag_connected)
		return wifi_ip;
	else
		return NULL;
}

inline bool wifi_query_connected()
{
	return wifi_flag_connected;
}

inline void wifi_wait_connected()
{
	xEventGroupWaitBits(wifi_event_group, BIT_WIFI_CONNECTED, pdFALSE, pdTRUE, portMAX_DELAY);
}

void wifi_save_new_config(char* new_SSID, char* new_password)
{
	strcpy((char*)wifi_config.sta.ssid, new_SSID);
	strcpy((char*)wifi_config.sta.password, new_password);
	wifi_config.sta.bssid_set = 0;
	wifi_config.sta.channel = 0;

	xEventGroupClearBits(wifi_event_group, BIT_WIFI_STOPPED|BIT_WIFI_CONNECTED);
	wifi_flag_connecting = false;
	esp_wifi_stop();
	xEventGroupWaitBits( wifi_event_group, BIT_WIFI_STOPPED, pdTRUE, pdTRUE, portMAX_DELAY);

	wifi_flag_connected = false;

	ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

	wifi_flag_connecting = true;

	printf("Conecting to SSID: %s\n", wifi_config.sta.ssid);

	ESP_ERROR_CHECK( esp_wifi_start());
}

void wifi_api_init()
{

	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	  ESP_ERROR_CHECK(nvs_flash_erase());
	  ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	tcpip_adapter_init();

	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_init( wifi_event_handler, NULL));

	wifi_flag_connecting = true;
	wifi_flag_connected = false;

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_FLASH) );
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK( esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N));
	//ESP_ERROR_CHECK(esp_wifi_set_country(&configuracion_regional));

	ESP_ERROR_CHECK( esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_config));

	printf("Conncting to SSID: %s.\n", wifi_config.sta.ssid);

	ESP_ERROR_CHECK( esp_wifi_start());
	ESP_LOGI(TAG, "Wifi Initiated.");
}


esp_err_t wifi_event_handler (void *ctx, system_event_t *event)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if (wifi_flag_connecting)
	{
		switch(event->event_id)
		{
		case SYSTEM_EVENT_STA_CONNECTED:
			break;
		case  SYSTEM_EVENT_STA_GOT_IP:
			strcpy (wifi_ip, ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
			wifi_flag_connected = true;
			xEventGroupSetBitsFromISR(wifi_event_group, BIT_WIFI_CONNECTED, &xHigherPriorityTaskWoken );
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
		case SYSTEM_EVENT_STA_START:
			esp_wifi_connect();
			wifi_flag_connected = false;
			xEventGroupClearBitsFromISR(wifi_event_group, BIT_WIFI_CONNECTED);
			break;
		default:
			break;
		}
	}
	else
	{
		if (event->event_id != SYSTEM_EVENT_STA_STOP) {return ESP_OK;}
		xEventGroupSetBitsFromISR(wifi_event_group, BIT_WIFI_STOPPED, &xHigherPriorityTaskWoken );
	}
	return ESP_OK;
}
