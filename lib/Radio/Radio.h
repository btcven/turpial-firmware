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

#if RAD_ENABLED == true

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
     * @brief Start the Radio task communication
     * 
     * @param data 
     */
    void run(void* data) override;
};

#endif

} // namespace radio

#endif // RADIO_H