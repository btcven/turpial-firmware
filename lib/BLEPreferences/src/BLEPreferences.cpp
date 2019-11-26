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

#include <cstring>

#include "esp_log.h"

#include "WiFiMode.h"

namespace ble_preferences {

static const char* TAG = "BLE_Preferences";

// BLE Preferences organization:
//
// Service: WIFI_PREFERENCES_UUID
//   - Characteristic: AP_SSID_UUID

static const char* WIFI_PREFERENCES_UUID = "4a2ccc6f55f847da8b918aa04dc1251c";
static const char* AP_SSID_UUID = "35ac95c13d524d64a427943240c71a85";
static const char* AP_PASSWORD_UUID = "7bb1a7812d1249bbab344a548d3a9e8c";

class APSSIDCallback : public ble::CharacteristicCallback
{
public:
    APSSIDCallback()
    {
    }

    virtual void onWrite(ble::Characteristic& characteristic)
    {
        ESP_LOGI(TAG, "onWrite AP SSID");

        wifi::WiFiMode& wifi = wifi::WiFiMode::getInstance();

        wifi_config_t config;
        wifi.get_ap_config(config);

        std::vector<std::uint8_t>& value = characteristic.value().get();
        if (value.size() > 32) {
            ESP_LOGW(TAG,
                "AP SSID value is higher than 32 characters, trimming");
            std::memcpy(config.ap.ssid, value.data(), 32);
            config.ap.ssid_len = 32;
        } else {
            std::memcpy(config.ap.ssid, value.data(), value.size());
            config.ap.ssid[value.size()] = '\0';
            config.ap.ssid_len = 0;
        }

        wifi.stop();
        wifi.set_ap_config(config);
        wifi.start();
    }

    virtual void onRead(ble::Characteristic& characteristic)
    {
        wifi::WiFiMode& wifi = wifi::WiFiMode::getInstance();

        wifi_config_t config;
        wifi.get_ap_config(config);

        if (config.ap.ssid_len == 0) {
            char* ssid_str = reinterpret_cast<char*>(config.ap.ssid);
            std::size_t len = std::strlen(ssid_str);
            characteristic.value().set(config.ap.ssid, len);
        } else {
            characteristic.value().set(config.ap.ssid, config.ap.ssid_len);
        }
    }
};

class APPasswordCallback : public ble::CharacteristicCallback
{
public:
    APPasswordCallback()
    {
    }

    virtual void onWrite(ble::Characteristic& characteristic)
    {
        ESP_LOGI(TAG, "onWrite AP password");

        wifi::WiFiMode& wifi = wifi::WiFiMode::getInstance();

        wifi_config_t config;
        wifi.get_ap_config(config);

        std::vector<std::uint8_t>& value = characteristic.value().get();
        if (value.size() > 63) {
            ESP_LOGW(TAG,
                "AP Password value is higher than 63 characters, trimming");
            std::memcpy(config.ap.password, value.data(), 63);
            config.ap.password[63] = '\0';
        } else {
            std::memcpy(config.ap.password, value.data(), value.size());
            config.ap.password[value.size()] = '\0';
        }

        wifi.stop();
        wifi.set_ap_config(config);
        wifi.start();
    }

    virtual void onRead(ble::Characteristic& characteristic)
    {
        wifi::WiFiMode& wifi = wifi::WiFiMode::getInstance();

        wifi_config_t config;
        wifi.get_ap_config(config);

        char* password_str = reinterpret_cast<char*>(config.ap.password);
        std::size_t len = std::strlen(password_str);
        characteristic.value().set(config.ap.password, len);
    }
};

void start(ble::ServerParams server_params)
{
    ble::Server& server = ble::Server::getInstance();

    server.init(server_params);

    server.registerApp();

    ble::Uuid wifi_preferences_uuid;
    ble::Uuid::fromHex(WIFI_PREFERENCES_UUID, wifi_preferences_uuid);
    ble::Service wifi_preferences(wifi_preferences_uuid, 5, 0);

    std::unique_ptr<APSSIDCallback> ap_ssid_cb(new APSSIDCallback());
    ble::Uuid ap_ssid_uuid;
    ble::Uuid::fromHex(AP_SSID_UUID, ap_ssid_uuid);
    ble::Characteristic ap_ssid_char(ap_ssid_uuid,
        std::move(ap_ssid_cb),
        ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED,
        ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE);
    wifi_preferences.addCharacteristic(std::move(ap_ssid_char));

    std::unique_ptr<APPasswordCallback> ap_password_cb(new APPasswordCallback());
    ble::Uuid ap_password_uuid;
    ble::Uuid::fromHex(AP_PASSWORD_UUID, ap_password_uuid);
    ble::Characteristic ap_password_char(ap_password_uuid,
        std::move(ap_password_cb),
        ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED,
        ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE);
    wifi_preferences.addCharacteristic(std::move(ap_password_char));

    server.createService(std::move(wifi_preferences));

    ESP_LOGI(TAG, "BLE Server start finished");
}

} // namespace ble_preferences