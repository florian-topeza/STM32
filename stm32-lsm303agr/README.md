# STM32-LSM303AGR

## C driver to interface LSM303AGR 6-DOF IMU (accelerometer + magnetometer) with an STM32 microcontroller

This driver is largely inspired by the work from ST. Instead of using files spread apart the BSP, this driver has the advantage of being self sufficient to interface this sensor with an ST microcontroller.

The driver itself is made of the files lsm303agr.c and lsm303agr.h. It communicates with the sensor over I2C, using two distinct device addresses: one for the accelerometer and one for the magnetometer (see `ACC_I2C_ADDRESS` / `MAG_I2C_ADDRESS` in lsm303agr.h).

It requires the file errors.h, common to all my drivers, which declares the error type returned by the driver's functions. It can be found here https://github.com/astarus-pyxis/stm32-common.

Defining `DEBUG_LSM303AGR` enables debug logs over `printf`. This additionally requires console.h and console.c, also common to all my drivers, which retarget `printf` output over UART. They can be found at the same link above.

## How to use this driver in a project

To use this driver in an STM32 CMake project, the I2C peripheral for the sensor shall be configured by the user, with CubeMX or directly in the code. This is not done by the driver.

The C file lsm303agr.c shall be placed in the Core > Src folder of the project, and lsm303agr.h and errors.h in the Core > Inc folder. If `DEBUG_LSM303AGR` is defined, console.c shall also be placed in Core > Src, and console.h in Core > Inc.

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
    "Core/Src/lsm303agr.c"
)
```

## Licence & Warranty

This driver is licensed under GNU V3.0. It comes with no warranty.

Please note that this is a minimal driver to work with this IMU. It does not provide all the functions that can be performed by this device.
