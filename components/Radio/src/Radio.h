/**
 * @file Radio.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-12-08
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#ifndef RADIO_H
#define RADIO_H

#include "Task.h"
#include "defaults.h"

namespace radio {
/**
 * @brief this type of data represent the callback to send data to websocket server
 * 
 */

typedef void (*radio_rx_cb_t)(void*,void*);

/**
 * @brief Radio communication task
 * 
 */
class Radio : public Task
{
public:
    /**
     * @brief Construct a new Radio
     * 
     */
    Radio();
    /**
     * @brief This function need to receive the callback to return the data from UART interface
     * the user of this module will need to provide a callback function
     *in order to receive the data from serial port
     * 
     * @param fn callback function to receive data out of this class
     * void myCallback(void* buffer, void* size)
     * {
     *  //some implementation
     * }
     */
    void init(radio_rx_cb_t fn);
    void sendDataToRadio(void* data, size_t length);
    radio_rx_cb_t forward_data_callback;

    /**
     * @brief Start the Radio task communication
     * 
     * @param data 
     */
    void run(void* data) override;
};

#endif

} // namespace radio
