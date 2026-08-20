/**
 * @file        sch16t.h
 * @author      Florian Topeza & Merlin Kooshmanian
 * @brief       Header file for Murata SCH16T-K01 inertial sensor driver
 * @version     0.1
 * @date        2026-05-29
 *
 * @copyright Copyright (c) 2026 Florian Topeza
 *
 *
 * @attention
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 */

/* ==== Header guard ==== */
#ifndef SCH16T_H
#define SCH16T_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==== Includes ==== */

#include <stdint.h>
#include <stdio.h>

// Include the HAL of the STM32 microcontroller in use
#include "stm32h7xx_hal.h"

#include "errors.h"

/* ==== Defines ==== */

/* Debug MURATA */
#ifdef DEBUG_MURATA
#include "console.h"
#define logs_murata(...) printf(__VA_ARGS__)
#else
#define logs_murata(...)
#endif

/**
 * @defgroup SCH16T_LPF Low-Pass Filter Settings (3 bits, nominal digital cut-off frequency -3dB)
 * @{
 */

#define LPF0 (0x00U) /**< 0b000, 68 Hz (default) */
#define LPF1 (0x01U) /**< 0b001, 30 Hz */
#define LPF2 (0x02U) /**< 0b010, 13 Hz */
#define LPF3 (0x03U) /**< 0b011, 280 Hz */
#define LPF4 (0x04U) /**< 0b100, 370 Hz */
#define LPF5 (0x05U) /**< 0b101, 235 Hz */
#define LPF6 (0x06U) /**< 0b110, Reserved */
#define LPF7 (0x07U) /**< 0b111, Bypass */

/** @} */

/**
 * @defgroup SCH16T_DYN Dynamic Range Settings (3 bits, for CTRL_RATE, CTRL_ACC12 and CTRL_ACC3 registers)
 * @{
 */

#define UNDEFINED (0x00U) /**< 0b000, Undefined */
#define DYN0      (0x00U) /**< 0b000 */
#define DYN1      (0x01U) /**< 0b001, Default */
#define DYN2      (0x02U) /**< 0b010 */
#define DYN3      (0x03U) /**< 0b011 */
#define DYN4      (0x04U) /**< 0b100 */

/** @} */

/**
 * @defgroup SCH16T_DEC Decimation Ratio Settings (3 bits, for CTRL_RATE and CTRL_ACC12 registers)
 * @{
 */

#define DEC1 (0x00U) /**< 0b000, No decimation, reduction factor 1, nominal F_PRIM = 11.8 kHz */
#define DEC2 (0x01U) /**< 0b001, Reduction factor 2, nominal F_PRIM = 5.9 kHz */
#define DEC3 (0x02U) /**< 0b010, Reduction factor 4, nominal F_PRIM = 2.95 kHz */
#define DEC4 (0x03U) /**< 0b011, Reduction factor 8, nominal F_PRIM = 1.475 kHz */
#define DEC5 (0x04U) /**< 0b100, Reduction factor 16, nominal F_PRIM = 738.5 Hz */

/** @} */

/**
 * @defgroup SCH16T_REG_CTRL Sensor Control Block Registers (read/write)
 * @{
 */

#define CTRL_FILT_RATE  (0x0025U) /**< RATE_XYZ filter settings */
#define CTRL_FILT_ACC12 (0x0026U) /**< ACC_(X,Y,Z)12 filter settings */
#define CTRL_FILT_ACC3  (0x0027U) /**< Filter settings for ACC_(X,Y,Z)3 */

#define CTRL_RATE  (0x0028U) /**< Gyro post-processing decimation ratio and dynamic range settings */
#define CTRL_ACC12 (0x0029U) /**< ACC_(X,Y,Z)12 post-processing decimation and dynamic range settings */
#define CTRL_ACC3  (0x002AU) /**< ACC_(X,Y,Z)3 post-processing shift dynamic range settings */

#define CTRL_MODE  (0x0035U) /**< Test mode, EOI, EN_SENSOR */
#define CTRL_RESET (0x0036U) /**< SPI soft reset command */

/** @} */

/**
 * @defgroup SCH16T_REG_DATA Sensor Data Block Registers (read-only, 16-bit or 20-bit depending on the SPI frame type)
 * @{
 */

#define RATE_X1 (0x0001U) /**< Output, x-axis gyroscope, interpolation, common LPF with RATE_X2 */
#define RATE_Y1 (0x0002U) /**< Output, y-axis gyroscope, interpolation, common LPF with RATE_Y2 */
#define RATE_Z1 (0x0003U) /**< Output, z-axis gyroscope, interpolation, common LPF with RATE_Z2 */
#define RATE_X2 (0x000AU) /**< Output, x-axis gyroscope, configurable decimation filter, common LPF with RATE_X1 */
#define RATE_Y2 (0x000BU) /**< Output, y-axis gyroscope, configurable decimation filter, common LPF with RATE_Y1 */
#define RATE_Z2 (0x000CU) /**< Output, z-axis gyroscope, configurable decimation filter, common LPF with RATE_Z1 */

#define ACC_X1 (0x0004U) /**< Output, x-axis accelerometer, interpolation, common LPF with ACC_X2 */
#define ACC_Y1 (0x0005U) /**< Output, y-axis accelerometer, interpolation, common LPF with ACC_Y2 */
#define ACC_Z1 (0x0006U) /**< Output, z-axis accelerometer, interpolation, common LPF with ACC_Z2 */
#define ACC_X2 (0x000DU) /**< Output, x-axis accelerometer, configurable decimation filter, common LPF with ACC_X1 */
#define ACC_Y2 (0x000EU) /**< Output, y-axis accelerometer, configurable decimation filter, common LPF with ACC_Y1 */
#define ACC_Z2 (0x000FU) /**< Output, z-axis accelerometer, configurable decimation filter, common LPF with ACC_Z1 */
#define ACC_X3 (0x0007U) /**< Output, x-axis accelerometer, auxiliary signal path with interpolation and individually configurable LPF */
#define ACC_Y3 (0x0008U) /**< Output, y-axis accelerometer, auxiliary signal path with interpolation and individually configurable LPF */
#define ACC_Z3 (0x0009U) /**< Output, z-axis accelerometer, auxiliary signal path with interpolation and individually configurable LPF */

#define TEMP (0x0010U) /**< Output, temperature sensor */

/** @} */

/**
 * @defgroup SCH16T_REG_STATUS Sensor Status Registers (read-only 16-bit data unless otherwise noted)
 * @{
 */

#define STAT_SUM         (0x0014U) /**< Status, summary for non-saturation related flags */
#define STAT_SUM_SAT     (0x0015U) /**< Status, summary saturation flags */
#define STAT_COM         (0x0016U) /**< Common status flags, including TEMP, first level status register */
#define STAT_RATE_COM    (0x0017U) /**< Common gyro status flags (primary channel), first level status register */
#define STAT_RATE_X      (0x0018U) /**< Status, rate x-axis, first level status register */
#define STAT_RATE_Y      (0x0019U) /**< Status, rate y-axis, first level status register */
#define STAT_RATE_Z      (0x001AU) /**< Status, rate z-axis, first level status register */
#define STAT_ACC_X       (0x001BU) /**< Status, accelerometer x-axis, first level status register */
#define STAT_ACC_Y       (0x001CU) /**< Status, accelerometer y-axis, first level status register */
#define STAT_ACC_Z       (0x001DU) /**< Status, accelerometer z-axis, first level status register */
#define STAT_SYNC_ACTIVE (0x001EU) /**< Status of SYNC on each channel, 11 bits */
#define STAT_INFO        (0x001FU) /**< Low power mode indications, 8 bits */

/** @} */

/**
 * @defgroup SCH16T_REG_MISC Miscellaneous Registers
 * @{
 */

/**
 * @brief 16-bit read/write register used to check accessibility of the device, or that CS signals are
 *        working properly when multiple devices share the SPI bus.
 *        Due to the off-frame protocol, the test sequence should be:
 *        1. Write data into SYS_TEST register.
 *        2. Read SYS_TEST register content.
 *        3. Issue a dummy read command to receive the response from the previous frame.
 *        SYS_TEST register is not locked by the EOI bit.
 */
#define SYS_TEST (0x0037U)

#define SN_ID1 (0x003DU) /**< Component serial number field 1, 4-bit read-only register, F part of the serial number, format DDDYYHHHHHH01 */
#define SN_ID2 (0x003EU) /**< Component serial number field 2, 16-bit read-only register, DDDYY part of the serial number, format DDDYYHHHHHH01 */
#define SN_ID3 (0x003FU) /**< Component serial number field 3, 16-bit read-only register, HHHH part of the serial number, format DDDYYHHHHHH01 */

/** @} */

/**
 * @defgroup SCH16T_REQ_DATA SPI Standard Requests: Rate and Acceleration Read Commands
 * @{
 */

#define REQ_READ_RATE_X1 (0x0048000000ACULL)
#define REQ_READ_RATE_Y1 (0x00880000009AULL)
#define REQ_READ_RATE_Z1 (0x00C80000006DULL)
#define REQ_READ_ACC_X1  (0x0108000000F6ULL)
#define REQ_READ_ACC_Y1  (0x014800000001ULL)
#define REQ_READ_ACC_Z1  (0x018800000037ULL)
#define REQ_READ_ACC_X3  (0x01C8000000C0ULL)
#define REQ_READ_ACC_Y3  (0x02080000002EULL)
#define REQ_READ_ACC_Z3  (0x0248000000D9ULL)
#define REQ_READ_RATE_X2 (0x0288000000EFULL)
#define REQ_READ_RATE_Y2 (0x02C800000018ULL)
#define REQ_READ_RATE_Z2 (0x030800000083ULL)
#define REQ_READ_ACC_X2  (0x034800000074ULL)
#define REQ_READ_ACC_Y2  (0x038800000042ULL)
#define REQ_READ_ACC_Z2  (0x03C8000000B5ULL)

/** @} */

/**
 * @defgroup SCH16T_REQ_STATUS SPI Standard Requests: Status Read Commands
 * @{
 */

#define REQ_READ_STAT_SUM      (0x05080000001CULL)
#define REQ_READ_STAT_SUM_SAT  (0x0548000000EBULL)
#define REQ_READ_STAT_COM      (0x0588000000DDULL)
#define REQ_READ_STAT_RATE_COM (0x05C80000002AULL)
#define REQ_READ_STAT_RATE_X   (0x0608000000C4ULL)
#define REQ_READ_STAT_RATE_Y   (0x064800000033ULL)
#define REQ_READ_STAT_RATE_Z   (0x068800000005ULL)
#define REQ_READ_STAT_ACC_X    (0x06C8000000F2ULL)
#define REQ_READ_STAT_ACC_Y    (0x070800000069ULL)
#define REQ_READ_STAT_ACC_Z    (0x07480000009EULL)

/** @} */

/**
 * @defgroup SCH16T_REQ_TEMP_SN SPI Standard Requests: Temperature and Traceability Read Commands
 * @{
 */

#define REQ_READ_TEMP     (0x0408000000B1ULL)
#define REQ_READ_SN_ID1   (0x0F4800000065ULL)
#define REQ_READ_SN_ID2   (0x0F8800000053ULL)
#define REQ_READ_SN_ID3   (0x0FC8000000A4ULL)
#define REQ_READ_COMP_ID  (0x0F0800000092ULL)

/** @} */

/**
 * @defgroup SCH16T_REQ_FILTERS SPI Standard Requests: Filter Read/Set Commands
 * @{
 */

#define REQ_READ_FILT_RATE   (0x0948000000FAULL)
#define REQ_READ_FILT_ACC12  (0x0988000000CCULL)
#define REQ_READ_FILT_ACC3   (0x09C80000003BULL)
#define REQ_READ_RATE_CTRL   (0x0A08000000D5ULL)
#define REQ_READ_ACC12_CTRL  (0x0A4800000022ULL)
#define REQ_READ_ACC3_CTRL   (0x0A8800000014ULL)
#define REQ_READ_MODE_CTRL   (0x0D4800000010ULL)

#define REQ_SET_FILT_RATE  (0x0968000000ULL) /**< For building the Rate_XYZ1/2 filter setting frame */
#define REQ_SET_FILT_ACC12 (0x09A8000000ULL) /**< For building the Acc_XYZ1/2 filter setting frame */
#define REQ_SET_FILT_ACC3  (0x09E8000000ULL) /**< For building the Acc_XYZ3 filter setting frame */

/** @} */

/**
 * @defgroup SCH16T_REQ_SENS_DEC SPI Standard Requests: Sensitivity and Decimation Set Commands
 * @{
 */

#define REQ_SET_RATE_CTRL  (0x0A28000000ULL) /**< For building the Rate_XYZ1/2 sensitivity and Rate_XYZ2 decimation setting frame */
#define REQ_SET_ACC12_CTRL (0x0A68000000ULL) /**< For building the Acc_XYZ1/2 sensitivity and Acc_XYZ2 decimation setting frame */
#define REQ_SET_ACC3_CTRL  (0x0AA8000000ULL) /**< For building the Acc_XYZ3 sensitivity setting frame */
#define REQ_SET_MODE_CTRL  (0x0D68000000ULL) /**< For building the MODE register setting frame */

/** @} */

/**
 * @defgroup SCH16T_REQ_DRY_SYNC SPI Standard Requests: DRY/SYNC Configuration Commands
 * @{
 */

#define REQ_READ_USER_IF_CTRL (0x0CC80000007CULL)
#define REQ_SET_USER_IF_CTRL  (0x0CE8000000ULL) /**< For building the USER_IF_CTRL register setting frame */

/** @} */

/**
 * @defgroup SCH16T_REQ_OTHER SPI Standard Requests: Other Commands
 * @{
 */

#define REQ_SOFTRESET (0x0DA800000AC3ULL) /**< SPI soft reset command */

/** @} */

/**
 * @defgroup SCH16T_FRAME_MASK Frame Field Masks
 * @{
 */

#define TA_FIELD_MASK    (0xFFC000000000ULL)
#define SA_FIELD_MASK    (0x7FE000000000ULL)
#define DATA_FIELD_MASK  (0x00000FFFFF00ULL)
#define CRC_FIELD_MASK   (0x0000000000FFULL)
#define ERROR_FIELD_MASK (0x001E00000000ULL)

/** @} */

/* ==== Typedefs ==== */

/**
 * @brief Sensor status registers snapshot
 */
typedef struct
{
    uint16_t stat_sum_rx;      /**< STAT_SUM register data */
    uint16_t stat_sum_sat_rx;  /**< STAT_SUM_SAT register data */
    uint16_t stat_com_rx;      /**< STAT_COM register data */
    uint16_t stat_rate_com_rx; /**< STAT_RATE_COM register data */
    uint16_t stat_rate_x_rx;   /**< STAT_RATE_X register data */
    uint16_t stat_rate_y_rx;   /**< STAT_RATE_Y register data */
    uint16_t stat_rate_z_rx;   /**< STAT_RATE_Z register data */
    uint16_t stat_acc_x_rx;    /**< STAT_ACC_X register data */
    uint16_t stat_acc_y_rx;    /**< STAT_ACC_Y register data */
    uint16_t stat_acc_z_rx;    /**< STAT_ACC_Z register data */
} SCH1_Status;

/**
 * @brief Low-pass filter configuration structure
 */
typedef struct
{
    uint8_t Rate12; /**< RATE_XYZ low-pass filter */
    uint8_t Acc12;  /**< ACC low-pass filter 1 */
    uint8_t Acc3;   /**< ACC low-pass filter for ACC_(X,Y,Z)3 */
} SCH1_Filter_t;

/**
 * @brief Sensitivity (dynamic range) configuration structure
 */
typedef struct
{
    uint8_t Rate1; /**< Sensitivity for RATE_X1, RATE_Y1, RATE_Z1 */
    uint8_t Rate2; /**< Sensitivity for RATE_X2, RATE_Y2, RATE_Z2 */
    uint8_t Acc1;  /**< Sensitivity for ACC_X1, ACC_Y1, ACC_Z1 */
    uint8_t Acc2;  /**< Sensitivity for ACC_X2, ACC_Y2, ACC_Z2 */
    uint8_t Acc3;  /**< Sensitivity for ACC_X3, ACC_Y3, ACC_Z3 */
} SCH1_Sensitivity_t;

/**
 * @brief Decimation configuration structure
 */
typedef struct
{
    uint8_t Rate2; /**< Decimation ratio for RATE_X2, RATE_Y2, RATE_Z2 */
    uint8_t Acc2;  /**< Decimation ratio for ACC_X2, ACC_Y2, ACC_Z2 */
} SCH1_Decimation_t;

/**
 * @brief Gyroscope raw unprocessed data values
 */
typedef struct
{
    int32_t x; /**< Raw x-axis gyroscope value */
    int32_t y; /**< Raw y-axis gyroscope value */
    int32_t z; /**< Raw z-axis gyroscope value */
} SCH1_raw_rate_t;

/**
 * @brief Accelerometer raw unprocessed data values
 */
typedef struct
{
    int32_t x; /**< Raw x-axis acceleration value */
    int32_t y; /**< Raw y-axis acceleration value */
    int32_t z; /**< Raw z-axis acceleration value */
} SCH1_raw_acc_t;

/**
 * @brief Temperature raw unprocessed data value
 */
typedef int32_t SCH1_raw_temp_t;

/**
 * @brief Gyroscope converted data values, in dps
 */
typedef struct
{
    float x; /**< Converted x-axis gyroscope value */
    float y; /**< Converted y-axis gyroscope value */
    float z; /**< Converted z-axis gyroscope value */
} SCH1_rate_t;

/**
 * @brief Accelerometer converted data values, in g
 */
typedef struct
{
    float x; /**< Converted x-axis acceleration value */
    float y; /**< Converted y-axis acceleration value */
    float z; /**< Converted z-axis acceleration value */
} SCH1_acc_t;

/**
 * @brief Temperature converted data value, in degrees Celsius
 */
typedef float SCH1_temp_t;

/**
 * @brief Scaled measurement results for every channel
 */
typedef struct
{
    float Rate1[3]; /**< Interpolated gyroscope values, X, Y, Z */
    float Rate2[3]; /**< Decimated gyroscope values, X, Y, Z */
    float Acc1[3];  /**< Interpolated acceleration values, X, Y, Z */
    float Acc2[3];  /**< Decimated acceleration values, X, Y, Z */
    float Acc3[3];  /**< Auxiliary acceleration values, X, Y, Z */
    float Temp;     /**< Temperature value */
} SCH1_result_t;

/**
 * @brief SCH16T sensor handle
 */
typedef struct
{
    SPI_HandleTypeDef *p_hspi; /**< SPI handle instance used to communicate with the sensor */

    uint32_t cs_pin;            /**< Chip select GPIO pin number */
    GPIO_TypeDef *cs_gpio_port; /**< Chip select GPIO port */

    uint32_t reset_pin;            /**< Reset GPIO pin number */
    GPIO_TypeDef *reset_gpio_port; /**< Reset GPIO port */

    SCH1_Filter_t filter_settings;           /**< Low-pass filter configuration */
    SCH1_Sensitivity_t sensitivity_settings;  /**< Dynamic range (sensitivity) configuration */
    SCH1_Decimation_t decimation_settings;    /**< Decimation configuration */
} SCH1Sensor_t;

/**
 * @brief Sensor serial number, as a null-terminated string
 */
typedef char SCH1_Snbr_t[11];

/* ==== Macros ==== */

/**
 * @defgroup SCH16T_SPI48BF SPI 48-bit Frame Construction
 * @{
 */

#define SPI48BF_CS_OFFSET    (46U)
#define SPI48BF_CS_MASK      (0x02ULL)
#define SPI48BF_ADDR_OFFSET  (38U)
#define SPI48BF_ADDR_MASK    (0xFFULL)
#define SPI48BF_RW_OFFSET    (37U)
#define SPI48BF_RW_MASK      (0x01ULL)
#define SPI48BF_FT_OFFSET    (35U)
#define SPI48BF_FT_MASK      (0x01ULL)
#define SPI48BF_DATAI_OFFSET (8U)
#define SPI48BF_DATAI_MASK   (0xFFFFFULL)
#define SPI48BF_CRC8_OFFSET  (0U)
#define SPI48BF_CRC8_MASK    (0xFFULL)

/** @brief Build a 48-bit SPI frame from its CS, address, R/W, frame type, data and CRC8 fields */
#define SPI_FRAME48BF(CS, ADDR, RW, FT, DATAI, CRC8)                    \
    (((uint64_t)(CS) & SPI48BF_CS_MASK) << SPI48BF_CS_OFFSET |          \
     ((uint64_t)(ADDR) & SPI48BF_ADDR_MASK) << SPI48BF_ADDR_OFFSET |    \
     ((uint64_t)(RW) & SPI48BF_RW_MASK) << SPI48BF_RW_OFFSET |          \
     ((uint64_t)(FT) & SPI48BF_FT_MASK) << SPI48BF_FT_OFFSET |          \
     ((uint64_t)(DATAI) & SPI48BF_DATAI_MASK) << SPI48BF_DATAI_OFFSET | \
     ((uint64_t)(CRC8) & SPI48BF_CRC8_MASK) << SPI48BF_CRC8_OFFSET)

/** @} */

/**
 * @defgroup SCH16T_SPI32BF SPI 32-bit Frame Construction
 * @{
 */

#define SPI32BF_CS_OFFSET    (30U)
#define SPI32BF_CS_MASK      (0x02ULL)
#define SPI32BF_ADDR_OFFSET  (22U)
#define SPI32BF_ADDR_MASK    (0xFFULL)
#define SPI32BF_RW_OFFSET    (21U)
#define SPI32BF_RW_MASK      (0x01ULL)
#define SPI32BF_FT_OFFSET    (19U)
#define SPI32BF_FT_MASK      (0x01ULL)
#define SPI32BF_DATAI_OFFSET (3U)
#define SPI32BF_DATAI_MASK   (0xFFFFULL)
#define SPI32BF_CRC3_OFFSET  (0U)
#define SPI32BF_CRC3_MASK    (0x07ULL)

/** @brief Build a 32-bit SPI frame from its CS, address, R/W, frame type, data and CRC3 fields */
#define SPI_FRAME32BF(CS, ADDR, RW, FT, DATAI, CRC3)                    \
    (((uint64_t)(CS) & SPI32BF_CS_MASK) << SPI32BF_CS_OFFSET |          \
     ((uint64_t)(ADDR) & SPI32BF_ADDR_MASK) << SPI32BF_ADDR_OFFSET |    \
     ((uint64_t)(RW) & SPI32BF_RW_MASK) << SPI32BF_RW_OFFSET |          \
     ((uint64_t)(FT) & SPI32BF_FT_MASK) << SPI32BF_FT_OFFSET |          \
     ((uint64_t)(DATAI) & SPI32BF_DATAI_MASK) << SPI32BF_DATAI_OFFSET | \
     ((uint64_t)(CRC3) & SPI32BF_CRC3_MASK) << SPI32BF_CRC3_OFFSET)

/** @} */

/**
 * @defgroup SCH16T_FRAME_FILTERS Filter Frame Construction
 * @{
 */

#define LPF_X_OFFSET (3U)
#define LPF_Y_OFFSET (6U)
#define LPF_Z_OFFSET (9U)
#define LPF_MASK     (0x03ULL)

/** @brief Build the data field of a RATE_XYZ filter setting frame */
#define RATE_XYZ_DATA_FIELD(LPF_X, LPF_Y, LPF_Z)      \
    (((uint64_t)(LPF_X) & LPF_MASK) << LPF_X_OFFSET | \
     ((uint64_t)(LPF_Y) & LPF_MASK) << LPF_Y_OFFSET | \
     ((uint64_t)(LPF_Z) & LPF_MASK) << LPF_Z_OFFSET)

/** @brief Build the data field of an ACC12 filter setting frame */
#define ACC12_DATA_FIELD(LPF_X, LPF_Y, LPF_Z)             \
    ((uint64_t)(LPF_X) & LPF_MASK) << LPF_X_OFFSET |      \
        ((uint64_t)((LPF_Y) & LPF_MASK) << LPF_Y_OFFSET | \
         ((uint64_t)(LPF_Z) & LPF_MASK) << LPF_Z_OFFSET)

/** @brief Build the data field of an ACC3 filter setting frame */
#define ACC3_DATA_FIELD(LPF_X, LPF_Y, LPF_Z)          \
    (((uint64_t)(LPF_X) & LPF_MASK) << LPF_X_OFFSET | \
     ((uint64_t)(LPF_Y) & LPF_MASK) << LPF_Y_OFFSET | \
     ((uint64_t)(LPF_Z) & LPF_MASK) << LPF_Z_OFFSET)

/** @} */

/**
 * @defgroup SCH16T_FRAME_DYN_DEC Dynamic Range and Decimation Frame Construction
 * @{
 */

#define DYN_RATE_XYZ1_OFFSET (12U)
#define DYN_RATE_XYZ2_OFFSET (19U)
#define DEC_RATE_Z2_OFFSET   (6U)
#define DEC_RATE_Y2_OFFSET   (3U)
#define DEC_RATE_X2_OFFSET   (0U)
#define DYN_RATE_XYZ1_MASK   (0x03ULL)
#define DYN_RATE_XYZ2_MASK   (0x03ULL)
#define DEC_RATE_Z2_MASK     (0x03ULL)
#define DEC_RATE_Y2_MASK     (0x03ULL)
#define DEC_RATE_X2_MASK     (0x03ULL)

#define DYN_ACC_XYZ3_OFFSET (0U)
#define DYN_ACC_XYZ3_MASK   (0x03ULL)

/** @brief Build the data field of a CTRL_RATE setting frame */
#define CTRL_RATE_DATA_FIELD(DYN_RATE_XYZ1, DYN_RATE_XYZ2, DEC_RATE_X2, DEC_RATE_Y2, DEC_RATE_Z2) \
    (((uint64_t)(DYN_RATE_XYZ1) & DYN_RATE_XYZ1_MASK) << DYN_RATE_XYZ1_OFFSET |                   \
     ((uint64_t)(DYN_RATE_XYZ2) & DYN_RATE_XYZ2_MASK) << DYN_RATE_XYZ2_OFFSET |                   \
     ((uint64_t)(DEC_RATE_X2) & DEC_RATE_X2_MASK) << DEC_RATE_X2_OFFSET |                         \
     ((uint64_t)(DEC_RATE_Y2) & DEC_RATE_Y2_MASK) << DEC_RATE_Y2_OFFSET |                         \
     ((uint64_t)(DEC_RATE_Z2) & DEC_RATE_Z2_MASK) << DEC_RATE_Z2_OFFSET)

/** @brief Build the data field of a CTRL_ACC12 setting frame */
#define CTRL_ACC12_DATA_FIELD(DYN_RATE_XYZ1, DYN_RATE_XYZ2, DEC_RATE_X2, DEC_RATE_Y2, DEC_RATE_Z2) \
    (((uint64_t)(DYN_RATE_XYZ1) & DYN_RATE_XYZ1_MASK) << DYN_RATE_XYZ1_OFFSET |                    \
     ((uint64_t)(DYN_RATE_XYZ2) & DYN_RATE_XYZ2_MASK) << DYN_RATE_XYZ2_OFFSET |                    \
     ((uint64_t)(DEC_RATE_X2) & DEC_RATE_X2_MASK) << DEC_RATE_X2_OFFSET |                          \
     ((uint64_t)(DEC_RATE_Y2) & DEC_RATE_Y2_MASK) << DEC_RATE_Y2_OFFSET |                          \
     ((uint64_t)(DEC_RATE_Z2) & DEC_RATE_Z2_MASK) << DEC_RATE_Z2_OFFSET)

/** @brief Build the data field of a CTRL_ACC3 setting frame */
#define CTRL_ACC3_DATA_FIELD(DYN_ACC_XYZ3) \
    (((uint64_t)(DYN_ACC_XYZ3) & DYN_ACC_XYZ3_MASK) << DYN_ACC_XYZ3_OFFSET)

/** @} */

/**
 * @defgroup SCH16T_FRAME_MODE Sensor Mode Control and Soft Reset Frame Construction
 * @{
 */

#define EOI_OFFSET       (1U)
#define EOI_MASK         (0x01ULL)
#define EN_SENSOR_OFFSET (0U)
#define EN_SENSOR_MASK   (0x01ULL)
#define CTRL_MODE_MASK   (0x03ULL)

/** @brief Build the data field of a CTRL_MODE setting frame */
#define CTRL_MODE_DATA_FIELD(EOI, EN_SENSOR)                          \
    ((((uint64_t)(EOI) & EOI_MASK) << EOI_OFFSET |                    \
      ((uint64_t)(EN_SENSOR) & EN_SENSOR_MASK) << EN_SENSOR_OFFSET) & \
     CTRL_MODE_MASK)

#define CTRL_RESET_DATA_FIELD (0x0000AU) /**< Soft reset data field */

/** @} */

/**
 * @defgroup SCH16T_FRAME_UTILS Frame Utility Macros
 * @{
 */

/**
 * @brief Extract a 20-bit signed data field from a received 48-bit SPI frame buffer
 *
 * @param data_buffer Buffer holding the received 48-bit SPI frame (6 bytes)
 */
#define SPI48_DATA_INT32(data_buffer) ((int32_t)(data_buffer[2] << 28 | \
                            data_buffer[3] << 20 | data_buffer[4] << 12) >> 12)

/** @brief Extract the 20-bit unsigned data field from a 48-bit SPI frame value */
#define SPI48_DATA_UINT32(a) ((uint32_t)(((a) >> 8) & 0x000fffffUL))
/** @brief Extract the 16-bit unsigned data field from a 48-bit SPI frame value */
#define SPI48_DATA_UINT16(a) ((uint16_t)(((a) >> 8) & 0x0000ffffUL))

/**
 * @brief Byte-swap a 64-bit SPI frame to the sensor's expected transmission byte order
 *
 * @details The sensor receives data byte per byte, MSB first, starting with the most significant
 *          byte of the first word and ending with the least significant byte of the sixth word.
 *          The bytes of the frame must therefore be swapped, and the result shifted right by 16
 *          bits, to align the data correctly for transmission.
 *
 * @param double_word 64-bit SPI frame to byte-swap
 */
#define DOUBLE_WORD_BYTE_SWAP(double_word) \
    (((0xff00000000000000ull & (double_word)) >> 56) |  \
    ((0x00ff000000000000ull & (double_word)) >> 40) |   \
    ((0x0000ff0000000000ull & (double_word)) >> 24) |   \
    ((0x000000ff00000000ull & (double_word)) >> 8) |    \
    ((0x00000000ff000000ull & (double_word)) << 8) |    \
    ((0x0000000000ff0000ull & (double_word)) << 24) |   \
    ((0x000000000000ff00ull & (double_word)) << 40) |   \
    ((0x00000000000000ffull & (double_word)) << 56)) >> 16

/** @} */

/* ==== Functions prototypes ==== */

/**
 * @defgroup SCH16T_FUNC_IO IO / Register Access Functions
 * @{
 */

/**
 * @brief Transmit and receive a full-duplex SPI frame
 *
 * @param sensor Handle of the sensor in use
 * @param msg 48-bit SPI frame to transmit (already byte-order swapped, cf. DOUBLE_WORD_BYTE_SWAP)
 * @param rx_buffer Buffer to store the received response
 * @param buffer_size Size of rx_buffer, in bytes
 * @return error_t
 */
error_t SCH1_TransmitReceive(SCH1Sensor_t *sensor, uint64_t msg, uint8_t *rx_buffer, uint16_t buffer_size);

/**
 * @brief Compute the CRC8 of a 48-bit SPI frame
 *
 * @param SPIframe 48-bit SPI frame (CRC field ignored)
 * @return Computed CRC8 value
 */
uint8_t CRC8(uint64_t SPIframe);

/**
 * @brief Compute the CRC3 of a 32-bit SPI frame
 *
 * @param SPIframe 32-bit SPI frame (CRC field ignored)
 * @return Computed CRC3 value
 */
uint8_t CRC3(uint32_t SPIframe);

/** @} */

/**
 * @defgroup SCH16T_FUNC_INIT Initialization and Reset Functions
 * @{
 */

/**
 * @brief Hardware-reset the sensor via the RESET pin
 *
 * @param sensor Handle of the sensor in use
 * @return Always 0
 */
uint8_t SCH1_reset(SCH1Sensor_t *sensor);

/**
 * @brief Soft-reset the sensor over SPI (CTRL_RESET register write)
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t SCH1_SPI_soft_reset(SCH1Sensor_t *sensor);

/**
 * @brief Start-up sequence: hardware reset, enable the sensor, then set EOI
 *
 * @param sensor Handle of the sensor in use
 * @return error_t
 */
error_t SCH1_init(SCH1Sensor_t *sensor);

/**
 * @brief Read and format the sensor serial number
 *
 * @param sensor Handle of the sensor in use
 * @param snbr Buffer to store the formatted serial number string
 * @return error_t
 */
error_t SCH1_getSnbr(SCH1Sensor_t *sensor, SCH1_Snbr_t *snbr);

/**
 * @brief Read all status registers of the sensor
 *
 * @param sensor Handle of the sensor in use
 * @param status Structure to store the read status register values
 * @return error_t
 */
error_t SCH1_read_status(SCH1Sensor_t *sensor, SCH1_Status *status);

/** @} */

/**
 * @defgroup SCH16T_FUNC_READ Data Read Functions
 * @{
 */

/**
 * @brief Read the raw interpolated accelerometer data (ACC_X1/Y1/Z1)
 *
 * @param sensor Handle of the sensor in use
 * @param acc1_raw_data Buffer to store the raw X, Y, Z acceleration values
 * @return error_t
 */
error_t SCH1_Read_Acc1_Data(SCH1Sensor_t *sensor, SCH1_raw_acc_t *acc1_raw_data);

/**
 * @brief Read the raw interpolated gyroscope data (RATE_X1/Y1/Z1)
 *
 * @param sensor Handle of the sensor in use
 * @param rate1_raw_data Buffer to store the raw X, Y, Z angular rate values
 * @return error_t
 */
error_t SCH1_Read_Rate1_Data(SCH1Sensor_t *sensor, SCH1_raw_rate_t *rate1_raw_data);

/**
 * @brief Read the raw temperature data (TEMP)
 *
 * @param sensor Handle of the sensor in use
 * @param temp_raw_data Buffer to store the raw temperature value
 * @return error_t
 */
error_t SCH1_Read_Temp_Data(SCH1Sensor_t *sensor, SCH1_raw_temp_t *temp_raw_data);

/** @} */

/**
 * @defgroup SCH16T_FUNC_CONVERT Data Conversion Functions
 * @{
 */

/**
 * @brief Convert raw interpolated accelerometer data to g using the sensor's configured sensitivity
 *
 * @param sensor Handle of the sensor in use
 * @param raw_acc Raw X, Y, Z acceleration values
 * @param acc Buffer to store the converted X, Y, Z acceleration values in g
 * @return 0 on success, or (uint8_t)-1 if the configured sensitivity setting is invalid
 */
uint8_t SCH1_Convert_Acc_Interpolated_Data(SCH1Sensor_t *sensor, SCH1_raw_acc_t *raw_acc, SCH1_acc_t *acc);

/**
 * @brief Convert raw interpolated gyroscope data to dps using the sensor's configured sensitivity
 *
 * @param sensor Handle of the sensor in use
 * @param raw_rate Raw X, Y, Z angular rate values
 * @param rate Buffer to store the converted X, Y, Z angular rate values in dps
 * @return 0 on success, or (uint8_t)-1 if the configured sensitivity setting is invalid
 */
uint8_t SCH1_Convert_Rate_Interpolated_Data(SCH1Sensor_t *sensor, SCH1_raw_rate_t *raw_rate, SCH1_rate_t *rate);

/**
 * @brief Convert raw temperature data to degrees Celsius
 *
 * @param sensor Handle of the sensor in use
 * @param raw_temp Raw temperature value
 * @param temp Buffer to store the converted temperature value
 * @return Always 0
 */
uint8_t SCH1_Convert_Temp_Data(SCH1Sensor_t *sensor, SCH1_raw_temp_t *raw_temp, SCH1_temp_t *temp);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SCH16T_H */
