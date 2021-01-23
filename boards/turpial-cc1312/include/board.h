/*
 * Copyright (C) 2021 Locha Mesh Developers
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         boards_turpial_cc1312
 * @{
 *
 * @file
 * @brief           Board specific definitions for Locha Mesh Turpial
 *                  (CC1312 MCU).
 *
 * @author          Locha Mesh Developers <contact@locha.io>
 */

#ifndef BOARD_H
#define BOARD_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      (25)
#define XTIMER_ISR_BACKOFF  (20)
/** @} */

/**
 * @name    On-board button configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 13)
#define BTN0_MODE           GPIO_IN_PU

#define BTN1_PIN            GPIO_PIN(0, 14)
#define BTN1_MODE           GPIO_IN_PU
/** @} */

/**
 * @brief   On-board LED configuration and controlling
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 23)          /**< red   */
#define LED1_PIN            GPIO_PIN(0, 24)          /**< green */
#define LED2_PIN            GPIO_PIN(0, 25)          /**< blue */

#define LED0_ON             gpio_set(LED0_PIN)
#define LED0_OFF            gpio_clear(LED0_PIN)
#define LED0_TOGGLE         gpio_toggle(LED0_PIN)

#define LED1_ON             gpio_set(LED1_PIN)
#define LED1_OFF            gpio_clear(LED1_PIN)
#define LED1_TOGGLE         gpio_toggle(LED1_PIN)

#define LED2_ON             gpio_set(LED2_PIN)
#define LED2_OFF            gpio_clear(LED2_PIN)
#define LED2_TOGGLE         gpio_toggle(LED2_PIN)
/** @} */

/**
 * @brief   RF Front-End configuration
 * @{
 */
#define HGM_PIN             GPIO_PIN(0, 28)
#define LNA_EN_PIN          GPIO_PIN(0, 29)
#define PA_EN_PIN           GPIO_PIN(0, 30)

#define HGM_ON              gpio_set(HGM_PIN)
#define HGM_OFF             gpio_clear(HGM_PIN)
#define HGM_TOGGLE          gpio_toggle(HGM_PIN)
/** @} */

/**
 * @brief   BQ24298 charger IC configuration
 */
#define BQ2429X_PARAM_INT_PIN   GPIO_PIN(0, 6)
#define BQ2429X_PARAM_VLIM      BQ2429X_VLIM_4360
#define BQ2429X_PARAM_ILIM      BQ2429X_ILIM_500
#define BQ2429X_PARAM_ICHG      BQ2429X_ICHG_512
#define BQ2429X_PARAM_VREG      BQ2429X_VREG_4208
/** @} */

/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
