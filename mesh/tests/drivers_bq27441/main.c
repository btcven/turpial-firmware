/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       Test application for TI BQ27441 Single Cell Li-Ion Fuel Gauge
 * @author      Locha Mesh Developers <developers@locha.io>
 * @file
 *
 * The test application demonstrates the use of the BQ27441. It uses the default
 * configuration parameters:
 *
 * - TODO
 *
 * The application may choose two approaches to receive new data from the Fuel
 * Gauge, either
 *
 * - Periodically fetching data.
 * - Fetching data when an interrupt is generated.
 *
 * To use the latter approach, module `bq27441_int` needs to be enabled, and the
 * GPIO to which the sensor **GPOUT** output pin is connected has to be defined
 * by #BQ27441_PARAM_INT_PIN, for example:
 *
 * ```
 * USEMODULE=bq27441_int CFLAGS="-DBQ27441_PARAM_INT_PIN=\(GPIO_PIN\(0,12\)\)" \
 * make flash -C tests/driver_bq27441 BOARD=...
 * ```
 */

#include <stdio.h>

#include "thread.h"
#include "xtimer.h"

#include "bq27441.h"
#include "bq27441_params.h"

#define BQ27441_SLEEP (500 * US_PER_MS)

static kernel_pid_t p_main;

#if IS_ACTIVE(MODULE_BQ27441_INT)
static void bq27441_isr_data_ready(void *arg)
{
    (void)arg;

    msg_t msg;
    msg_send(&msg, p_main);
}
#endif

int main(void)
{
    bq27441_t dev;

    p_main = sched_active_pid;

    puts("BQ27441 fuel gauge test application\n");
    puts("Initializing BQ27441 fuel gauge");

    if (bq27441_init(&dev, &bq27441_params[0]) != BQ27441_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

#if IS_ACTIVE(MODULE_BQ27441_INT)
    bq27441_init_int(&dev, bq27441_isr_data_ready, 0);
#endif

    while (1) {
#if IS_ACTIVE(MODULE_BQ27441_INT)
        msg_t msg;
        msg_receive(&msg);
#else
        xtimer_usleep(BQ27441_SLEEP);
#endif
        uint16_t volts;
        uint16_t rem_cap;
        int16_t pwr;

        if ((bq27441_voltage(&dev, &volts) == BQ27441_OK) &&
            (bq27441_remaining_capacity_filtered(&dev, &rem_cap) == BQ27441_OK) &&
            (bq27441_average_power(&dev, &pwr) == BQ27441_OK)) {
            printf("bat mV: %-5" PRIu16 " mAh: %-5" PRIu16 " mW: %-5" PRIi16 "\n",
                   volts, rem_cap, pwr);
        }
    }

    return 0;
}
