 /**
 * @file        bq25798.c
 * @author      Florian Topeza
 * @brief       Minimal STM32H7 HAL-based implementation for BQ25798 I2C register access and basic charger configuration.
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


/* ==== Includes ==== */

#include "bq25798.h"


/* === Functions ==== */

static inline BQ25798_Status_t BQ25798_ValidateHandle(const BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_OK;
    if ((h == NULL) || (h->hi2c == NULL)) {
        status = BQ25798_INVALID_PARAM;
    }
    return status;
}

static inline BQ25798_Status_t BQ25798_ValidateVREG(uint16_t vreg_mv, uint8_t cell_count)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (cell_count != 0U) {
        /* If no cell count set, allow any valid VREG */

        switch (cell_count) {
            case 1U:
                if ((vreg_mv < 3000U) || (vreg_mv > 4500U)) {
                    status = BQ25798_INVALID_PARAM;
                }
                break;
            case 2U:
                if ((vreg_mv < 6000U) || (vreg_mv > 9000U)) {
                    status = BQ25798_INVALID_PARAM;
                }
                break;
            case 3U:
                if ((vreg_mv < 9000U) || (vreg_mv > 13500U)) {
                    status = BQ25798_INVALID_PARAM;
                }
                break;
            case 4U:
                if ((vreg_mv < 12000U) || (vreg_mv > 18000U)) {
                    status = BQ25798_INVALID_PARAM;
                }
                break;
            default:
                status = BQ25798_INVALID_PARAM;
                break;
        }

    }

    return status;
}

static inline BQ25798_Status_t BQ25798_ValidateVSYSMIN(uint16_t vsysmin_mv, uint8_t cell_count, uint16_t vreg_mv)
{
    BQ25798_Status_t status = BQ25798_OK;

    /* VSYSMIN should be less than VREG if both are set */
    if ((vreg_mv > 0U) && (vsysmin_mv >= vreg_mv)) {
        status = BQ25798_INVALID_PARAM;
    }

    if (cell_count == 0U) {
        return status;
    }

    /* VSYSMIN should be reasonable for cell count */
    switch (cell_count) {
        case 1U:
            if (vsysmin_mv > 3500U) {
                status = BQ25798_INVALID_PARAM;
            }
            break;
        case 2U:
            if (vsysmin_mv > 7000U) {
                status = BQ25798_INVALID_PARAM;
            }
            break;
        case 3U:
            if (vsysmin_mv > 10500U) {
                status = BQ25798_INVALID_PARAM;
            }
            break;
        case 4U:
            if (vsysmin_mv > 14000U) {
                status = BQ25798_INVALID_PARAM;
            }
            break;
        default:
            status = BQ25798_INVALID_PARAM;
            break;
    }

    return status;
}

BQ25798_Status_t BQ25798_Init(BQ25798_Handle_t *h, void *hi2c)
{
    BQ25798_Status_t status = BQ25798_OK;

    if ((h == NULL) || (hi2c == NULL)) {
        status = BQ25798_INVALID_PARAM;
    } else {
        h->hi2c = hi2c;
        h->i2c_addr = BQ25798_I2C_ADDR;
    }

    return status;
}

BQ25798_Status_t BQ25798_ReadReg8(const BQ25798_Handle_t *h, uint8_t reg, uint8_t *data)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (BQ25798_ValidateHandle(h) != BQ25798_OK || data == NULL) {
        status = BQ25798_INVALID_PARAM;
    } else if (HAL_I2C_Mem_Read((I2C_HandleTypeDef*)h->hi2c,
                                (uint16_t)(h->i2c_addr << 1),
                                reg,
                                I2C_MEMADD_SIZE_8BIT,
                                data,
                                1,
                                BQ25798_I2C_TIMEOUT_MS) != HAL_OK) {
        status = BQ25798_ERROR;
    }

    return status;
}

BQ25798_Status_t BQ25798_WriteReg8(const BQ25798_Handle_t *h, uint8_t reg, uint8_t data)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (BQ25798_ValidateHandle(h) != BQ25798_OK) {
        status = BQ25798_INVALID_PARAM;
    } else if (HAL_I2C_Mem_Write((I2C_HandleTypeDef*)h->hi2c,
                                 (uint16_t)(h->i2c_addr << 1),
                                 reg,
                                 I2C_MEMADD_SIZE_8BIT,
                                 &data,
                                 1,
                                 BQ25798_I2C_TIMEOUT_MS) != HAL_OK) {
        status = BQ25798_ERROR;
    }

    return status;
}

BQ25798_Status_t BQ25798_ReadReg16(const BQ25798_Handle_t *h, uint8_t reg, uint16_t *data)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (BQ25798_ValidateHandle(h) != BQ25798_OK || data == NULL) {
        status = BQ25798_INVALID_PARAM;
    } else {
        uint8_t raw[2];
        if (HAL_I2C_Mem_Read((I2C_HandleTypeDef*)h->hi2c,
                             (uint16_t)(h->i2c_addr << 1),
                             reg,
                             I2C_MEMADD_SIZE_8BIT,
                             raw,
                             2,
                             BQ25798_I2C_TIMEOUT_MS) != HAL_OK) {
            status = BQ25798_ERROR;
        } else {
            // Data is received MSB first
            *data = (uint16_t)raw[1] | ((uint16_t)raw[0] << 8);
        }
    }

    return status;
}

BQ25798_Status_t BQ25798_WriteReg16(const BQ25798_Handle_t *h, uint8_t reg, uint16_t data)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (BQ25798_ValidateHandle(h) != BQ25798_OK) {
        status = BQ25798_INVALID_PARAM;
    } else {
        // Data is sent MSB first
        const uint8_t raw[2] = {(uint8_t)((data >> 8) & 0xFFU), (uint8_t)(data & 0xFFU)};
        if (HAL_I2C_Mem_Write((I2C_HandleTypeDef*)h->hi2c,
                              (uint16_t)(h->i2c_addr << 1),
                              reg,
                              I2C_MEMADD_SIZE_8BIT,
                              (uint8_t*)raw,
                              2,
                              BQ25798_I2C_TIMEOUT_MS) != HAL_OK) {
            status = BQ25798_ERROR;
        }
    }

    return status;
}

BQ25798_Status_t BQ25798_VoltageToVREG(uint16_t mv, uint16_t *reg)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (reg == NULL) {
        status = BQ25798_INVALID_PARAM;
    } else if ((mv < BQ25798_VREG_MIN_MV) || (mv > BQ25798_VREG_MAX_MV)) {
        status = BQ25798_INVALID_PARAM;
    } else {
        uint32_t raw = (uint32_t)(mv - BQ25798_VREG_OFFSET_MV) / BQ25798_VREG_STEP_MV;
        raw = BQ25798_CLAMP(raw, 0U, BQ25798_VREG_MASK);
        *reg = (uint16_t)raw;
    }

    return status;
}

BQ25798_Status_t BQ25798_CurrentToICHG(uint16_t ma, uint16_t *reg)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (reg == NULL) {
        status = BQ25798_INVALID_PARAM;
    } else if ((ma < BQ25798_ICHG_MIN_MA) || (ma > BQ25798_ICHG_MAX_MA)) {
        status = BQ25798_INVALID_PARAM;
    } else {
        uint32_t raw = (uint32_t)(ma - BQ25798_ICHG_OFFSET_MA) / BQ25798_ICHG_STEP_MA;
        raw = BQ25798_CLAMP(raw, 0U, BQ25798_ICHG_MASK);
        *reg = (uint16_t)raw;
    }

    return status;
}

BQ25798_Status_t BQ25798_VoltageToVINDPM(uint16_t mv, uint8_t *reg)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (reg == NULL) {
        status = BQ25798_INVALID_PARAM;
    } else if ((mv < BQ25798_VINDPM_MIN_MV) || (mv > BQ25798_VINDPM_MAX_MV)) {
        status = BQ25798_INVALID_PARAM;
    } else {
        uint32_t raw = (uint32_t)(mv - BQ25798_VINDPM_OFFSET_MV) / BQ25798_VINDPM_STEP_MV;
        raw = BQ25798_CLAMP(raw, 0U, BQ25798_VINDPM_MASK);
        *reg = (uint8_t)raw;
    }

    return status;
}

BQ25798_Status_t BQ25798_CurrentToIINDPM(uint16_t ma, uint16_t *reg)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (reg == NULL) {
        status = BQ25798_INVALID_PARAM;
    } else if ((ma < BQ25798_IINDPM_MIN_MA) || (ma > BQ25798_IINDPM_MAX_MA)) {
        status = BQ25798_INVALID_PARAM;
    } else {
        uint32_t raw = (uint32_t)(ma - BQ25798_IINDPM_OFFSET_MA) / BQ25798_IINDPM_STEP_MA;
        raw = BQ25798_CLAMP(raw, 0U, BQ25798_IINDPM_MASK);
        *reg = (uint16_t)raw;
    }

    return status;
}

BQ25798_Status_t BQ25798_VoltageToVSYSMIN(uint16_t mv, uint8_t *reg)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (reg == NULL) {
        status = BQ25798_INVALID_PARAM;
    } else if ((mv < BQ25798_VSYSMIN_MIN_MV) || (mv > BQ25798_VSYSMIN_MAX_MV)) {
        status = BQ25798_INVALID_PARAM;
    } else {
        uint32_t raw = (uint32_t)(mv - BQ25798_VSYSMIN_OFFSET_MV) / BQ25798_VSYSMIN_STEP_MV;
        raw = BQ25798_CLAMP(raw, 0U, BQ25798_VSYSMIN_MASK);
        *reg = (uint8_t)raw;
    }

    return status;
}

BQ25798_Status_t BQ25798_CurrentToIPRECHG(uint16_t ma, uint8_t *reg)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (reg == NULL) {
        status = BQ25798_INVALID_PARAM;
    } else if ((ma < BQ25798_IPRECHG_MIN_MA) || (ma > BQ25798_IPRECHG_MAX_MA)) {
        status = BQ25798_INVALID_PARAM;
    } else {
        uint32_t raw = (uint32_t)(ma - BQ25798_IPRECHG_OFFSET_MA) / BQ25798_IPRECHG_STEP_MA;
        raw = BQ25798_CLAMP(raw, 0U, BQ25798_IPRECHG_MASK);
        *reg = (uint8_t)raw;
    }

    return status;
}

BQ25798_Status_t BQ25798_CurrentToITERM(uint16_t ma, uint8_t *reg)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (reg == NULL) {
        status = BQ25798_INVALID_PARAM;
    } else if ((ma < BQ25798_ITERM_MIN_MA) || (ma > BQ25798_ITERM_MAX_MA)) {
        status = BQ25798_INVALID_PARAM;
    } else {
        uint32_t raw = (uint32_t)(ma - BQ25798_ITERM_OFFSET_MA) / BQ25798_ITERM_STEP_MA;
        raw = BQ25798_CLAMP(raw, 0U, BQ25798_ITERM_MASK);
        *reg = (uint8_t)raw;
    }

    return status;
}

BQ25798_Status_t BQ25798_VoltageToVRECHG(uint16_t mv, uint8_t *reg)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (reg == NULL) {
        status = BQ25798_INVALID_PARAM;
    } else if ((mv < BQ25798_VRECHG_MIN_MV) || (mv > BQ25798_VRECHG_MAX_MV)) {
        status = BQ25798_INVALID_PARAM;
    } else {
        uint32_t raw = (uint32_t)(mv - BQ25798_VRECHG_OFFSET_MV) / BQ25798_VRECHG_STEP_MV;
        raw = BQ25798_CLAMP(raw, 0U, BQ25798_VRECHG_MASK);
        *reg = (uint8_t)raw;
    }

    return status;
}

BQ25798_Status_t BQ25798_VoltageToVOTG(uint16_t mv, uint16_t *reg)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (reg == NULL) {
        status = BQ25798_INVALID_PARAM;
    } else if ((mv < BQ25798_VOTG_MIN_MV) || (mv > BQ25798_VOTG_MAX_MV)) {
        status = BQ25798_INVALID_PARAM;
    } else {
        uint32_t raw = (uint16_t)(mv - BQ25798_VOTG_OFFSET_MV) / BQ25798_VOTG_STEP_MV;
        raw = BQ25798_CLAMP(raw, 0U, BQ25798_VOTG_MASK);
        *reg = (uint16_t)raw;
    }

    return status;
}

BQ25798_Status_t BQ25798_CurrentToIOTG(uint16_t ma, uint8_t *reg)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (reg == NULL) {
        status = BQ25798_INVALID_PARAM;
    } else if ((ma < BQ25798_IOTG_MIN_MA) || (ma > BQ25798_IOTG_MAX_MA)) {
        status = BQ25798_INVALID_PARAM;
    } else {
        uint32_t raw = (uint32_t)(ma - BQ25798_IOTG_OFFSET_MA) / BQ25798_IOTG_STEP_MA;
        raw = BQ25798_CLAMP(raw, 0U, BQ25798_IOTG_MASK);
        *reg = (uint8_t)raw;
    }

    return status;
}

static BQ25798_Status_t BQ25798_UpdateRegBits(const BQ25798_Handle_t *h,
                                               uint8_t reg,
                                               uint8_t mask,
                                               uint8_t shift,
                                               uint8_t value)
{
    uint8_t current_value;
    BQ25798_Status_t status = BQ25798_OK;

    status = BQ25798_ReadReg8(h, reg, &current_value);
    if (status == BQ25798_OK) {
        uint8_t new_value = BQ25798_INSERT_BITS(current_value, value, mask, shift);
        status = BQ25798_WriteReg8(h, reg, new_value);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetVSYSMIN(BQ25798_Handle_t *h, uint16_t voltage_mv)
{
    BQ25798_Status_t status = BQ25798_ValidateVSYSMIN(voltage_mv, h->cell_count, h->vreg_mv);

    if (status == BQ25798_OK) {
        uint8_t reg;
        status = BQ25798_VoltageToVSYSMIN(voltage_mv, &reg);
        if (status == BQ25798_OK) {
            status = BQ25798_WriteReg8(h, BQ25798_REG_VSYSMIN, reg);
            if (status == BQ25798_OK) {
                h->vsysmin_mv = voltage_mv;
            }
        }
    }

    return status;
}

BQ25798_Status_t BQ25798_SetVBAT_LOWV(BQ25798_Handle_t *h, uint8_t level)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (level > 3U) {
        status = BQ25798_INVALID_PARAM;
    } else {
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_PRECHARGE_CTRL,
                                       BQ25798_VBAT_LOWV_MASK,
                                       BQ25798_VBAT_LOWV_SHIFT,
                                       level);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetPrechargeCurrent(BQ25798_Handle_t *h, uint16_t current_ma)
{
    uint8_t reg;
    BQ25798_Status_t status = BQ25798_CurrentToIPRECHG(current_ma, &reg);

    if (status == BQ25798_OK) {
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_PRECHARGE_CTRL,
                                       BQ25798_IPRECHG_MASK,
                                       0U,
                                       reg);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetTerminationCurrent(BQ25798_Handle_t *h, uint16_t current_ma)
{
    uint8_t reg;
    BQ25798_Status_t status = BQ25798_CurrentToITERM(current_ma, &reg);

    if (status == BQ25798_OK) {
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_TERM_CTRL,
                                       BQ25798_ITERM_MASK,
                                       0U,
                                       reg);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetBatteryCellCount(BQ25798_Handle_t *h, uint8_t cell_count)
{
    BQ25798_Status_t status = BQ25798_OK;

    if ((cell_count < 1U) || (cell_count > 4U)) {
        status = BQ25798_INVALID_PARAM;
    } else {
        uint8_t level = cell_count - 1U; /* 0=1S, 1=2S, 2=3S, 3=4S */
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_RECHARGE_CTRL,
                                       BQ25798_CELL_MASK,
                                       BQ25798_CELL_SHIFT,
                                       level);
        if (status == BQ25798_OK) {
            h->cell_count = cell_count;
        }
    }

    return status;
}

BQ25798_Status_t BQ25798_SetRechargeDeglitch(BQ25798_Handle_t *h, uint8_t deglitch)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (deglitch > 3U) {
        status = BQ25798_INVALID_PARAM;
    } else {
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_RECHARGE_CTRL,
                                       BQ25798_TRECHG_MASK,
                                       BQ25798_TRECHG_SHIFT,
                                       deglitch);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetRechargeVoltageOffset(BQ25798_Handle_t *h, uint16_t voltage_mv)
{
    uint8_t reg;
    BQ25798_Status_t status = BQ25798_VoltageToVRECHG(voltage_mv, &reg);

    if (status == BQ25798_OK) {
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_RECHARGE_CTRL,
                                       BQ25798_VRECHG_MASK,
                                       0U,
                                       reg);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetOTGVoltage(BQ25798_Handle_t *h, uint16_t voltage_mv)
{
    uint16_t reg;
    BQ25798_Status_t status = BQ25798_VoltageToVOTG(voltage_mv, &reg);

    if (status == BQ25798_OK) {
        status = BQ25798_WriteReg16(h, BQ25798_REG_VOTG, reg);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetOTGCurrent(BQ25798_Handle_t *h, uint16_t current_ma)
{
    uint8_t reg;
    BQ25798_Status_t status = BQ25798_CurrentToIOTG(current_ma, &reg);

    if (status == BQ25798_OK) {
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_IOTG,
                                       BQ25798_IOTG_MASK,
                                       0U,
                                       reg);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetOTGTimer(BQ25798_Handle_t *h, uint8_t enable)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_IOTG,
                                                    BQ25798_PRECHG_TMR_BIT,
                                                    7U,
                                                    enable ? 1U : 0U);
    return status;
}

BQ25798_Status_t BQ25798_SetVAC_OVP(BQ25798_Handle_t *h, uint8_t level)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (level > 3U) {
        status = BQ25798_INVALID_PARAM;
    } else {
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_1,
                                       BQ25798_VAC_OVP_MASK,
                                       BQ25798_VAC_OVP_SHIFT,
                                       level);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetTREG(BQ25798_Handle_t *h, uint8_t level)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (level > 3U) {
        status = BQ25798_INVALID_PARAM;
    } else {
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_TEMP_CTRL,
                                       BQ25798_TREG_MASK,
                                       BQ25798_TREG_SHIFT,
                                       level);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetTSHUT(BQ25798_Handle_t *h, uint8_t level)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (level > 3U) {
        status = BQ25798_INVALID_PARAM;
    } else {
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_TEMP_CTRL,
                                       BQ25798_TSHUT_MASK,
                                       BQ25798_TSHUT_SHIFT,
                                       level);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetNTCControl(BQ25798_Handle_t *h, uint8_t ntc0, uint8_t ntc1)
{
    BQ25798_Status_t status = BQ25798_WriteReg8(h, BQ25798_REG_NTC_CTRL_0, ntc0);

    if (status == BQ25798_OK) {
        status = BQ25798_WriteReg8(h, BQ25798_REG_NTC_CTRL_1, ntc1);
    }

    return status;
}

BQ25798_Status_t BQ25798_IgnoreTS(BQ25798_Handle_t *h)
{

    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_NTC_CTRL_1,
                                       BQ25798_IGNORE_TS,
                                       0,
                                       1);

    return status;

}

BQ25798_Status_t BQ25798_ConfigADC(BQ25798_Handle_t *h, uint8_t res, uint8_t ctrl, uint8_t dis0, uint8_t dis1)
{
    ctrl &= 0x7F;   /* This function only configures ADC but does not start it */

    h->adc_handle.res = res;
    h->adc_handle.ctrl = ctrl;
    h->adc_handle.dis0 = dis0;
    h->adc_handle.dis1 = dis1;

    BQ25798_Status_t status = BQ25798_WriteReg8(h, BQ25798_REG_ADC_CTRL, ctrl);

    if (status == BQ25798_OK) {
        status = BQ25798_WriteReg8(h, BQ25798_REG_ADC_FUNC_DIS_0, dis0);
    }

    if (status == BQ25798_OK) {
        status = BQ25798_WriteReg8(h, BQ25798_REG_ADC_FUNC_DIS_1, dis1);
    }

    if (status == BQ25798_OK)
    {
        // Enable IBAT Pin
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_5,
                                BQ25798_EN_IBAT_BIT, BQ25798_EN_IBAT_BIT_SHIFT,
                                BQ25798_EN_IBAT);
    }

    return status;
}

BQ25798_Status_t BQ25798_EnableADC(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_ADC_CTRL,
                                BQ25798_ADC_EN_MASK, BQ25798_ADC_EN_OFFSET,
                                BQ25798_ADC_EN);

    return status;

}

BQ25798_Status_t BQ25798_DisableADC(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_ADC_CTRL,
                                BQ25798_ADC_EN_MASK, BQ25798_ADC_EN_OFFSET,
                                BQ25798_ADC_DISABLE);

    return status;

}

BQ25798_Status_t BQ25798_ADCReadAll(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ADCReadIBUS(h);

    if (status == BQ25798_OK)
    {
        status = BQ25798_ADCReadIBAT(h);
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ADCReadVBUS(h);
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ADCReadVBAT(h);
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ADCReadVSYS(h);
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ADCReadDP(h);
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ADCReadDM(h);
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ADCReadVAC1(h);
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ADCReadVAC2(h);
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ADCReadTS(h);
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ADCReadTDIE(h);
    }

    return status;
}

BQ25798_Status_t BQ25798_ADCReadIBUS(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_IBUS_ADC, &(h->adc_handle.ibus_data));

    return status;
}

BQ25798_Status_t BQ25798_ADCReadIBAT(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_IBAT_ADC, &(h->adc_handle.ibat_data));

    return status;

}

BQ25798_Status_t BQ25798_ADCReadVBUS(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_VBUS_ADC, &(h->adc_handle.vbus_data));

    return status;
}

BQ25798_Status_t BQ25798_ADCReadVAC1(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_VAC1_ADC, &(h->adc_handle.vac1_data));

    return status;
}

BQ25798_Status_t BQ25798_ADCReadVAC2(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_VAC2_ADC, &(h->adc_handle.vac2_data));

    return status;
}

BQ25798_Status_t BQ25798_ADCReadVBAT(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_VBAT_ADC, &(h->adc_handle.vbat_data));

    return status;
}

BQ25798_Status_t BQ25798_ADCReadVSYS(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_VSYS_ADC, &(h->adc_handle.vsys_data));

    return status;
}

BQ25798_Status_t BQ25798_ADCReadTS(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_TS_ADC, &(h->adc_handle.ts_data));

    return status;
}

BQ25798_Status_t BQ25798_ADCReadTDIE(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_TDIE_ADC, &(h->adc_handle.tdie_data));

    return status;
}

BQ25798_Status_t BQ25798_ADCReadDP(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_DP_ADC, &(h->adc_handle.dp_data));

    return status;
}

BQ25798_Status_t BQ25798_ADCReadDM(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_DM_ADC, &(h->adc_handle.dm_data));

    return status;
}

BQ25798_Status_t BQ25798_SetChargeVoltage(BQ25798_Handle_t *h, uint16_t voltage_mv)
{
    BQ25798_Status_t status = BQ25798_ValidateVREG(voltage_mv, h->cell_count);

    if (status == BQ25798_OK) {
        uint16_t reg = 0U;
        status = BQ25798_VoltageToVREG(voltage_mv, &reg);

        if (status == BQ25798_OK) {
            status = BQ25798_WriteReg16(h, BQ25798_REG_VREG, reg);
        }

        if (status == BQ25798_OK) {
            h->vreg_mv = voltage_mv;
        }

    }

    return status;
}

BQ25798_Status_t BQ25798_SetChargeCurrent(BQ25798_Handle_t *h, uint16_t current_ma)
{
    uint16_t reg;
    BQ25798_Status_t status = BQ25798_CurrentToICHG(current_ma, &reg);

    if (status == BQ25798_OK) {
        status = BQ25798_WriteReg16(h, BQ25798_REG_ICHG, reg);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetInputVoltageLimit(BQ25798_Handle_t *h, uint16_t voltage_mv)
{
    uint8_t reg;
    BQ25798_Status_t status = BQ25798_VoltageToVINDPM(voltage_mv, &reg);

    if (status == BQ25798_OK) {
        status = BQ25798_WriteReg8(h, BQ25798_REG_VINDPM, reg);
    }

    return status;
}

BQ25798_Status_t BQ25798_SetInputCurrentLimit(BQ25798_Handle_t *h, uint16_t current_ma)
{
    uint16_t reg;
    BQ25798_Status_t status = BQ25798_CurrentToIINDPM(current_ma, &reg);

    if (status == BQ25798_OK) {
        status = BQ25798_WriteReg16(h, BQ25798_REG_IINDPM, reg);
    }

    return status;
}

BQ25798_Status_t BQ25798_EnableCharge(BQ25798_Handle_t *h, uint8_t enable)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_0, BQ25798_EN_CHG_BIT, 0, enable);
    return status;
}

BQ25798_Status_t BQ25798_ENIBAT(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_5, BQ25798_EN_IBAT_BIT, BQ25798_EN_IBAT_BIT_SHIFT, BQ25798_EN_IBAT);
    return status;
}

BQ25798_Status_t BQ25798_EnableOTG(BQ25798_Handle_t *h, uint8_t enable)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_3, BQ25798_EN_OTG_BIT, 0, enable);
    return status;
}

BQ25798_Status_t BQ25798_SetMPPT(BQ25798_Handle_t *h, uint8_t enable)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_MPPT_CTRL, BQ25798_EN_MPPT_BIT, 0, enable);
    return status;
}

BQ25798_Status_t BQ25798_SetWatchdogTimer(BQ25798_Handle_t *h, uint8_t watchdog_timer_settings)
{
    BQ25798_Status_t status = BQ25798_OK;

    if (watchdog_timer_settings > 7U) {
        status = BQ25798_INVALID_PARAM;
    } else {
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_1,
                                       BQ25798_WATCHDOG_MASK,
                                       BQ25798_WD_SHIFT,
                                       watchdog_timer_settings);
    }

    return status;
}

BQ25798_Status_t BQ25798_ResetWatchdogTimer(BQ25798_Handle_t *h)
{

    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_1,
                                                    BQ25798_WD_RST_BIT,
                                                    BQ25798_WD_RST_SHIFT,
                                                    BQ25798_WD_RST);

    return status;

}

BQ25798_Status_t BQ25798_EnableBackupMode(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_0, BQ25798_EN_BACKUP_BIT, 0, BQ25798_ENABLE_BACKUP_MODE);
    return status;
}

BQ25798_Status_t BQ25798_ExitBackupMode(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_OK;
    uint8_t reg_value;

    /* Step 1: Write BKUP_ACFET1_ON = 1 */
    status = BQ25798_UpdateRegBits(h, BQ25798_REG_TEMP_CTRL, BQ25798_BKUP_ACFET1_ON_BIT, BQ25798_BKUP_ACFET1_ON_SHIFT, BQ25798_BKUP_ACFET_ON);
    if (status == BQ25798_OK)
    {

        /* Step 2: Read EN_ACDRV1 to verify it is 1 */
        status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_CTRL_4, &reg_value);

    }

    if (status == BQ25798_OK) {

        if ((reg_value & BQ25798_EN_ACDRV1_BIT) == 0U) {
            status = BQ25798_ERROR; /* External power not reconnected */
        }
        else {

            /* Step 3: Write BKUP_ACFET1_ON = 0 */
            status = BQ25798_UpdateRegBits(h, BQ25798_REG_TEMP_CTRL, BQ25798_BKUP_ACFET1_ON_BIT, BQ25798_BKUP_ACFET1_ON_SHIFT, BQ25798_BKUP_ACFET_OFF);
        }

    }

    if (status == BQ25798_OK) {

        /* Step 4: Write EN_OTG = 0 */
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_3, BQ25798_EN_OTG_BIT, BQ25798_EN_OTG_BIT_SHIFT, BQ25798_DISABLE_OTG);

    }

    if (status == BQ25798_OK) {

        /* Step 5: Wait 5 seconds */
        HAL_Delay(5000U);

        /* Step 6: Write EN_Backup_mode = 1 */
        status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_0, BQ25798_EN_BACKUP_BIT, BQ25798_EN_CHG_BIT_SHIFT, BQ25798_ENABLE_BACKUP_MODE);

    }

    return status;

}

BQ25798_Status_t BQ25798_EnableShipFET(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_5, BQ25798_SFET_PRESENT_BIT, BQ25798_SFET_PRESENT_BIT_SHIFT, BQ25798_SFET_PRES);
    return status;
}


BQ25798_Status_t BQ25798_EnableIdleMode(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_2, BQ25798_SDRV_CTRL_MASK, BQ25798_SDRV_CTRL_SHIFT, BQ25798_SFET_IDLE);
    return status;
}

BQ25798_Status_t BQ25798_EnableShipMode(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_2, BQ25798_SDRV_CTRL_MASK, BQ25798_SDRV_CTRL_SHIFT, BQ25798_SFET_SHIP);
    return status;
}

BQ25798_Status_t BQ25798_EnableShutdownMode(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_CHG_CTRL_2, BQ25798_SDRV_CTRL_MASK, BQ25798_SDRV_CTRL_SHIFT, BQ25798_SFET_SHUTDOWN);
    return status;
}


BQ25798_Status_t BQ25798_ReadVSYSMIN(BQ25798_Handle_t *h)
{
    uint8_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_VSYSMIN, &reg_val);

    h->vsysmin_mv = (reg_val & BQ25798_VSYSMIN_MASK) * BQ25798_VSYSMIN_STEP_MV + BQ25798_VSYSMIN_OFFSET_MV;

    return status;
}

BQ25798_Status_t BQ25798_ReadVREG(BQ25798_Handle_t *h)
{
    uint16_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_VREG, &reg_val);

    h->vreg_mv = (reg_val & BQ25798_VREG_MASK) * BQ25798_VREG_STEP_MV;

    return status;
}

BQ25798_Status_t BQ25798_ReadCellCount(BQ25798_Handle_t *h)
{
    uint8_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_RECHARGE_CTRL, &reg_val);

    h->cell_count = ((reg_val & BQ25798_CELL_MASK) >> BQ25798_CELL_SHIFT) + 0x01;

    return status;
}

BQ25798_Status_t BQ25798_ReadChgCurrent(BQ25798_Handle_t *h)
{
    uint16_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_ICHG, &reg_val);

    h->chg_current_mA = (reg_val & BQ25798_ICHG_MASK) * BQ25798_ICHG_STEP_MA;

    return status;
}

BQ25798_Status_t BQ25798_ReadPchgCurrent(BQ25798_Handle_t *h)
{
    uint8_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_PRECHARGE_CTRL, &reg_val);

    h->pchg_current_mA = (reg_val & BQ25798_IPRECHG_MASK) * BQ25798_IPRECHG_STEP_MA;

    return status;
}

BQ25798_Status_t BQ25798_ReadWatchdogSettings(BQ25798_Handle_t *h)
{
    uint8_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_CTRL_1, &reg_val);

    switch(reg_val & BQ25798_WATCHDOG_MASK)
    {
        case 0:
            h->wd_timer = 0;
        break;

        case 1:
            h->wd_timer = 0.5;
        break;

        case 2:
            h->wd_timer = 1;
        break;

        case 3:
            h->wd_timer = 2;
        break;

        case 4:
            h->wd_timer = 20;
        break;

        case 5:
            h->wd_timer = 40;
        break;

        case 6:
            h->wd_timer = 80;
        break;

        case 7:
            h->wd_timer = 160;
        break;
    }

    return status;
}

BQ25798_Status_t BQ25798_ReadENCharge(BQ25798_Handle_t *h)
{
    uint16_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_CTRL_0, &reg_val);

    h->en_chg = (reg_val & BQ25798_EN_CHG_BIT) >> BQ25798_EN_CHG_BIT_SHIFT;

    return status;
}

BQ25798_Status_t BQ25798_ReadENACDRV(BQ25798_Handle_t *h)
{
    uint8_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_CTRL_4, &reg_val);

    h->en_acdrv1 = (reg_val & BQ25798_EN_ACDRV1_BIT) >> BQ25798_EN_ACDRV1_BIT_SHIFT;
    h->en_acdrv2 = (reg_val & BQ25798_EN_ACDRV2_BIT) >> BQ25798_EN_ACDRV2_BIT_SHIFT;

    return status;
}

BQ25798_Status_t BQ25798_ReadShipFETPres(BQ25798_Handle_t *h)
{
    uint8_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_CTRL_5, &reg_val);

    h->ship_fet_pres = (reg_val & BQ25798_SFET_PRESENT_BIT) >> BQ25798_SFET_PRESENT_BIT_SHIFT;

    return status;
}

BQ25798_Status_t BQ25798_ReadSDRVCtrl(BQ25798_Handle_t *h)
{
    uint8_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_CTRL_2, &reg_val);

    h->sdrv_ctrl = (reg_val & BQ25798_SDRV_CTRL_MASK) >> BQ25798_SDRV_CTRL_SHIFT;

    return status;
}

BQ25798_Status_t BQ25798_ReadENIBAT(BQ25798_Handle_t *h)
{
    uint8_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_CTRL_5, &reg_val);

    h->en_ibat = (reg_val & BQ25798_EN_IBAT_BIT) >> BQ25798_EN_IBAT_BIT_SHIFT;

    return status;
}

BQ25798_Status_t BQ25798_ReadVOTG(BQ25798_Handle_t *h)
{
    uint16_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_VOTG, &reg_val);

    h->votg_mv = (reg_val & BQ25798_VOTG_MASK) * BQ25798_VOTG_STEP_MV + BQ25798_VOTG_OFFSET_MV;

    return status;
}

BQ25798_Status_t BQ25798_ReadIOTG(BQ25798_Handle_t *h)
{
    uint8_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_IOTG, &reg_val);

    h->iotg_ma = (reg_val & BQ25798_IOTG_MASK) * BQ25798_IOTG_STEP_MA + BQ25798_IOTG_OFFSET_MA;

    return status;
}

BQ25798_Status_t BQ25798_ReadVINDPM(BQ25798_Handle_t *h)
{
    uint8_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_VINDPM, &reg_val);

    h->vindpm_mv = (reg_val & BQ25798_VINDPM_MASK) * BQ25798_VINDPM_STEP_MV + BQ25798_VINDPM_OFFSET_MV;

    return status;
}

BQ25798_Status_t BQ25798_ReadIINDPM(BQ25798_Handle_t *h)
{
    uint16_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg16(h, BQ25798_REG_IINDPM, &reg_val);

    h->iindpm_ma = (reg_val & BQ25798_IINDPM_MASK) * BQ25798_IINDPM_STEP_MA + BQ25798_IINDPM_OFFSET_MA;

    return status;
}


BQ25798_Status_t BQ25798_ReadENBckup(BQ25798_Handle_t *h)
{
    uint8_t reg_val = 0;

    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_CTRL_0, &reg_val);

    h->en_bckup = reg_val & BQ25798_EN_BACKUP_BIT;

    return status;
}

BQ25798_Status_t BQ25798_ReadChargeConf(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadVSYSMIN(h);


    if (status == BQ25798_OK)
    {
        BQ25798_ReadVREG(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadCellCount(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadChgCurrent(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadPchgCurrent(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadWatchdogSettings(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadENCharge(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadENACDRV(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadShipFETPres(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadSDRVCtrl(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadENIBAT(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadVOTG(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadIOTG(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadVINDPM(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadIINDPM(h);
    }

    if (status == BQ25798_OK)
    {
        BQ25798_ReadENBckup(h);
    }

    return status;
}

BQ25798_Status_t BQ25798_ReadStatus(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_STAT_0, &(h->charger_status_0));

    if (status == BQ25798_OK)
    {
        status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_STAT_1, &(h->charger_status_1));
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_STAT_2, &(h->charger_status_2));
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_STAT_3, &(h->charger_status_3));
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_STAT_4, &(h->charger_status_4));
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ReadReg8(h, BQ25798_REG_FAULT_STAT_0, &(h->fault_status_0));
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ReadReg8(h, BQ25798_REG_FAULT_STAT_1, &(h->fault_status_1));
    }

    return status;

}

BQ25798_Status_t BQ25798_ReadFlags(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_FLAG_0, &(h->charger_flag_0));

    if (status == BQ25798_OK)
    {
        status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_FLAG_1, &(h->charger_flag_1));
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_FLAG_2, &(h->charger_flag_2));
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ReadReg8(h, BQ25798_REG_CHG_FLAG_3, &(h->charger_flag_3));
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ReadReg8(h, BQ25798_REG_FAULT_FLAG_0, &(h->fault_flag_0));
    }

    if (status == BQ25798_OK)
    {
        status = BQ25798_ReadReg8(h, BQ25798_REG_FAULT_FLAG_1, &(h->fault_flag_1));
    }

    return status;

}

BQ25798_Status_t BQ25798_Reset(BQ25798_Handle_t *h)
{
    BQ25798_Status_t status = BQ25798_UpdateRegBits(h, BQ25798_REG_TERM_CTRL, BQ25798_REG_RST_BIT, BQ25798_REG_RST_SHIFT, BQ25798_RST);
    return status;
}