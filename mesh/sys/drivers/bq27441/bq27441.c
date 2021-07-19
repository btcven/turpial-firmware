/*
 * Copyright (C) 2021 btcven and Locha Mesh developers <contact@locha.io>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drviers_bq27441
 * @{
 *
 * @file
 * @brief       BQ27441 Single Cell Li-Ion Fuel Gauge
 *
 * @author      Locha Mesh developers <contact@locha.io>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <xtimer.h>

#include "bq27441.h"
#include "bq27441_regs.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if ENABLE_DEBUG

#define DEBUG_DEV(f, d, ...) \
    do { \
        DEBUG("[bq27441] %s i2c dev=%d: " f "\n", \
              __func__, d->params.dev, ## __VA_ARGS__); \
    } while (0)

#else

#define DEBUG_DEV(f, d, ...)

#endif

#define BQ27441_BUS_FREE_US        (66) /**< Bus-free waiting time (66 us) */
#define BQ27441_CFGUPMODE_RETRIES (100) /**< Number of retries when polling */

static void _cmd_wait_time(bq27441_t *dev)
{
    timex_t null = timex_set(0, 0);
    timex_t sec = timex_set(1, 0);

    /* Compare the difference between the last two sent commands, if they're
     * both under 1 second, wait the difference to complete 1 s, this is to
     * avoid resetting the fuel gauge as TI says it could result in "expiration
     * of the watchdog timer". */
    timex_t diff = timex_sub(dev->last_cmd[1], dev->last_cmd[0]);
    if (timex_cmp(diff, sec) < 0 &&
        timex_cmp(dev->last_cmd[0], null) != 0 &&
        timex_cmp(dev->last_cmd[1], null) != 0) {

        /* Sleep the difference to complete 1 s */
        xtimer_usleep(timex_uint64(diff));

        /* Reset timestamps */
        dev->last_cmd[0] = null;
        dev->last_cmd[1] = null;
    }
}

static void _cmd_update_timestamp(bq27441_t *dev)
{
    timex_t null = timex_set(0, 0);
    timex_t now;
    xtimer_now_timex(&now);

    if (timex_cmp(dev->last_cmd[0], null) == 0) {
        dev->last_cmd[0] = now;
        dev->last_cmd[1] = null;
    }
    else {
        dev->last_cmd[1] = now;
    }
}

static int _bq27441_write_regs(bq27441_t *dev, uint8_t reg, uint8_t *data,
                               size_t len)
{
    assert(len <= sizeof(uint16_t));
    int ret;

    i2c_acquire(dev->params.dev);
    _cmd_wait_time(dev);
    for (size_t i = 0; i < len; i++) {
        if ((ret = i2c_write_reg(dev->params.dev, BQ27441_ADDRESS, reg + 1,
                        data[i], 0)) < 0) {
            DEBUG_DEV("Failed to write reg %02x", dev, reg);
            i2c_release(dev->params.dev);
            return BQ27441_ERROR_I2C;
        }
        xtimer_usleep(BQ27441_BUS_FREE_US);
    }
    _cmd_update_timestamp(dev);
    i2c_release(dev->params.dev);
    return BQ27441_OK;
}

static int _bq27441_read_regs(bq27441_t *dev, uint8_t reg, uint8_t *data,
                              size_t len)
{
    assert(len <= sizeof(uint16_t));
    int ret;

    i2c_acquire(dev->params.dev);
    _cmd_wait_time(dev);
    for (size_t i = 0; i < len; i++) {
        if ((ret = i2c_read_reg(dev->params.dev, BQ27441_ADDRESS, reg + i,
                                &data[i], 0)) < 0) {
            DEBUG_DEV("Failed to read reg %02x", dev, reg);
            i2c_release(dev->params.dev);
            return BQ27441_ERROR_I2C;
        }
        xtimer_usleep(BQ27441_BUS_FREE_US);
    }
    _cmd_update_timestamp(dev);
    i2c_release(dev->params.dev);
    return BQ27441_OK;
}

static int _bq27441_read_word(bq27441_t *dev, uint16_t reg,
                              uint16_t *data)
{
    uint8_t buf[2];
    int ret = _bq27441_read_regs(dev, reg, buf, sizeof(buf));
    DEBUG_DEV("read: %02x%02x", dev, buf[1], buf[0]);
    *data = ((uint16_t)buf[1] << 8) | (uint16_t)buf[0];
    return ret;
}

int bq27441_init(bq27441_t *dev, const bq27441_params_t *params)
{
    DEBUG_DEV("params=%p", dev, params);

    dev->params = *params;

    return BQ27441_OK;
}

#if IS_ACTIVE(MODULE_BQ27441_INT)
int bq27441_init_int(bq27441_t *dev, bq27441_int_cb_t cb, void *arg)
{
    assert(dev != NULL);
    assert(dev->params.int_pin != GPIO_UNDEF);

    DEBUG_DEV("cb=%p, arg=%p", dev, cb, arg);

    if (dev->params.int_level == BQ27441_INT_HIGH) {
        gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_RISING, cb, arg);
    }
    else {
        gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_FALLING, cb, arg);
    }

    return BQ27441_OK;
}
#endif

int bq27441_control(bq27441_t *dev, uint16_t function, uint16_t *res)
{
    DEBUG_DEV("func=%04x, res=%p", dev, function, res);
    int ret;

    uint8_t func[2] = { (uint8_t)(function & 0x00FF), (uint8_t)(function >> 8)};
    if ((ret = _bq27441_write_regs(dev, BQ27441_REG_CONTROL, func,
                                   sizeof(func))) != BQ27441_OK) {
        DEBUG_DEV("Couldn't write control function", dev);
        return ret;
    }

    /* Read operation result if wanted */
    if (res != NULL) {
        return _bq27441_read_word(dev, BQ27441_REG_CONTROL, res);
    }

    return BQ27441_OK;
}

int bq27441_unseal(bq27441_t *dev)
{
    DEBUG_DEV("", dev);
    int ret;

    if ((ret = bq27441_control(dev, BQ27441_CONTROL_UNSEAL,
                                NULL)) != BQ27441_OK) {
        DEBUG_DEV("First unseal write failed", dev);
        return ret;
    }

    if ((ret = bq27441_control(dev, BQ27441_CONTROL_UNSEAL,
                                NULL)) != BQ27441_OK) {
        DEBUG_DEV("Second unseal write failed", dev);
        return ret;
    }

    return BQ27441_OK;
}

int bq27441_seal(bq27441_t *dev)
{
    DEBUG_DEV("", dev);

    return bq27441_control(dev, BQ27441_CONTROL_SEALED, NULL);
}

int bq27441_sealed(bq27441_t *dev, bool *sealed)
{
    DEBUG_DEV("sealed=%p", dev, sealed);
    int ret;
    uint16_t control_status;

    if ((ret = bq27441_control(dev, BQ27441_CONTROL_CONTROL_STATUS,
                               &control_status)) != BQ27441_OK) {
        DEBUG_DEV("Failed to seal", dev);
        return ret;
    }

    DEBUG_DEV("CONTROL_STATUS=%04x", dev, control_status);

    *sealed = (control_status & BQ27441_CONTROL_STATUS_SS) ? true : false;

    return BQ27441_OK;
}

int bq27441_temperature(bq27441_t *dev, uint16_t *temperature)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_TEMPERATURE, temperature);
}

int bq27441_set_temperature(bq27441_t *dev, uint16_t temperature)
{
    DEBUG_DEV("", dev);

    uint8_t temp[2] = {
        (uint8_t)(temperature & 0x00FF),
        (uint8_t)(temperature >> 8)
    };
    return _bq27441_write_regs(dev, BQ27441_REG_TEMPERATURE, temp,
                               sizeof(temp));
}

int bq27441_voltage(bq27441_t *dev, uint16_t *voltage)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_VOLTAGE, voltage);
}

int bq27441_flags(bq27441_t *dev, uint16_t *flags)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_FLAGS, flags);
}

int bq27441_nominal_available_capacity(bq27441_t *dev, uint16_t *cap)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_NOMINAL_AVAILABLE_CAPACITY, cap);
}

int bq27441_full_available_capacity(bq27441_t *dev, uint16_t *cap)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_FULL_AVAILABLE_CAPACITY, cap);
}

int bq27441_remaining_capacity(bq27441_t *dev, uint16_t *cap)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_REMAINING_CAPACITY, cap);
}

int bq27441_full_charge_capacity(bq27441_t *dev, uint16_t *cap)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_FULL_CHARGE_CAPACITY, cap);
}

int bq27441_average_current(bq27441_t *dev, int16_t *current)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_AVERAGE_CURRENT,
                              (uint16_t *)current);
}

int bq27441_standby_current(bq27441_t *dev, int16_t *current)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_STANDBY_CURRENT,
                             (uint16_t *)current);
}

int bq27441_max_load_current(bq27441_t *dev, int16_t *current)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_MAX_LOAD_CURRENT,
                              (uint16_t *)current);
}

int bq27441_average_power(bq27441_t *dev, int16_t *power)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_AVERAGE_POWER, (uint16_t *)power);
}

int bq27441_state_of_charge(bq27441_t *dev, uint16_t *state)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_STATE_OF_CHARGE, state);
}


int bq27441_internal_temperature(bq27441_t *dev, uint16_t *temperature)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_INTERNAL_TEMPERATURE,
                              temperature);
}

int bq27441_state_of_health(bq27441_t *dev, uint16_t *state)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_STATE_OF_HEALTH, state);
}

int bq27441_remaining_capacity_unfiltered(bq27441_t *dev, uint16_t *cap)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_REMAINING_CAPACITY_UNFILTERED,
                              cap);
}

int bq27441_remaining_capacity_filtered(bq27441_t *dev, uint16_t *cap)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_REMAINING_CAPACITY_FILTERED,
                              cap);
}

int bq27441_full_charge_capacity_unfiltered(bq27441_t *dev, uint16_t *cap)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_FULL_CHARGE_CAPACITY_UNFILTERED,
                              cap);
}

int bq27441_full_charge_capacity_filtered(bq27441_t *dev, uint16_t *cap)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_FULL_CHARGE_CAPACITY_FILTERED,
                              cap);
}

int bq27441_state_of_charge_unfiltered(bq27441_t *dev, uint16_t *state)
{
    DEBUG_DEV("", dev);

    return _bq27441_read_word(dev, BQ27441_REG_STATE_OF_CHARGE_UNFILTERED,
                              state);
}
