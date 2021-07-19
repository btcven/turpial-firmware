/*
 * Copyright (C) 2021 btcven and Locha Mesh Developers
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_turpial_esp32
 * @brief       Peripheral MCU configuration for the ESP32 of the Locha Mesh Turpial.
 * @{
 *
 * @file
 * @author      Locha Mesh Developers <contact@locha.iot>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC and DAC channel configuration
 * @{
 */

/**
 * @brief   Declaration of GPIOs that can be used as ADC channels
 *
 * When the camera is connected there are no GPIOs left that could be used
 * as ADC channels.
 *
 * @note As long as the GPIOs listed in ADC_GPIOS are not initialized as ADC
 * channels with the ```adc_init``` function, they can be used for other
 * purposes.
 */
#ifndef ADC_GPIOS
#define ADC_GPIOS   { GPIO34, GPIO35 }
#endif

/**
 * @brief   Declaration of GPIOs that can be used as DAC channels
 *
 * Turpial has no GPIOs left that might be used as DAC channels.
 */
#ifndef DAC_GPIOS
#define DAC_GPIOS   { }
#endif
/** @} */


/**
 * @name   I2C configuration
 *
 * @note The GPIOs listed in the configuration are only initialized as I2C
 * signals when module ```perpih_i2c``` is used. Otherwise they are not
 * allocated and can be used for other purposes.
 *
 * @{
 */
#ifndef I2C0_SPEED
#define I2C0_SPEED  I2C_SPEED_FAST  /**< I2C bus speed of I2C_DEV(0) */
#endif
#ifndef I2C0_SCL
#define I2C0_SCL    GPIO27          /**< SCL signal of I2C_DEV(0) [UEXT1] */
#endif
#ifndef I2C0_SDA
#define I2C0_SDA    GPIO26          /**< SDA signal of I2C_DEV(0) [UEXT1] */
#endif
/** @} */

/**
 * @name   PWM channel configuration
 *
 * LEDs are used as PWM channels for device PWM_DEV(0).
 *
 * @note As long as the according PWM device is not initialized with function
 * pwm_init, the GPIOs declared for this device can be used for other
 * purposes.
 *
 * @note As long as the according PWM device is not initialized with
 * the ```pwm_init```, the GPIOs declared for this device can be used
 * for other purposes.
 *
 * @{
 */
#ifndef PWM0_GPIOS
#define PWM0_GPIOS  { }
#endif

/** PWM_DEV(1) is not used */
#ifndef PWM1_GPIOS
#define PWM1_GPIOS  { }
#endif
/** @} */


/**
 * @name    SPI configuration
 *
 * SPI configuration depends on configured/connected components.
 *
 * HSPI is is always available and therefore used as SPI_DEV(0)
 * VSPI is only available when the camera is not plugged.
 *
 * @{
 */

#ifndef SPI0_CTRL
#define SPI0_CTRL   HSPI    /**< HSPI is configured as SPI_DEV(0) */
#endif

#ifndef SPI0_SCK
#define SPI0_SCK    GPIO14
#endif
#ifndef SPI0_MOSI
#define SPI0_MOSI   GPIO15
#endif
#ifndef SPI0_MISO
#define SPI0_MISO   GPIO2
#endif
#ifndef SPI0_CS0
#define SPI0_CS0    GPIO13  /**< SD Card CS */
#endif

/**
 * @name   UART configuration
 *
 * ESP32 provides 3 UART interfaces at maximum:
 *
 * UART_DEV(0) uses fixed standard configuration.<br>
 * UART_DEV(1) is connected to @ref board_turpial_cc1312 UART.<br>
 * UART_DEV(2) is not available.<br>
 * @{
 */
#define UART0_TXD   GPIO1   /**< direct I/O pin for UART_DEV(0) TxD, can't be changed */
#define UART0_RXD   GPIO3   /**< direct I/O pin for UART_DEV(0) RxD, can't be changed */

#define UART1_TXD   GPIO21  /**< direct I/O pin for UART_DEV(1) TxD, can't be changed */
#define UART1_RXD   GPIO22  /**< direct I/O pin for UART_DEV(1) RxD, can't be changed */
/** @} */


#ifdef __cplusplus
} /* end extern "C" */
#endif

/* include common board definitions as last step */
#include "periph_conf_common.h"

#endif /* PERIPH_CONF_H */
/** @} */
