
/**
 * @file test_wifi_mode.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-17
 * @copyright Copyright (c) 2019
 * 
 */



#include <cstdint>
#include <sstream>
#include <Arduino.h>
#include "sdkconfig.h"
#include "WiFiMode.h"
#include "defaults.h"
#include <unity.h>



void setDefaultWiFiParams(wifi::DTOConfig& wifi_params) {
    wifi_params.apChannel = WAP_CHANNEL;
    wifi_params.apMaxConn = WAP_MAXCONN;
    wifi_params.WAP_enabled = WAP_ENABLED;
    wifi_params.WST_enabled = WST_ENABLED;
    wifi_params.isOpen = false;
    wifi_params.apSSID = tinystring::String(WAP_SSID);
    wifi_params.apPassword = tinystring::String(WAP_PASS);
    wifi_params.wstSSID = tinystring::String(WST_SSID);
    wifi_params.wstPassword = tinystring::String(WST_PASS);
} 

void test_wifi_mode_with_default_values(void) {
    const TickType_t xDelay = 10000 / portTICK_PERIOD_MS;
    wifi::DTOConfig wifi_parameters;
    setDefaultWiFiParams(wifi_parameters);
    std::cout << "wst_ssid: " << wifi_parameters.wstSSID.c_str()  << std::endl;
    std::cout << "wst_password: " << wifi_parameters.wstPassword.c_str()  << std::endl;
    std::cout << "wap_enabled: " <<wifi_parameters.WAP_enabled << std::endl;
    std::cout << "wst_enabled: " <<wifi_parameters.WST_enabled  << std::endl;
    wifi::mode::begin(wifi_parameters);
    vTaskDelay( xDelay );
}


extern "C" void app_main()
{
    initArduino();
    UNITY_BEGIN();
        RUN_TEST(test_wifi_mode_with_default_values);
    UNITY_END();
}

 