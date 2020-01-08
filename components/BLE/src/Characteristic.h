/**
 * @file Characteristic.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-22
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#ifndef BLE_CHARACTERISTIC_H
#define BLE_CHARACTERISTIC_H

#include <cstdint>
#include <memory>
#include <vector>

#include <esp_gatts_api.h>

#include "Sema.h"
#include "Uuid.h"

namespace ble {

/**
 * @brief The value of a Characteristic
 * 
 */
class Value
{
public:
    /**
     * @brief Construct a new Value
     * 
     */
    Value();

    Value(Value const&) = delete;
    Value& operator=(Value const&) = delete;
    Value& operator=(Value&&) = delete;

    /**
     * @brief Construct a new Value by moving an already existing value
     * 
     * @param other: Value to move from
     */
    Value(Value&& other);

    /**
     * @brief Get a reference to the value inner buffer.
     * 
     * @attention 1. Changing the contents of this buffer changes the
     * Characteristic value.
     * 
     * @return std::vector<std::uint8_t>& the value buffer reference
     */
    std::vector<std::uint8_t>& get() { return m_value; }

    /**
     * @brief Set the value from a data buffer
     * 
     * @param value: data buffer
     * @param len: length of the data buffer
     */
    void set(std::uint8_t* value, std::size_t len);

    /**
     * @brief Add a part of the write from a data buffer
     * 
     * @attention 1. This method is used internally by Characteristic for
     * partial writes, don't use this method outisde of Characteristic.
     * 
     * @attention 2. After you are complete adding parts you must call commit()
     * or cancel() to set the value or to cancel the write respectively.
     * 
     * @param value: data buffer
     * @param len: data buffer length
     */
    void addPart(std::uint8_t* value, std::size_t len);

    /**
     * @brief Sets the value of the partial write
     * 
     */
    void commit();

    /**
     * @brief Cancels the partial write
     * 
     */
    void cancel();

    /**
     * @brief Get the partial read offset
     * 
     * @return std::size_t offset
     */
    std::size_t getReadOffset() { return m_read_offset; }

    /**
     * @brief Set the partial read offset
     * 
     * @param offset: the offset
     */
    void setReadOffset(std::size_t offset) { m_read_offset = offset; }

private:
    // The value contained in a Characteristic
    std::vector<std::uint8_t> m_value;
    // Write buffer for partial writes
    std::vector<std::uint8_t> m_part_value;
    // Current read offset for partial reads
    std::size_t m_read_offset;
};

class Characteristic;

/**
 * @brief Callback handler class, this is intended to be used by
 * characteristics to know when a value has been written/readed.
 * 
 */
class CharacteristicCallback
{
public:
    /**
     * @brief The BLE client requested a read of the characteristic
     * 
     * @param characteristic: Characteristic instance reference.
     */
    virtual void onRead(Characteristic& characteristic) = 0;

    /**
     * @brief The BLE client requested a write to the characteristic
     * 
     * @param characteristic: Characteristic instance reference.
     */
    virtual void onWrite(Characteristic& characteristic) = 0;
};

/**
 * @brief A BLE Characteristic
 * 
 */
class Characteristic
{
public:
    /**
     * @brief Construct a new Characteristic
     * 
     * @param uuid: UUID
     * @param char_cb: Callback handler
     * @param perm: Permissions
     * @param property: Properties
     */
    Characteristic(
        Uuid uuid,
        std::unique_ptr<CharacteristicCallback>&& char_cb,
        esp_gatt_perm_t perm,
        esp_gatt_char_prop_t property);

    Characteristic(Characteristic const&) = delete;
    Characteristic& operator=(Characteristic const&) = delete;

    /**
     * @brief Construct a new Characteristic by moving a Characteristic
     * 
     * @param other: the Characteristic to be moved from
     */
    Characteristic(Characteristic&& other);

    /**
     * @brief Create the characteristic
     * 
     * @attention 1. This is meant to be used by the Service class, don't use
     * this method directly.
     * 
     * @param service_handle: the Service handle, this is where the
     * Characteristic is being added to.
     */
    void create(std::uint16_t service_handle);

    /**
     * @brief Get the actual value of this Characteristic as a reference
     * 
     * @attention 1. Changing the it changes the value of the Characteristic,
     * this method is intended to be used inside onRead, onWrite callbacks.
     * 
     * @return Value& the value reference
     */
    Value& value() { return m_value; }

    /**
     * @brief Set the Characteristic handle
     * 
     * @attention 1. This method is used by the Service class to save the
     * Characteristic handle when the creation is complete.
     * 
     * @param handle: Characteristic raw handle
     */
    void setHandle(std::uint16_t handle) { m_handle = handle; }

    /**
     * @brief Handle Characteristic events
     * 
     * @param event: the GATT event
     * @param gatts_if: the GATT interface
     * @param param: event information
     */
    void handleEvent(
        esp_gatts_cb_event_t event,
        esp_gatt_if_t gatts_if,
        esp_ble_gatts_cb_param_t* param);

private:
    // Characteristic value
    Uuid m_uuid;
    // Callback handler
    std::unique_ptr<CharacteristicCallback> m_char_cb;
    // Permissions
    esp_gatt_perm_t m_perm;
    // Properties
    esp_gatt_char_prop_t m_property;

    // Characteristic handle
    std::uint16_t m_handle;
    // The value of the characteristic
    Value m_value;
    // Whether we are in a characteristic event.
    bool m_write_evt;
}; // namespace ble

} // namespace ble

#endif // BLE_CHARACTERISTIC_H
