/* #ifndef EVENT_SOURCE_H_
#define EVENT_SOURCE_H_

#include "esp_event.h"
#include "esp_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

ESP_EVENT_DECLARE_BASE(WIFI_EVENTS);         // declaration of the task events family

enum {
    // raised during an iteration of the loop within the task
    STA_AUTHMODE_CHANGE_EVENT,
    STA_CONNECTED_EVENT,
    STA_DISCONNECTED_EVENT,
    STA_GOT_IP_EVENT,
    STA_START_EVENT,
    STA_STOP_EVENT,
    STOP_EVENT,
    WIFI_READY_EVENT,
    AP_STOP_EVENT,
    AP_STACONNECTED_EVENT,
    AP_STADISCONNECTED_EVENT,
    SCAN_DONE_EVENT
};

#ifdef __cplusplus
}
#endif

#endif // #ifndef EVENT_SOURCE_H_ */









#ifndef WIFI_EVENT_SOURCE_H_
#define WIFI_EVENT_SOURCE_H_

#include "esp_timer.h"

#include "esp_err.h"
#include "esp_event.h"
#include "esp_wifi.h"


#ifdef __cplusplus
extern "C" {
#endif

// This example makes use of two event sources: a periodic timer, and a task.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declarations for event source 1: periodic timer
#define TIMER_EXPIRIES_COUNT        3        // number of times the periodic timer expires before being stopped
#define TIMER_PERIOD                1000000  // period of the timer event source in microseconds

extern esp_timer_handle_t g_timer;           // the periodic timer object

// Declare an event base
ESP_EVENT_DECLARE_BASE(TIMER_EVENTS);        // declaration of the timer events family

enum {                                       // declaration of the specific events under the timer event family
    TIMER_EVENT_STARTED,                     // raised when the timer is first started
    TIMER_EVENT_EXPIRY,                      // raised when a period of the timer has elapsed
    TIMER_EVENT_STOPPED                      // raised when the timer has been stopped
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Declarations for event source 2: task
#define TASK_ITERATIONS_COUNT        5       // number of times the task iterates
#define TASK_ITERATIONS_UNREGISTER   3       // count at which the task event handler is unregistered
#define TASK_PERIOD                  500     // period of the task loop in milliseconds

ESP_EVENT_DECLARE_BASE(TASK_EVENTS);         // declaration of the task events family

enum {
    TASK_ITERATION_EVENT,                    // raised during an iteration of the loop within the task
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declarations for event source 2: task
/* #define TASK_ITERATIONS_COUNT        5       // number of times the task iterates
#define TASK_ITERATIONS_UNREGISTER   3       // count at which the task event handler is unregistered
#define TASK_PERIOD                  500     // period of the task loop in milliseconds
 */
ESP_EVENT_DECLARE_BASE(WIFI_EVENTS);         // declaration of the task events family

enum {
    // raised during an iteration of the loop within the task
    STA_AUTHMODE_CHANGE_EVENT,
    STA_CONNECTED_EVENT,
    STA_DISCONNECTED_EVENT,
    STA_GOT_IP_EVENT,
    STA_START_EVENT,
    STA_STOP_EVENT,
    WIFI_READY_EVENT,
};
#ifdef __cplusplus
}
#endif

#endif