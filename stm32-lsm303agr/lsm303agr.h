/**
 * @file        lsm303agr.h
 * @author      Florian Topeza & Merlin Kooshmanian
 * @brief       Header file for LSM303AGR Accelerometer/Magnetometer driver
 * @version     0.1
 * @date        2026-05-29
 *
 * @copyright Copyright (c) 2026 Florian Topeza
 *
 *
 * @attention
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 */

/* ==== Header guard ==== */
#ifndef LSM303AGR_H
#define LSM303AGR_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==== Includes ==== */

#include <stdint.h>
#include <stdio.h>

// Include the HAL of the STM32 microcontroller in use
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_def.h"

#include "errors.h"

/* ==== Defines ==== */

/* Debug LSM303AGR */
#ifdef DEBUG_LSM303AGR
#include "console.h"
#define logs_lsm303agr(...) printf(__VA_ARGS__)
#else
#define logs_lsm303agr(...)
#endif

/**
 * @defgroup LSM303AGR_IO_CMD SPI Command Bits
 * @{
 */

#define READWRITE_CMD    ((uint8_t)0x80U) /**< Read/Write command bit. Unused: legacy from the SPI reference driver, this port uses I2C only */
#define MULTIPLEBYTE_CMD ((uint8_t)0x40U) /**< Multiple byte read/write command bit. Unused: legacy from the SPI reference driver, this port uses I2C only */
#define DUMMY_BYTE       ((uint8_t)0x00U) /**< Dummy byte sent by the SPI master to generate the clock. Unused: legacy from the SPI reference driver, this port uses I2C only */

/** @} */

/**
 * @defgroup LSM303AGR_I2C I2C Configuration Constants
 * @{
 */

#define ACC_I2C_ADDRESS     (0x32U) /**< Accelerometer 8-bit I2C address (7-bit address pre-shifted) */
#define MAG_I2C_ADDRESS     (0x3CU) /**< Magnetometer 8-bit I2C address (7-bit address pre-shifted) */
#define I2C_BUFFER_MAX_SIZE (32U)   /**< Maximum size of the internal I2C write buffer, in bytes */

/** @} */

/**
 * @defgroup LSM303AGR_REG Register Address Map
 * @{
 */

/* Temperature sensor registers */
#define LSM303AGR_STATUS_REG_AUX_A (0x07U) /**< Auxiliary (temperature) status register */
#define LSM303AGR_OUT_TEMP_L_A     (0x0CU) /**< Temperature output, low byte */
#define LSM303AGR_OUT_TEMP_H_A     (0x0DU) /**< Temperature output, high byte */
#define LSM303AGR_IN_COUNTER_REG_A (0x0EU) /**< Internal counter register */

/* Accelerometer registers */
#define LSM303AGR_WHO_AM_I_A       (0x0FU) /**< Device identification register, expected value 0x33 */

/* Temperature sensor registers */
#define LSM303AGR_TEMP_CFG_REG_A   (0x1FU) /**< Temperature sensor configuration register */

/* Accelerometer registers */
#define LSM303AGR_CTRL_REG1_A      (0x20U) /**< Accelerometer control register 1 (ODR, power mode, axes enable) */
#define LSM303AGR_CTRL_REG2_A      (0x21U) /**< Accelerometer control register 2 (high-pass filter) */
#define LSM303AGR_CTRL_REG3_A      (0x22U) /**< Accelerometer control register 3 (INT1 function routing) */
#define LSM303AGR_CTRL_REG4_A      (0x23U) /**< Accelerometer control register 4 (BDU, endianness, full scale) */
#define LSM303AGR_CTRL_REG5_A      (0x24U) /**< Accelerometer control register 5 (FIFO, boot, latch) */
#define LSM303AGR_CTRL_REG6_A      (0x25U) /**< Accelerometer control register 6 (INT2 function routing) */
#define LSM303AGR_REFERENCE_A      (0x26U) /**< Reference register for interrupt generation */
#define LSM303AGR_STATUS_REG_A     (0x27U) /**< Accelerometer status register (data-ready flags) */
#define LSM303AGR_OUT_X_L_A        (0x28U) /**< Linear acceleration output, X axis, low byte */
#define LSM303AGR_OUT_X_H_A        (0x29U) /**< Linear acceleration output, X axis, high byte */
#define LSM303AGR_OUT_Y_L_A        (0x2AU) /**< Linear acceleration output, Y axis, low byte */
#define LSM303AGR_OUT_Y_H_A        (0x2BU) /**< Linear acceleration output, Y axis, high byte */
#define LSM303AGR_OUT_Z_L_A        (0x2CU) /**< Linear acceleration output, Z axis, low byte */
#define LSM303AGR_OUT_Z_H_A        (0x2DU) /**< Linear acceleration output, Z axis, high byte */
#define LSM303AGR_FIFO_CTRL_REG_A  (0x2EU) /**< FIFO control register */
#define LSM303AGR_FIFO_SRC_REG_A   (0x2FU) /**< FIFO source register */

#define LSM303AGR_INT1_CFG_A       (0x30U) /**< Interrupt 1 configuration register */
#define LSM303AGR_INT1_SOURCE_A    (0x31U) /**< Interrupt 1 source register */
#define LSM303AGR_INT1_THS_A       (0x32U) /**< Interrupt 1 threshold register */
#define LSM303AGR_INT1_DURATION_A  (0x33U) /**< Interrupt 1 duration register */

#define LSM303AGR_INT2_CFG_A       (0x34U) /**< Interrupt 2 configuration register */
#define LSM303AGR_INT2_SOURCE_A    (0x35U) /**< Interrupt 2 source register */
#define LSM303AGR_INT2_THS_A       (0x36U) /**< Interrupt 2 threshold register */
#define LSM303AGR_INT2_DURATION_A  (0x37U) /**< Interrupt 2 duration register */

#define LSM303AGR_CLICK_CFG_A      (0x38U) /**< Click configuration register */
#define LSM303AGR_CLICK_SOURCE_A   (0x39U) /**< Click source register */
#define LSM303AGR_CLICK_THS_A      (0x3AU) /**< Click threshold register */

#define LSM303AGR_TIME_LIMIT_A     (0x3BU) /**< Click time limit register */
#define LSM303AGR_TIME_LATENCY_A   (0x3CU) /**< Click time latency register */
#define LSM303AGR_TIME_WINDOW_A    (0x3DU) /**< Click time window register */

/* System registers */
#define LSM303AGR_Act_THS_A        (0x3EU) /**< Return-to-sleep activation threshold register */
#define LSM303AGR_Act_DUR_A        (0x3FU) /**< Return-to-sleep duration register */

/* Magnetometer registers */
#define LSM303AGR_X_REG_L_M        (0x45U) /**< Hard-iron X magnetic offset, low byte */
#define LSM303AGR_X_REG_H_M        (0x46U) /**< Hard-iron X magnetic offset, high byte */
#define LSM303AGR_Y_REG_L_M        (0x47U) /**< Hard-iron Y magnetic offset, low byte */
#define LSM303AGR_Y_REG_H_M        (0x48U) /**< Hard-iron Y magnetic offset, high byte */
#define LSM303AGR_Z_REG_L_M        (0x49U) /**< Hard-iron Z magnetic offset, low byte */
#define LSM303AGR_Z_REG_H_M        (0x4AU) /**< Hard-iron Z magnetic offset, high byte */

/** @deprecated Legacy alias kept for backward compatibility, use LSM303AGR_Z_REG_H_M instead */
#define LSM303AGR_Z_REH_H_M        LSM303AGR_Z_REG_H_M

#define LSM303AGR_WHO_AM_I_M       (0x4FU) /**< Device identification register, expected value 0x40 */
#define LSM303AGR_CFG_REG_A_M      (0x60U) /**< Magnetometer configuration register A (mode, ODR) */
#define LSM303AGR_CFG_REG_B_M      (0x61U) /**< Magnetometer configuration register B (offset cancellation) */
#define LSM303AGR_CFG_REG_C_M      (0x62U) /**< Magnetometer configuration register C (interrupt, endianness) */
#define LSM303AGR_INT_CTRL_REG_M   (0x63U) /**< Magnetometer interrupt control register */
#define LSM303AGR_INT_SOURCE_REG_M (0x64U) /**< Magnetometer interrupt source register */
#define LSM303AGR_INT_THS_L_REG_M  (0x65U) /**< Magnetometer interrupt threshold register, low byte */
#define LSM303AGR_INT_THS_H_REG_M  (0x66U) /**< Magnetometer interrupt threshold register, high byte */
#define LSM303AGR_STATUS_REG_M     (0x67U) /**< Magnetometer status register (data-ready flags) */
#define LSM303AGR_OUTX_L_REG_M     (0x68U) /**< Magnetic field output, X axis, low byte */
#define LSM303AGR_OUTX_H_REG_M     (0x69U) /**< Magnetic field output, X axis, high byte */
#define LSM303AGR_OUTY_L_REG_M     (0x6AU) /**< Magnetic field output, Y axis, low byte */
#define LSM303AGR_OUTY_H_REG_M     (0x6BU) /**< Magnetic field output, Y axis, high byte */
#define LSM303AGR_OUTZ_L_REG_M     (0x6CU) /**< Magnetic field output, Z axis, low byte */
#define LSM303AGR_OUTZ_H_REG_M     (0x6DU) /**< Magnetic field output, Z axis, high byte */

/** @} */

/**
 * @defgroup LSM303AGR_WHO_AM_I Device Identification
 * @{
 */

#define I_AM_LSM303AGR (0x33U) /**< Expected value of the accelerometer WHO_AM_I register */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_POWER_MODE Accelerometer Power Mode Selection (CTRL_REG1_A)
 * @{
 */

#define LSM303AGR_NORMAL_MODE   ((uint8_t)0x00U) /**< Normal power mode */
#define LSM303AGR_LOWPOWER_MODE ((uint8_t)0x08U) /**< Low-power mode */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_ODR Accelerometer Output Data Rate Selection (CTRL_REG1_A)
 * @{
 */

#define LSM303AGR_ODR_1_HZ       ((uint8_t)0x10U) /**< Output Data Rate = 1 Hz */
#define LSM303AGR_ODR_10_HZ      ((uint8_t)0x20U) /**< Output Data Rate = 10 Hz */
#define LSM303AGR_ODR_25_HZ      ((uint8_t)0x30U) /**< Output Data Rate = 25 Hz */
#define LSM303AGR_ODR_50_HZ      ((uint8_t)0x40U) /**< Output Data Rate = 50 Hz */
#define LSM303AGR_ODR_100_HZ     ((uint8_t)0x50U) /**< Output Data Rate = 100 Hz */
#define LSM303AGR_ODR_200_HZ     ((uint8_t)0x60U) /**< Output Data Rate = 200 Hz */
#define LSM303AGR_ODR_400_HZ     ((uint8_t)0x70U) /**< Output Data Rate = 400 Hz */
#define LSM303AGR_ODR_1620_HZ_LP ((uint8_t)0x80U) /**< Output Data Rate = 1620 Hz, Low Power mode only */
#define LSM303AGR_ODR_1344_HZ    ((uint8_t)0x90U) /**< Output Data Rate = 1344 Hz Normal mode / 5376 Hz Low Power mode */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_AXES Accelerometer Axes Selection (CTRL_REG1_A)
 * @{
 */

#define LSM303AGR_X_ENABLE     ((uint8_t)0x01U) /**< Enable X axis */
#define LSM303AGR_Y_ENABLE     ((uint8_t)0x02U) /**< Enable Y axis */
#define LSM303AGR_Z_ENABLE     ((uint8_t)0x04U) /**< Enable Z axis */
#define LSM303AGR_AXES_ENABLE  ((uint8_t)0x07U) /**< Enable all axes */
#define LSM303AGR_AXES_DISABLE ((uint8_t)0x00U) /**< Disable all axes */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_HR Accelerometer High-Resolution Mode (CTRL_REG4_A)
 * @{
 */

#define LSM303AGR_HR_ENABLE  ((uint8_t)0x08U) /**< Enable high-resolution mode */
#define LSM303AGR_HR_DISABLE ((uint8_t)0x00U) /**< Disable high-resolution mode */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_FULLSCALE Accelerometer Full-Scale Selection (CTRL_REG4_A)
 * @{
 */

#define LSM303AGR_FULLSCALE_2G  ((uint8_t)0x00U) /**< +/-2 g full scale */
#define LSM303AGR_FULLSCALE_4G  ((uint8_t)0x10U) /**< +/-4 g full scale */
#define LSM303AGR_FULLSCALE_8G  ((uint8_t)0x20U) /**< +/-8 g full scale */
#define LSM303AGR_FULLSCALE_16G ((uint8_t)0x30U) /**< +/-16 g full scale */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_SENSITIVITY Accelerometer Full-Scale Sensitivity
 * @{
 */

#define LSM303AGR_ACC_SENSITIVITY_2G  ((uint8_t)1U)  /**< Sensitivity for 2 g full scale [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_4G  ((uint8_t)2U)  /**< Sensitivity for 4 g full scale [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_8G  ((uint8_t)4U)  /**< Sensitivity for 8 g full scale [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_16G ((uint8_t)12U) /**< Sensitivity for 16 g full scale [mg/LSB] */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_BDU Accelerometer Block Data Update (CTRL_REG4_A)
 * @{
 */

#define LSM303AGR_BlockUpdate_Continous ((uint8_t)0x00U) /**< Continuous update of output registers */
#define LSM303AGR_BlockUpdate_Single    ((uint8_t)0x80U) /**< Output registers not updated until MSB and LSB are read */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_ENDIAN Accelerometer Endianness Selection (CTRL_REG4_A)
 * @{
 */

#define LSM303AGR_BLE_LSB ((uint8_t)0x00U) /**< Little Endian: data LSB at lower address */
#define LSM303AGR_BLE_MSB ((uint8_t)0x40U) /**< Big Endian: data MSB at lower address */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_BOOT Accelerometer Boot Mode Selection (CTRL_REG5_A)
 * @{
 */

#define LSM303AGR_BOOT_NORMALMODE   ((uint8_t)0x00U) /**< Normal mode */
#define LSM303AGR_BOOT_REBOOTMEMORY ((uint8_t)0x80U) /**< Reboot memory content */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_HPF_MODE Accelerometer High-Pass Filter Mode (CTRL_REG2_A)
 * @{
 */

#define LSM303AGR_HPM_NORMAL_MODE_RES ((uint8_t)0x00U) /**< Normal mode, resets the filter reference registers */
#define LSM303AGR_HPM_REF_SIGNAL      ((uint8_t)0x40U) /**< Reference signal for filtering */
#define LSM303AGR_HPM_NORMAL_MODE     ((uint8_t)0x80U) /**< Normal mode */
#define LSM303AGR_HPM_AUTORESET_INT   ((uint8_t)0xC0U) /**< Autoreset on interrupt event */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_HPF_CUTOFF Accelerometer High-Pass Filter Cut-Off Frequency (CTRL_REG2_A)
 * @{
 */

#define LSM303AGR_HPFCF_8  ((uint8_t)0x00U) /**< High-pass filter cut-off frequency divider 8 */
#define LSM303AGR_HPFCF_16 ((uint8_t)0x10U) /**< High-pass filter cut-off frequency divider 16 */
#define LSM303AGR_HPFCF_32 ((uint8_t)0x20U) /**< High-pass filter cut-off frequency divider 32 */
#define LSM303AGR_HPFCF_64 ((uint8_t)0x30U) /**< High-pass filter cut-off frequency divider 64 */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_HPF_STATUS Accelerometer High-Pass Filter Enable (CTRL_REG2_A)
 * @{
 */

#define LSM303AGR_HIGHPASSFILTER_DISABLE ((uint8_t)0x00U) /**< High-pass filter disabled */
#define LSM303AGR_HIGHPASSFILTER_ENABLE  ((uint8_t)0x08U) /**< High-pass filter enabled */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_HPF_CLICK_STATUS Accelerometer High-Pass Filter for Click Function (CTRL_REG2_A)
 * @{
 */

#define LSM303AGR_HPF_CLICK_DISABLE ((uint8_t)0x00U) /**< High-pass filter bypassed for click function */
#define LSM303AGR_HPF_CLICK_ENABLE  ((uint8_t)0x04U) /**< High-pass filter enabled for click function */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_HPF_AOI1_STATUS Accelerometer High-Pass Filter for AOI on Interrupt 1 (CTRL_REG2_A)
 * @{
 */

#define LSM303AGR_HPF_AOI1_DISABLE ((uint8_t)0x00U) /**< High-pass filter bypassed for AOI function on interrupt 1 */
#define LSM303AGR_HPF_AOI1_ENABLE  ((uint8_t)0x01U) /**< High-pass filter enabled for AOI function on interrupt 1 */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_HPF_AOI2_STATUS Accelerometer High-Pass Filter for AOI on Interrupt 2 (CTRL_REG2_A)
 * @{
 */

#define LSM303AGR_HPF_AOI2_DISABLE ((uint8_t)0x00U) /**< High-pass filter bypassed for AOI function on interrupt 2 */
#define LSM303AGR_HPF_AOI2_ENABLE  ((uint8_t)0x02U) /**< High-pass filter enabled for AOI function on interrupt 2 */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_IT1_CFG Accelerometer Interrupt 1 Configuration (CTRL_REG3_A)
 * @{
 */

#define LSM303AGR_IT1_CLICK    ((uint8_t)0x80U) /**< Click interrupt routed to INT1 */
#define LSM303AGR_IT1_AOI1     ((uint8_t)0x40U) /**< AOI1 interrupt routed to INT1 */
#define LSM303AGR_IT1_AOI2     ((uint8_t)0x20U) /**< AOI2 interrupt routed to INT1 */
#define LSM303AGR_IT1_DRY1     ((uint8_t)0x10U) /**< Accelerometer data-ready 1 interrupt routed to INT1 */
#define LSM303AGR_IT1_DRY2     ((uint8_t)0x08U) /**< Accelerometer data-ready 2 interrupt routed to INT1 */
#define LSM303AGR_IT1_WTM      ((uint8_t)0x04U) /**< FIFO watermark interrupt routed to INT1 */
#define LSM303AGR_IT1_OVERRUN  ((uint8_t)0x02U) /**< FIFO overrun interrupt routed to INT1 */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_IT2_CFG Accelerometer Interrupt 2 Configuration (CTRL_REG6_A)
 * @{
 */

#define LSM303AGR_IT2_CLICK     ((uint8_t)0x80U) /**< Click interrupt routed to INT2 */
#define LSM303AGR_IT2_INT1      ((uint8_t)0x40U) /**< Interrupt 1 function routed to INT2 */
#define LSM303AGR_IT2_INT2      ((uint8_t)0x20U) /**< Interrupt 2 function routed to INT2 */
#define LSM303AGR_IT2_BOOT      ((uint8_t)0x10U) /**< Boot interrupt routed to INT2 */
#define LSM303AGR_IT2_ACT       ((uint8_t)0x08U) /**< Activity interrupt routed to INT2 */
#define LSM303AGR_IT2_HLACTIVE  ((uint8_t)0x02U) /**< INT2 pin active level: 0 = active high, 1 = active low */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_INT_COMBINATION Accelerometer Interrupt Combination Mode (INT1/2_CFG_A)
 * @{
 */

#define LSM303AGR_OR_COMBINATION  ((uint8_t)0x00U) /**< OR combination of enabled interrupt events */
#define LSM303AGR_AND_COMBINATION ((uint8_t)0x80U) /**< AND combination of enabled interrupt events */
#define LSM303AGR_MOV_RECOGNITION ((uint8_t)0x40U) /**< 6D movement recognition */
#define LSM303AGR_POS_RECOGNITION ((uint8_t)0xC0U) /**< 6D position recognition */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_INT_AXES Accelerometer Interrupt Axes Selection (INT1/2_CFG_A)
 * @{
 */

#define LSM303AGR_Z_HIGH ((uint8_t)0x20U) /**< Z high event enabled for interrupt */
#define LSM303AGR_Z_LOW  ((uint8_t)0x10U) /**< Z low event enabled for interrupt */
#define LSM303AGR_Y_HIGH ((uint8_t)0x08U) /**< Y high event enabled for interrupt */
#define LSM303AGR_Y_LOW  ((uint8_t)0x04U) /**< Y low event enabled for interrupt */
#define LSM303AGR_X_HIGH ((uint8_t)0x02U) /**< X high event enabled for interrupt */
#define LSM303AGR_X_LOW  ((uint8_t)0x01U) /**< X low event enabled for interrupt */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_INT_CLICK Accelerometer Click Interrupt Axes Selection (CLICK_CFG_A)
 * @{
 */

#define LSM303AGR_Z_DOUBLE_CLICK ((uint8_t)0x20U) /**< Z double-click event enabled for interrupt */
#define LSM303AGR_Z_SINGLE_CLICK ((uint8_t)0x10U) /**< Z single-click event enabled for interrupt */
#define LSM303AGR_Y_DOUBLE_CLICK ((uint8_t)0x08U) /**< Y double-click event enabled for interrupt */
#define LSM303AGR_Y_SINGLE_CLICK ((uint8_t)0x04U) /**< Y single-click event enabled for interrupt */
#define LSM303AGR_X_DOUBLE_CLICK ((uint8_t)0x02U) /**< X double-click event enabled for interrupt */
#define LSM303AGR_X_SINGLE_CLICK ((uint8_t)0x01U) /**< X single-click event enabled for interrupt */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_INT1_STATUS Accelerometer INT1 Pin Enable (CTRL_REG3_A)
 * @{
 */

#define LSM303AGR_INT1INTERRUPT_DISABLE ((uint8_t)0x00U) /**< INT1 pin disabled */
#define LSM303AGR_INT1INTERRUPT_ENABLE  ((uint8_t)0x80U) /**< INT1 pin enabled */

/** @} */

/**
 * @defgroup LSM303AGR_ACC_INT1_EDGE Accelerometer INT1 Pin Active Edge (CTRL_REG3_A)
 * @{
 */

#define LSM303AGR_INT1INTERRUPT_LOW_EDGE  ((uint8_t)0x20U) /**< INT1 pin active low */
#define LSM303AGR_INT1INTERRUPT_HIGH_EDGE ((uint8_t)0x00U) /**< INT1 pin active high */

/** @} */

/**
 * @defgroup LSM303AGR_MAG_ODR Magnetometer Output Data Rate Selection (CFG_REG_A_M)
 * @{
 */

#define LSM303AGR_ODR_0_75_HZ ((uint8_t)0x00U) /**< Output Data Rate = 0.75 Hz */
#define LSM303AGR_ODR_1_5_HZ  ((uint8_t)0x04U) /**< Output Data Rate = 1.5 Hz */
#define LSM303AGR_ODR_3_0_HZ  ((uint8_t)0x08U) /**< Output Data Rate = 3 Hz */
#define LSM303AGR_ODR_7_5_HZ  ((uint8_t)0x0CU) /**< Output Data Rate = 7.5 Hz */
#define LSM303AGR_ODR_15_HZ   ((uint8_t)0x10U) /**< Output Data Rate = 15 Hz */
#define LSM303AGR_ODR_30_HZ   ((uint8_t)0x14U) /**< Output Data Rate = 30 Hz */
#define LSM303AGR_ODR_75_HZ   ((uint8_t)0x18U) /**< Output Data Rate = 75 Hz */
#define LSM303AGR_ODR_220_HZ  ((uint8_t)0x1CU) /**< Output Data Rate = 220 Hz */

/** @} */

/**
 * @defgroup LSM303AGR_MAG_FULLSCALE Magnetometer Full Scale
 * @{
 */

#define LSM303AGR_FS_1_3_GA ((uint8_t)0x20U) /**< Full scale = +/-1.3 Gauss */
#define LSM303AGR_FS_1_9_GA ((uint8_t)0x40U) /**< Full scale = +/-1.9 Gauss */
#define LSM303AGR_FS_2_5_GA ((uint8_t)0x60U) /**< Full scale = +/-2.5 Gauss */
#define LSM303AGR_FS_4_0_GA ((uint8_t)0x80U) /**< Full scale = +/-4.0 Gauss */
#define LSM303AGR_FS_4_7_GA ((uint8_t)0xA0U) /**< Full scale = +/-4.7 Gauss */
#define LSM303AGR_FS_5_6_GA ((uint8_t)0xC0U) /**< Full scale = +/-5.6 Gauss */
#define LSM303AGR_FS_8_1_GA ((uint8_t)0xE0U) /**< Full scale = +/-8.1 Gauss */

/** @} */

/**
 * @defgroup LSM303AGR_MAG_SENSITIVITY Magnetometer Full-Scale Sensitivity
 * @{
 */

#define LSM303AGR_M_SENSITIVITY_XY_1_3Ga (1100U) /**< X/Y axes sensitivity for 1.3 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_XY_1_9Ga (855U)  /**< X/Y axes sensitivity for 1.9 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_XY_2_5Ga (670U)  /**< X/Y axes sensitivity for 2.5 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_XY_4Ga   (450U)  /**< X/Y axes sensitivity for 4 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_XY_4_7Ga (400U)  /**< X/Y axes sensitivity for 4.7 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_XY_5_6Ga (330U)  /**< X/Y axes sensitivity for 5.6 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_XY_8_1Ga (230U)  /**< X/Y axes sensitivity for 8.1 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_Z_1_3Ga  (980U)  /**< Z axis sensitivity for 1.3 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_Z_1_9Ga  (760U)  /**< Z axis sensitivity for 1.9 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_Z_2_5Ga  (600U)  /**< Z axis sensitivity for 2.5 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_Z_4Ga    (400U)  /**< Z axis sensitivity for 4 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_Z_4_7Ga  (355U)  /**< Z axis sensitivity for 4.7 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_Z_5_6Ga  (295U)  /**< Z axis sensitivity for 5.6 Ga full scale [LSB/Ga] */
#define LSM303AGR_M_SENSITIVITY_Z_8_1Ga  (205U)  /**< Z axis sensitivity for 8.1 Ga full scale [LSB/Ga] */

/** @} */

/**
 * @defgroup LSM303AGR_MAG_WORKING_MODE Magnetometer Working Mode (CFG_REG_A_M)
 * @{
 */

#define LSM303AGR_CONTINUOUS_CONVERSION ((uint8_t)0x00U) /**< Continuous-conversion mode */
#define LSM303AGR_SINGLE_CONVERSION     ((uint8_t)0x01U) /**< Single-conversion mode */
#define LSM303AGR_SLEEP                 ((uint8_t)0x02U) /**< Sleep mode */

/** @} */

/**
 * @defgroup LSM303AGR_MAG_TEMPSENSOR Magnetometer Temperature Sensor (CFG_REG_A_M)
 * @{
 */

#define LSM303AGR_TEMPSENSOR_ENABLE  ((uint8_t)0x80U) /**< Temperature sensor enabled */
#define LSM303AGR_TEMPSENSOR_DISABLE ((uint8_t)0x00U) /**< Temperature sensor disabled */

/** @} */

/**
 * @defgroup LSM303AGR_MAG_ENDIAN Magnetometer Endianness Selection (CFG_REG_C_M)
 * @{
 */

#define LSM303AGR_MAG_BLE_MSB ((uint8_t)0x08U) /**< Invert high and low parts of the data (Big Endian) */

/** @} */

/* ==== Typedefs ==== */

/**
 * @brief Accelerometer high-pass filter configuration structure (CTRL_REG2_A)
 */
typedef struct
{
    uint8_t HighPassFilter_Mode_Selection;   /**< Internal filter mode (HPM) */
    uint8_t HighPassFilter_CutOff_Frequency; /**< High-pass filter cut-off frequency (DFC) */
    uint8_t HighPassFilter_AOI1;             /**< High-pass filter enable for AOI function on interrupt 1 (HPIS1) */
    uint8_t HighPassFilter_AOI2;             /**< High-pass filter enable for AOI function on interrupt 2 (HPIS2) */
    uint8_t HighPassFilter_Data_Sel;         /**< Filter data selection routed to output registers and FIFO (FDS) */
    uint8_t HighPassFilter_Stat;             /**< High-pass filter enable for click function (HPCLICK) */
} ACCELERO_FilterConfig_t;

/**
 * @brief Accelerometer interrupt 1 configuration structure (CTRL_REG3_A). Default values are all 0
 */
typedef struct
{
    uint8_t Click_En;    /**< Click interrupt on INT1 pin */
    uint8_t AOI1_En;     /**< AOI1 interrupt on INT1 pin */
    uint8_t AOI2_En;     /**< AOI2 interrupt on INT1 pin */
    uint8_t DRY1_En;     /**< Data-ready 1 interrupt on INT1 pin */
    uint8_t DRY2_En;     /**< Data-ready 2 interrupt on INT1 pin */
    uint8_t WTM_En;      /**< FIFO watermark interrupt on INT1 pin */
    uint8_t Overrun_En;  /**< FIFO overrun interrupt on INT1 pin */
} ACCELERO_IT1Config_t;

/**
 * @brief Accelerometer interrupt 2 configuration structure (CTRL_REG6_A). Default values are all 0
 */
typedef struct
{
    uint8_t Click_En;               /**< Click interrupt on INT2 pin */
    uint8_t IT1_En;                 /**< Interrupt 1 function enable on INT2 pin */
    uint8_t IT2_En;                 /**< Interrupt 2 function enable on INT2 pin */
    uint8_t Boot_En;                /**< Boot interrupt enable on INT2 pin */
    uint8_t Activity_En;            /**< Activity interrupt enable on INT2 pin */
    uint8_t High_Latency_Active_En; /**< INT2 pin active level: 0 = active high, 1 = active low */
} ACCELERO_IT2Config_t;

/**
 * @brief Accelerometer INT1 interrupt configuration structure (INT1_CFG_A). Default values are all 0
 */
typedef struct
{
    uint8_t AOI;  /**< AND/OR combination of enabled interrupt events */
    uint8_t SixD; /**< 6-direction detection function enable */
    uint8_t ZHIE; /**< Z high event enable for interrupt */
    uint8_t ZLIE; /**< Z low event enable for interrupt */
    uint8_t YHIE; /**< Y high event enable for interrupt */
    uint8_t YLIE; /**< Y low event enable for interrupt */
    uint8_t XHIE; /**< X high event enable for interrupt */
    uint8_t XLIE; /**< X low event enable for interrupt */

} ACCELERO_INT1Config_t;

/**
 * @brief Accelerometer INT2 interrupt configuration structure (INT2_CFG_A). Default values are all 0
 */
typedef struct
{
    uint8_t AOI;  /**< AND/OR combination of enabled interrupt events */
    uint8_t SixD; /**< 6-direction detection function enable */
    uint8_t ZDIE; /**< Z double-click event enable for interrupt */
    uint8_t ZSIE; /**< Z single-click event enable for interrupt */
    uint8_t YDIE; /**< Y double-click event enable for interrupt */
    uint8_t YSIE; /**< Y single-click event enable for interrupt */
    uint8_t XDIE; /**< X double-click event enable for interrupt */
    uint8_t XSIE; /**< X single-click event enable for interrupt */

} ACCELERO_INT2Config_t;

/**
 * @brief Accelerometer configuration structure
 */
typedef struct
{
    uint8_t Power_Mode;          /**< Power-down/Normal mode */
    uint8_t AccOutput_DataRate;  /**< Output Data Rate */
    uint8_t Axes_Enable;         /**< Axes enable */
    uint8_t High_Resolution;     /**< High-resolution mode enable/disable */
    uint8_t BlockData_Update;    /**< Block Data Update */
    uint8_t Endianness;          /**< Endian data selection */
    uint8_t AccFull_Scale;       /**< Full scale selection */
    uint8_t Communication_Mode;  /**< Communication interface mode selection */

    ACCELERO_FilterConfig_t FilterStruct; /**< High-pass filter configuration */

    ACCELERO_IT1Config_t IT1Struct; /**< Interrupt 1 configuration */
    ACCELERO_IT2Config_t IT2Struct; /**< Interrupt 2 configuration */

    ACCELERO_INT1Config_t INT1Struct; /**< INT1 interrupt configuration */
    ACCELERO_INT2Config_t INT2Struct; /**< INT2 interrupt configuration */

} ACCELERO_t;

/**
 * @brief Magnetometer configuration structure
 */
typedef struct
{
    uint8_t Comp_Temp_En; /**< Enable temperature compensation. Default value: 0 */
    uint8_t Mag_ODR;      /**< Output Data Rate configuration. Default value: 00 (10 Hz ODR) */
    uint8_t INT_MAG_Pin;  /**< If 1, the INTERRUPT signal (INT bit inside INT_SOURCE_REG_M) is driven on INT_MAG_PIN */
    uint8_t I2C_DIS;      /**< If 1, the I2C interface is disabled and only the SPI interface can be used. Default value: 0 */
    uint8_t Self_test;    /**< Self-test enabled if 1. Default value: 0 */
    uint8_t INT_MAG;      /**< If 1, the DRDY pin is configured as digital output */
} MAGNETO_Init_t;

/**
 * @brief LSM303AGR sensor handle
 */
typedef struct
{
    I2C_HandleTypeDef *p_hi2c; /**< I2C handle instance used to communicate with the sensor */

    ACCELERO_t Accelero;    /**< Accelerometer configuration */
    MAGNETO_Init_t Magneto; /**< Magnetometer configuration */

} LSM303AGR_t;

/* ==== Functions prototypes ==== */

/**
 * @defgroup LSM303AGR_FUNC_INIT Initialization Functions
 * @{
 */

/**
 * @brief Sensor start-up sequence: configure the magnetometer ODR/mode and the accelerometer ODR
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM303AGR_Init(LSM303AGR_t *sensor);

/** @} */

/**
 * @defgroup LSM303AGR_FUNC_ACC Accelerometer Functions
 * @{
 */

/**
 * @brief Read accelerometer ID
 *
 * @param sensor Handle of the sensor in use
 * @param pBuffer Pointer to the buffer to store the read ID
 * @return error_t
 */
error_t LSM303AGR_AccReadID(LSM303AGR_t *sensor, uint8_t *pBuffer);

/**
 * @brief Configure the accelerometer high-pass filter
 *
 * @param sensor Handle of the sensor in use
 * @param pBuffer Pointer to the buffer used to read back CTRL_REG2_A before updating it
 * @param FilterStruct Desired high-pass filter configuration
 * @return error_t
 */
error_t LSM303AGR_AccFilterConfig(LSM303AGR_t *sensor, uint8_t *pBuffer, ACCELERO_FilterConfig_t FilterStruct);

/**
 * @brief Read raw X, Y and Z acceleration values
 *
 * @param sensor Handle of the sensor in use
 * @param pnRawData Pointer to the buffer to store the raw X, Y, Z acceleration values
 * @return error_t
 */
error_t LSM303AGR_AccReadXYZ(LSM303AGR_t *sensor, int16_t *pnRawData);

/**
 * @brief Convert raw acceleration values to mg using the current sensitivity
 *
 * @param sensor Handle of the sensor in use
 * @param pnRawData Pointer to the buffer holding the raw X, Y, Z acceleration values
 * @param pData Pointer to the buffer to store the converted X, Y, Z acceleration values in mg
 * @return error_t
 */
error_t LSM303AGR_AccConvertXYZ(LSM303AGR_t *sensor, const int16_t *pnRawData, int16_t *pData);

/**
 * @brief Enable accelerometer interrupt 1 as configured in the sensor handle
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM303AGR_AccIT1Enable(LSM303AGR_t *sensor);

/**
 * @brief Disable accelerometer interrupt 1 as configured in the sensor handle
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM303AGR_AccIT1Disable(LSM303AGR_t *sensor);

/**
 * @brief Enable accelerometer interrupt 2 as configured in the sensor handle
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM303AGR_AccIT2Enable(LSM303AGR_t *sensor);

/**
 * @brief Disable accelerometer interrupt 2 as configured in the sensor handle
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM303AGR_AccIT2Disable(LSM303AGR_t *sensor);

/**
 * @brief Enable the INT1 interrupt events configured in the sensor handle (INT1_CFG_A)
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM303AGR_AccINT1InterruptEnable(LSM303AGR_t *sensor);

/**
 * @brief Disable the INT1 interrupt events configured in the sensor handle (INT1_CFG_A)
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM303AGR_AccINT1InterruptDisable(LSM303AGR_t *sensor);

/**
 * @brief Enable the INT2 interrupt events configured in the sensor handle (INT2_CFG_A)
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM303AGR_AccINT2InterruptEnable(LSM303AGR_t *sensor);

/**
 * @brief Disable the INT2 interrupt events configured in the sensor handle (INT2_CFG_A)
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM303AGR_AccINT2InterruptDisable(LSM303AGR_t *sensor);

/** @} */

/**
 * @defgroup LSM303AGR_FUNC_MAG Magnetometer Functions
 * @{
 */

/**
 * @brief Initialize the magnetometer using the configuration in the sensor handle
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM303AGR_MagInit(LSM303AGR_t *sensor);

/**
 * @brief Read magnetometer ID
 *
 * @param sensor Handle of the sensor in use
 * @param pBuffer Pointer to the buffer to store the read ID
 * @return error_t
 */
error_t LSM303AGR_MagReadID(LSM303AGR_t *sensor, uint8_t *pBuffer);

/**
 * @brief Read the magnetometer data-status register
 *
 * @param sensor Handle of the sensor in use
 * @param pBuffer Pointer to the buffer to store the read status
 * @return error_t
 */
error_t LSM303AGR_MagGetDataStatus(LSM303AGR_t *sensor, uint8_t *pBuffer);

/**
 * @brief Read X, Y and Z magnetometer values
 *
 * @param sensor Handle of the sensor in use
 * @param pData Pointer to the buffer to store the X, Y, Z magnetic field values
 * @return error_t
 */
error_t LSM303AGR_MagReadXYZ(LSM303AGR_t *sensor, int16_t *pData);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LSM303AGR_H */
