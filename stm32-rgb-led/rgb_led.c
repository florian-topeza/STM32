/**
 * @file rgb_led.c
 * @brief Implementation of RGB LED driver
 * @author Florian TOPEZA - 2026
 */

 /* ==== Includes ==== */
#include "rgb_led.h"

/* ==== Private variables ==== */

static RGB_LED_Config_t rgb_led_config;
static uint8_t rgb_led_blink_active = 0U;
static uint8_t rgb_led_blink_r = 0U;
static uint8_t rgb_led_blink_g = 0U;
static uint8_t rgb_led_blink_b = 0U;
static uint32_t rgb_led_blink_period = 0U;
static uint32_t rgb_led_last_toggle_time = 0U;
static uint8_t rgb_led_blink_state = RGB_LED_BLINK_STATE_OFF;

/* ==== Public functions ==== */

error_t RGB_LED_Init(const RGB_LED_Config_t* config)
{
    error_t status = ERR_OK;

    if (config == NULL)
    {
        status = ERR_INVALID_ARGUMENT;
    }

    rgb_led_config = *config;

    /* Initialize GPIOs to high (off state, active low) */
    HAL_GPIO_WritePin(rgb_led_config.portR, rgb_led_config.pinR, GPIO_PIN_SET);

    HAL_GPIO_WritePin(rgb_led_config.portG, rgb_led_config.pinG, GPIO_PIN_SET);

    HAL_GPIO_WritePin(rgb_led_config.portB, rgb_led_config.pinB, GPIO_PIN_SET);

    return status;
}

void RGB_LED_SetRed(uint8_t on)
{
    GPIO_PinState state = (on != 0U) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    HAL_GPIO_WritePin(rgb_led_config.portR, rgb_led_config.pinR, state);

}

void RGB_LED_SetGreen(uint8_t on)
{
    GPIO_PinState state = (on != 0U) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    HAL_GPIO_WritePin(rgb_led_config.portG, rgb_led_config.pinG, state);

}

void RGB_LED_SetBlue(uint8_t on)
{
    GPIO_PinState state = (on != 0U) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    HAL_GPIO_WritePin(rgb_led_config.portB, rgb_led_config.pinB, state);
}

void RGB_LED_SetColor(uint8_t r, uint8_t g, uint8_t b)
{
    RGB_LED_SetRed(r);
    RGB_LED_SetGreen(g);
    RGB_LED_SetBlue(b);
}


void RGB_LED_TurnOff(void)
{
    RGB_LED_SetColor(0U, 0U, 0U);
}

error_t RGB_LED_BlinkStart(uint8_t r, uint8_t g, uint8_t b, uint32_t period_ms)
{
    error_t status = ERR_OK;
    if (period_ms == 0U)
    {
        status = ERR_INVALID_ARGUMENT;
    }
    else
    {

    rgb_led_blink_active = 1U;
    rgb_led_blink_r = r;
    rgb_led_blink_g = g;
    rgb_led_blink_b = b;
    rgb_led_blink_period = period_ms;
    rgb_led_last_toggle_time = HAL_GetTick();
    rgb_led_blink_state = RGB_LED_BLINK_STATE_OFF;


    /* Start with off */
    RGB_LED_SetColor(0U, 0U, 0U);

    }

    return status;
}

void RGB_LED_BlinkStop(void)
{
    rgb_led_blink_active = 0U;
    RGB_LED_TurnOff();
}

error_t RGB_LED_Update(void)
{
    error_t status = ERR_OK;

    if (rgb_led_blink_active != 0U)
    {
        uint32_t current_time = HAL_GetTick();
        if ((current_time - rgb_led_last_toggle_time) >= rgb_led_blink_period)
        {
            rgb_led_last_toggle_time = current_time;
            if (rgb_led_blink_state == RGB_LED_BLINK_STATE_OFF)
            {
                RGB_LED_SetColor(rgb_led_blink_r, rgb_led_blink_g, rgb_led_blink_b);
                if (status == ERR_OK)
                {
                    rgb_led_blink_state = RGB_LED_BLINK_STATE_ON;
                }
            }
            else
            {
                RGB_LED_SetColor(0U, 0U, 0U);
                if (status == ERR_OK)
                {
                    rgb_led_blink_state = RGB_LED_BLINK_STATE_OFF;
                }
            }
        }
    }
    return status;
}
