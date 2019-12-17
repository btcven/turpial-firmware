/**
 * @file Service.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-21
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#include <cstdint>
#include <vector>

#include <esp_gatts_api.h>

#include "Characteristic.h"
#include "Sema.h"
#include "Uuid.h"

namespace ble {

/**
 * @brief BLE Service
 * 
 */
class Service
{
public:
    /**
     * @brief Construct a new Service
     * 
     * @param uuid: Service UUID
     * @param num_handles: Maximum number of handles for this Service. You can
     * can calculate how much handles you are going to need using the following
     * formula:
     *      num_handles = 1 + (2 * num_characteristics) + num_descriptors
     * @param inst_id: Service instance id
     */
    Service(Uuid uuid, std::uint16_t num_handles, std::uint8_t inst_id);

    /**
     * @brief Construct a new Service by moving another Service
     * 
     * @param other: the other Service
     */
    Service(Service&& other);

    Service(Service const&) = delete;
    Service& operator=(Service const&) = delete;
    Service& operator=(Service&&) = delete;

    /**
     * @brief Create the Service
     * 
     * @attention 1. This method is intended to be used by the Server class,
     * don't use it directly.
     */
    void create();

    /**
     * @brief Start the service
     * 
     * @attention 1. This method is intended to be used by the Server class,
     * don't use it directly.
     */
    void start();

    /**
     * @brief Add a Characteristic
     * 
     * @param characteristic: the Characteristic
     */
    void addCharacteristic(Characteristic&& characteristic);

    /**
     * @brief Set the Service handle
     *
     * @attention 1. This method is intended to be used by the Server class,
     * don't use it directly.
     * 
     * @param handle: the raw handle
     */
    void setHandle(std::uint16_t handle) { m_handle = handle; }

    /**
     * @brief Get the Service raw handle
     * 
     * @return std::uint16_t the handle
     */
    std::uint16_t getHandle() { return m_handle; }

    /**
     * @brief Handle Service events
     * 
     * @param event: the GATT evnt
     * @param gatts_if: the GATT server interface
     * @param param: the event parameter
     */
    void handleEvent(
        esp_gatts_cb_event_t event,
        esp_gatt_if_t gatts_if,
        esp_ble_gatts_cb_param_t* param);

private:
    Uuid m_uuid;                 /**< Service UUID */
    std::uint16_t m_num_handles; /**< Maximum number of handles */
    std::uint8_t m_inst_id;      /**< Instance ID */

    util::Semaphore m_char_create_sema; /**< Characteristic creation semaphore */
    std::size_t m_char_create_index;    /**< Current index of Characteristic creation*/

    std::vector<Characteristic> m_characteristics; /**< Characteristics list */
    std::uint16_t m_handle;                        /**< Raw Service handle */
};

} // namespace ble

#endif // BLE_SERVICE_H
