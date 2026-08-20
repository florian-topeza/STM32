/**
 * @file rgb_led.h
 * @brief Header file for RGB LED driver
 * @author Florian TOPEZA - 2026
 */

/* ==== Header guard ==== */
#ifndef RGB_LED_H
#define RGB_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==== Includes ==== */
#include <stdint.h>
#include "main.h"  /* For GPIO_TypeDef and HAL types */
#include "errors.h"

/* ==== Defines ==== */
#define RGB_LED_BLINK_STATE_ON      (1U)
#define RGB_LED_BLINK_STATE_OFF     (0U)
#define LED_ON                      (0U)  /* Active low */
#define LED_OFF                     (1U)  /* Active low */

/* ==== Typedefs ==== */

/**
 * @brief Configuration structure for RGB LED
 */
typedef struct {
    GPIO_TypeDef* portR;  /**< GPIO port for Red LED */
    uint16_t pinR;        /**< GPIO pin for Red LED */
    GPIO_TypeDef* portG;  /**< GPIO port for Green LED */
    uint16_t pinG;        /**< GPIO pin for Green LED */
    GPIO_TypeDef* portB;  /**< GPIO port for Blue LED */
    uint16_t pinB;        /**< GPIO pin for Blue LED */
} RGB_LED_Config_t;

/* ==== Functions prototypes ==== */

/**
 * @brief Initialize the RGB LED with the given configuration
 * @param config Pointer to the configuration structure
 * @return error_t Status of the operation
 */
error_t RGB_LED_Init(const RGB_LED_Config_t* config);

/**
 * @brief Set the state of the Red LED
 * @param on 1 to turn on (active low), 0 to turn off
 * @return void
 */
void RGB_LED_SetRed(uint8_t on);

/**
 * @brief Set the state of the Green LED
 * @param on 1 to turn on (active low), 0 to turn off
 * @return void
 */
void RGB_LED_SetGreen(uint8_t on);

/**
 * @brief Set the state of the Blue LED
 * @param on 1 to turn on (active low), 0 to turn off
 * @return void
 */
void RGB_LED_SetBlue(uint8_t on);

/**
 * @brief Set the color of the RGB LED
 * @param r 1 to turn on Red, 0 to turn off
 * @param g 1 to turn on Green, 0 to turn off
 * @param b 1 to turn on Blue, 0 to turn off
 * @return void
 */
void RGB_LED_SetColor(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Turn off all colors of the RGB LED
 * @return void
 */
void RGB_LED_TurnOff(void);

/**
 * @brief Start blinking the RGB LED with the specified color and period
 * @param r 1 to include Red in blink, 0 otherwise
 * @param g 1 to include Green in blink, 0 otherwise
 * @param b 1 to include Blue in blink, 0 otherwise
 * @param period_ms Period of the blink in milliseconds
 * @return error_t Status of the operation
 */
error_t RGB_LED_BlinkStart(uint8_t r, uint8_t g, uint8_t b, uint32_t period_ms);

/**
 * @brief Stop blinking the RGB LED
 * @return void
 */
void RGB_LED_BlinkStop(void);

/**
 * @brief Update function to handle blinking. Call this periodically in the main loop.
 * @return error_t Status of the operation
 */
error_t RGB_LED_Update(void);


#ifdef __cplusplus
}
#endif

#endif /* RGB_LED_H */