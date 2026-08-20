/**
 * @file        console.h
 * @author      Florian Topeza
 * @brief        Header for the driver console.c.
 * @version     0.1
 * @date        2026-05-30
 *
 * @copyright   Copyright (c) 2026 Florian Topeza
 *
 */

/* ==== Header guard ==== */

#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==== Includes ==== */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Include the HAL of the microcontroller in use
#include "stm32h7xx_hal.h"

/* ==== Defines ==== */

#define CONSOLE_UART_TIMEOUT 300 /**< Timeout for UART transmission in milliseconds */

#define STDIN_FILENO  0   /**< File descriptor of STDIN */
#define STDOUT_FILENO 1   /**< File descriptor of STDOUT */
#define STDERR_FILENO 2   /**< File descriptor of STDERR */

/* ==== Functions prototypes ==== */

/**
 * @brief                   Initialize UART console
 *
 * @param p_uart_handle     Pointe to the UART handle to use
 * @return int
 */
extern int init_console(UART_HandleTypeDef *p_uart_handle);

#ifdef __cplusplus
}
#endif

#endif // CONSOLE_H

