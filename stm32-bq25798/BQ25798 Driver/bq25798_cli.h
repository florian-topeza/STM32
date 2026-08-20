/**
 * @file        bq25798_cli.h
 * @author      Florian Topeza
 * @brief       Header for bq25798_cli.c file
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

#ifndef BQ25798_CLI_H
#define BQ25798_CLI_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==== Includes ==== */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "bq25798.h"

// Include the HAL of the STM32 microcontroller in use

#include "stm32l4xx_hal.h"


/* ==== Defines ==== */

/**
 * @defgroup BQ25798_CLI_CONF CLI configuration constants
 * @{
 */
#define BQ25798_CLI_MAX_UART_RX_BUFFER_SIZE    (128U)   /**< Maximum number of bytes received per UART transaction */
#define BQ25798_CLI_MAX_UART_TX_BUFFER_SIZE    (2048U)  /**< Maximum number of bytes sent per UART transaction */
#define BQ25798_CLI_DEFAULT_REFRESH_PERIOD     (500U)   /**< Default data refresh period (500ms) */

/** @} */


/**
 * @defgroup BQ25798_CLI_ADC ADC configuration for CLI use
 * @{
 */

/* ADC CLI configuration is:
    - one shot rate (convert data only when requested)
    - 14 bits resolution
    - single average*/

#define CLI_ADC_RES             (BQ25798_ADC_RES_14_BITS)

#define CLI_ADC_CTRL            ( (BQ25798_ADC_ONE_SHOT << BQ25798_ADC_RATE_OFFSET) & BQ25798_ADC_RATE_MASK ) | \
                                        ( (BQ25798_ADC_RES_14_BITS << BQ25798_ADC_RES_OFFSET) & BQ25798_ADC_RES_MASK ) | \
                                        ( (BQ25798_ADC_SINGLE_AVG << BQ25798_ADC_AVG_OFFSET) & BQ25798_ADC_AVG_MASK )
    /* ADC enabled functions for CLI are:
    - IBUS
    - IBAT
    - VBUS
    - VBAT
    - VSYS
    - TS
    - TDIE
    - VAC2
    - VAC1
    D+ and D- are disabled */

#define CLI_ADC_DIS0            (0x00U)
#define CLI_ADC_DIS1            (BQ25798_DP_ADC_DIS | BQ25798_DM_ADC_DIS)

/** @} */

/* ==== Typedefs ==== */

/**
 * @brief BQ25798 CLI API status codes
 */
typedef enum
{
    BQ25798_CLI_OK,
    BQ25798_CLI_ERROR_INVALID_CMD,
    BQ25798_CLI_ERROR_INVALID_ARGS,
    BQ25798_CLI_ERROR_EXECUTION,

} BQ25798_CLI_Status_t;

/**
 * @brief CLI Handle
 *
 */
typedef struct {
    BQ25798_Handle_t bq;
    UART_HandleTypeDef* huart;      /* UART handle for serial output (USART3 on Nucleo). */
    uint16_t data_refresh_period;    /* Period to refresh the data sent to serial monitor, in milliseconds */

    uint8_t send_adc_data;          /**< If 1, send ADC data, if 0, do not */
    uint8_t send_chg_conf;          /**< If 1, send charge configuration, if 0, do not */
    uint8_t send_status;            /**< If 1, send status, if 0, do not */
    uint8_t send_flags;             /**< If 1, send flags, if 0, do not */
    uint8_t pause;                  /**< If 1, pause command line interface (used when displaying help menu) */

} BQ25798_CLI_Handle_t;


/* ==== Functions prototypes ==== */

/**
 * @brief Configure CLI handle and calls BQ25798_CLI_Reset.
 *
 * @param cli       pointer to the CLI handle instance to be configured
 * @param bq        BQ25798 handle instance to map to the CLI handle
 * @param huart     UART port of the MCU connected to the serial monitor on the computer
 * @return BQ25798_CLI_Status_t
 */
BQ25798_CLI_Status_t BQ25798_CLI_Init(BQ25798_CLI_Handle_t *cli, BQ25798_Handle_t bq, UART_HandleTypeDef *huart);

/**
 * @brief           Reset CLI (activate ADC, activate IBAT pin, disable watchdog and set ship FET as present)
 *
 * @param cli       pointer to the CLI handle instance in use
 * @return BQ25798_CLI_Status_t
 */
BQ25798_CLI_Status_t BQ25798_CLI_Reset(BQ25798_CLI_Handle_t *cli);

/**
 * @brief           clear serial monitor screen by sending a clear screen ANSI sequence (works on most terminals)
 *
 * @param cli       pointer to the CLI handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_CLI_ClearScreen(BQ25798_CLI_Handle_t *cli);

/**
 * @brief           get and send ADC data through the CLI
 *
 * @param cli       pointer to the CLI handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_CLI_SendADCData(BQ25798_CLI_Handle_t *cli);

/**
 * @brief           get and send charge configuration essential parameters through the CLI
 *
 * @param cli       pointer to the CLI handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_CLI_SendChargingConf(BQ25798_CLI_Handle_t *cli);

/**
 * @brief           get and send status and faults indicators through the CLI
 *
 * @param cli       pointer to the CLI handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_CLI_SendStatus(BQ25798_CLI_Handle_t *cli);

/**
 * @brief           get and send interrupt flags through the CLI
 *
 * @param cli       pointer to the CLI handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_CLI_SendFlags(BQ25798_CLI_Handle_t *cli);

/**
 * @brief                   process a command received via UART
 *
 * @param cli               pointer to the CLI handle instance in use
 * @param rx_data_buffer    data buffer to receive the command
 * @return BQ25798_Status_t
 */
BQ25798_CLI_Status_t BQ25798_CLI_ProcessCmd(BQ25798_CLI_Handle_t *cli, uint8_t *rx_data_buffer);

/**
 * @brief               set the refresh period of the CLI, in milliseconds
 *
 * @param cli           pointer to the CLI handle instance in use
 * @param period_ms     desired refresh period in milliseconds
 * @return BQ25798_CLI_Status_t
 */
BQ25798_CLI_Status_t BQ25798_CLI_SetDataRefreshPeriod(BQ25798_CLI_Handle_t *cli, uint16_t period_ms);

/**
 * @brief               print requested data in the serial monitor
 *
 * @param cli           pointer to the CLI handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_CLI_Print(BQ25798_CLI_Handle_t *cli);


/* ==== Macros ====*/

/**
 * @brief Extract bits from register value
 *
 */
#define BQ25798_CONVERT_UINT16T_2_INT16T(val)  ( (int16_t)(val << 1) >> 1)


#ifdef __cplusplus
}
#endif

#endif /* BQ25798_CLI_H */