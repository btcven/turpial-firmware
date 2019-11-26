/**
 * @file Service.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#include <cstdint>
#include <vector>

#include "esp_gatts_api.h"

#include "Characteristic.h"
#include "Sema.h"
#include "Uuid.h"

namespace ble {

class Service
{
public:
    Service(Uuid uuid, std::uint16_t num_handles, std::uint8_t inst_id);

    Service(Service&& other);

    Service(Service const&) = delete;
    Service& operator=(Service const&) = delete;
    Service& operator=(Service&&) = delete;

    void create();
    void start();

    void addCharacteristic(Characteristic&& characteristic);

    void setHandle(std::uint16_t handle) { m_handle = handle; }
    std::uint16_t getHandle() { return m_handle; }

    void handleEvent(
        esp_gatts_cb_event_t event,
        esp_gatt_if_t gatts_if,
        esp_ble_gatts_cb_param_t* param);

private:
    Uuid m_uuid;
    std::uint16_t m_num_handles;
    std::uint8_t m_inst_id;

    util::Semaphore m_char_create_sema;
    std::size_t m_char_create_index;

    std::vector<Characteristic> m_characteristics;
    std::uint16_t m_handle;
};

} // namespace ble

#endif // BLE_SERVICE_H