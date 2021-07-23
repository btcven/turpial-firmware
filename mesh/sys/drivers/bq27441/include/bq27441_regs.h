/*
 * Copyright (C) 2021 btcven and Locha Mesh developers <contact@locha.io>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    drivers_bq27441
 * @{
 *
 * @file
 * @brief       Fuel Gauge BQ27441 register definitions
 *
 * @author      Locha Mesh developers <contact@locha.io>
 */

#ifndef BQ27441_REGS_H
#define BQ27441_REGS_H

#include "bitarithm.h"

/**
 * @name    BQ27441 registers
 * @{
 */
#define BQ27441_REG_CONTROL                         (0x00)
#define BQ27441_REG_TEMPERATURE                     (0x02)
#define BQ27441_REG_VOLTAGE                         (0x04)
#define BQ27441_REG_FLAGS                           (0x06)
#define BQ27441_REG_NOMINAL_AVAILABLE_CAPACITY      (0x08)
#define BQ27441_REG_FULL_AVAILABLE_CAPACITY         (0x0A)
#define BQ27441_REG_REMAINING_CAPACITY              (0x0C)
#define BQ27441_REG_FULL_CHARGE_CAPACITY            (0x0E)
#define BQ27441_REG_AVERAGE_CURRENT                 (0x10)
#define BQ27441_REG_STANDBY_CURRENT                 (0x12)
#define BQ27441_REG_MAX_LOAD_CURRENT                (0x14)
#define BQ27441_REG_AVERAGE_POWER                   (0x18)
#define BQ27441_REG_STATE_OF_CHARGE                 (0x1C)
#define BQ27441_REG_INTERNAL_TEMPERATURE            (0x1E)
#define BQ27441_REG_STATE_OF_HEALTH                 (0x20)
#define BQ27441_REG_REMAINING_CAPACITY_UNFILTERED   (0x28)
#define BQ27441_REG_REMAINING_CAPACITY_FILTERED     (0x2A)
#define BQ27441_REG_FULL_CHARGE_CAPACITY_UNFILTERED (0x2C)
#define BQ27441_REG_FULL_CHARGE_CAPACITY_FILTERED   (0x2E)
#define BQ27441_REG_STATE_OF_CHARGE_UNFILTERED      (0x30)
/** @} */

/**
 * @name    BQ27441 extended registers
 *
 * @note These registers are not limited to 2-bytes.
 * @{
 */
#define BQ27441_EXT_REG_OPCOFNIG                    (0x3A)
#define BQ27441_EXT_REG_DESIGN_CAPACITY             (0x3C)
#define BQ27441_EXT_REG_DATA_CLASS                  (0x3E)
#define BQ27441_EXT_REG_DATA_BLOCK                  (0x3F)
#define BQ27441_EXT_REG_BLOCK_DATA                  (0x40)
#define BQ27441_EXT_REG_BLOCK_DATA_CHECK_SUM        (0x60)
#define BQ27441_EXT_REG_BLOCK_DATA_CONTROL          (0x61)
/** @} */

/**
 * @name    BQ27441 `Control()` subcommands
 * @{
 */
#define BQ27441_CONTROL_CONTROL_STATUS              (0x0000)
#define BQ27441_CONTROL_FW_VERSION                  (0x0002)
#define BQ27441_CONTROL_DM_CODE                     (0x0004)
#define BQ27441_CONTROL_PREV_MACWRITE               (0x0007)
#define BQ27441_CONTROL_CHEM_ID                     (0x0008)
#define BQ27441_CONTROL_BAT_INSERT                  (0x000C)
#define BQ27441_CONTROL_BAT_REMOVE                  (0x000D)
#define BQ27441_CONTROL_SET_HIBERNATE               (0x0011)
#define BQ27441_CONTROL_CLEAR_HIBERNATE             (0x0012)
#define BQ27441_CONTROL_SET_CFGUPDATE               (0x0013)
#define BQ27441_CONTROL_SHUTDOWN_ENABLE             (0x001B)
#define BQ27441_CONTROL_SHUTDOWN                    (0x001C)
#define BQ27441_CONTROL_SEALED                      (0x0020)
#define BQ27441_CONTROL_TOGGLE_GPOUT                (0x0023)
#define BQ27441_CONTROL_RESET                       (0x0041)
#define BQ27441_CONTROL_SOFT_RESET                  (0x0042)
#define BQ27441_CONTROL_EXIT_CFGUPDATE              (0x0043)
#define BQ27441_CONTROL_EXIT_RESIM                  (0x0044)
#define BQ27441_CONTROL_UNSEAL                      (0x8000)
/** @} */

/**
 * @name    `CONTROL_STATUS` bit definitions
 * @{
 */
#define BQ27441_CONTROL_STATUS_SHUTDOWNEN           BIT15
#define BQ27441_CONTROL_STATUS_WDRESET              BIT14
#define BQ27441_CONTROL_STATUS_SS                   BIT13
#define BQ27441_CONTROL_STATUS_CALMODE              BIT12
#define BQ27441_CONTROL_STATUS_CCA                  BIT11
#define BQ27441_CONTROL_STATUS_BCA                  BIT10
#define BQ27441_CONTROL_STATUS_QMAX_UP              BIT9
#define BQ27441_CONTROL_STATUS_RES_UP               BIT8
#define BQ27441_CONTROL_STATUS_INITCOMP             BIT7
#define BQ27441_CONTROL_STATUS_HIBERNATE            BIT6
#define BQ27441_CONTROL_STATUS_SLEEP                BIT4
#define BQ27441_CONTROL_STATUS_LDMD                 BIT3
#define BQ27441_CONTROL_STATUS_RUP_DIS              BIT2
#define BQ27441_CONTROL_STATUS_VOK                  BIT1
/** @} */

/**
 * @name    `Flags()` bit definitions
 * @{
 */
#define BQ27441_FLAG_OT                             BIT15
#define BQ27441_FLAG_UT                             BIT14
#define BQ27441_FLAG_FC                             BIT9
#define BQ27441_FLAG_CHG                            BIT8
#define BQ27441_FLAG_OCVTAKEN                       BIT7
#define BQ27441_FLAG_ITPOR                          BIT5
#define BQ27441_FLAG_CFGUPMODE                      BIT4
#define BQ27441_FLAG_BAT_DET                        BIT3
#define BQ27441_FLAG_SOC1                           BIT2
#define BQ27441_FLAG_SOCF                           BIT1
#define BQ27441_FLAG_DSG                            BIT0
/** @} */

#endif /* BQ27441_REGS_H */
/** @} */
