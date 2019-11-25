/**
 * @file main.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <cstdio>
#include <sstream>

#include "esp_log.h"
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "NVS.h"
#include "WiFiMode.h"

#include "WiFiEventHandler.h"

#include "defaults.h"
#include "wifi_event_source.h"
static const char* TAG = "app_main";

esp_err_t getIsConfigured(bool& is_configured)
{
    esp_err_t err;

    storage::NVS app_nvs;
    err = app_nvs.open(NVS_APP_NAMESPACE, NVS_READWRITE);
    if (err != ESP_OK) {
        const char* err_str = esp_err_to_name(err);
        ESP_LOGE(TAG,
            "Couldn't open namespace \"%s\" (%s)",
            NVS_APP_NAMESPACE,
            err_str);
        return err;
    }

    err = app_nvs.get_bool(NVS_IS_CONFIGURED_KEY, is_configured);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // Set is_configured to true on flash so on next init the config is
        // readed directly by the ESP-IDF Wi-Fi library component.
        err = app_nvs.set_bool(NVS_IS_CONFIGURED_KEY, true);
        if (err != ESP_OK) return err;
        err = app_nvs.commit();
        if (err != ESP_OK) return err;
        // Set the return variable to "false" to forcibly set the default
        // configuration
        is_configured = false;
    } else {
        return err;
    }

    return ESP_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



static char* get_id_string(esp_event_base_t base, int32_t id) {
    char* event = "";
    if (base == TIMER_EVENTS) {
        switch(id) {
            case TIMER_EVENT_STARTED:
            event = "TIMER_EVENT_STARTED";
            break;
            case TIMER_EVENT_EXPIRY:
            event = "TIMER_EVENT_EXPIRY";
            break;
            case TIMER_EVENT_STOPPED:
            event = "TIMER_EVENT_STOPPED";
            break;
        }
    }  else if (base == WIFI_EVENTS){
        switch(id) {
            case STA_AUTHMODE_CHANGE_EVENT:
            event = "SYSTEM_EVENT_STA_AUTHMODE_CHANGE";
            break;
            case STA_CONNECTED_EVENT:
            event = "SYSTEM_EVENT_STA_CONNECTED";
            break;
            case STA_DISCONNECTED_EVENT:
            event = "SYSTEM_EVENT_STA_DISCONNECTED";
            break;

            case STA_GOT_IP_EVENT:
            event = "SYSTEM_EVENT_STA_GOT_IP";
            break;
            case STA_START_EVENT:
            event = "SYSTEM_EVENT_STA_START";
            break;
            case STA_STOP_EVENT:
            event = "SYSTEM_EVENT_STA_STOP";
            break;
            case WIFI_READY_EVENT:
            event = "SYSTEM_EVENT_WIFI_READY";
            break;    
        }

    } else {
        event = "TASK_ITERATION_EVENT";
    }
    return event;
}




/* Event source periodic timer related definitions */
ESP_EVENT_DEFINE_BASE(TIMER_EVENTS);
ESP_EVENT_DEFINE_BASE(WIFI_EVENTS);
esp_timer_handle_t TIMER;

// Callback that will be executed when the timer period lapses. Posts the timer expiry event
// to the default event loop.
static void timer_callback(void* arg)
{
    ESP_LOGI(TAG, "%s:%s: posting to default loop", TIMER_EVENTS, get_id_string(TIMER_EVENTS, TIMER_EVENT_EXPIRY));
    ESP_ERROR_CHECK(esp_event_post(TIMER_EVENTS, TIMER_EVENT_EXPIRY, NULL, 0, portMAX_DELAY));
}

// Handler which executes when the timer started event gets executed by the loop.
static void timer_started_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "%s:%s: timer_started_handler", base, get_id_string(base, id));
}

// Handler which executes when the timer expiry event gets executed by the loop. This handler keeps track of
// how many times the timer expired. When a set number of expiry is reached, the handler stops the timer
// and sends a timer stopped event.
static void timer_expiry_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    static int count = 0;

    count++;

    if (count >= TIMER_EXPIRIES_COUNT) {
        // Stop the timer
        ESP_ERROR_CHECK(esp_timer_stop(TIMER));

        ESP_LOGI(TAG, "%s:%s: posting to default loop", base, get_id_string(base, TIMER_EVENT_STOPPED));

        // Post the event that the timer has been stopped
        ESP_ERROR_CHECK(esp_event_post(TIMER_EVENTS, TIMER_EVENT_STOPPED, NULL, 0, portMAX_DELAY));
    }

    ESP_LOGI(TAG, "%s:%s: timer_expiry_handler, executed %d out of %d times", base, get_id_string(base, id), count, TIMER_EXPIRIES_COUNT);
}

// Handler which executes when any timer event (started, expiry, stopped) get executed by the loop
static void timer_any_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "%s:%s: timer_any_handler", base, get_id_string(base, id));
}

/* static void wifi_any_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "%s:%s: timer_any_handler", base, get_id_string(base, id));
    ESP_LOGI(TAG, "?????????????????????????????????????????????-------------------------------------.>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>: timer_any_handler");
} */

// Handler which executes when the timer stopped event gets executed by the loop. Since the timer has been
// stopped, it is safe to delete it.
static void timer_stopped_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "%s:%s: timer_stopped_handler", base, get_id_string(base, id));

    // Delete the timer
    esp_timer_delete(TIMER);

    ESP_LOGI(TAG, "%s:%s: deleted timer event source", base, get_id_string(base, id));
}

/* Event source task related definitions */
ESP_EVENT_DEFINE_BASE(TASK_EVENTS);

static void task_iteration_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    int iteration = *((int*) event_data);
    ESP_LOGI(TAG, "%s:%s: task_iteration_handler, executed %d times", base, get_id_string(base, id), iteration);
}

static void task_event_source(void* args)
{
    for(int iteration = 1; iteration <= TASK_ITERATIONS_COUNT; iteration++) {

        ESP_LOGI(TAG, "%s:%s: posting to default loop, %d out of %d", TASK_EVENTS,
                get_id_string(TASK_EVENTS, TASK_ITERATION_EVENT), iteration, TASK_ITERATIONS_COUNT);

        // Post that the loop has iterated. Notice that the iteration count is passed to the handler. Take note
        // that data passed during event posting is a deep copy of the original data.
        ESP_ERROR_CHECK(esp_event_post(TASK_EVENTS, TASK_ITERATION_EVENT, &iteration, sizeof(iteration), portMAX_DELAY));

        if (iteration == TASK_ITERATIONS_UNREGISTER) {
            ESP_LOGI(TAG, "%s:%s: unregistering task_iteration_handler", TASK_EVENTS, get_id_string(TASK_EVENTS, TASK_ITERATION_EVENT));
            ESP_ERROR_CHECK(esp_event_handler_unregister(TASK_EVENTS, TASK_ITERATION_EVENT, task_iteration_handler));
        }

        vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD));
    }

    vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD));

    ESP_LOGI(TAG, "%s:%s: deleting task event source", TASK_EVENTS, get_id_string(TASK_EVENTS, TASK_ITERATION_EVENT));

    vTaskDelete(NULL);
}

/* Handler for all events */
static void all_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "%s:%s: all_event_handler", base, get_id_string(base, id));
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



extern "C" void app_main()
{
    esp_err_t err;
    wifi::WiFiEventHandler* event_handler;
    event_handler = new wifi::WiFiEventHandler();
    wifi::WiFiMode* wifi_mode;
    wifi_mode = new wifi::WiFiMode();


    bool is_nvs_initialized = true;
    err = storage::init();
    if (err != ESP_OK) {
        const char* err_name = esp_err_to_name(err);
        ESP_LOGE(TAG, "Couldn't initialize NVS, error (%s)", err_name);
        is_nvs_initialized = false;
    }

    ESP_LOGD(TAG, "Init TCP/IP adapter");
    tcpip_adapter_init();

    bool is_configured = false;
    if (is_nvs_initialized) {
        err = getIsConfigured(is_configured);
        if (err != ESP_OK) {
            const char* err_str = esp_err_to_name(err);
            ESP_LOGE(TAG,
                "Couldn't get \"is_configured\" value (%s)",
                err_str);
        }
    }

    err = wifi_mode->init(is_nvs_initialized);
    if (err != ESP_OK) {
        const char* err_name = esp_err_to_name(err);
        ESP_LOGE(TAG, "Couldn't initalize Wi-Fi interface (%s)", err_name);
        // TODO: fallback to bluetooth mode to configure Wi-Fi?
        return;
    }

    if (!is_configured) {
        wifi_mode->set_mode(WIFI_MODE);

        wifi::APConfig ap_config = {
            .ssid = WAP_SSID,
            .password = WAP_PASS,
            .authmode = WAP_AUTHMODE,
            .max_conn = WAP_MAXCONN,
            .channel = WAP_CHANNEL,
        };
        wifi_mode->set_ap_config(ap_config);

        wifi::STAConfig sta_config = {
            .ssid = WST_SSID,
            .password = WST_PASS,
        };
        wifi_mode->set_sta_config(sta_config);
    }
/////////////////////////////////////////////////////////////////////////////////////
ESP_LOGI(TAG, "setting up");

    // Create the default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Register the specific timer event handlers.
    ESP_ERROR_CHECK(esp_event_handler_register(TIMER_EVENTS, TIMER_EVENT_STARTED, timer_started_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(TIMER_EVENTS, TIMER_EVENT_EXPIRY, timer_expiry_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(TIMER_EVENTS, TIMER_EVENT_STOPPED, timer_stopped_handler, NULL));

    // Register the handler for all timer family events. This will execute if the timer is started, expired or is stopped.
    ESP_ERROR_CHECK(esp_event_handler_register(TIMER_EVENTS, ESP_EVENT_ANY_ID, timer_any_handler, NULL));

    // Register the handler for task iteration event.
    ESP_ERROR_CHECK(esp_event_handler_register(TASK_EVENTS, TASK_ITERATION_EVENT, task_iteration_handler, NULL));

    // Register the handler for all event. This will execute if either the timer events or the task iteration event
    // is posted to the default loop.
    ESP_ERROR_CHECK(esp_event_handler_register(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, all_event_handler, NULL));

    // Create and start the event sources
    esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &TIMER));

    ESP_LOGI(TAG, "starting event sources");

     // Create the event source task with the same priority as the current task
    xTaskCreate(task_event_source, "task_event_source", 2048, NULL, uxTaskPriorityGet(NULL), NULL);

    ESP_ERROR_CHECK(esp_timer_start_periodic(TIMER, TIMER_PERIOD));

    // Post the timer started event
    ESP_LOGI(TAG, "%s:%s: posting to default loop", TIMER_EVENTS, get_id_string(TIMER_EVENTS, TIMER_EVENT_STARTED));
    ESP_ERROR_CHECK(esp_event_post(TIMER_EVENTS, TIMER_EVENT_STARTED, NULL, 0, portMAX_DELAY));




//////////////////////////////////////////////////////////////////////////////////
    err = wifi_mode->start();
    // TODO: app loop
}