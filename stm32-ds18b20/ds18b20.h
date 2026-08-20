/**
 * @file ds18b20.h
 * @author Florian Topeza
 * @brief Header file for the driver ds18b20.c
 * @version 0.1
 * @date 2026-05-30
 *
 * @copyright Copyright (c) 2026 Florian Topeza
 *
 */

/* ==== Header guard ==== */
#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_

/* ==== Includes ==== */
#include <stdbool.h> 		// Required to use booleans
#include <stdint.h> 		// Required to use uint8_t and uint16_t
#include <stdlib.h>			// To dynamically allocate memory

// Include STM32 HAL of the microcontroller in use
#include "stm32h7xx_hal.h"

// Include error type
#include "errors.h"

/* ==== Defines ==== */
// Debug DS18B20
#ifdef DEBUG_DS18B20
#include "console.h"
#define log_ds18b20(...)  printf(__VA_ARGS__)
#else
#define log_ds18b20(...)
#endif

/**
 * @defgroup DS18B20_DEF_ROM DS18B20 ROM Commands
 * @{
 */

#define SEARCH_ROM		(0xF0U)     /**< Search ROM command code */
#define READ_ROM		(0x33U)     /**< Read ROM command code */
#define MATCH_ROM		(0x55U)     /**< Match ROM command code */
#define SKIP_ROM		(0xCCU)     /**< Skip ROM command code */
#define ALARM_SEARCH	(0xECU)     /**< Alarm search command code */

/** @} */

/**
 * @defgroup DS18B20_DEF_FUNC DS18B20 Function Commands
 * @{
 */
#define CONVERT_T			(0x44U) /**< Convert temperature command code */
#define WRITE_SCRATCHPAD	(0x4EU) /**< Write scratchpad command code */
#define READ_SCRATCHPAD		(0xBEU) /**< Read scratchpad command code */
#define COPY_SCRATCHPAD		(0x48U) /**< Copy scratchpad command code */
#define RECALL_EE			(0xB8U) /**< Recall EE command code */
#define READ_PWR_SUPPLY		(0xB4U) /**< Read power supply command code */

/** @} */

/* ==== Typedefs ==== */

/**
 * @brief DS18B20 Handle structure
 *
 */
typedef struct
{
    TIM_HandleTypeDef htim;             // Timer TypeDef instance for the timer to use delays in microseconds
    GPIO_TypeDef * gpio_port;           // GPIO Port for the onewire pin of the sensor
    uint16_t gpio_pin;                  // GPIO Pin number for the onewire of the sensor

} DS18B20_t;

/**
 * @brief Search state structure
 *
 */

typedef struct {

    // The highest bit position where a bit was ambiguous and a zero was written
    int8_t last_zero_branch;

    // Internal flag to indicate if the search is complete
    // This flag is set once there are no more branches to search
    bool done;

    // Discovered 64-bit device address (LSB first)
    // After a successful search, this contains the found device address.
    // During a search this is overwritten LSB-first with a new address.
    uint8_t address[8];

} onewire_search_state_t;

/* ==== Functions prototypes ==== */

/**
 * @brief                   Initialize DS18B20 Handle
 *
 * @param sensor            Pointer to the handle to use
 * @param sensor_htim       Pointer to the timer handle to use for the onewire delays
 * @param sensor_gpio_port  GPIO Port of the GPIO pin used for the onewire bus
 * @param sensor_gpio_pin   GPIO pin used for the onewire bus
 * @return error_t
 */
error_t DS18B20_Init(DS18B20_t *sensor, TIM_HandleTypeDef *sensor_htim,
                        GPIO_TypeDef * sensor_gpio_port, uint16_t sensor_gpio_pin);

/**
 * @brief                   Search DS18B20 sensors present on the onewire bus
 *
 * @param sensor            Pointer to the handle of one of the sensors in use (if multiple sensors)
 * @param ROM_Codes_array   Array to store the ROM codes of the sensors found on the bus
 */
void DS18B20_Search(DS18B20_t* sensor, uint64_t ROM_Codes_array[]);

/**
 * @brief                   Get the temperature data of the sensors on the bus
 *
 * @param sensor            Pointer to one of the sensors handles
 * @param ROM_Codes_array   Array to store the ROM codes of the sensors found on the bus
 * @param temperature       Pointer to store the temperature data, in raw signed 1/16 degC units
 *                          (e.g. -16 = -1.0 degC, 400 = 25.0 degC)
 */
void DS18B20_GetTemp(DS18B20_t *sensor, const uint64_t ROM_Codes_array[], int16_t *temperature);

 #endif /* INC_DS18B20_H_ */

