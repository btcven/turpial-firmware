/**
 * Copyright 2020 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** 
 * @file Radio.h
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */

#ifndef RADIO_H
#define RADIO_H

#include <cstdint>

#include <esp_err.h>

namespace radio {

#ifndef CONFIG_RADIO_UART
/**
 * @brief   UART device to use
 */
#define CONFIG_RADIO_UART UART_NUM_2
#endif

#ifndef CONFIG_RADIO_TX
/**
 * @brief   UART TX pin
 */
#define CONFIG_RADIO_TX 5
#endif

#ifndef CONFIG_RADIO_RX
/**
 * @brief   UART RX pin
 */
#define CONFIG_RADIO_RX 4
#endif

#ifndef CONFIG_RADIO_RX_BUF_SIZE
/**
 * @brief   RX buffer size
 */
#define CONFIG_RADIO_RX_BUF_SIZE (512)
#endif

/**
 * @brief   Write data to the radio
 *
 * @param[in] buffer Buffer to write.
 * @param[in] length Length of the buffer.
 *
 * @return -1 on error.
 * @return >0 number of bytes written.
 */
int write(const std::uint8_t *buffer, std::size_t length);

/**
 * @brief   Initialize radio
 *
 * @param[in] rx_cb Callback that will be called on data reception.
 */
esp_err_t init();

} // namespace radio

#endif // RADIO_H
