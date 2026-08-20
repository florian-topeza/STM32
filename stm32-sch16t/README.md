# STM32-SCH16T

## C driver to interface the Murata SCH16T-K01 IMU with an STM32 microcontroller

The driver itself is made of the files sch16t.c and sch16t.h. It communicates with the sensor over SPI, using the sensor's 48-bit SPI frame format (32-bit frame support is declared in sch16t.h but not currently exercised by the driver).

It requires the file errors.h, common to all my drivers, which declares the error type returned by the driver's functions. It can be found here https://github.com/astarus-pyxis/stm32-common.

Defining `DEBUG_MURATA` enables debug logs over `printf`. This additionally requires console.h and console.c, also common to all my drivers, which retarget `printf` output over UART. They can be found at the same link above.

The file main.c is an example of main that uses the driver.

## How to use this driver in a project

To use this driver in an STM32 CMake project, the SPI peripheral for the sensor, as well as its CS and RESET GPIO pins, shall be configured by the user, with CubeMX or directly in the code. This is not done by the driver.

The C file sch16t.c shall be placed in the Core > Src folder of the project, and sch16t.h and errors.h in the Core > Inc folder. If `DEBUG_MURATA` is defined, console.c shall also be placed in Core > Src, and console.h in Core > Inc.

It also requires to add the sources to executable in the CMakeLists.txt file at the root of the project. To do this, the following at line 48 of this file

```
# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

```

shall be changed to

```
# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    "Core/Src/sch16t.c"
)
```

## Licence & Warranty

This driver is licensed under GNU V3.0. It comes with no warranty.

Please note that this is a minimal driver to work with this IMU. It does not provide all the functions that can be performed by this device. Notably, the initialization sequence uses the default configuration of the sensor.
