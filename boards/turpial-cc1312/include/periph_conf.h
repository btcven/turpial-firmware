/*
 * Copyright (C) 2021 Locha Mesh Developers
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         boards_turpial_cc1312
 * @{
 *
 * @file
 * @brief           Peripheral MCU configuration for Locha Mesh Turpial
 *                  (CC1312 MCU).
 *
 * @author          Locha Mesh Developers <contact@locha.io>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @name    Clock configuration
* @{
*/
/* the main clock is fixed to 48MHZ */
#define CLOCK_CORECLOCK     (48000000U)
/** @} */

/**
* @name    Timer configuration
*
* General purpose timers (GPT[0-3]) are configured consecutively and in order
* (without gaps) starting from GPT0, i.e. if multiple timers are enabled.
*
* @{
*/
static const timer_conf_t timer_config[] = {
 {
     .cfg = GPT_CFG_16T,
     .chn = 2,
 },
 {
     .cfg = GPT_CFG_32T,
     .chn = 1,
 },
 {
     .cfg = GPT_CFG_16T,
     .chn = 2,
 },
 {
     .cfg = GPT_CFG_32T,
     .chn = 1,
 }
};

#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 *
 * The used LAUNCHXL-CC1312R board has a single UART device available through
 * the debugger, so all we need to configure are the RX and TX pins.
 *
 * Optionally we can enable hardware flow control, by setting UART_HW_FLOW_CTRL
 * to 1 and defining pins for cts_pin and rts_pin.
 *
 * Add a second UART configuration if using external pins.
 * @{
 */

static const uart_conf_t uart_config[] = {
 {
     .regs = UART0,
     .tx_pin = 3,
     .rx_pin = 2,
#ifdef MODULE_PERIPH_UART_HW_FC
     /* we don't do that here :-) */
     .rts_pin = GPIO_UNDEF,
     .cts_pin = GPIO_UNDEF,
#endif
     .intn = UART0_IRQN
 },
 {
     .regs = UART1,
     .tx_pin = 12,
     .rx_pin = 13,
#ifdef MODULE_PERIPH_UART_HW_FC
     /* we don't do that here :-) */
     .rts_pin = 0,
     .cts_pin = 0,
#endif
     .intn = UART1_IRQN
  }
};
#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
#define I2C_NUMOF           (1)
#define I2C_SCL_PIN         (4)
#define I2C_SDA_PIN         (5)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
