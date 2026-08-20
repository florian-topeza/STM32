# STM32-LSM6DSL

## C driver to interface LSM6DSL 6-DOF IMU with an STM32 microcontroller

This driver is largely inspired by the work from ST. Instead of using files spread apart the BSP, this driver has the advantage of being self sufficient to interface this sensor with an ST microcontroller.

The driver itself is made of the files lsm6dsl.c and lsm6dsl.h. It communicates with the sensor over SPI, in 3-wire mode by default (see `LSM6DSL_SPI_3WIRE` in lsm6dsl.h).

It only requires the file errors.h, common to all my drivers, which declares the error type returned by the driver's functions. It can be found here https://github.com/astarus-pyxis/stm32-common.

Defining `DEBUG_LSM6DSL` enables debug logs over `printf`.

## How to use this driver in a project

To use this driver in an STM32 CMake project, the SPI peripheral and the CS GPIO pin for the sensor shall be configured by the user, with CubeMX or directly in the code. This is not done by the driver.

The C file lsm6dsl.c shall be placed in the Core > Src folder of the project, and lsm6dsl.h and errors.h in the Core > Inc folder.

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
    "Core/Src/lsm6dsl.c"
)
```

## Licence & Warranty

This driver is licensed under GNU V3.0. It comes with no warranty.

Please note that this is a minimal driver to work with this IMU. It does not provide all the functions that can be performed by this device.
