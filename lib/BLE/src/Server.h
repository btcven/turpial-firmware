/**
 * @file Server.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <cstdint>
#include <vector>

#include "esp_gatts_api.h"

#include "Advertising.h"
#include "Sema.h"
#include "Service.h"

namespace ble {

struct ServerParams {
    ServerParams() : device_name(nullptr), static_passkey(123456), app_id(0) {}

    const char* device_name;
    std::uint32_t static_passkey;
    std::uint16_t app_id;
};

class Server
{
public:
    Server(Server const&) = delete;
    Server(Server&&) = delete;

    Server& operator=(Server const&) = delete;
    Server& operator=(Server&&) = delete;

    static Server& getInstance()
    {
        // Instatiated only once, won't be created again
        static Server g_instance;
        return g_instance;
    }

    esp_err_t init(ServerParams server_params);

    void registerApp();

    void createService(Service&& service);

    Advertising& advertising() { return m_advertising; }
    const ServerParams& params() const { return m_server_params; }

    void setGattsIf(esp_gatt_if_t gatts_if) { m_gatts_if = gatts_if; }
    esp_gatt_if_t getGattsIf() { return m_gatts_if; }

    std::uint16_t getMTU() { return m_mtu; }

private:
    Server()
        : m_advertising(),
          m_server_params(),
          m_services(),
          m_gatts_if(0),
          m_register_sema("RegisterApp"),
          m_conn_id(0),
          m_mtu(0)
    {
    }

    static void handleGapEvent(esp_gap_ble_cb_event_t event,
        esp_ble_gap_cb_param_t* param);

    static void handleGattsEvent(esp_gatts_cb_event_t event,
        esp_gatt_if_t gatts_if,
        esp_ble_gatts_cb_param_t* param);

    Advertising m_advertising;
    ServerParams m_server_params;
    std::vector<Service> m_services;
    esp_gatt_if_t m_gatts_if;

    util::Semaphore m_register_sema;

    std::uint16_t m_conn_id;
    std::uint16_t m_mtu;
};

} // namespace ble