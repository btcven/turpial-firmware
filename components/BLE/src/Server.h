/**
 * @file Server.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-21
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#include <cstdint>
#include <vector>

#include <esp_gatts_api.h>

#include "Advertising.h"
#include "Sema.h"
#include "Service.h"

namespace ble {

/**
 * @brief Server configuration parameters
 * 
 */
struct ServerParams {
    /**
     * @brief Construct a new ServerParams with default parameters
     * 
     */
    ServerParams() : device_name(nullptr), static_passkey(123456), app_id(0) {}

    /// The device name. Null terminated string.
    const char* device_name;
    /// Static Passkey (used to authenticate and pair BLE clients).
    std::uint32_t static_passkey;
    /// App ID that's going to be registered
    std::uint16_t app_id;
};

/**
 * @brief Representation of a BLE GATT server
 * 
 */
class Server
{
public:
    Server(Server const&) = delete;
    Server(Server&&) = delete;

    Server& operator=(Server const&) = delete;
    Server& operator=(Server&&) = delete;

    /**
     * @brief Get the BLE Server unique instance
     * 
     * @return Server& a reference to the Server
     */
    static Server& getInstance()
    {
        // Instatiated only once, won't be created again
        static Server g_instance;
        return g_instance;
    }

    /**
     * @brief Initialize the BLE server
     * 
     * @param server_params: Server configuration parameters
     * 
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t init(ServerParams server_params);

    /**
     * @brief Stop the BLE Server
     * 
     */
    void stop();

    /**
     * @brief Register BLE Server Profile application
     * 
     */
    void registerApp();

    /**
     * @brief Create a BLE service
     * 
     * @param service: the service to be created and added to the Server
     */
    void createService(Service&& service);

    /**
     * @brief Get the GATT server interface
     * 
     * @return esp_gatt_if_t interface
     */
    esp_gatt_if_t getGattsIf() { return m_gatts_if; }

    /**
     * @brief Get the Maximum Transfer Unit
     * 
     * @return std::uint16_t MTU value
     */
    std::uint16_t getMTU() { return m_mtu; }

private:
    /**
     * @brief Private constructor of Server
     * 
     */
    Server()
        : m_initialized(false),
          m_advertising(),
          m_server_params(),
          m_services(),
          m_gatts_if(0),
          m_register_sema("RegisterApp"),
          m_srvc_create_sema("ServiceCreate"),
          m_srvc_start_sema("ServiceStart"),
          m_conn_id(0),
          m_mtu(0)
    {
    }

    /**
     * @brief Set the GATT sever interface
     * 
     * @param gatts_if: interface
     */
    void setGattsIf(esp_gatt_if_t gatts_if) { m_gatts_if = gatts_if; }

    /**
     * @brief handle BLE GAP event
     * 
     * @param event: GAP event
     * @param param: GAP parameter
     */
    static void handleGapEvent(esp_gap_ble_cb_event_t event,
        esp_ble_gap_cb_param_t* param);

    /**
     * @brief handle GATT event
     * 
     * @param event: GATT event
     * @param gatts_if: GATT server interface
     * @param param: event parameters
     */
    static void handleGattsEvent(esp_gatts_cb_event_t event,
        esp_gatt_if_t gatts_if,
        esp_ble_gatts_cb_param_t* param);

    bool m_initialized; /**< Whether the BLE controller is initialized or not */

    Advertising m_advertising;       /**< Advertisement manager  */
    ServerParams m_server_params;    /**< Server parameters */
    std::vector<Service> m_services; /**< Server list of services */
    esp_gatt_if_t m_gatts_if;        /**< GATT server interface */

    util::Semaphore m_register_sema;    /**< App registration semaphore */
    util::Semaphore m_srvc_create_sema; /**< Service creation semaphore*/
    util::Semaphore m_srvc_start_sema;  /**< Service start semaphore*/

    std::uint16_t m_conn_id; /**< BLE connection id with client */
    std::uint16_t m_mtu;     /**< Maximum Transfer Unit */
};

} // namespace ble
