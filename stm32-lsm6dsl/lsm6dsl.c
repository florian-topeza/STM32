/**
 * @file        lsm6dsl.c
 * @author      Florian Topeza & Merlin Kooshmanian
 * @brief       Minimalistic driver to interface LSM6DSL Sensor with STM32 microcontrollers
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
#include "lsm6dsl.h"
#include "errors.h"

static uint8_t LSM6DSL_CS_Init(LSM6DSL_t *sensor);


/* ==== Functions ====*/

/* Initialize the IO interface for the sensor */
error_t LSM6DSL_IO_Init(LSM6DSL_t *sensor)
{

  error_t result = ERR_OK;

  LSM6DSL_CS_Init(sensor);

  // Set 3-Wire Mode if defined

  #ifdef LSM6DSL_SPI_3WIRE
    uint8_t pBuffer[1] = {0x0C}; // 00001100
    result = LSM6DSL_Write(sensor, pBuffer, LSM6DSL_ACC_GYRO_CTRL3_C, 1);
  #endif

  return result;
}

/* Initialize the chip select (CS) pin for the sensor */
static uint8_t LSM6DSL_CS_Init(LSM6DSL_t *sensor)
{
  HAL_GPIO_WritePin(sensor->cs_gpio_port, sensor->cs_pin, GPIO_PIN_SET); // Set CS high
  return 0;
}

error_t LSM6DSL_Write(LSM6DSL_t *sensor, uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{

  error_t result = ERR_OK;
  HAL_StatusTypeDef HAL_result = HAL_OK;

  // RW = 0 to write

  /* Set chip select Low at the start of the transmission */
  HAL_GPIO_WritePin(sensor->cs_gpio_port, sensor->cs_pin, GPIO_PIN_RESET);

  uint8_t msg = WriteAddr; // Initialise 8-bit message

  /* Send the Address of the indexed register */
  /* Auto-increment for multi-byte writes relies on IF_INC being enabled in
   * CTRL3_C, which is configured once at init time, not per-transaction. */
  HAL_result = HAL_SPI_Transmit(sensor->p_hspi, (uint8_t *)&msg, 1,
                        HAL_MAX_DELAY);
  if (HAL_result != HAL_OK)
  {
    result = ERR_HAL;
  }
  /* Send the data that will be written into the device (MSB First) */
  while (NumByteToWrite >= 0x01)
  {
    HAL_result = HAL_SPI_Transmit(sensor->p_hspi, pBuffer, 1,
                          HAL_MAX_DELAY);
  if (HAL_result != HAL_OK)
  {
    result = ERR_HAL;
  }
    NumByteToWrite--;
    pBuffer++;
  }

  /* Set chip select High at the end of the transmission */
  HAL_GPIO_WritePin(sensor->cs_gpio_port, sensor->cs_pin, GPIO_PIN_SET);

  return result;
}

error_t LSM6DSL_Read(LSM6DSL_t *sensor, uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
  error_t result = ERR_OK;
  HAL_StatusTypeDef HAL_result = HAL_OK;

  /* Configure RW bit */
  ReadAddr |= 0x80; // Set RW bit to 1 to

  /* Set chip select Low at the start of the transmission */
  HAL_GPIO_WritePin(sensor->cs_gpio_port, sensor->cs_pin, GPIO_PIN_RESET);

  /* Send the Address of the indexed register */
  HAL_result = HAL_SPI_Transmit(sensor->p_hspi, (uint8_t *)&ReadAddr, 1, HAL_MAX_DELAY);
  if (HAL_result != HAL_OK)
  {
    result = ERR_HAL;
  }

  /* Receive the data that will be read from the device (MSB First) */
  while (NumByteToRead > 0x00)
  {
    HAL_result = HAL_SPI_Receive(sensor->p_hspi, pBuffer, 1, HAL_MAX_DELAY);
    if (HAL_result != HAL_OK)
    {
      result = ERR_HAL;
    }
    NumByteToRead--;
    pBuffer++;
  }

  /* Set chip select High at the end of the transmission */
  HAL_GPIO_WritePin(sensor->cs_gpio_port, sensor->cs_pin, GPIO_PIN_SET);

  return result;
}

/******************************* IO FUNCTIONS END ****************************************** */

/******************************* LINK OPERATIONS *********************************************/

/* Read accelerometer ID */
error_t LSM6DSL_ReadID(LSM6DSL_t *sensor, uint8_t *pBuffer)
{

  error_t result = ERR_OK;
  /* Read value at Who am I register address */
  result = LSM6DSL_Read(sensor, pBuffer, LSM6DSL_ACC_GYRO_WHO_AM_I_REG, 1); // Returned value is stored in pBuffer

  logs_lsm6dsl("LSM6DSL ID: %x \n\r", pBuffer[0]);

  return result;
}

/***************************** LINK ACCELEROMETER *****************************/

/* Accelerometer initialisation */
error_t LSM6DSL_AccInit(LSM6DSL_t *sensor)
{

  error_t result = ERR_OK;

  uint8_t ctrl = 0x00;
  uint8_t tmp = 0x00;

  /* Read CTRL1_XL */
  result = LSM6DSL_Read(sensor, &tmp, LSM6DSL_ACC_GYRO_CTRL1_XL, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  /* Write value to ACC MEMS CTRL1_XL register: FS and Data Rate */
  ctrl = sensor->Accelero.ODR << 4 | sensor->Accelero.Full_Scale << 2;
  tmp &= ~(0xFC);
  tmp |= ctrl;
  result = LSM6DSL_Write(sensor, &tmp, LSM6DSL_ACC_GYRO_CTRL1_XL, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid writing to sensor.\n\r");
  }

  /* Read CTRL3_C */
  result = LSM6DSL_Read(sensor, &tmp, LSM6DSL_ACC_GYRO_CTRL3_C, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  /* Write value to ACC MEMS CTRL3_C register: BDU and Auto-increment */
  ctrl = sensor->Config.BDU << 6 | sensor->Config.IF_INC << 2;
  tmp &= ~(0x44);
  tmp |= ctrl;
  result = LSM6DSL_Write(sensor, &tmp, LSM6DSL_ACC_GYRO_CTRL3_C, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid writing to sensor.\n\r");
  }

  return result;
}

/* Accelerometer De-initialization */
error_t LSM6DSL_AccDeInit(LSM6DSL_t *sensor)
{
  error_t result = ERR_OK;
  uint8_t ctrl = 0x00;

  /* Read control register 1 value */
  result = LSM6DSL_Read(sensor, &ctrl, LSM6DSL_ACC_GYRO_CTRL1_XL, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  /* Clear ODR bits */
  ctrl &= ~(LSM6DSL_ODR_BITPOSITION);

  /* Set Power down */
  ctrl |= LSM6DSL_ODR_POWER_DOWN;

  /* write back control register */
  result = LSM6DSL_Write(sensor, &ctrl, LSM6DSL_ACC_GYRO_CTRL1_XL, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid writing to sensor.\n\r");
  }

  return result;
}

/* Set/Unset Accelerometer in low power mode */
error_t LSM6DSL_AccLowPower(LSM6DSL_t *sensor, uint16_t set_low_pwr)
{
  error_t result = ERR_OK;
  uint8_t ctrl = 0x00;

  /* Read CTRL6_C value */
  result = LSM6DSL_Read(sensor, &ctrl, LSM6DSL_ACC_GYRO_CTRL6_C, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  /* Clear Low Power Mode bit */
  ctrl &= ~(0x10);

  /* Set Low Power Mode */
  if (set_low_pwr)
  {
    ctrl |= LSM6DSL_ACC_GYRO_LP_XL_ENABLED;
  }
  else
  {
    ctrl |= LSM6DSL_ACC_GYRO_LP_XL_DISABLED;
  }

  /* Write back control register */
  result = LSM6DSL_Write(sensor, &ctrl, LSM6DSL_ACC_GYRO_CTRL6_C, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid writing to sensor.\n\r");
  }
  return result;
}

/* Read X, Y & Z Acceleration values */
error_t LSM6DSL_AccReadXYZ(LSM6DSL_t *sensor, int16_t *pData)
{
  error_t result = ERR_OK;

  int16_t pnRawData[3];
  uint8_t ctrlx = 0;
  uint8_t buffer[6];
  uint8_t i = 0;
  float sensitivity = 0;

  /* Read the acceleration control register content */
  result = LSM6DSL_Read(sensor, &ctrlx, LSM6DSL_ACC_GYRO_CTRL1_XL, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  /* Read output register X, Y & Z acceleration */
  LSM6DSL_Read(sensor, buffer, LSM6DSL_ACC_GYRO_OUTX_L_XL, 6);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  for (i = 0; i < 3; i++)
  {
    pnRawData[i] = ((((uint16_t)buffer[2 * i + 1]) << 8) + (uint16_t)buffer[2 * i]);
  }

  /* Normal mode */
  /* Switch the sensitivity value set in the CRTL1_XL */
  switch (ctrlx & 0x0C)
  {
  case LSM6DSL_ACC_FULLSCALE_2G:
    sensitivity = LSM6DSL_ACC_SENSITIVITY_2G;
    break;
  case LSM6DSL_ACC_FULLSCALE_4G:
    sensitivity = LSM6DSL_ACC_SENSITIVITY_4G;
    break;
  case LSM6DSL_ACC_FULLSCALE_8G:
    sensitivity = LSM6DSL_ACC_SENSITIVITY_8G;
    break;
  case LSM6DSL_ACC_FULLSCALE_16G:
    sensitivity = LSM6DSL_ACC_SENSITIVITY_16G;
    break;
  }

  /* Obtain the mg value for the three axis */
  for (i = 0; i < 3; i++)
  {
    pData[i] = (int16_t)(pnRawData[i] * sensitivity);
  }

  return result;
}

/* Gyroscope Initialization */
error_t LSM6DSL_GyroInit(LSM6DSL_t *sensor)
{
  error_t result = ERR_OK;

  uint8_t ctrl = 0x00;
  uint8_t tmp;

  /* Read CTRL2_G */
  result = LSM6DSL_Read(sensor, &tmp, LSM6DSL_ACC_GYRO_CTRL2_G, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  /* Write value to GYRO MEMS CTRL2_G register: FS and Data Rate */
  ctrl = sensor->Gyro.ODR << 4 | sensor->Gyro.Full_Scale << 2;
  tmp &= ~(0xFC);
  tmp |= ctrl;
  result = LSM6DSL_Write(sensor, &tmp, LSM6DSL_ACC_GYRO_CTRL2_G, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid writing to sensor.\n\r");
  }

  /* Read CTRL3_C */
  result = LSM6DSL_Read(sensor, &tmp, LSM6DSL_ACC_GYRO_CTRL3_C, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  /* Write value to ACC MEMS CTRL3_C register: BDU and Auto-increment */
  ctrl = sensor->Config.BDU << 6 | sensor->Config.IF_INC << 2;
  tmp &= ~(0x44);
  tmp |= ctrl;
  result = LSM6DSL_Write(sensor, &tmp, LSM6DSL_ACC_GYRO_CTRL3_C, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid writing to sensor.\n\r");
  }
  return result;
}

/* Gyroscope De-initialization */
error_t LSM6DSL_GyroDeInit(LSM6DSL_t *sensor)
{
  error_t result = ERR_OK;

  uint8_t ctrl = 0x00;

  /* Read control register 2 value */
  result = LSM6DSL_Read(sensor, &ctrl, LSM6DSL_ACC_GYRO_CTRL2_G, 1);
    if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  /* Clear ODR bits */
  ctrl &= ~(LSM6DSL_ODR_BITPOSITION);

  /* Set Power down */
  ctrl |= LSM6DSL_ODR_POWER_DOWN;

  /* write back control register */
  result = LSM6DSL_Write(sensor, &ctrl, LSM6DSL_ACC_GYRO_CTRL2_G, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid writing to sensor.\n\r");
  }
  return result;
}

/* Set/Unset LSM6DSL Gyroscope in low power mode */
error_t LSM6DSL_GyroLowPower(LSM6DSL_t *sensor, uint16_t set_low_pwr)
{
  error_t result = ERR_OK;

  uint8_t ctrl = 0x00;

  /* Read CTRL6_C value */
  result = LSM6DSL_Read(sensor, &ctrl, LSM6DSL_ACC_GYRO_CTRL7_G, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  /* Clear Low Power Mode bit */
  ctrl &= ~(0x80);

  /* Set Low Power Mode */
  if (set_low_pwr)
  {
    ctrl |= LSM6DSL_ACC_GYRO_LP_G_ENABLED;
  }
  else
  {
    ctrl |= LSM6DSL_ACC_GYRO_LP_G_DISABLED;
  }

  /* Write back control register */
  result = LSM6DSL_Write(sensor, &ctrl, LSM6DSL_ACC_GYRO_CTRL7_G, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid writing to sensor.\n\r");
  }
  return result;
}

/* Read the LSM6DSL angular data */
error_t LSM6DSL_GyroReadXYZAngRate(LSM6DSL_t *sensor, uint8_t *pBuffer)
{
  error_t result = ERR_OK;

  uint8_t ctrlg = 0;

  /* Read the gyro control register content */
  result = LSM6DSL_Read(sensor, &ctrlg, LSM6DSL_ACC_GYRO_CTRL2_G, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  /* Read output register X, Y & Z acceleration */
  result = LSM6DSL_Read(sensor, pBuffer, LSM6DSL_ACC_GYRO_OUTX_L_G, 6);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  return result;
}

error_t LSM6DSL_GyroConvertAngRate(LSM6DSL_t *sensor, uint8_t *buffer, float *pfData)
{

  error_t result = ERR_OK;
  int16_t pnRawData[3];
  uint8_t i = 0;
  float sensitivity = 0;
  uint8_t ctrlg = 0;

  for (i = 0; i < 3; i++)
  {
    pnRawData[i] = ((((uint16_t)buffer[2 * i + 1]) << 8) + (uint16_t)buffer[2 * i]);
  }

  /* Normal mode */

  /* Read the gyro control register content */
  result = LSM6DSL_Read(sensor, &ctrlg, LSM6DSL_ACC_GYRO_CTRL2_G, 1);
  if (result != ERR_OK)
  {
    logs_lsm6dsl("Error: invalid reading from sensor.\n\r");
  }

  /* Switch the sensitivity value set in the CRTL2_G */
  switch (ctrlg & 0x0C)
  {
  case LSM6DSL_GYRO_FS_245:
    sensitivity = LSM6DSL_GYRO_SENSITIVITY_245DPS;
    break;
  case LSM6DSL_GYRO_FS_500:
    sensitivity = LSM6DSL_GYRO_SENSITIVITY_500DPS;
    break;
  case LSM6DSL_GYRO_FS_1000:
    sensitivity = LSM6DSL_GYRO_SENSITIVITY_1000DPS;
    break;
  case LSM6DSL_GYRO_FS_2000:
    sensitivity = LSM6DSL_GYRO_SENSITIVITY_2000DPS;
    break;
  }

  /* Obtain the mg value for the three axis */
  for (i = 0; i < 3; i++)
  {
    pfData[i] = (float)(pnRawData[i] * sensitivity);
  }

  return result;
}
