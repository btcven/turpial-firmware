
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
 * @file VainaTest.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief VAINA - Versatile Address Interface | Network Administration
 */

#include "Vaina.h"
#include <iostream>
#include <unity.h>

TEST_CASE("execute conexion event", "[Vaina]")
{
    Vaina& vaina = Vaina::getInstance();

    wifi_event_ap_staconnected_t data;
    data.mac = {0xf0,
        0x18,
        0x98,
        0x82,
        0x72,
        0x44};
    data.aid = 44;

    esp_err_t err = vaina.staConnected(&data);
    if (err != ESP_OK) TEST_FAIL();
}


TEST_CASE("execute conexion event", "[Vaina]")
{
    Vaina& vaina = Vaina::getInstance();

    wifi_event_ap_staconnected_t data;
    data.mac = {0xf0,
        0x18,
        0x98,
        0x82,
        0x72,
        0x44};
    data.aid = 44;

    esp_err_t err = vaina.staDisconected(&data);
    if (err != ESP_OK) TEST_FAIL();
}