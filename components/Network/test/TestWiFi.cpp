
/**
 * Copyright 2020 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @ingroup     network
 * 
 * @file        test_wifi_mode.cpp
 * @author      Locha Mesh Developers (contact@locha.io)
 * @brief       Wi-Fi network interface
 * 
 */

#include "Network/WiFi.h"
#include "Storage.h"
#include "defaults.h"
#include <Network/Network.h>
#include <cstring>
#include <iostream>
#include <unity.h>

TEST_CASE("Network initialize", "[network]")
{
    esp_err_t err = network::init();
    if (err != ESP_OK) TEST_FAIL();
}


TEST_CASE("Set mode WiFi", "[network]")
{
    esp_err_t err = network::netif_wifi.setMode(WIFI_MODE);
    if (err != ESP_OK) TEST_FAIL();
}


TEST_CASE("Set ap config", "[network]")
{
    wifi_config_t ap_config;
    ap_config.ap.ssid_len = 0;
    memcpy(ap_config.ap.ssid, WAP_SSID, sizeof(WAP_SSID));
    memcpy(ap_config.ap.password, WAP_PASS, sizeof(WAP_PASS));
    ap_config.ap.authmode = WAP_AUTHMODE;
    ap_config.ap.max_connection = WAP_MAXCONN;
    ap_config.ap.channel = WAP_CHANNEL;
    esp_err_t err = network::netif_wifi.setApConfig(ap_config);
    if (err != ESP_OK) TEST_FAIL();
}


TEST_CASE("Set ap config", "[network]")
{
    wifi_config_t ap_config;
    ap_config.ap.ssid_len = 0;
    memcpy(ap_config.ap.ssid, WAP_SSID, sizeof(WAP_SSID));
    memcpy(ap_config.ap.password, WAP_PASS, sizeof(WAP_PASS));
    ap_config.ap.authmode = WAP_AUTHMODE;
    ap_config.ap.max_connection = WAP_MAXCONN;
    ap_config.ap.channel = WAP_CHANNEL;
    esp_err_t err = network::netif_wifi.setApConfig(ap_config);
    if (err != ESP_OK) TEST_FAIL();
}


TEST_CASE("set STA config", "[network]")
{
    wifi_config_t sta_config;
    std::memcpy(sta_config.sta.ssid, WST_SSID, sizeof(WST_SSID));
    std::memcpy(sta_config.sta.password, WST_PASS, sizeof(WST_PASS));
    esp_err_t err = network::netif_wifi.setStaConfig(sta_config);
    if (err != ESP_OK) TEST_FAIL();
}

TEST_CASE("checking if it is ap mode", "[network]")
{
    bool result = network::netif_wifi.isAp();
    TEST_ASSERT_TRUE(result);
}

TEST_CASE("checking if it is STA mode", "[network]")
{
    bool result = network::netif_wifi.isSta();
    TEST_ASSERT_TRUE(result);
}


TEST_CASE("stop WiFi", "[network]")
{
    esp_err_t err = network::netif_wifi.stop();
    if (err != ESP_OK) TEST_FAIL();
}
