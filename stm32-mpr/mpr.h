/**
 * @file        mpr.h
 * @author      Florian Topeza & Merlin Kooshmanian
 * @brief       Header file for MPR Pressure Sensor driver
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
#ifndef MPR_H
#define MPR_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==== Includes ==== */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Include the HAL of the STM32 microcontroller in use
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_def.h"

#include "errors.h"
#include "console.h"

/* ==== Defines ==== */

/* Debug MPR */
#ifdef DEBUG_MPR
#include "console.h"
#define logs_mpr(...) printf(__VA_ARGS__)
#else
#define logs_mpr(...)
#endif

/**
 * @defgroup MPR_I2C I2C Configuration Constants
 * @{
 */

#define MPR_I2C_ADDRESS	            (0x30U)	    /**< 7-bit address shifted to the left */
#define MPR_PRESSURE_READ_CMD_SIZE	(3U)		/**< MPR Pressure read command size */
#define MPR_NOP_CMD_SIZE		    (1U)		/**< MPR NOP command size */
#define MPR_DATA_SIZE		        (4U)		/**< MPR data size (4 bytes) */

/** @} */

/**
 * @defgroup MPR_PRESSURE Pressure Configuration Constants
 * @{
 */

#define MPR_PMAX                    (25.0f)   /**< MPR maximum pressure (PSI) */
#define MPR_PMIN                    (0.0f)    /**< MPR minimum pressure (PSI) */

/* ==== Typedef ==== */

typedef struct {

    I2C_HandleTypeDef *p_hi2c;                  /**< I2C TypeDef instance pointer */

} MPR_t;

/* ==== Function Prototypes ==== */

/**
 * @brief Receive data from the MPR sensor
 *
 * @param sensor    handle of the sensor in use
 * @param data      pointer to the buffer to store the received data
 * @return error_t
 */
error_t MPR_Receive_Data(MPR_t* sensor, uint8_t* data);

/**
 * @brief calculate the pressure in PSI from the raw data received from the MPR sensor
 *
 * @param raw_data  pointer to the raw data received from the MPR sensor
 * @return float
 */
float MPR_Calculate_Pressure(const uint8_t* raw_data);


#ifdef __cplusplus
}
#endif

#endif /* MPR_H */

