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

#include <esp_err.h>
#include "WiFi.h"

class Vaina
{
private:
    Vaina();
    /* data */
public:
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
};


#endif // end vaina
