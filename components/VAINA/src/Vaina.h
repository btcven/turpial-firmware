/**
 * @file Vaina.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief VAINA - Versatile Address Interface | Network Administration
 * @version 0.1
 * @date 2020-04-23
 *
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 *
 */


#ifndef VAINA_H
#define VAINA_H

#include "WiFi.h"
#include <esp_err.h>
#include <iostream>


struct sta_data_t {
    esp_ip4_addr_t sta_ipv4;
    const char sta_mac[12];
    const char sta_ipv6;
};
struct mac_length_t {
    char mac[13];
};


class Vaina
{
private:
    Vaina();

    std::vector<sta_data_t> m_sta_connected;
    std::vector<esp_ip4_addr_t> m_ipv4_array;
    std::vector<mac_length_t> m_mac_array;
    /* data */
public:
    friend network::WiFi

    Vaina(Vaina const&) = delete; // Copy construct
    Vaina(Vaina&&) = delete;      // Move construct

    Vaina& operator=(Vaina const&) = delete; // Copy assign
    Vaina& operator=(Vaina&&) = delete;      // Move assign

    /**
     * @brief Get the unique instance of the object
     *
     * @return Vaina&
     */
    static Vaina& getInstance()
    {
        static Vaina instance;
        return instance;
    }

    esp_err_t staConnected(wifi_event_ap_staconnected_t* info);
    esp_err_t staDisconected(wifi_event_ap_stadisconnected_t* info);
    void setArrayIpv4(esp_ip4_addr_t ipv4);
    void ipv4ToIpv6(uint32_t* ipv4);
    void decToHexa(int n);
};


#endif // end vaina
