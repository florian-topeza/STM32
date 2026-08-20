/******************************************************************************************* */
/*                                                                                           */
/* LSM303AGR library                                                                         */
/*                                                                                           */
/* This library provides functions to interface an STM32 microcontroller with the            */
/* ST LSM303AGR sensor using I2C.                                                            */
/*                                                                                           */
/* Florian TOPEZA - 2025 & Merlin KOOSHMANIAN                                                */
/*                                                                                           */
/******************************************************************************************* */

#include "lsm303agr.h"

/************************ STATIC FUNCTIONS PROTOTYPES BEGIN ***********************************/

static error_t LSM303AGR_AccWrite(LSM303AGR_t* sensor, const uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
static error_t LSM303AGR_MagWrite(LSM303AGR_t* sensor, const uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);

static error_t LSM303AGR_AccRead(LSM303AGR_t* sensor, uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
static error_t LSM303AGR_MagRead(LSM303AGR_t* sensor, uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);

static void LSM303AGR_KeepFirstError(error_t *result, error_t step_result);

/************************** STATIC FUNCTIONS PROTOTYPES END ************************************/

/******************************** IO FUNCTIONS BEGIN *******************************************/

/* Write data from the accelerometer */
static error_t LSM303AGR_AccWrite(LSM303AGR_t *sensor, const uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  error_t result = ERR_OK;
  if ( ( (NumByteToWrite + 1U) > I2C_BUFFER_MAX_SIZE) || pBuffer == NULL )
  {
    result = ERR_INVALID_ARGUMENT;
    logs_lsm303agr("Error: invalid argument.\n\r");
  }
  else
  {
    uint8_t data[I2C_BUFFER_MAX_SIZE] = {0};
    data[0] = WriteAddr;

    for (uint16_t i = 0U ; i < NumByteToWrite; i++)
    {
      data[i+1] = pBuffer[i];
    }

    HAL_StatusTypeDef HAL_result = HAL_OK;
    HAL_result = HAL_I2C_Master_Transmit(sensor->p_hi2c, ACC_I2C_ADDRESS,
                                    data, NumByteToWrite + 1u, HAL_MAX_DELAY);
    if (HAL_result != HAL_OK)
    {
      result = ERR_HAL;
      logs_lsm303agr("Error from HAL.\n\r");
    }
  }

  return result;
}

/* Write data from the magnetometer */
static error_t LSM303AGR_MagWrite(LSM303AGR_t *sensor, const uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  error_t result = ERR_OK;

  if ( ((NumByteToWrite + 1U) > I2C_BUFFER_MAX_SIZE) || pBuffer == NULL )
  {
    result = ERR_INVALID_ARGUMENT;
    logs_lsm303agr("Error: invalid argument.\n\r");
  }
  else
  {
    uint8_t data[I2C_BUFFER_MAX_SIZE] = {0};
    data[0] = WriteAddr;

    for (uint16_t i = 0U ; i < NumByteToWrite; i++)
    {
      data[i+1] = pBuffer[i];
    }

    HAL_StatusTypeDef HAL_result = HAL_OK;
    HAL_result = HAL_I2C_Master_Transmit(sensor->p_hi2c, MAG_I2C_ADDRESS, data, NumByteToWrite + 1u, HAL_MAX_DELAY);
    if (HAL_result != HAL_OK)
    {
      result = ERR_HAL;
      logs_lsm303agr("Error from HAL.\n\r");
    }
  }

  return result;
}


/* Read data from the accelerometer*/
static error_t LSM303AGR_AccRead(LSM303AGR_t *sensor, uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
  error_t result = ERR_OK;
  HAL_StatusTypeDef HAL_result = HAL_OK;

  if (pBuffer == NULL)
  {
    result = ERR_INVALID_ARGUMENT;
    logs_lsm303agr("Error: invalid argument.\n\r");
  }
  else
  {
    // Start writing the ReadAddr
    HAL_result = HAL_I2C_Master_Transmit(sensor->p_hi2c, ACC_I2C_ADDRESS, &ReadAddr, 1u,
                                          HAL_MAX_DELAY);
    if (HAL_result != HAL_OK)
    {
      result = ERR_HAL;
      logs_lsm303agr("Error from HAL.\n\r");
    }
    // Then read the data
    HAL_result = HAL_I2C_Master_Receive(sensor->p_hi2c, ACC_I2C_ADDRESS,
                                  pBuffer, NumByteToRead, HAL_MAX_DELAY);
    if (HAL_result != HAL_OK)
    {
      result = ERR_HAL;
      logs_lsm303agr("Error from HAL.\n\r");
    }
  }
  return result;
}

/* Read data from the magnetometer */
static error_t LSM303AGR_MagRead(LSM303AGR_t *sensor, uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
  error_t result = ERR_OK;
  HAL_StatusTypeDef HAL_result = HAL_OK;
  if (pBuffer == NULL)
  {
    result = ERR_INVALID_ARGUMENT;
    logs_lsm303agr("Error: invalid argument.\n\r");
  }
  else
  {
    // Start writing the ReadAddr
    HAL_result = HAL_I2C_Master_Transmit(sensor->p_hi2c, MAG_I2C_ADDRESS,
                                    &ReadAddr, 1u, HAL_MAX_DELAY);
    if (HAL_result != HAL_OK)
    {
      result = ERR_HAL;
      logs_lsm303agr("Error from HAL.\n\r");
    }
    // Then read the data
    HAL_result = HAL_I2C_Master_Receive(sensor->p_hi2c, MAG_I2C_ADDRESS,
                                  pBuffer, NumByteToRead, HAL_MAX_DELAY);
    if (HAL_result != HAL_OK)
    {
      result = ERR_HAL;
      logs_lsm303agr("Error from HAL.\n\r");
    }
  }
  return result;
}

/* Keep the first error encountered across a sequence of independent I2C operations, so that a
   later successful step cannot silently overwrite (and hide) an earlier failure */
static void LSM303AGR_KeepFirstError(error_t *result, error_t step_result)
{
  if (*result == ERR_OK)
  {
    *result = step_result;
  }
}

/******************************* IO FUNCTIONS END ****************************************** */

/*******************************************************************************
                            LINK OPERATIONS
*******************************************************************************/

/* Sensor start-up sequence */
error_t LSM303AGR_Init(LSM303AGR_t *sensor)
{
  error_t result = ERR_OK;
  error_t step_result = ERR_OK;

  /*
  The following general-purpose sequence can be used to configure the magnetometer and
  accelerometer:
  1. Write CFG_REG_A_M = 00h // Mag = 10 Hz (high-resolution and continuous mode)
  2. Write CFG_REG_C_M = 01h // Mag data-ready interrupt enable
  3. Write CTRL_REG1_A = 57h // Accel  = 100 Hz (normal mode)
  */

  uint8_t CFG_REG_A_M_DATA = 0x00;
  uint8_t CFG_REG_C_M_DATA = 0x01;
  uint8_t CTRL_REG1_A_DATA = 0x57;

  /* Write value to CFG_REG_A_M register */
  step_result = LSM303AGR_MagWrite(sensor, &CFG_REG_A_M_DATA, LSM303AGR_CFG_REG_A_M, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error writing to magnetometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }
  /* Write value to CFG_REG_C_M register */
  step_result = LSM303AGR_MagWrite(sensor, &CFG_REG_C_M_DATA, LSM303AGR_CFG_REG_C_M, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error writing to magnetometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }
  /* Write value to CTRL_REG1_A register */
  step_result = LSM303AGR_AccWrite(sensor, &CTRL_REG1_A_DATA, LSM303AGR_CTRL_REG1_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error writing to accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  return result;
}

/***************************** LINK ACCELEROMETER *****************************/

/* Read accelerometer ID */
error_t LSM303AGR_AccReadID(LSM303AGR_t *sensor, uint8_t *pBuffer)
{
  error_t result = ERR_OK;

  if (pBuffer == NULL)
  {
    result = ERR_INVALID_ARGUMENT;
    logs_lsm303agr("Error: invalid argument.\n\r");
  }

  /* Read value at Who am I register address */
  result = LSM303AGR_AccRead(sensor, pBuffer, LSM303AGR_WHO_AM_I_A, 1); // Returned value is stored in pBuffer

  return result;
}

/* Set High Pass Filter Modality*/
error_t LSM303AGR_AccFilterConfig(LSM303AGR_t *sensor, uint8_t *pBuffer, ACCELERO_FilterConfig_t FilterStruct)
{

  error_t result = ERR_OK;

  if (pBuffer == NULL)
  {
    result = ERR_INVALID_ARGUMENT;
    logs_lsm303agr("Error: invalid argument.\n\r");
  }
  else
  {
    /* Mode_Sel, CutOff_Freq, AOI1 and AOI2 come from the LSM303AGR_ACC_HPF_*
       groups in the header: they are already positioned at their register
       bit offset, so they must be OR'ed as-is, not shifted again */
    uint8_t Mode_Sel = FilterStruct.HighPassFilter_Mode_Selection;
    uint8_t CutOff_Freq = FilterStruct.HighPassFilter_CutOff_Frequency;
    uint8_t AOI2 = FilterStruct.HighPassFilter_AOI2;
    uint8_t AOI1 = FilterStruct.HighPassFilter_AOI1;

    uint8_t FilterConfigMask = Mode_Sel | CutOff_Freq | AOI2 | AOI1;

    error_t step_result = ERR_OK;

    /* Read CTRL_REG2 register */
    step_result = LSM303AGR_AccRead(sensor, pBuffer, LSM303AGR_CTRL_REG2_A, 1); // Returned value stored in *pBuffer
    if (step_result != ERR_OK)
    {
      logs_lsm303agr("Error reading from accelerometer\n\r");
      LSM303AGR_KeepFirstError(&result, step_result);
    }
    uint8_t tmpreg = *pBuffer;

    tmpreg &= 0x0C;
    tmpreg |= FilterConfigMask;

    /* Write value to ACC MEMS CTRL_REG2 register */
    step_result = LSM303AGR_AccWrite(sensor, &tmpreg, LSM303AGR_CTRL_REG2_A, 1);
    if (step_result != ERR_OK)
    {
      logs_lsm303agr("Error writing to accelerometer\n\r");
      LSM303AGR_KeepFirstError(&result, step_result);
    }
  }

  return result;

}

/* Read X, Y & Z Acceleration values */
error_t LSM303AGR_AccReadXYZ(LSM303AGR_t *sensor, int16_t *pnRawData)
{

  error_t result = ERR_OK;
  error_t step_result = ERR_OK;
  uint8_t i = 0;
  uint8_t ctrlx[2] = {0, 0};
  uint8_t buffer[6];

  /* Read the acceleration control register content */
  step_result = LSM303AGR_AccRead(sensor, &ctrlx[0], LSM303AGR_CTRL_REG4_A, 1);
  LSM303AGR_KeepFirstError(&result, step_result);
  step_result = LSM303AGR_AccRead(sensor, &ctrlx[1], LSM303AGR_CTRL_REG5_A, 1);
  LSM303AGR_KeepFirstError(&result, step_result);

  /* Read output register X, Y & Z acceleration */
  step_result = LSM303AGR_AccRead(sensor, &buffer[0], LSM303AGR_OUT_X_L_A, 1);
  LSM303AGR_KeepFirstError(&result, step_result);
  step_result = LSM303AGR_AccRead(sensor, &buffer[1], LSM303AGR_OUT_X_H_A, 1);
  LSM303AGR_KeepFirstError(&result, step_result);
  step_result = LSM303AGR_AccRead(sensor, &buffer[2], LSM303AGR_OUT_Y_L_A, 1);
  LSM303AGR_KeepFirstError(&result, step_result);
  step_result = LSM303AGR_AccRead(sensor, &buffer[3], LSM303AGR_OUT_Y_H_A, 1);
  LSM303AGR_KeepFirstError(&result, step_result);
  step_result = LSM303AGR_AccRead(sensor, &buffer[4], LSM303AGR_OUT_Z_L_A, 1);
  LSM303AGR_KeepFirstError(&result, step_result);
  step_result = LSM303AGR_AccRead(sensor, &buffer[5], LSM303AGR_OUT_Z_H_A, 1);
  LSM303AGR_KeepFirstError(&result, step_result);

  /* Check in the control register 4 the data alignment*/
  if (!(ctrlx[0] & LSM303AGR_BLE_MSB))
  {
    for (i = 0; i < 3; i++)
    {
      pnRawData[i] = ((int16_t)((uint16_t)buffer[2 * i + 1] << 8) + buffer[2 * i]);
    }
  }
  else /* Big Endian Mode */
  {
    for (i = 0; i < 3; i++)
    {
      pnRawData[i] = ((int16_t)((uint16_t)buffer[2 * i] << 8) + buffer[2 * i + 1]);
    }
  }

  if (result != ERR_OK)
  {
    logs_lsm303agr("Error reading from accelerometer\n\r");
  }
  return result;
}

error_t LSM303AGR_AccConvertXYZ(LSM303AGR_t *sensor, const int16_t *pnRawData, int16_t *pData)
{

  error_t result = ERR_OK;
  error_t step_result = ERR_OK;

  uint8_t i = 0;
  uint8_t sensitivity = LSM303AGR_ACC_SENSITIVITY_2G;
  uint8_t ctrlx[2] = {0, 0};

  /* Read the acceleration control register content */
  step_result = LSM303AGR_AccRead(sensor, &ctrlx[0], LSM303AGR_CTRL_REG4_A, 1);
  LSM303AGR_KeepFirstError(&result, step_result);
  step_result = LSM303AGR_AccRead(sensor, &ctrlx[1], LSM303AGR_CTRL_REG5_A, 1);
  LSM303AGR_KeepFirstError(&result, step_result);

  /* High-resolution mode */
  /* Switch the sensitivity value set in the CRTL4 */
  switch (ctrlx[0] & LSM303AGR_FULLSCALE_16G)
  {
  case LSM303AGR_FULLSCALE_2G:
    sensitivity = LSM303AGR_ACC_SENSITIVITY_2G;
    break;
  case LSM303AGR_FULLSCALE_4G:
    sensitivity = LSM303AGR_ACC_SENSITIVITY_4G;
    break;
  case LSM303AGR_FULLSCALE_8G:
    sensitivity = LSM303AGR_ACC_SENSITIVITY_8G;
    break;
  case LSM303AGR_FULLSCALE_16G:
    sensitivity = LSM303AGR_ACC_SENSITIVITY_16G;
    break;
  }

  /* Obtain the mg value for the three axis */
  for (i = 0; i < 3; i++)
  {
    pData[i] = (pnRawData[i] * sensitivity);
  }

  if (result != ERR_OK)
  {
    logs_lsm303agr("Error reading from accelerometer\n\r");
  }
  return result;
}

/* Enable LSM303AGR interrupt 1 */
error_t LSM303AGR_AccIT1Enable(LSM303AGR_t *sensor)
{
  error_t result = ERR_OK;
  error_t step_result = ERR_OK;

  uint8_t tmpval = 0x00;

  /* Read CTRL_REG3 register */
  step_result = LSM303AGR_AccRead(sensor, &tmpval, LSM303AGR_CTRL_REG3_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error reading from accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }
  /* Construct new value of CTRL_REG3_A content with interrupt 1 struct of the sensor */
  uint8_t LSM303AGR_IT = sensor->Accelero.IT1Struct.Click_En << 7 |
                         sensor->Accelero.IT1Struct.AOI1_En << 6 |
                         sensor->Accelero.IT1Struct.AOI2_En << 5 |
                         sensor->Accelero.IT1Struct.DRY1_En << 4 |
                         sensor->Accelero.IT1Struct.DRY2_En << 3 |
                         sensor->Accelero.IT1Struct.WTM_En << 2 |
                         sensor->Accelero.IT1Struct.Overrun_En << 1;

  /* Enable IT1 */
  tmpval |= LSM303AGR_IT;

  /* Write value to MEMS CTRL_REG3 register */
  step_result = LSM303AGR_AccWrite(sensor, &tmpval, LSM303AGR_CTRL_REG3_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error writing to accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  return result;
}

/* Disable LSM303AGR interrupt 1 */
error_t LSM303AGR_AccIT1Disable(LSM303AGR_t *sensor)
{
  error_t result = ERR_OK;
  error_t step_result = ERR_OK;

  uint8_t tmpval = 0x00;

  /* Read CTRL_REG3 register */
  step_result = LSM303AGR_AccRead(sensor, &tmpval, LSM303AGR_CTRL_REG3_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error reading from accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  /* Construct new value of CTRL_REG3_A content with interrupt 1 struct of the sensor */
  uint8_t LSM303AGR_IT = sensor->Accelero.IT1Struct.Click_En << 7 |
                         sensor->Accelero.IT1Struct.AOI1_En << 6 |
                         sensor->Accelero.IT1Struct.AOI2_En << 5 |
                         sensor->Accelero.IT1Struct.DRY1_En << 4 |
                         sensor->Accelero.IT1Struct.DRY2_En << 3 |
                         sensor->Accelero.IT1Struct.WTM_En << 2 |
                         sensor->Accelero.IT1Struct.Overrun_En << 1;

  /* Disable IT1 */
  tmpval &= ~LSM303AGR_IT;

  /* Write value to MEMS CTRL_REG3 register */
  step_result = LSM303AGR_AccWrite(sensor, &tmpval, LSM303AGR_CTRL_REG3_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error writing to accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  return result;
}

/* Enable LSM303AGR interrupt 2 */
error_t LSM303AGR_AccIT2Enable(LSM303AGR_t *sensor)
{
  error_t result = ERR_OK;
  error_t step_result = ERR_OK;

  uint8_t tmpval = 0x00;

  /* Read CTRL_REG6 register */
  step_result = LSM303AGR_AccRead(sensor, &tmpval, LSM303AGR_CTRL_REG6_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error reading from accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  /* Construct new value of CTRL_REG3_A content with interrupt 2 struct of the sensor */
  uint8_t LSM303AGR_IT = sensor->Accelero.IT2Struct.Click_En << 7 |
                         sensor->Accelero.IT2Struct.IT1_En << 6 |
                         sensor->Accelero.IT2Struct.IT2_En << 5 |
                         sensor->Accelero.IT2Struct.Boot_En << 4 |
                         sensor->Accelero.IT2Struct.Activity_En << 3 |
                         sensor->Accelero.IT2Struct.High_Latency_Active_En << 1;

  /* Enable IT2 */
  tmpval |= LSM303AGR_IT;

  /* Write value to MEMS CTRL_REG6 register */
  step_result = LSM303AGR_AccWrite(sensor, &tmpval, LSM303AGR_CTRL_REG6_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error writing to accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  return result;
}

/* Disable LSM303AGR interrupt 2 */
error_t LSM303AGR_AccIT2Disable(LSM303AGR_t *sensor)
{
  error_t result = ERR_OK;
  error_t step_result = ERR_OK;

  uint8_t tmpval = 0x00;

  /* Read CTRL_REG6 register */
  step_result = LSM303AGR_AccRead(sensor, &tmpval, LSM303AGR_CTRL_REG6_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error reading from accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  /* Construct new value of CTRL_REG3_A content with interrupt 2 struct of the sensor */
  uint8_t LSM303AGR_IT = sensor->Accelero.IT2Struct.Click_En << 7 |
                         sensor->Accelero.IT2Struct.IT1_En << 6 |
                         sensor->Accelero.IT2Struct.IT2_En << 5 |
                         sensor->Accelero.IT2Struct.Boot_En << 4 |
                         sensor->Accelero.IT2Struct.Activity_En << 3 |
                         sensor->Accelero.IT2Struct.High_Latency_Active_En << 1;

  /* Disable IT2 */
  tmpval &= ~LSM303AGR_IT;

  /* Write value to MEMS CTRL_REG6 register */
  step_result = LSM303AGR_AccWrite(sensor, &tmpval, LSM303AGR_CTRL_REG6_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error writing to accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  return result;
}

/* INT1 interrupt enable */
error_t LSM303AGR_AccINT1InterruptEnable(LSM303AGR_t *sensor)
{
  error_t result = ERR_OK;
  error_t step_result = ERR_OK;

  uint8_t tmpval = 0x00;

  /* Read INT1_CFG_A register */
  step_result = LSM303AGR_AccRead(sensor, &tmpval, LSM303AGR_INT1_CFG_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error reading from accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  /* Construct new value of INT1_CFG content with interrupt 1 struct of the sensor */
  uint8_t ITCombination = sensor->Accelero.INT1Struct.AOI << 7 |
                          sensor->Accelero.INT1Struct.SixD << 6;

  uint8_t ITAxes = sensor->Accelero.INT1Struct.ZHIE << 5 |
                   sensor->Accelero.INT1Struct.ZLIE << 4 |
                   sensor->Accelero.INT1Struct.YHIE << 3 |
                   sensor->Accelero.INT1Struct.YLIE << 2 |
                   sensor->Accelero.INT1Struct.XHIE << 1 |
                   sensor->Accelero.INT1Struct.XLIE;

  /* Enable the selected interrupt */
  tmpval |= (ITAxes | ITCombination);

  /* Write value to MEMS INT1_CFG_A register */
  step_result = LSM303AGR_AccWrite(sensor, &tmpval, LSM303AGR_INT1_CFG_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error writing to accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  return result;
}

/* INT1 interrupt disable */
error_t LSM303AGR_AccINT1InterruptDisable(LSM303AGR_t *sensor)
{

  error_t result = ERR_OK;
  error_t step_result = ERR_OK;
  uint8_t tmpval = 0x00;

  /* Read INT1_CFG_A register */
  step_result = LSM303AGR_AccRead(sensor, &tmpval, LSM303AGR_INT1_CFG_A, 1); // Returned value stored in tmpval
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error reading from accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  /* Construct new value of INT1_CFG content with interrupt 1 struct of the sensor */
  uint8_t ITCombination = sensor->Accelero.INT1Struct.AOI << 7 |
                          sensor->Accelero.INT1Struct.SixD << 6;

  uint8_t ITAxes = sensor->Accelero.INT1Struct.ZHIE << 5 |
                   sensor->Accelero.INT1Struct.ZLIE << 4 |
                   sensor->Accelero.INT1Struct.YHIE << 3 |
                   sensor->Accelero.INT1Struct.YLIE << 2 |
                   sensor->Accelero.INT1Struct.XHIE << 1 |
                   sensor->Accelero.INT1Struct.XLIE;

  /* Disable the selected interrupt */
  tmpval &= ~(ITAxes | ITCombination);

  /* Write value to MEMS INT1_CFG_A register */
  step_result = LSM303AGR_AccWrite(sensor, &tmpval, LSM303AGR_INT1_CFG_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error writing to accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  return result;
}

/* INT2 interrupt enable */
error_t LSM303AGR_AccINT2InterruptEnable(LSM303AGR_t *sensor)
{
  error_t result = ERR_OK;
  error_t step_result = ERR_OK;

  uint8_t tmpval = 0x00;

  /* Read INT2_CFG_A register */
  step_result = LSM303AGR_AccRead(sensor, &tmpval, LSM303AGR_INT2_CFG_A, 1); // Value stored in tmpval
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error reading from accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  /* Construct new value of INT2_CFG content with interrupt 2 struct of the sensor */
  uint8_t ITCombination = sensor->Accelero.INT2Struct.AOI << 7 |
                          sensor->Accelero.INT2Struct.SixD << 6;

  uint8_t ITAxes = sensor->Accelero.INT2Struct.ZDIE << 5 |
                   sensor->Accelero.INT2Struct.ZSIE << 4 |
                   sensor->Accelero.INT2Struct.YDIE << 3 |
                   sensor->Accelero.INT2Struct.YSIE << 2 |
                   sensor->Accelero.INT2Struct.XDIE << 1 |
                   sensor->Accelero.INT2Struct.XSIE;

  /* Enable the selected interrupt */
  tmpval |= (ITAxes | ITCombination);

  /* Write value to MEMS INT1_CFG_A register */
  step_result = LSM303AGR_AccWrite(sensor, &tmpval, LSM303AGR_INT2_CFG_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error writing to accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  return result;
}

/* INT2 interrupt config */
error_t LSM303AGR_AccINT2InterruptDisable(LSM303AGR_t *sensor)
{

  error_t result = ERR_OK;
  error_t step_result = ERR_OK;
  uint8_t tmpval = 0x00;

  /* Read INT2_CFG_A register */
  step_result = LSM303AGR_AccRead(sensor, &tmpval, LSM303AGR_INT2_CFG_A, 1); // Value stored in tmpval
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error reading from accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  /* Construct new value of INT2_CFG content with interrupt 2 struct of the sensor */
  uint8_t ITCombination = sensor->Accelero.INT2Struct.AOI << 7 |
                          sensor->Accelero.INT2Struct.SixD << 6;

  uint8_t ITAxes = sensor->Accelero.INT2Struct.ZDIE << 5 |
                   sensor->Accelero.INT2Struct.ZSIE << 4 |
                   sensor->Accelero.INT2Struct.YDIE << 3 |
                   sensor->Accelero.INT2Struct.YSIE << 2 |
                   sensor->Accelero.INT2Struct.XDIE << 1 |
                   sensor->Accelero.INT2Struct.XSIE;

  /* Disable the selected interrupt */
  tmpval &= ~(ITAxes | ITCombination);

  /* Write value to MEMS INT1_CFG_A register */
  step_result = LSM303AGR_AccWrite(sensor, &tmpval, LSM303AGR_INT2_CFG_A, 1);
  if (step_result != ERR_OK)
  {
    logs_lsm303agr("Error writing to accelerometer\n\r");
    LSM303AGR_KeepFirstError(&result, step_result);
  }

  return result;

}

/***************************** END LINK ACCELEROMETER **************************/

/***************************** LINK MAGNETOMETER *******************************/

/* Set LSM303AGR Magnetometer Initialization */
error_t LSM303AGR_MagInit(LSM303AGR_t *sensor)
{

  error_t result = ERR_OK;
  error_t step_result = ERR_OK;
  uint8_t cfg_reg_a_m = 0x00, cfg_reg_c_m = 0x00;

  /* Configure MEMS: Temperature compensation and Data rate */
  cfg_reg_a_m |= sensor->Magneto.Comp_Temp_En << 7 |
                 sensor->Magneto.Mag_ODR << 2;

  /* Configure MEMS: Interrupts and interface */
  cfg_reg_c_m |= sensor->Magneto.INT_MAG_Pin << 6 |
                 sensor->Magneto.I2C_DIS << 5 |
                 sensor->Magneto.Self_test << 1 |
                 sensor->Magneto.INT_MAG;

  /* Write value to Mag MEMS CFG_REG_A_M register */
  step_result = LSM303AGR_MagWrite(sensor, &cfg_reg_a_m, LSM303AGR_CFG_REG_A_M, 1);
  LSM303AGR_KeepFirstError(&result, step_result);

  /* Write value to Mag MEMS CFG_REG_C_M register */
  step_result = LSM303AGR_MagWrite(sensor, &cfg_reg_c_m, LSM303AGR_CFG_REG_C_M, 1);
  LSM303AGR_KeepFirstError(&result, step_result);

  if (result != ERR_OK)
  {
    logs_lsm303agr("Error writing to magnetometer\n\r");
  }

  return result;
}

/* Read magnetometer ID */
error_t LSM303AGR_MagReadID(LSM303AGR_t *sensor, uint8_t *pBuffer)
{

  error_t result = ERR_OK;

  if (pBuffer == NULL)
  {
    result = ERR_INVALID_ARGUMENT;
    logs_lsm303agr("Error: invalid argument.\n\r");
  }
  else
  {
    /* Read value at Who am I register address */
    result = LSM303AGR_MagRead(sensor, pBuffer, LSM303AGR_WHO_AM_I_M, 1); // Returned value is stored in pBuffer

    logs_lsm303agr("Magneto ID: %x \n\r", pBuffer[0]);
  }

  return result;
}

/* Get status for Mag LSM303AGR data */
error_t LSM303AGR_MagGetDataStatus(LSM303AGR_t *sensor, uint8_t *pBuffer)
{
  error_t result = ERR_OK;

  if (pBuffer == NULL)
  {
    result = ERR_INVALID_ARGUMENT;
    logs_lsm303agr("Error: invalid argument.\n\r");
  }

  /* Read Mag STATUS register */
  result = LSM303AGR_MagRead(sensor, pBuffer, LSM303AGR_STATUS_REG_M, 1); // Value stored in pBuffer

  return result;
}

/* Read X, Y & Z Magnetometer values */
error_t LSM303AGR_MagReadXYZ(LSM303AGR_t *sensor, int16_t *pData)
{

  error_t result = ERR_OK;

  if (pData == NULL)
  {
    result = ERR_INVALID_ARGUMENT;
    logs_lsm303agr("Error: invalid argument.\n\r");
  }
  else
  {
    error_t step_result = ERR_OK;
    uint8_t buffer[6];
    uint8_t i = 0;

    /* Read output register X, Y & Z magnetometer */
    step_result = LSM303AGR_MagRead(sensor, &buffer[0], LSM303AGR_OUTX_L_REG_M, 1);
    LSM303AGR_KeepFirstError(&result, step_result);
    step_result = LSM303AGR_MagRead(sensor, &buffer[1], LSM303AGR_OUTX_H_REG_M, 1);
    LSM303AGR_KeepFirstError(&result, step_result);
    step_result = LSM303AGR_MagRead(sensor, &buffer[2], LSM303AGR_OUTY_L_REG_M, 1);
    LSM303AGR_KeepFirstError(&result, step_result);
    step_result = LSM303AGR_MagRead(sensor, &buffer[3], LSM303AGR_OUTY_H_REG_M, 1);
    LSM303AGR_KeepFirstError(&result, step_result);
    step_result = LSM303AGR_MagRead(sensor, &buffer[4], LSM303AGR_OUTZ_L_REG_M, 1);
    LSM303AGR_KeepFirstError(&result, step_result);
    step_result = LSM303AGR_MagRead(sensor, &buffer[5], LSM303AGR_OUTZ_H_REG_M, 1);
    LSM303AGR_KeepFirstError(&result, step_result);

    /* Check in the control register4 the data alignment*/
    uint8_t ctrlx = 0;
    step_result = LSM303AGR_MagRead(sensor, &ctrlx, LSM303AGR_CFG_REG_C_M, 1);
    LSM303AGR_KeepFirstError(&result, step_result);
    if ((ctrlx & LSM303AGR_MAG_BLE_MSB))
    {
      for (i = 0; i < 3; i++)
      {
        pData[i] = ((int16_t)((uint16_t)buffer[2 * i] << 8) + buffer[2 * i + 1]);
      }
    }
    else
    {
      for (i = 0; i < 3; i++)
      {
        pData[i] = ((int16_t)((uint16_t)buffer[2 * i + 1] << 8) + buffer[2 * i]);
      }
    }
  }

  return result;
}

/********************************** END OF FILE ******************************************** */
