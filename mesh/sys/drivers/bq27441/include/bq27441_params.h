/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Fuel Gauge BQ27441
 *
 * @author      Locha Mesh Developers <developers@locha.io>
 */

#ifndef BQ27441_PARAMS_H
#define BQ27441_PARAMS_H

#include "bq27441.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configurations parameters
 * @{
 */
#define BQ27441_PARAM_I2C          I2C_DEV(0)
#define BQ27441_PARAM_DESIGN_CAP   1000

#define BQ27441_PARAM_INT_PIN      GPIO_UNDEF
#define BQ27441_PARAM_INT_LEVEL    BQ27441_INT_HIGH

#if IS_ACTIVE(MODULE_BQ27441_INT)
#define BQ27441_PARAMS             { .dev = BQ27441_PARAM_I2C, \
                                     .design_cap = BQ27441_PARAM_DESIGN_CAP, \
                                     .int_pin = BQ27441_PARAM_INT_PIN, \
                                     .int_level = BQ27441_PARAM_INT_LEVEL }
#else
#define BQ27441_PARAMS             { .dev = BQ27441_PARAM_I2C, \
                                     .design_cap = BQ27441_PARAM_DESIGN_CAP, }
#endif
/** @} */

const bq27441_params_t bq27441_params[] =
{
    BQ27441_PARAMS
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BQ27441_PARAMS_H */
