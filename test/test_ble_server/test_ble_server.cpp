/**
 * @file test_ble_server.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-12-02
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unity.h>

#include "NVS.h"
#include "Server.h"

class DummyCallback : public ble::CharacteristicCallback
{
public:
    DummyCallback()
    {
    }

    virtual void onWrite(ble::Characteristic& characteristic)
    {
    }

    virtual void onRead(ble::Characteristic& characteristic)
    {
    }
};

void testInit()
{
    esp_err_t err;

    ble::ServerParams server_params;
    server_params.device_name = "Dummy";
    server_params.static_passkey = 123456;
    server_params.app_id = 0;

    ble::Server& server = ble::Server::getInstance();

    err = server.init(server_params);
    if (err != ESP_OK) TEST_FAIL();

    server.registerApp();

    ble::Uuid service_uuid;
    ble::Uuid::fromHex("005658e46ddf4cf5a8a9542668b90137", service_uuid);
    ble::Service service(service_uuid, 5, 0);

    std::unique_ptr<DummyCallback> dummy_cb(new DummyCallback());
    ble::Uuid dummy_uuid;
    ble::Uuid::fromHex("0d194f7945e9484fa0b77edfebaf325f", dummy_uuid);
    ble::Characteristic dummy_char(dummy_uuid,
        std::move(dummy_cb),
        ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED,
        ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE);
    service.addCharacteristic(std::move(dummy_char));

    server.createService(std::move(service));

    vTaskDelay(1000);

    server.stop();

    // Try to init/stop again to see if the stop method works as intended
    err = server.init(server_params);
    if (err != ESP_OK) TEST_FAIL();

    vTaskDelay(1000);

    server.stop();
}

extern "C" void app_main()
{
    vTaskDelay(2000);

    esp_err_t err = storage::init();
    if (err != ESP_OK) return;

    UNITY_BEGIN();
    RUN_TEST(testInit);
    UNITY_END();
}