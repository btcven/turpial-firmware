/**
 * @file BLEPreferences.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-25
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
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
//   - Characteristic:
//      - Uuid: WIFI_MODE_UUID
//      - Value: one of WIFI_MODE_STA, WIFI_MODE_AP, or WIFI_MODE_STA
//   - Characteristic:
//      - Uuid: AP_SSID_UUID
//      - Value: a maximum of 32 chars
//   - Characteristic:
//      - Uuid: AP_PASSWORD_UUID
//      - Value: a maximum of 63 chars
//   - Characteristic:
//      - Uuid: STA_SSID_UUID
//      - Value: a maximum of 31 chars
//   - Characteristic:
//      - Uuid: STA_PASSWORD_UUID
//      - Value: a maximum of 63 chars

// This the equivalent to: 1 (Service handle) + (2 * num characteristic)
static std::uint16_t WIFI_PREFERENCES_NUM_HANDLES = 1 + (2 * 5);

static const char* WIFI_PREFERENCES_UUID = "4a2ccc6f55f847da8b918aa04dc1251c";

static const char* WIFI_MODE_UUID = "3ab002a921d94e909e880346f37cb3f0";
static const char* AP_SSID_UUID = "35ac95c13d524d64a427943240c71a85";
static const char* AP_PASSWORD_UUID = "7bb1a7812d1249bbab344a548d3a9e8c";
static const char* STA_SSID_UUID = "03118c93de744b09b26b5953924ba715";
static const char* STA_PASSWORD_UUID = "90a96e194f0a453a9c1e8737b1e3d809";

class WiFiModeCallback : public ble::CharacteristicCallback
{
public:
    WiFiModeCallback()
    {
    }

    virtual void onWrite(ble::Characteristic& characteristic)
    {
        ESP_LOGI(TAG, "onWrite AP SSID");

        wifi::WiFiMode& wifi = wifi::WiFiMode::getInstance();

        std::vector<std::uint8_t>& value = characteristic.value().get();
        if (value.size() > 1) {
            ESP_LOGW(TAG,
                "client sent invalid Wi-Fi mode, length = %d",
                value.size());
        }

        std::uint8_t mode = value[0];
        switch (mode) {
        case WIFI_MODE_STA:
        case WIFI_MODE_AP:
        case WIFI_MODE_APSTA: {
            ESP_LOGI(TAG, "setting Wi-Fi mode = %d", mode);
            wifi.stop();
            wifi.set_mode(static_cast<wifi_mode_t>(mode));
            wifi.start();
            break;
        }
        default: {
            ESP_LOGW(TAG,
                "client sent invalid Wi-Fi mode, mode = %d",
                mode);
            break;
        }
        }
    }

    virtual void onRead(ble::Characteristic& characteristic)
    {
        wifi::WiFiMode& wifi = wifi::WiFiMode::getInstance();

        wifi_mode_t mode;
        wifi.get_mode(mode);

        std::uint8_t v = 0;
        switch (mode) {
        case WIFI_MODE_STA:
        case WIFI_MODE_AP:
        case WIFI_MODE_APSTA: {
            v = mode;
            break;
        }
        default: {
            ESP_LOGW(TAG, "no Wi-Fi mode is set, changing to AP mode...");
            wifi.stop();
            wifi.set_mode(WIFI_MODE_AP);
            wifi.start();
            v = WIFI_MODE_AP;
            break;
        }
        }

        characteristic.value().set(&v, 1);
    }
};

class APSSIDCallback : public ble::CharacteristicCallback
{
public:
    APSSIDCallback()
    {
    }

    virtual void onWrite(ble::Characteristic& characteristic)
    {
        wifi::WiFiMode& wifi = wifi::WiFiMode::getInstance();

        wifi_config_t config;
        wifi.get_ap_config(config);

        std::vector<std::uint8_t>& value = characteristic.value().get();
        if (value.size() >= 32) {
            ESP_LOGW(TAG,
                "AP SSID value is higher than 32 characters, trimming");
            std::memcpy(config.ap.ssid, value.data(), 32);
            config.ap.ssid_len = 32;
        } else if (value.size() == 0) {
            ESP_LOGW(TAG,
                "AP SSID value is empty, not changing");
            return;
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
        wifi::WiFiMode& wifi = wifi::WiFiMode::getInstance();

        wifi_config_t config;
        wifi.get_ap_config(config);

        std::vector<std::uint8_t>& value = characteristic.value().get();
        if (value.size() > 63) {
            ESP_LOGW(TAG,
                "AP Password value is higher than 63 characters, trimming");
            std::memcpy(config.ap.password, value.data(), 63);
            config.ap.password[63] = '\0';
        } else if (value.size() == 0) {
            ESP_LOGW(TAG, "AP Password is empty, not changing");
            return;
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

class STASSIDCallback : public ble::CharacteristicCallback
{
public:
    STASSIDCallback()
    {
    }

    virtual void onWrite(ble::Characteristic& characteristic)
    {
        wifi::WiFiMode& wifi = wifi::WiFiMode::getInstance();

        wifi_config_t config;
        wifi.get_sta_config(config);

        std::vector<std::uint8_t>& value = characteristic.value().get();
        if (value.size() > 31) {
            ESP_LOGW(TAG,
                "STA SSID value is higher than 31 characters, trimming");
            std::memcpy(config.sta.ssid, value.data(), 31);
            config.sta.ssid[31] = '\0';
        } else if (value.size() == 0) {
            ESP_LOGW(TAG, "STA SSID value is empty, not changing");
            return;
        } else {
            std::memcpy(config.sta.ssid, value.data(), value.size());
            config.sta.ssid[value.size()] = '\0';
        }

        wifi.stop();
        wifi.set_sta_config(config);
        wifi.start();
    }

    virtual void onRead(ble::Characteristic& characteristic)
    {
        wifi::WiFiMode& wifi = wifi::WiFiMode::getInstance();

        wifi_config_t config;
        wifi.get_sta_config(config);

        char* ssid_str = reinterpret_cast<char*>(config.sta.ssid);
        std::size_t len = std::strlen(ssid_str);
        characteristic.value().set(config.sta.ssid, len);
    }
};

class STAPasswordCallback : public ble::CharacteristicCallback
{
public:
    STAPasswordCallback()
    {
    }

    virtual void onWrite(ble::Characteristic& characteristic)
    {
        wifi::WiFiMode& wifi = wifi::WiFiMode::getInstance();

        wifi_config_t config;
        wifi.get_sta_config(config);

        std::vector<std::uint8_t>& value = characteristic.value().get();
        if (value.size() > 63) {
            ESP_LOGW(TAG,
                "AP Password value is higher than 63 characters, trimming");
            std::memcpy(config.sta.password, value.data(), 63);
            config.sta.password[63] = '\0';
        } else if (value.size() == 0) {
            config.sta.password[0] = '\0';
        } else {
            std::memcpy(config.ap.password, value.data(), value.size());
            config.sta.password[value.size()] = '\0';
        }

        wifi.stop();
        wifi.set_sta_config(config);
        wifi.start();
    }

    virtual void onRead(ble::Characteristic& characteristic)
    {
        wifi::WiFiMode& wifi = wifi::WiFiMode::getInstance();

        wifi_config_t config;
        wifi.get_sta_config(config);

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
    ble::Service wifi_preferences(wifi_preferences_uuid,
        WIFI_PREFERENCES_NUM_HANDLES,
        0);

    std::unique_ptr<WiFiModeCallback> wifi_mode_cb(new WiFiModeCallback());
    ble::Uuid wifi_mode_uuid;
    ble::Uuid::fromHex(WIFI_MODE_UUID, wifi_mode_uuid);
    ble::Characteristic wifi_mode_char(wifi_mode_uuid,
        std::move(wifi_mode_cb),
        ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED,
        ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE);
    wifi_preferences.addCharacteristic(std::move(wifi_mode_char));

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

    std::unique_ptr<STASSIDCallback> sta_ssid_cb(new STASSIDCallback());
    ble::Uuid sta_ssid_uuid;
    ble::Uuid::fromHex(STA_SSID_UUID, sta_ssid_uuid);
    ble::Characteristic sta_ssid_char(sta_ssid_uuid,
        std::move(sta_ssid_cb),
        ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED,
        ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE);
    wifi_preferences.addCharacteristic(std::move(sta_ssid_char));

    std::unique_ptr<STAPasswordCallback> sta_password_cb(new STAPasswordCallback());
    ble::Uuid sta_password_uuid;
    ble::Uuid::fromHex(STA_PASSWORD_UUID, sta_password_uuid);
    ble::Characteristic sta_password_char(sta_password_uuid,
        std::move(sta_password_cb),
        ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED,
        ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE);
    wifi_preferences.addCharacteristic(std::move(sta_password_char));

    server.createService(std::move(wifi_preferences));

    ESP_LOGI(TAG, "BLE Server start finished");
}

} // namespace ble_preferences