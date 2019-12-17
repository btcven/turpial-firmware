/**
 * @file Characteristic.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-22
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#include "Characteristic.h"

#include <cstring>

#include "Server.h"

namespace ble {

static const char* TAG = "BLE_Characteristic";

Value::Value()
    : m_value(),
      m_part_value(),
      m_read_offset(0)
{
}

Value::Value(Value&& other)
    : m_value(std::move(other.m_value)),
      m_part_value(std::move(other.m_part_value)),
      m_read_offset(other.m_read_offset)
{
}

void Value::set(std::uint8_t* value, std::size_t len)
{
    ESP_LOGD(TAG, "Set value, len = 0%x", len);
    m_value.clear();
    m_value.reserve(len);
    for (int i = 0; i < len; ++i) {
        m_value.push_back(value[i]);
    }
}

void Value::addPart(std::uint8_t* value, std::size_t len)
{
    ESP_LOGD(TAG, "Add to value, len = 0%x", len);

    std::size_t n = m_value.size();
    m_part_value.reserve(n + len);
    for (int i = n; i < len; ++i) {
        m_part_value.push_back(value[i]);
    }
}

void Value::commit()
{
    // Set m_value to the contents of m_part_value
    m_part_value.swap(m_value);

    // Erase m_part_value contents
    m_part_value.clear();
    m_part_value.shrink_to_fit();
}

void Value::cancel()
{
    // Erase m_part_value contents
    m_part_value.clear();
    m_part_value.shrink_to_fit();
}

Characteristic::Characteristic(Uuid uuid,
    std::unique_ptr<CharacteristicCallback>&& char_cb,
    esp_gatt_perm_t perm,
    esp_gatt_char_prop_t property)
    : m_uuid(uuid),
      m_char_cb(std::move(char_cb)),
      m_perm(perm),
      m_property(property),
      m_handle(0),
      m_value()
{
}

Characteristic::Characteristic(Characteristic&& other)
    : m_uuid(other.m_uuid),
      m_char_cb(std::move(other.m_char_cb)),
      m_perm(other.m_perm),
      m_property(other.m_property),
      m_handle(other.m_handle),
      m_value(std::move(other.m_value))
{
    other.m_handle = 0;
}

void Characteristic::create(std::uint16_t service_handle)
{
    if (m_handle != 0) {
        ESP_LOGE(TAG,
            "Characteristic has been already created, m_handle = 0x%x",
            m_handle);
        return;
    }

    ESP_LOGI(TAG, "Creating characteristic (esp_ble_gatts_add_char)");

    esp_attr_control_t control;
    control.auto_rsp = ESP_GATT_RSP_BY_APP;

    esp_err_t err = esp_ble_gatts_add_char(service_handle,
        &m_uuid.getRaw(),
        m_perm,
        m_property,
        nullptr,
        &control);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't register characteristic");
    }
}

void Characteristic::handleEvent(
    esp_gatts_cb_event_t event,
    esp_gatt_if_t gatts_if,
    esp_ble_gatts_cb_param_t* param)
{
    Server& server = Server::getInstance();

    switch (event) {
    case ESP_GATTS_READ_EVT: {
        if (param->read.handle == m_handle) {
            ESP_LOGI(TAG,
                "Characteristic read event, handle = %d",
                param->read.handle);

            uint16_t max_offset = server.getMTU() - 1;
            ESP_LOGD(TAG, "mtu value: %d", max_offset);
            if (param->read.need_rsp) {
                ESP_LOGD(TAG,
                    "Sending a response (esp_ble_gatts_send_response)");
                esp_gatt_rsp_t rsp;

                if (param->read.is_long) {
                    std::vector<std::uint8_t>& value = m_value.get();

                    if ((value.size() - m_value.getReadOffset()) < max_offset) {
                        rsp.attr_value.len = value.size() - m_value.getReadOffset();
                        rsp.attr_value.offset = m_value.getReadOffset();
                        std::memcpy(rsp.attr_value.value,
                            value.data() + rsp.attr_value.offset,
                            rsp.attr_value.len);
                        m_value.setReadOffset(0);
                    } else {
                        rsp.attr_value.len = max_offset;
                        rsp.attr_value.offset = m_value.getReadOffset();
                        memcpy(rsp.attr_value.value,
                            value.data() + rsp.attr_value.offset,
                            rsp.attr_value.len);
                        m_value.setReadOffset(rsp.attr_value.offset + max_offset);
                    }
                } else {
                    m_char_cb->onRead(*this);

                    std::vector<std::uint8_t>& value = m_value.get();
                    if (value.size() + 1 > max_offset) {
                        m_value.setReadOffset(max_offset);
                        rsp.attr_value.len = max_offset;
                        rsp.attr_value.offset = 0;
                        std::memcpy(rsp.attr_value.value,
                            value.data(),
                            rsp.attr_value.len);
                    } else {
                        rsp.attr_value.len = value.size();
                        rsp.attr_value.offset = 0;
                        std::memcpy(rsp.attr_value.value,
                            value.data(),
                            rsp.attr_value.len);
                    }
                }
                rsp.attr_value.handle = param->read.handle;
                rsp.attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;

                esp_err_t err = esp_ble_gatts_send_response(
                    gatts_if, param->read.conn_id,
                    param->read.trans_id,
                    ESP_GATT_OK,
                    &rsp);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG,
                        "esp_ble_gatts_send_response: err = %s",
                        esp_err_to_name(err));
                }
            }
        }
        break;
    }
    case ESP_GATTS_WRITE_EVT: {
        if (param->write.handle == m_handle) {
            ESP_LOGI(TAG,
                "Characteristic Write Event, handle = %d",
                param->write.handle);

            if (param->write.is_prep) {
                m_value.addPart(param->write.value, param->write.len);
                m_write_evt = true;
            } else {
                m_value.set(param->write.value, param->write.len);
                m_char_cb->onWrite(*this);
            }

            ESP_LOGD(TAG, "Response to write event");

            if (param->write.need_rsp) {
                esp_gatt_rsp_t rsp;

                rsp.attr_value.len = param->write.len;
                rsp.attr_value.handle = m_handle;
                rsp.attr_value.offset = param->write.offset;
                rsp.attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
                std::memcpy(rsp.attr_value.value,
                    param->write.value,
                    param->write.len);

                esp_err_t err = esp_ble_gatts_send_response(
                    gatts_if,
                    param->write.conn_id,
                    param->write.trans_id,
                    ESP_GATT_OK,
                    &rsp);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG,
                        "esp_ble_gatts_send_response: rc = %s",
                        esp_err_to_name(err));
                }
            }
        }
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT: {
        ESP_LOGD(TAG, "Characteristic Execute Write");
        if (m_write_evt) {
            m_write_evt = false;
            if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC) {
                m_value.commit();
                m_char_cb->onWrite(*this);
            } else {
                m_value.cancel();
            }

            esp_err_t err = esp_ble_gatts_send_response(gatts_if,
                param->write.conn_id,
                param->write.trans_id,
                ESP_GATT_OK,
                nullptr);

            if (err != ESP_OK) {
                ESP_LOGE(TAG,
                    "esp_ble_gatts_send_response: rc = %s",
                    esp_err_to_name(err));
            }
        }
        break;
    }
    default: {
        break;
    }
    }
} // namespace ble

} // namespace ble