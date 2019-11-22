/**
 * @file BLESettings.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-25
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "BLEPreferences.h"

#include "esp_log.h"

namespace ble_preferences {

static const char* TAG = "BLE_Preferences";

class SSIDCallback : public ble::CharacteristicCallback
{
public:
    SSIDCallback()
    {
    }

    virtual void onWrite(ble::Characteristic& characteristic)
    {
    }

    virtual void onRead(ble::Characteristic& characteristic)
    {
    }
};

void start(ble::ServerParams server_params)
{
    ble::Server& server = ble::Server::getInstance();

    server.init(server_params);

    server.registerApp();

    ble::Service wifi_settings(ble::Uuid(0x00ff), 4, 0);

    std::unique_ptr<SSIDCallback> ssid_cb(new SSIDCallback());
    ble::Characteristic ssid_char(ble::Uuid(0xff00),
        std::move(ssid_cb),
        ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED,
        ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE);
    wifi_settings.addCharacteristic(std::move(ssid_char));

    server.createService(std::move(wifi_settings));

    ESP_LOGI(TAG, "BLE Server start finished");
}

} // namespace ble_preferences