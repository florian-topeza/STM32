/**
 * @file        console.h
 * @author      Florian Topeza
 * @brief       Redirect printf function to print through UART (for example in a serial monitor)
 * @version     0.1
 * @date        2026-05-30
 *
 * @copyright   Copyright (c) 2026 Florian Topeza
 *
 */

/* ==== Includes ==== */
#include "console.h"

/* ==== Static variable declaration ==== */
static UART_HandleTypeDef *p_console_uart;

/* ==== Functions ==== */

/**
 * @brief 					Initialize the console with the specified UART handle
 *
 * @param p_uart_handle 	Pointer to the UART handle to use
 * @return int
 */
int init_console(UART_HandleTypeDef *p_uart_handle) {

    p_console_uart = p_uart_handle;

    return 0;
}

/**
 * @brief   		Write a message in STDOUT or STDERR
 * @param   fd 		File descriptor
 * @param   ptr 	Message pointer
 * @param   len 	Message length
 * @retval  		len if write is successful
 * @retval  		EIO (Error IO) if writing fails
 * @retval  		EBADF if fd is not STDOUT_FILENO or STDERR_FILENO
 */
int _write(int fd, char *ptr, int len)
{
  if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
  {
	  HAL_StatusTypeDef status = HAL_UART_Transmit(p_console_uart, (uint8_t *)ptr, len, CONSOLE_UART_TIMEOUT);
	  if (status == HAL_OK) {
		  return len;
	  }
	  else
	  {
		  return EIO;
	  }
  }

  return EBADF;
}

/**
 * @brief   		Read a message from STDIN
 * @param   fd 		File descriptor
 * @param   ptr 	Message pointer
 * @retval  		1 if read is successful
 * @retval  		EIO (Error IO) if reading fails
 * @retval  		EBADF if fd is not STDIN_FILENO
 */
int _read(int fd, char *ptr)
{
  if (fd == STDIN_FILENO)
  {
	  HAL_StatusTypeDef status = HAL_UART_Receive(p_console_uart, (uint8_t *)ptr, 1, CONSOLE_UART_TIMEOUT);
	  if (status == HAL_OK) {
		  return 1;
	  }
	  else
	  {
		  return EIO;
	  }
  }

  return EBADF;
}
