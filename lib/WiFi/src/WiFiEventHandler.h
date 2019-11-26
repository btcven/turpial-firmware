#ifndef WIFI_EVENT_HANDLER_H_
#define WIFI_EVENT_HANDLER_H_
#include <esp_event.h>
#include <esp_event_loop.h>


/**
 * @brief %WiFi state event handler.
 * 
 * WiFiEventHandler.h
 * 
 * A WiFiEventHandler defines a class that has methods that will be called back when a WiFi event is
 * detected.
 *
 * Typically this class is subclassed to provide implementations for the callbacks we want to handle.
 *
 * class MyHandler: public WiFiEventHandler {
 *   esp_err_t apStart() {
 *      ESP_LOGD(tag, "MyHandler(Class): apStart");
 *      return ESP_OK;
 *   }
 * }
 *
 * WiFiMode* wifi_mode = new WiFiMode();
 * MyHandler *event_handler = new MyHandler();
 * wifi_mode->setWifiEventHandler(event_handler);
 * wifi_mode->start();
 *
*/
namespace wifi {
class WiFiEventHandler
{
public:
    WiFiEventHandler();
    virtual ~WiFiEventHandler();
    virtual esp_err_t apStaConnected(system_event_ap_staconnected_t info);
    virtual esp_err_t apStaDisconnected(system_event_ap_stadisconnected_t info);
    virtual esp_err_t apStart();
    virtual esp_err_t apStop();
    system_event_cb_t getEventHandler();
    virtual esp_err_t staConnected(system_event_sta_connected_t info);
    virtual esp_err_t staDisconnected(system_event_sta_disconnected_t info);
    virtual esp_err_t staGotIp(system_event_sta_got_ip_t info);
    virtual esp_err_t staScanDone(system_event_sta_scan_done_t info);
    virtual esp_err_t staAuthChange(system_event_sta_authmode_change_t info);
    virtual esp_err_t staStart();
    virtual esp_err_t staStop();
    virtual esp_err_t wifiReady();

    /**
	 * Get the next WiFi event handler in the chain, if there is one.
	 * @return The next WiFi event handler in the chain or nullptr if there is none.
	 */
    WiFiEventHandler* getNextHandler()
    {
        return m_next_handler;
    }

    /**
	 * Set the next WiFi event handler in the chain.
	 * @param [in] nextHandler The next WiFi event handler in the chain.
	 */
    void setNextHandler(WiFiEventHandler* nextHandler)
    {
        this->m_next_handler = nextHandler;
    }

private:
    friend class WiFiMode;
    WiFiEventHandler* m_next_handler;
    static esp_err_t eventHandler(void* ctx, system_event_t* event);
};
} //namespace wifi
#endif /* WIFI_EVENT_HANDLER_H_ */