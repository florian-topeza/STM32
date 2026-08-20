# STM32-BQ25798

## C driver to interface the BQ25798 battery charger by TI with STM32 microcontrollers

The driver is made of the files bq25798.h and bq25798.c. It is a minimalistic driver that provides the essentials functions to pilot a BQ25798 charger with an STM32 microcontroller.

## Serial interface to monitor the charger from a computer

The files bq25798_cli.h and bq25798_cli.c use the driver to provide a command line interface (CLI) to display the charger data into a serial monitor, and rto send some commands to the charger through this interface.
It can be used directly in a standard serial monitor, such as the one integrated in VSCode, or with the serial monitor provided by the BQ25798 GUI, a dedicated Python user interface.

## How to use this driver in a project

To use this driver in an STM32 CMake project, the C file  flash_xx.c  shall be placed in the Core > Src folder of the project, and flash_xx.h, errors.h  in the Core > Inc folder.

It also requires to add the sources to executable in the CMakeLists.txt file at the root of the project. To do this, the following at line 48 of this file.


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
    "Core/Src/bq25798.c"
    # Add the following line to use the CLI
    "Core/Src/bq25798_cli.c"
)
```

## Licence & Warranty

This driver is licensed under GNU V3.0. It comes with no warranty.
