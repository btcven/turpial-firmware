/**
 * @file main.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#include <cstdio>
#include <sstream>


#include "sdkconfig.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


#include "WiFiMode.h"
#include "testRTOSCPP/Hello.hpp"

#include "BQ27441.h"
#include "Namespace.h"


#include "defaults.h"

esp_err_t readWiFiParams(wifi::DTOConfig& wifi_params)
{
    ESP_LOGD(__func__, "Reading WiFi configuration from NVS");
    nvs::Namespace wifi_nvs;
    auto err = wifi_nvs.open("wifi", NVS_READWRITE);
    if (err != ESP_OK) {
        return err;
    }

    std::stringstream blob;
    err = wifi_nvs.get_blob("wifi_dto_config", blob);
    if (err != ESP_OK) {
        return err;
    }

    wifi_params.deserialize(blob);
    return ESP_OK;
}

void setDefaultWiFiParams(wifi::DTOConfig& wifi_params)
{
    wifi_params.apChannel = WAP_CHANNEL;
    wifi_params.apMaxConn = WAP_MAXCONN;
    wifi_params.WAP_enabled = WAP_ENABLED;
    wifi_params.WST_enabled = WST_ENABLED;
    wifi_params.isOpen = false;
    wifi_params.apSSID = tinystring::String(WAP_SSID);
    wifi_params.apPassword = tinystring::String(WAP_PASS);
}

/**
 * @brief Initializes the I2C_NUM_0 i2c port as master. It also initializes
 * the BQ27441 object to manage battery status.
 * 
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t initBatteryI2C()
{
    ESP_LOGI(__func__, "initializing battery I2C port");

    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.scl_io_num = GPIO_NUM_22;
    conf.sda_io_num = GPIO_NUM_23;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 1000; // TODO: 1000?
    i2c_param_config(I2C_NUM_0, &conf);

    esp_err_t err = i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
    if (err != ESP_OK) {
        ESP_LOGE(__func__, "couldn't install master I2C driver");
        return err;
    }

    return bq27441::bq27441.begin(I2C_NUM_0);
}

void printBatteryInfo()
{
    std::uint16_t voltage;
    esc::g_bq27441.voltage(voltage);
    std::cout << "voltage:" << voltage << "mV" << std::endl;

    std::int16_t current;
    esc::g_bq27441.current(esc::CurrentMeasure::Average, current);
    std::cout << "current:" << voltage << "mA" << std::endl;

    std::uint16_t soc;
    esc::g_bq27441.soc(esc::SocMeasure::Filtered, soc);
    std::cout << "soc:" << soc << "%" << std::endl;
}

extern "C" void app_main()
{
    esp_err_t err;

    // Initialize arduino as a component
    initArduino();

    // Set logging level for all tags.
    esp_log_level_set("*", ESP_LOG_VERBOSE);

    // Initialize NVS.
    // TODO: check for error.
    err = nvs::begin();

    err = initBatteryI2C();
    if (err != ESP_OK) {
        const char* e = esp_err_to_name(err);
        ESP_LOGE(__func__, "couldn't initialize battery I2C, err: %s", e);
        return;
    }

    printBatteryInfo();
}