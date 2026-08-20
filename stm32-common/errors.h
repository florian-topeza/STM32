/**
 * @file errors.h
 * @author Florian Topeza
 * @brief Declare the enum type used to return error types in functions
 * @version 0.2
 * @date 2026-07-09
 *
 * @copyright Copyright (c) 2026 Florian Topeza
 *
 */

/* ==== Header guard ==== */
#ifndef ERRORS_H
#define ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==== Typedefs ==== */

/**
 * @brief Common return/status codes shared by the drivers in this repo.
 *
 * @note  Every enumerator is prefixed with `ERR_` on purpose: the STM32 HAL
 *        headers already define `HAL_OK`, `HAL_ERROR`, `HAL_BUSY` and
 *        `HAL_TIMEOUT` as macros/enumerators, and a flat, unprefixed name
 *        (e.g. a bare `OK`) is one `#include` away from clashing with a
 *        vendor header or another module in the same translation unit.
 * @note  Values are assigned explicitly and must be treated as a stable
 *        ABI: never renumber or reuse a value, since callers may compare
 *        against these codes numerically (logs, wire formats, debugger
 *        watches). Only ever append new codes at the end.
 */
typedef enum
{
    ERR_OK                  = 0U,  /**< Operation completed successfully. */
    ERR_HAL                 = 1U,  /**< Underlying HAL call returned HAL_ERROR. */
    ERR_INVALID_ARGUMENT    = 2U,  /**< An argument is missing or out of its valid range. */
    ERR_NULL_POINTER        = 3U,  /**< A required pointer argument is NULL. */
    ERR_NO_DEVICE           = 4U,  /**< No response/acknowledgment from the target device. */
    ERR_INVALID_ADDRESS     = 5U,  /**< Address/index argument is outside the valid range. */
    ERR_HAL_BUSY            = 6U,  /**< Underlying HAL call returned HAL_BUSY. */
    ERR_HAL_TIMEOUT         = 7U,  /**< Underlying HAL call returned HAL_TIMEOUT. */
    ERR_NOT_INITIALIZED     = 8U,  /**< Handle/peripheral was used before being initialized. */

} error_t;

#ifdef __cplusplus
}
#endif

#endif  // ERRORS_H