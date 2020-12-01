/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         boards_turpial
 * @{
 *
 * @file
 * @brief           Board specific definitions for Locha Mesh Turpial
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
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
 * @name    Front-End configuration
 * @{
 */
#define RF_HGM_PIN          GPIO_PIN(0, 28)
#define RF_LNA_EN_PIN       GPIO_PIN(0, 29)
#define RF_PA_EN_PIN        GPIO_PIN(0, 30)
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
