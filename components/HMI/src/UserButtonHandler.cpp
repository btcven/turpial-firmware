#include "UserButtonHandler.h"

#include <esp_log.h>


#define LED_R DEFAULT_RGB_LED_R 
#define LED_G DEFAULT_RGB_LED_G
#define LED_B DEFAULT_RGB_LED_B

#define RGB_GPIO_OUTPUT_PIN_SEL ( ( 1ULL<<LED_R ) | ( 1ULL<<LED_G ) | ( 1ULL<<LED_B ) )

static const char* TAG = "USER-BUTTON-HANDLER";

void UserButtonHandler::initRGBPort(void)
{
    //disable interrupt
    io_conf.intr_type = static_cast<gpio_int_type_t>(GPIO_PIN_INTR_DISABLE);
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO0/2/4
    io_conf.pin_bit_mask = RGB_GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = static_cast<gpio_pulldown_t>(0);
    //disable pull-up mode
    io_conf.pull_up_en = static_cast<gpio_pullup_t>(0);
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

void UserButtonHandler::click(void)
{
    printf(">>>>>>>>>>>>>>>> click event-----callback called------------>>>>\n");
}

void UserButtonHandler::doubleClick(void)
{
    printf(">>>>>>>>>>>>>>>> DOUBLEclick event----callback called------------->>>>\n");
}

void UserButtonHandler::longClick(void)
{
     printf(">>>>>>>>>>>>>>>> LONG CLICK EVENT-----callback called----------->>>>\n");
}
