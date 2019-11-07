/**
 * @file hardware.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#if defined(HELTEC_WIFI_LORA_32)
    #include "heltec_v2.h"
#elif defined(ARDUINO_ESP_FEATHER)
    #include "featheresp32.h"
#elif defined(ARDUINO_Pocket32)
    #include "esp32_wemos.h"
#endif

#endif // HARDWARE_H_