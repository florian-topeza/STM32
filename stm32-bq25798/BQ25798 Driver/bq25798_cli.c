/**
 * @file        bq25798_cli.c
 * @author      Florian Topeza
 * @brief       Command line interface (CLI) driver for the BQ25798 battery charger from Texas Instruments.
                It uses the driver bq25798.c, which is a driver to interface the BQ25798 battery charger with an STM32 microcontroller.
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

#include "bq25798_cli.h"
#include "bq25798.h"


/* === Functions ==== */

BQ25798_CLI_Status_t BQ25798_CLI_Init(BQ25798_CLI_Handle_t *cli, BQ25798_Handle_t bq, UART_HandleTypeDef *huart)
{

    BQ25798_CLI_Status_t cli_status = BQ25798_CLI_OK;

    if ((cli == NULL) || (huart == NULL))
    {
        cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
    }

    else
    {
        // Initialize BQ25798 command line interface (CLI)
        cli->bq                   = bq;
        cli->huart                = huart;

        cli_status = BQ25798_CLI_Reset(cli);

    }

    return cli_status;

}

BQ25798_CLI_Status_t BQ25798_CLI_Reset(BQ25798_CLI_Handle_t *cli)
{
    BQ25798_CLI_Status_t cli_status = BQ25798_CLI_OK;

    cli->data_refresh_period  = BQ25798_CLI_DEFAULT_REFRESH_PERIOD;

    cli->send_adc_data        = 0;
    cli->send_chg_conf        = 0;
    cli->send_status          = 0;
    cli->send_flags           = 0;
    cli->pause                = 0;

    // Configure ADC
    if (BQ25798_ConfigADC(&(cli->bq), CLI_ADC_RES, CLI_ADC_CTRL, CLI_ADC_DIS0, CLI_ADC_DIS1) != BQ25798_OK)
    {
        cli_status = BQ25798_CLI_ERROR_EXECUTION;
    }

    // Enable IBAT pin to track current from and to the battery
    if (BQ25798_ENIBAT(&(cli->bq)) != BQ25798_OK)
    {
        cli_status = BQ25798_CLI_ERROR_EXECUTION;
    }

    // Disable watchdog to ensure that settings changed by user through the CLI are not reset periodically
    if (BQ25798_SetWatchdogTimer(&(cli->bq), BQ25798_WATCHDOG_DISABLE) != BQ25798_OK)
    {
        cli_status = BQ25798_CLI_ERROR_EXECUTION;
    }

    // Set ship FET as present to enable the use of idle, ship and shutdown modes
    if (BQ25798_EnableShipFET(&(cli->bq)) != BQ25798_OK)
    {
        cli_status = BQ25798_CLI_ERROR_EXECUTION;
    }

    return cli_status;
}

BQ25798_Status_t BQ25798_CLI_ClearScreen(BQ25798_CLI_Handle_t *cli)
{
    BQ25798_Status_t status = BQ25798_OK;

    // Clear screen and place cursor on the top left
    char clear[] = "\033[2J\033[H"; // ANSI clear screen sequence, works on most terminals

    if (HAL_UART_Transmit(cli->huart, (uint8_t*)clear, strlen(clear), HAL_MAX_DELAY) != HAL_OK)
    {
        status = BQ25798_ERROR;
    }

    return status;

}

BQ25798_Status_t BQ25798_CLI_SendADCData(BQ25798_CLI_Handle_t *cli)
{

    BQ25798_Status_t status = BQ25798_OK;

    // Start ADC
    // The ADC performs one conversion in one shot mode and then deactivates.
    status = BQ25798_EnableADC( &(cli->bq));

    status = BQ25798_ADCReadAll(&(cli->bq));

    if (status == BQ25798_OK)
    {

        uint8_t data_buffer[BQ25798_CLI_MAX_UART_TX_BUFFER_SIZE] = {0};

        // Collect the data.
        int16_t ibus = BQ25798_CONVERT_UINT16T_2_INT16T(cli->bq.adc_handle.ibus_data);
        int16_t ibat = BQ25798_CONVERT_UINT16T_2_INT16T(cli->bq.adc_handle.ibat_data);
        uint16_t vbus = cli->bq.adc_handle.vbus_data;
        uint16_t vbat = cli->bq.adc_handle.vbat_data;
        uint16_t vac1 = cli->bq.adc_handle.vac1_data;
        uint16_t vac2 = cli->bq.adc_handle.vac2_data;
        uint16_t vsys = cli->bq.adc_handle.vsys_data;
        uint16_t ts = cli->bq.adc_handle.ts_data * BQ25798_ADC_TS_STEP_NUM / BQ25798_ADC_TS_STEP_DEN;
        int16_t tdie = BQ25798_CONVERT_UINT16T_2_INT16T(cli->bq.adc_handle.tdie_data) >> BQ25798_ADC_TDIE_SHIFT;
        uint16_t dp = cli->bq.adc_handle.dp_data;
        uint16_t dm = cli->bq.adc_handle.dm_data;


        int len = snprintf((char*)data_buffer, sizeof(data_buffer),
                "---- BQ25798 ADC DATA ----\r\n"
                "\r\n"
                "  IBUS = %d mA              IBAT = %d mA              VBUS = %d mV           VBAT = %d mV\r\n"
                "  VAC1 = %d mV           VAC2 = %d mV              VSYS = %d mV\r\n"
                "  TS   = %d (percents)      TDIE = %d deg             DP   = %d mV              DM   = %d mV\r\n"
                "\r\n"
                "",
                ibus, ibat, vbus, vbat, vac1, vac2, vsys, ts, tdie, dp, dm);

        // Send data
        if (status == BQ25798_OK)
        {
            if (HAL_UART_Transmit(cli->huart, data_buffer, len, HAL_MAX_DELAY) != HAL_OK)
            {
                status = BQ25798_ERROR;
            }
        }

    }

    return status;
}

BQ25798_Status_t BQ25798_CLI_SendChargingConf(BQ25798_CLI_Handle_t *cli)
{

    BQ25798_Status_t status = BQ25798_OK;

    uint8_t data_buffer[BQ25798_CLI_MAX_UART_TX_BUFFER_SIZE] = {0};

    status = BQ25798_ReadChargeConf( &(cli->bq) );

    if (status == BQ25798_OK)
    {

        uint16_t vreg_mv            = cli->bq.vreg_mv;
        uint16_t chg_current_mA     = cli->bq.chg_current_mA;
        uint8_t cell_count          = cli->bq.cell_count;
        float wd_timer              = cli->bq.wd_timer;
        uint16_t vsysmin_mv         = cli->bq.vsysmin_mv;
        uint16_t pchg_current_mA    = cli->bq.pchg_current_mA;
        uint8_t en_chg              = cli->bq.en_chg;
        uint8_t en_acdrv1           = cli->bq.en_acdrv1;
        uint8_t en_acdrv2           = cli->bq.en_acdrv2;
        uint8_t ship_fet_pres       = cli->bq.ship_fet_pres;
        uint8_t sdrv_ctrl           = cli->bq.sdrv_ctrl;
        uint8_t en_ibat             = cli->bq.en_ibat;
        uint8_t en_bckup            = cli->bq.en_bckup;
        uint16_t votg_mv            = cli->bq.votg_mv;
        uint16_t iotg_ma            = cli->bq.iotg_ma;
        uint16_t vindpm_mv           = cli->bq.vindpm_mv;
        uint16_t iindpm_ma           = cli->bq.iindpm_ma;


        int len = snprintf((char*)data_buffer, sizeof(data_buffer),
                "---- BQ25798 CHARGING CONFIGURATION ----\r\n"
                "\r\n"
                "  VREG = %d mV            ICHG = %d mA           CELLS = %d                 WATCHDOG = %.1f s\r\n"
                "  VSYSMIN = %d mV         IPCHG = %d mA           EN CHG = %d                SHIP FET Present ? %d\r\n"
                "  EN ACDRV1 = %d             EN ACDRV2 = %d            SDRV CTRL = %d             EN IBAT Pin = %d \r\n"
                "  EN BACKUP = %d             VOTG = %d mV          IOTG = %d mA            VINDPM = %d mV        IINDPM = %d mA\r\n"
                "\r\n"
                "",
                vreg_mv, chg_current_mA, cell_count, wd_timer, vsysmin_mv, pchg_current_mA, en_chg,
                ship_fet_pres, en_acdrv1, en_acdrv2, sdrv_ctrl, en_ibat, en_bckup, votg_mv, iotg_ma, vindpm_mv, iindpm_ma);

        // Send data
        if (status == BQ25798_OK)
        {
            if (HAL_UART_Transmit(cli->huart, data_buffer, len, HAL_MAX_DELAY) != HAL_OK)
            {
                status = BQ25798_ERROR;
            }
        }

    }

    return status;
}

BQ25798_Status_t BQ25798_CLI_SendStatus(BQ25798_CLI_Handle_t *cli)
{
    BQ25798_Status_t status = BQ25798_OK;

    uint8_t data_buffer[BQ25798_CLI_MAX_UART_TX_BUFFER_SIZE] = {0};

    status = BQ25798_ReadStatus( &(cli->bq) );

    if (status == BQ25798_OK)
    {

        uint8_t charger_status_0 = cli->bq.charger_status_0;
        uint8_t charger_status_1 = cli->bq.charger_status_1;
        uint8_t charger_status_2 = cli->bq.charger_status_2;
        uint8_t charger_status_3 = cli->bq.charger_status_3;
        uint8_t charger_status_4 = cli->bq.charger_status_4;

        uint8_t fault_status_0 = cli->bq.fault_status_0;
        uint8_t fault_status_1 = cli->bq.fault_status_1;

        uint8_t indpm_status = (charger_status_0 & BQ25798_INDPM_STATUS_BIT_MASK) >> BQ25798_INDPM_STATUS_BIT_SHIFT;
        uint8_t vindpm_status = (charger_status_0 & BQ25798_VINDPM_STATUS_BIT_MASK) >> BQ25798_VINDPM_STATUS_BIT_SHIFT;
        uint8_t wd_status = (charger_status_0 & BQ25798_WD_STATUS_BIT_MASK) >> BQ25798_WD_STATUS_BIT_SHIFT;
        uint8_t poor_src_status = (charger_status_0 & BQ25798_POOR_SRC_STATUS_BIT_MASK) >> BQ25798_POOR_SRC_STATUS_BIT_SHIFT;
        uint8_t pg_status = (charger_status_0 & BQ25798_PG_STATUS_BIT_MASK) >> BQ25798_PG_STATUS_BIT_SHIFT;
        uint8_t ac2_present_status = (charger_status_0 & BQ25798_AC2_PRESENT_STATUS_BIT_MASK) >> BQ25798_AC2_PRESENT_STATUS_BIT_SHIFT;
        uint8_t ac1_present_status = (charger_status_0 & BQ25798_AC1_PRESENT_STATUS_BIT_MASK) >> BQ25798_AC1_PRESENT_STATUS_BIT_SHIFT;
        uint8_t vbus_present_status = (charger_status_0 & BQ25798_VBUS_PRESENT_STATUS_BIT_MASK) >> BQ25798_VBUS_PRESENT_STATUS_BIT_SHIFT;

        uint8_t chg_status = (charger_status_1 & BQ25798_CHG_STATUS_BIT_MASK) >> BQ25798_CHG_STATUS_BIT_SHIFT;
        uint8_t vbus_stat = (charger_status_1 & BQ25798_VBUS_STAT_BIT_MASK) >> BQ25798_VBUS_STAT_BIT_SHIFT;
        uint8_t adapter_detect_status = (charger_status_1 & BQ25798_ADAPTER_DETECT_STATUS_BIT_MASK) >> BQ25798_ADAPTER_DETECT_STATUS_BIT_SHIFT;

        uint8_t ico_status = (charger_status_2 & BQ25798_ICO_STATUS_BIT_MASK) >> BQ25798_ICO_STATUS_BIT_SHIFT;
        uint8_t treg_status = (charger_status_2 & BQ25798_TREG_STATUS_BIT_MASK) >> BQ25798_TREG_STATUS_BIT_SHIFT;
        uint8_t dpdm_status = (charger_status_2 & BQ25798_DPDM_STATUS_BIT_MASK) >> BQ25798_DPDM_STATUS_BIT_SHIFT;
        uint8_t vbat_present_status = (charger_status_2 & BQ25798_VBAT_PRESENT_STATUS_BIT_MASK) >> BQ25798_VBAT_PRESENT_STATUS_BIT_SHIFT;

        uint8_t acfet2_rbfet2_status = (charger_status_3 & BQ25798_ACFET2_RBFET2_STATUS_BIT_MASK) >> BQ25798_ACFET2_RBFET2_STATUS_BIT_SHIFT;
        uint8_t acfet1_rbfet1_status = (charger_status_3 & BQ25798_ACFET1_RBFET1_STATUS_BIT_MASK) >> BQ25798_ACFET1_RBFET1_STATUS_BIT_SHIFT;
        uint8_t adc_conv_status = (charger_status_3 & BQ25798_ADC_CONV_STATUS_BIT_MASK) >> BQ25798_ADC_CONV_STATUS_BIT_SHIFT;
        uint8_t vsys_status = (charger_status_3 & BQ25798_VSYS_STATUS_BIT_MASK) >> BQ25798_VSYS_STATUS_BIT_SHIFT;
        uint8_t chg_safety_timer_status = (charger_status_3 & BQ25798_CHG_SAFETY_TIM_STATUS_BIT_MASK) >> BQ25798_CHG_SAFETY_TIM_STATUS_BIT_SHIFT;
        uint8_t trickle_timer_status = (charger_status_3 & BQ25798_TRICKLE_TIM_STATUS_BIT_MASK) >> BQ25798_TRICKLE_TIM_STATUS_BIT_SHIFT;
        uint8_t prechg_timer_status = (charger_status_3 & BQ25798_PRECHG_TIM_STATUS_BIT_MASK) >> BQ25798_PRECHG_TIM_STATUS_BIT_SHIFT;
        uint8_t topoff_timer_status = (charger_status_3 & BQ25798_TOPOFF_TIM_STATUS_BIT_MASK) >> BQ25798_TOPOFF_TIM_STATUS_BIT_SHIFT;

        uint8_t vbatotg_lowv_status = (charger_status_4 & BQ25798_VBATOTG_LOWV_STATUS_BIT_MASK) >> BQ25798_VBATOTG_LOWV_STATUS_BIT_SHIFT;
        uint8_t ts_cold_status = (charger_status_4 & BQ25798_TS_COLD_STATUS_BIT_MASK) >> BQ25798_TS_COLD_STATUS_BIT_SHIFT;
        uint8_t ts_cool_status = (charger_status_4 & BQ25798_TS_COOL_STATUS_BIT_MASK) >> BQ25798_TS_COOL_STATUS_BIT_SHIFT;
        uint8_t ts_warm_status = (charger_status_4 & BQ25798_TS_WARM_STATUS_BIT_MASK) >> BQ25798_TS_WARM_STATUS_BIT_SHIFT;
        uint8_t ts_hot_status = (charger_status_4 & BQ25798_TS_HOT_STATUS_BIT_MASK) >> BQ25798_TS_HOT_STATUS_BIT_SHIFT;

        uint8_t ibat_reg_status = (fault_status_0 & BQ25798_IBAT_REG_STATUS_BIT_MASK) >> BQ25798_IBAT_REG_STATUS_BIT_SHIFT;
        uint8_t vbus_ovp_status = (fault_status_0 & BQ25798_VBUS_OVP_STATUS_BIT_MASK) >> BQ25798_VBUS_OVP_STATUS_BIT_SHIFT;
        uint8_t vbat_ovp_status = (fault_status_0 & BQ25798_VBAT_OVP_STATUS_BIT_MASK) >> BQ25798_VBAT_OVP_STATUS_BIT_SHIFT;
        uint8_t ibus_ocp_status = (fault_status_0 & BQ25798_IBUS_OCP_STATUS_BIT_MASK) >> BQ25798_IBUS_OCP_STATUS_BIT_SHIFT;
        uint8_t ibat_ocp_status = (fault_status_0 & BQ25798_IBAT_OCP_STATUS_BIT_MASK) >> BQ25798_IBAT_OCP_STATUS_BIT_SHIFT;
        uint8_t conv_status = (fault_status_0 & BQ25798_CONV_OCP_STATUS_BIT_MASK) >> BQ25798_CONV_OCP_STATUS_BIT_SHIFT;
        uint8_t vac2_ovp_status = (fault_status_0 & BQ25798_VAC2_OVP_STATUS_BIT_MASK) >> BQ25798_VAC2_OVP_STATUS_BIT_SHIFT;
        uint8_t vac1_ovp_status = (fault_status_0 & BQ25798_VAC1_OVP_STATUS_BIT_MASK) >> BQ25798_VAC1_OVP_STATUS_BIT_SHIFT;

        uint8_t vsys_short_status = (fault_status_1 & BQ25798_VSYS_SHORT_STATUS_BIT_MASK) >> BQ25798_VSYS_SHORT_STATUS_BIT_SHIFT;
        uint8_t vsys_ovp_status = (fault_status_1 & BQ25798_VSYS_OVP_STATUS_BIT_MASK) >> BQ25798_VSYS_OVP_STATUS_BIT_SHIFT;
        uint8_t otg_ovp_status = (fault_status_1 & BQ25798_OTG_OVP_STATUS_BIT_MASK) >> BQ25798_OTG_OVP_STATUS_BIT_SHIFT;
        uint8_t otg_uvp_status = (fault_status_1 & BQ25798_OTG_UVP_STATUS_BIT_MASK) >> BQ25798_OTG_UVP_STATUS_BIT_SHIFT;
        uint8_t tshutdown_status = (fault_status_1 & BQ25798_TSHUTDOWN_STATUS_BIT_MASK) >> BQ25798_TSHUTDOWN_STATUS_BIT_SHIFT;

        int len = snprintf((char*)data_buffer, sizeof(data_buffer),
            "---- BQ25798 STATUS REGISTERS ----\r\n"
            "\r\n"
            "--- Charger Status 0 ---\r\n"
            "  INDPM = %d       VINDPM = %d       WD = %d       POOR_SRC = %d       PG = %d\r\n"
            "  AC2_PRES = %d    AC1_PRES = %d    VBUS_PRES = %d\r\n"
            "\r\n"
            "--- Charger Status 1 ---\r\n"
            "  CHG_STAT = %d     VBUS_STAT = %d     ADAPTER_DETECT = %d\r\n"
            "\r\n"
            "--- Charger Status 2 ---\r\n"
            "  ICO = %d         TREG = %d         DPDM = %d         VBAT_PRES = %d\r\n"
            "\r\n"
            "--- Charger Status 3 ---\r\n"
            "  ACFET2_RBFET2 = %d   ACFET1_RBFET1 = %d   ADC_CONV = %d   VSYS = %d\r\n"
            "  CHG_SAFETY_TIM = %d   TRICKLE_TIM = %d   PRECHG_TIM = %d   TOPOFF_TIM = %d\r\n"
            "\r\n"
            "--- Charger Status 4 ---\r\n"
            "  VBATOTG_LOWV = %d   TS_COLD = %d   TS_COOL = %d   TS_WARM = %d   TS_HOT = %d\r\n"
            "\r\n"
            "--- Fault Status 0 ---\r\n"
            "  IBAT_REG = %d     VBUS_OVP = %d     VBAT_OVP = %d     IBUS_OCP = %d\r\n"
            "  IBAT_OCP = %d     CONV_OCP = %d     VAC2_OVP = %d     VAC1_OVP = %d\r\n"
            "\r\n"
            "--- Fault Status 1 ---\r\n"
            "  VSYS_SHORT = %d   VSYS_OVP = %d     OTG_OVP = %d     OTG_UVP = %d     TSHUTDOWN = %d\r\n"
            "\r\n",
            // Charger Status 0
            indpm_status, vindpm_status, wd_status, poor_src_status, pg_status,
            ac2_present_status, ac1_present_status, vbus_present_status,
            // Charger Status 1
            chg_status, vbus_stat, adapter_detect_status,
            // Charger Status 2
            ico_status, treg_status, dpdm_status, vbat_present_status,
            // Charger Status 3
            acfet2_rbfet2_status, acfet1_rbfet1_status, adc_conv_status, vsys_status,
            chg_safety_timer_status, trickle_timer_status, prechg_timer_status, topoff_timer_status,
            // Charger Status 4
            vbatotg_lowv_status, ts_cold_status, ts_cool_status, ts_warm_status, ts_hot_status,
            // Fault Status 0
            ibat_reg_status, vbus_ovp_status, vbat_ovp_status, ibus_ocp_status,
            ibat_ocp_status, conv_status, vac2_ovp_status, vac1_ovp_status,
            // Fault Status 1
            vsys_short_status, vsys_ovp_status, otg_ovp_status, otg_uvp_status, tshutdown_status
        );

        // Send data

        if (HAL_UART_Transmit(cli->huart, data_buffer, len, HAL_MAX_DELAY) != HAL_OK)
        {
            status = BQ25798_ERROR;
        }

    }

    return status;

}

BQ25798_Status_t BQ25798_CLI_SendFlags(BQ25798_CLI_Handle_t *cli)
{
    BQ25798_Status_t status = BQ25798_OK;
    uint8_t data_buffer[BQ25798_CLI_MAX_UART_TX_BUFFER_SIZE] = {0};

    status = BQ25798_ReadFlags( &(cli->bq) );

    if (status == BQ25798_OK)
    {
        // --- Extraire les bits de REG22 (Charger Flag 0) ---
        uint8_t iindpm_flag = (cli->bq.charger_flag_0 & BQ25798_IINDPM_FLAG_BIT_MASK) >> BQ25798_IINDPM_FLAG_BIT_SHIFT;
        uint8_t vindpm_flag = (cli->bq.charger_flag_0 & BQ25798_VINDPM_FLAG_BIT_MASK) >> BQ25798_VINDPM_FLAG_BIT_SHIFT;
        uint8_t wd_flag = (cli->bq.charger_flag_0 & BQ25798_WD_FLAG_BIT_MASK) >> BQ25798_WD_FLAG_BIT_SHIFT;
        uint8_t poorsrc_flag = (cli->bq.charger_flag_0 & BQ25798_POORSRC_FLAG_BIT_MASK) >> BQ25798_POORSRC_FLAG_BIT_SHIFT;
        uint8_t pg_flag = (cli->bq.charger_flag_0 & BQ25798_PG_FLAG_BIT_MASK) >> BQ25798_PG_FLAG_BIT_SHIFT;
        uint8_t ac2_present_flag = (cli->bq.charger_flag_0 & BQ25798_AC2_PRESENT_FLAG_BIT_MASK) >> BQ25798_AC2_PRESENT_FLAG_BIT_SHIFT;
        uint8_t ac1_present_flag = (cli->bq.charger_flag_0 & BQ25798_AC1_PRESENT_FLAG_BIT_MASK) >> BQ25798_AC1_PRESENT_FLAG_BIT_SHIFT;
        uint8_t vbus_present_flag = (cli->bq.charger_flag_0 & BQ25798_VBUS_PRESENT_FLAG_BIT_MASK) >> BQ25798_VBUS_PRESENT_FLAG_BIT_SHIFT;

        // --- Extraire les bits de REG23 (Charger Flag 1) ---
        uint8_t chg_flag = (cli->bq.charger_flag_1 & BQ25798_CHG_FLAG_BIT_MASK) >> BQ25798_CHG_FLAG_BIT_SHIFT;
        uint8_t ico_flag = (cli->bq.charger_flag_1 & BQ25798_ICO_FLAG_BIT_MASK) >> BQ25798_ICO_FLAG_BIT_SHIFT;
        uint8_t vbus_flag_reg23 = (cli->bq.charger_flag_1 & BQ25798_VBUS_FLAG_BIT_MASK) >> BQ25798_VBUS_FLAG_BIT_SHIFT;
        uint8_t treg_flag = (cli->bq.charger_flag_1 & BQ25798_TREG_FLAG_BIT_MASK) >> BQ25798_TREG_FLAG_BIT_SHIFT;
        uint8_t vbat_present_flag_reg23 = (cli->bq.charger_flag_1 & BQ25798_VBAT_PRESENT_FLAG_BIT_MASK) >> BQ25798_VBAT_PRESENT_FLAG_BIT_SHIFT;
        uint8_t bc1p2_done_flag = (cli->bq.charger_flag_1 & BQ25798_BC1P2_DONE_FLAG_BIT_MASK) >> BQ25798_BC1P2_DONE_FLAG_BIT_SHIFT;

        // --- Extraire les bits de REG24 (Charger Flag 2) ---
        uint8_t dpdm_done_flag = (cli->bq.charger_flag_2 & BQ25798_DPDM_DONE_FLAG_BIT_MASK) >> BQ25798_DPDM_DONE_FLAG_BIT_SHIFT;
        uint8_t adc_done_flag = (cli->bq.charger_flag_2 & BQ25798_ADC_DONE_FLAG_BIT_MASK) >> BQ25798_ADC_DONE_FLAG_BIT_SHIFT;
        uint8_t vsys_flag_reg24 = (cli->bq.charger_flag_2 & BQ25798_VSYS_FLAG_BIT_MASK) >> BQ25798_VSYS_FLAG_BIT_SHIFT;
        uint8_t chg_tmr_flag = (cli->bq.charger_flag_2& BQ25798_CHG_TMR_FLAG_BIT_MASK) >> BQ25798_CHG_TMR_FLAG_BIT_SHIFT;
        uint8_t trichg_tmr_flag = (cli->bq.charger_flag_2 & BQ25798_TRICHG_TMR_FLAG_BIT_MASK) >> BQ25798_TRICHG_TMR_FLAG_BIT_SHIFT;
        uint8_t prechg_tmr_flag = (cli->bq.charger_flag_2 & BQ25798_PRECHG_TMR_FLAG_BIT_MASK) >> BQ25798_PRECHG_TMR_FLAG_BIT_SHIFT;
        uint8_t topoff_tmr_flag = (cli->bq.charger_flag_2 & BQ25798_TOPOFF_TMR_FLAG_BIT_MASK) >> BQ25798_TOPOFF_TMR_FLAG_BIT_SHIFT;

        // --- Extraire les bits de REG25 (Charger Flag 3) ---
        uint8_t vbatotg_low_flag = (cli->bq.charger_flag_3 & BQ25798_VBATOTG_LOW_FLAG_BIT_MASK) >> BQ25798_VBATOTG_LOW_FLAG_BIT_SHIFT;
        uint8_t ts_cold_flag = (cli->bq.charger_flag_3 & BQ25798_TS_COLD_FLAG_BIT_MASK) >> BQ25798_TS_COLD_FLAG_BIT_SHIFT;
        uint8_t ts_cool_flag = (cli->bq.charger_flag_3 & BQ25798_TS_COOL_FLAG_BIT_MASK) >> BQ25798_TS_COOL_FLAG_BIT_SHIFT;
        uint8_t ts_warm_flag = (cli->bq.charger_flag_3 & BQ25798_TS_WARM_FLAG_BIT_MASK) >> BQ25798_TS_WARM_FLAG_BIT_SHIFT;
        uint8_t ts_hot_flag = (cli->bq.charger_flag_3 & BQ25798_TS_HOT_FLAG_BIT_MASK) >> BQ25798_TS_HOT_FLAG_BIT_SHIFT;

        // --- Extraire les bits de REG26 (Fault Flag 0) ---
        uint8_t ibat_reg_flag = (cli->bq.fault_flag_0 & BQ25798_IBAT_REG_FLAG_BIT_MASK) >> BQ25798_IBAT_REG_FLAG_BIT_SHIFT;
        uint8_t vbus_ovp_flag = (cli->bq.fault_flag_0 & BQ25798_VBUS_OVP_FLAG_BIT_MASK) >> BQ25798_VBUS_OVP_FLAG_BIT_SHIFT;
        uint8_t vbat_ovp_flag = (cli->bq.fault_flag_0 & BQ25798_VBAT_OVP_FLAG_BIT_MASK) >> BQ25798_VBAT_OVP_FLAG_BIT_SHIFT;
        uint8_t ibus_ocp_flag = (cli->bq.fault_flag_0 & BQ25798_IBUS_OCP_FLAG_BIT_MASK) >> BQ25798_IBUS_OCP_FLAG_BIT_SHIFT;
        uint8_t ibat_ocp_flag = (cli->bq.fault_flag_0 & BQ25798_IBAT_OCP_FLAG_BIT_MASK) >> BQ25798_IBAT_OCP_FLAG_BIT_SHIFT;
        uint8_t conv_ocp_flag = (cli->bq.fault_flag_0 & BQ25798_CONV_OCP_FLAG_BIT_MASK) >> BQ25798_CONV_OCP_FLAG_BIT_SHIFT;
        uint8_t vac2_ovp_flag = (cli->bq.fault_flag_0 & BQ25798_VAC2_OVP_FLAG_BIT_MASK) >> BQ25798_VAC2_OVP_FLAG_BIT_SHIFT;
        uint8_t vac1_ovp_flag = (cli->bq.fault_flag_0 & BQ25798_VAC1_OVP_FLAG_BIT_MASK) >> BQ25798_VAC1_OVP_FLAG_BIT_SHIFT;

        // --- Extraire les bits de REG27 (Fault Flag 1) ---
        uint8_t vsys_short_flag = (cli->bq.fault_flag_1 & BQ25798_VSYS_SHORT_FLAG_BIT_MASK) >> BQ25798_VSYS_SHORT_FLAG_BIT_SHIFT;
        uint8_t vsys_ovp_flag = (cli->bq.fault_flag_1 & BQ25798_VSYS_OVP_FLAG_BIT_MASK) >> BQ25798_VSYS_OVP_FLAG_BIT_SHIFT;
        uint8_t otg_ovp_flag = (cli->bq.fault_flag_1 & BQ25798_OTG_OVP_FLAG_BIT_MASK) >> BQ25798_OTG_OVP_FLAG_BIT_SHIFT;
        uint8_t otg_uvp_flag = (cli->bq.fault_flag_1 & BQ25798_OTG_UVP_FLAG_BIT_MASK) >> BQ25798_OTG_UVP_FLAG_BIT_SHIFT;
        uint8_t tshut_flag = (cli->bq.fault_flag_1 & BQ25798_TSHUT_FLAG_BIT_MASK) >> BQ25798_TSHUT_FLAG_BIT_SHIFT;

        // Formater et envoyer via UART
        int len = snprintf((char*)data_buffer, sizeof(data_buffer),
            "---- BQ25798 FLAG REGISTERS ----\r\n"
            "\r\n"
            "--- Charger Flag 0 (REG22) ---\r\n"
            "  IINDPM = %d       VINDPM = %d       WD = %d       POORSRC = %d       PG = %d\r\n"
            "  AC2_PRES = %d    AC1_PRES = %d    VBUS_PRES = %d\r\n"
            "\r\n"
            "--- Charger Flag 1 (REG23) ---\r\n"
            "  CHG = %d         ICO = %d         VBUS = %d       TREG = %d\r\n"
            "  VBAT_PRES = %d   BC1.2_DONE = %d\r\n"
            "\r\n"
            "--- Charger Flag 2 (REG24) ---\r\n"
            "  DPDM_DONE = %d   ADC_DONE = %d     VSYS = %d       CHG_TMR = %d\r\n"
            "  TRICHG_TMR = %d  PRECHG_TMR = %d  TOPOFF_TMR = %d\r\n"
            "\r\n"
            "--- Charger Flag 3 (REG25) ---\r\n"
            "  VBATOTG_LOW = %d  TS_COLD = %d    TS_COOL = %d    TS_WARM = %d    TS_HOT = %d\r\n"
            "\r\n"
            "--- Fault Flag 0 (REG26) ---\r\n"
            "  IBAT_REG = %d    VBUS_OVP = %d    VBAT_OVP = %d    IBUS_OCP = %d\r\n"
            "  IBAT_OCP = %d    CONV_OCP = %d    VAC2_OVP = %d    VAC1_OVP = %d\r\n"
            "\r\n"
            "--- Fault Flag 1 (REG27) ---\r\n"
            "  VSYS_SHORT = %d  VSYS_OVP = %d   OTG_OVP = %d    OTG_UVP = %d    TSHUT = %d\r\n"
            "\r\n",
            // Charger Flag 0 (REG22)
            iindpm_flag, vindpm_flag, wd_flag, poorsrc_flag, pg_flag,
            ac2_present_flag, ac1_present_flag, vbus_present_flag,
            // Charger Flag 1 (REG23)
            chg_flag, ico_flag, vbus_flag_reg23, treg_flag,
            vbat_present_flag_reg23, bc1p2_done_flag,
            // Charger Flag 2 (REG24)
            dpdm_done_flag, adc_done_flag, vsys_flag_reg24, chg_tmr_flag,
            trichg_tmr_flag, prechg_tmr_flag, topoff_tmr_flag,
            // Charger Flag 3 (REG25)
            vbatotg_low_flag, ts_cold_flag, ts_cool_flag, ts_warm_flag, ts_hot_flag,
            // Fault Flag 0 (REG26)
            ibat_reg_flag, vbus_ovp_flag, vbat_ovp_flag, ibus_ocp_flag,
            ibat_ocp_flag, conv_ocp_flag, vac2_ovp_flag, vac1_ovp_flag,
            // Fault Flag 1 (REG27)
            vsys_short_flag, vsys_ovp_flag, otg_ovp_flag, otg_uvp_flag, tshut_flag
        );

        if (HAL_UART_Transmit(cli->huart, data_buffer, len, HAL_MAX_DELAY) != HAL_OK)
        {
            status = BQ25798_ERROR;
        }
    }

    return status;
}

// Function to compare strings (case-insensitive)
static bool str_equals(const char *a, const char *b)
{
    return strcmp(a, b) == 0;
}

// Function to convert a string into an int
static uint16_t atoi_safe(const char *str, bool *success)
{
    char *endptr;
    long val = strtol(str, &endptr, 10);
    *success = (endptr != str && *endptr == '\0');
    return (uint16_t)val;
}

BQ25798_CLI_Status_t BQ25798_CLI_ProcessCmd(BQ25798_CLI_Handle_t *cli, uint8_t *rx_data_buffer)
{
    BQ25798_CLI_Status_t cli_status = BQ25798_CLI_OK;
    BQ25798_Status_t status = BQ25798_OK;

    // Command parsing

    char *cmd = strtok((char *)rx_data_buffer, " \n\r"); // Get the first argument of the command

    if (cmd == NULL)
    {
        cli_status = BQ25798_CLI_ERROR_INVALID_CMD;
    }


    // Process command

    if (cli_status == BQ25798_CLI_OK)
    {
        if (str_equals(cmd, "GET") && cli-> pause == 0)
        {
            char *arg = strtok(NULL, " \n\r"); // Get the second argument of the command

            if (arg == NULL)
            {
                cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
            }

            if (str_equals(arg, "ADC"))
            {
                cli->send_adc_data = 1;
            }
            else if (str_equals(arg, "CONF"))
            {
                cli->send_chg_conf = 1;
            }
            else if (str_equals(arg, "STATUS"))
            {
                cli->send_status = 1;
            }
            else if (str_equals(arg, "FLAGS"))
            {
                cli->send_flags = 1;
            }
            else
            {
                cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
            }
        }

        else if (str_equals(cmd, "ENABLE") && cli-> pause == 0)
        {
            char *arg = strtok(NULL, " \n\r"); // Get the second argument of the command

            if (arg == NULL)
            {
                cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
            }

            if (str_equals(arg, "BACKUP"))
            {
                status = BQ25798_EnableBackupMode(&(cli->bq));

            }
            else if (str_equals(arg, "IDLE"))
            {
               status = BQ25798_EnableIdleMode(&(cli->bq));

            }
            else if (str_equals(arg, "SHIP"))
            {
               status = BQ25798_EnableShipMode(&(cli->bq));

            }
            else if (str_equals(arg, "SHUTDOWN"))
            {
                status = BQ25798_EnableShutdownMode(&(cli->bq));

            }
            else
            {
                cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
            }
        }
        else if (str_equals(cmd, "DISABLE") && cli-> pause == 0)
        {
            char *arg = strtok(NULL, " \n\r"); // Get the second argument of the command

            if (arg == NULL)
            {
                cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
            }

            if (str_equals(arg, "BACKUP"))
            {
                status = BQ25798_ExitBackupMode(&(cli->bq));

            }
        }
        else if (str_equals(cmd, "CLEAR") && cli-> pause == 0)
        {
            char *arg = strtok(NULL, " \n\r"); // Get the second argument of the command

            if (arg == NULL)
            {
                cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
            }

            if (str_equals(arg, "ADC"))
            {
                cli->send_adc_data = 0;
            }
            else if (str_equals(arg, "CONF"))
            {
                cli->send_chg_conf = 0;
            }
            else if (str_equals(arg, "STATUS"))
            {
                cli->send_status = 0;
            }
            else if (str_equals(arg, "FLAGS"))
            {
                cli->send_flags = 0;
            }
            else if (str_equals(arg, "ALL"))
            {
                cli->send_adc_data = 0;
                cli->send_chg_conf = 0;
                cli->send_status = 0;
                cli->send_flags = 0;
            }
            else
            {
                cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
            }
        }

        else if (str_equals(cmd, "SET") && cli-> pause == 0)
        {
            char *arg1 = strtok(NULL, " \n\r"); // Get the first argument
            char *arg2 = strtok(NULL, " \n\r"); // Get the second argument

            if (arg1 == NULL || arg2 == NULL)
            {
                cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
            }

            else
            {
                if (str_equals(arg1, "VREG"))
                {
                    bool success;
                    uint16_t vreg_mv = atoi_safe(arg2, &success);
                    if (!success)
                    {
                        cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
                    }
                    else
                    {
                        status = BQ25798_SetChargeVoltage(&(cli->bq), vreg_mv);

                    }
                }
                else if (str_equals(arg1, "VOTG"))
                {
                    bool success;
                    uint16_t votg_mv = atoi_safe(arg2, &success);
                    if (!success)
                    {
                        cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
                    }
                    else
                    {
                        status = BQ25798_SetOTGVoltage(&(cli->bq), votg_mv);

                    }
                }
                else if (str_equals(arg1, "VSYSMIN"))
                {
                    bool success;
                    uint16_t vsysmin_mv = atoi_safe(arg2, &success);
                    if (!success)
                    {
                        cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
                    }
                    else
                    {
                        status = BQ25798_SetVSYSMIN(&(cli->bq), vsysmin_mv);

                    }
                }
                else if (str_equals(arg1, "VBATLOWV"))
                {
                    bool success;
                    uint8_t vbat_lowv = atoi_safe(arg2, &success);
                    if (!success)
                    {
                        cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
                    }
                    else
                    {
                        status = BQ25798_SetVBAT_LOWV(&(cli->bq), vbat_lowv);

                    }
                }
                else if (str_equals(arg1, "ICHG"))
                {
                    bool success;
                    uint16_t ichg_ma = atoi_safe(arg2, &success);
                    if (!success)
                    {
                        cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
                    }
                    else
                    {
                        status = BQ25798_SetChargeCurrent(&(cli->bq), ichg_ma);

                    }
                }
                else if (str_equals(arg1, "IPCHG"))
                {
                    bool success;
                    uint16_t ipchg_ma = atoi_safe(arg2, &success);
                    if (!success)
                    {
                        cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
                    }
                    else
                    {
                        status = BQ25798_SetPrechargeCurrent(&(cli->bq), ipchg_ma);

                    }
                }
                else if (str_equals(arg1, "IINDPM"))
                {
                    bool success;
                    uint16_t iindpm_ma = atoi_safe(arg2, &success);
                    if (!success)
                    {
                        cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
                    }
                    else
                    {
                        status = BQ25798_SetInputCurrentLimit(&(cli->bq), iindpm_ma);

                    }
                }
                else if (str_equals(arg1, "VINDPM"))
                {
                    bool success;
                    uint16_t vindpm_mv = atoi_safe(arg2, &success);
                    if (!success)
                    {
                        cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
                    }
                    else
                    {
                        status = BQ25798_SetInputVoltageLimit(&(cli->bq), vindpm_mv);

                    }
                }
                else if (str_equals(arg1, "WATCHDOG"))
                {
                    bool success;
                    uint16_t wd_timer = atoi_safe(arg2, &success);
                    if (!success)
                    {
                        cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
                    }
                    else
                    {
                        status = BQ25798_SetWatchdogTimer(&(cli->bq), wd_timer);

                    }
                }
                else
                {
                    cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
                }
            }

        }
        else if (str_equals(cmd, "RESET") && cli-> pause == 0)
        {
            char *arg = strtok(NULL, " \n\r"); // Get the second argument of the command

            if (arg == NULL)
            {
                cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
            }

            if (str_equals(arg, "CHARGER"))
            {
                status = BQ25798_Reset(&(cli->bq));

            }
            else if (str_equals(arg, "CLI"))
            {
                cli_status = BQ25798_CLI_Reset(cli);

            }
            else
            {
                cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
            }
        }
        else if (str_equals(cmd, "HELP") && cli-> pause == 0)
        {
            const char *help_msg =
                "Available commands:\n"
                "\n"
                "  GET ADC (to display ADC data)\n"
                "  GET CONF (to display charge configuration)\n"
                "  GET STATUS (to display charger status)\n"
                "  GET FLAGS (to display charge interrupt flags)\n"
                "\n"
                "  CLEAR ADC (to stop displaying ADC data)\n"
                "  CLEAR CONF (to stop displaying charge configuration)\n"
                "  CLEAR STATUS (to stop displaying charger status)\n"
                "  CLEAR FLAGS (to stop displaying charge interrupt flags)\n"
                "  CLEAR ALL (to not display any data)\n"
                "\n"
                "  SET VREG <value_mV>\n"
                "  SET VSYSMIN <value_mV>\n"
                "  SET VOTG <value_mV>\n"
                "  SET VBATLOWV <hex> (enter decimal value to set the VBAT_LOWV threshold, for instance 01 for 62,2% of VREG)\n"
                "  SET ICHG <value_mA>\n"
                "  SET IPCHG <value_mA>\n"
                "  SET WATCHDOG <hex> (enter decimal value to set the watchdog timer, for instance 01 for 0,5s)\n"
                "\n"
                "  ENABLE BACKUP (to enable backup mode)\n"
                "  ENABLE IDLE (to switch back to idle mode)\n"
                "  ENABLE SHIP (to enter ship mode)\n"
                "  ENABLE SHUTDOWN (to enter shutdown mode)\n"
                "\n"
                "  DISABLE BACKUP (to exit backup mode once power supply is back)\n"
                "\n"
                "  RESET CHARGER (to reset charger to default configuration)\n"
                "  RESET CLI (to reset command line interface configuration)\n"
                "\n"
                "Enter EXIT to quit help menu.\n";

            status = BQ25798_CLI_ClearScreen(cli);

            if ( HAL_UART_Transmit(cli->huart, (uint8_t *)help_msg, strlen(help_msg), HAL_MAX_DELAY) != HAL_OK )
            {
                status = BQ25798_ERROR;
            }

            if (status == BQ25798_OK)
            {
                cli->pause = 1;
            }
        }

        else if (str_equals(cmd, "EXIT") && cli->pause == 1)
        {
            cli->pause = 0;
        }

        else
        {
            cli_status = BQ25798_CLI_ERROR_INVALID_CMD;
        }
    }

    if ( (cli_status == BQ25798_CLI_ERROR_INVALID_ARGS) || (cli_status == BQ25798_CLI_ERROR_INVALID_CMD) )
    {
        cli_status = BQ25798_CLI_OK;
        status = BQ25798_CLI_ClearScreen(cli);
        const char *help_msg = "INVALID COMMAND.\n";

        if ( HAL_UART_Transmit(cli->huart, (uint8_t *)help_msg, strlen(help_msg), HAL_MAX_DELAY) != HAL_OK )
        {
            status = BQ25798_ERROR;
        }

    }

    if (cli_status == BQ25798_CLI_OK && status != BQ25798_OK)
    {
        cli_status = BQ25798_CLI_ERROR_EXECUTION;
    }

    return cli_status;
}

BQ25798_CLI_Status_t BQ25798_CLI_SetDataRefreshPeriod(BQ25798_CLI_Handle_t *cli, uint16_t period_ms)
{
    BQ25798_CLI_Status_t cli_status = BQ25798_CLI_OK;

    if (period_ms == 0)
    {
        cli_status = BQ25798_CLI_ERROR_INVALID_ARGS;
    }
    else
    {
        cli->data_refresh_period = period_ms;
    }

    return cli_status;

}

BQ25798_Status_t BQ25798_CLI_Print(BQ25798_CLI_Handle_t *cli)
{

    BQ25798_Status_t status = BQ25798_OK;

    if (cli->pause == 0)
    {
        status = BQ25798_CLI_ClearScreen(cli);

        if (status == BQ25798_OK && cli->send_adc_data + cli->send_status + cli-> send_chg_conf + cli->send_flags == 0)
        {
            const char *msg = "No data to display \r\n";

            if ( HAL_UART_Transmit(cli->huart, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY) != HAL_OK)
            {
                status = BQ25798_ERROR;
            }
        }

        if (status == BQ25798_OK && cli->send_adc_data == 1)
        {
            status = BQ25798_CLI_SendADCData(cli);
        }

        if (status == BQ25798_OK && cli->send_chg_conf == 1)
        {
            status = BQ25798_CLI_SendChargingConf(cli);
        }

        if (status == BQ25798_OK && cli->send_status == 1)
        {
            status = BQ25798_CLI_SendStatus(cli);
        }

        if (status == BQ25798_OK && cli->send_flags == 1)
        {
            status = BQ25798_CLI_SendFlags(cli);
        }
    }

    return status;
}
