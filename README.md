# ESP32-DynamicWiFi
Hot managing of WiFi settings for ESP32.

This library is intended to be used with official c++ libraries from Espressif (originally, the ESP32 library included in AWS client library from 2018).

This c++ library was developed based on https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html

Based on Kolban Septembre-2018 book.

## Use
Initialization of the wifi module (mandatory): void wifi_api_init(void)   

Changing wifi settings: void wifi_save_new_config(char*, char*)
    First argument: SSID string
    Second argument: Password string

Getting IP:  char* wifi_get_ip(void)
    Returns the IP of the ESP32 in "xxx.xxx.xxx.xxx" string format.

Waiting for connection:  void wifi_wait_connected(void)
    Pauses the execution with an semaphore until connection is established.

Gets connection status: bool wifi_query_connected(void)
    Returns true if the connection was stablished.

