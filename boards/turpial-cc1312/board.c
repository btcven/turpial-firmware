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
 * @brief           Board specific implementations for Locha Mesh Turpial
 *
 */

#include "cpu.h"
#include "board.h"

/**
 * @brief           Initialise the board.
 */
void board_init(void)
{
    cpu_init();

    /* The Turpial board has an integrated Front-End that any CC13x2 is capable
     * of controlling by routing specific pins in the IOC
     */
    if (IS_USED(MODULE_CC26X2_CC13X2_RF)) {
        /* Set to logical 1 the HGM pin, this enables the High-Gain mode.
         *
         * swrs089a, Controlling the Output Power form CC1190, pag. 8
         */
        gpio_init(RF_HGM_PIN, GPIO_OUT);
        gpio_set(RF_HGM_PIN);

        IOC->CFG[RF_PA_EN_PIN] = IOCFG_PORTID_RFC_GPO1;
        IOC->CFG[RF_LNA_EN_PIN] = IOCFG_PORTID_RFC_GPO0;
    }
}
