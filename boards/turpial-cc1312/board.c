/*
 * Copyright (C) 2020 Locha Mesh Developers
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
 * @brief           Board specific implementations for Locha Mesh Turpial
 *                  (CC1312 MCU).
 *
 * @author          Locha Mesh Developers <contact@locha.io>
 */

#include "cpu.h"
#include "board.h"

/**
 * @brief    Initialize the board.
 */
void board_init(void)
{
    cpu_init();

    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);

    if (IS_USED(MODULE_CC26X2_CC13X2_RF)) {
        IOC->CFG[LNA_EN_PIN] = IOCFG_PORTID_RFC_GPO0;
        IOC->CFG[PA_EN_PIN] = IOCFG_PORTID_RFC_GPO1;

        gpio_init(HGM_PIN, GPIO_OUT);
        HGM_ON;
    }
}
