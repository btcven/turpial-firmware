/*
 * Copyright (C)  2018 Gunar Schorcht
 * Copyright (C)  2021 btcven and Locha Mesh Developers <contact@locha.io>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_turpial_esp32
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino API
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Locha Mesh developers <contact@locha.io>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "arduino_board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The on-board LED is not available
 */
#define ARDUINO_LED         (0)

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */
