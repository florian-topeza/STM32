/**
 * @file        mpr.h
 * @author      Florian Topeza & Merlin Kooshmanian
 * @brief       Minimal driver to interface MPR pressure sensors with STM32 microcontrollers
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

 /* ==== Includes ==== */
#include "mpr.h"

/* ==== Functions ==== */

/* Receive data from the sensor */
error_t MPR_Receive_Data(MPR_t *sensor, uint8_t *pressure_read_buffer)
{
    error_t result = ERR_OK;
    HAL_StatusTypeDef HAL_result = HAL_OK;

    uint8_t output_measure_cmd[MPR_PRESSURE_READ_CMD_SIZE] = {0xAA, 0x00, 0x00}; // Command to output measurement

    HAL_result = HAL_I2C_Master_Transmit(sensor->p_hi2c, MPR_I2C_ADDRESS, output_measure_cmd, MPR_PRESSURE_READ_CMD_SIZE, HAL_MAX_DELAY);

    if (HAL_result != HAL_OK)
    {
        result = ERR_HAL;
    }

    HAL_Delay(30);

    HAL_result = HAL_I2C_Master_Receive(sensor->p_hi2c, MPR_I2C_ADDRESS, pressure_read_buffer, MPR_DATA_SIZE, HAL_MAX_DELAY);

    if (HAL_result != HAL_OK)
    {
        result = ERR_HAL;
    }

    logs_mpr("MPR Raw data: %x, %2x, %2x, %2x\n\r", pressure_read_buffer[0], pressure_read_buffer[1], pressure_read_buffer[2], pressure_read_buffer[3]);

    return result;
}

/* Calculate pressure in psi */
float MPR_Calculate_Pressure(const uint8_t *raw_data)
{

    // raw_data is the received data buffer from the sensor
    // The pressure calculation is based on the digital counts received from the sensor

    // Constants for pressure calculation
    uint32_t press_counts = 0;     // digital pressure reading [counts]
    float pressure = 0.0;          // pressure reading [psi]
    uint32_t outputmax = 15099494; // output at maximum pressure [counts] (90% of 2^(24) counts)
    uint32_t outputmin = 1677722;  // output at minimum pressure [counts] (10% of 2^(24) counts)
    float pmax = MPR_PMAX;         // maximum value of pressure range [psi]
    float pmin = MPR_PMIN;         // minimum value of pressure range [psi]

    press_counts = (uint32_t)(raw_data[1] << 16) | (uint32_t)(raw_data[2] << 8) | (uint32_t)(raw_data[3]); // calculate digital pressure counts

    // calculation of pressure value according to equation 2 of datasheet
    pressure = ((float)(press_counts - outputmin) * (pmax - pmin)) / (float)(outputmax - outputmin) + pmin;

    logs_mpr("Calculated pressure: %.2f\n\r", pressure);

    return pressure;
}
