/**
 * @file    bq40z50r2.h
 * @brief   Platform-independent C driver for the Texas Instruments
 *          BQ40Z50-R2 1S-4S Li-Ion/Li-Polymer battery pack manager.
 *
 * @details This driver implements the SMBus v1.1 communication layer required
 *          to talk to the BQ40Z50-R2 gas gauge, as well as a set of high-level
 *          helpers to read measurements and status registers and to drive the
 *          device through its Manufacturer Access System (MAC).
 *
 *          The driver is hardware-agnostic: the application must provide the
 *          low-level SMBus transfer primitives through the
 *          @ref BQ40Z50R2_Handle_t structure. A reference binding for the
 *          STM32 HAL is documented in bq40z50r2.c.
 *
 *          Reference: BQ40Z50-R2 Technical Reference Manual (SLUUBK0B) and
 *          BQ40Z50-R2 datasheet (SLUSCS4C).
 *
 */

 /* ==== Header Guard ==== */
#ifndef BQ40Z50R2_H
#define BQ40Z50R2_H

#ifdef __cplusplus
extern "C" {
#endif

/* ==== Includes ==== */
#include <stdint.h>
#include <stddef.h>

/* ==== Defines ==== */

/**
 * @defgroup BQ40Z50R2_DEF_DA Device addressing
 * @{
 *
 */

/**
 * @brief Default 7-bit SMBus device address of the BQ40Z50-R2.
 * @details The device defaults to 0x16 if the data-flash Address/Address Check
 *          values are invalid (see TRM section 13). This is a 7-bit address;
 *          the application's transfer callback is responsible for any required
 *          left shift to build the 8-bit read/write byte.
 */
#define BQ40Z50R2_DEFAULT_ADDR_7BIT          (0x16U)

/**
 * @brief Shift to take the 7-bit address of the sensor to an 8-bit I2C address.
 *
 */
#define BQ40Z50R2_ADDR_I2C_SHIFT             (1U)

/**
 * @brief Default 8-bit I2C devide address of the BQ40Z50-R2
 *
 */
#define BQ40Z50R2_DEFAULT_ADDR_I2C           (BQ40Z50R2_DEFAULT_ADDR_7BIT << BQ40Z50R2_ADDR_I2C_SHIFT)

/**  @} */

/**
 * @defgroup BQ40Z50R2_DEF_BTS Block transfer sizing
 * @{
 *
 */

/**
 * @brief Maximum SMBus block payload size handled by this driver.
 * @details The SMBus block protocol allows up to 32 data bytes. MAC block
 *          replies on this device never exceed 32 data bytes, so an internal
 *          working buffer must be at least this large plus the optional
 *          2-byte echoed MAC command.
 */
#define BQ40Z50R2_MAX_BLOCK_LEN              (32U)

/**
 * @brief Size of the 2-byte MAC command echoed back on a 0x44 block read.
 */
#define BQ40Z50R2_MAC_CMD_LEN               (2U)

/**  @} */

/**
 * @defgroup BQ40Z50R2_DEF_SBS Standard SBS commands (TRM chapter 14)
 * @{
 *
 */

#define BQ40Z50R2_CMD_MANUFACTURER_ACCESS       (0x00U) /**< ManufacturerAccess(), word.   */
#define BQ40Z50R2_CMD_REMAINING_CAP_ALARM       (0x01U) /**< RemainingCapacityAlarm(), U2. */
#define BQ40Z50R2_CMD_REMAINING_TIME_ALARM      (0x02U) /**< RemainingTimeAlarm(), U2.     */
#define BQ40Z50R2_CMD_BATTERY_MODE              (0x03U) /**< BatteryMode(), H2.            */
#define BQ40Z50R2_CMD_AT_RATE                   (0x04U) /**< AtRate(), I2.                 */
#define BQ40Z50R2_CMD_AT_RATE_TIME_TO_FULL      (0x05U) /**< AtRateTimeToFull(), U2.       */
#define BQ40Z50R2_CMD_AT_RATE_TIME_TO_EMPTY     (0x06U) /**< AtRateTimeToEmpty(), U2.      */
#define BQ40Z50R2_CMD_AT_RATE_OK                (0x07U) /**< AtRateOK(), U2.               */
#define BQ40Z50R2_CMD_TEMPERATURE               (0x08U) /**< Temperature(), U2, 0.1 K.     */
#define BQ40Z50R2_CMD_VOLTAGE                   (0x09U) /**< Voltage(), U2, mV.            */
#define BQ40Z50R2_CMD_CURRENT                   (0x0AU) /**< Current(), I2, mA.            */
#define BQ40Z50R2_CMD_AVERAGE_CURRENT           (0x0BU) /**< AverageCurrent(), I2, mA.     */
#define BQ40Z50R2_CMD_MAX_ERROR                 (0x0CU) /**< MaxError(), U1, %.            */
#define BQ40Z50R2_CMD_REL_STATE_OF_CHARGE       (0x0DU) /**< RelativeStateOfCharge(), U1.  */
#define BQ40Z50R2_CMD_ABS_STATE_OF_CHARGE       (0x0EU) /**< AbsoluteStateOfCharge(), U1.  */
#define BQ40Z50R2_CMD_REMAINING_CAPACITY        (0x0FU) /**< RemainingCapacity(), U2.      */
#define BQ40Z50R2_CMD_FULL_CHARGE_CAPACITY      (0x10U) /**< FullChargeCapacity(), U2.     */
#define BQ40Z50R2_CMD_RUN_TIME_TO_EMPTY         (0x11U) /**< RunTimeToEmpty(), U2, min.    */
#define BQ40Z50R2_CMD_AVG_TIME_TO_EMPTY         (0x12U) /**< AverageTimeToEmpty(), U2.     */
#define BQ40Z50R2_CMD_AVG_TIME_TO_FULL          (0x13U) /**< AverageTimeToFull(), U2.      */
#define BQ40Z50R2_CMD_CHARGING_CURRENT          (0x14U) /**< ChargingCurrent(), U2, mA.    */
#define BQ40Z50R2_CMD_CHARGING_VOLTAGE          (0x15U) /**< ChargingVoltage(), U2, mV.    */
#define BQ40Z50R2_CMD_BATTERY_STATUS            (0x16U) /**< BatteryStatus(), H2.          */
#define BQ40Z50R2_CMD_CYCLE_COUNT               (0x17U) /**< CycleCount(), U2, cycles.     */
#define BQ40Z50R2_CMD_DESIGN_CAPACITY           (0x18U) /**< DesignCapacity(), U2.         */
#define BQ40Z50R2_CMD_DESIGN_VOLTAGE            (0x19U) /**< DesignVoltage(), U2, mV.      */
#define BQ40Z50R2_CMD_SPECIFICATION_INFO        (0x1AU) /**< SpecificationInfo(), H2.      */
#define BQ40Z50R2_CMD_MANUFACTURER_DATE         (0x1BU) /**< ManufacturerDate(), U2.       */
#define BQ40Z50R2_CMD_SERIAL_NUMBER             (0x1CU) /**< SerialNumber(), H2.           */
#define BQ40Z50R2_CMD_MANUFACTURER_NAME         (0x20U) /**< ManufacturerName(), block.    */
#define BQ40Z50R2_CMD_DEVICE_NAME               (0x21U) /**< DeviceName(), block.          */
#define BQ40Z50R2_CMD_DEVICE_CHEMISTRY          (0x22U) /**< DeviceChemistry(), block.     */
#define BQ40Z50R2_CMD_MANUFACTURER_DATA         (0x23U) /**< ManufacturerData(), block.    */
#define BQ40Z50R2_CMD_AUTHENTICATE              (0x2FU) /**< Authenticate(), block.        */
#define BQ40Z50R2_CMD_CELL_VOLTAGE_4            (0x3CU) /**< CellVoltage4(), U2, mV.        */
#define BQ40Z50R2_CMD_CELL_VOLTAGE_3            (0x3DU) /**< CellVoltage3(), U2, mV.        */
#define BQ40Z50R2_CMD_CELL_VOLTAGE_2            (0x3EU) /**< CellVoltage2(), U2, mV.        */
#define BQ40Z50R2_CMD_CELL_VOLTAGE_1            (0x3FU) /**< CellVoltage1(), U2, mV.        */
#define BQ40Z50R2_CMD_BTP_DISCHARGE_SET         (0x4AU) /**< BTPDischargeSet(), I2, mAh.    */
#define BQ40Z50R2_CMD_BTP_CHARGE_SET            (0x4BU) /**< BTPChargeSet(), I2, mAh.      */
#define BQ40Z50R2_CMD_STATE_OF_HEALTH           (0x4FU) /**< State-of-Health, word, %.     */
#define BQ40Z50R2_CMD_SAFETY_ALERT              (0x50U) /**< SafetyAlert(), block H4.      */
#define BQ40Z50R2_CMD_SAFETY_STATUS             (0x51U) /**< SafetyStatus(), block H4.     */
#define BQ40Z50R2_CMD_PF_ALERT                  (0x52U) /**< PFAlert(), block H4.          */
#define BQ40Z50R2_CMD_PF_STATUS                 (0x53U) /**< PFStatus(), block H4.         */
#define BQ40Z50R2_CMD_OPERATION_STATUS          (0x54U) /**< OperationStatus(), block H4.  */
#define BQ40Z50R2_CMD_CHARGING_STATUS           (0x55U) /**< ChargingStatus(), block.      */
#define BQ40Z50R2_CMD_GAUGING_STATUS            (0x56U) /**< GaugingStatus(), block.       */
#define BQ40Z50R2_CMD_MANUFACTURING_STATUS      (0x57U) /**< ManufacturingStatus(), block. */
#define BQ40Z50R2_CMD_AFE_REGISTER              (0x58U) /**< AFERegister(), block.         */
#define BQ40Z50R2_CMD_MAX_TURBO_PWR             (0x59U) /**< MaxTurboPwr(), word.          */
#define BQ40Z50R2_CMD_SUS_TURBO_PWR             (0x5AU) /**< SusTurboPwr(), word.          */
#define BQ40Z50R2_CMD_TURBO_PACK_R              (0x5BU) /**< TURBO_PACK_R, word.           */
#define BQ40Z50R2_CMD_TURBO_SYS_R               (0x5CU) /**< TURBO_SYS_R, word.            */
#define BQ40Z50R2_CMD_TURBO_EDV                 (0x5DU) /**< TURBO_EDV, word.              */
#define BQ40Z50R2_CMD_MAX_TURBO_CURR            (0x5EU) /**< MaxTurboCurr(), word.         */
#define BQ40Z50R2_CMD_SUS_TURBO_CURR            (0x5FU) /**< SusTurboCurr(), word.         */
#define BQ40Z50R2_CMD_LIFETIME_DATA_BLOCK1      (0x60U) /**< Lifetime Data Block 1, block. */
#define BQ40Z50R2_CMD_LIFETIME_DATA_BLOCK2      (0x61U) /**< Lifetime Data Block 2, block. */
#define BQ40Z50R2_CMD_LIFETIME_DATA_BLOCK3      (0x62U) /**< Lifetime Data Block 3, block. */
#define BQ40Z50R2_CMD_LIFETIME_DATA_BLOCK4      (0x63U) /**< Lifetime Data Block 4, block. */
#define BQ40Z50R2_CMD_LIFETIME_DATA_BLOCK5      (0x64U) /**< Lifetime Data Block 5, block. */
#define BQ40Z50R2_CMD_MANUFACTURER_INFO         (0x70U) /**< ManufacturerInfo, block.      */
#define BQ40Z50R2_CMD_DA_STATUS1                (0x71U) /**< DAStatus1, block (32 bytes).  */
#define BQ40Z50R2_CMD_DA_STATUS2                (0x72U) /**< DAStatus2, block (16 bytes).  */
#define BQ40Z50R2_CMD_GAUGE_STATUS1             (0x73U) /**< GaugeStatus1, block.          */
#define BQ40Z50R2_CMD_GAUGE_STATUS2             (0x74U) /**< GaugeStatus2, block.          */
#define BQ40Z50R2_CMD_GAUGE_STATUS3             (0x75U) /**< GaugeStatus3, block.          */
#define BQ40Z50R2_CMD_CB_STATUS                 (0x76U) /**< CBStatus, block.              */
#define BQ40Z50R2_CMD_STATE_OF_HEALTH_77        (0x77U) /**< State-of-Health, block.       */
#define BQ40Z50R2_CMD_FILTERED_CAPACITY         (0x78U) /**< FilteredCapacity, block.      */

/**
 * @brief ManufacturerBlockAccess() command (0x44).
 * @details Preferred way of issuing MAC commands and reading their block
 *          replies on the bq40zxy family (TRM section 14.1).
 */
#define BQ40Z50R2_CMD_MANUFACTURER_BLOCK_ACCESS (0x44U)

/**  @} */

/**
 * @defgroup BQ40Z50R2_DEF_MAC ManufacturerAccess() / MAC sub-commands (TRM section 14.1)
 * @{
 *
 */

#define BQ40Z50R2_MAC_OPERATION_STATUS_LOW      (0x0000U) /**< Returns low 16 bits of OperationStatus(). */
#define BQ40Z50R2_MAC_DEVICE_TYPE               (0x0001U) /**< DeviceType.                  */
#define BQ40Z50R2_MAC_FIRMWARE_VERSION          (0x0002U) /**< FirmwareVersion.             */
#define BQ40Z50R2_MAC_HARDWARE_VERSION          (0x0003U) /**< HardwareVersion.             */
#define BQ40Z50R2_MAC_IF_CHECKSUM               (0x0004U) /**< Instruction Flash signature. */
#define BQ40Z50R2_MAC_STATIC_DF_SIGNATURE       (0x0005U) /**< Static DF signature.         */
#define BQ40Z50R2_MAC_CHEM_ID                   (0x0006U) /**< Chemical ID.                 */
#define BQ40Z50R2_MAC_STATIC_CHEM_DF_SIGNATURE  (0x0008U) /**< Static Chem DF signature.    */
#define BQ40Z50R2_MAC_ALL_DF_SIGNATURE          (0x0009U) /**< All DF signature.            */
#define BQ40Z50R2_MAC_SHUTDOWN_MODE             (0x0010U) /**< Enter SHUTDOWN mode.         */
#define BQ40Z50R2_MAC_SLEEP_MODE                (0x0011U) /**< Enter SLEEP mode.            */
#define BQ40Z50R2_MAC_DEVICE_RESET_LEGACY       (0x0012U) /**< Device reset (bq30zxy compat). */
#define BQ40Z50R2_MAC_AUTO_CC_OFFSET            (0x0013U) /**< Start Auto CC Offset cal.    */
#define BQ40Z50R2_MAC_FUSE_TOGGLE               (0x001DU) /**< Toggle FUSE output.          */
#define BQ40Z50R2_MAC_PCHG_FET_TOGGLE           (0x001EU) /**< Toggle PCHG FET.             */
#define BQ40Z50R2_MAC_CHG_FET_TOGGLE            (0x001FU) /**< Toggle CHG FET.              */
#define BQ40Z50R2_MAC_DSG_FET_TOGGLE            (0x0020U) /**< Toggle DSG FET.              */
#define BQ40Z50R2_MAC_GAUGING                   (0x0021U) /**< Toggle gauging.              */
#define BQ40Z50R2_MAC_FET_CONTROL               (0x0022U) /**< Toggle FW FET control.       */
#define BQ40Z50R2_MAC_LIFETIME_DATA_COLLECTION  (0x0023U) /**< Toggle lifetime collection.  */
#define BQ40Z50R2_MAC_PERMANENT_FAILURE         (0x0024U) /**< Toggle permanent failure.    */
#define BQ40Z50R2_MAC_BLACK_BOX_RECORDER        (0x0025U) /**< Toggle black box recorder.   */
#define BQ40Z50R2_MAC_FUSE                      (0x0026U) /**< Toggle FW fuse control.      */
#define BQ40Z50R2_MAC_LED_DISPLAY_ENABLE        (0x0027U) /**< Toggle LED display enable.   */
#define BQ40Z50R2_MAC_LIFETIME_DATA_RESET       (0x0028U) /**< Reset lifetime data.         */
#define BQ40Z50R2_MAC_PERMANENT_FAIL_DATA_RESET (0x0029U) /**< Reset PF data.               */
#define BQ40Z50R2_MAC_BLACK_BOX_RECORDER_RESET  (0x002AU) /**< Reset black box data.        */
#define BQ40Z50R2_MAC_LED_TOGGLE                (0x002BU) /**< Toggle LED display.          */
#define BQ40Z50R2_MAC_LED_DISPLAY_PRESS         (0x002CU) /**< Simulate DISP press.         */
#define BQ40Z50R2_MAC_CALIBRATION_MODE          (0x002DU) /**< Toggle calibration mode.     */
#define BQ40Z50R2_MAC_LIFETIME_DATA_FLUSH       (0x002EU) /**< Flush lifetime data to DF.   */
#define BQ40Z50R2_MAC_LIFETIME_DATA_SPEED_UP    (0x002FU) /**< Toggle lifetime speed-up.    */
#define BQ40Z50R2_MAC_SEAL_DEVICE               (0x0030U) /**< Seal the device.             */
#define BQ40Z50R2_MAC_SECURITY_KEYS             (0x0035U) /**< Read/write security keys.    */
#define BQ40Z50R2_MAC_AUTHENTICATION_KEY        (0x0037U) /**< Update authentication key.   */
#define BQ40Z50R2_MAC_DEVICE_RESET              (0x0041U) /**< Device reset.                */
#define BQ40Z50R2_MAC_SAFETY_ALERT              (0x0050U) /**< SafetyAlert block.           */
#define BQ40Z50R2_MAC_SAFETY_STATUS             (0x0051U) /**< SafetyStatus block.          */
#define BQ40Z50R2_MAC_PF_ALERT                  (0x0052U) /**< PFAlert block.               */
#define BQ40Z50R2_MAC_PF_STATUS                 (0x0053U) /**< PFStatus block.              */
#define BQ40Z50R2_MAC_OPERATION_STATUS          (0x0054U) /**< OperationStatus block.       */
#define BQ40Z50R2_MAC_CHARGING_STATUS           (0x0055U) /**< ChargingStatus block.        */
#define BQ40Z50R2_MAC_GAUGING_STATUS            (0x0056U) /**< GaugingStatus block.         */
#define BQ40Z50R2_MAC_MANUFACTURING_STATUS      (0x0057U) /**< ManufacturingStatus block.   */
#define BQ40Z50R2_MAC_AFE_REGISTER              (0x0058U) /**< AFE register block.          */
#define BQ40Z50R2_MAC_NO_LOAD_REM_CAP           (0x005AU) /**< NoLoadRemCap block.          */
#define BQ40Z50R2_MAC_LIFETIME_DATA_BLOCK1      (0x0060U) /**< Lifetime Data Block 1.       */
#define BQ40Z50R2_MAC_LIFETIME_DATA_BLOCK2      (0x0061U) /**< Lifetime Data Block 2.       */
#define BQ40Z50R2_MAC_LIFETIME_DATA_BLOCK3      (0x0062U) /**< Lifetime Data Block 3.       */
#define BQ40Z50R2_MAC_LIFETIME_DATA_BLOCK4      (0x0063U) /**< Lifetime Data Block 4.       */
#define BQ40Z50R2_MAC_LIFETIME_DATA_BLOCK5      (0x0064U) /**< Lifetime Data Block 5.       */
#define BQ40Z50R2_MAC_MANUFACTURER_INFO         (0x0070U) /**< ManufacturerInfo block.      */
#define BQ40Z50R2_MAC_DA_STATUS1                (0x0071U) /**< DAStatus1 block.             */
#define BQ40Z50R2_MAC_DA_STATUS2                (0x0072U) /**< DAStatus2 block.             */
#define BQ40Z50R2_MAC_GAUGE_STATUS1             (0x0073U) /**< GaugeStatus1 block.          */
#define BQ40Z50R2_MAC_GAUGE_STATUS2             (0x0074U) /**< GaugeStatus2 block.          */
#define BQ40Z50R2_MAC_GAUGE_STATUS3             (0x0075U) /**< GaugeStatus3 block.          */
#define BQ40Z50R2_MAC_CB_STATUS                 (0x0076U) /**< CBStatus block.              */
#define BQ40Z50R2_MAC_STATE_OF_HEALTH           (0x0077U) /**< State-of-Health block.       */
#define BQ40Z50R2_MAC_FILTER_CAPACITY           (0x0078U) /**< FilterCapacity block.        */
#define BQ40Z50R2_MAC_RSOC_WRITE                (0x0079U) /**< RSOC write.                  */
#define BQ40Z50R2_MAC_MANUFACTURER_INFO_B       (0x007AU) /**< ManufacturerInfoB block.     */
#define BQ40Z50R2_MAC_IATA_SHUTDOWN             (0x00F0U) /**< IATA shutdown.               */
#define BQ40Z50R2_MAC_IATA_RM                   (0x00F1U) /**< IATA remaining capacity.     */
#define BQ40Z50R2_MAC_IATA_FCC                  (0x00F2U) /**< IATA full charge capacity.   */
#define BQ40Z50R2_MAC_ROM_MODE                  (0x0F00U) /**< Enter ROM (bootloader) mode. */

/**
 * @brief Default security keys (TRM section 14.1.33).
 * @details It is strongly recommended to change these in production packs.
 */
#define BQ40Z50R2_DEFAULT_UNSEAL_KEY1           (0x0414U) /**< First word of default UNSEAL key.       */
#define BQ40Z50R2_DEFAULT_UNSEAL_KEY2           (0x3672U) /**< Second word of default UNSEAL key.      */
#define BQ40Z50R2_DEFAULT_FULL_ACCESS_KEY1      (0xFFFFU) /**< First word of default FULL ACCESS key.  */
#define BQ40Z50R2_DEFAULT_FULL_ACCESS_KEY2      (0xFFFFU) /**< Second word of default FULL ACCESS key. */

/**  @} */

/**
 * @defgroup BQ40Z50R2_DEF_SAF SafetyStatus() / SafetyAlert() bit masks (32-bit, TRM sections 14.36 and 14.1.37)
 * @{
 *
 */


#define BQ40Z50R2_SAFETY_OCDL   (1UL << 29) /**< Overcurrent in Discharge Latch.        */
#define BQ40Z50R2_SAFETY_COVL   (1UL << 28) /**< Cell Overvoltage Latch.                */
#define BQ40Z50R2_SAFETY_UTD    (1UL << 27) /**< Undertemperature During Discharge.     */
#define BQ40Z50R2_SAFETY_UTC    (1UL << 26) /**< Undertemperature During Charge.        */
#define BQ40Z50R2_SAFETY_PCHGC  (1UL << 25) /**< Over-Precharge Current.                */
#define BQ40Z50R2_SAFETY_CHGV   (1UL << 24) /**< Overcharging Voltage.                  */
#define BQ40Z50R2_SAFETY_CHGC   (1UL << 23) /**< Overcharging Current.                  */
#define BQ40Z50R2_SAFETY_OC     (1UL << 22) /**< Overcharge.                            */
#define BQ40Z50R2_SAFETY_CTO    (1UL << 20) /**< Charge Timeout.                        */
#define BQ40Z50R2_SAFETY_PTO    (1UL << 18) /**< Precharge Timeout.                     */
#define BQ40Z50R2_SAFETY_OTF    (1UL << 16) /**< Overtemperature FET.                   */
#define BQ40Z50R2_SAFETY_CUVC   (1UL << 14) /**< Cell Undervoltage Compensated.         */
#define BQ40Z50R2_SAFETY_OTD    (1UL << 13) /**< Overtemperature During Discharge.      */
#define BQ40Z50R2_SAFETY_OTC    (1UL << 12) /**< Overtemperature During Charge.         */
#define BQ40Z50R2_SAFETY_ASCDL  (1UL << 11) /**< Short-circuit During Discharge Latch.  */
#define BQ40Z50R2_SAFETY_ASCD   (1UL << 10) /**< Short-circuit During Discharge.        */
#define BQ40Z50R2_SAFETY_ASCCL  (1UL <<  9) /**< Short-circuit During Charge Latch.     */
#define BQ40Z50R2_SAFETY_ASCC   (1UL <<  8) /**< Short-circuit During Charge.           */
#define BQ40Z50R2_SAFETY_AOLDL  (1UL <<  7) /**< Overload During Discharge Latch.       */
#define BQ40Z50R2_SAFETY_AOLD   (1UL <<  6) /**< Overload During Discharge.             */
#define BQ40Z50R2_SAFETY_OCD2   (1UL <<  5) /**< Overcurrent During Discharge 2.        */
#define BQ40Z50R2_SAFETY_OCD1   (1UL <<  4) /**< Overcurrent During Discharge 1.        */
#define BQ40Z50R2_SAFETY_OCC2   (1UL <<  3) /**< Overcurrent During Charge 2.           */
#define BQ40Z50R2_SAFETY_OCC1   (1UL <<  2) /**< Overcurrent During Charge 1.           */
#define BQ40Z50R2_SAFETY_COV    (1UL <<  1) /**< Cell Overvoltage.                      */
#define BQ40Z50R2_SAFETY_CUV    (1UL <<  0) /**< Cell Undervoltage.                     */

/**  @} */

/**
 * @defgroup BQ40Z50R2_DEF_PF PFAlert() / PFStatus() bit masks (32-bit, TRM sections 14.1.38 and 14.1.39)
 * @{
 *
 */

#define BQ40Z50R2_PF_TS4    (1UL << 31) /**< Open Thermistor - TS4 Failure */
#define BQ40Z50R2_PF_TS3    (1UL << 30) /**< Open Thermistor - TS3 Failure */
#define BQ40Z50R2_PF_TS2    (1UL << 29) /**< Open Thermistor - TS2 Failure */
#define BQ40Z50R2_PF_TS1    (1UL << 28) /**< Open Thermistor - TS1 Failure */
#define BQ40Z50R2_PF_2LVL   (1UL << 22) /**< Second Level Protector Failure */
#define BQ40Z50R2_PF_AFEC   (1UL << 21) /**< AFE Communication Failure */
#define BQ40Z50R2_PF_AFER   (1UL << 20) /**< AFE Register Failure */
#define BQ40Z50R2_PF_FUSE   (1UL << 19) /**< Chemical Fuse Failure */
#define BQ40Z50R2_PF_OCDL   (1UL << 18) /**< Overcurrent in Discharge */
#define BQ40Z50R2_PF_DFETF  (1UL << 17) /**< Discharge FET Failure */
#define BQ40Z50R2_PF_CFETF  (1UL << 16) /**< Charge FET Failure */
#define BQ40Z50R2_PF_ASCDL  (1UL << 15) /**< Short Circuit in Discharge */
#define BQ40Z50R2_PF_ASCCL  (1UL << 14) /**< Short Circuit in Charge */
#define BQ40Z50R2_PF_AOLDL  (1UL << 13) /**< Overload in Discharge */
#define BQ40Z50R2_PF_VIMA   (1UL << 12) /**< Voltage Imbalance While Pack Is Active Failure */
#define BQ40Z50R2_PF_VIMR   (1UL << 11) /**< Voltage Imbalance While Pack Is At Rest Failure */
#define BQ40Z50R2_PF_CD     (1UL << 10) /**< Capacity Degradation Failure */
#define BQ40Z50R2_PF_IMP    (1UL <<  9) /**< Impedance Failure */
#define BQ40Z50R2_PF_CB     (1UL <<  8) /**< Cell Balancing Failure */
#define BQ40Z50R2_PF_QIM    (1UL <<  7) /**< QMax Imbalance Failure */
#define BQ40Z50R2_PF_SOTF   (1UL <<  6) /**< Safety Over temperature FET Failure */
#define BQ40Z50R2_PF_COVL   (1UL <<  5) /**< Cell Overvoltage Latch */
#define BQ40Z50R2_PF_SOT    (1UL <<  4) /**< Safety Over temperature Cell Failure */
#define BQ40Z50R2_PF_SOCD   (1UL <<  3) /**< Safety Overcurrent in Discharge */
#define BQ40Z50R2_PF_SOCC   (1UL <<  2) /**< Safety Overcurrent in Charge */
#define BQ40Z50R2_PF_SOV    (1UL <<  1) /**< Safety Cell Overvoltage Failure */
#define BQ40Z50R2_PF_SUV    (1UL <<  0) /**< Safety Cell Undervoltage Failure */

/**  @} */

/**
 * @defgroup BQ40Z50R2_DEF_OP OperationStatus() bit masks (32-bit, TRM section 14.1.40)
 * @{
 *
 */

#define BQ40Z50R2_OPSTAT_EMSHUT     (1UL << 29)       /**< Emergency FET Shutdown.              */
#define BQ40Z50R2_OPSTAT_CB         (1UL << 28)       /**< Cell balancing active.               */
#define BQ40Z50R2_OPSTAT_SLPCC      (1UL << 27)       /**< CC measurement in SLEEP mode.        */
#define BQ40Z50R2_OPSTAT_SLPAD      (1UL << 26)       /**< ADC measurement in SLEEP mode.       */
#define BQ40Z50R2_OPSTAT_SMBLCAL    (1UL << 25)       /**< Auto CC cal when bus low.            */
#define BQ40Z50R2_OPSTAT_INIT       (1UL << 24)       /**< Initialization after full reset.     */
#define BQ40Z50R2_OPSTAT_SLEEPM     (1UL << 23)       /**< SLEEP mode triggered via command.    */
#define BQ40Z50R2_OPSTAT_XL         (1UL << 22)       /**< 400-kHz SMBus mode active.           */
#define BQ40Z50R2_OPSTAT_CALOFFSET  (1UL << 21)       /**< Calibration output (shorted CC).    */
#define BQ40Z50R2_OPSTAT_CAL        (1UL << 20)       /**< Calibration output (raw ADC and CC). */
#define BQ40Z50R2_OPSTAT_AUTOCALM   (1UL << 19)       /**< Auto CC Offset cal by MAC.           */
#define BQ40Z50R2_OPSTAT_AUTH       (1UL << 18)       /**< Authentication in progress.          */
#define BQ40Z50R2_OPSTAT_LED        (1UL << 17)       /**< LED display on.                      */
#define BQ40Z50R2_OPSTAT_SDM        (1UL << 16)       /**< Shutdown triggered via command.      */
#define BQ40Z50R2_OPSTAT_SLEEP      (1UL << 15)       /**< SLEEP mode conditions met.           */
#define BQ40Z50R2_OPSTAT_XCHG       (1UL << 14)       /**< Charging disabled.                   */
#define BQ40Z50R2_OPSTAT_XDSG       (1UL << 13)       /**< Discharging disabled.                */
#define BQ40Z50R2_OPSTAT_PF         (1UL << 12)       /**< PERMANENT FAILURE mode active.       */
#define BQ40Z50R2_OPSTAT_SS         (1UL << 11)       /**< SAFETY status (OR of all bits).      */
#define BQ40Z50R2_OPSTAT_SDV        (1UL << 10)       /**< Shutdown via low pack voltage.       */
#define BQ40Z50R2_OPSTAT_SEC_MASK   (0x00000300UL)    /**< SECURITY mode field mask (bits 9-8). */
#define BQ40Z50R2_OPSTAT_SEC_SHIFT  (8U)              /**< SECURITY mode field shift.          */
#define BQ40Z50R2_OPSTAT_BTP_INT    (1UL <<  7)       /**< Battery Trip Point interrupt.        */
#define BQ40Z50R2_OPSTAT_FUSE       (1UL <<  5)       /**< Fuse status.                         */
#define BQ40Z50R2_OPSTAT_PCHG       (1UL <<  3)       /**< Precharge FET status.                */
#define BQ40Z50R2_OPSTAT_CHG        (1UL <<  2)       /**< CHG FET status.                      */
#define BQ40Z50R2_OPSTAT_DSG        (1UL <<  1)       /**< DSG FET status.                      */
#define BQ40Z50R2_OPSTAT_PRES       (1UL <<  0)       /**< System present (low) active.         */

/**
 * @brief SECURITY mode encodings extracted from OperationStatus() SEC field.
 */
#define BQ40Z50R2_SEC_FULL_ACCESS (0x1U) /**< Full Access (0b01).  */
#define BQ40Z50R2_SEC_UNSEALED    (0x2U) /**< Unsealed (0b10).     */
#define BQ40Z50R2_SEC_SEALED      (0x3U) /**< Sealed (0b11).       */

/**  @} */

/**
 * @defgroup BQ40Z50R2_DEF_CHG ChargingStatus() bit masks (TRM section 14.1.41)
 * @{
 *
 */

#define BQ40Z50R2_CHGSTAT_NCT  (1UL << 19) /**< Near charge termination.            */
#define BQ40Z50R2_CHGSTAT_CCC  (1UL << 18) /**< Charging loss compensation.         */
#define BQ40Z50R2_CHGSTAT_CVR  (1UL << 17) /**< Charging voltage rate of change.    */
#define BQ40Z50R2_CHGSTAT_CCR  (1UL << 16) /**< Charging current rate of change.    */
#define BQ40Z50R2_CHGSTAT_VCT  (1U  << 15) /**< Charge termination.                 */
#define BQ40Z50R2_CHGSTAT_MCHG (1U  << 14) /**< Maintenance charge.                 */
#define BQ40Z50R2_CHGSTAT_SU   (1U  << 13) /**< Suspend charge.                     */
#define BQ40Z50R2_CHGSTAT_IN   (1U  << 12) /**< Charge inhibit.                     */
#define BQ40Z50R2_CHGSTAT_HV   (1U  << 11) /**< High voltage region.                */
#define BQ40Z50R2_CHGSTAT_MV   (1U  << 10) /**< Mid voltage region.                 */
#define BQ40Z50R2_CHGSTAT_LV   (1U  <<  9) /**< Low voltage region.                 */
#define BQ40Z50R2_CHGSTAT_PV   (1U  <<  8) /**< Precharge voltage region.           */
#define BQ40Z50R2_CHGSTAT_OT   (1U  <<  6) /**< Overtemperature region.             */
#define BQ40Z50R2_CHGSTAT_HT   (1U  <<  5) /**< High temperature region.            */
#define BQ40Z50R2_CHGSTAT_STH  (1U  <<  4) /**< Standard temperature high region.   */
#define BQ40Z50R2_CHGSTAT_RT   (1U  <<  3) /**< Recommended temperature region.     */
#define BQ40Z50R2_CHGSTAT_STL  (1U  <<  2) /**< Standard temperature low region.    */
#define BQ40Z50R2_CHGSTAT_LT   (1U  <<  1) /**< Low temperature region.             */
#define BQ40Z50R2_CHGSTAT_UT   (1U  <<  0) /**< Undertemperature region.            */

/**  @} */

/**
 * @defgroup BQ40Z50R2_DEF_GAU GaugingStatus() bit masks (TRM section 14.1.42)
 * @{
 *
 */

#define BQ40Z50R2_GAUGE_OCVFR  (1UL << 20) /**< Open Circuit Voltage in Flat Region.   */
#define BQ40Z50R2_GAUGE_LDMD   (1UL << 19) /**< LOAD mode (1 = constant power).        */
#define BQ40Z50R2_GAUGE_RX     (1UL << 18) /**< Resistance update toggle.              */
#define BQ40Z50R2_GAUGE_QMAX   (1UL << 17) /**< QMax update toggle.                    */
#define BQ40Z50R2_GAUGE_VDQ    (1UL << 16) /**< Discharge qualified for learning.      */
#define BQ40Z50R2_GAUGE_NSFM   (1U  << 15) /**< Negative scale factor mode.            */
#define BQ40Z50R2_GAUGE_SLPQMAX (1U << 13) /**< OCV update in SLEEP mode.              */
#define BQ40Z50R2_GAUGE_QEN    (1U  << 12) /**< Impedance Track gauging enabled.       */
#define BQ40Z50R2_GAUGE_VOK    (1U  << 11) /**< Voltages OK for QMax update.           */
#define BQ40Z50R2_GAUGE_R_DIS  (1U  << 10) /**< Resistance updates disabled.           */
#define BQ40Z50R2_GAUGE_REST   (1U  <<  8) /**< Rest (OCV reading taken).              */
#define BQ40Z50R2_GAUGE_CF     (1U  <<  7) /**< Condition flag.                        */
#define BQ40Z50R2_GAUGE_DSG    (1U  <<  6) /**< Discharge/Relax.                       */
#define BQ40Z50R2_GAUGE_EDV    (1U  <<  5) /**< End-of-Discharge Termination Voltage.  */
#define BQ40Z50R2_GAUGE_BAL_EN (1U  <<  4) /**< Cell balancing possible.               */
#define BQ40Z50R2_GAUGE_TC     (1U  <<  3) /**< Terminate Charge.                      */
#define BQ40Z50R2_GAUGE_TD     (1U  <<  2) /**< Terminate Discharge.                   */
#define BQ40Z50R2_GAUGE_FC     (1U  <<  1) /**< Fully Charged.                         */
#define BQ40Z50R2_GAUGE_FD     (1U  <<  0) /**< Fully Discharged.                      */

/**  @} */

/**
 * @defgroup BQ40Z50R2_DEF_MFG ManufacturingStatus() bit masks (16-bit, TRM section 14.1.43)
 * @{
 *
 */

#define BQ40Z50R2_MFGSTAT_CAL_TEST (1U << 15) /**< Calibration mode enabled.   */
#define BQ40Z50R2_MFGSTAT_LT_TEST  (1U << 14) /**< Lifetime speed-up enabled.  */
#define BQ40Z50R2_MFGSTAT_LED_EN   (1U <<  9) /**< LED display enabled.        */
#define BQ40Z50R2_MFGSTAT_FUSE_EN  (1U <<  8) /**< Fuse action enabled.        */
#define BQ40Z50R2_MFGSTAT_BBR_EN   (1U <<  7) /**< Black box recorder enabled. */
#define BQ40Z50R2_MFGSTAT_PF_EN    (1U <<  6) /**< Permanent failure enabled.  */
#define BQ40Z50R2_MFGSTAT_LF_EN    (1U <<  5) /**< Lifetime collection enabled.*/
#define BQ40Z50R2_MFGSTAT_FET_EN   (1U <<  4) /**< All FET action enabled.     */
#define BQ40Z50R2_MFGSTAT_GAUGE_EN (1U <<  3) /**< Gas gauging enabled.        */
#define BQ40Z50R2_MFGSTAT_DSG_EN   (1U <<  2) /**< Discharge FET test active.  */
#define BQ40Z50R2_MFGSTAT_CHG_EN   (1U <<  1) /**< Charge FET test active.     */
#define BQ40Z50R2_MFGSTAT_PCHG_EN  (1U <<  0) /**< Precharge FET test active.  */

/**  @} */

/**
 * @defgroup BQ40Z50R2_DEF_DAS DAStatus block field counts (TRM sections 14.1.52 / 14.1.53)
 * @{
 *
 */

#define BQ40Z50R2_DASTATUS1_BYTES   (32U) /**< DAStatus1 payload size in bytes.  */
#define BQ40Z50R2_DASTATUS1_WORDS   (16U) /**< DAStatus1 payload size in words.  */
#define BQ40Z50R2_DASTATUS2_BYTES   (16U) /**< DAStatus2 payload size in bytes.  */
#define BQ40Z50R2_DASTATUS2_WORDS   (8U)  /**< DAStatus2 payload size in words.  */

/**  @} */

/**
 * @defgroup BQ40Z50R2_DEF_BATT BatteryMode() 0x03 bit masks (16-bit, TRM section 14.4)
 * @{
 *
 */

#define BQ40Z50R2_BATTMODE_CAPM     (1U << 15) /**< Capacity mode: 0 = mA/mAh, 1 = cW/cWh. */
#define BQ40Z50R2_BATTMODE_CHGM     (1U << 14) /**< Charger broadcasts disable.   */
#define BQ40Z50R2_BATTMODE_AM       (1U << 13) /**< Alarm broadcasts disable.     */
#define BQ40Z50R2_BATTMODE_PB       (1U <<  9) /**< Primary battery.              */
#define BQ40Z50R2_BATTMODE_CC       (1U <<  8) /**< Charge controller enabled.    */
#define BQ40Z50R2_BATTMODE_CF       (1U <<  7) /**< Condition flag.               */
#define BQ40Z50R2_BATTMODE_PBS      (1U <<  1) /**< Primary battery support.      */
#define BQ40Z50R2_BATTMODE_ICC      (1U <<  0) /**< Internal charge controller.   */

/**  @} */

/**
 * @defgroup BQ40Z50R2_DEF_BATTSTAT BatteryStatus() 0x16 bit masks (16-bit, TRM section 14.23)
 * @{
 *
 */

#define BQ40Z50R2_BATTSTAT_OCA      (1U << 15) /**< Overcharged Alarm.            */
#define BQ40Z50R2_BATTSTAT_TCA      (1U << 14) /**< Terminate Charge Alarm.       */
#define BQ40Z50R2_BATTSTAT_OTA      (1U << 12) /**< Overtemperature Alarm.        */
#define BQ40Z50R2_BATTSTAT_TDA      (1U << 11) /**< Terminate Discharge Alarm.    */
#define BQ40Z50R2_BATTSTAT_RCA      (1U <<  9) /**< Remaining Capacity Alarm.     */
#define BQ40Z50R2_BATTSTAT_RTA      (1U <<  8) /**< Remaining Time Alarm.         */
#define BQ40Z50R2_BATTSTAT_INIT     (1U <<  7) /**< Initialization complete.      */
#define BQ40Z50R2_BATTSTAT_DSG      (1U <<  6) /**< Discharging or Relax.         */
#define BQ40Z50R2_BATTSTAT_FC       (1U <<  5) /**< Fully Charged.                */
#define BQ40Z50R2_BATTSTAT_FD       (1U <<  4) /**< Fully Discharged.             */
#define BQ40Z50R2_BATTSTAT_EC_MASK  (0x000FU)  /**< Error Code field mask.        */
#define BQ40Z50R2_BATTSTAT_EC_SHIFT (0U)       /**< Error Code field shift.       */

/**  @} */

/* ==== Driver Types ==== */

/**
 * @defgroup BQ40Z50R2_TYPE Driver types
 * @{
 *
 */

/**
 * @brief Return/status codes produced by the driver.
 */
typedef enum
{
    BQ40Z50R2_OK = 0,            /**< Operation completed successfully.            */
    BQ40Z50R2_INVALID_ARG,       /**< A NULL pointer or out-of-range argument.     */
    BQ40Z50R2_NOT_INITIALIZED,   /**< Handle was not initialized.                  */
    BQ40Z50R2_COMM_ERROR,        /**< Underlying SMBus transfer failed.            */
    BQ40Z50R2_SIZE_ERROR,        /**< Block length out of range / buffer too small.*/
    BQ40Z50R2_MAC_MISMATCH       /**< Echoed MAC command did not match request.    */
} BQ40Z50R2_Status_t;

/**
 * @brief Direction tag for the user-supplied SMBus transfer callback.
 */
typedef enum
{
    BQ40Z50R2_DIR_WRITE = 0,     /**< Master transmits to the device.  */
    BQ40Z50R2_DIR_READ           /**< Master receives from the device. */
} BQ40Z50R2_Dir_t;

/**
 * @brief User-supplied SMBus byte-transfer callback.
 *
 * @details The driver builds complete SMBus frames (command byte, length byte
 *          for block transfers, data, etc.) inside @p buf and delegates the
 *          raw bus access to this callback. The callback must perform a
 *          single START / ... / STOP transaction with the slave at
 *          @p dev_addr (7-bit). For a repeated-start read the driver issues
 *          a separate write (the command/offset) followed by a read; the
 *          callback therefore only needs to support plain write and plain
 *          read transactions, exactly like HAL_I2C_Master_Transmit() and
 *          HAL_I2C_Master_Receive().
 *
 * @param[in]      ctx      Opaque user context (e.g. an I2C handle pointer).
 * @param[in]      dev_addr 7-bit slave address.
 * @param[in]      dir      Transfer direction (write or read).
 * @param[in,out]  buf      Data buffer (source for write, sink for read).
 * @param[in]      len      Number of bytes to transfer.
 *
 * @return 0 on success, non-zero on bus error.
 */
typedef int32_t (*BQ40Z50R2_Transfer_f)(void           *ctx,
                                         uint8_t         dev_addr,
                                         BQ40Z50R2_Dir_t dir,
                                         uint8_t        *buf,
                                         uint16_t        len);

/**
 * @brief Optional millisecond delay callback (may be NULL).
 *
 * @param[in] ctx     Opaque user context.
 * @param[in] delay_ms Delay duration in milliseconds.
 */
typedef void (*BQ40Z50R2_Delay_f)(void *ctx, uint32_t delay_ms);

/**
 * @brief Driver handle / context.
 *
 * @details The application allocates this structure statically and populates
 *          it through @ref BQ40Z50R2_Init. The @c work_buf member is a private
 *          scratch buffer used to assemble SMBus block frames, avoiding any
 *          dynamic allocation. The BQ40Z50-R2 can provide many data about the
 *          the cells and their charge status. These data can be stored in data
 *          block structures, but these have not been included
 *          in the handle structure not to complexify it unnecessarily. Only the
 *          useful data block structures can be used in the application code.
 */
typedef struct
{
    BQ40Z50R2_Transfer_f transfer;   /**< Mandatory SMBus transfer callback.   */
    BQ40Z50R2_Delay_f    delay;      /**< Optional delay callback (may be NULL).*/
    void                *ctx;        /**< Opaque user context passed to callbacks. */
    uint8_t              dev_addr;   /**< 7-bit SMBus device address.           */
    uint8_t              initialized;/**< Non-zero once successfully initialized.*/

    /** Private scratch buffer: command (1) + length (1) + 2-byte MAC echo
     *  + max block payload. Not to be accessed by the application. */
    uint8_t work_buf[BQ40Z50R2_MAX_BLOCK_LEN + BQ40Z50R2_MAC_CMD_LEN + 2U];


} BQ40Z50R2_Handle_t;

/**
 * @defgroup BQ40Z50R2_TYPE_STATUS Decoded status and gauging data structures
 * @{
 */

 /**
 * @brief Decoded FirmwareVersion block (TRM section 14.1.3, MAC 0x0002, 11 bytes).
 *
 * @details Returned as a block on ManufacturerBlockAccess() or ManufacturerData()
 *          in the format DDddVVvvBBbbTTZZzzRREE, where DDdd = device number,
 *          VVvv = version, BBbb = build number, TT = firmware type,
 *          ZZzz = Impedance Track version, RR/EE = reserved.
 */
typedef struct
{
    uint16_t device_number;  /**< DDdd – IC device number (e.g. 0x4050 for bq40z50). */
    uint16_t version;        /**< VVvv – Firmware version.                            */
    uint16_t build_number;   /**< BBbb – Firmware build number.                       */
    uint8_t  fw_type;        /**< TT   – Firmware type.                               */
    uint16_t it_version;     /**< ZZzz – Impedance Track algorithm version.           */
    uint8_t  reserved[2];    /**< RR, EE – Reserved bytes.                            */
} BQ40Z50R2_FirmwareVersion_t;

/**
 * @brief Decoded SafetyAlert() block (TRM section 14.1.36, MAC 0x0050, 32-bit).
 *
 * @details Returns the SafetyAlert() flags via ManufacturerBlockAccess() or
 *          ManufacturerData(). SafetyAlert differs from SafetyStatus in that
 *          it additionally carries two timeout-suspend flags (CTOS, PTOS) which
 *          do not appear in SafetyStatus. Each field is 1 when the corresponding
 *          condition is active/detected, 0 otherwise.
 */
typedef struct
{
    uint8_t ocdl;   /**< Bit 29 – Overcurrent in Discharge Latch.                  */
    uint8_t covl;   /**< Bit 28 – Cell Overvoltage Latch.                          */
    uint8_t utd;    /**< Bit 27 – Undertemperature During Discharge.               */
    uint8_t utc;    /**< Bit 26 – Undertemperature During Charge.                  */
    uint8_t pchgc;  /**< Bit 25 – Over-Precharge Current.                          */
    uint8_t chgv;   /**< Bit 24 – Overcharging Voltage.                            */
    uint8_t chgc;   /**< Bit 23 – Overcharging Current.                            */
    uint8_t oc;     /**< Bit 22 – Overcharge.                                      */
    uint8_t ctos;   /**< Bit 21 – Charge Timeout Suspend. Present in Alert only.   */
    uint8_t cto;    /**< Bit 20 – Charge Timeout.                                  */
    uint8_t ptos;   /**< Bit 19 – Precharge Timeout Suspend. Present in Alert only.*/
    uint8_t pto;    /**< Bit 18 – Precharge Timeout.                               */
    uint8_t otf;    /**< Bit 16 – Overtemperature FET.                             */
    uint8_t cuvc;   /**< Bit 14 – Cell Undervoltage Compensated.                   */
    uint8_t otd;    /**< Bit 13 – Overtemperature During Discharge.                */
    uint8_t otc;    /**< Bit 12 – Overtemperature During Charge.                   */
    uint8_t ascdl;  /**< Bit 11 – Short-Circuit During Discharge Latch.            */
    uint8_t ascd;   /**< Bit 10 – Short-Circuit During Discharge.                  */
    uint8_t asccl;  /**< Bit  9 – Short-Circuit During Charge Latch.               */
    uint8_t ascc;   /**< Bit  8 – Short-Circuit During Charge.                     */
    uint8_t aoldl;  /**< Bit  7 – Overload During Discharge Latch.                 */
    uint8_t aold;   /**< Bit  6 – Overload During Discharge.                       */
    uint8_t ocd2;   /**< Bit  5 – Overcurrent During Discharge 2.                  */
    uint8_t ocd1;   /**< Bit  4 – Overcurrent During Discharge 1.                  */
    uint8_t occ2;   /**< Bit  3 – Overcurrent During Charge 2.                     */
    uint8_t occ1;   /**< Bit  2 – Overcurrent During Charge 1.                     */
    uint8_t cov;    /**< Bit  1 – Cell Overvoltage.                                */
    uint8_t cuv;    /**< Bit  0 – Cell Undervoltage.                               */
} BQ40Z50R2_SafetyAlert_t;


/**
 * @brief Decoded SafetyStatus() block (TRM sections 14.1.37, MAC 0x0051, 32-bit).
 *
 * @details SafetyStatus (0x0051) populate from the 4-byte block returned by the MAC command.
 *          Each field is 1 when the corresponding condition is active/detected,
 *          0 otherwise.
 */
typedef struct
{
    uint8_t ocdl;   /**< Bit 29 – Overcurrent in Discharge Latch.          */
    uint8_t covl;   /**< Bit 28 – Cell Overvoltage Latch.                  */
    uint8_t utd;    /**< Bit 27 – Undertemperature During Discharge.        */
    uint8_t utc;    /**< Bit 26 – Undertemperature During Charge.           */
    uint8_t pchgc;  /**< Bit 25 – Over-Precharge Current.                  */
    uint8_t chgv;   /**< Bit 24 – Overcharging Voltage.                    */
    uint8_t chgc;   /**< Bit 23 – Overcharging Current.                    */
    uint8_t oc;     /**< Bit 22 – Overcharge.                              */
    uint8_t cto;    /**< Bit 20 – Charge Timeout.                          */
    uint8_t pto;    /**< Bit 18 – Precharge Timeout.                       */
    uint8_t otf;    /**< Bit 16 – Overtemperature FET.                     */
    uint8_t cuvc;   /**< Bit 14 – Cell Undervoltage Compensated.           */
    uint8_t otd;    /**< Bit 13 – Overtemperature During Discharge.        */
    uint8_t otc;    /**< Bit 12 – Overtemperature During Charge.           */
    uint8_t ascdl;  /**< Bit 11 – Short-Circuit During Discharge Latch.    */
    uint8_t ascd;   /**< Bit 10 – Short-Circuit During Discharge.          */
    uint8_t asccl;  /**< Bit  9 – Short-Circuit During Charge Latch.       */
    uint8_t ascc;   /**< Bit  8 – Short-Circuit During Charge.             */
    uint8_t aoldl;  /**< Bit  7 – Overload During Discharge Latch.         */
    uint8_t aold;   /**< Bit  6 – Overload During Discharge.               */
    uint8_t ocd2;   /**< Bit  5 – Overcurrent During Discharge 2.          */
    uint8_t ocd1;   /**< Bit  4 – Overcurrent During Discharge 1.          */
    uint8_t occ2;   /**< Bit  3 – Overcurrent During Charge 2.             */
    uint8_t occ1;   /**< Bit  2 – Overcurrent During Charge 1.             */
    uint8_t cov;    /**< Bit  1 – Cell Overvoltage.                        */
    uint8_t cuv;    /**< Bit  0 – Cell Undervoltage.                       */
} BQ40Z50R2_SafetyStatus_t;

/**
 * @brief Decoded PFAlert() / PFStatus() block (TRM sections 14.1.38–14.1.39, 32-bit).
 *
 * @details Both PFAlert (0x0052) and PFStatus (0x0053) share the same bit
 *          layout. Populate from the 4-byte block returned by the MAC command.
 *          Each field is 1 when the permanent failure is detected, 0 otherwise.
 */
typedef struct
{
    uint8_t ts4;    /**< Bit 31 – Open Thermistor TS4 Failure.             */
    uint8_t ts3;    /**< Bit 30 – Open Thermistor TS3 Failure.             */
    uint8_t ts2;    /**< Bit 29 – Open Thermistor TS2 Failure.             */
    uint8_t ts1;    /**< Bit 28 – Open Thermistor TS1 Failure.             */
    uint8_t lvl2;   /**< Bit 22 – Second Level Protector Failure.          */
    uint8_t afec;   /**< Bit 21 – AFE Communication Failure.               */
    uint8_t afer;   /**< Bit 20 – AFE Register Failure.                    */
    uint8_t fuse;   /**< Bit 19 – Chemical Fuse Failure.                   */
    uint8_t ocdl;   /**< Bit 18 – Overcurrent in Discharge Failure.        */
    uint8_t dfetf;  /**< Bit 17 – Discharge FET Failure.                   */
    uint8_t cfetf;  /**< Bit 16 – Charge FET Failure.                      */
    uint8_t ascdl;  /**< Bit 15 – Short Circuit in Discharge Failure.      */
    uint8_t asccl;  /**< Bit 14 – Short Circuit in Charge Failure.         */
    uint8_t aoldl;  /**< Bit 13 – Overload in Discharge Failure.           */
    uint8_t vima;   /**< Bit 12 – Voltage Imbalance While Pack Active.     */
    uint8_t vimr;   /**< Bit 11 – Voltage Imbalance While Pack At Rest.    */
    uint8_t cd;     /**< Bit 10 – Capacity Degradation Failure.            */
    uint8_t imp;    /**< Bit  9 – Impedance Failure.                       */
    uint8_t cb;     /**< Bit  8 – Cell Balancing Failure.                  */
    uint8_t qim;    /**< Bit  7 – QMax Imbalance Failure.                  */
    uint8_t sotf;   /**< Bit  6 – Safety Overtemperature FET Failure.      */
    uint8_t covl;   /**< Bit  5 – Cell Overvoltage Latch Failure.          */
    uint8_t sot;    /**< Bit  4 – Safety Overtemperature Cell Failure.     */
    uint8_t socd;   /**< Bit  3 – Safety Overcurrent in Discharge Failure. */
    uint8_t socc;   /**< Bit  2 – Safety Overcurrent in Charge Failure.    */
    uint8_t sov;    /**< Bit  1 – Safety Cell Overvoltage Failure.         */
    uint8_t suv;    /**< Bit  0 – Safety Cell Undervoltage Failure.        */
} BQ40Z50R2_PFStatus_t;

/**
 * @brief Decoded OperationStatus() block (TRM section 14.1.40, MAC 0x0054, 32-bit).
 *
 * @details Populate from the 4-byte block returned by the MAC OperationStatus
 *          command. Fields are 1 when the described condition is active, 0 otherwise,
 *          except for @p sec which encodes the security mode as a 2-bit value.
 */
typedef struct
{
    uint8_t emshut;     /**< Bit 29 – Emergency FET Shutdown active.            */
    uint8_t cb;         /**< Bit 28 – Cell balancing active.                    */
    uint8_t slpcc;      /**< Bit 27 – CC measurement running in SLEEP mode.     */
    uint8_t slpad;      /**< Bit 26 – ADC measurement running in SLEEP mode.    */
    uint8_t smblcal;    /**< Bit 25 – Auto CC calibration triggered (bus low).  */
    uint8_t init;       /**< Bit 24 – Initialization after full reset active.   */
    uint8_t sleepm;     /**< Bit 23 – SLEEP mode triggered via command.         */
    uint8_t xl;         /**< Bit 22 – 400-kHz SMBus mode active.               */
    uint8_t cal_offset; /**< Bit 21 – Raw shorted CC offset data available.     */
    uint8_t cal;        /**< Bit 20 – Raw ADC/CC calibration data available.    */
    uint8_t autocalm;   /**< Bit 19 – Auto CC Offset calibration in progress.   */
    uint8_t auth;       /**< Bit 18 – Authentication in progress.               */
    uint8_t led;        /**< Bit 17 – LED display is on.                        */
    uint8_t sdm;        /**< Bit 16 – Shutdown triggered via command.           */
    uint8_t sleep;      /**< Bit 15 – SLEEP mode conditions met.                */
    uint8_t xchg;       /**< Bit 14 – Charging disabled.                        */
    uint8_t xdsg;       /**< Bit 13 – Discharging disabled.                     */
    uint8_t pf;         /**< Bit 12 – PERMANENT FAILURE mode active.            */
    uint8_t ss;         /**< Bit 11 – SAFETY status (OR of all SafetyStatus).   */
    uint8_t sdv;        /**< Bit 10 – Shutdown triggered by low pack voltage.   */
    uint8_t sec;        /**< Bits 9–8 – SECURITY mode: 0b01=Full, 0b10=Unsealed, 0b11=Sealed. */
    uint8_t btp_int;    /**< Bit  7 – Battery Trip Point interrupt active.      */
    uint8_t fuse;       /**< Bit  5 – Fuse status active.                       */
    uint8_t pchg;       /**< Bit  3 – Precharge FET on.                         */
    uint8_t chg;        /**< Bit  2 – CHG FET on.                               */
    uint8_t dsg;        /**< Bit  1 – DSG FET on.                               */
    uint8_t pres;       /**< Bit  0 – System present (active-low PRES pin low). */
} BQ40Z50R2_OperationStatus_t;

/**
 * @brief Decoded ChargingStatus() block (TRM section 14.1.41, MAC 0x0055, 24-bit).
 *
 * @details Populate from the 3-byte block returned by the MAC ChargingStatus
 *          command. Bits 23–8 carry charging status flags; bits 7–0 carry
 *          temperature-range flags. Each field is 1 when active, 0 otherwise.
 */
typedef struct
{
    /* Charging Status Flags (bits 23–8) */
    uint8_t nct;    /**< Bit 19 – Near Charge Termination (within ~40 s).  */
    uint8_t ccc;    /**< Bit 18 – Charging Loss Compensation active.        */
    uint8_t cvr;    /**< Bit 17 – Charging Voltage Rate of Change active.   */
    uint8_t ccr;    /**< Bit 16 – Charging Current Rate of Change active.   */
    uint8_t vct;    /**< Bit 15 – Charge Termination detected.              */
    uint8_t mchg;   /**< Bit 14 – Maintenance Charge active.                */
    uint8_t su;     /**< Bit 13 – Suspend Charge active.                    */
    uint8_t in;     /**< Bit 12 – Charge Inhibit active.                    */
    uint8_t hv;     /**< Bit 11 – High Voltage Region.                      */
    uint8_t mv;     /**< Bit 10 – Mid Voltage Region.                       */
    uint8_t lv;     /**< Bit  9 – Low Voltage Region.                       */
    uint8_t pv;     /**< Bit  8 – Precharge Voltage Region.                 */

    /* Temperature Range Flags (bits 7–0) */
    uint8_t ot;     /**< Bit  6 – Overtemperature Region.                   */
    uint8_t ht;     /**< Bit  5 – High Temperature Region.                  */
    uint8_t sth;    /**< Bit  4 – Standard Temperature High Region.         */
    uint8_t rt;     /**< Bit  3 – Recommended Temperature Region.           */
    uint8_t stl;    /**< Bit  2 – Standard Temperature Low Region.          */
    uint8_t lt;     /**< Bit  1 – Low Temperature Region.                   */
    uint8_t ut;     /**< Bit  0 – Undertemperature Region.                  */
} BQ40Z50R2_ChargingStatus_t;

/**
 * @brief Decoded GaugingStatus() block (TRM section 14.1.42, MAC 0x0056, 24-bit).
 *
 * @details Populate from the 3-byte block returned by the MAC GaugingStatus
 *          command. Fields are 1 when active/detected, 0 otherwise, except
 *          @p rx and @p qmax which toggle on every corresponding update event.
 */
typedef struct
{
    uint8_t ocvfr;   /**< Bit 20 – OCV in Flat Region detected (during RELAX). */
    uint8_t ldmd;    /**< Bit 19 – Load mode: 1 = Constant Power, 0 = Constant Current. */
    uint8_t rx;      /**< Bit 18 – Resistance Update toggle (flips each Ra update).    */
    uint8_t qmax;    /**< Bit 17 – QMax Update toggle (flips each QMax update).        */
    uint8_t vdq;     /**< Bit 16 – Discharge Qualified for Learning (opposite of R_DIS). */
    uint8_t nsfm;    /**< Bit 15 – Negative Scale Factor Mode detected.              */
    uint8_t slpqmax; /**< Bit 13 – OCV update in SLEEP mode in progress.             */
    uint8_t qen;     /**< Bit 12 – Impedance Track Gauging (Ra and QMax updates) enabled. */
    uint8_t vok;     /**< Bit 11 – Voltages OK for QMax update (DOD saved for next update). */
    uint8_t r_dis;   /**< Bit 10 – Resistance Updates disabled.                      */
    uint8_t rest;    /**< Bit  8 – OCV Reading taken (in RELAX).                     */
    uint8_t cf;      /**< Bit  7 – Condition Flag: MaxError() > Max Error limit.      */
    uint8_t dsg;     /**< Bit  6 – Discharge/Relax (charging not detected).           */
    uint8_t edv;     /**< Bit  5 – End-of-Discharge Termination Voltage reached.      */
    uint8_t bal_en;  /**< Bit  4 – Cell Balancing possible (if enabled in config).    */
    uint8_t tc;      /**< Bit  3 – Terminate Charge detected.                         */
    uint8_t td;      /**< Bit  2 – Terminate Discharge detected.                      */
    uint8_t fc;      /**< Bit  1 – Fully Charged.                                     */
    uint8_t fd;      /**< Bit  0 – Fully Discharged.                                  */
} BQ40Z50R2_GaugingStatus_t;

/**
 * @brief Decoded ManufacturingStatus() block (TRM section 14.1.43, MAC 0x0057, 16-bit).
 *
 * @details Populate from the 2-byte block returned by the MAC ManufacturingStatus
 *          command. Each field is 1 when the feature is enabled / the test is
 *          active, 0 otherwise.
 */
typedef struct
{
    uint8_t cal_test;  /**< Bit 15 – CALIBRATION mode enabled.                   */
    uint8_t lt_test;   /**< Bit 14 – LIFETIME SPEED UP mode enabled.             */
    uint8_t led_en;    /**< Bit  9 – LED display enabled via push button.         */
    uint8_t fuse_en;   /**< Bit  8 – Fuse action enabled.                         */
    uint8_t bbr_en;    /**< Bit  7 – Black Box Recorder enabled.                  */
    uint8_t pf_en;     /**< Bit  6 – Permanent Failure enabled.                   */
    uint8_t lf_en;     /**< Bit  5 – Lifetime Data Collection enabled.            */
    uint8_t fet_en;    /**< Bit  4 – All FET Action enabled (overrides FW ctrl).  */
    uint8_t gauge_en;  /**< Bit  3 – Gas Gauging enabled.                         */
    uint8_t dsg_en;    /**< Bit  2 – Discharge FET test activated.               */
    uint8_t chg_en;    /**< Bit  1 – Charge FET test activated.                   */
    uint8_t pchg_en;   /**< Bit  0 – Precharge FET test activated.                */
} BQ40Z50R2_ManufacturingStatus_t;

/**
 * @brief Decoded AFERegister() block (TRM section 14.1.44, MAC 0x0058, 21 bytes).
 *
 * @details Raw AFE hardware register snapshot. Intended for internal debug use
 *          only. Each byte maps directly to the corresponding AFE register;
 *          interpretation is device-specific.
 */
typedef struct
{
    uint8_t int_status;   /**< AA – AFE Interrupt Status.          */
    uint8_t fet_status;   /**< BB – AFE FET Status.                */
    uint8_t rxin;         /**< CC – AFE RXIN I/O port input.       */
    uint8_t latch_status; /**< DD – AFE Latch Status.              */
    uint8_t int_enable;   /**< EE – AFE Interrupt Enable.          */
    uint8_t control;      /**< FF – AFE Control / FET enable.      */
    uint8_t rxien;        /**< GG – AFE RXIEN I/O input enable.    */
    uint8_t rlout;        /**< HH – AFE RLOUT I/O output status.   */
    uint8_t rhout;        /**< II – AFE RHOUT I/O output status.   */
    uint8_t rhint;        /**< JJ – AFE RHINT I/O interrupt status.*/
    uint8_t cell_bal;     /**< KK – AFE Cell Balance enable/status.*/
    uint8_t adc_cc;       /**< LL – AFE ADC/CC Control.            */
    uint8_t adc_mux;      /**< MM – AFE ADC Mux Control.           */
    uint8_t led;          /**< NN – AFE LED Control.               */
    uint8_t hw_ctrl;      /**< OO – AFE Control (HW features).     */
    uint8_t timer;        /**< PP – AFE Timer Control.             */
    uint8_t protection;   /**< QQ – AFE Protection delay control.  */
    uint8_t ocd;          /**< RR – AFE OCD Settings.              */
    uint8_t scc;          /**< SS – AFE SCC Settings.              */
    uint8_t scd1;         /**< TT – AFE SCD1 Settings.             */
    uint8_t scd2;         /**< UU – AFE SCD2 Settings.             */
} BQ40Z50R2_AFERegister_t;

/**
 * @brief Decoded Lifetime Data Block 1 (TRM section 14.1.46, MAC 0x0060, 32 bytes).
 *
 * @details Cell voltages and delta in mV (int16_t, I2 type in data flash).
 *          Charge/discharge currents and average discharge power in mA / cW
 *          (int16_t, I2). Temperatures are signed single-byte values in °C (int8_t, I1).
 */
typedef struct
{
    int16_t cell_max_mv[4];     /**< Cell 1–4 maximum recorded voltage (mV).        */
    int16_t cell_min_mv[4];     /**< Cell 1–4 minimum recorded voltage (mV).        */
    int16_t max_delta_cell_mv;  /**< Maximum recorded cell voltage imbalance (mV).  */
    int16_t max_chg_curr_ma;    /**< Maximum reported charge current (mA).          */
    int16_t max_dsg_curr_ma;    /**< Maximum reported discharge current (mA).       */
    int16_t max_avg_dsg_curr_ma;/**< Maximum average discharge current (mA).        */
    int16_t max_avg_dsg_pwr_cw; /**< Maximum average discharge power (cW).          */
    int8_t  max_temp_cell_c;    /**< Maximum reported cell temperature (°C).        */
    int8_t  min_temp_cell_c;    /**< Minimum reported cell temperature (°C).        */
    int8_t  max_delta_cell_temp_c; /**< Maximum recorded cell temperature delta (°C). */
    int8_t  max_temp_int_c;     /**< Maximum reported internal sensor temperature (°C). */
    int8_t  min_temp_int_c;     /**< Minimum reported internal sensor temperature (°C). */
    int8_t  max_temp_fet_c;     /**< Maximum reported FET temperature (°C).         */
} BQ40Z50R2_LifetimeDataBlock1_t;

/**
 * @brief Decoded Lifetime Data Block 2 (TRM section 14.1.47, MAC 0x0061, 8 bytes).
 *
 * @details Reset counters (U1, single unsigned byte each) and per-cell
 *          balancing time accumulators (U1, unit: 2 h per LSB).
 */
typedef struct
{
    uint8_t nb_shutdowns;    /**< AA – Number of shutdown events.             */
    uint8_t nb_partial_rst;  /**< BB – Number of partial reset events.        */
    uint8_t nb_full_rst;     /**< CC – Number of full reset events.           */
    uint8_t nb_wdt_rst;      /**< DD – Number of watchdog timer reset events. */
    uint8_t cb_time_cell[4]; /**< EE–HH – Cumulative cell-balance bypass time
                              *           per cell (unit: 2 h per LSB).       */
} BQ40Z50R2_LifetimeDataBlock2_t;

/**
 * @brief Decoded Lifetime Data Block 3 (TRM section 14.1.48, MAC 0x0062, 16 bytes).
 *
 * @details Firmware runtime and time-in-temperature-zone counters. All fields
 *          are unsigned 16-bit words (U2) with a resolution of 2 hours per LSB.
 */
typedef struct
{
    uint16_t total_fw_runtime; /**< AAaa – Total firmware runtime (2 h/LSB).          */
    uint16_t time_ut;          /**< BBbb – Time spent in Undertemperature (2 h/LSB).  */
    uint16_t time_lt;          /**< CCcc – Time spent in Low Temperature (2 h/LSB).   */
    uint16_t time_stl;         /**< DDdd – Time spent in Std Temp Low (2 h/LSB).      */
    uint16_t time_rt;          /**< EEee – Time spent in Recommended Temp (2 h/LSB).  */
    uint16_t time_sth;         /**< FFff – Time spent in Std Temp High (2 h/LSB).     */
    uint16_t time_ht;          /**< GGgg – Time spent in High Temperature (2 h/LSB).  */
    uint16_t time_ot;          /**< HHhh – Time spent in Overtemperature (2 h/LSB).   */
} BQ40Z50R2_LifetimeDataBlock3_t;

/**
 * @brief Decoded Lifetime Data Block 4 (TRM section 14.1.49, MAC 0x0063, 32 bytes).
 *
 * @details Protection event counters and timestamps. Counts are unsigned 16-bit
 *          words; "last event" timestamps are device-internal time references
 *          stored as unsigned 16-bit words.
 */
typedef struct
{
    uint16_t nb_cov;    /**< AAaa – Number of Cell Overvoltage events.            */
    uint16_t last_cov;  /**< BBbb – Timestamp of last COV event.                  */
    uint16_t nb_cuv;    /**< CCcc – Number of Cell Undervoltage events.           */
    uint16_t last_cuv;  /**< DDdd – Timestamp of last CUV event.                  */
    uint16_t nb_ocd1;   /**< EEee – Number of OCD1 events.                        */
    uint16_t last_ocd1; /**< FFff – Timestamp of last OCD1 event.                 */
    uint16_t nb_ocd2;   /**< GGgg – Number of OCD2 events.                        */
    uint16_t last_ocd2; /**< HHhh – Timestamp of last OCD2 event.                 */
    uint16_t nb_occ1;   /**< IIii – Number of OCC1 events.                        */
    uint16_t last_occ1; /**< JJjj – Timestamp of last OCC1 event.                 */
    uint16_t nb_occ2;   /**< KKkk – Number of OCC2 events.                        */
    uint16_t last_occ2; /**< LLll – Timestamp of last OCC2 event.                 */
    uint16_t nb_aold;   /**< MMmm – Number of Overload During Discharge events.   */
    uint16_t last_aold; /**< NNnn – Timestamp of last AOLD event.                 */
    uint16_t nb_ascd;   /**< OOoo – Number of Short-Circuit During Discharge events. */
    uint16_t last_ascd; /**< PPpp – Timestamp of last ASCD event.                 */
} BQ40Z50R2_LifetimeDataBlock4_t;

/**
 * @brief Decoded Lifetime Data Block 5 (TRM section 14.1.50, MAC 0x0064, 32 bytes).
 *
 * @details Protection event counters and timestamps continued, plus gauging
 *          update counters. Same encoding as Block 4 (U2 words throughout).
 */
typedef struct
{
    uint16_t nb_ascc;        /**< AAaa – Number of Short-Circuit During Charge events. */
    uint16_t last_ascc;      /**< BBbb – Timestamp of last ASCC event.               */
    uint16_t nb_otc;         /**< CCcc – Number of Overtemperature During Charge events. */
    uint16_t last_otc;       /**< DDdd – Timestamp of last OTC event.                */
    uint16_t nb_otd;         /**< EEee – Number of Overtemperature During Discharge events. */
    uint16_t last_otd;       /**< FFff – Timestamp of last OTD event.                */
    uint16_t nb_otf;         /**< GGgg – Number of Overtemperature FET events.       */
    uint16_t last_otf;       /**< HHhh – Timestamp of last OTF event.               */
    uint16_t nb_valid_chgterm;  /**< IIii – Number of Valid Charge Terminations.     */
    uint16_t last_valid_chgterm;/**< JJjj – Timestamp of last valid charge term.     */
    uint16_t nb_qmax_updates;   /**< KKkk – Number of QMax updates.                 */
    uint16_t last_qmax_update;  /**< LLll – Timestamp of last QMax update.          */
    uint16_t nb_ra_updates;     /**< MMmm – Number of Ra table updates.             */
    uint16_t last_ra_update;    /**< NNnn – Timestamp of last Ra update.            */
    uint16_t nb_ra_disable;     /**< OOoo – Number of Ra disable events.            */
    uint16_t last_ra_disable;   /**< PPpp – Timestamp of last Ra disable event.     */
} BQ40Z50R2_LifetimeDataBlock5_t;

/**
 * @brief Decoded ManufacturerInfo block (TRM section 14.1.51, MAC 0x0070, 32 bytes).
 *
 * @details Raw 32-byte user-defined manufacturer information string stored in
 *          data flash (subclass Manufacturer Data). The content and encoding
 *          are entirely application-defined; the block is returned verbatim.
 */
typedef struct
{
    uint8_t data[32]; /**< Raw manufacturer info payload (application-defined). */
} BQ40Z50R2_ManufacturerInfo_t;

/**
 * @brief Decoded DAStatus1 block (TRM section 14.1.52). All voltages in mV,
 *        currents in mA, powers in cW.
 */
typedef struct
{
    uint16_t cell_voltage_mv[4]; /**< Cell 1..4 voltages (mV).               */
    uint16_t bat_voltage_mv;     /**< Voltage at the BAT pin (mV).           */
    uint16_t pack_voltage_mv;    /**< PACK voltage (mV).                     */
    int16_t  cell_current_ma[4]; /**< Simultaneous cell 1..4 currents (mA).  */
    int16_t  cell_power[4];      /**< Cell 1..4 power (cW).                  */
    int16_t  power_cw;           /**< Voltage() x Current() power (cW).      */
    int16_t  avg_power_cw;       /**< Average power (cW).                    */
} BQ40Z50R2_DAStatus1_t;

/**
 * @brief Decoded DAStatus2 block (TRM section 14.1.53). All temperatures in
 *        units of 0.1 K.
 */
typedef struct
{
    uint16_t int_temp_dk;     /**< Internal sensor temperature (0.1 K).  */
    uint16_t ts_temp_dk[4];   /**< TS1..TS4 temperatures (0.1 K).        */
    uint16_t cell_temp_dk;    /**< Cell temperature (0.1 K).             */
    uint16_t fet_temp_dk;     /**< FET temperature (0.1 K).              */
    uint16_t gauge_temp_dk;   /**< Gauging temperature (0.1 K).          */
} BQ40Z50R2_DAStatus2_t;

/**
 * @brief Decoded GaugeStatus1 block (TRM section 14.1.54, MAC 0x0073, 32 bytes).
 *
 * @details Impedance Track simulation results: true remaining/full-charge
 *          capacity and energy (before smoothing), simulation temperature,
 *          Ra table scaling factors, and compensated resistances for each cell.
 *          Capacity values are signed (can be negative or exceed FCC).
 */
typedef struct
{
    int16_t  true_rem_q_mah;       /**< AAaa – True remaining capacity from IT sim (mAh). */
    int16_t  true_rem_e_cwh;       /**< BBbb – True remaining energy from IT sim (cWh).   */
    int16_t  initial_q_mah;        /**< CCcc – Initial capacity from IT simulation (mAh). */
    int16_t  initial_e_cwh;        /**< DDdd – Initial energy from IT simulation (cWh).   */
    int16_t  true_fcc_q_mah;       /**< EEee – True FCC from IT sim, no smoothing (mAh).  */
    int16_t  true_fcc_e_cwh;       /**< FFff – True FCC energy from IT sim (cWh).         */
    uint16_t t_sim_dk;             /**< GGgg – Temperature during last IT simulation (0.1 K). */
    uint16_t t_ambient_dk;         /**< HHhh – Assumed ambient temperature for IT algorithm (0.1 K). */
    uint16_t ra_scale[4];          /**< IIii–LLll – Ra table scaling factor for Cell 1–4. */
    uint16_t comp_res[4];          /**< MMmm–PPpp – Compensated resistance of Cell 1–4 (2^-10 Ω). */
} BQ40Z50R2_GaugeStatus1_t;

/**
 * @brief Decoded GaugeStatus2 block (TRM section 14.1.55, MAC 0x0074, 32 bytes).
 *
 * @details Impedance Track runtime tracking data: pack/cell grid points,
 *          state machine timing, depth-of-discharge at start (DOD0) and at
 *          end-of-charge (DODEOC) for each cell.
 */
typedef struct
{
    uint8_t  pack_grid;         /**< AA   – Active pack grid point (min of CellGrid 0–3). Valid only in DISCHARGE when R_DIS=0. */
    uint8_t  l_status;          /**< BB   – Learned status byte (QMax/CF/ITEN flags, see TRM 14.55). */
    uint8_t  cell_grid[4];      /**< CC–FF – Active grid point for Cell 1–4. Valid only in DISCHARGE when R_DIS=0. */
    uint32_t state_time_s;      /**< GGggHHhh – Time since last state change (DISCHARGE/CHARGE/REST) (s). */
    uint16_t dod0[4];           /**< IIii–LLll – DOD0 depth-of-discharge for Cell 1–4. */
    int16_t  dod0_passed_q_mah; /**< MMmm – Passed capacity since last DOD0 update (mAh). */
    int16_t  dod0_passed_e_cwh; /**< NNnn – Passed energy since last DOD0 update (cWh).   */
    uint16_t dod0_time_hr16;    /**< OOoo – Time since last DOD0 update (hr/16).           */
    uint16_t dodeoc[4];         /**< PPpp–SSss – DODEOC depth of discharge at end of charge for Cell 1–4. */
} BQ40Z50R2_GaugeStatus2_t;

/**
 * @brief Decoded GaugeStatus3 block (TRM section 14.1.56, MAC 0x0075, 24 bytes).
 *
 * @details QMax values for each cell, the saved DOD0 used for the next QMax
 *          update, passed capacity/time since last save, and thermal model
 *          coefficients.
 */
typedef struct
{
    uint16_t qmax_mah[4];        /**< AAaa–DDdd – QMax for Cell 1–4 (mAh).                  */
    uint16_t qmax_dod0[4];       /**< EEee–HHhh – DOD0 saved for next QMax update, Cell 1–4.
                                  *   Valid only when GaugingStatus()[VOK] = 1.              */
    int16_t  qmax_passed_q_mah;  /**< IIii – Passed capacity since last QMax DOD save (mAh). */
    uint16_t qmax_time_hr16;     /**< JJjj – Time since last QMax DOD save (hr/16).          */
    uint16_t temp_k;             /**< KKkk – Thermal Model temperature factor.               */
    uint16_t temp_a;             /**< LLll – Thermal Model temperature.                      */
} BQ40Z50R2_GaugeStatus3_t;

/**
 * @brief Decoded CBStatus block (TRM section 14.57, MAC 0x0076, 18 bytes).
 *
 * @details Cell balancing time calculated for each cell, the depth-of-discharge
 *          target used for balancing, and the total DOD charge reference.
 */
typedef struct
{
    uint16_t cb_time_s[4];   /**< AAaa–DDdd – Calculated cell balancing time for Cell 1–4 (s). */
    uint16_t cb_dod[4];      /**< EEee–HHhh – Cell balance DOD target for Cell 1–4.            */
    uint16_t total_dod_chg;  /**< IIii – Total DOD Charge reference.                           */
} BQ40Z50R2_CBStatus_t;

/**
 * @brief Decoded State-of-Health block (TRM section 14.1.58, MAC 0x0077, 4 bytes).
 *
 * @details Returns the state-of-health full-charge capacity and energy,
 *          reflecting long-term battery ageing relative to design capacity.
 */
typedef struct
{
    uint16_t soh_fcc_mah;  /**< AAaa – State-of-Health full charge capacity (mAh). */
    uint16_t soh_energy_cwh;/**< BBbb – State-of-Health energy (cWh).               */
} BQ40Z50R2_StateOfHealth_t;

/**
 * @brief Decoded FilterCapacity block (TRM section 14.1.59, MAC 0x0078, 8 bytes).
 *
 * @details Filtered remaining capacity and full-charge capacity, reported even
 *          when the [SMOOTH] flag is 0. Useful to observe raw IT output before
 *          any smoothing algorithm is applied.
 */
typedef struct
{
    uint16_t filtered_rem_mah;  /**< AAaa – Filtered remaining capacity (mAh).       */
    uint16_t filtered_rem_cwh;  /**< BBbb – Filtered remaining energy (cWh).          */
    uint16_t filtered_fcc_mah;  /**< CCcc – Filtered full charge capacity (mAh).      */
    uint16_t filtered_fcc_cwh;  /**< DDdd – Filtered full charge energy (cWh).         */
} BQ40Z50R2_FilterCapacity_t;

/**
 * @brief Decoded ManufacturerInfoB block (TRM section 14.1.61, MAC 0x007A, 4 bytes).
 *
 * @details Secondary manufacturer information block returned in the format
 *          AABBCCDD. Content and encoding are application-defined.
 */
typedef struct
{
    uint8_t data[4]; /**< Raw secondary manufacturer info payload (application-defined). */
} BQ40Z50R2_ManufacturerInfoB_t;

/** @} */

/**  @} */

/* ==== Functions prototypes ==== */

/**
 * @defgroup BQ40Z50R2_FUNC Functions prototypes
 * @{
 *
 */

 /**
 * @defgroup BQ40Z50R2_FUNC_INIT Initialization
 * @{
 *
 */

/**
 * @brief Initialize a driver handle.
 *
 * @param[out] handle   Handle to initialize (must not be NULL).
 * @param[in]  transfer Mandatory SMBus transfer callback (must not be NULL).
 * @param[in]  delay    Optional delay callback (may be NULL).
 * @param[in]  ctx      Opaque context forwarded to the callbacks (may be NULL).
 * @param[in]  dev_addr 7-bit device address (use
 *                      @ref BQ40Z50R2_DEFAULT_ADDR_7BIT if unsure).
 *
 * @return BQ40Z50R2_OK on success, BQ40Z50R2_INVALID_ARG on bad arguments.
 */
BQ40Z50R2_Status_t BQ40Z50R2_Init(BQ40Z50R2_Handle_t  *handle,
                                  BQ40Z50R2_Transfer_f transfer,
                                  BQ40Z50R2_Delay_f    delay,
                                  void                *ctx,
                                  uint8_t              dev_addr);

/**  @} */

 /**
 * @defgroup BQ40Z50R2_FUNC_SMB Low-level SMBus primitives
 * @{
 *
 */

/**
 * @brief SMBus write-word transaction.
 *
 * @param[in] handle Initialized handle.
 * @param[in] cmd    SMBus command code.
 * @param[in] value  16-bit value (sent little-endian on the bus).
 *
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_WriteWord(BQ40Z50R2_Handle_t *handle,
                                       uint8_t             cmd,
                                       uint16_t            value);

/**
 * @brief SMBus read-word transaction.
 *
 * @param[in]  handle Initialized handle.
 * @param[in]  cmd    SMBus command code.
 * @param[out] value  Destination for the 16-bit value (host endianness).
 *
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_ReadWord(BQ40Z50R2_Handle_t *handle,
                                      uint8_t             cmd,
                                      uint16_t           *value);

/**
 * @brief SMBus read-block transaction.
 *
 * @details Reads the SMBus length byte followed by the payload. The number of
 *          payload bytes actually returned is written to @p out_len.
 *
 * @param[in]  handle  Initialized handle.
 * @param[in]  cmd     SMBus command code.
 * @param[out] data    Destination buffer for the payload.
 * @param[in]  max_len Capacity of @p data in bytes.
 * @param[out] out_len Number of payload bytes returned (may be NULL).
 *
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_ReadBlock(BQ40Z50R2_Handle_t *handle,
                                       uint8_t             cmd,
                                       uint8_t            *data,
                                       uint8_t             max_len,
                                       uint8_t            *out_len);

/**
 * @brief SMBus write-block transaction.
 *
 * @param[in] handle Initialized handle.
 * @param[in] cmd    SMBus command code.
 * @param[in] data   Payload to write.
 * @param[in] len    Payload length in bytes (1..BQ40Z50R2_MAX_BLOCK_LEN).
 *
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_WriteBlock(BQ40Z50R2_Handle_t *handle,
                                        uint8_t             cmd,
                                        const uint8_t      *data,
                                        uint8_t             len);

/**  @} */

 /**
 * @defgroup BQ40Z50R2_FUNC_MAC Manufacturer Access System (MAC) helpers
 * @{
 *
 */

/**
 * @brief Issue a MAC sub-command that takes no data payload.
 *
 * @details Writes the 2-byte sub-command (little-endian) to
 *          ManufacturerBlockAccess() (0x44). Used for toggles, resets and
 *          commands that simply trigger an action.
 *
 * @param[in] handle  Initialized handle.
 * @param[in] subcmd  MAC sub-command (e.g. @ref BQ40Z50R2_MAC_DEVICE_RESET).
 *
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_MACCommand(BQ40Z50R2_Handle_t *handle,
                                        uint16_t            subcmd);

/**
 * @brief Issue a MAC sub-command together with a data payload.
 *
 * @param[in] handle Initialized handle.
 * @param[in] subcmd MAC sub-command.
 * @param[in] data   Payload appended after the sub-command (may be NULL if
 *                   @p len is 0).
 * @param[in] len    Payload length in bytes.
 *
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_MACWrite(BQ40Z50R2_Handle_t *handle,
                                      uint16_t            subcmd,
                                      const uint8_t      *data,
                                      uint8_t             len);

/**
 * @brief Issue a MAC sub-command and read back its block reply.
 *
 * @details Writes the sub-command via ManufacturerBlockAccess() (0x44), then
 *          reads the block reply. The first two echoed bytes are verified
 *          against @p subcmd and stripped; only the payload is returned.
 *
 * @param[in]  handle  Initialized handle.
 * @param[in]  subcmd  MAC sub-command.
 * @param[out] data    Destination buffer for the payload (without MAC echo).
 * @param[in]  max_len Capacity of @p data in bytes.
 * @param[out] out_len Number of payload bytes returned (may be NULL).
 *
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_MACRead(BQ40Z50R2_Handle_t *handle,
                                     uint16_t            subcmd,
                                     uint8_t            *data,
                                     uint8_t             max_len,
                                     uint8_t            *out_len);

/**
 * @brief Read a 32-bit status register via its MAC sub-command.
 *
 * @details Convenience wrapper for the four-byte status registers
 *          (SafetyStatus, OperationStatus, etc.). The payload is assembled
 *          little-endian into a host-endian 32-bit value.
 *
 * @param[in]  handle Initialized handle.
 * @param[in]  subcmd MAC sub-command of the status register.
 * @param[out] value  Destination for the 32-bit value.
 *
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_MACReadU32(BQ40Z50R2_Handle_t *handle,
                                        uint16_t            subcmd,
                                        uint32_t           *value);

/** @} */

/**
 * @defgroup BQ40Z50R2_FUNC_HLVL High-level measurement helpers
 * @{
 *
 */

/**
 * @brief Read the pack voltage (sum of cell voltages) via Voltage() (0x09).
 * @param[in]  handle    Initialized handle.
 * @param[out] voltage_mv Voltage in millivolts.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetVoltage(BQ40Z50R2_Handle_t *handle,
                                        uint16_t           *voltage_mv);

/**
 * @brief Read the instantaneous current via Current() (0x0A).
 * @param[in]  handle     Initialized handle.
 * @param[out] current_ma Signed current in milliamps (positive = charge).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetCurrent(BQ40Z50R2_Handle_t *handle,
                                        int16_t            *current_ma);

/**
 * @brief Read the average current via AverageCurrent() (0x0B).
 * @param[in]  handle     Initialized handle.
 * @param[out] current_ma Signed average current in milliamps.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetAverageCurrent(BQ40Z50R2_Handle_t *handle,
                                               int16_t            *current_ma);

/**
 * @brief Read the temperature via Temperature() (0x08).
 * @param[in]  handle  Initialized handle.
 * @param[out] temp_dk Temperature in units of 0.1 K.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetTemperature(BQ40Z50R2_Handle_t *handle,
                                            uint16_t           *temp_dk);

/**
 * @brief Read the Relative State Of Charge via RelativeStateOfCharge() (0x0D).
 * @param[in]  handle  Initialized handle.
 * @param[out] rsoc_pct State of charge in percent (0..100).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetRSOC(BQ40Z50R2_Handle_t *handle,
                                     uint8_t            *rsoc_pct);

/**
 * @brief Read the remaining capacity via RemainingCapacity() (0x0F).
 * @param[in]  handle  Initialized handle.
 * @param[out] cap     Remaining capacity (mAh or cWh per BatteryMode()[CAPM]).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetRemainingCapacity(BQ40Z50R2_Handle_t *handle,
                                                  uint16_t           *cap);

/**
 * @brief Read the full-charge capacity via FullChargeCapacity() (0x10).
 * @param[in]  handle Initialized handle.
 * @param[out] cap    Full charge capacity (mAh or cWh per BatteryMode()[CAPM]).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetFullChargeCapacity(BQ40Z50R2_Handle_t *handle,
                                                   uint16_t           *cap);

/**
 * @brief Read the cycle count via CycleCount() (0x17).
 * @param[in]  handle Initialized handle.
 * @param[out] cycles Number of discharge cycles.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetCycleCount(BQ40Z50R2_Handle_t *handle,
                                          uint16_t           *cycles);

/**
 * @brief Read one cell voltage.
 *
 * @param[in]  handle      Initialized handle.
 * @param[in]  cell_index  Cell index 1..4 (1 = least positive cell).
 * @param[out] voltage_mv  Cell voltage in millivolts.
 *
 * @return Driver status code (BQ40Z50R2_INVALID_ARG if @p cell_index invalid).
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetCellVoltage(BQ40Z50R2_Handle_t *handle,
                                           uint8_t             cell_index,
                                           uint16_t           *voltage_mv);

/**
 * @brief Read BatteryStatus() (0x16).
 * @param[in]  handle Initialized handle.
 * @param[out] status 16-bit BatteryStatus bitfield.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetBatteryStatus(BQ40Z50R2_Handle_t *handle,
                                             uint16_t           *status);

/**
 * @brief Read and decode the DAStatus1 block (cell voltages/currents/powers).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded structure.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetDAStatus1(BQ40Z50R2_Handle_t    *handle,
                                         BQ40Z50R2_DAStatus1_t  *out);

/**
 * @brief Read and decode the DAStatus2 block (temperatures).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded structure.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetDAStatus2(BQ40Z50R2_Handle_t    *handle,
                                         BQ40Z50R2_DAStatus2_t  *out);

/**
 * @brief Read and decode the FirmwareVersion block (MAC 0x0002, 11 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded firmware version structure.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetFirmwareVersion(
        BQ40Z50R2_Handle_t         *handle,
        BQ40Z50R2_FirmwareVersion_t *out);

/**
 * @brief Read and decode the ManufacturerInfo block (MAC 0x0070, 32 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Structure holding the raw 32-byte payload.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetManufacturerInfo(
        BQ40Z50R2_Handle_t          *handle,
        BQ40Z50R2_ManufacturerInfo_t *out);

/**
 * @brief Read and decode the ManufacturerInfoB block (MAC 0x007A, 4 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Structure holding the raw 4-byte payload.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetManufacturerInfoB(
        BQ40Z50R2_Handle_t           *handle,
        BQ40Z50R2_ManufacturerInfoB_t *out);

/**
 * @brief Read and decode the AFERegister block (MAC 0x0058, 21 bytes).
 * @note  Intended for internal debug use only (TRM section 14.1.44).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded AFE register snapshot.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetAFERegister(
        BQ40Z50R2_Handle_t   *handle,
        BQ40Z50R2_AFERegister_t *out);

/**
 * @brief Read and decode the GaugeStatus1 block (MAC 0x0073, 32 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded Impedance Track simulation results.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetGaugeStatus1(
        BQ40Z50R2_Handle_t      *handle,
        BQ40Z50R2_GaugeStatus1_t *out);

/**
 * @brief Read and decode the GaugeStatus2 block (MAC 0x0074, 32 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded IT runtime tracking data (DOD0, grid points, etc.).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetGaugeStatus2(
        BQ40Z50R2_Handle_t      *handle,
        BQ40Z50R2_GaugeStatus2_t *out);

/**
 * @brief Read and decode the GaugeStatus3 block (MAC 0x0075, 24 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded QMax values and thermal model coefficients.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetGaugeStatus3(
        BQ40Z50R2_Handle_t      *handle,
        BQ40Z50R2_GaugeStatus3_t *out);

/**
 * @brief Read and decode the CBStatus block (MAC 0x0076, 18 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded cell balancing time and DOD data.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetCBStatus(
        BQ40Z50R2_Handle_t  *handle,
        BQ40Z50R2_CBStatus_t *out);

/**
 * @brief Read and decode the State-of-Health block (MAC 0x0077, 4 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded SoH FCC (mAh) and energy (cWh).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetStateOfHealth(
        BQ40Z50R2_Handle_t      *handle,
        BQ40Z50R2_StateOfHealth_t *out);

/**
 * @brief Read and decode the FilterCapacity block (MAC 0x0078, 8 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded filtered remaining and full-charge capacity/energy.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetFilterCapacity(
        BQ40Z50R2_Handle_t       *handle,
        BQ40Z50R2_FilterCapacity_t *out);

/**
 * @brief Read and decode the Lifetime Data Block 1 (MAC 0x0060, 32 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded min/max voltages, currents, and temperatures.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetLifetimeDataBlock1(
        BQ40Z50R2_Handle_t             *handle,
        BQ40Z50R2_LifetimeDataBlock1_t  *out);

/**
 * @brief Read and decode the Lifetime Data Block 2 (MAC 0x0061, 8 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded reset counters and cell-balance time accumulators.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetLifetimeDataBlock2(
        BQ40Z50R2_Handle_t             *handle,
        BQ40Z50R2_LifetimeDataBlock2_t  *out);

/**
 * @brief Read and decode the Lifetime Data Block 3 (MAC 0x0062, 16 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded firmware runtime and time-in-temperature-zone counters
 *                    (resolution: 2 h/LSB).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetLifetimeDataBlock3(
        BQ40Z50R2_Handle_t             *handle,
        BQ40Z50R2_LifetimeDataBlock3_t  *out);

/**
 * @brief Read and decode the Lifetime Data Block 4 (MAC 0x0063, 32 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded protection event counters and cycle-count timestamps.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetLifetimeDataBlock4(
        BQ40Z50R2_Handle_t             *handle,
        BQ40Z50R2_LifetimeDataBlock4_t  *out);

/**
 * @brief Read and decode the Lifetime Data Block 5 (MAC 0x0064, 32 bytes).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded protection and gauging-update event counters/timestamps.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetLifetimeDataBlock5(
        BQ40Z50R2_Handle_t             *handle,
        BQ40Z50R2_LifetimeDataBlock5_t  *out);

/** @} */

/**
 * @defgroup BQ40Z50R2_FUNC_HLVLSTAT High-level status helpers
 * @{
 *
 */

/**
 * @brief Read the 32-bit SafetyStatus() register.
 * @param[in]  handle Initialized handle.
 * @param[out] status 32-bit SafetyStatus bitfield.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetSafetyStatus(BQ40Z50R2_Handle_t *handle,
                                             uint32_t           *status);

/**
 * @brief Read the 32-bit OperationStatus() register.
 * @param[in]  handle Initialized handle.
 * @param[out] status 32-bit OperationStatus bitfield.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetOperationStatus(BQ40Z50R2_Handle_t *handle,
                                               uint32_t           *status);

/**
 * @brief Read the 32-bit ChargingStatus() register.
 * @param[in]  handle Initialized handle.
 * @param[out] status 32-bit ChargingStatus bitfield (24 significant bits).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetChargingStatus(BQ40Z50R2_Handle_t *handle,
                                              uint32_t           *status);

/**
 * @brief Read the 32-bit GaugingStatus() register.
 * @param[in]  handle Initialized handle.
 * @param[out] status 32-bit GaugingStatus bitfield (21 significant bits).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetGaugingStatus(BQ40Z50R2_Handle_t *handle,
                                             uint32_t           *status);

/**
 * @brief Read the 16-bit ManufacturingStatus() register.
 * @param[in]  handle Initialized handle.
 * @param[out] status 16-bit ManufacturingStatus bitfield.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetManufacturingStatus(BQ40Z50R2_Handle_t *handle,
                                                   uint16_t           *status);

/**
 * @brief Extract the SECURITY mode field from an OperationStatus() value.
 *
 * @param[in] operation_status A value previously read with
 *                             @ref BQ40Z50R2_GetOperationStatus.
 *
 * @return One of @ref BQ40Z50R2_SEC_FULL_ACCESS, @ref BQ40Z50R2_SEC_UNSEALED,
 *         @ref BQ40Z50R2_SEC_SEALED (or 0 for the reserved encoding).
 */
uint8_t BQ40Z50R2_GetSecurityMode(uint32_t operation_status);

/**
 * @brief Read the raw 32-bit SafetyAlert() register (MAC 0x0050).
 * @param[in]  handle Initialized handle.
 * @param[out] alert  32-bit SafetyAlert bitfield (use BQ40Z50R2_SAFETY_* masks).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetSafetyAlert(BQ40Z50R2_Handle_t *handle,
                                             uint32_t           *alert);

/**
 * @brief Read and decode the SafetyAlert block into named fields (MAC 0x0050).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded SafetyAlert structure.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetSafetyAlertDecoded(
        BQ40Z50R2_Handle_t    *handle,
        BQ40Z50R2_SafetyAlert_t *out);

/**
 * @brief Read and decode the SafetyStatus block into named fields (MAC 0x0051).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded SafetyStatus structure.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetSafetyStatusDecoded(
        BQ40Z50R2_Handle_t     *handle,
        BQ40Z50R2_SafetyStatus_t *out);

/**
 * @brief Read the raw 32-bit PFAlert() register (MAC 0x0052).
 * @param[in]  handle Initialized handle.
 * @param[out] alert  32-bit PFAlert bitfield (use BQ40Z50R2_PF_* masks).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetPFAlert(BQ40Z50R2_Handle_t *handle,
                                         uint32_t           *alert);

/**
 * @brief Read and decode the PFAlert block into named fields (MAC 0x0052).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded PFAlert structure (same layout as PFStatus).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetPFAlertDecoded(BQ40Z50R2_Handle_t *handle,
                                                BQ40Z50R2_PFStatus_t *out);

/**
 * @brief Read the raw 32-bit PFStatus() register (MAC 0x0053).
 * @param[in]  handle Initialized handle.
 * @param[out] status 32-bit PFStatus bitfield (use BQ40Z50R2_PF_* masks).
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetPFStatus(BQ40Z50R2_Handle_t *handle,
                                          uint32_t           *status);

/**
 * @brief Read and decode the PFStatus block into named fields (MAC 0x0053).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded PFStatus structure.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetPFStatusDecoded(BQ40Z50R2_Handle_t *handle,
                                                 BQ40Z50R2_PFStatus_t *out);

/**
 * @brief Read and decode the OperationStatus block into named fields (MAC 0x0054).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded OperationStatus structure.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetOperationStatusDecoded(
        BQ40Z50R2_Handle_t         *handle,
        BQ40Z50R2_OperationStatus_t *out);

/**
 * @brief Read and decode the ChargingStatus block into named fields (MAC 0x0055).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded ChargingStatus structure.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetChargingStatusDecoded(
        BQ40Z50R2_Handle_t        *handle,
        BQ40Z50R2_ChargingStatus_t *out);

/**
 * @brief Read and decode the GaugingStatus block into named fields (MAC 0x0056).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded GaugingStatus structure.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetGaugingStatusDecoded(
        BQ40Z50R2_Handle_t        *handle,
        BQ40Z50R2_GaugingStatus_t  *out);

/**
 * @brief Read and decode the ManufacturingStatus block into named fields (MAC 0x0057).
 * @param[in]  handle Initialized handle.
 * @param[out] out    Decoded ManufacturingStatus structure.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetManufacturingStatusDecoded(
        BQ40Z50R2_Handle_t              *handle,
        BQ40Z50R2_ManufacturingStatus_t  *out);

/** @} */

/**
 * @defgroup BQ40Z50R2_FUNC_ID Device identification
 * @{
 *
 */


/**
 * @brief Read the device type via MAC DeviceType() (0x0001).
 * @param[in]  handle      Initialized handle.
 * @param[out] device_type 16-bit device type code.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetDeviceType(BQ40Z50R2_Handle_t *handle,
                                          uint16_t           *device_type);

/**
 * @brief Read the chemical ID via MAC ChemID() (0x0006).
 * @param[in]  handle  Initialized handle.
 * @param[out] chem_id 16-bit chemistry identifier.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_GetChemID(BQ40Z50R2_Handle_t *handle,
                                      uint16_t           *chem_id);

/** @} */

/**
 * @defgroup BQ40Z50R2_FUNC_CTRL Control / actuation helpers
 * @{
 *
 */

/**
 * @brief Reset the device via MAC DeviceReset() (0x0041).
 * @param[in] handle Initialized handle.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_DeviceReset(BQ40Z50R2_Handle_t *handle);

/**
 * @brief Enter SHUTDOWN mode via MAC ShutdownMode() (0x0010).
 *
 * @note When the device is SEALED this command must be sent twice within 4 s
 *       to take effect (TRM section 14.1.10). This helper sends it once; call
 *       it twice if operating on a sealed device.
 *
 * @param[in] handle Initialized handle.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_ShutdownMode(BQ40Z50R2_Handle_t *handle);

/**
 * @brief Enter SLEEP mode via MAC SleepMode() (0x0011).
 * @param[in] handle Initialized handle.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_SleepMode(BQ40Z50R2_Handle_t *handle);

/**
 * @brief Toggle the CHG FET test state via MAC CHG FET Toggle (0x001F).
 * @note  Only effective when ManufacturingStatus()[FET_EN] = 0.
 * @param[in] handle Initialized handle.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_ToggleChargeFET(BQ40Z50R2_Handle_t *handle);

/**
 * @brief Toggle the DSG FET test state via MAC DSG FET Toggle (0x0020).
 * @note  Only effective when ManufacturingStatus()[FET_EN] = 0.
 * @param[in] handle Initialized handle.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_ToggleDischargeFET(BQ40Z50R2_Handle_t *handle);

/**
 * @brief Toggle the PCHG FET test state via MAC PCHG FET Toggle (0x001E).
 * @note  Only effective when ManufacturingStatus()[FET_EN] = 0.
 * @param[in] handle Initialized handle.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_TogglePrechargeFET(BQ40Z50R2_Handle_t *handle);

/**
 * @brief Toggle firmware FET control via MAC FET Control (0x0022).
 * @param[in] handle Initialized handle.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_ToggleFETControl(BQ40Z50R2_Handle_t *handle);

/**
 * @brief Toggle gauging via MAC Gauging (0x0021).
 * @param[in] handle Initialized handle.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_ToggleGauging(BQ40Z50R2_Handle_t *handle);

/**
 * @brief Toggle the FUSE output via MAC Fuse Toggle (0x001D).
 * @param[in] handle Initialized handle.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_ToggleFuse(BQ40Z50R2_Handle_t *handle);

/**
 * @brief Seal the device via MAC SealDevice() (0x0030).
 * @param[in] handle Initialized handle.
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_Seal(BQ40Z50R2_Handle_t *handle);

/**
 * @brief Unseal the device by writing the two UNSEAL key words.
 *
 * @details Sends the two key words to ManufacturerAccess() (0x00) as two
 *          consecutive write-word transactions, as required to transition out
 *          of SEALED mode.
 *
 * @param[in] handle Initialized handle.
 * @param[in] key1   First word of the UNSEAL key.
 * @param[in] key2   Second word of the UNSEAL key.
 *
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_Unseal(BQ40Z50R2_Handle_t *handle,
                                    uint16_t            key1,
                                    uint16_t            key2);

/**
 * @brief Enter FULL ACCESS mode by writing the two FULL ACCESS key words.
 *
 * @details The device must already be UNSEALED. Sends the two key words to
 *          ManufacturerAccess() (0x00) as two consecutive write-word
 *          transactions.
 *
 * @param[in] handle Initialized handle.
 * @param[in] key1   First word of the FULL ACCESS key.
 * @param[in] key2   Second word of the FULL ACCESS key.
 *
 * @return Driver status code.
 */
BQ40Z50R2_Status_t BQ40Z50R2_FullAccess(BQ40Z50R2_Handle_t *handle,
                                        uint16_t            key1,
                                        uint16_t            key2);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BQ40Z50R2_H */
