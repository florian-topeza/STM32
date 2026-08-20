/**
 * @file        lsm6dsl.h
 * @author      Florian Topeza & Merlin Kooshmanian
 * @brief       Header file for LSM6DSL Sensor driver
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
#ifndef LSM6DSL_H
#define LSM6DSL_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==== Includes ==== */

#include <stdint.h>
#include <stdio.h>

// Include the HAL of the STM32 microcontroller in use
#include "stm32h7xx_hal.h"

#include "errors.h"

/* ==== Defines ==== */

/* Debug LSM6DSL */
#ifdef DEBUG_LSM6DSL
#define logs_lsm6dsl(...) printf(__VA_ARGS__)
#else
#define logs_lsm6dsl(...)
#endif

/**
 * @defgroup LSM6DSL_SPI_MODE SPI Interface Mode
 * @{
 */

#define LSM6DSL_SPI_3WIRE /**< Define to use the SPI interface in 3-wire mode (SDO/SDI shared) */

/** @} */

/**
 * @defgroup LSM6DSL_I2C I2C Configuration Constants
 * @{
 */

#define LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW  (0xD4U) /**< 7-bit I2C address, SDO/SA0 pin low (SAD[0] = 0) */
#define LSM6DSL_ACC_GYRO_I2C_ADDRESS_HIGH (0xD6U) /**< 7-bit I2C address, SDO/SA0 pin high (SAD[0] = 1) */

/** @} */

/**
 * @defgroup LSM6DSL_WHO_AM_I Device Identification
 * @{
 */

#define LSM6DSL_ACC_GYRO_WHO_AM_I (0x6AU) /**< Expected value of the WHO_AM_I register (address 0x0F) */

/** @} */

/**
 * @defgroup LSM6DSL_REG Register Address Map
 * @{
 */

#define LSM6DSL_ACC_GYRO_FUNC_CFG_ACCESS   (0x01U) /**< Embedded functions configuration register access */

#define LSM6DSL_ACC_GYRO_SENSOR_SYNC_TIME  (0x04U) /**< External sensor sync time frame register */
#define LSM6DSL_ACC_GYRO_SENSOR_RES_RATIO  (0x05U) /**< External sensor sync resolution ratio register */

#define LSM6DSL_ACC_GYRO_FIFO_CTRL1        (0x06U) /**< FIFO control register 1 (FIFO threshold) */
#define LSM6DSL_ACC_GYRO_FIFO_CTRL2        (0x07U) /**< FIFO control register 2 (FIFO threshold, decimation) */
#define LSM6DSL_ACC_GYRO_FIFO_CTRL3        (0x08U) /**< FIFO control register 3 (gyro/accel decimation) */
#define LSM6DSL_ACC_GYRO_FIFO_CTRL4        (0x09U) /**< FIFO control register 4 (decimation, ODR) */
#define LSM6DSL_ACC_GYRO_FIFO_CTRL5        (0x0AU) /**< FIFO control register 5 (FIFO ODR, FIFO mode) */

#define LSM6DSL_ACC_GYRO_DRDY_PULSE_CFG_G  (0x0BU) /**< Data-ready pulsed mode configuration register */
#define LSM6DSL_ACC_GYRO_INT1_CTRL         (0x0DU) /**< INT1 pin control register */
#define LSM6DSL_ACC_GYRO_INT2_CTRL         (0x0EU) /**< INT2 pin control register */
#define LSM6DSL_ACC_GYRO_WHO_AM_I_REG      (0x0FU) /**< WHO_AM_I register address */
#define LSM6DSL_ACC_GYRO_CTRL1_XL          (0x10U) /**< Accelerometer control register 1 (ODR, full scale) */
#define LSM6DSL_ACC_GYRO_CTRL2_G           (0x11U) /**< Gyroscope control register 2 (ODR, full scale) */
#define LSM6DSL_ACC_GYRO_CTRL3_C           (0x12U) /**< Control register 3 (BDU, IF_INC, SW_RESET, ...) */
#define LSM6DSL_ACC_GYRO_CTRL4_C           (0x13U) /**< Control register 4 */
#define LSM6DSL_ACC_GYRO_CTRL5_C           (0x14U) /**< Control register 5 (self-test) */
#define LSM6DSL_ACC_GYRO_CTRL6_C           (0x15U) /**< Accelerometer control register 6 (low-power, filtering) */
#define LSM6DSL_ACC_GYRO_CTRL7_G           (0x16U) /**< Gyroscope control register 7 (high-perf, low-power) */
#define LSM6DSL_ACC_GYRO_CTRL8_XL          (0x17U) /**< Accelerometer control register 8 (filtering) */
#define LSM6DSL_ACC_GYRO_CTRL9_XL          (0x18U) /**< Accelerometer control register 9 (axis enable) */
#define LSM6DSL_ACC_GYRO_CTRL10_C          (0x19U) /**< Control register 10 (gyro axis enable, embedded functions) */

#define LSM6DSL_ACC_GYRO_MASTER_CONFIG     (0x1AU) /**< Sensor hub (I2C master) configuration register */
#define LSM6DSL_ACC_GYRO_WAKE_UP_SRC       (0x1BU) /**< Wake-up interrupt source register */
#define LSM6DSL_ACC_GYRO_TAP_SRC           (0x1CU) /**< Tap source register */
#define LSM6DSL_ACC_GYRO_D6D_SRC           (0x1DU) /**< Orientation (6D/4D) source register */
#define LSM6DSL_ACC_GYRO_STATUS_REG        (0x1EU) /**< Status register (data-ready flags) */

#define LSM6DSL_ACC_GYRO_OUT_TEMP_L        (0x20U) /**< Temperature output, low byte */
#define LSM6DSL_ACC_GYRO_OUT_TEMP_H        (0x21U) /**< Temperature output, high byte */
#define LSM6DSL_ACC_GYRO_OUTX_L_G          (0x22U) /**< Angular rate output, X axis, low byte */
#define LSM6DSL_ACC_GYRO_OUTX_H_G          (0x23U) /**< Angular rate output, X axis, high byte */
#define LSM6DSL_ACC_GYRO_OUTY_L_G          (0x24U) /**< Angular rate output, Y axis, low byte */
#define LSM6DSL_ACC_GYRO_OUTY_H_G          (0x25U) /**< Angular rate output, Y axis, high byte */
#define LSM6DSL_ACC_GYRO_OUTZ_L_G          (0x26U) /**< Angular rate output, Z axis, low byte */
#define LSM6DSL_ACC_GYRO_OUTZ_H_G          (0x27U) /**< Angular rate output, Z axis, high byte */
#define LSM6DSL_ACC_GYRO_OUTX_L_XL         (0x28U) /**< Linear acceleration output, X axis, low byte */
#define LSM6DSL_ACC_GYRO_OUTX_H_XL         (0x29U) /**< Linear acceleration output, X axis, high byte */
#define LSM6DSL_ACC_GYRO_OUTY_L_XL         (0x2AU) /**< Linear acceleration output, Y axis, low byte */
#define LSM6DSL_ACC_GYRO_OUTY_H_XL         (0x2BU) /**< Linear acceleration output, Y axis, high byte */
#define LSM6DSL_ACC_GYRO_OUTZ_L_XL         (0x2CU) /**< Linear acceleration output, Z axis, low byte */
#define LSM6DSL_ACC_GYRO_OUTZ_H_XL         (0x2DU) /**< Linear acceleration output, Z axis, high byte */
#define LSM6DSL_ACC_GYRO_SENSORHUB1_REG    (0x2EU) /**< Sensor hub output register 1 */
#define LSM6DSL_ACC_GYRO_SENSORHUB2_REG    (0x2FU) /**< Sensor hub output register 2 */
#define LSM6DSL_ACC_GYRO_SENSORHUB3_REG    (0x30U) /**< Sensor hub output register 3 */
#define LSM6DSL_ACC_GYRO_SENSORHUB4_REG    (0x31U) /**< Sensor hub output register 4 */
#define LSM6DSL_ACC_GYRO_SENSORHUB5_REG    (0x32U) /**< Sensor hub output register 5 */
#define LSM6DSL_ACC_GYRO_SENSORHUB6_REG    (0x33U) /**< Sensor hub output register 6 */
#define LSM6DSL_ACC_GYRO_SENSORHUB7_REG    (0x34U) /**< Sensor hub output register 7 */
#define LSM6DSL_ACC_GYRO_SENSORHUB8_REG    (0x35U) /**< Sensor hub output register 8 */
#define LSM6DSL_ACC_GYRO_SENSORHUB9_REG    (0x36U) /**< Sensor hub output register 9 */
#define LSM6DSL_ACC_GYRO_SENSORHUB10_REG   (0x37U) /**< Sensor hub output register 10 */
#define LSM6DSL_ACC_GYRO_SENSORHUB11_REG   (0x38U) /**< Sensor hub output register 11 */
#define LSM6DSL_ACC_GYRO_SENSORHUB12_REG   (0x39U) /**< Sensor hub output register 12 */
#define LSM6DSL_ACC_GYRO_FIFO_STATUS1      (0x3AU) /**< FIFO status register 1 (FIFO fill level) */
#define LSM6DSL_ACC_GYRO_FIFO_STATUS2      (0x3BU) /**< FIFO status register 2 (FIFO fill level, flags) */
#define LSM6DSL_ACC_GYRO_FIFO_STATUS3      (0x3CU) /**< FIFO status register 3 (pattern) */
#define LSM6DSL_ACC_GYRO_FIFO_STATUS4      (0x3DU) /**< FIFO status register 4 (pattern) */
#define LSM6DSL_ACC_GYRO_FIFO_DATA_OUT_L   (0x3EU) /**< FIFO data output, low byte */
#define LSM6DSL_ACC_GYRO_FIFO_DATA_OUT_H   (0x3FU) /**< FIFO data output, high byte */
#define LSM6DSL_ACC_GYRO_TIMESTAMP0_REG    (0x40U) /**< Timestamp register, byte 0 */
#define LSM6DSL_ACC_GYRO_TIMESTAMP1_REG    (0x41U) /**< Timestamp register, byte 1 */
#define LSM6DSL_ACC_GYRO_TIMESTAMP2_REG    (0x42U) /**< Timestamp register, byte 2 */

#define LSM6DSL_ACC_GYRO_TIMESTAMP_L       (0x49U) /**< Timestamp output, low byte */
#define LSM6DSL_ACC_GYRO_TIMESTAMP_H       (0x4AU) /**< Timestamp output, high byte */

#define LSM6DSL_ACC_GYRO_STEP_COUNTER_L    (0x4BU) /**< Step counter output, low byte */
#define LSM6DSL_ACC_GYRO_STEP_COUNTER_H    (0x4CU) /**< Step counter output, high byte */

#define LSM6DSL_ACC_GYRO_SENSORHUB13_REG   (0x4DU) /**< Sensor hub output register 13 */
#define LSM6DSL_ACC_GYRO_SENSORHUB14_REG   (0x4EU) /**< Sensor hub output register 14 */
#define LSM6DSL_ACC_GYRO_SENSORHUB15_REG   (0x4FU) /**< Sensor hub output register 15 */
#define LSM6DSL_ACC_GYRO_SENSORHUB16_REG   (0x50U) /**< Sensor hub output register 16 */
#define LSM6DSL_ACC_GYRO_SENSORHUB17_REG   (0x51U) /**< Sensor hub output register 17 */
#define LSM6DSL_ACC_GYRO_SENSORHUB18_REG   (0x52U) /**< Sensor hub output register 18 */

#define LSM6DSL_ACC_GYRO_FUNC_SRC          (0x53U) /**< Embedded function source register */
#define LSM6DSL_ACC_GYRO_TAP_CFG1          (0x58U) /**< Tap/pedometer configuration register 1 */
#define LSM6DSL_ACC_GYRO_TAP_THS_6D        (0x59U) /**< Tap threshold / 6D orientation threshold register */
#define LSM6DSL_ACC_GYRO_INT_DUR2          (0x5AU) /**< Tap recognition duration register */
#define LSM6DSL_ACC_GYRO_WAKE_UP_THS       (0x5BU) /**< Wake-up threshold register */
#define LSM6DSL_ACC_GYRO_WAKE_UP_DUR       (0x5CU) /**< Wake-up duration register */
#define LSM6DSL_ACC_GYRO_FREE_FALL         (0x5DU) /**< Free-fall configuration register */
#define LSM6DSL_ACC_GYRO_MD1_CFG           (0x5EU) /**< Function routing to INT1 register */
#define LSM6DSL_ACC_GYRO_MD2_CFG           (0x5FU) /**< Function routing to INT2 register */

#define LSM6DSL_ACC_GYRO_OUT_MAG_RAW_X_L   (0x66U) /**< External magnetometer raw output, X axis, low byte */
#define LSM6DSL_ACC_GYRO_OUT_MAG_RAW_X_H   (0x67U) /**< External magnetometer raw output, X axis, high byte */
#define LSM6DSL_ACC_GYRO_OUT_MAG_RAW_Y_L   (0x68U) /**< External magnetometer raw output, Y axis, low byte */
#define LSM6DSL_ACC_GYRO_OUT_MAG_RAW_Y_H   (0x69U) /**< External magnetometer raw output, Y axis, high byte */
#define LSM6DSL_ACC_GYRO_OUT_MAG_RAW_Z_L   (0x6AU) /**< External magnetometer raw output, Z axis, low byte */
#define LSM6DSL_ACC_GYRO_OUT_MAG_RAW_Z_H   (0x6BU) /**< External magnetometer raw output, Z axis, high byte */

#define LSM6DSL_ACC_GYRO_X_OFS_USR         (0x73U) /**< Accelerometer user offset correction, X axis */
#define LSM6DSL_ACC_GYRO_Y_OFS_USR         (0x74U) /**< Accelerometer user offset correction, Y axis */
#define LSM6DSL_ACC_GYRO_Z_OFS_USR         (0x75U) /**< Accelerometer user offset correction, Z axis */

/** @} */

/**
 * @defgroup LSM6DSL_ACC_FULLSCALE Accelerometer Full-Scale Selection (CTRL1_XL)
 * @{
 */

#define LSM6DSL_ACC_FULLSCALE_2G  ((uint8_t)0x00U) /**< 2 g full scale */
#define LSM6DSL_ACC_FULLSCALE_4G  ((uint8_t)0x08U) /**< 4 g full scale */
#define LSM6DSL_ACC_FULLSCALE_8G  ((uint8_t)0x0CU) /**< 8 g full scale */
#define LSM6DSL_ACC_FULLSCALE_16G ((uint8_t)0x04U) /**< 16 g full scale */

/** @} */

/**
 * @defgroup LSM6DSL_ACC_SENSITIVITY Accelerometer Full-Scale Sensitivity
 * @{
 */

#define LSM6DSL_ACC_SENSITIVITY_2G  ((float)0.061f) /**< Sensitivity for 2 g full scale [mg/LSB] */
#define LSM6DSL_ACC_SENSITIVITY_4G  ((float)0.122f) /**< Sensitivity for 4 g full scale [mg/LSB] */
#define LSM6DSL_ACC_SENSITIVITY_8G  ((float)0.244f) /**< Sensitivity for 8 g full scale [mg/LSB] */
#define LSM6DSL_ACC_SENSITIVITY_16G ((float)0.488f) /**< Sensitivity for 16 g full scale [mg/LSB] */

/** @} */

/**
 * @defgroup LSM6DSL_ACC_POWER_MODE Accelerometer Power Mode Selection (CTRL6_C)
 * @{
 */

#define LSM6DSL_ACC_GYRO_LP_XL_DISABLED ((uint8_t)0x00U) /**< Accelerometer low-power mode disabled */
#define LSM6DSL_ACC_GYRO_LP_XL_ENABLED  ((uint8_t)0x10U) /**< Accelerometer low-power mode enabled */

/** @} */

/**
 * @defgroup LSM6DSL_ODR Output Data Rate Selection (CTRL1_XL / CTRL2_G)
 * @{
 */

#define LSM6DSL_ODR_BITPOSITION ((uint8_t)0xF0U) /**< Output Data Rate bit position */
#define LSM6DSL_ODR_POWER_DOWN  ((uint8_t)0x00U) /**< Power-down mode */
#define LSM6DSL_ODR_13Hz        ((uint8_t)0x10U) /**< 13 Hz, Low Power mode */
#define LSM6DSL_ODR_26Hz        ((uint8_t)0x20U) /**< 26 Hz, Low Power mode */
#define LSM6DSL_ODR_52Hz        ((uint8_t)0x30U) /**< 52 Hz, Low Power mode */
#define LSM6DSL_ODR_104Hz       ((uint8_t)0x40U) /**< 104 Hz, Normal mode */
#define LSM6DSL_ODR_208Hz       ((uint8_t)0x50U) /**< 208 Hz, Normal mode */
#define LSM6DSL_ODR_416Hz       ((uint8_t)0x60U) /**< 416 Hz, High Performance mode */
#define LSM6DSL_ODR_833Hz       ((uint8_t)0x70U) /**< 833 Hz, High Performance mode */
#define LSM6DSL_ODR_1660Hz      ((uint8_t)0x80U) /**< 1660 Hz, High Performance mode */
#define LSM6DSL_ODR_3330Hz      ((uint8_t)0x90U) /**< 3330 Hz, High Performance mode */
#define LSM6DSL_ODR_6660Hz      ((uint8_t)0xA0U) /**< 6660 Hz, High Performance mode */

/** @} */

/**
 * @defgroup LSM6DSL_GYRO_FULLSCALE Gyroscope Full-Scale Selection (CTRL2_G)
 * @{
 */

#define LSM6DSL_GYRO_FS_245  ((uint8_t)0x00U) /**< 245 dps full scale */
#define LSM6DSL_GYRO_FS_500  ((uint8_t)0x04U) /**< 500 dps full scale */
#define LSM6DSL_GYRO_FS_1000 ((uint8_t)0x08U) /**< 1000 dps full scale */
#define LSM6DSL_GYRO_FS_2000 ((uint8_t)0x0CU) /**< 2000 dps full scale */

/** @} */

/**
 * @defgroup LSM6DSL_GYRO_SENSITIVITY Gyroscope Full-Scale Sensitivity
 * @{
 */

#define LSM6DSL_GYRO_SENSITIVITY_245DPS  ((float)8.750f) /**< Sensitivity for 245 dps full scale [mdps/LSB] */
#define LSM6DSL_GYRO_SENSITIVITY_500DPS  ((float)17.50f) /**< Sensitivity for 500 dps full scale [mdps/LSB] */
#define LSM6DSL_GYRO_SENSITIVITY_1000DPS ((float)35.00f) /**< Sensitivity for 1000 dps full scale [mdps/LSB] */
#define LSM6DSL_GYRO_SENSITIVITY_2000DPS ((float)70.00f) /**< Sensitivity for 2000 dps full scale [mdps/LSB] */

/** @} */

/**
 * @defgroup LSM6DSL_GYRO_POWER_MODE Gyroscope Power Mode Selection (CTRL7_G)
 * @{
 */

#define LSM6DSL_ACC_GYRO_LP_G_DISABLED ((uint8_t)0x00U) /**< Gyroscope low-power mode disabled */
#define LSM6DSL_ACC_GYRO_LP_G_ENABLED  ((uint8_t)0x80U) /**< Gyroscope low-power mode enabled */

/** @} */

/**
 * @defgroup LSM6DSL_BDU Block Data Update Selection (CTRL3_C)
 * @{
 */

#define LSM6DSL_BDU_CONTINUOS    ((uint8_t)0x00U) /**< Continuous update of output registers */
#define LSM6DSL_BDU_BLOCK_UPDATE ((uint8_t)0x40U) /**< Output registers not updated until MSB and LSB are read */

/** @} */

/**
 * @defgroup LSM6DSL_IF_INC Register Address Auto-Increment Selection (CTRL3_C)
 * @{
 */

#define LSM6DSL_ACC_GYRO_IF_INC_DISABLED ((uint8_t)0x00U) /**< Auto-increment disabled */
#define LSM6DSL_ACC_GYRO_IF_INC_ENABLED  ((uint8_t)0x04U) /**< Register address auto-incremented on multi-byte access */

/** @} */

/* ==== Typedefs ==== */

/**
 * @brief Sensor configuration structure
 */
typedef struct
{
    uint8_t BDU;    /**< Block Data Update. Default value: 0 */
    uint8_t IF_INC; /**< Register address auto-increment on multi-byte access (I2C or SPI). Default value: 1 */

} Sensor_Config_t;

/**
 * @brief Accelerometer configuration structure
 */
typedef struct
{
    uint8_t ODR;        /**< Output Data Rate selection. Default value: 0000 */
    uint8_t Full_Scale; /**< Full scale selection. Default value: 00 */

} ACCELERO_Config_t;

/**
 * @brief Gyroscope configuration structure
 */
typedef struct
{
    uint8_t ODR;        /**< Output Data Rate selection. Default value: 0000 */
    uint8_t Full_Scale; /**< Full scale selection. Default value: 00 */

} GYRO_Config_t;

/**
 * @brief LSM6DSL sensor handle
 */
typedef struct
{
    SPI_HandleTypeDef *p_hspi;  /**< SPI handle instance used to communicate with the sensor */
    uint32_t cs_pin;            /**< Chip select GPIO pin number */
    GPIO_TypeDef *cs_gpio_port; /**< Chip select GPIO port */

    Sensor_Config_t Config;     /**< General sensor configuration */
    ACCELERO_Config_t Accelero; /**< Accelerometer configuration */
    GYRO_Config_t Gyro;         /**< Gyroscope configuration */

} LSM6DSL_t;

/* ==== Functions prototypes ==== */

/**
 * @defgroup LSM6DSL_FUNC_IO IO / Register Access Functions
 * @{
 */

/**
 * @brief Initialize the LSM6DSL sensor
 *
 * @param sensor Handle of the LSM6DSL sensor
 * @return error_t
 */
error_t LSM6DSL_IO_Init(LSM6DSL_t *sensor);

/**
 * @brief Write a command to the sensor
 *
 * @param sensor Handle of the sensor in use
 * @param pBuffer Pointer to the buffer to store the data to write
 * @param WriteAddr Write address
 * @param NumByteToWrite Number of bytes to write
 * @return error_t
 */
error_t LSM6DSL_Write(LSM6DSL_t *sensor, uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);

/**
 * @brief Read from the sensor
 *
 * @param sensor Handle of the sensor in use
 * @param pBuffer Pointer to the buffer to store the data to read
 * @param ReadAddr Read address
 * @param NumByteToRead Number of bytes to read
 * @return error_t
 */
error_t LSM6DSL_Read(LSM6DSL_t *sensor, uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);

/**
 * @brief Read sensor ID
 *
 * @param sensor Handle of the sensor in use
 * @param pBuffer Pointer to the buffer to store the read ID
 * @return error_t
 */
error_t LSM6DSL_ReadID(LSM6DSL_t *sensor, uint8_t *pBuffer);

/** @} */

/**
 * @defgroup LSM6DSL_FUNC_ACC Accelerometer Functions
 * @{
 */

/**
 * @brief Initialize accelerometer
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM6DSL_AccInit(LSM6DSL_t *sensor);

/**
 * @brief Deinitialize accelerometer
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM6DSL_AccDeInit(LSM6DSL_t *sensor);

/**
 * @brief Enter accelerometer low power mode
 *
 * @param sensor Handle of the sensor in use
 * @param set_low_pwr Low power mode setting
 * @return error_t
 */
error_t LSM6DSL_AccLowPower(LSM6DSL_t *sensor, uint16_t set_low_pwr);

/**
 * @brief Read X, Y and Z acceleration values
 *
 * @param sensor Handle of the sensor in use
 * @param pData Pointer to the buffer to store the X, Y, Z acceleration values in mg
 * @return error_t
 */
error_t LSM6DSL_AccReadXYZ(LSM6DSL_t *sensor, int16_t *pData);

/** @} */

/**
 * @defgroup LSM6DSL_FUNC_GYRO Gyroscope Functions
 * @{
 */

/**
 * @brief Initialize gyroscope
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM6DSL_GyroInit(LSM6DSL_t *sensor);

/**
 * @brief Deinitialize gyroscope
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t LSM6DSL_GyroDeInit(LSM6DSL_t *sensor);

/**
 * @brief Enter gyroscope low power mode
 *
 * @param sensor Handle of the sensor in use
 * @param set_low_pwr Low power mode setting
 * @return error_t
 */
error_t LSM6DSL_GyroLowPower(LSM6DSL_t *sensor, uint16_t set_low_pwr);

/**
 * @brief Read raw X, Y and Z angular rate values
 *
 * @param sensor Handle of the sensor in use
 * @param pBuffer Pointer to the buffer to store the raw X, Y, Z angular rate values
 * @return error_t
 */
error_t LSM6DSL_GyroReadXYZAngRate(LSM6DSL_t *sensor, uint8_t *pBuffer);

/**
 * @brief Convert raw angular rate values to dps using the current sensitivity
 *
 * @param sensor Handle of the sensor in use
 * @param buffer Pointer to the buffer holding the raw X, Y, Z angular rate values
 * @param pfData Pointer to the buffer to store the converted X, Y, Z angular rate values in dps
 * @return error_t
 */
error_t LSM6DSL_GyroConvertAngRate(LSM6DSL_t *sensor, uint8_t *buffer, float *pfData);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LSM6DSL_H */