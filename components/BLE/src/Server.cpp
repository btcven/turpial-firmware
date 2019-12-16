/**
 * @file Server.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-21
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#include "Server.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <esp_bt.h>
#include <esp_bt_defs.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gatt_common_api.h>
#include <esp_gatts_api.h>
#include <esp_log.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>

namespace ble {

static const char* TAG = "BLE_Server";

esp_err_t Server::init(ServerParams server_params)
{
    if (m_initialized) {
        ESP_LOGE(TAG, "BLE Server is already initialized");
        return ESP_FAIL;
    }

    m_server_params = server_params;

    esp_err_t err;

    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    err = esp_bt_controller_init(&bt_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(err));
        return err;
    }

    err = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(err));
        return err;
    }

    err = esp_bluedroid_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(err));
        return err;
    }

    err = esp_bluedroid_enable();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(err));
        return err;
    }

    // Bluetooth controller has been initialized
    m_initialized = true;

    err = esp_ble_gatts_register_callback(Server::handleGattsEvent);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "gatts register error, error code = %x", err);
        return err;
    }

    err = esp_ble_gap_register_callback(Server::handleGapEvent);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "gap register error, error code = %x", err);
        return err;
    }

    esp_err_t local_mtu_err = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_err != ESP_OK) {
        ESP_LOGE(TAG, "set local  MTU failed, error code = %x", local_mtu_err);
    }

    // Bonding with peer device after authentication
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;

    // Set the IO capability to only output (to force client use Passkey auth)
    esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;

    std::uint8_t key_size = 16;
    std::uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    std::uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;

    // Set static Passkey
    std::uint32_t passkey = m_server_params.static_passkey;
    std::uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    std::uint8_t oob_support = ESP_BLE_OOB_DISABLE;

    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY,
        &passkey,
        sizeof(std::uint32_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE,
        &auth_req,
        sizeof(std::uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE,
        &iocap,
        sizeof(std::uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE,
        &key_size,
        sizeof(std::uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH,
        &auth_option,
        sizeof(std::uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT,
        &oob_support,
        sizeof(std::uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY,
        &init_key,
        sizeof(std::uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY,
        &rsp_key,
        sizeof(std::uint8_t));

    return ESP_OK;
}

void Server::stop()
{
    if (!m_initialized) return;

    m_advertising.stop();
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();

    m_services.clear();
    m_services.shrink_to_fit();

    m_initialized = false;
}

void Server::registerApp()
{
    m_register_sema.take();
    esp_err_t err = esp_ble_gatts_app_register(m_server_params.app_id);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "gatts app register error, error code = %x", err);
        return;
    }
    m_register_sema.wait();
}

void Server::createService(Service&& service)
{
    m_services.push_back(std::move(service));

    m_srvc_create_sema.take();
    m_services.back().create();
    m_srvc_create_sema.wait();

    m_srvc_start_sema.take();
    m_services.back().start();
    m_srvc_start_sema.wait();
}

void Server::handleGapEvent(esp_gap_ble_cb_event_t event,
    esp_ble_gap_cb_param_t* param)
{
    ble::Server& server = ble::Server::getInstance();
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        ESP_LOGD(TAG, "ADV_DATA_RAW_SET_COMPLETE_EVT");
        if (server.m_advertising.state().is_config_state()) {
            server.m_advertising.start();
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
        ESP_LOGD(TAG, "SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT");
        if (server.m_advertising.state().is_scan_rsp_config_state()) {
            server.m_advertising.start();
        }
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Advertising start failed\n");
        }
        break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Advertising stop failed\n");
        } else {
            ESP_LOGI(TAG, "Stop adv successfully\n");
        }
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
            param->update_conn_params.status,
            param->update_conn_params.min_int,
            param->update_conn_params.max_int,
            param->update_conn_params.conn_int,
            param->update_conn_params.latency,
            param->update_conn_params.timeout);
        break;
    default:
        break;
    }
}

void Server::handleGattsEvent(esp_gatts_cb_event_t event,
    esp_gatt_if_t gatts_if,
    esp_ble_gatts_cb_param_t* param)
{
    ble::Server& server = ble::Server::getInstance();

    ESP_LOGD(TAG, "handleGattsEvent, event = %d", event);

    switch (event) {
    case ESP_GATTS_REG_EVT: {
        if (event == ESP_GATTS_REG_EVT) {
            ESP_LOGI(TAG,
                "ESP_GATTS_REG_EVT app_id %d\n",
                param->reg.app_id);

            if (param->reg.status == ESP_GATT_OK) {
                server.setGattsIf(gatts_if);
            } else {
                ESP_LOGE(TAG, "Reg app failed, status %d\n",
                    param->reg.status);
                return;
            }

            // Release the app register semaphore so the program execution
            // continues where registerApp was called
            server.m_register_sema.give();
        }

        esp_err_t err =
            esp_ble_gap_set_device_name(server.m_server_params.device_name);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Couldn't set the local device name");
        }

        // Start GAP advertising and let the world know the device is out there
        ble::AdvertisementData adv_data;
        adv_data.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
        server.m_advertising.setData(adv_data);

        ble::AdvertisementData scan_rsp_data;
        scan_rsp_data.setName(server.m_server_params.device_name);
        server.m_advertising.setScanResponseData(scan_rsp_data);
        break;
    }
    case ESP_GATTS_MTU_EVT: {
        ESP_LOGI(TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        server.m_mtu = param->mtu.mtu;
        break;
    }
    case ESP_GATTS_CONNECT_EVT: {
        ESP_LOGI(TAG, "ESP_GATTS_CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
            param->connect.conn_id,
            param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
            param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        if (server.m_conn_id != 0) {
            ESP_LOGI(TAG, "Another device tried to connect");
            break;
        }

        // Encrypt the connection, requires the client to enter the Passkey
        esp_ble_set_encryption(param->connect.remote_bda,
            ESP_BLE_SEC_ENCRYPT_MITM);

        // Save the connection ID
        server.m_conn_id = param->connect.conn_id;

        // Stop advertising
        server.m_advertising.stop();

        esp_ble_conn_update_params_t conn_params;
        std::memcpy(conn_params.bda,
            param->connect.remote_bda,
            sizeof(esp_bd_addr_t));

        conn_params.latency = 0;
        conn_params.max_int = 0x18; // 15 ms
        conn_params.min_int = 0x18; // 15 ms
        conn_params.timeout = 400;  // 4000 ms

        // Send the update connection parameters to the peer device.
        esp_ble_gap_update_conn_params(&conn_params);
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT: {
        ESP_LOGI(TAG, "ESP_GATTS_DISCONNECT_EVT");

        // Start adversiting again
        server.m_advertising.start();

        server.m_conn_id = 0;
        break;
    }
    case ESP_GATTS_CREATE_EVT: {
        ESP_LOGI(TAG, "Service Event, status %d, service_handle %d\n",
            param->create.status, param->create.service_handle);
        server.m_services.back().setHandle(param->create.service_handle);
        server.m_srvc_create_sema.give();
        break;
    }
    case ESP_GATTS_START_EVT: {
        if (param->start.service_handle == server.m_services.back().getHandle()) {
            ESP_LOGI(TAG, "Start Service Event, status %d, service_handle %d\n",
                param->start.status, param->start.service_handle);

            server.m_srvc_start_sema.give();
        }
        break;
    }
    default:
        break;
    }

    for (auto& service : server.m_services) {
        service.handleEvent(event, gatts_if, param);
    }
}

} // namespace ble
