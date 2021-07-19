/*
 * Copyright (C) 2021 btcven and Locha Mesh developers <contact@locha.io>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_turpial_esp32
 * @brief       Board specific definitions for the ESP32 of the Locha Mesh Turpial.
 * @{
 *
 * @file
 * @author      Locha Mesh developers <contact@locha.io>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

/* include common board definitions as last step */
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the board specific hardware
 */
static inline void board_init(void) {
    /* there is nothing special to initialize on this board */
    board_init_common();
}

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H */
/** @} */
