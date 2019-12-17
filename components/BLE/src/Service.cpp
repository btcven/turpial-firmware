/**
 * @file Service.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-21
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#include "Service.h"

#include <esp_gatt_defs.h>
#include <esp_gatts_api.h>
#include <esp_log.h>

#include "Server.h"

namespace ble {

static const char* TAG = "BLE_Service";

Service::Service(Uuid uuid, std::uint16_t num_handles, std::uint8_t inst_id)
    : m_uuid(uuid),
      m_num_handles(num_handles),
      m_inst_id(inst_id),
      m_char_create_sema("CharCreateSema"),
      m_char_create_index(0),
      m_characteristics(),
      m_handle(0)
{
}

Service::Service(Service&& other)
    : m_uuid(other.m_uuid),
      m_num_handles(other.m_num_handles),
      m_inst_id(other.m_inst_id),
      m_char_create_sema(std::move(other.m_char_create_sema)),
      m_char_create_index(other.m_char_create_index),
      m_characteristics(std::move(other.m_characteristics)),
      m_handle(other.m_handle)
{
    other.m_handle = 0;
}

void Service::create()
{
    ESP_LOGI(TAG, "create new service");

    Server& server = Server::getInstance();

    esp_gatt_srvc_id_t svrc;
    svrc.is_primary = true;
    svrc.id.inst_id = m_inst_id;
    m_uuid.copyTo(svrc.id.uuid);

    esp_err_t err;
    err = esp_ble_gatts_create_service(server.getGattsIf(),
        &svrc,
        m_num_handles);

    if (err != ESP_OK) {
        ESP_LOGE(TAG,
            "esp_ble_gatts_create_service_failed: %s",
            esp_err_to_name(err));
    }
}

void Service::start()
{
    ESP_LOGI(TAG, "start service");

    // Create all of the characteristics
    for (auto& characteristic : m_characteristics) {
        m_char_create_sema.take();
        characteristic.create(m_handle);
        m_char_create_sema.wait();
        m_char_create_index += 1;
    }

    esp_err_t err = esp_ble_gatts_start_service(m_handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG,
            "esp_ble_gatts_start_service: err =%s",
            esp_err_to_name(err));
        return;
    }
}

void Service::addCharacteristic(Characteristic&& characteristic)
{
    ESP_LOGD(TAG, "add characteristic");

    m_characteristics.push_back(std::move(characteristic));
}


void Service::handleEvent(esp_gatts_cb_event_t event,
    esp_gatt_if_t gatts_if,
    esp_ble_gatts_cb_param_t* param)
{
    switch (event) {
    case ESP_GATTS_ADD_CHAR_EVT: {
        if (param->add_char.service_handle == m_handle) {
            ESP_LOGD(TAG,
                "Characteristic add event, status %d, attr_handle = %d",
                param->add_char.status,
                param->add_char.attr_handle);

            Characteristic& characterisic = m_characteristics[m_char_create_index];
            characterisic.setHandle(param->add_char.attr_handle);
            m_char_create_sema.give();
        }
        break;
    }
    default: {
        break;
    }
    }

    for (auto& characteristic : m_characteristics) {
        characteristic.handleEvent(event, gatts_if, param);
    }
}

} // namespace ble
