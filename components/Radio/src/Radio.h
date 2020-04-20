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
#define CONFIG_RADIO_RX_BUF_SIZE (257)
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
