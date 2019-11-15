#include <cstdint>
#include <sstream>

#include <Arduino.h>
#include <unity.h>
#
#include "WiFiMode.h"
#include "defaults.h"


void setDefaultWiFiParams(wifi::DTOConfig& wifi_params) {
    wifi_params.apChannel = WAP_CHANNEL;
    wifi_params.apMaxConn = WAP_MAXCONN;
    wifi_params.WAP_enabled = WAP_ENABLED;
    wifi_params.WST_enabled = WST_ENABLED;
    wifi_params.isOpen = false;
    wifi_params.apSSID = tinystring::String(WAP_SSID);
    wifi_params.apPassword = tinystring::String(WAP_PASS);
} 

void test_wifi_mode_with_default_values(void) {
    wifi::DTOConfig wifi_parameters;
    setDefaultWiFiParams(wifi_parameters);
    wifi::mode::begin(wifi_parameters);
}


extern "C" void app_main() {
    delay(2000);
    UNITY_BEGIN();
        //RUN_TEST();
    UNITY_END();
}