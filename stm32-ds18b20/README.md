# STM32-DS18B20

C driver to interface one or more DS18B20 1-Wire temperature sensors with an STM32 microcontroller.

The driver is made of `ds18b20.c` and `ds18b20.h`. It requires `errors.h` from [stm32-common](https://github.com/astarus-pyxis/stm32-common) for the shared `error_t` return codes, and optionally `console.h`/`console.c` from the same repo if `DEBUG_DS18B20` is defined (enables `log_ds18b20` traces over UART).

## Hardware setup

- The onewire GPIO pin must be configured as **open-drain output** with a pull-up (external ~4.7kΩ, or the internal pull-up if your MCU/bus length allows it). The driver only calls `HAL_GPIO_WritePin`/`HAL_GPIO_ReadPin`; it never switches the pin direction, so a push-pull configuration will short the bus.
- A general-purpose timer must be configured (via CubeMX) with a 1µs tick (e.g. prescaler set so the counter runs at 1 MHz). `DS18B20_Init` starts it — it does not need to be started beforehand.

## API

```c
DS18B20_t sensor;
DS18B20_Init(&sensor, &htim6, GPIOA, GPIO_PIN_1);

uint64_t rom_codes[8] = {0};      // zero-initialized, sized for the max number of sensors expected
DS18B20_Search(&sensor, rom_codes);

int16_t temperatures[8];
DS18B20_GetTemp(&sensor, rom_codes, temperatures);
float degC = temperatures[0] / 16.0f;  // temperature is a signed 1/16 degC fixed-point value
```

- `DS18B20_Init` — binds the handle to a timer and GPIO pin, and starts the timer.
- `DS18B20_Search` — walks the bus and fills `rom_codes` with the ROM code of every sensor found (array must be zero-initialized; `DS18B20_GetTemp` uses a `0` entry as the end-of-list marker).
- `DS18B20_GetTemp` — triggers a conversion and reads back the temperature of every sensor in `rom_codes`, in signed 1/16 degC units (negative values are valid, e.g. `-16` = -1.0°C).

## How to use this driver in a project

Copy `ds18b20.c` and `ds18b20.h` (and `errors.h`, plus `console.h`/`console.c` if using `DEBUG_DS18B20`) into your project's source tree, and add `ds18b20.c` (and `console.c` if used) to your build system's source list.

## Licence & Warranty

This driver is licensed under GNU V3.0. It comes with no warranty.
