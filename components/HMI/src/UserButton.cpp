#include "UserButton.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <esp_log.h>

namespace hmi {

static xQueueHandle gpio_evt_queue = nullptr;

static void IRAM_ATTR interruptIsrHandler(void* arg)
{
    gpio_num_t* button = static_cast<gpio_num_t*>(arg);
    // gpio_intr_disable(GPIO_NUM_21);
    xQueueSendFromISR(gpio_evt_queue, arg, NULL);
}


UserButton::UserButton()
{
}

void UserButton::init(gpio_num_t user_button,
    bool active,
    callbackFunction fnClick,
    callbackFunction fn2Click,
    callbackFunction fnLongClick)
{
    static Button* btn = new Button(user_button, true);

    btn->_gpio_btn = user_button;
    active ? btn->_buttonPressed = LOW : btn->_buttonPressed = HIGH;

    btn->attachClick(fnClick);
    btn->attachDoubleClick(fn2Click);
    btn->attachLongClick(fnLongClick);

    static Interrupt g_int_task;
    gpio_num_t USER_BUTTON = btn->_gpio_btn;
    gpio_num_t SYSOFF = GPIO_NUM_27;
    const int ESP_INTR_FLAG_DEFAULT = 0;

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1 << SYSOFF);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = (1ULL << btn->_gpio_btn); //GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    //change gpio intrrupt type
    gpio_set_intr_type(btn->_gpio_btn, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(btn->_gpio_btn, interruptIsrHandler, &btn->_gpio_btn);
    gpio_evt_queue = xQueueCreate(10, sizeof(gpio_num_t));

    g_int_task.start(btn);
}


Interrupt::Interrupt()
    : Task("Interrupt", 2000, 5)
{
}

void Interrupt::run(void* task_data)
{
    Button* btn = static_cast<Button*>(task_data);
    unsigned long time_now;
    if (btn->_doubleClickFunc != NULL) {
    }
    gpio_num_t io_pin = GPIO_NUM_MAX;
    while (true) {
        do {
            time_now = esp_timer_get_time();
            if (xQueueReceive(gpio_evt_queue, &io_pin, portMAX_DELAY)) {
                if (btn->_state == 6) //restart the loop
                    btn->reset();
            }

        } while (uxQueueMessagesWaiting(gpio_evt_queue));
        
        do {
            time_now = esp_timer_get_time();
            btn->tick(gpio_get_level(btn->_gpio_btn) == btn->_buttonPressed, time_now);
        } while ((btn->_state != 0 || btn->_state == 2 || btn->_state == 1)); // to ensure the state machine is able to detect long click and release the resource in right time
    }
}

} // namespace hmi