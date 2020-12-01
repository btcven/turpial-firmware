/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_bq27441
 * @ingroup     drivers_bq27441
 *
 * @{
 *
 * @file
 * @brief       BQ27441 Single Cell Li-Ion Fuel Gauge
 *
 * @author      Locha Mesh Developers <developers@locha.io>
 */
#ifndef BQ27441_H
#define BQ27441_H

#include <stdint.h>

#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   BQ27441 I2C address
 */
#define BQ27441_ADDRESS 0x55

/**
 * @brief   Named return values
 */
enum {
    BQ27441_OK              =  0, /**< success */
    BQ27441_ERROR_I2C       = -1, /**< I2C error */
    BQ27441_ERROR_CFGUPMODE = -2, /**< Can't enter CFGUPMODE */
};

enum {
    BQ27441_INT_LOW         = 0,
    BQ27441_INT_HIGH        = 1,
};

/**
 * @brief    BQ27441 device parameters
 */
typedef struct {
    i2c_t dev;      /**< I2C bus */
    int design_cap; /**< Battery design capacity in mAh */
#if IS_ACTIVE(MODULE_BQ27441_INT_PIN)
    gpio_t int_pin; /**< GPOUT interrupt pin */
    int int_level;  /**< Interrupt level */
#endif
} bq27441_params_t;

/**
 * @brief    BQ27441 device
 */
typedef struct {
    bq27441_params_t params; /**< Device parameters */
    timex_t last_cmd[2]; /**< Timestamp of last sent commands */
} bq27441_t;

/**
 * @brief    Initialize BQ27441 device.
 *
 * @params[in]  dev    BQ27441 device.
 * @params[in]  params BQ27441 device parameters.
 */
int bq27441_init(bq27441_t *dev, const bq27441_params_t *params);

#if IS_ACTIVE(MODULE_BQ27441_INT_PIN)
/**
 * @brief   Initialize ISR for BQ27441 interrupt pin.
 *
 * @param[in]  dev     BQ27441 device.
 * @param[in]  cb      ISR callback.
 * @param[in]  arg     Callback argument.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_init_int(bq27441_t *dev, bq27441_int_cb_t cb, void *arg);
#endif

/**
 * @brief    Issue a `Control()` subcommand.
 *
 * @note If the control function doesn't return anything, @p res should be
 *       `NULL`.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[in]  function    Desired function.
 * @param[out] res         Result.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_control(bq27441_t *dev, uint16_t function, uint16_t *res);

/**
 * @brief    Unseal the fuel gauge.
 *
 * @param[in]  dev         BQ27441 device.
 */
int bq27441_unseal(bq27441_t *dev);

/**
 * @brief    Seal the fuel gauge.
 *
 * @param[in]  dev         BQ27441 device.
 */
int bq27441_seal(bq27441_t *dev);

/**
 * @brief    Checks if the fuel gauge is sealed.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[out] sealed      Return value.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_sealed(bq27441_t *dev, bool *sealed);

/**
 * @brief    Temperature in units of 0.1°K measured by the fuel gauge.
 *
 * @note If `OpConfig[TEMPS]` bit = 0 (default), a read command will return the
 *       internal temperature sensor value and a write command will be ignored.
 *
 * @note If `OpConfig[TEMPS]` bit = 1, a write command sets the temperature to
 *       be used for gauging calculations while a read command returns to the
 *       temperature previously written. To set the temperature
 *       @ref bq27441_set_temperature can be used.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[out] temperature Temperature in 0.1°K units.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_temperature(bq27441_t *dev, uint16_t *temperature);

/**
 * @brief    Set the temperature to be used for gauging calculations.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[in]  temperature Temperature in 0.1°K units.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_set_temperature(bq27441_t *dev, uint16_t temperature);

/**
 * @brief   Measured cell-pack voltage in mV, ranges from 0 to 6000 mV.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[in]  voltage     Voltage in mV.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_voltage(bq27441_t *dev, uint16_t *voltage);

/**
 * @brief   Fuel gauge status.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[out] flags       Flags.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_flags(bq27441_t *dev, uint16_t *flags);

/**
 * @brief   Uncompensated (less than C/20 load) battery capacity remaining.
 *
 * @param[in] dev      BQ27441 device.
 * @param[in] capacity Capacity in mAh.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_nominal_available_capacity(bq27441_t *dev, uint16_t *cap);

/**
 * @brief    Uncompensated (less than C/20 load) capacity of the battery when
 *           fully charged.
 *
 * @param[in] dev      BQ27441 device.
 * @param[in] capacity Capacity in mAh.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_full_available_capacity(bq27441_t *dev, uint16_t *cap);

/**
 * @brief   Remaining battery capacity compensated for load and temperature.
 *
 * @note If `OpConfigB[SMOOTHEN]= 1`, this is equal to
 *       @ref bq27441_remaining_capacity_filtered() otherwise, it is equal to
 *       @ref bq27441_remaining_capacity_unfiltered().
 *
 * @param[in] dev      BQ27441 device.
 * @param[in] capacity Capacity in mAh.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_remaining_capacity(bq27441_t *dev, uint16_t *cap);

/**
 * @brief    Compensated capacity of the battery when fully charged.
 *
 * This value is updated at regular intervals and is compensated for load and
 * temperature.
 *
 * @note If `OpConfigB[SMOOTHEN]= 1`, this is equal to
 *       @ref bq27441_full_charge_capacity_filtered(); otherwise,it is equal to
 *       @ref bq27441_full_charge_capacity_unfiltered().
 *
 * @param[in] dev      BQ27441 device.
 * @param[in] capacity Capacity in mAh.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_full_charge_capacity(bq27441_t *dev, uint16_t *cap);

/**
 * @brief  Average current flow through the sense resistor.
 *
 * In NORMAL mode, it is updated once per second and is calculated by dividing
 * the 1-second change in coulomb counter data by 1 second. Large current spikes
 * of short duration will be averaged out in this measurement.
 *
 * @param[in] dev      BQ27441 device.
 * @param[in] current  Current in mAh.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_average_current(bq27441_t *dev, int16_t *current);

/**
 * @brief   Measured standby current through the sense resistor.
 *
 * The standby current is an adaptive measurement. Initially, it reports the
 * standby current programmed in `Initial Standby` and, after spending several
 * seconds in standby, reportsthe measured standby current.
 *
 * The register value is updated every second when the measured current is above
 * the `Deadband` and is less than or equal to `2 x InitialStandby`. The first
 * and last values that meet this criteria are not averaged in, because they may
 * not be stable values. To approximate a 1-minute time constant, each new
 * standby current value is computed by taking approximately 93% of the last
 * measured standby current and approximately 7% of the currently measured
 * average current.
 *
 * @param[in] dev      BQ27441 device.
 * @param[in] current  Current in mAh.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_standby_current(bq27441_t *dev, int16_t *current);

/**
 * @brief   Maximum load conditions.
 *
 * The value is an adaptiv emeasurement which is initially reported as the
 * maximum load current programmed in `InitialMaxLoad` current. If the measured
 * current is greater than `InitialMaxLoad`, then max load current updates to
 * the new current. Max Load Current is reduced to the average of the previous
 * value and `InitialMaxLoad` whenever the battery is charged to full after
 * a previous discharge to an SOC less than 50%. This prevents the reported
 * value from maintaining an unusually high value.
 *
 * @param[in] dev      BQ27441 device.
 * @param[in] current  Current in mAh.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_max_load_current(bq27441_t *dev, int16_t *current);

/**
 * @brief    Average power during charging and discharging of the battery.
 *
 * It is negative during discharge and positive during charge. A valueof 0
 * indicates that the battery is not being discharged.
 *
 * @param[in] dev      BQ27441 device.
 * @param[in] power    Power in mW.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_average_power(bq27441_t *dev, int16_t *power);

/** @brief   Predicted remaining battery capacity.
 *
 * Expressed as a percentage of @ref bq27441_full_charge_capacity() with a range
 * of 0 to 100%
 *
 * @param[in]  dev      BQ27441 device.
 * @param[out] state    State of charge in %.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_state_of_charge(bq27441_t *dev, uint16_t *state);

/**
 * @brief   Internal temperature sensor in units of 0.1°K as measured by the
 *          fuel gauge.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[out] temperature Internal temperature in units of 0.1°K.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_internal_temperature(bq27441_t *dev, uint16_t *temperature);

/**
 * @brief   Percentage of the ratio of predicted State of Health.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[out] state       State of Health in %.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_state_of_health(bq27441_t *dev, uint16_t *state);

/**
 * @brief   True battery capacity remaining.
 *
 * This value can jump as the gauge updates its predictions dynamically.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[out] cap         Capacity in mAh.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_remaining_capacity_unfiltered(bq27441_t *dev, uint16_t *cap);

/**
 * @brief   Filtered battery capacity remaining.
 *
 * This valueis not allowed to jump unless
 * @ref bq27441_remaining_capacity_unfiltered reaches empty or full before
 * @ref bq27441_remaining_capacity_filtered does.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[out] cap         Capacity in mAh.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_remaining_capacity_filtered(bq27441_t *dev, uint16_t *cap);

/**
 * @brief   Compensated capacity of the battery when fully charged.
 *
 * This value is updated at regular intervals.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[out] cap         Capacity in mAh.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_full_charge_capacity_unfiltered(bq27441_t *dev, uint16_t *cap);

/**
 * @brief   Compensated capacity of the battery when fully charged.
 *
 * @note It has no physical meaning and is manipulated to ensure the
 *       @ref bq72441_state_of_charge register is smoothed if
 *       `OpConfigB[SMOOTHEN]= 1`.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[out] cap         Capacity in mAh.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_full_charge_capacity_filtered(bq27441_t *dev, uint16_t *cap);

/**
 * @brief   True State of Charge.
 *
 * This value is updated at regular intervals, and may jump as the gauge updates
 * its predictions dynamically.
 *
 * @param[in]  dev         BQ27441 device.
 * @param[out] state       State of Charge in %.
 *
 * @return BQ27441_OK on success.
 */
int bq27441_state_of_charge_unfiltered(bq27441_t *dev, uint16_t *state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* BQ27441_H */
/** @} */
