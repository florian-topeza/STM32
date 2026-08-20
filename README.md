# STM32

## Repository content

This repository provides the following STM32 drivers:

- STM32 common contains code common to all my drivers
- STM32 RGB LED is a driver to interface RGB LEDs with STM32 microcontrollers
- STM32 DS18B20 is a driver to interface DS18B20 temperature sensors with STM32 microcontrollers
- STM32 DS2406 is a driver to interface DS2406 addressable switches with STM32 microcontrollers
- STM32 LSM6DSL is a driver to interface LSM6DSL 6-DOF IMUs with STM32 microcontrollers
- STM32 LSM303AGR is a driver to interface LSM303AGR 6-DOF IMUs with STM32 microcontrollers
- STM32 MPR is a driver to interface MPR pressure sensors with STM32 microcontrollers
- STM32 DS2406 is a driver to interface LSM303AGR 6-DOF IMUs with STM32 microcontrollers
- STM32 SCH16T is a driver to interface SCH16T IMUs with STM32 microcontrollers

## How to use these drivers in an STM32 project

To use these drivers in STM32 projects, the C files  shall be placed in the Core > Src folder of the project, and the header files in the Core > Inc folder.

For CMake projects, is also requires to add the sources to executable at line 48 of the CMakeLists.txt file at the root of the project. For instance, the following enables the use of the RGB LED driver.

```
# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    "Core/Src/rgb_led.c"
)
```

## Licence & Warranty

The code provided in this repository is licensed under GNU V3.0. It comes with no warranty.