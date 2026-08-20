/**
 * @file    bq40z50r2.c
 * @brief   Implementation of the platform-independent BQ40Z50-R2 driver.
 *
 * @details All bus access goes through the user-supplied transfer callback
 *          stored in the handle, so the same source compiles unchanged on any
 *          MCU.
 *
 */

 /* ==== Includes ==== */
#include "bq40z50r2.h"

/* ==== Block size constants (internal) ==== */

/** @cond INTERNAL */
#define BQ40Z50R2_FWVER_BYTES       (11U) /**< FirmwareVersion payload size.      */
#define BQ40Z50R2_MFGINFO_BYTES     (32U) /**< ManufacturerInfo payload size.     */
#define BQ40Z50R2_MFGINFOB_BYTES    (4U)  /**< ManufacturerInfoB payload size.    */
#define BQ40Z50R2_AFEREG_BYTES      (21U) /**< AFERegister payload size.          */
#define BQ40Z50R2_GAUGESTAT1_BYTES  (32U) /**< GaugeStatus1 payload size.         */
#define BQ40Z50R2_GAUGESTAT2_BYTES  (32U) /**< GaugeStatus2 payload size.         */
#define BQ40Z50R2_GAUGESTAT3_BYTES  (24U) /**< GaugeStatus3 payload size.         */
#define BQ40Z50R2_CBSTATUS_BYTES    (18U) /**< CBStatus payload size.             */
#define BQ40Z50R2_SOH_BYTES         (4U)  /**< State-of-Health payload size.      */
#define BQ40Z50R2_FILTCAP_BYTES     (8U)  /**< FilterCapacity payload size.       */
#define BQ40Z50R2_LDB1_BYTES        (32U) /**< Lifetime Data Block 1 payload size.*/
#define BQ40Z50R2_LDB2_BYTES        (8U)  /**< Lifetime Data Block 2 payload size.*/
#define BQ40Z50R2_LDB3_BYTES        (16U) /**< Lifetime Data Block 3 payload size.*/
#define BQ40Z50R2_LDB4_BYTES        (32U) /**< Lifetime Data Block 4 payload size.*/
#define BQ40Z50R2_LDB5_BYTES        (32U) /**< Lifetime Data Block 5 payload size.*/
/** @endcond */

/* ==== Static functions ==== */

/**
 * @defgroup BQ40Z50R2_FUNC_PVH Private helpers
 * @{
 */

/**
 * @brief Perform a low-level write of @p len bytes from the work buffer.
 *
 * @param[in] handle Initialized handle.
 * @param[in] len    Number of bytes from handle->work_buf to transmit.
 *
 * @return BQ40Z50R2_OK or BQ40Z50R2_COMM_ERROR.
 */
static BQ40Z50R2_Status_t bq_bus_write(BQ40Z50R2_Handle_t *handle, uint16_t len)
{
    BQ40Z50R2_Status_t status;
    int32_t            rc;

    rc = handle->transfer(handle->ctx, handle->dev_addr,
                          BQ40Z50R2_DIR_WRITE, handle->work_buf, len);
    status = (rc == 0) ? BQ40Z50R2_OK : BQ40Z50R2_COMM_ERROR;

    return status;
}

/**
 * @brief Set the device read pointer to @p cmd, then read @p len bytes.
 *
 * @details Implements the SMBus read as a command write followed by a data
 *          read (repeated-start behaviour is provided by the application's
 *          callback / underlying I2C peripheral).
 *
 * @param[in]  handle Initialized handle.
 * @param[in]  cmd    Command/offset byte.
 * @param[out] dst    Destination buffer for the read bytes.
 * @param[in]  len    Number of bytes to read.
 *
 * @return Driver status code.
 */
static BQ40Z50R2_Status_t bq_bus_read(BQ40Z50R2_Handle_t *handle,
                                      uint8_t             cmd,
                                      uint8_t            *dst,
                                      uint16_t            len)
{
    BQ40Z50R2_Status_t status;
    int32_t            rc;

    handle->work_buf[0] = cmd;
    rc = handle->transfer(handle->ctx, handle->dev_addr,
                          BQ40Z50R2_DIR_WRITE, handle->work_buf, 1U);
    if (rc != 0)
    {
        status = BQ40Z50R2_COMM_ERROR;
    }
    else
    {
        rc = handle->transfer(handle->ctx, handle->dev_addr,
                              BQ40Z50R2_DIR_READ, dst, len);
        status = (rc == 0) ? BQ40Z50R2_OK : BQ40Z50R2_COMM_ERROR;
    }

    return status;
}

/**
 * @brief Assemble a little-endian 16-bit value from two bytes.
 * @param[in] lsb Least-significant byte.
 * @param[in] msb Most-significant byte.
 * @return Combined 16-bit value.
 */
static uint16_t bq_le16(uint8_t lsb, uint8_t msb)
{
    return (uint16_t)((uint16_t)lsb | ((uint16_t)msb << 8));
}

/** @} */

/* ==== Functions ==== */

/*==========================================================================*/
/* Initialization                                                           */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_Init(BQ40Z50R2_Handle_t  *handle,
                                  BQ40Z50R2_Transfer_f transfer,
                                  BQ40Z50R2_Delay_f    delay,
                                  void                *ctx,
                                  uint8_t              dev_addr)
{
    BQ40Z50R2_Status_t status;

    if ((handle == NULL) || (transfer == NULL))
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        handle->transfer    = transfer;
        handle->delay       = delay;
        handle->ctx         = ctx;
        handle->dev_addr    = dev_addr;
        handle->initialized = 1U;
        status              = BQ40Z50R2_OK;
    }

    return status;
}

/*==========================================================================*/
/* Low-level SMBus primitives                                               */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_WriteWord(BQ40Z50R2_Handle_t *handle,
                                       uint8_t             cmd,
                                       uint16_t            value)
{
    BQ40Z50R2_Status_t status;

    if (handle == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else if (handle->initialized == 0U)
    {
        status = BQ40Z50R2_NOT_INITIALIZED;
    }
    else
    {
        handle->work_buf[0] = cmd;
        handle->work_buf[1] = (uint8_t)(value & 0xFFU);        /* LSB first. */
        handle->work_buf[2] = (uint8_t)((value >> 8) & 0xFFU); /* MSB.       */
        status = bq_bus_write(handle, 3U);
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_ReadWord(BQ40Z50R2_Handle_t *handle,
                                      uint8_t             cmd,
                                      uint16_t           *value)
{
    BQ40Z50R2_Status_t status;
    uint8_t            rx[2];

    if ((handle == NULL) || (value == NULL))
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else if (handle->initialized == 0U)
    {
        status = BQ40Z50R2_NOT_INITIALIZED;
    }
    else
    {
        status = bq_bus_read(handle, cmd, rx, 2U);
        if (status == BQ40Z50R2_OK)
        {
            *value = bq_le16(rx[0], rx[1]);
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_ReadBlock(BQ40Z50R2_Handle_t *handle,
                                       uint8_t             cmd,
                                       uint8_t            *data,
                                       uint8_t             max_len,
                                       uint8_t            *out_len)
{
    BQ40Z50R2_Status_t status;
    uint8_t            count;
    uint16_t           i;

    if ((handle == NULL) || (data == NULL))
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else if (handle->initialized == 0U)
    {
        status = BQ40Z50R2_NOT_INITIALIZED;
    }
    else
    {
        /* SMBus block read: first byte is the payload length, followed by the
         * payload itself. Read length byte + maximum payload into the private
         * work buffer, then copy out only what fits. */
        status = bq_bus_read(handle, cmd, handle->work_buf,
                             (uint16_t)(BQ40Z50R2_MAX_BLOCK_LEN + 1U));
        if (status == BQ40Z50R2_OK)
        {
            count = handle->work_buf[0];
            if (count > BQ40Z50R2_MAX_BLOCK_LEN)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else if (count > max_len)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                for (i = 0U; i < (uint16_t)count; i++)
                {
                    data[i] = handle->work_buf[i + 1U];
                }
                if (out_len != NULL)
                {
                    *out_len = count;
                }
            }
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_WriteBlock(BQ40Z50R2_Handle_t *handle,
                                        uint8_t             cmd,
                                        const uint8_t      *data,
                                        uint8_t             len)
{
    BQ40Z50R2_Status_t status;
    uint16_t           i;

    if ((handle == NULL) || (data == NULL))
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else if (handle->initialized == 0U)
    {
        status = BQ40Z50R2_NOT_INITIALIZED;
    }
    else if ((len == 0U) || (len > BQ40Z50R2_MAX_BLOCK_LEN))
    {
        status = BQ40Z50R2_SIZE_ERROR;
    }
    else
    {
        /* Frame: command, length byte, payload. */
        handle->work_buf[0] = cmd;
        handle->work_buf[1] = len;
        for (i = 0U; i < (uint16_t)len; i++)
        {
            handle->work_buf[i + 2U] = data[i];
        }
        status = bq_bus_write(handle, (uint16_t)(len + 2U));
    }

    return status;
}

/*==========================================================================*/
/* Manufacturer Access System (MAC) helpers                                 */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_MACCommand(BQ40Z50R2_Handle_t *handle,
                                        uint16_t            subcmd)
{
    return BQ40Z50R2_MACWrite(handle, subcmd, NULL, 0U);
}

BQ40Z50R2_Status_t BQ40Z50R2_MACWrite(BQ40Z50R2_Handle_t *handle,
                                      uint16_t            subcmd,
                                      const uint8_t      *data,
                                      uint8_t             len)
{
    BQ40Z50R2_Status_t status;
    uint16_t           i;

    if (handle == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else if (handle->initialized == 0U)
    {
        status = BQ40Z50R2_NOT_INITIALIZED;
    }
    else if ((len > 0U) && (data == NULL))
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else if (len > BQ40Z50R2_MAX_BLOCK_LEN)
    {
        status = BQ40Z50R2_SIZE_ERROR;
    }
    else
    {
        /* Block-write to 0x44: command, length, subcmd (LE), payload. The
         * length byte counts the 2-byte sub-command plus any payload. */
        handle->work_buf[0] = BQ40Z50R2_CMD_MANUFACTURER_BLOCK_ACCESS;
        handle->work_buf[1] = (uint8_t)(BQ40Z50R2_MAC_CMD_LEN + len);
        handle->work_buf[2] = (uint8_t)(subcmd & 0xFFU);
        handle->work_buf[3] = (uint8_t)((subcmd >> 8) & 0xFFU);
        for (i = 0U; i < (uint16_t)len; i++)
        {
            handle->work_buf[i + 4U] = data[i];
        }
        status = bq_bus_write(handle, (uint16_t)(len + 4U));
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_MACRead(BQ40Z50R2_Handle_t *handle,
                                     uint16_t            subcmd,
                                     uint8_t            *data,
                                     uint8_t             max_len,
                                     uint8_t            *out_len)
{
    BQ40Z50R2_Status_t status;
    uint8_t            blk_len;
    uint16_t           echoed;
    uint8_t            payload_len;
    uint16_t           i;

    if ((handle == NULL) || (data == NULL))
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else if (handle->initialized == 0U)
    {
        status = BQ40Z50R2_NOT_INITIALIZED;
    }
    else
    {
        /* Write the MAC sub-command, then read the block reply. The reply
         * begins with the 2-byte echoed sub-command (little-endian). */
        status = BQ40Z50R2_MACWrite(handle, subcmd, NULL, 0U);
        if (status == BQ40Z50R2_OK)
        {
            status = bq_bus_read(handle,
                                 BQ40Z50R2_CMD_MANUFACTURER_BLOCK_ACCESS,
                                 handle->work_buf,
                                 (uint16_t)(BQ40Z50R2_MAX_BLOCK_LEN + 1U));
        }

        if (status == BQ40Z50R2_OK)
        {
            blk_len = handle->work_buf[0];
            if ((blk_len < BQ40Z50R2_MAC_CMD_LEN) ||
                (blk_len > BQ40Z50R2_MAX_BLOCK_LEN))
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                echoed = bq_le16(handle->work_buf[1], handle->work_buf[2]);
                if (echoed != subcmd)
                {
                    status = BQ40Z50R2_MAC_MISMATCH;
                }
                else
                {
                    payload_len = (uint8_t)(blk_len - BQ40Z50R2_MAC_CMD_LEN);
                    if (payload_len > max_len)
                    {
                        status = BQ40Z50R2_SIZE_ERROR;
                    }
                    else
                    {
                        for (i = 0U; i < (uint16_t)payload_len; i++)
                        {
                            /* +3: skip length byte and 2-byte echoed command. */
                            data[i] = handle->work_buf[i + 3U];
                        }
                        if (out_len != NULL)
                        {
                            *out_len = payload_len;
                        }
                    }
                }
            }
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_MACReadU32(BQ40Z50R2_Handle_t *handle,
                                        uint16_t            subcmd,
                                        uint32_t           *value)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[4];
    uint8_t            n;

    if ((handle == NULL) || (value == NULL))
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, subcmd, buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < 4U)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                *value = (uint32_t)buf[0] |
                         ((uint32_t)buf[1] << 8) |
                         ((uint32_t)buf[2] << 16) |
                         ((uint32_t)buf[3] << 24);
            }
        }
    }

    return status;
}

/*==========================================================================*/
/* High-level measurement helpers                                           */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_GetVoltage(BQ40Z50R2_Handle_t *handle,
                                        uint16_t           *voltage_mv)
{
    return BQ40Z50R2_ReadWord(handle, BQ40Z50R2_CMD_VOLTAGE, voltage_mv);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetCurrent(BQ40Z50R2_Handle_t *handle,
                                        int16_t            *current_ma)
{
    BQ40Z50R2_Status_t status;
    uint16_t           raw;

    if (current_ma == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_ReadWord(handle, BQ40Z50R2_CMD_CURRENT, &raw);
        if (status == BQ40Z50R2_OK)
        {
            *current_ma = (int16_t)raw;
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetAverageCurrent(BQ40Z50R2_Handle_t *handle,
                                               int16_t            *current_ma)
{
    BQ40Z50R2_Status_t status;
    uint16_t           raw;

    if (current_ma == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_ReadWord(handle, BQ40Z50R2_CMD_AVERAGE_CURRENT, &raw);
        if (status == BQ40Z50R2_OK)
        {
            *current_ma = (int16_t)raw;
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetTemperature(BQ40Z50R2_Handle_t *handle,
                                            uint16_t           *temp_dk)
{
    return BQ40Z50R2_ReadWord(handle, BQ40Z50R2_CMD_TEMPERATURE, temp_dk);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetRSOC(BQ40Z50R2_Handle_t *handle,
                                     uint8_t            *rsoc_pct)
{
    BQ40Z50R2_Status_t status;
    uint16_t           raw;

    if (rsoc_pct == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_ReadWord(handle,
                                    BQ40Z50R2_CMD_REL_STATE_OF_CHARGE, &raw);
        if (status == BQ40Z50R2_OK)
        {
            *rsoc_pct = (uint8_t)(raw & 0xFFU);
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetRemainingCapacity(BQ40Z50R2_Handle_t *handle,
                                                  uint16_t           *cap)
{
    return BQ40Z50R2_ReadWord(handle, BQ40Z50R2_CMD_REMAINING_CAPACITY, cap);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetFullChargeCapacity(BQ40Z50R2_Handle_t *handle,
                                                   uint16_t           *cap)
{
    return BQ40Z50R2_ReadWord(handle, BQ40Z50R2_CMD_FULL_CHARGE_CAPACITY, cap);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetCycleCount(BQ40Z50R2_Handle_t *handle,
                                          uint16_t           *cycles)
{
    return BQ40Z50R2_ReadWord(handle, BQ40Z50R2_CMD_CYCLE_COUNT, cycles);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetCellVoltage(BQ40Z50R2_Handle_t *handle,
                                           uint8_t             cell_index,
                                           uint16_t           *voltage_mv)
{
    BQ40Z50R2_Status_t status;
    uint8_t            cmd;

    if (voltage_mv == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        switch (cell_index)
        {
            case 1U:
                cmd    = BQ40Z50R2_CMD_CELL_VOLTAGE_1;
                status = BQ40Z50R2_OK;
                break;
            case 2U:
                cmd    = BQ40Z50R2_CMD_CELL_VOLTAGE_2;
                status = BQ40Z50R2_OK;
                break;
            case 3U:
                cmd    = BQ40Z50R2_CMD_CELL_VOLTAGE_3;
                status = BQ40Z50R2_OK;
                break;
            case 4U:
                cmd    = BQ40Z50R2_CMD_CELL_VOLTAGE_4;
                status = BQ40Z50R2_OK;
                break;
            default:
                cmd    = 0U;
                status = BQ40Z50R2_INVALID_ARG;
                break;
        }

        if (status == BQ40Z50R2_OK)
        {
            status = BQ40Z50R2_ReadWord(handle, cmd, voltage_mv);
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetBatteryStatus(BQ40Z50R2_Handle_t *handle,
                                             uint16_t           *status_out)
{
    return BQ40Z50R2_ReadWord(handle, BQ40Z50R2_CMD_BATTERY_STATUS, status_out);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetDAStatus1(BQ40Z50R2_Handle_t    *handle,
                                         BQ40Z50R2_DAStatus1_t  *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_DASTATUS1_BYTES];
    uint8_t            n;
    uint8_t            i;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_DA_STATUS1,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_DASTATUS1_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* 16 little-endian words in order:
                 * cell V1..4, BAT V, PACK V, cell I1..4, cell P1..4,
                 * power, average power. */
                for (i = 0U; i < 4U; i++)
                {
                    out->cell_voltage_mv[i] =
                        bq_le16(buf[i * 2U], buf[(i * 2U) + 1U]);
                }
                out->bat_voltage_mv  = bq_le16(buf[8],  buf[9]);
                out->pack_voltage_mv = bq_le16(buf[10], buf[11]);
                for (i = 0U; i < 4U; i++)
                {
                    out->cell_current_ma[i] =
                        (int16_t)bq_le16(buf[12U + (i * 2U)],
                                         buf[13U + (i * 2U)]);
                }
                for (i = 0U; i < 4U; i++)
                {
                    out->cell_power[i] =
                        (int16_t)bq_le16(buf[20U + (i * 2U)],
                                         buf[21U + (i * 2U)]);
                }
                out->power_cw     = (int16_t)bq_le16(buf[28], buf[29]);
                out->avg_power_cw = (int16_t)bq_le16(buf[30], buf[31]);
            }
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetDAStatus2(BQ40Z50R2_Handle_t    *handle,
                                         BQ40Z50R2_DAStatus2_t  *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_DASTATUS2_BYTES];
    uint8_t            n;
    uint8_t            i;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_DA_STATUS2,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_DASTATUS2_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* 8 little-endian words: Int, TS1..TS4, Cell, FET, Gauge. */
                out->int_temp_dk = bq_le16(buf[0], buf[1]);
                for (i = 0U; i < 4U; i++)
                {
                    out->ts_temp_dk[i] =
                        bq_le16(buf[2U + (i * 2U)], buf[3U + (i * 2U)]);
                }
                out->cell_temp_dk  = bq_le16(buf[10], buf[11]);
                out->fet_temp_dk   = bq_le16(buf[12], buf[13]);
                out->gauge_temp_dk = bq_le16(buf[14], buf[15]);
            }
        }
    }

    return status;
}

/*==========================================================================*/
/* High-level status helpers                                                */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_GetSafetyStatus(BQ40Z50R2_Handle_t *handle,
                                             uint32_t           *status_out)
{
    return BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_SAFETY_STATUS, status_out);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetOperationStatus(BQ40Z50R2_Handle_t *handle,
                                               uint32_t           *status_out)
{
    return BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_OPERATION_STATUS,
                                status_out);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetChargingStatus(BQ40Z50R2_Handle_t *handle,
                                              uint32_t           *status_out)
{
    return BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_CHARGING_STATUS,
                                status_out);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetGaugingStatus(BQ40Z50R2_Handle_t *handle,
                                             uint32_t           *status_out)
{
    return BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_GAUGING_STATUS,
                                status_out);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetManufacturingStatus(BQ40Z50R2_Handle_t *handle,
                                                   uint16_t           *status_out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[2];
    uint8_t            n;

    if (status_out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_MANUFACTURING_STATUS,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < 2U)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                *status_out = bq_le16(buf[0], buf[1]);
            }
        }
    }

    return status;
}

uint8_t BQ40Z50R2_GetSecurityMode(uint32_t operation_status)
{
    return (uint8_t)((operation_status & BQ40Z50R2_OPSTAT_SEC_MASK)
                     >> BQ40Z50R2_OPSTAT_SEC_SHIFT);
}

/*==========================================================================*/
/* Device identification                                                    */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_GetDeviceType(BQ40Z50R2_Handle_t *handle,
                                          uint16_t           *device_type)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[2];
    uint8_t            n;

    if (device_type == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_DEVICE_TYPE,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < 2U)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                *device_type = bq_le16(buf[0], buf[1]);
            }
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetChemID(BQ40Z50R2_Handle_t *handle,
                                      uint16_t           *chem_id)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[2];
    uint8_t            n;

    if (chem_id == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_CHEM_ID,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < 2U)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                *chem_id = bq_le16(buf[0], buf[1]);
            }
        }
    }

    return status;
}

/*==========================================================================*/
/* Control / actuation helpers                                              */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_DeviceReset(BQ40Z50R2_Handle_t *handle)
{
    return BQ40Z50R2_MACCommand(handle, BQ40Z50R2_MAC_DEVICE_RESET);
}

BQ40Z50R2_Status_t BQ40Z50R2_ShutdownMode(BQ40Z50R2_Handle_t *handle)
{
    return BQ40Z50R2_MACCommand(handle, BQ40Z50R2_MAC_SHUTDOWN_MODE);
}

BQ40Z50R2_Status_t BQ40Z50R2_SleepMode(BQ40Z50R2_Handle_t *handle)
{
    return BQ40Z50R2_MACCommand(handle, BQ40Z50R2_MAC_SLEEP_MODE);
}

BQ40Z50R2_Status_t BQ40Z50R2_ToggleChargeFET(BQ40Z50R2_Handle_t *handle)
{
    return BQ40Z50R2_MACCommand(handle, BQ40Z50R2_MAC_CHG_FET_TOGGLE);
}

BQ40Z50R2_Status_t BQ40Z50R2_ToggleDischargeFET(BQ40Z50R2_Handle_t *handle)
{
    return BQ40Z50R2_MACCommand(handle, BQ40Z50R2_MAC_DSG_FET_TOGGLE);
}

BQ40Z50R2_Status_t BQ40Z50R2_TogglePrechargeFET(BQ40Z50R2_Handle_t *handle)
{
    return BQ40Z50R2_MACCommand(handle, BQ40Z50R2_MAC_PCHG_FET_TOGGLE);
}

BQ40Z50R2_Status_t BQ40Z50R2_ToggleFETControl(BQ40Z50R2_Handle_t *handle)
{
    return BQ40Z50R2_MACCommand(handle, BQ40Z50R2_MAC_FET_CONTROL);
}

BQ40Z50R2_Status_t BQ40Z50R2_ToggleGauging(BQ40Z50R2_Handle_t *handle)
{
    return BQ40Z50R2_MACCommand(handle, BQ40Z50R2_MAC_GAUGING);
}

BQ40Z50R2_Status_t BQ40Z50R2_ToggleFuse(BQ40Z50R2_Handle_t *handle)
{
    return BQ40Z50R2_MACCommand(handle, BQ40Z50R2_MAC_FUSE_TOGGLE);
}

BQ40Z50R2_Status_t BQ40Z50R2_Seal(BQ40Z50R2_Handle_t *handle)
{
    return BQ40Z50R2_MACCommand(handle, BQ40Z50R2_MAC_SEAL_DEVICE);
}

BQ40Z50R2_Status_t BQ40Z50R2_Unseal(BQ40Z50R2_Handle_t *handle,
                                    uint16_t            key1,
                                    uint16_t            key2)
{
    BQ40Z50R2_Status_t status;

    /* The UNSEAL keys are sent as two consecutive write-word transactions to
     * ManufacturerAccess() (0x00). Data written to 0x00 is NOT little-endian
     * swapped by the device, so the words are written as-is (TRM 14.1). */
    status = BQ40Z50R2_WriteWord(handle, BQ40Z50R2_CMD_MANUFACTURER_ACCESS,
                                 key1);
    if (status == BQ40Z50R2_OK)
    {
        status = BQ40Z50R2_WriteWord(handle, BQ40Z50R2_CMD_MANUFACTURER_ACCESS,
                                     key2);
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_FullAccess(BQ40Z50R2_Handle_t *handle,
                                        uint16_t            key1,
                                        uint16_t            key2)
{
    BQ40Z50R2_Status_t status;

    status = BQ40Z50R2_WriteWord(handle, BQ40Z50R2_CMD_MANUFACTURER_ACCESS,
                                 key1);
    if (status == BQ40Z50R2_OK)
    {
        status = BQ40Z50R2_WriteWord(handle, BQ40Z50R2_CMD_MANUFACTURER_ACCESS,
                                     key2);
    }

    return status;
}

/*==========================================================================*/
/* Private helper: decode a 32-bit status word into individual flag fields  */
/*==========================================================================*/

/**
 * @brief Decode a raw 32-bit SafetyAlert or SafetyStatus word into the
 *        flag fields of a @ref BQ40Z50R2_SafetyStatus_t.
 *
 * @note  SafetyAlert carries two extra bits (CTOS, PTOS) that do not exist
 *        in SafetyStatus; they will be zero when decoding a SafetyStatus word.
 *        Use the @ref BQ40Z50R2_SafetyAlert_t overload for SafetyAlert.
 */
static void bq_decode_safety_status(uint32_t                  raw,
                                    BQ40Z50R2_SafetyStatus_t *out)
{
    out->ocdl  = (uint8_t)((raw >> 29) & 1U);
    out->covl  = (uint8_t)((raw >> 28) & 1U);
    out->utd   = (uint8_t)((raw >> 27) & 1U);
    out->utc   = (uint8_t)((raw >> 26) & 1U);
    out->pchgc = (uint8_t)((raw >> 25) & 1U);
    out->chgv  = (uint8_t)((raw >> 24) & 1U);
    out->chgc  = (uint8_t)((raw >> 23) & 1U);
    out->oc    = (uint8_t)((raw >> 22) & 1U);
    out->cto   = (uint8_t)((raw >> 20) & 1U);
    out->pto   = (uint8_t)((raw >> 18) & 1U);
    out->otf   = (uint8_t)((raw >> 16) & 1U);
    out->cuvc  = (uint8_t)((raw >> 14) & 1U);
    out->otd   = (uint8_t)((raw >> 13) & 1U);
    out->otc   = (uint8_t)((raw >> 12) & 1U);
    out->ascdl = (uint8_t)((raw >> 11) & 1U);
    out->ascd  = (uint8_t)((raw >> 10) & 1U);
    out->asccl = (uint8_t)((raw >>  9) & 1U);
    out->ascc  = (uint8_t)((raw >>  8) & 1U);
    out->aoldl = (uint8_t)((raw >>  7) & 1U);
    out->aold  = (uint8_t)((raw >>  6) & 1U);
    out->ocd2  = (uint8_t)((raw >>  5) & 1U);
    out->ocd1  = (uint8_t)((raw >>  4) & 1U);
    out->occ2  = (uint8_t)((raw >>  3) & 1U);
    out->occ1  = (uint8_t)((raw >>  2) & 1U);
    out->cov   = (uint8_t)((raw >>  1) & 1U);
    out->cuv   = (uint8_t)((raw >>  0) & 1U);
}

/** @brief Decode a raw 32-bit PFAlert/PFStatus word into named fields. */
static void bq_decode_pf_status(uint32_t raw, BQ40Z50R2_PFStatus_t *out)
{
    out->ts4   = (uint8_t)((raw >> 31) & 1U);
    out->ts3   = (uint8_t)((raw >> 30) & 1U);
    out->ts2   = (uint8_t)((raw >> 29) & 1U);
    out->ts1   = (uint8_t)((raw >> 28) & 1U);
    out->lvl2  = (uint8_t)((raw >> 22) & 1U);
    out->afec  = (uint8_t)((raw >> 21) & 1U);
    out->afer  = (uint8_t)((raw >> 20) & 1U);
    out->fuse  = (uint8_t)((raw >> 19) & 1U);
    out->ocdl  = (uint8_t)((raw >> 18) & 1U);
    out->dfetf = (uint8_t)((raw >> 17) & 1U);
    out->cfetf = (uint8_t)((raw >> 16) & 1U);
    out->ascdl = (uint8_t)((raw >> 15) & 1U);
    out->asccl = (uint8_t)((raw >> 14) & 1U);
    out->aoldl = (uint8_t)((raw >> 13) & 1U);
    out->vima  = (uint8_t)((raw >> 12) & 1U);
    out->vimr  = (uint8_t)((raw >> 11) & 1U);
    out->cd    = (uint8_t)((raw >> 10) & 1U);
    out->imp   = (uint8_t)((raw >>  9) & 1U);
    out->cb    = (uint8_t)((raw >>  8) & 1U);
    out->qim   = (uint8_t)((raw >>  7) & 1U);
    out->sotf  = (uint8_t)((raw >>  6) & 1U);
    out->covl  = (uint8_t)((raw >>  5) & 1U);
    out->sot   = (uint8_t)((raw >>  4) & 1U);
    out->socd  = (uint8_t)((raw >>  3) & 1U);
    out->socc  = (uint8_t)((raw >>  2) & 1U);
    out->sov   = (uint8_t)((raw >>  1) & 1U);
    out->suv   = (uint8_t)((raw >>  0) & 1U);
}

/*==========================================================================*/
/* Device identification — FirmwareVersion                                  */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_GetFirmwareVersion(
        BQ40Z50R2_Handle_t          *handle,
        BQ40Z50R2_FirmwareVersion_t  *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_FWVER_BYTES];
    uint8_t            n;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_FIRMWARE_VERSION,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_FWVER_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* Format: DDddVVvvBBbbTTZZzzRREE */
                out->device_number = bq_le16(buf[0], buf[1]);
                out->version       = bq_le16(buf[2], buf[3]);
                out->build_number  = bq_le16(buf[4], buf[5]);
                out->fw_type       = buf[6];
                out->it_version    = bq_le16(buf[7], buf[8]);
                out->reserved[0]   = buf[9];
                out->reserved[1]   = buf[10];
            }
        }
    }

    return status;
}

/*==========================================================================*/
/* Manufacturer info blocks                                                 */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_GetManufacturerInfo(
        BQ40Z50R2_Handle_t           *handle,
        BQ40Z50R2_ManufacturerInfo_t  *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            n;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_MANUFACTURER_INFO,
                                   out->data, BQ40Z50R2_MFGINFO_BYTES, &n);
        if ((status == BQ40Z50R2_OK) && (n < BQ40Z50R2_MFGINFO_BYTES))
        {
            status = BQ40Z50R2_SIZE_ERROR;
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetManufacturerInfoB(
        BQ40Z50R2_Handle_t            *handle,
        BQ40Z50R2_ManufacturerInfoB_t  *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            n;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_MANUFACTURER_INFO_B,
                                   out->data, BQ40Z50R2_MFGINFOB_BYTES, &n);
        if ((status == BQ40Z50R2_OK) && (n < BQ40Z50R2_MFGINFOB_BYTES))
        {
            status = BQ40Z50R2_SIZE_ERROR;
        }
    }

    return status;
}

/*==========================================================================*/
/* AFE register snapshot                                                    */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_GetAFERegister(
        BQ40Z50R2_Handle_t    *handle,
        BQ40Z50R2_AFERegister_t *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_AFEREG_BYTES];
    uint8_t            n;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_AFE_REGISTER,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_AFEREG_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* Each byte maps directly to the corresponding AFE register
                 * in the order AA..UU (TRM section 14.1.44). */
                out->int_status   = buf[0];
                out->fet_status   = buf[1];
                out->rxin         = buf[2];
                out->latch_status = buf[3];
                out->int_enable   = buf[4];
                out->control      = buf[5];
                out->rxien        = buf[6];
                out->rlout        = buf[7];
                out->rhout        = buf[8];
                out->rhint        = buf[9];
                out->cell_bal     = buf[10];
                out->adc_cc       = buf[11];
                out->adc_mux      = buf[12];
                out->led          = buf[13];
                out->hw_ctrl      = buf[14];
                out->timer        = buf[15];
                out->protection   = buf[16];
                out->ocd          = buf[17];
                out->scc          = buf[18];
                out->scd1         = buf[19];
                out->scd2         = buf[20];
            }
        }
    }

    return status;
}

/*==========================================================================*/
/* Safety registers                                                         */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_GetSafetyAlert(BQ40Z50R2_Handle_t *handle,
                                             uint32_t           *alert)
{
    return BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_SAFETY_ALERT, alert);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetSafetyAlertDecoded(
        BQ40Z50R2_Handle_t     *handle,
        BQ40Z50R2_SafetyAlert_t *out)
{
    BQ40Z50R2_Status_t status;
    uint32_t           raw;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_SAFETY_ALERT,
                                      &raw);
        if (status == BQ40Z50R2_OK)
        {
            /* SafetyAlert has two extra bits vs SafetyStatus: CTOS (21) and
             * PTOS (19). Decode the shared fields first via the common helper,
             * then overlay the Alert-only bits. */
            bq_decode_safety_status(raw,
                                    (BQ40Z50R2_SafetyStatus_t *)(void *)out);
            out->ctos = (uint8_t)((raw >> 21) & 1U);
            out->ptos = (uint8_t)((raw >> 19) & 1U);
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetSafetyStatusDecoded(
        BQ40Z50R2_Handle_t      *handle,
        BQ40Z50R2_SafetyStatus_t *out)
{
    BQ40Z50R2_Status_t status;
    uint32_t           raw;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_SAFETY_STATUS,
                                      &raw);
        if (status == BQ40Z50R2_OK)
        {
            bq_decode_safety_status(raw, out);
        }
    }

    return status;
}

/*==========================================================================*/
/* Permanent Failure registers                                              */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_GetPFAlert(BQ40Z50R2_Handle_t *handle,
                                         uint32_t           *alert)
{
    return BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_PF_ALERT, alert);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetPFAlertDecoded(BQ40Z50R2_Handle_t  *handle,
                                                BQ40Z50R2_PFStatus_t *out)
{
    BQ40Z50R2_Status_t status;
    uint32_t           raw;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_PF_ALERT, &raw);
        if (status == BQ40Z50R2_OK)
        {
            bq_decode_pf_status(raw, out);
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetPFStatus(BQ40Z50R2_Handle_t *handle,
                                          uint32_t           *status_out)
{
    return BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_PF_STATUS, status_out);
}

BQ40Z50R2_Status_t BQ40Z50R2_GetPFStatusDecoded(BQ40Z50R2_Handle_t  *handle,
                                                 BQ40Z50R2_PFStatus_t *out)
{
    BQ40Z50R2_Status_t status;
    uint32_t           raw;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_PF_STATUS, &raw);
        if (status == BQ40Z50R2_OK)
        {
            bq_decode_pf_status(raw, out);
        }
    }

    return status;
}

/*==========================================================================*/
/* Operation / Charging / Gauging / Manufacturing status — decoded          */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_GetOperationStatusDecoded(
        BQ40Z50R2_Handle_t          *handle,
        BQ40Z50R2_OperationStatus_t  *out)
{
    BQ40Z50R2_Status_t status;
    uint32_t           raw;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_OPERATION_STATUS,
                                      &raw);
        if (status == BQ40Z50R2_OK)
        {
            out->emshut     = (uint8_t)((raw >> 29) & 1U);
            out->cb         = (uint8_t)((raw >> 28) & 1U);
            out->slpcc      = (uint8_t)((raw >> 27) & 1U);
            out->slpad      = (uint8_t)((raw >> 26) & 1U);
            out->smblcal    = (uint8_t)((raw >> 25) & 1U);
            out->init       = (uint8_t)((raw >> 24) & 1U);
            out->sleepm     = (uint8_t)((raw >> 23) & 1U);
            out->xl         = (uint8_t)((raw >> 22) & 1U);
            out->cal_offset = (uint8_t)((raw >> 21) & 1U);
            out->cal        = (uint8_t)((raw >> 20) & 1U);
            out->autocalm   = (uint8_t)((raw >> 19) & 1U);
            out->auth       = (uint8_t)((raw >> 18) & 1U);
            out->led        = (uint8_t)((raw >> 17) & 1U);
            out->sdm        = (uint8_t)((raw >> 16) & 1U);
            out->sleep      = (uint8_t)((raw >> 15) & 1U);
            out->xchg       = (uint8_t)((raw >> 14) & 1U);
            out->xdsg       = (uint8_t)((raw >> 13) & 1U);
            out->pf         = (uint8_t)((raw >> 12) & 1U);
            out->ss         = (uint8_t)((raw >> 11) & 1U);
            out->sdv        = (uint8_t)((raw >> 10) & 1U);
            out->sec        = (uint8_t)((raw >>  8) & 0x3U);
            out->btp_int    = (uint8_t)((raw >>  7) & 1U);
            out->fuse       = (uint8_t)((raw >>  5) & 1U);
            out->pchg       = (uint8_t)((raw >>  3) & 1U);
            out->chg        = (uint8_t)((raw >>  2) & 1U);
            out->dsg        = (uint8_t)((raw >>  1) & 1U);
            out->pres       = (uint8_t)((raw >>  0) & 1U);
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetChargingStatusDecoded(
        BQ40Z50R2_Handle_t         *handle,
        BQ40Z50R2_ChargingStatus_t  *out)
{
    BQ40Z50R2_Status_t status;
    uint32_t           raw;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_CHARGING_STATUS,
                                      &raw);
        if (status == BQ40Z50R2_OK)
        {
            out->nct  = (uint8_t)((raw >> 19) & 1U);
            out->ccc  = (uint8_t)((raw >> 18) & 1U);
            out->cvr  = (uint8_t)((raw >> 17) & 1U);
            out->ccr  = (uint8_t)((raw >> 16) & 1U);
            out->vct  = (uint8_t)((raw >> 15) & 1U);
            out->mchg = (uint8_t)((raw >> 14) & 1U);
            out->su   = (uint8_t)((raw >> 13) & 1U);
            out->in   = (uint8_t)((raw >> 12) & 1U);
            out->hv   = (uint8_t)((raw >> 11) & 1U);
            out->mv   = (uint8_t)((raw >> 10) & 1U);
            out->lv   = (uint8_t)((raw >>  9) & 1U);
            out->pv   = (uint8_t)((raw >>  8) & 1U);
            out->ot   = (uint8_t)((raw >>  6) & 1U);
            out->ht   = (uint8_t)((raw >>  5) & 1U);
            out->sth  = (uint8_t)((raw >>  4) & 1U);
            out->rt   = (uint8_t)((raw >>  3) & 1U);
            out->stl  = (uint8_t)((raw >>  2) & 1U);
            out->lt   = (uint8_t)((raw >>  1) & 1U);
            out->ut   = (uint8_t)((raw >>  0) & 1U);
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetGaugingStatusDecoded(
        BQ40Z50R2_Handle_t         *handle,
        BQ40Z50R2_GaugingStatus_t   *out)
{
    BQ40Z50R2_Status_t status;
    uint32_t           raw;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACReadU32(handle, BQ40Z50R2_MAC_GAUGING_STATUS,
                                      &raw);
        if (status == BQ40Z50R2_OK)
        {
            out->ocvfr   = (uint8_t)((raw >> 20) & 1U);
            out->ldmd    = (uint8_t)((raw >> 19) & 1U);
            out->rx      = (uint8_t)((raw >> 18) & 1U);
            out->qmax    = (uint8_t)((raw >> 17) & 1U);
            out->vdq     = (uint8_t)((raw >> 16) & 1U);
            out->nsfm    = (uint8_t)((raw >> 15) & 1U);
            out->slpqmax = (uint8_t)((raw >> 13) & 1U);
            out->qen     = (uint8_t)((raw >> 12) & 1U);
            out->vok     = (uint8_t)((raw >> 11) & 1U);
            out->r_dis   = (uint8_t)((raw >> 10) & 1U);
            out->rest    = (uint8_t)((raw >>  8) & 1U);
            out->cf      = (uint8_t)((raw >>  7) & 1U);
            out->dsg     = (uint8_t)((raw >>  6) & 1U);
            out->edv     = (uint8_t)((raw >>  5) & 1U);
            out->bal_en  = (uint8_t)((raw >>  4) & 1U);
            out->tc      = (uint8_t)((raw >>  3) & 1U);
            out->td      = (uint8_t)((raw >>  2) & 1U);
            out->fc      = (uint8_t)((raw >>  1) & 1U);
            out->fd      = (uint8_t)((raw >>  0) & 1U);
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetManufacturingStatusDecoded(
        BQ40Z50R2_Handle_t               *handle,
        BQ40Z50R2_ManufacturingStatus_t   *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[2];
    uint8_t            n;
    uint16_t           raw;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_MANUFACTURING_STATUS,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < 2U)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                raw = bq_le16(buf[0], buf[1]);
                out->cal_test = (uint8_t)((raw >> 15) & 1U);
                out->lt_test  = (uint8_t)((raw >> 14) & 1U);
                out->led_en   = (uint8_t)((raw >>  9) & 1U);
                out->fuse_en  = (uint8_t)((raw >>  8) & 1U);
                out->bbr_en   = (uint8_t)((raw >>  7) & 1U);
                out->pf_en    = (uint8_t)((raw >>  6) & 1U);
                out->lf_en    = (uint8_t)((raw >>  5) & 1U);
                out->fet_en   = (uint8_t)((raw >>  4) & 1U);
                out->gauge_en = (uint8_t)((raw >>  3) & 1U);
                out->dsg_en   = (uint8_t)((raw >>  2) & 1U);
                out->chg_en   = (uint8_t)((raw >>  1) & 1U);
                out->pchg_en  = (uint8_t)((raw >>  0) & 1U);
            }
        }
    }

    return status;
}

/*==========================================================================*/
/* GaugeStatus1 / 2 / 3                                                     */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_GetGaugeStatus1(
        BQ40Z50R2_Handle_t       *handle,
        BQ40Z50R2_GaugeStatus1_t  *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_GAUGESTAT1_BYTES];
    uint8_t            n;
    uint8_t            i;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_GAUGE_STATUS1,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_GAUGESTAT1_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* Format (TRM 14.1.54), all little-endian int16 / uint16:
                 * [0..1]  AAaa True Rem Q (mAh)
                 * [2..3]  BBbb True Rem E (cWh)
                 * [4..5]  CCcc Initial Q  (mAh)
                 * [6..7]  DDdd Initial E  (cWh)
                 * [8..9]  EEee True FCC Q (mAh)
                 * [10..11]FFff True FCC E (cWh)
                 * [12..13]GGgg T_sim      (0.1 K)
                 * [14..15]HHhh T_ambient  (0.1 K)
                 * [16..17]IIii RaScale[0]
                 * [18..19]JJjj RaScale[1]
                 * [20..21]KKkk RaScale[2]
                 * [22..23]LLll RaScale[3]
                 * [24..25]MMmm CompRes[0] (2^-10 Ω)
                 * [26..27]NNnn CompRes[1]
                 * [28..29]OOoo CompRes[2]
                 * [30..31]PPpp CompRes[3]
                 */
                out->true_rem_q_mah  = (int16_t)bq_le16(buf[0],  buf[1]);
                out->true_rem_e_cwh  = (int16_t)bq_le16(buf[2],  buf[3]);
                out->initial_q_mah   = (int16_t)bq_le16(buf[4],  buf[5]);
                out->initial_e_cwh   = (int16_t)bq_le16(buf[6],  buf[7]);
                out->true_fcc_q_mah  = (int16_t)bq_le16(buf[8],  buf[9]);
                out->true_fcc_e_cwh  = (int16_t)bq_le16(buf[10], buf[11]);
                out->t_sim_dk        = bq_le16(buf[12], buf[13]);
                out->t_ambient_dk    = bq_le16(buf[14], buf[15]);
                for (i = 0U; i < 4U; i++)
                {
                    out->ra_scale[i] =
                        bq_le16(buf[16U + (i * 2U)], buf[17U + (i * 2U)]);
                }
                for (i = 0U; i < 4U; i++)
                {
                    out->comp_res[i] =
                        bq_le16(buf[24U + (i * 2U)], buf[25U + (i * 2U)]);
                }
            }
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetGaugeStatus2(
        BQ40Z50R2_Handle_t       *handle,
        BQ40Z50R2_GaugeStatus2_t  *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_GAUGESTAT2_BYTES];
    uint8_t            n;
    uint8_t            i;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_GAUGE_STATUS2,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_GAUGESTAT2_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* Format (TRM 14.1.55):
                 * [0]     AA   Pack Grid
                 * [1]     BB   LStatus
                 * [2..5]  CC..FF Cell Grid 0..3 (1 byte each)
                 * [6..9]  GGggHHhh State Time (32-bit LE, s)
                 * [10..11]IIii DOD0[0]
                 * [12..13]JJjj DOD0[1]
                 * [14..15]KKkk DOD0[2]
                 * [16..17]LLll DOD0[3]
                 * [18..19]MMmm DOD0 Passed Q (mAh, signed)
                 * [20..21]NNnn DOD0 Passed E (cWh, signed)
                 * [22..23]OOoo DOD0 Time (hr/16)
                 * [24..25]PPpp DODEOC[0]
                 * [26..27]QQqq DODEOC[1]
                 * [28..29]RRrr DODEOC[2]
                 * [30..31]SSss DODEOC[3]
                 */
                out->pack_grid = buf[0];
                out->l_status  = buf[1];
                for (i = 0U; i < 4U; i++)
                {
                    out->cell_grid[i] = buf[2U + i];
                }
                out->state_time_s = (uint32_t)buf[6]        |
                                    ((uint32_t)buf[7] << 8)  |
                                    ((uint32_t)buf[8] << 16) |
                                    ((uint32_t)buf[9] << 24);
                for (i = 0U; i < 4U; i++)
                {
                    out->dod0[i] =
                        bq_le16(buf[10U + (i * 2U)], buf[11U + (i * 2U)]);
                }
                out->dod0_passed_q_mah =
                    (int16_t)bq_le16(buf[18], buf[19]);
                out->dod0_passed_e_cwh =
                    (int16_t)bq_le16(buf[20], buf[21]);
                out->dod0_time_hr16 = bq_le16(buf[22], buf[23]);
                for (i = 0U; i < 4U; i++)
                {
                    out->dodeoc[i] =
                        bq_le16(buf[24U + (i * 2U)], buf[25U + (i * 2U)]);
                }
            }
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetGaugeStatus3(
        BQ40Z50R2_Handle_t       *handle,
        BQ40Z50R2_GaugeStatus3_t  *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_GAUGESTAT3_BYTES];
    uint8_t            n;
    uint8_t            i;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_GAUGE_STATUS3,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_GAUGESTAT3_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* Format (TRM 14.1.56):
                 * [0..1]  AAaa QMax[0] (mAh)
                 * [2..3]  BBbb QMax[1]
                 * [4..5]  CCcc QMax[2]
                 * [6..7]  DDdd QMax[3]
                 * [8..9]  EEee QMax DOD0[0]
                 * [10..11]FFff QMax DOD0[1]
                 * [12..13]GGgg QMax DOD0[2]
                 * [14..15]HHhh QMax DOD0[3]
                 * [16..17]IIii QMax Passed Q (mAh, signed)
                 * [18..19]JJjj QMax Time (hr/16)
                 * [20..21]KKkk Temp k
                 * [22..23]LLll Temp a
                 */
                for (i = 0U; i < 4U; i++)
                {
                    out->qmax_mah[i] =
                        bq_le16(buf[i * 2U], buf[(i * 2U) + 1U]);
                }
                for (i = 0U; i < 4U; i++)
                {
                    out->qmax_dod0[i] =
                        bq_le16(buf[8U + (i * 2U)], buf[9U + (i * 2U)]);
                }
                out->qmax_passed_q_mah = (int16_t)bq_le16(buf[16], buf[17]);
                out->qmax_time_hr16    = bq_le16(buf[18], buf[19]);
                out->temp_k            = bq_le16(buf[20], buf[21]);
                out->temp_a            = bq_le16(buf[22], buf[23]);
            }
        }
    }

    return status;
}

/*==========================================================================*/
/* CBStatus / State-of-Health / FilterCapacity                              */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_GetCBStatus(
        BQ40Z50R2_Handle_t   *handle,
        BQ40Z50R2_CBStatus_t  *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_CBSTATUS_BYTES];
    uint8_t            n;
    uint8_t            i;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_CB_STATUS,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_CBSTATUS_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* Format (TRM 14.1.57):
                 * [0..1]  AAaa CB time cell 0 (s)
                 * [2..3]  BBbb CB time cell 1 (s)
                 * [4..5]  CCcc CB time cell 2 (s)
                 * [6..7]  DDdd CB time cell 3 (s)
                 * [8..9]  EEee CB DOD cell 0
                 * [10..11]FFff CB DOD cell 1
                 * [12..13]GGgg CB DOD cell 2
                 * [14..15]HHhh CB DOD cell 3
                 * [16..17]IIii Total DOD charge
                 */
                for (i = 0U; i < 4U; i++)
                {
                    out->cb_time_s[i] =
                        bq_le16(buf[i * 2U], buf[(i * 2U) + 1U]);
                }
                for (i = 0U; i < 4U; i++)
                {
                    out->cb_dod[i] =
                        bq_le16(buf[8U + (i * 2U)], buf[9U + (i * 2U)]);
                }
                out->total_dod_chg = bq_le16(buf[16], buf[17]);
            }
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetStateOfHealth(
        BQ40Z50R2_Handle_t       *handle,
        BQ40Z50R2_StateOfHealth_t *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_SOH_BYTES];
    uint8_t            n;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_STATE_OF_HEALTH,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_SOH_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                out->soh_fcc_mah    = bq_le16(buf[0], buf[1]);
                out->soh_energy_cwh = bq_le16(buf[2], buf[3]);
            }
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetFilterCapacity(
        BQ40Z50R2_Handle_t        *handle,
        BQ40Z50R2_FilterCapacity_t *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_FILTCAP_BYTES];
    uint8_t            n;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_FILTER_CAPACITY,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_FILTCAP_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                out->filtered_rem_mah  = bq_le16(buf[0], buf[1]);
                out->filtered_rem_cwh  = bq_le16(buf[2], buf[3]);
                out->filtered_fcc_mah  = bq_le16(buf[4], buf[5]);
                out->filtered_fcc_cwh  = bq_le16(buf[6], buf[7]);
            }
        }
    }

    return status;
}

/*==========================================================================*/
/* Lifetime Data Blocks 1–5                                                 */
/*==========================================================================*/

BQ40Z50R2_Status_t BQ40Z50R2_GetLifetimeDataBlock1(
        BQ40Z50R2_Handle_t              *handle,
        BQ40Z50R2_LifetimeDataBlock1_t   *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_LDB1_BYTES];
    uint8_t            n;
    uint8_t            i;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_LIFETIME_DATA_BLOCK1,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_LDB1_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* Format (TRM 14.1.46):
                 * [0..7]   cell_max_mv[0..3]       (4 x int16, mV)
                 * [8..15]  cell_min_mv[0..3]        (4 x int16, mV)
                 * [16..17] max_delta_cell_mv        (int16, mV)
                 * [18..19] max_chg_curr_ma          (int16, mA)
                 * [20..21] max_dsg_curr_ma          (int16, mA)
                 * [22..23] max_avg_dsg_curr_ma      (int16, mA)
                 * [24..25] max_avg_dsg_pwr_cw       (int16, cW)
                 * [26]     max_temp_cell_c          (int8,  °C)
                 * [27]     min_temp_cell_c          (int8,  °C)
                 * [28]     max_delta_cell_temp_c    (int8,  °C)
                 * [29]     max_temp_int_c           (int8,  °C)
                 * [30]     min_temp_int_c           (int8,  °C)
                 * [31]     max_temp_fet_c           (int8,  °C)
                 */
                for (i = 0U; i < 4U; i++)
                {
                    out->cell_max_mv[i] =
                        (int16_t)bq_le16(buf[i * 2U], buf[(i * 2U) + 1U]);
                }
                for (i = 0U; i < 4U; i++)
                {
                    out->cell_min_mv[i] =
                        (int16_t)bq_le16(buf[8U + (i * 2U)],
                                         buf[9U + (i * 2U)]);
                }
                out->max_delta_cell_mv      = (int16_t)bq_le16(buf[16], buf[17]);
                out->max_chg_curr_ma        = (int16_t)bq_le16(buf[18], buf[19]);
                out->max_dsg_curr_ma        = (int16_t)bq_le16(buf[20], buf[21]);
                out->max_avg_dsg_curr_ma    = (int16_t)bq_le16(buf[22], buf[23]);
                out->max_avg_dsg_pwr_cw     = (int16_t)bq_le16(buf[24], buf[25]);
                out->max_temp_cell_c        = (int8_t)buf[26];
                out->min_temp_cell_c        = (int8_t)buf[27];
                out->max_delta_cell_temp_c  = (int8_t)buf[28];
                out->max_temp_int_c         = (int8_t)buf[29];
                out->min_temp_int_c         = (int8_t)buf[30];
                out->max_temp_fet_c         = (int8_t)buf[31];
            }
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetLifetimeDataBlock2(
        BQ40Z50R2_Handle_t              *handle,
        BQ40Z50R2_LifetimeDataBlock2_t   *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_LDB2_BYTES];
    uint8_t            n;
    uint8_t            i;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_LIFETIME_DATA_BLOCK2,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_LDB2_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* Format (TRM 14.1.47): all single bytes (U1).
                 * [0] AA nb_shutdowns
                 * [1] BB nb_partial_rst
                 * [2] CC nb_full_rst
                 * [3] DD nb_wdt_rst
                 * [4] EE cb_time_cell[0]  (2 h/LSB)
                 * [5] FF cb_time_cell[1]
                 * [6] GG cb_time_cell[2]
                 * [7] HH cb_time_cell[3]
                 */
                out->nb_shutdowns   = buf[0];
                out->nb_partial_rst = buf[1];
                out->nb_full_rst    = buf[2];
                out->nb_wdt_rst     = buf[3];
                for (i = 0U; i < 4U; i++)
                {
                    out->cb_time_cell[i] = buf[4U + i];
                }
            }
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetLifetimeDataBlock3(
        BQ40Z50R2_Handle_t              *handle,
        BQ40Z50R2_LifetimeDataBlock3_t   *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_LDB3_BYTES];
    uint8_t            n;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_LIFETIME_DATA_BLOCK3,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_LDB3_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* Format (TRM 14.1.48): 8 x uint16 (U2), unit 2 h/LSB. */
                out->total_fw_runtime = bq_le16(buf[0],  buf[1]);
                out->time_ut          = bq_le16(buf[2],  buf[3]);
                out->time_lt          = bq_le16(buf[4],  buf[5]);
                out->time_stl         = bq_le16(buf[6],  buf[7]);
                out->time_rt          = bq_le16(buf[8],  buf[9]);
                out->time_sth         = bq_le16(buf[10], buf[11]);
                out->time_ht          = bq_le16(buf[12], buf[13]);
                out->time_ot          = bq_le16(buf[14], buf[15]);
            }
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetLifetimeDataBlock4(
        BQ40Z50R2_Handle_t              *handle,
        BQ40Z50R2_LifetimeDataBlock4_t   *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_LDB4_BYTES];
    uint8_t            n;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_LIFETIME_DATA_BLOCK4,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_LDB4_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* Format (TRM 14.1.49): 16 x uint16 (U2). */
                out->nb_cov    = bq_le16(buf[0],  buf[1]);
                out->last_cov  = bq_le16(buf[2],  buf[3]);
                out->nb_cuv    = bq_le16(buf[4],  buf[5]);
                out->last_cuv  = bq_le16(buf[6],  buf[7]);
                out->nb_ocd1   = bq_le16(buf[8],  buf[9]);
                out->last_ocd1 = bq_le16(buf[10], buf[11]);
                out->nb_ocd2   = bq_le16(buf[12], buf[13]);
                out->last_ocd2 = bq_le16(buf[14], buf[15]);
                out->nb_occ1   = bq_le16(buf[16], buf[17]);
                out->last_occ1 = bq_le16(buf[18], buf[19]);
                out->nb_occ2   = bq_le16(buf[20], buf[21]);
                out->last_occ2 = bq_le16(buf[22], buf[23]);
                out->nb_aold   = bq_le16(buf[24], buf[25]);
                out->last_aold = bq_le16(buf[26], buf[27]);
                out->nb_ascd   = bq_le16(buf[28], buf[29]);
                out->last_ascd = bq_le16(buf[30], buf[31]);
            }
        }
    }

    return status;
}

BQ40Z50R2_Status_t BQ40Z50R2_GetLifetimeDataBlock5(
        BQ40Z50R2_Handle_t              *handle,
        BQ40Z50R2_LifetimeDataBlock5_t   *out)
{
    BQ40Z50R2_Status_t status;
    uint8_t            buf[BQ40Z50R2_LDB5_BYTES];
    uint8_t            n;

    if (out == NULL)
    {
        status = BQ40Z50R2_INVALID_ARG;
    }
    else
    {
        status = BQ40Z50R2_MACRead(handle, BQ40Z50R2_MAC_LIFETIME_DATA_BLOCK5,
                                   buf, sizeof(buf), &n);
        if (status == BQ40Z50R2_OK)
        {
            if (n < BQ40Z50R2_LDB5_BYTES)
            {
                status = BQ40Z50R2_SIZE_ERROR;
            }
            else
            {
                /* Format (TRM 14.1.50): 16 x uint16 (U2). */
                out->nb_ascc             = bq_le16(buf[0],  buf[1]);
                out->last_ascc           = bq_le16(buf[2],  buf[3]);
                out->nb_otc              = bq_le16(buf[4],  buf[5]);
                out->last_otc            = bq_le16(buf[6],  buf[7]);
                out->nb_otd              = bq_le16(buf[8],  buf[9]);
                out->last_otd            = bq_le16(buf[10], buf[11]);
                out->nb_otf              = bq_le16(buf[12], buf[13]);
                out->last_otf            = bq_le16(buf[14], buf[15]);
                out->nb_valid_chgterm    = bq_le16(buf[16], buf[17]);
                out->last_valid_chgterm  = bq_le16(buf[18], buf[19]);
                out->nb_qmax_updates     = bq_le16(buf[20], buf[21]);
                out->last_qmax_update    = bq_le16(buf[22], buf[23]);
                out->nb_ra_updates       = bq_le16(buf[24], buf[25]);
                out->last_ra_update      = bq_le16(buf[26], buf[27]);
                out->nb_ra_disable       = bq_le16(buf[28], buf[29]);
                out->last_ra_disable     = bq_le16(buf[30], buf[31]);
            }
        }
    }

    return status;
}
