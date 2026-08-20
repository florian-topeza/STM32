/**
 * @file        bq25798.h
 * @author      Florian Topeza
 * @brief       Header file for BQ25798 Battery Charger IC driver
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
#ifndef BQ25798_H
#define BQ25798_H

#ifdef __cplusplus
extern "C" {
#endif


/* ==== Includes ====*/

#include <stdint.h>
#include <float.h>

// Include the HAL of the STM32 microcontroller in use

#include "stm32l4xx_hal.h"

/* ==== Defines ==== */

/**
 * @defgroup BQ25798_REG_I2C I2C Configuration Constants
 * @{
 */

#define BQ25798_I2C_ADDR                   (0x6BU)  /**< I2C 7-bit address of BQ25798 */
#define BQ25798_I2C_TIMEOUT_MS              (100U)  /**< Minimum HAL timeout for I2C operations */

/** @} */


/**
 * @defgroup BQ25798_REG Register Address Offsets
 * @{
 */

/* Configuration Registers */
#define BQ25798_REG_VSYSMIN                (0x00U)  /**< Minimal System Voltage */
#define BQ25798_REG_VREG                   (0x01U)  /**< Charge Voltage Limit */
#define BQ25798_REG_ICHG                   (0x03U)  /**< Charge Current Limit */
#define BQ25798_REG_VINDPM                 (0x05U)  /**< Input Voltage Limit */
#define BQ25798_REG_IINDPM                 (0x06U)  /**< Input Current Limit */
#define BQ25798_REG_PRECHARGE_CTRL         (0x08U)  /**< Precharge Control */
#define BQ25798_REG_TERM_CTRL              (0x09U)  /**< Termination Control */
#define BQ25798_REG_RECHARGE_CTRL          (0x0AU)  /**< Re-charge Control */
#define BQ25798_REG_VOTG                   (0x0BU)  /**< OTG Mode Regulation Voltage */
#define BQ25798_REG_IOTG                   (0x0DU)  /**< OTG Current Limit */
#define BQ25798_REG_TIMER_CTRL             (0x0EU)  /**< Timer Control */
#define BQ25798_REG_CHG_CTRL_0             (0x0FU)  /**< Charger Control 0 */
#define BQ25798_REG_CHG_CTRL_1             (0x10U)  /**< Charger Control 1 */
#define BQ25798_REG_CHG_CTRL_2             (0x11U)  /**< Charger Control 2 */
#define BQ25798_REG_CHG_CTRL_3             (0x12U)  /**< Charger Control 3 */
#define BQ25798_REG_CHG_CTRL_4             (0x13U)  /**< Charger Control 4 */
#define BQ25798_REG_CHG_CTRL_5             (0x14U)  /**< Charger Control 5 */
#define BQ25798_REG_MPPT_CTRL              (0x15U)  /**< MPPT Control */
#define BQ25798_REG_TEMP_CTRL              (0x16U)  /**< Temperature Control */
#define BQ25798_REG_NTC_CTRL_0             (0x17U)  /**< NTC Control 0 */
#define BQ25798_REG_NTC_CTRL_1             (0x18U)  /**< NTC Control 1 */
#define BQ25798_REG_ICO_ILIM               (0x19U)  /**< ICO Current Limit */

/* Status Registers */
#define BQ25798_REG_CHG_STAT_0             (0x1BU)  /**< Charger Status 0 */
#define BQ25798_REG_CHG_STAT_1             (0x1CU)  /**< Charger Status 1 */
#define BQ25798_REG_CHG_STAT_2             (0x1DU)  /**< Charger Status 2 */
#define BQ25798_REG_CHG_STAT_3             (0x1EU)  /**< Charger Status 3 */
#define BQ25798_REG_CHG_STAT_4             (0x1FU)  /**< Charger Status 4 */
#define BQ25798_REG_FAULT_STAT_0           (0x20U)  /**< FAULT Status 0 */
#define BQ25798_REG_FAULT_STAT_1           (0x21U)  /**< FAULT Status 1 */

/* Flag Registers */
#define BQ25798_REG_CHG_FLAG_0             (0x22U)  /**< Charger Flag 0 */
#define BQ25798_REG_CHG_FLAG_1             (0x23U)  /**< Charger Flag 1 */
#define BQ25798_REG_CHG_FLAG_2             (0x24U)  /**< Charger Flag 2 */
#define BQ25798_REG_CHG_FLAG_3             (0x25U)  /**< Charger Flag 3 */
#define BQ25798_REG_FAULT_FLAG_0           (0x26U)  /**< FAULT Flag 0 */
#define BQ25798_REG_FAULT_FLAG_1           (0x27U)  /**< FAULT Flag 1 */

/* Mask Registers */
#define BQ25798_REG_CHG_MASK_0             (0x28U)  /**< Charger Mask 0 */
#define BQ25798_REG_CHG_MASK_1             (0x29U)  /**< Charger Mask 1 */
#define BQ25798_REG_CHG_MASK_2             (0x2AU)  /**< Charger Mask 2 */
#define BQ25798_REG_CHG_MASK_3             (0x2BU)  /**< Charger Mask 3 */
#define BQ25798_REG_FAULT_MASK_0           (0x2CU)  /**< FAULT Mask 0 */
#define BQ25798_REG_FAULT_MASK_1           (0x2DU)  /**< FAULT Mask 1 */

/* ADC Control Registers */
#define BQ25798_REG_ADC_CTRL               (0x2EU)  /**< ADC Control */
#define BQ25798_REG_ADC_FUNC_DIS_0         (0x2FU)  /**< ADC Function Disable 0 */
#define BQ25798_REG_ADC_FUNC_DIS_1         (0x30U)  /**< ADC Function Disable 1 */

/* ADC Data Registers */
#define BQ25798_REG_IBUS_ADC               (0x31U)  /**< IBUS ADC */
#define BQ25798_REG_IBAT_ADC               (0x33U)  /**< IBAT ADC */
#define BQ25798_REG_VBUS_ADC               (0x35U)  /**< VBUS ADC */
#define BQ25798_REG_VAC1_ADC               (0x37U)  /**< VAC1 ADC */
#define BQ25798_REG_VAC2_ADC               (0x39U)  /**< VAC2 ADC */
#define BQ25798_REG_VBAT_ADC               (0x3BU)  /**< VBAT ADC */
#define BQ25798_REG_VSYS_ADC               (0x3DU)  /**< VSYS ADC */
#define BQ25798_REG_TS_ADC                 (0x3FU)  /**< TS ADC */
#define BQ25798_REG_TDIE_ADC               (0x41U)  /**< TDIE ADC */
#define BQ25798_REG_DP_ADC                 (0x43U)  /**< D+ ADC */
#define BQ25798_REG_DM_ADC                 (0x45U)  /**< D- ADC */

/* DPDM and Part Info Registers */
#define BQ25798_REG_DPDM_DRV               (0x47U)  /**< DPDM Driver */
#define BQ25798_REG_PART_INFO              (0x48U)  /**< Part Information */

/** @} */


/**
 * @defgroup BQ25798_REG_VSYSMIN Minimal System Voltage Register (REG00)
 * @{
 */

#define BQ25798_VSYSMIN_MASK               (0x3FU)  /**< VSYSMIN_5:0 mask */
#define BQ25798_VSYSMIN_MIN_MV             (2500U)  /**< Minimum VSYSMIN value in mV */
#define BQ25798_VSYSMIN_MAX_MV             (16000U) /**< Maximum VSYSMIN value in mV */
#define BQ25798_VSYSMIN_OFFSET_MV          (2500U)  /**< VSYSMIN offset in mV */
#define BQ25798_VSYSMIN_STEP_MV            (250U)   /**< VSYSMIN step size in mV */

#define BQ25798_VSYSMIN_1_CELL             (3500U)  /**< VSYSMIN for single cell configuration in mV */
#define BQ25798_VSYSMIN_2_CELLS            (7000U)  /**< VSYSMIN for two cells configuration in mV */
#define BQ25798_VSYSMIN_3_CELLS            (9000U)  /**< VSYSMIN for three cells configuration in mV */
#define BQ25798_VSYSMIN_4_CELLS            (12000U) /**< VSYSMIN for four cells configuration in mV */

/** @} */


/**
 * @defgroup BQ25798_REG_VREG Charge Voltage Limit Register (REG01)
 * @{
 */

#define BQ25798_VREG_MASK                  (0x07FFU) /**< VREG_10:0 mask */
#define BQ25798_VREG_MIN_MV                (3000U)   /**< Minimum VREG value in mV */
#define BQ25798_VREG_MAX_MV                (18800U)  /**< Maximum VREG value in mV */
#define BQ25798_VREG_OFFSET_MV             (0U)      /**< VREG offset in mV */
#define BQ25798_VREG_STEP_MV               (10U)     /**< VREG step size in mV */

#define BQ25798_VREG_1_CELL                (4200U)  /**< VREG for single cell configuration in mV */
#define BQ25798_VREG_2_CELLS               (8400U)  /**< VREG for two cells configuration in mV */
#define BQ25798_VREG_3_CELLS               (12600U) /**< VREG for three cells configuration in mV */
#define BQ25798_VREG_4_CELLS               (16800U) /**< VREG for four cells configuration in mV */


/** @} */


/**
 * @defgroup BQ25798_REG_ICHG Charge Current Limit Register (REG03)
 * @{
 */

#define BQ25798_ICHG_MASK                  (0x01FFU) /**< ICHG_8:0 mask */
#define BQ25798_ICHG_MIN_MA                (50U)     /**< Minimum ICHG value in mA */
#define BQ25798_ICHG_MAX_MA                (5000U)   /**< Maximum ICHG value in mA */
#define BQ25798_ICHG_OFFSET_MA             (0U)      /**< ICHG offset in mA */
#define BQ25798_ICHG_STEP_MA               (10U)     /**< ICHG step size in mA */

/** @} */


/**
 * @defgroup BQ25798_REG_VINDPM Input Voltage Limit Register (REG05)
 * @{
 */

#define BQ25798_VINDPM_MASK                (0xFFU)   /**< VINDPM_7:0 mask */
#define BQ25798_VINDPM_MIN_MV              (3600U)   /**< Minimum VINDPM value in mV */
#define BQ25798_VINDPM_MAX_MV              (22000U)  /**< Maximum VINDPM value in mV */
#define BQ25798_VINDPM_OFFSET_MV           (0U)      /**< VINDPM offset in mV */
#define BQ25798_VINDPM_STEP_MV             (100U)    /**< VINDPM step size in mV */

/** @} */


/**
 * @defgroup BQ25798_REG_IINDPM Input Current Limit Register (REG06)
 * @{
 */

#define BQ25798_IINDPM_MASK                (0x01FFU) /**< IINDPM_8:0 mask */
#define BQ25798_IINDPM_MIN_MA              (100U)    /**< Minimum IINDPM value in mA */
#define BQ25798_IINDPM_MAX_MA              (3300U)   /**< Maximum IINDPM value in mA */
#define BQ25798_IINDPM_OFFSET_MA           (0U)      /**< IINDPM offset in mA */
#define BQ25798_IINDPM_STEP_MA             (10U)     /**< IINDPM step size in mA */

/** @} */


/**
 * @defgroup BQ25798_REG_PRECHARGE_CTRL Precharge Control Register (REG08)
 * @{
 */

#define BQ25798_VBAT_LOWV_MASK             (0xC0U)   /**< VBAT_LOWV_1:0 mask */
#define BQ25798_VBAT_LOWV_SHIFT            (6U)      /**< VBAT_LOWV shift value */
#define BQ25798_IPRECHG_MASK               (0x3FU)   /**< IPRECHG_5:0 mask */
#define BQ25798_IPRECHG_MIN_MA             (40U)     /**< Minimum IPRECHG value in mA */
#define BQ25798_IPRECHG_MAX_MA             (2000U)   /**< Maximum IPRECHG value in mA */
#define BQ25798_IPRECHG_OFFSET_MA          (0U)      /**< IPRECHG offset in mA */
#define BQ25798_IPRECHG_STEP_MA            (40U)     /**< IPRECHG step size in mA */

/** @} */


/**
 * @defgroup BQ25798_REG_TERM_CTRL Termination Control Register (REG09)
 * @{
 */

#define BQ25798_REG_RST_BIT                (0x40U)   /**< REG_RST bit mask */
#define BQ25798_REG_RST_SHIFT              (6U)      /**< REG_RST bit shift */
#define BQ25798_STOP_WD_CHG_BIT            (0x20U)   /**< STOP_WD_CHG bit mask */
#define BQ25798_ITERM_MASK                 (0x1FU)   /**< ITERM_4:0 mask */
#define BQ25798_ITERM_MIN_MA               (40U)     /**< Minimum ITERM value in mA */
#define BQ25798_ITERM_MAX_MA               (1000U)   /**< Maximum ITERM value in mA */
#define BQ25798_ITERM_OFFSET_MA            (0U)      /**< ITERM offset in mA */
#define BQ25798_ITERM_STEP_MA              (40U)     /**< ITERM step size in mA */

#define BQ25798_RST                        (1U)      /**< Reset charger */

/** @} */


/**
 * @defgroup BQ25798_REG_RECHARGE_CTRL Re-charge Control Register (REG0A)
 * @{
 */

#define BQ25798_CELL_MASK                  (0xC0U)   /**< CELL_1:0 mask */
#define BQ25798_CELL_SHIFT                 (6U)      /**< CELL shift value */
#define BQ25798_TRECHG_MASK                (0x30U)   /**< TRECHG_1:0 mask */
#define BQ25798_TRECHG_SHIFT               (4U)      /**< TRECHG shift value */
#define BQ25798_VRECHG_MASK                (0x0FU)   /**< VRECHG_3:0 mask */
#define BQ25798_VRECHG_MIN_MV              (50U)     /**< Minimum recharge threshold offest value in mV */
#define BQ25798_VRECHG_MAX_MV              (800U)    /**< Maximum recharge threshold offest value in mV */
#define BQ25798_VRECHG_OFFSET_MV           (50U)     /**< VRECHG offset in mV */
#define BQ25798_VRECHG_STEP_MV             (50U)     /**< VRECHG step size in mV */

#define BQ25798_1_CELL                     (0x00U)   /**< Single cell configuration */
#define BQ25798_2_CELLS                    (0x01U)   /**< Two cells configuration */
#define BQ25798_3_CELLS                    (0x02U)   /**< Three cells configuration */
#define BQ25798_4_CELLS                    (0x03U)   /**< Four cells configuration */

/** @} */


/**
 * @defgroup BQ25798_REG_VOTG OTG Voltage Regulation Register (REG0B)
 * @{
 */

#define BQ25798_VOTG_MASK                  (0x07FFU) /**< VOTG_10:0 mask */
#define BQ25798_VOTG_MIN_MV                (2800U)   /**< Minimum VOTG value in mV */
#define BQ25798_VOTG_MAX_MV                (22000U)  /**< Maximum VOTG value in mV */
#define BQ25798_VOTG_OFFSET_MV             (2800U)   /**< VOTG offset in mV */
#define BQ25798_VOTG_STEP_MV               (10U)     /**< VOTG step size in mV */

/** @} */


/**
 * @defgroup BQ25798_REG_IOTG OTG Current Limit Register (REG0D)
 * @{
 */

#define BQ25798_PRECHG_TMR_BIT             (0x80U)   /**< PRECHG_TMR bit mask */
#define BQ25798_IOTG_MASK                  (0x7FU)   /**< IOTG_6:0 mask */
#define BQ25798_IOTG_MIN_MA                (160U)    /**< Minimum IOTG value in mA */
#define BQ25798_IOTG_MAX_MA                (3360U)   /**< Maximum IOTG value in mA */
#define BQ25798_IOTG_OFFSET_MA             (0U)      /**< IOTG offset in mA */
#define BQ25798_IOTG_STEP_MA               (40U)     /**< IOTG step size in mA */

/** @} */


/**
 * @defgroup BQ25798_REG_TIMER_CTRL Timer Control Register (REG0E)
 * @{
 */

#define BQ25798_TOPOFF_TMR_MASK            (0xC0U)   /**< TOPOFF_TMR_1:0 mask */
#define BQ25798_TOPOFF_TMR_SHIFT           (6U)      /**< TOPOFF_TMR shift value */
#define BQ25798_EN_TRICHG_TMR_BIT          (0x20U)   /**< EN_TRICHG_TMR bit mask */
#define BQ25798_EN_PRECHG_TMR_BIT          (0x10U)   /**< EN_PRECHG_TMR bit mask */
#define BQ25798_EN_CHG_TMR_BIT             (0x08U)   /**< EN_CHG_TMR bit mask */
#define BQ25798_CHG_TMR_MASK               (0x06U)   /**< CHG_TMR_1:0 mask */
#define BQ25798_CHG_TMR_SHIFT              (1U)      /**< CHG_TMR shift value */
#define BQ25798_TMR2X_EN_BIT               (0x01U)   /**< TMR2X_EN bit mask */

/** @} */


/**
 * @defgroup BQ25798_REG_CHG_CTRL_0 Charger Control 0 Register (REG0F)
 * @{
 */

#define BQ25798_EN_AUTO_IBATDIS_BIT        (0x80U)   /**< EN_AUTO_IBATDIS bit mask */
#define BQ25798_FORCE_IBATDIS_BIT          (0x40U)   /**< FORCE_IBATDIS bit mask */
#define BQ25798_EN_CHG_BIT                 (0x20U)   /**< EN_CHG bit mask */
#define BQ25798_EN_ICO_BIT                 (0x10U)   /**< EN_ICO bit mask */
#define BQ25798_FORCE_ICO_BIT              (0x08U)   /**< FORCE_ICO bit mask */
#define BQ25798_EN_HIZ_BIT                 (0x04U)   /**< EN_HIZ bit mask */
#define BQ25798_EN_TERM_BIT                (0x02U)   /**< EN_TERM bit mask */
#define BQ25798_EN_BACKUP_BIT              (0x01U)   /**< EN_BACKUP bit mask */
#define BQ25798_EN_CHG_BIT_SHIFT           (5U)     /**< Enable charge bit shift */

#define BQ25798_ENABLE_BACKUP_MODE         (1U)      /**< Enable backup mode */
#define BQ25798_DISABLE_BACKUP_MODE        (0U)      /**< Disable backup mode */
#define BQ25798_ENABLE_CHG                 (1U)      /**< Enable charge (if CE pin is hold low) */
#define BQ25798_DISABLE_CHG                (0U)      /**< Disable charge */

/** @} */


/**
 * @defgroup BQ25798_REG_CHG_CTRL_1 Charger Control 1 Register (REG10)
 * @{
 */

#define BQ25798_VBUS_BACKUP_MASK           (0xC0U)   /**< VBUS_BACKUP_1:0 mask */
#define BQ25798_VBUS_BACKUP_SHIFT          (6U)      /**< VBUS_BACKUP shift value */
#define BQ25798_VAC_OVP_MASK               (0x30U)   /**< VAC_OVP_1:0 mask */
#define BQ25798_VAC_OVP_SHIFT              (4U)      /**< VAC_OVP shift value */
#define BQ25798_WD_RST_BIT                 (0x08U)   /**< WD_RST bit mask */
#define BQ25798_WD_RST_SHIFT               (1U)      /**< WD_RST shift value */
#define BQ25798_WATCHDOG_MASK              (0x07U)   /**< WATCHDOG_2:0 mask */
#define BQ25798_WD_SHIFT                   (0U)      /**< WATCHDOG_2:0 shift */

#define BQ25798_WD_RST                     (1U)      /**< Reset Watchdog */

#define BQ25798_WATCHDOG_DISABLE           (0x00U)   /**< WATCHDOG_2:0 bits configuration to disable watchdog */
#define BQ25798_WATCHDOG_05S               (0x01U)   /**< WATCHDOG_2:0 bits configuration to set watchdog timer to 0.5s */
#define BQ25798_WATCHDOG_1S                (0x02U)   /**< WATCHDOG_2:0 bits configuration to set watchdog timer to 1s */
#define BQ25798_WATCHDOG_2S                (0x03U)   /**< WATCHDOG_2:0 bits configuration to set watchdog timer to 2s */
#define BQ25798_WATCHDOG_20S               (0x04U)   /**< WATCHDOG_2:0 bits configuration to set watchdog timer to 20s */
#define BQ25798_WATCHDOG_40S               (0x05U)   /**< WATCHDOG_2:0 bits configuration to set watchdog timer to 40s */
#define BQ25798_WATCHDOG_80S               (0x06U)   /**< WATCHDOG_2:0 bits configuration to set watchdog timer to 80s */
#define BQ25798_WATCHDOG_160S              (0x07U)   /**< WATCHDOG_2:0 bits configuration to set watchdog timer to 160s */

/** @} */


/**
 * @defgroup BQ25798_REG_CHG_CTRL_2 Charger Control 2 Register (REG11)
 * @{
 */

#define BQ25798_FORCE_INDET_BIT            (0x80U)   /**< FORCE_INDET bit mask */
#define BQ25798_AUTO_INDET_EN_BIT          (0x40U)   /**< AUTO_INDET_EN bit mask */
#define BQ25798_EN_12V_BIT                 (0x20U)   /**< EN_12V bit mask */
#define BQ25798_EN_9V_BIT                  (0x10U)   /**< EN_9V bit mask */
#define BQ25798_HVDCP_EN_BIT               (0x08U)   /**< HVDCP_EN bit mask */
#define BQ25798_SDRV_CTRL_MASK             (0x06U)   /**< SDRV_CTRL_1:0 mask */
#define BQ25798_SDRV_CTRL_SHIFT            (1U)      /**< SDRV_CTRL shift value */
#define BQ25798_SDRV_DLY_BIT               (0x01U)   /**< SDRV_DLY bit mask */

#define BQ25798_SFET_IDLE                  (0x00U)   /**< Ship FET controlled to idle mode */
#define BQ25798_SFET_SHUTDOWN              (0x01U)   /**< Ship FET controlled to shutdown mode */
#define BQ25798_SFET_SHIP                  (0x02U)   /**< Ship FET controlled to ship mode */
#define BQ25798_SFET_PWR_RST               (0x03U)   /**< Ship FET controlled to power reset */

/** @} */


/**
 * @defgroup BQ25798_REG_CHG_CTRL_3 Charger Control 3 Register (REG12)
 * @{
 */

#define BQ25798_DIS_ACDRV_BIT              (0x80U)   /**< DIS_ACDRV bit mask */
#define BQ25798_EN_OTG_BIT                 (0x40U)   /**< EN_OTG bit mask */
#define BQ25798_EN_OTG_BIT_SHIFT           (6U)      /**< EN_OTG bit shift */
#define BQ25798_PFM_OTG_DIS_BIT            (0x20U)   /**< PFM_OTG_DIS bit mask */
#define BQ25798_PFM_FWD_DIS_BIT            (0x10U)   /**< PFM_FWD_DIS bit mask */
#define BQ25798_WKUP_DLY_BIT               (0x08U)   /**< WKUP_DLY bit mask */
#define BQ25798_DIS_LDO_BIT                (0x04U)   /**< DIS_LDO bit mask */
#define BQ25798_DIS_OTG_OOA_BIT            (0x02U)   /**< DIS_OTG_OOA bit mask */
#define BQ25798_DIS_FWD_OOA_BIT            (0x01U)   /**< DIS_FWD_OOA bit mask */

#define BQ25798_EN_OTG                     (1U)      /**< Enable OTG */
#define BQ25798_DISABLE_OTG                (0U)      /**< Disable OTG */

/** @} */


/**
 * @defgroup BQ25798_REG_CHG_CTRL_4 Charger Control 4 Register (REG13)
 * @{
 */

#define BQ25798_EN_ACDRV2_BIT              (0x80U)   /**< EN_ACDRV2 bit mask */
#define BQ25798_EN_ACDRV1_BIT              (0x40U)   /**< EN_ACDRV1 bit mask */
#define BQ25798_PWM_FREQ_BIT               (0x20U)   /**< PWM_FREQ bit mask */
#define BQ25798_DIS_STAT_BIT               (0x10U)   /**< DIS_STAT bit mask */
#define BQ25798_DIS_VSYS_SHORT_BIT         (0x08U)   /**< DIS_VSYS_SHORT bit mask */
#define BQ25798_DIS_VOTG_UVP_BIT           (0x04U)   /**< DIS_VOTG_UVP bit mask */
#define BQ25798_FORCE_VINDPM_DET_BIT       (0x02U)   /**< FORCE_VINDPM_DET bit mask */
#define BQ25798_EN_IBUS_OCP_BIT            (0x01U)   /**< EN_IBUS_OCP bit mask */

#define BQ25798_PWM_FREQ_BIT_OFFSET        (5U)      /**< PWM_FREQ byt offset */
#define BQ25798_ENABLE_750KHZ_FREQ         (1U)      /**< Enable 750kHz switching frequency of the regulator */
#define BQ25798_DISABLE_750KHZ_FREQ        (0U)      /**< Disable 750kHz switching frequency of the regulator */

#define BQ25798_EN_ACDRV1_BIT_SHIFT        (6U)     /**< EN ACDRV1 bit shift */
#define BQ25798_EN_ACDRV2_BIT_SHIFT        (7U)     /**< EN ACDRV1 bit shift */

/** @} */


/**
 * @defgroup BQ25798_REG_CHG_CTRL_5 Charger Control 5 Register (REG14)
 * @{
 */

#define BQ25798_SFET_PRESENT_BIT           (0x80U)   /**< SFET_PRESENT bit mask */
#define BQ25798_EN_IBAT_BIT                (0x20U)   /**< EN_IBAT bit mask */
#define BQ25798_IBAT_REG_MASK              (0x18U)   /**< IBAT_REG_1:0 mask */
#define BQ25798_IBAT_REG_SHIFT             (3U)      /**< IBAT_REG shift value */
#define BQ25798_EN_IINDPM_BIT              (0x04U)   /**< EN_IINDPM bit mask */
#define BQ25798_EN_EXTILIM_BIT             (0x02U)   /**< EN_EXTILIM bit mask */
#define BQ25798_EN_BATOC_BIT               (0x01U)   /**< EN_BATOC bit mask */

#define BQ25798_SFET_PRESENT_BIT_SHIFT     (7U)     /**< Ship FET present bit shift */
#define BQ25798_EN_IBAT_BIT_SHIFT          (5U)     /**< EN_IBAT bit shift */
#define BQ25798_EN_IBAT                    (1U)     /**< Enable IBAT */
#define BQ25798_DIS_IBAT                   (0U)     /**< Disable IBAT */

#define BQ25798_SFET_PRES                  (1U)     /**< Set Ship FET present */

/** @} */


/**
 * @defgroup BQ25798_REG_MPPT_CTRL MPPT Control Register (REG15)
 * @{
 */

#define BQ25798_VOC_PCT_MASK               (0xE0U)   /**< VOC_PCT_2:0 mask */
#define BQ25798_VOC_PCT_SHIFT              (5U)      /**< VOC_PCT shift value */
#define BQ25798_VOC_DLY_MASK               (0x18U)   /**< VOC_DLY_1:0 mask */
#define BQ25798_VOC_DLY_SHIFT              (3U)      /**< VOC_DLY shift value */
#define BQ25798_VOC_RATE_MASK              (0x06U)   /**< VOC_RATE_1:0 mask */
#define BQ25798_VOC_RATE_SHIFT             (1U)      /**< VOC_RATE shift value */
#define BQ25798_EN_MPPT_BIT                (0x01U)   /**< EN_MPPT bit mask */

/** @} */

/**
 * @defgroup BQ25798_REG_NTC_CONTROL_1 Register (REG18)
 * @{
 */

#define BQ25798_IGNORE_TS                  (0x01U)  /**< Ignore TS */

 /** @} */

/**
 * @defgroup BQ25798_REG_TEMP_CTRL Temperature Control Register (REG16)
 * @{
 */

#define BQ25798_TREG_MASK                  (0xC0U)   /**< TREG_1:0 mask */
#define BQ25798_TREG_SHIFT                 (6U)      /**< TREG shift value */
#define BQ25798_TSHUT_MASK                 (0x30U)   /**< TSHUT_1:0 mask */
#define BQ25798_TSHUT_SHIFT                (4U)      /**< TSHUT shift value */
#define BQ25798_VBUS_PD_EN_BIT             (0x08U)   /**< VBUS_PD_EN bit mask */
#define BQ25798_VAC1_PD_EN_BIT             (0x04U)   /**< VAC1_PD_EN bit mask */
#define BQ25798_VAC2_PD_EN_BIT             (0x02U)   /**< VAC2_PD_EN bit mask */
#define BQ25798_BKUP_ACFET1_ON_BIT         (0x01U)   /**< BKUP_ACFET1_ON bit mask */
#define BQ25798_BKUP_ACFET1_ON_SHIFT       (0U)      /**< BKUP_ACFET1_ON bit shift */

#define BQ25798_BKUP_ACFET_ON              (1U)     /**< BKUP_ACFET1_ON enabled */
#define BQ25798_BKUP_ACFET_OFF             (0U)     /**< BKUP_ACFET1_ON disabled */

/** @} */


/**
 * @defgroup BQ25798_ADC_RANGES ADC Measurement Ranges
 * @{
 */

#define BQ25798_IBUS_MAX_MA                (5000U)   /**< Maximum IBUS measurement in mA */
#define BQ25798_IBAT_MAX_MA                (8000U)   /**< Maximum IBAT measurement in mA */
#define BQ25798_VBUS_MAX_MV                (30000U)  /**< Maximum VBUS measurement in mV */
#define BQ25798_VAC_MAX_MV                 (30000U)  /**< Maximum VAC measurement in mV */
#define BQ25798_VBAT_MAX_MV                (20000U)  /**< Maximum VBAT measurement in mV */
#define BQ25798_VSYS_MAX_MV                (24000U)  /**< Maximum VSYS measurement in mV */
#define BQ25798_TDIE_MIN_C                 (-40)     /**< Minimum TDIE measurement in°C */
#define BQ25798_TDIE_MAX_C                 (150)     /**< Maximum TDIE measurement in°C */
#define BQ25798_DP_DM_MAX_MV               (3600U)   /**< Maximum D+/D- measurement in mV */

/** @} */

/**
 * @defgroup BQ25798_ADC_RANGES ADC step sizes
 * @{
 */

#define BQ25798_ADC_TDIE_SHIFT             (1)          /**< Shift TDIE of 1 instead of multiplying by 0,5 */
#define BQ25798_ADC_TS_STEP_NUM            (976563)     /**< TS bit step size numerator */
#define BQ25798_ADC_TS_STEP_DEN            (10000000)   /**< TS bit step size denominator */

 /** @} */


/**
 * @defgroup BQ25798 ADC configuration masks and offsets
 * @{
 */

#define BQ25798_ADC_EN_MASK                (0x80)  /**< ADC enable bit mask */
#define BQ25798_ADC_RATE_MASK              (0x40)  /**< ADC rate conrol bit mask */
#define BQ25798_ADC_RES_MASK               (0x30)  /**< ADC resolution control bits mask */
#define BQ25798_ADC_AVG_MASK               (0x08)  /**< ADC average control bit mask */
#define BQ25798_ADC_AVG_INIT_MASK          (0x04)  /**< ADC average init control bit mask */

#define BQ25798_ADC_EN_OFFSET              (7U)
#define BQ25798_ADC_RATE_OFFSET            (6U)
#define BQ25798_ADC_RES_OFFSET             (4U)
#define BQ25798_ADC_AVG_OFFSET             (3U)
#define BQ25798_ADC_AVG_INIT_OFFSET        (2U)

 /** @} */

 /**
 * @defgroup BQ25798_DEFAULTS ADC configuration bits
 * @{
 */

#define BQ25798_ADC_EN                     (1U)     /**< ADC enable bit */
#define BQ25798_ADC_DISABLE                (0U)     /**< ADC disable bit */
#define BQ25798_ADC_ONE_SHOT               (1U)     /**< ADC one shot rate bit */
#define BQ25798_ADC_CONTINUOUS             (0U)     /**< ADC continuous rate bit */
#define BQ25798_ADC_RES_15_BITS            (0x0U)   /**< ADC 15 bit resolution */
#define BQ25798_ADC_RES_14_BITS            (0x1U)   /**< ADC 14 bit resolution */
#define BQ25798_ADC_RES_13_BITS            (0x2U)   /**< ADC 13 bit resolution */
#define BQ25798_ADC_RES_12_BITS            (0x3U)   /**< ADC 12 bit resolution (not recommended) */
#define BQ25798_ADC_SINGLE_AVG             (0U)     /**< ADC single average bit */
#define BQ25798_ADC_RUNNING_AVG            (1U)     /**< ADC running average bit */
#define BQ25798_ADC_AVG_EXISTING           (0U)     /**< ADC start average with existing value */
#define BQ25798_ADC_AVG_NEW                (1U)     /**< ADC start aveage with new value */

/** @} */

/**
* @defgroup BQ25798 ADC disable functions
*/

#define BQ25798_IBUS_ADC_DIS               (0x80U)  /**< Disable IBUS ADC measurement */
#define BQ25798_IBAT_ADC_DIS               (0x40U)  /**< Disable IBAT ADC measurement */
#define BQ25798_VBUS_ADC_DIS               (0x20U)  /**< Disable VBUS ADC measurement */
#define BQ25798_VBAT_ADC_DIS               (0x10U)  /**< Disable VBAT ADC measurement */
#define BQ25798_VSYS_ADC_DIS               (0x08U)  /**< Disable VSYS ADC measurement */
#define BQ25798_TS_ADC_DIS                 (0x04U)  /**< Disable TS ADC measurement */
#define BQ25798_TDIE_ADC_DIS               (0x01U)  /**< Disable TDIE ADC measurement */

#define BQ25798_DP_ADC_DIS                 (0x80U)  /**< Disable D+ ADC measurement */
#define BQ25798_DM_ADC_DIS                 (0x40U)  /**< Disable D- ADC measurement */
#define BQ25798_VAC2_ADC_DIS               (0x20U)  /**< Disable VAC2 ADC measurement */
#define BQ25798_VAC1_ADC_DIS               (0x10U)  /**< Disable VAC1 ADC measurement */

/** @} */

/**
 * @defgroup BQ25798_DEFAULTS Default Values
 * @{
 */

#define BQ25798_DEFAULT_VREG_CV_MV         (4200U)   /**< Default charge voltage for 1S */
#define BQ25798_DEFAULT_ICHG_MA            (1000U)   /**< Default charge current in mA */
#define BQ25798_DEFAULT_IINDPM_MA          (3000U)   /**< Default input current limit in mA */
#define BQ25798_DEFAULT_VINDPM_MV          (3600U)   /**< Default input voltage limit in mV */

/** @} */

/**
 * @defgroup BQ25798 Status bits
 * @{
 */

#define BQ25798_INDPM_STATUS_BIT_MASK              (0x80U)  /**< INDPM Status bit mask */
#define BQ25798_INDPM_STATUS_BIT_SHIFT             (7U)     /**< INDPM Status bit shift */
#define BQ25798_VINDPM_STATUS_BIT_MASK             (0x40U)  /**< VINDPM Status bit mask */
#define BQ25798_VINDPM_STATUS_BIT_SHIFT            (6U)     /**< VINDPM Status bit shift */
#define BQ25798_WD_STATUS_BIT_MASK                 (0x20U)  /**< Watchdog Status bit mask */
#define BQ25798_WD_STATUS_BIT_SHIFT                (5U)     /**< Watchdog Status bit shift */
#define BQ25798_POOR_SRC_STATUS_BIT_MASK           (0x10U)  /**< Poor Source Status bit mask */
#define BQ25798_POOR_SRC_STATUS_BIT_SHIFT          (4U)     /**< Poor Source Status bit shift */
#define BQ25798_PG_STATUS_BIT_MASK                 (0x08U)  /**< Power Good Status bit mask */
#define BQ25798_PG_STATUS_BIT_SHIFT                (3U)     /**< Power Good Status bit shift */
#define BQ25798_AC2_PRESENT_STATUS_BIT_MASK        (0x04U)  /**< AC2 Present Status bit mask */
#define BQ25798_AC2_PRESENT_STATUS_BIT_SHIFT       (2U)     /**< AC2 Present Status bit */
#define BQ25798_AC1_PRESENT_STATUS_BIT_MASK        (0x02U)  /**< AC1 Present Status bit mask */
#define BQ25798_AC1_PRESENT_STATUS_BIT_SHIFT       (1U)     /**< AC1 Present Status bit */
#define BQ25798_VBUS_PRESENT_STATUS_BIT_MASK       (0x01U)  /**< VBUS Present Status bit mask */
#define BQ25798_VBUS_PRESENT_STATUS_BIT_SHIFT      (0U)     /**< VBUS Present Status bit */

#define BQ25798_CHG_STATUS_BIT_MASK                (0xE0U)  /**< Charge Status bit mask */
#define BQ25798_CHG_STATUS_BIT_SHIFT               (5U)     /**< Charge Status bit shift */
#define BQ25798_VBUS_STAT_BIT_MASK                 (0x1EU)  /**< VBUS Status bit mask */
#define BQ25798_VBUS_STAT_BIT_SHIFT                (1U)     /**< VBUS Status bit shift */
#define BQ25798_ADAPTER_DETECT_STATUS_BIT_MASK     (0x01U)  /**< Adapter Detect Status bit mask */
#define BQ25798_ADAPTER_DETECT_STATUS_BIT_SHIFT    (0U)     /**< Adapter Detect Status bit */

#define BQ25798_ICO_STATUS_BIT_MASK                (0xC0U)  /**< ICO Status bit mask */
#define BQ25798_ICO_STATUS_BIT_SHIFT               (6U)     /**< ICO Status bit shift */
#define BQ25798_TREG_STATUS_BIT_MASK               (0x04U)  /**< TREG Status bit mask */
#define BQ25798_TREG_STATUS_BIT_SHIFT              (2U)     /**< TREG Status bit shift */
#define BQ25798_DPDM_STATUS_BIT_MASK               (0x02U)  /**< DPDM Status bit mask */
#define BQ25798_DPDM_STATUS_BIT_SHIFT              (1U)     /**< DPDM Status bit shift */
#define BQ25798_VBAT_PRESENT_STATUS_BIT_MASK       (0x01U)  /**< VBAT Present Status bit mask */
#define BQ25798_VBAT_PRESENT_STATUS_BIT_SHIFT      (0U)     /**< VBAT Present Status bit */

#define BQ25798_ACFET2_RBFET2_STATUS_BIT_MASK      (0x80U)  /**< ACFET2_RBFET2 Status bit mask */
#define BQ25798_ACFET2_RBFET2_STATUS_BIT_SHIFT     (7U)     /**< ACFET2_RBFET2 Status bit shift */
#define BQ25798_ACFET1_RBFET1_STATUS_BIT_MASK      (0x40U)  /**< ACFET1_RBFET1 Status bit mask */
#define BQ25798_ACFET1_RBFET1_STATUS_BIT_SHIFT     (6U)     /**< ACFET1_RBFET1 Status bit shift */
#define BQ25798_ADC_CONV_STATUS_BIT_MASK           (0x20U)  /**< ADC Conversion Status bit mask */
#define BQ25798_ADC_CONV_STATUS_BIT_SHIFT          (5U)     /**< ADC Conversion Status bit shift */
#define BQ25798_VSYS_STATUS_BIT_MASK               (0x10U)  /**< VSYS Status bit mask */
#define BQ25798_VSYS_STATUS_BIT_SHIFT              (4U)     /**< VSYS Status bit shift */
#define BQ25798_CHG_SAFETY_TIM_STATUS_BIT_MASK     (0x08U)  /**< Charge Safety Timer Status bit mask */
#define BQ25798_CHG_SAFETY_TIM_STATUS_BIT_SHIFT    (3U)     /**< Charge Safety Timer Status bit shift */
#define BQ25798_TRICKLE_TIM_STATUS_BIT_MASK        (0x04U)  /**< Trickle Timer Status bit mask */
#define BQ25798_TRICKLE_TIM_STATUS_BIT_SHIFT       (2U)     /**< Trickle Timer Status bit shift */
#define BQ25798_PRECHG_TIM_STATUS_BIT_MASK         (0x02U)  /**< Pre-charge Timer Status bit mask */
#define BQ25798_PRECHG_TIM_STATUS_BIT_SHIFT        (1U)     /**< Pre-charge Timer Status bit shift */
#define BQ25798_TOPOFF_TIM_STATUS_BIT_MASK         (0x01U)  /**< Top-off Timer Status bit mask */
#define BQ25798_TOPOFF_TIM_STATUS_BIT_SHIFT        (0U)     /**< Top-off Timer Status bit shift */

#define BQ25798_VBATOTG_LOWV_STATUS_BIT_MASK       (0x10U)  /**< VBATOTG_LOWV Status bit mask */
#define BQ25798_VBATOTG_LOWV_STATUS_BIT_SHIFT      (4U)     /**< VBATOTG_LOWV Status bit shift */
#define BQ25798_TS_COLD_STATUS_BIT_MASK            (0x08U)  /**< TS_COLD Status bit mask */
#define BQ25798_TS_COLD_STATUS_BIT_SHIFT           (3U)     /**< TS_COLD Status bit shift */
#define BQ25798_TS_COOL_STATUS_BIT_MASK            (0x04U)  /**< TS_COOL Status bit mask */
#define BQ25798_TS_COOL_STATUS_BIT_SHIFT           (2U)     /**< TS_COOL Status bit shift */
#define BQ25798_TS_WARM_STATUS_BIT_MASK            (0x02U)  /**< TS_WARM Status bit mask */
#define BQ25798_TS_WARM_STATUS_BIT_SHIFT           (1U)     /**< TS_WARM Status bit shift */
#define BQ25798_TS_HOT_STATUS_BIT_MASK             (0x01U)  /**< TS_HOT Status bit mask */
#define BQ25798_TS_HOT_STATUS_BIT_SHIFT            (0U)     /**< TS_HOT Status bit shift */

#define BQ25798_IBAT_REG_STATUS_BIT_MASK           (0x80U)  /**< IBAT_REG Status bit mask */
#define BQ25798_IBAT_REG_STATUS_BIT_SHIFT          (7U)     /**< IBAT_REG Status bit shift */
#define BQ25798_VBUS_OVP_STATUS_BIT_MASK           (0x40U)  /**< VBUS_OVP Status bit mask */
#define BQ25798_VBUS_OVP_STATUS_BIT_SHIFT          (6U)     /**< VBUS_OVP Status bit shift */
#define BQ25798_VBAT_OVP_STATUS_BIT_MASK           (0x20U)  /**< VBAT_OVP Status bit mask */
#define BQ25798_VBAT_OVP_STATUS_BIT_SHIFT          (5U)     /**< VBAT_OVP Status bit shift */
#define BQ25798_IBUS_OCP_STATUS_BIT_MASK           (0x10U)  /**< IBUS_OCP Status bit mask */
#define BQ25798_IBUS_OCP_STATUS_BIT_SHIFT          (4U)     /**< IBUS_OCP Status bit shift */
#define BQ25798_IBAT_OCP_STATUS_BIT_MASK           (0x08U)  /**< IBAT_OCP Status bit mask */
#define BQ25798_IBAT_OCP_STATUS_BIT_SHIFT          (3U)     /**< IBAT_OCP Status bit shift */
#define BQ25798_CONV_OCP_STATUS_BIT_MASK           (0x04U)  /**< CONV_OCP Status bit mask */
#define BQ25798_CONV_OCP_STATUS_BIT_SHIFT          (2U)     /**< CONV_OCP Status bit shift */
#define BQ25798_VAC2_OVP_STATUS_BIT_MASK           (0x02U)  /**< VAC2_OVP Status bit mask */
#define BQ25798_VAC2_OVP_STATUS_BIT_SHIFT          (1U)     /**< VAC2_OVP Status bit shift */
#define BQ25798_VAC1_OVP_STATUS_BIT_MASK           (0x01U)  /**< VAC1_OVP Status bit mask */
#define BQ25798_VAC1_OVP_STATUS_BIT_SHIFT          (0U)     /**< VAC1_OVP Status bit shift */

#define BQ25798_VSYS_SHORT_STATUS_BIT_MASK         (0x80U)  /**< VSYS_SHORT Status bit mask */
#define BQ25798_VSYS_SHORT_STATUS_BIT_SHIFT        (7U)     /**< VSYS_SHORT Status bit shift */
#define BQ25798_VSYS_OVP_STATUS_BIT_MASK           (0x40U)  /**< VSYS_OVP Status bit mask */
#define BQ25798_VSYS_OVP_STATUS_BIT_SHIFT          (6U)     /**< VSYS_OVP Status bit shift */
#define BQ25798_OTG_OVP_STATUS_BIT_MASK            (0x20U)  /**< OTG_OVP Status bit mask */
#define BQ25798_OTG_OVP_STATUS_BIT_SHIFT           (5U)     /**< OTG_OVP Status bit shift */
#define BQ25798_OTG_UVP_STATUS_BIT_MASK            (0x10U)  /**< OTG_UVP Status bit mask */
#define BQ25798_OTG_UVP_STATUS_BIT_SHIFT           (4U)     /**< OTG_UVP Status bit shift */
#define BQ25798_TSHUTDOWN_STATUS_BIT_MASK          (0x04U)  /**< TSHUTDOWN Status bit mask */
#define BQ25798_TSHUTDOWN_STATUS_BIT_SHIFT         (2U)     /**< TSHUTDOWN Status bit shift */


/** @} */

/**
* @defgroup BQ25798_FLAGS Flag bits
* @{
*/

// --- REG22_Charger_Flag_0 ---
#define BQ25798_IINDPM_FLAG_BIT_MASK          (0x80U)  /**< IINDPM / IOTG flag bit mask */
#define BQ25798_IINDPM_FLAG_BIT_SHIFT         (7U)     /**< IINDPM / IOTG flag bit shift */

#define BQ25798_VINDPM_FLAG_BIT_MASK          (0x40U)  /**< VINDPM / VOTG flag bit mask */
#define BQ25798_VINDPM_FLAG_BIT_SHIFT         (6U)     /**< VINDPM / VOTG flag bit shift */

#define BQ25798_WD_FLAG_BIT_MASK              (0x20U)  /**< I2C watchdog timer flag bit mask */
#define BQ25798_WD_FLAG_BIT_SHIFT             (5U)     /**< I2C watchdog timer flag bit shift */

#define BQ25798_POORSRC_FLAG_BIT_MASK         (0x10U)  /**< Poor source detection flag bit mask */
#define BQ25798_POORSRC_FLAG_BIT_SHIFT        (4U)     /**< Poor source detection flag bit shift */

#define BQ25798_PG_FLAG_BIT_MASK              (0x08U)  /**< Power good flag bit mask */
#define BQ25798_PG_FLAG_BIT_SHIFT             (3U)     /**< Power good flag bit shift */

#define BQ25798_AC2_PRESENT_FLAG_BIT_MASK     (0x04U)  /**< VAC2 present flag bit mask */
#define BQ25798_AC2_PRESENT_FLAG_BIT_SHIFT    (2U)     /**< VAC2 present flag bit shift */

#define BQ25798_AC1_PRESENT_FLAG_BIT_MASK     (0x02U)  /**< VAC1 present flag bit mask */
#define BQ25798_AC1_PRESENT_FLAG_BIT_SHIFT    (1U)     /**< VAC1 present flag bit shift */

#define BQ25798_VBUS_PRESENT_FLAG_BIT_MASK    (0x01U)  /**< VBUS present flag bit mask */
#define BQ25798_VBUS_PRESENT_FLAG_BIT_SHIFT   (0U)     /**< VBUS present flag bit shift */

// --- REG23_Charger_Flag_1 ---
#define BQ25798_CHG_FLAG_BIT_MASK             (0x80U)  /**< Charge status flag bit mask */
#define BQ25798_CHG_FLAG_BIT_SHIFT            (7U)     /**< Charge status flag bit shift */

#define BQ25798_ICO_FLAG_BIT_MASK             (0x40U)  /**< ICO status flag bit mask */
#define BQ25798_ICO_FLAG_BIT_SHIFT            (6U)     /**< ICO status flag bit shift */

#define BQ25798_VBUS_FLAG_BIT_MASK            (0x10U)  /**< VBUS status flag bit mask */
#define BQ25798_VBUS_FLAG_BIT_SHIFT           (4U)     /**< VBUS status flag bit shift */

#define BQ25798_TREG_FLAG_BIT_MASK            (0x04U)  /**< IC thermal regulation flag bit mask */
#define BQ25798_TREG_FLAG_BIT_SHIFT           (2U)     /**< IC thermal regulation flag bit shift */

#define BQ25798_VBAT_PRESENT_FLAG_BIT_MASK    (0x02U)  /**< VBAT present flag bit mask */
#define BQ25798_VBAT_PRESENT_FLAG_BIT_SHIFT   (1U)     /**< VBAT present flag bit shift */

#define BQ25798_BC1P2_DONE_FLAG_BIT_MASK      (0x01U)  /**< BC1.2 status flag bit mask */
#define BQ25798_BC1P2_DONE_FLAG_BIT_SHIFT     (0U)     /**< BC1.2 status flag bit shift */

// --- REG24_Charger_Flag_2 ---
#define BQ25798_DPDM_DONE_FLAG_BIT_MASK       (0x40U)  /**< D+/D- detection done flag bit mask */
#define BQ25798_DPDM_DONE_FLAG_BIT_SHIFT      (6U)     /**< D+/D- detection done flag bit shift */

#define BQ25798_ADC_DONE_FLAG_BIT_MASK         (0x20U)  /**< ADC conversion flag bit mask */
#define BQ25798_ADC_DONE_FLAG_BIT_SHIFT        (5U)     /**< ADC conversion flag bit shift */

#define BQ25798_VSYS_FLAG_BIT_MASK            (0x10U)  /**< VSYSMIN regulation flag bit mask */
#define BQ25798_VSYS_FLAG_BIT_SHIFT           (4U)     /**< VSYSMIN regulation flag bit shift */

#define BQ25798_CHG_TMR_FLAG_BIT_MASK         (0x08U)  /**< Fast charge timer flag bit mask */
#define BQ25798_CHG_TMR_FLAG_BIT_SHIFT        (3U)     /**< Fast charge timer flag bit shift */

#define BQ25798_TRICHG_TMR_FLAG_BIT_MASK      (0x04U)  /**< Trickle charge timer flag bit mask */
#define BQ25798_TRICHG_TMR_FLAG_BIT_SHIFT     (2U)     /**< Trickle charge timer flag bit shift */

#define BQ25798_PRECHG_TMR_FLAG_BIT_MASK      (0x02U)  /**< Pre-charge timer flag bit mask */
#define BQ25798_PRECHG_TMR_FLAG_BIT_SHIFT     (1U)     /**< Pre-charge timer flag bit shift */

#define BQ25798_TOPOFF_TMR_FLAG_BIT_MASK      (0x01U)  /**< Top-off timer flag bit mask */
#define BQ25798_TOPOFF_TMR_FLAG_BIT_SHIFT     (0U)     /**< Top-off timer flag bit shift */

// --- REG25_Charger_Flag_3 ---
#define BQ25798_VBATOTG_LOW_FLAG_BIT_MASK     (0x10U)  /**< VBAT too low for OTG flag bit mask */
#define BQ25798_VBATOTG_LOW_FLAG_BIT_SHIFT    (4U)     /**< VBAT too low for OTG flag bit shift */

#define BQ25798_TS_COLD_FLAG_BIT_MASK         (0x08U)  /**< TS cold temperature flag bit mask */
#define BQ25798_TS_COLD_FLAG_BIT_SHIFT        (3U)     /**< TS cold temperature flag bit shift */

#define BQ25798_TS_COOL_FLAG_BIT_MASK         (0x04U)  /**< TS cool temperature flag bit mask */
#define BQ25798_TS_COOL_FLAG_BIT_SHIFT        (2U)     /**< TS cool temperature flag bit shift */

#define BQ25798_TS_WARM_FLAG_BIT_MASK         (0x02U)  /**< TS warm temperature flag bit mask */
#define BQ25798_TS_WARM_FLAG_BIT_SHIFT        (1U)     /**< TS warm temperature flag bit shift */

#define BQ25798_TS_HOT_FLAG_BIT_MASK          (0x01U)  /**< TS hot temperature flag bit mask */
#define BQ25798_TS_HOT_FLAG_BIT_SHIFT         (0U)     /**< TS hot temperature flag bit shift */

// --- REG26_FAULT_Flag_0 ---
#define BQ25798_IBAT_REG_FLAG_BIT_MASK        (0x80U)  /**< IBAT regulation flag bit mask */
#define BQ25798_IBAT_REG_FLAG_BIT_SHIFT       (7U)     /**< IBAT regulation flag bit shift */

#define BQ25798_VBUS_OVP_FLAG_BIT_MASK        (0x40U)  /**< VBUS over-voltage flag bit mask */
#define BQ25798_VBUS_OVP_FLAG_BIT_SHIFT       (6U)     /**< VBUS over-voltage flag bit shift */

#define BQ25798_VBAT_OVP_FLAG_BIT_MASK        (0x20U)  /**< VBAT over-voltage flag bit mask */
#define BQ25798_VBAT_OVP_FLAG_BIT_SHIFT       (5U)     /**< VBAT over-voltage flag bit shift */

#define BQ25798_IBUS_OCP_FLAG_BIT_MASK        (0x10U)  /**< IBUS over-current flag bit mask */
#define BQ25798_IBUS_OCP_FLAG_BIT_SHIFT       (4U)     /**< IBUS over-current flag bit shift */

#define BQ25798_IBAT_OCP_FLAG_BIT_MASK        (0x08U)  /**< IBAT over-current flag bit mask */
#define BQ25798_IBAT_OCP_FLAG_BIT_SHIFT       (3U)     /**< IBAT over-current flag bit shift */

#define BQ25798_CONV_OCP_FLAG_BIT_MASK        (0x04U)  /**< Converter over-current flag bit mask */
#define BQ25798_CONV_OCP_FLAG_BIT_SHIFT       (2U)     /**< Converter over-current flag bit shift */

#define BQ25798_VAC2_OVP_FLAG_BIT_MASK        (0x02U)  /**< VAC2 over-voltage flag bit mask */
#define BQ25798_VAC2_OVP_FLAG_BIT_SHIFT       (1U)     /**< VAC2 over-voltage flag bit shift */

#define BQ25798_VAC1_OVP_FLAG_BIT_MASK        (0x01U)  /**< VAC1 over-voltage flag bit mask */
#define BQ25798_VAC1_OVP_FLAG_BIT_SHIFT       (0U)     /**< VAC1 over-voltage flag bit shift */

// --- REG27_FAULT_Flag_1 ---
#define BQ25798_VSYS_SHORT_FLAG_BIT_MASK      (0x80U)  /**< VSYS short circuit flag bit mask */
#define BQ25798_VSYS_SHORT_FLAG_BIT_SHIFT     (7U)     /**< VSYS short circuit flag bit shift */

#define BQ25798_VSYS_OVP_FLAG_BIT_MASK        (0x40U)  /**< VSYS over-voltage flag bit mask */
#define BQ25798_VSYS_OVP_FLAG_BIT_SHIFT       (6U)     /**< VSYS over-voltage flag bit shift */

#define BQ25798_OTG_OVP_FLAG_BIT_MASK         (0x20U)  /**< OTG over-voltage flag bit mask */
#define BQ25798_OTG_OVP_FLAG_BIT_SHIFT        (5U)     /**< OTG over-voltage flag bit shift */

#define BQ25798_OTG_UVP_FLAG_BIT_MASK         (0x10U)  /**< OTG under-voltage flag bit mask */
#define BQ25798_OTG_UVP_FLAG_BIT_SHIFT        (4U)     /**< OTG under-voltage flag bit shift */

#define BQ25798_TSHUT_FLAG_BIT_MASK           (0x04U)  /**< IC thermal shutdown flag bit mask */
#define BQ25798_TSHUT_FLAG_BIT_SHIFT          (2U)     /**< IC thermal shutdown flag bit shift */

/** @} */


/* ==== Driver Types & API Prototypes ====*/

/**
 * @brief BQ2579 API status codes
 */
typedef enum {
    BQ25798_OK = 0,
    BQ25798_ERROR = -1,
    BQ25798_TIMEOUT = -2,
    BQ25798_INVALID_PARAM = -3
} BQ25798_Status_t;

/**
 * @brief ADC handle
 *
 */
typedef struct {

    uint8_t res;

    uint8_t ctrl;
    uint8_t dis0;
    uint8_t dis1;

    uint16_t ibus_data;
    uint16_t ibat_data;
    uint16_t vbus_data;
    uint16_t vbat_data;
    uint16_t vac1_data;
    uint16_t vac2_data;
    uint16_t vsys_data;
    uint16_t ts_data;
    uint16_t tdie_data;
    uint16_t dp_data;
    uint16_t dm_data;

} BQ25798_ADCHandle_t;


/**
 * @brief Abstract handle to BQ25798 device context
 *
 */
typedef struct {

    void *hi2c;                 /**< Pointer to HAL/I2C handle structure (e.g. I2C_HandleTypeDef*) */
    uint8_t i2c_addr;           /**< 7-bit I2C address (BQ25798_I2C_ADDR) */

    uint16_t vreg_mv;
    uint16_t chg_current_mA;
    uint8_t cell_count;
    float wd_timer;
    uint16_t vsysmin_mv;
    uint16_t pchg_current_mA;
    uint8_t en_chg;
    uint8_t en_acdrv1;
    uint8_t en_acdrv2;
    uint8_t ship_fet_pres;
    uint8_t sdrv_ctrl;
    uint8_t en_ibat;
    uint8_t en_bckup;
    uint16_t votg_mv;
    uint16_t iotg_ma;
    uint16_t vindpm_mv;
    uint16_t iindpm_ma;

    uint8_t charger_status_0;
    uint8_t charger_status_1;
    uint8_t charger_status_2;
    uint8_t charger_status_3;
    uint8_t charger_status_4;

    uint8_t fault_status_0;
    uint8_t fault_status_1;

    uint8_t charger_flag_0;
    uint8_t charger_flag_1;
    uint8_t charger_flag_2;
    uint8_t charger_flag_3;

    uint8_t fault_flag_0;
    uint8_t fault_flag_1;

    BQ25798_ADCHandle_t adc_handle;


} BQ25798_Handle_t;


/* ==== Functions prototypes ====*/

/**
 * @defgroup BQ25798_FUNC_I2C Basic I2C read/write
 * @{
 */

/**
 * @brief           configure BQ25798 handle instance
 *
 * @param h         pointer to the handle instance to be configured
 * @param hi2c      I2C handle to use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_Init(BQ25798_Handle_t *h, void *hi2c);

/**
 * @brief           read the less significant byte of a register of the charger
 *
 * @param h         pointer to the handle instance in use
 * @param reg       address of the register to read
 * @param data      buffer to store the read data
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadReg8(const BQ25798_Handle_t *h, uint8_t reg, uint8_t *data);

/**
 * @brief           write to the less significant byte of a register of the charger
 *
 * @param h         pointer to the handle instance in use
 * @param reg       address of the register to write to
 * @param data      buffer to store the data to be written
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_WriteReg8(const BQ25798_Handle_t *h, uint8_t reg, uint8_t data);

/**
 * @brief           read the two bytes of a register of the charger
 *
 * @param h         pointer to the handle instance in use
 * @param reg       address of the register to read
 * @param data      buffer to store the read data
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadReg16(const BQ25798_Handle_t *h, uint8_t reg, uint16_t *data);

/**
 * @brief           write to the two bytes of a register of the charger
 *
 * @param h         pointer to the handle instance in use
 * @param reg       address of the register to write to
 * @param data      buffer to store the data to be written
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_WriteReg16(const BQ25798_Handle_t *h, uint8_t reg, uint16_t data);

/** @} */

/**
 * @defgroup BQ25798_FUNC_CHG_HELP Charger configuration helpers
 * @{
 */

 /**
  * @brief              set charge voltage (VREG)
  *
  * @param h            pointer to the handle instance in use
  * @param voltage_mv   desired charge voltage in mV
  * @return BQ25798_Status_t
  */
BQ25798_Status_t BQ25798_SetChargeVoltage(BQ25798_Handle_t *h, uint16_t voltage_mv);

/**
 * @brief               set VSYSMIN
 *
 * @param h             pointer to the handle instance in use
 * @param voltage_mv    desired VSYSMIN in mV
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetVSYSMIN(BQ25798_Handle_t *h, uint16_t voltage_mv);

/**
 * @brief               set cell count
 *
 * @param h             pointer to the handle instance in use
 * @param cell_count    desired cell count
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetBatteryCellCount(BQ25798_Handle_t *h, uint8_t cell_count);

/**
 * @brief               set fast charge current
 *
 * @param h             pointer to the handle instance in use
 * @param current_ma    desired current in mA
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetChargeCurrent(BQ25798_Handle_t *h, uint16_t current_ma);

/**
 * @brief               set precharge current
 *
 * @param h             pointer to the handle instance in use
 * @param current_ma    desired current in mA
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetPrechargeCurrent(BQ25798_Handle_t *h, uint16_t current_ma);

/**
 * @brief                           set watchdog timer duration
 *
 * @param h                         pointer to the handle instance in use
 * @param watchdog_timer_settings   binary setting of the timer duration (cf. BQ25798 datasheet for correspondance between binary value and durations in seconds)
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetWatchdogTimer(BQ25798_Handle_t *h, uint8_t watchdog_timer_settings);

/**
 * @brief                           enable charge
 *
 * @param h                         pointer to the handle instance in use
 * @param enable                    if true, enable charge, else disable charge
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_EnableCharge(BQ25798_Handle_t *h, uint8_t enable);

/** @} */

/**
 * @defgroup BQ25798_FUNC_ADD_HELP Additional configuration helpers
 * @{
 */

/**
 * @brief                           set low battery voltage threshold
 *
 * @param h                         pointer to the handle instance in use
 * @param level                     low battery voltage threshold level (cf. BQ25798 datasheet for correspondance between level value and voltage in mV)
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetVBAT_LOWV(BQ25798_Handle_t *h, uint8_t level);

/**
 * @brief                           set input voltage limit
 *
 * @param h                         pointer to the handle instance in use
 * @param voltage_mv                desired input voltage limit in mV
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetInputVoltageLimit(BQ25798_Handle_t *h, uint16_t voltage_mv);

/**
 * @brief                           set input current limit
 *
 * @param h                         pointer to the handle instance in use
 * @param current_ma                desired input current limit in mA
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetInputCurrentLimit(BQ25798_Handle_t *h, uint16_t current_ma);

/**
 * @brief                           enable MPPT
 *
 * @param h                         pointer to the handle instance in use
 * @param enable                    if true, enable MPPT, else disable MPPT
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetMPPT(BQ25798_Handle_t *h, uint8_t enable);

/**
 * @brief                           set termination current
 *
 * @param h                         pointer to the handle instance in use
 * @param current_ma                desired termination current in mA
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetTerminationCurrent(BQ25798_Handle_t *h, uint16_t current_ma);

/**
 * @brief                           set recharge deglitch time
 *
 * @param h                         pointer to the handle instance in use
 * @param deglitch                  desired deglitch time (cf. BQ25798 datasheet for correspondance between deglitch value and time in milliseconds)
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetRechargeDeglitch(BQ25798_Handle_t *h, uint8_t deglitch);

/**
 * @brief                           set recharge voltage offset
 *
 * @param h                         pointer to the handle instance in use
 * @param voltage_mv                desired recharge voltage offset in mV
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetRechargeVoltageOffset(BQ25798_Handle_t *h, uint16_t voltage_mv);

/**
 * @brief                           set VAC_OVP threshold
 *
 * @param h                         pointer to the handle instance in use
 * @param level                     VAC_OVP threshold level (cf. BQ25798 datasheet for correspondance between level value and voltage in mV)
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetVAC_OVP(BQ25798_Handle_t *h, uint8_t level);

/**
 * @brief                           set TREG thermal regulation threshold
 *
 * @param h                         pointer to the handle instance in use
 * @param level                     TREG threshold level (cf. BQ25798 datasheet for correspondance between level value and temperature in degree Celsius)
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetTREG(BQ25798_Handle_t *h, uint8_t level);

/**
 * @brief                           set TSHUT thermal shutdown threshold
 *
 * @param h                         pointer to the handle instance in use
 * @param level                     TSHUT threshold level (cf. BQ25798 datasheet for correspondance between level value and temperature in degree Celsius)
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetTSHUT(BQ25798_Handle_t *h, uint8_t level);

/**
 * @brief                           set NTC control
 *
 * @param h                         pointer to the handle instance in use
 * @param ntc0                      NTC0 threshold level (cf. BQ25798 datasheet for correspondance between level value and temperature in degree Celsius)
 * @param ntc1                      NTC1 threshold level (cf. BQ25798 datasheet for correspondance between level value and temperature in degree Celsius)
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetNTCControl(BQ25798_Handle_t *h, uint8_t ntc0, uint8_t ntc1);

/**
 * @brief                           deactivate TS pin
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_IgnoreTS(BQ25798_Handle_t *h);

/** @} */

/**
 * @defgroup OTG and backup mode configuration
 * @{
 */

/**
 * @brief                           set OTG voltage
 *
 * @param h                         pointer to the handle instance in use
 * @param voltage_mv                desired OTG voltage in mV
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetOTGVoltage(BQ25798_Handle_t *h, uint16_t voltage_mv);

/**
 * @brief                           set OTG current
 *
 * @param h                         pointer to the handle instance in use
 * @param current_ma                desired OTG current in mA
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetOTGCurrent(BQ25798_Handle_t *h, uint16_t current_ma);

/**
 * @brief                           set OTG timer
 *
 * @param h                         pointer to the handle instance in use
 * @param enable                    if true, enable OTG timer, else disable OTG timer
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_SetOTGTimer(BQ25798_Handle_t *h, uint8_t enable);

/**
 * @brief                           enable OTG mode
 *
 * @param h                         pointer to the handle instance in use
 * @param enable                    if true, enable OTG mode, else disable OTG mode
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_EnableOTG(BQ25798_Handle_t *h, uint8_t enable);

/**
 * @brief                           enable backup mode
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_EnableBackupMode(BQ25798_Handle_t *h);

/**
 * @brief                           exit backup mode
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ExitBackupMode(BQ25798_Handle_t *h);

/** @} */

/**
 * @defgroup BQ25798_FUNC_SHUT_HELP Ship and shutdown mode helpers
 * @{
 */

 /**
 * @brief                           enable ship FET
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_EnableShipFET(BQ25798_Handle_t *h);

/**
 * @brief                           enable idle mode
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_EnableIdleMode(BQ25798_Handle_t *h);

/**
 * @brief                           enable ship mode
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_EnableShipMode(BQ25798_Handle_t *h);

/**
 * @brief                           enable shutdown mode
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_EnableShutdownMode(BQ25798_Handle_t *h);

/** @} */

/**
 * @defgroup BQ25798_FUNC_READ_HELP Read helpers
 * @{
 */

/**
 * @brief                           read VSYSMIN and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadVSYSMIN(BQ25798_Handle_t *h);

/**
 * @brief                           read VREG and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadVREG(BQ25798_Handle_t *h);

/**
 * @brief                           read cell count and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadCellCount(BQ25798_Handle_t *h);

/**
 * @brief                           read fast charge current and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadChgCurrent(BQ25798_Handle_t *h);

/**
 * @brief                           read precharge and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadPchgCurrent(BQ25798_Handle_t *h);

/**
 * @brief                           read watdchdog timer settings and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadWatchdogSettings(BQ25798_Handle_t *h);

/**
 * @brief                           read charge enable bit and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadENCharge(BQ25798_Handle_t *h);

/**
 * @brief                           read ACDRV1 and 2 presence bits and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadENACDRV(BQ25798_Handle_t *h);

/**
 * @brief                           read ship FET presence and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadShipFETPres(BQ25798_Handle_t *h);

/**
 * @brief                           read SDRV control mode and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadSDRVCtrl(BQ25798_Handle_t *h);

/**
 * @brief                           read IBAT pin enable bit and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadENIBAT(BQ25798_Handle_t *h);

/**
 * @brief                           enable IBAT pin enable bit
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ENIBAT(BQ25798_Handle_t *h);

/**
 * @brief                           read VOTG and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadVOTG(BQ25798_Handle_t *h);

/**
 * @brief                           read IOTG and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadIOTG(BQ25798_Handle_t *h);

/**
 * @brief                           read VINDPM and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadVINDPM(BQ25798_Handle_t *h);

/**
 * @brief                           read IINDPM and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadIINDPM(BQ25798_Handle_t *h);

/**
 * @brief                           read backup enable bit and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadENBckup(BQ25798_Handle_t *h);

/**
 * @brief                           read the whole charge configuration and store it in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ReadChargeConf(BQ25798_Handle_t *h);

/** @} */

/**
 * @defgroup BQ25798_FUNC_ADC_HELP ADC helpers
 * @{
 */

/**
 * @brief                           configure BQ25798 ADC but do not start conversion yet
 *
 * @param h                         pointer to the handle instance in use
 * @param res                       desired ADC resolution binary setting (cf. BQ25798 datasheet for correspondance between binary value and resolution in bits)
 * @param ctrl                      desired ADC control setting (cf. BQ25798 datasheet for correspondance between binary value and control mode)
 * @param dis0                      desired ADC disable setting 0 (cf. BQ25798 datasheet for correspondance between binary value and enabled/disabled channels)
 * @param dis1                      desired ADC disable setting 1 (cf. BQ25798 datasheet for correspondance between binary value and enabled/disabled channels)
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ConfigADC(BQ25798_Handle_t *h, uint8_t res, uint8_t ctrl, uint8_t dis0, uint8_t dis1);

/**
 * @brief                           enable ADC and start conversion with the configuration previously set by BQ25798_ConfigADC()
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_EnableADC(BQ25798_Handle_t *h);

/**
 * @brief                           disable ADC and stop conversion
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_DisableADC(BQ25798_Handle_t *h);

/**
 * @brief                           read ADC conversion results and store the values in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ADCReadAll(BQ25798_Handle_t *h);

/**
 * @brief                           read IBUS conversion result and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ADCReadIBUS(BQ25798_Handle_t *h);

/**
 * @brief                           read IBAT conversion result and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ADCReadIBAT(BQ25798_Handle_t *h);

/**
 * @brief                           read VBUS conversion result and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ADCReadVBUS(BQ25798_Handle_t *h);

/**
 * @brief                           read VAC1 conversion result and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ADCReadVAC1(BQ25798_Handle_t *h);

/**
 * @brief                           read VAC2 conversion result and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ADCReadVAC2(BQ25798_Handle_t *h);

/**
 * @brief                           read VBAT conversion result and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ADCReadVBAT(BQ25798_Handle_t *h);

/**
 * @brief                           read VSYS conversion result and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ADCReadVSYS(BQ25798_Handle_t *h);

/**
 * @brief                           read TS conversion result and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ADCReadTS(BQ25798_Handle_t *h);

/**
 * @brief                           read TDIE conversion result and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ADCReadTDIE(BQ25798_Handle_t *h);

/**
 * @brief                           read DP conversion result and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ADCReadDP(BQ25798_Handle_t *h);

/**
 * @brief                           read DM conversion result and store the value in the handle
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ADCReadDM(BQ25798_Handle_t *h);

/** @} */

/**
 * @defgroup BQ25798_FUNC_STAT_HELP Status read helpers
 * @{
 */

 /**
  * @brief                          read all status registers and store the values in the handle
  *
  * @param h                        pointer to the handle in use
  * @return BQ25798_Status_t
  */
BQ25798_Status_t BQ25798_ReadStatus(BQ25798_Handle_t *h);

 /**
  * @brief                          read all flag registers and store the values in the handle
  *
  * @param h                        pointer to the handle in use
  * @return BQ25798_Status_t
  */
BQ25798_Status_t BQ25798_ReadFlags(BQ25798_Handle_t *h);

/** @} */

/**
 * @defgroup BQ25798_FUNC_RST_HELP Reset helpers
 * @{
 */


/**
 * @brief                           power reset the charger
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_Reset(BQ25798_Handle_t *h);

/**
 * @brief                           reset watchdog timer (when watchdog is enabled)
 *
 * @param h                         pointer to the handle instance in use
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_ResetWatchdogTimer(BQ25798_Handle_t *h);

/** @} */

/**
 * @defgroup BQ25798_FUNC_CONV_HELP Conversion helpers
 * @{
 */

/**
 * @brief                           Convert battery regulation voltage in mV to VREG register value
 * @param mv                        target voltage in mV
 * @param reg                       11-bit output value pointer
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_VoltageToVREG(uint16_t mv, uint16_t *reg);

/**
 * @brief                           Convert charge current in mA to ICHG register value
 * @param ma                        target current in mA
 * @param reg                       9-bit output value pointer
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_CurrentToICHG(uint16_t ma, uint16_t *reg);

/**
 * @brief                           Convert input voltage limit in mV to VINDPM register value
 * @param mv                        target voltage in mV
 * @param reg                       8-bit output value pointer
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_VoltageToVINDPM(uint16_t mv, uint8_t *reg);

/**
 * @brief                           Convert input current limit in mA to IINDPM register value
 * @param ma                        target current in mA
 * @param reg                       9-bit output value pointer
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_CurrentToIINDPM(uint16_t ma, uint16_t *reg);

/**
 * @brief                           Convert VSYSMIN voltage in mV to register value
 * @param mv                        target voltage in mV
 * @param reg                       6-bit output value pointer
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_VoltageToVSYSMIN(uint16_t mv, uint8_t *reg);

/**
 * @brief                           Convert precharge current in mA to IPRECHG register value
 * @param ma                        target current in mA
 * @param reg                       6-bit output value pointer
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_CurrentToIPRECHG(uint16_t ma, uint8_t *reg);

/**
 * @brief                           Convert termination current in mA to ITERM register value
 * @param ma                        target current in mA
 * @param reg                       5-bit output value pointer
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_CurrentToITERM(uint16_t ma, uint8_t *reg);

/**
 * @brief                           Convert recharge voltage in mV to VRECHG register value
 * @param mv                        target voltage in mV
 * @param reg                       4-bit output value pointer
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_VoltageToVRECHG(uint16_t mv, uint8_t *reg);

/**
 * @brief                           Convert OTG voltage in mV to VOTG register value
 * @param mv                        target voltage in mV
 * @param reg                       11-bit output value pointer
 * @return BQ25798_Status_t
 */
BQ25798_Status_t BQ25798_VoltageToVOTG(uint16_t mv, uint16_t *reg);

/**
 * @brief                           Convert OTG current in mA to IOTG register value
 * @param ma                        target current in mA
 * @param reg                       7-bit output value pointer
 * @return                          BQ25798_OK or BQ25798_INVALID_PARAM
 */
BQ25798_Status_t BQ25798_CurrentToIOTG(uint16_t ma, uint8_t *reg);

/** @} */

/* ==== Macros ==== */

/**
 * @defgroup  BQ25798_MACRO_BIT Bit Manipulation Macros
 * @{
 */

/** Extract bits from register value */
#define BQ25798_EXTRACT_BITS(val, mask, shift)  (((val) & (mask)) >> (shift))

/** Insert bits into register value */
#define BQ25798_INSERT_BITS(val, data, mask, shift)  \
    (((val) & ~(mask)) | (((data) << (shift)) & (mask)))

/** @} */


/**
 * @defgroup BQ25798_MACRO_CONV Conversion helpers
 * @{
 */

/*  */
#define BQ25798_CLAMP(val, min, max)      (((val) < (min)) ? (min) : (((val) > (max)) ? (max) : (val)))

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BQ25798_H */
