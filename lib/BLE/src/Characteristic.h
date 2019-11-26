/**
 * @file Characteristic.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-22
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef BLE_CHARACTERISTIC_H
#define BLE_CHARACTERISTIC_H

#include "esp_gatts_api.h"
#include <cstdint>
#include <memory>
#include <vector>

#include "Sema.h"
#include "Uuid.h"

namespace ble {

class Value
{
public:
    Value();

    Value(Value const&) = delete;
    Value& operator=(Value const&) = delete;
    Value& operator=(Value&&) = delete;

    Value(Value&& other);


    std::vector<std::uint8_t>& get() { return m_value; }
    void set(std::uint8_t* value, std::size_t len);

    void addPart(std::uint8_t* value, std::size_t len);
    void commit();
    void cancel();

private:
    std::vector<std::uint8_t> m_value;
    std::vector<std::uint8_t> m_part_value;
};

class Characteristic;

class CharacteristicCallback
{
public:
    virtual void onRead(Characteristic& characteristic) = 0;
    virtual void onWrite(Characteristic& characteristic) = 0;
};

class Characteristic
{
public:
    Characteristic(
        Uuid uuid,
        std::unique_ptr<CharacteristicCallback>&& char_cb,
        esp_gatt_perm_t perm,
        esp_gatt_char_prop_t property);

    Characteristic(Characteristic const&) = delete;
    Characteristic& operator=(Characteristic const&) = delete;

    Characteristic(Characteristic&& other);

    void create(std::uint16_t service_handle);

    Value& value() { return m_value; }

    void handleEvent(
        esp_gatts_cb_event_t event,
        esp_gatt_if_t gatts_if,
        esp_ble_gatts_cb_param_t* param);

private:
    Uuid m_uuid;
    std::unique_ptr<CharacteristicCallback> m_char_cb;
    esp_gatt_perm_t m_perm;
    esp_gatt_char_prop_t m_property;

    std::uint16_t m_handle;
    Value m_value;
    bool m_write_evt;

    util::Semaphore m_create_sema;
}; // namespace ble

} // namespace ble

#endif // BLE_CHARACTERISTIC_H