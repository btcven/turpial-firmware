/**
 * @file test_wifi_mode.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-17
 * @copyright Copyright (c) 2019
 * 
 */


#include "WiFiMode.h"
#include "defaults.h"
#include "sdkconfig.h"
#include <Arduino.h>
#include <cstdint>
#include <sstream>
#include <unity.h>


void set_default_wifi_params(wifi::DTOConfig& wifi_params)
{
    wifi_params.ap_channel = WAP_CHANNEL;
    wifi_params.ap_max_conn = WAP_MAXCONN;
    wifi_params.wap_enabled = WAP_ENABLED;
    wifi_params.wst_enabled = WST_ENABLED;
    wifi_params.is_open = false;
    wifi_params.ap_ssid = tinystring::String(WAP_SSID);
    wifi_params.ap_password = tinystring::String(WAP_PASS);
    wifi_params.wst_ssid = tinystring::String(WST_SSID);
    wifi_params.wst_password = tinystring::String(WST_PASS);
}

void test_wifi_mode_with_default_values(void)
{
    const TickType_t xDelay = 10000 / portTICK_PERIOD_MS;
    wifi::DTOConfig wifi_parameters;
    set_default_wifi_params(wifi_parameters);
    std::cout << "wst_ssid: " << wifi_parameters.wst_ssid.c_str() << std::endl;
    std::cout << "wst_password: " << wifi_parameters.wst_password.c_str() << std::endl;
    std::cout << "wap_enabled: " << wifi_parameters.wap_enabled << std::endl;
    std::cout << "wst_enabled: " << wifi_parameters.wst_enabled << std::endl;
    wifi::mode::begin(wifi_parameters);
    vTaskDelay(xDelay);
}


extern "C" void app_main()
{
    initArduino();
    UNITY_BEGIN();
    RUN_TEST(test_wifi_mode_with_default_values);
    UNITY_END();
}