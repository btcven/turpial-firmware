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

// BLE Preferences organization:
//
// Service: WIFI_PREFERENCES_UUID
//   - Characteristic: SSID_UUID

static const char* WIFI_PREFERENCES_UUID = "4a2ccc6f55f847da8b918aa04dc1251c";
static const char* SSID_UUID = "35ac95c13d524d64a427943240c71a85";

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

    ble::Uuid wifi_preferences_uuid;
    ble::Uuid::fromHex(WIFI_PREFERENCES_UUID, wifi_preferences_uuid);
    ble::Service wifi_preferences(wifi_preferences_uuid, 4, 0);

    std::unique_ptr<SSIDCallback> ssid_cb(new SSIDCallback());
    ble::Uuid ssid_uuid;
    ble::Uuid::fromHex(SSID_UUID, ssid_uuid);
    ble::Characteristic ssid_char(ssid_uuid,
        std::move(ssid_cb),
        ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED,
        ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE);
    wifi_preferences.addCharacteristic(std::move(ssid_char));

    server.createService(std::move(wifi_preferences));

    ESP_LOGI(TAG, "BLE Server start finished");
}

} // namespace ble_preferences