/**
 * @file ds18b20.c
 * @author Florian Topeza
 * @brief Minimal driver to interface DS18B20 temperature sensors with STM32H7 microcontroller
 * @version 0.1
 * @date 2026-05-30
 *
 * @copyright Copyright (c) 2026 Florian Topeza
 *
 */

/* ==== Includes ==== */
#include "ds18b20.h"

/* ==== Static functions declarations ==== */

/**
 * @defgroup DS18B20_STATIC_INIT Sensor initialization helpers
 *
 */

/**
 * @brief Initialize timer for onewire communication
 *
 * @param sensor Pointer to the handle structure in use
 * @return error_t
 */
static error_t DS18B20_Timer_Init(DS18B20_t *sensor);

/**
 * @brief Start sensor
 *
 * @param sensor Pointer to the handle structure in use
 * @return error_t
 */
static error_t DS18B20_Start(DS18B20_t *sensor);

/** @} */

/**
 * @defgroup DS18B20_STATIC_RW Onewire read and write helpers
 *
 */

/**
 * @brief Generate a delay in microseconds (HAL_Delay can only manage delays more than 1ms)
 *
 * @param sensor Pointer to the sensor handle in use
 * @param us Desired microseconds delay
 */
static void DS18B20_delay(DS18B20_t *sensor, uint16_t us);

/**
 * @brief Write a byte on the onewire bus
 *
 * @param sensor Pointer to the sensor handle in use
 * @param data Byte of data to be written
 */
static void DS18B20_writeData(DS18B20_t *sensor, uint8_t data);

/**
 * @brief Write a 1 on the onewire bus
 *
 * @param sensor Pointer to the sensor handle in use
 */
static void DS18B20_write1(DS18B20_t *sensor);

/**
 * @brief Write a 0 on the onewire bus
 *
 * @param sensor Pointer to the sensor handle in use
 */
static void DS18B20_write0(DS18B20_t *sensor);

/**
 * @brief Read a byte on the onewire bus
 *
 * @param sensor Pointer to the sensor handle in use
 * @return uint8_t
 */
static uint8_t DS18B20_readByte(DS18B20_t *sensor);

/**
 * @brief Read a bit on the onewire bus
 *
 * @param sensor Pointer to the sensor handle in use
 * @return uint8_t
 */
static uint8_t DS18B20_read(DS18B20_t *sensor);

/** @} */

/**
 * @defgroup DS18B20_STATIC_SEARCH Search algorithm helpers
 *
 */

/**
 * @brief Initialize onewire search
 *
 * @param state Pointer to the search state handle to use
 */
static void onewireSearchInit(onewire_search_state_t *state);

/**
 * @brief Search next sensor on the bus
 *
 * @param sensor Pointer to the sensor handle in use
 * @param state Pointer to the search state handle in use
 * @return error_t
 */
static error_t searchNext(DS18B20_t *sensor, onewire_search_state_t *state);

/**
 * @brief Search all devices on the bus
 *
 * @param sensor Pointer to the sensor handle in use
 * @param state Pointer to the search state handle in use
 * @return error_t
 */
static error_t searchDevices(DS18B20_t *sensor, uint8_t command, onewire_search_state_t *state);

/**
 * @brief Generate CRC for a given input byte
 *
 * @param initial_crc Initial CRC value
 * @param input Input byte for CRC calculation
 * @return uint8_t
 */
static uint8_t crcGenerator(uint8_t initial_crc, uint8_t input);

/**
 * @brief  Check if the address is valid by computing the CRC
 *
 * @param ROM_code Input ROM code to validate
 * @return error_t
 */
static error_t addressValid(const uint8_t ROM_code[]);

/** @} */

static error_t DS18B20_Timer_Init(DS18B20_t *sensor)
{
	error_t result = ERR_OK;

	if (HAL_TIM_Base_Start(&sensor->htim) != HAL_OK)
	{
		result = ERR_HAL; // timer failed to start: microsecond delays would never elapse
	}

	return result;
}

error_t DS18B20_Init(DS18B20_t *sensor, TIM_HandleTypeDef *sensor_htim, GPIO_TypeDef * sensor_gpio_port, uint16_t sensor_gpio_pin)
{
	error_t result = ERR_OK;

	if (sensor == NULL || sensor_htim == NULL || sensor_gpio_port == NULL)
	{
		result = ERR_INVALID_ARGUMENT;
	}
	else
	{
		sensor->htim = *sensor_htim;
		sensor->gpio_port = sensor_gpio_port;
		sensor->gpio_pin = sensor_gpio_pin;

		result = DS18B20_Timer_Init(sensor);
	}

	return result;
}

static void DS18B20_delay(DS18B20_t *sensor, uint16_t us)
{

	__HAL_TIM_SET_COUNTER(&sensor->htim, 0); // set the counter value at 0
	while (__HAL_TIM_GET_COUNTER(&sensor->htim) < us)
		; // wait for the counter to reach the us input in the parameter
}

static error_t DS18B20_Start(DS18B20_t *sensor)
{
	error_t result = ERR_OK;

	HAL_GPIO_WritePin(sensor->gpio_port, sensor->gpio_pin, GPIO_PIN_RESET); // pull the pin low
	DS18B20_delay(sensor, 480);												// wait at least 480µs low according to datasheet

	HAL_GPIO_WritePin(sensor->gpio_port, sensor->gpio_pin, GPIO_PIN_SET); // release the pin

	DS18B20_delay(sensor, 80); // wait 80µs to receive presence pulse according to datasheet
							   // 80µs = 60µs max for transition + 20µs to receive presence pulse

	if ((HAL_GPIO_ReadPin(sensor->gpio_port, sensor->gpio_pin)))
	{ // check if pin is low

		result = ERR_NO_DEVICE; // if the pin is high, no presence pulse is detected
	}

	DS18B20_delay(sensor, 400); // at least 480 us DS18B20_delay totally, according to datasheet

	return result;
}

static uint8_t DS18B20_read(DS18B20_t *sensor)
{

	uint8_t response = 0;

	HAL_GPIO_WritePin(sensor->gpio_port, sensor->gpio_pin, GPIO_PIN_RESET); // set pin low
	DS18B20_delay(sensor, 3);												// wait at least 1µs low according to datasheet

	HAL_GPIO_WritePin(sensor->gpio_port, sensor->gpio_pin, GPIO_PIN_SET); // release the pin

	DS18B20_delay(sensor, 10); // data is valid for 15µs max after master releases the data line
							   // here we wait 8µs before DS18B20_reading data, to remain under the 15µs

	if (HAL_GPIO_ReadPin(sensor->gpio_port, sensor->gpio_pin))
	{					 // if the sensor pulled the line to high
		response = 1; // we DS18B20_read 1, otherwise 0
	}

	DS18B20_delay(sensor, 52); // wait another 50µs to complete the 60µs of a DS18B20_read time slot
							   // and another 2µs to ensure the minimum 1µs recovery between DS18B20_read slots
	return response;
}

static uint8_t DS18B20_readByte(DS18B20_t *sensor)
{

	uint8_t value = 0;

	for (uint8_t i = 0; i < 8; i++)
	{

		uint8_t bit = DS18B20_read(sensor); // DS18B20_read a bit
		value |= bit << i;
	}

	return value;
}

static void DS18B20_write0(DS18B20_t *sensor)
{

	HAL_GPIO_WritePin(sensor->gpio_port, sensor->gpio_pin, GPIO_PIN_RESET); // pull the pin low

	DS18B20_delay(sensor, 60); // wait at least 60µs low according to datasheet to write 0

	HAL_GPIO_WritePin(sensor->gpio_port, sensor->gpio_pin, GPIO_PIN_SET); // release the pin

	DS18B20_delay(sensor, 5); // wait another 1µs to for recovery between write time slots
}

static void DS18B20_write1(DS18B20_t *sensor)
{

	HAL_GPIO_WritePin(sensor->gpio_port, sensor->gpio_pin, GPIO_PIN_RESET); // pull the pin low

	DS18B20_delay(sensor, 5); // wait less than 15µs low according to datasheet to write 1

	HAL_GPIO_WritePin(sensor->gpio_port, sensor->gpio_pin, GPIO_PIN_SET); // release the pin

	DS18B20_delay(sensor, 60); // wait another 60µs to wait 65µs totally, according to datasheet (write time slot + recovery)

}

static void DS18B20_writeData(DS18B20_t *sensor, uint8_t data)
{

	for (uint8_t i = 0; i < 8; i++)
	{

		if ((data & (1 << i)) == (1 << i))
		{ // if the bit is high

			DS18B20_write1(sensor); // write 1
		}
		else
		{

			DS18B20_write0(sensor); // write 0 otherwise
		}
	}
}

static void onewireSearchInit(onewire_search_state_t *state)
{
	state->last_zero_branch = -1;
	state->done = false;

	// Zero-fill the address
	for (int i = 0; i < 8; i++)
	{

		state->address[i] = 0;
	}
}

static error_t searchNext(DS18B20_t *sensor, onewire_search_state_t *state)
{

	error_t result = ERR_OK;

	// States of ROM search DS18B20_reads
	enum
	{
		kConflict = 0x00,
		kZero = 0x02,
		kOne = 0x01,
	};

	// Value to write to the current position
	uint8_t bitValue = 0;

	// Keep track of the last zero branch within this search
	// If this value is not updated, the search is complete
	int8_t locallast_zero_branch = -1;

	for (uint8_t bitPosition = 0; bitPosition < 64; bitPosition++)
	{

		// Calculate bitPosition as an index in the address array
		// This is written as-is for DS18B20_readability. Compilers should reduce this to bit shifts and tests
		uint8_t byteIndex = bitPosition / 8;
		uint8_t bitIndex = bitPosition % 8;

		// DS18B20_read the current bit and its complement from the bus
		uint8_t DS18B20_reading = 0;
		DS18B20_reading |= DS18B20_read(sensor);	  // Bit
		DS18B20_reading |= DS18B20_read(sensor) << 1; // Complement of bit (negated)

		switch (DS18B20_reading)
		{
		case kZero:
		case kOne:
			// Bit was the same on all responding devices: it is a known value
			// The first bit is the value we want to write (rather than its complement)
			bitValue = (DS18B20_reading & 0x1);
			break;

		case kConflict:
			// Both 0 and 1 were written to the bus
			// Use the search state to continue walking through devices
			if (bitPosition == state->last_zero_branch)
			{
				// Current bit is the last position the previous search chose a zero: send one
				bitValue = 1;
			}
			else if (bitPosition < state->last_zero_branch)
			{
				// Before the last_zero_branch position, repeat the same choices as the previous search
				bitValue = state->address[byteIndex] & (1 << bitIndex);
			}
			else
			{
				// Current bit is past the last_zero_branch in the previous search: send zero
				bitValue = 0;
			}

			// Remember the last branch where a zero was written for the next search
			if (bitValue == 0)
			{
				locallast_zero_branch = bitPosition;
			}

			break;

		default:
			// If we see "11" there was a problem on the bus (no devices pulled it low)
			result = ERR_HAL;
		}

		// Write bit into address
		if (bitValue == 0)
		{
			state->address[byteIndex] &= ~(1 << bitIndex);
		}
		else
		{
			state->address[byteIndex] |= (bitValue << bitIndex);
		}

		// Write bit to the bus to continue the search
		if (bitValue == 0)
		{
			DS18B20_write0(sensor);
		}
		else
		{
			DS18B20_write1(sensor);
		}
	}

	// If the no branch points were found, mark the search as done.
	// Otherwise, mark the last zero branch we found for the next search
	if (locallast_zero_branch == -1)
	{
		state->done = true;
	}
	else
	{
		state->last_zero_branch = locallast_zero_branch;
	}

	// DS18B20_read a whole address - return ERR_OK
	return result;
}

/* Search for devices on the 1-Wire bus */
static error_t searchDevices(DS18B20_t *sensor, uint8_t command, onewire_search_state_t *state)
{
	error_t result = ERR_OK;
	// Bail out if the previous search was the end or if no sensor on the bus
	if ( (state->done) || (DS18B20_Start(sensor) != ERR_OK) )
	{
		result = ERR_NO_DEVICE;
	}
	else
	{
		DS18B20_writeData(sensor, command);
		result = searchNext(sensor, state);
	}

	return result;
}

static uint8_t crcGenerator(uint8_t initial_crc, uint8_t input)
{

	// Input is supposed to be a Byte where the most left bit is MSB (input not reversed).

	// 1. Calculate the CRC of data 0x00 to 0xFF and store them into one array in order.
	const uint8_t crc8_table[256] = {
		0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83, 0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
		0x9D, 0xC3, 0x21, 0x7F, 0xFC, 0xA2, 0x40, 0x1E, 0x5F, 0x01, 0xE3, 0xBD, 0x3E, 0x60, 0x82, 0xDC,
		0x23, 0x7D, 0x9F, 0xC1, 0x42, 0x1C, 0xFE, 0xA0, 0xE1, 0xBF, 0x5D, 0x03, 0x80, 0xDE, 0x3C, 0x62,
		0xBE, 0xE0, 0x02, 0x5C, 0xDF, 0x81, 0x63, 0x3D, 0x7C, 0x22, 0xC0, 0x9E, 0x1D, 0x43, 0xA1, 0xFF,
		0x46, 0x18, 0xFA, 0xA4, 0x27, 0x79, 0x9B, 0xC5, 0x84, 0xDA, 0x38, 0x66, 0xE5, 0xBB, 0x59, 0x07,
		0xDB, 0x85, 0x67, 0x39, 0xBA, 0xE4, 0x06, 0x58, 0x19, 0x47, 0xA5, 0xFB, 0x78, 0x26, 0xC4, 0x9A,
		0x65, 0x3B, 0xD9, 0x87, 0x04, 0x5A, 0xB8, 0xE6, 0xA7, 0xF9, 0x1B, 0x45, 0xC6, 0x98, 0x7A, 0x24,
		0xF8, 0xA6, 0x44, 0x1A, 0x99, 0xC7, 0x25, 0x7B, 0x3A, 0x64, 0x86, 0xD8, 0x5B, 0x05, 0xE7, 0xB9,
		0x8C, 0xD2, 0x30, 0x6E, 0xED, 0xB3, 0x51, 0x0F, 0x4E, 0x10, 0xF2, 0xAC, 0x2F, 0x71, 0x93, 0xCD,
		0x11, 0x4F, 0xAD, 0xF3, 0x70, 0x2E, 0xCC, 0x92, 0xD3, 0x8D, 0x6F, 0x31, 0xB2, 0xEC, 0x0E, 0x50,
		0xAF, 0xF1, 0x13, 0x4D, 0xCE, 0x90, 0x72, 0x2C, 0x6D, 0x33, 0xD1, 0x8F, 0x0C, 0x52, 0xB0, 0xEE,
		0x32, 0x6C, 0x8E, 0xD0, 0x53, 0x0D, 0xEF, 0xB1, 0xF0, 0xAE, 0x4C, 0x12, 0x91, 0xCF, 0x2D, 0x73,
		0xCA, 0x94, 0x76, 0x28, 0xAB, 0xF5, 0x17, 0x49, 0x08, 0x56, 0xB4, 0xEA, 0x69, 0x37, 0xD5, 0x8B,
		0x57, 0x09, 0xEB, 0xB5, 0x36, 0x68, 0x8A, 0xD4, 0x95, 0xCB, 0x29, 0x77, 0xF4, 0xAA, 0x48, 0x16,
		0xE9, 0xB7, 0x55, 0x0B, 0x88, 0xD6, 0x34, 0x6A, 0x2B, 0x75, 0x97, 0xC9, 0x4A, 0x14, 0xF6, 0xA8,
		0x74, 0x2A, 0xC8, 0x96, 0x15, 0x4B, 0xA9, 0xF7, 0xB6, 0xE8, 0x0A, 0x54, 0xD7, 0x89, 0x6B, 0x35};

	// 2. Do one XOR operation with the input data byte and the initial byte.
	uint8_t index = input ^ initial_crc;

	// 3. Use the calculated result in above second step as the array index to retrieve
	// its CRC value from the CRC array built in first step.

	uint8_t final_crc = crc8_table[index];

	return final_crc;
}

static error_t addressValid(const uint8_t ROM_code[])
{
	error_t result = ERR_OK;

	// Initially, the CRC is equal to 0 according to datasheet
	uint8_t initial_crc = 0x00;

	// Variable to store the computed CRC
	uint8_t computed_crc = 0x00;

	// Compute the CRC by iterating on the CRC computed for each data byte
	for (int i = 0; i < 7; i++)
	{
		computed_crc = crcGenerator(initial_crc, ROM_code[i]);
		initial_crc = computed_crc;
	}

	// Compare computed CRC with the stored one, sent by the sensor.
	if (computed_crc != ROM_code[7])
	{
		result = ERR_INVALID_ARGUMENT;
	}

	return result;
}

void DS18B20_Search(DS18B20_t *sensor, uint64_t ROM_codes_array[])
{
	// Searching ROM codes of all sensors is more complicated than just sending
	// the Search ROM Command and wait for responses.
	// It needs to perform a Search ROM Cycle that includes sending and receiving
	// data through a certain process.

	onewire_search_state_t search_state;
	onewireSearchInit(&search_state);

	// The detected ROM Codes will be stored in an array of uint64_t, provided as argument
	// of the function (ROM_codes_array). Its lenght shall be greater than the number of
	// sensors on the bus.

	uint8_t index = 0;
	uint8_t count = 0;

	log_ds18b20("Searching devices...\n\r");

	while (searchDevices(sensor, SEARCH_ROM, &search_state) == ERR_OK)
	{

		// Store the detected ROM code in the field ROM code of the current
		// ROM_code_Address structure.

		ROM_codes_array[index] = 0ULL;

		for (int i = 0; i < 8; i++)
		{
			// ROM codes are stored MSB first
			ROM_codes_array[index] |= search_state.address[i] << 8*(7-i);
		}

		// Check is the received ROM code is valid
		if (addressValid(search_state.address) == ERR_OK)
		{
			// Display through Serial
			log_ds18b20("Received valid ROM code !\n\r");
		}
		else
		{
			// Display through Serial
			log_ds18b20("Received ROM code not valid !\n\r");
		}

		count++;

		// Display the detected ROM Code of the sensor through serial.
		log_ds18b20("ROM Code for sensor %u: %x \n\r", count, ROM_codes_array[index]);

		index++;
	}

}

void DS18B20_GetTemp(DS18B20_t *sensor, const uint64_t ROM_codes_array[], int16_t *temperature)
{

	// We will ask the temperature to each sensor and display it through Serial
	// if the console is enabled.

	uint8_t ROM_code_byte = 0;
	uint8_t count = 0;

	// We will go through the array of ROM Codes
	while (ROM_codes_array[count] != 0)
	{
		DS18B20_Start(sensor);				  // Initiate the transaction on the 1-Wire bus
											  // sensor can be any of the sensors of the bus
		DS18B20_writeData(sensor, MATCH_ROM); // Match ROM

		for (uint8_t i = 0; i < 8; i++)
		{ // Send the ROM Code MSB first
			ROM_code_byte = ( ROM_codes_array[count] >> 8*(7-i) ) & 0xFF;
			DS18B20_writeData(sensor, ROM_code_byte);
		}

		DS18B20_writeData(sensor, CONVERT_T); // Temperature conversion

		DS18B20_Start(sensor);				  // Initiate the transaction
		DS18B20_writeData(sensor, MATCH_ROM); // Match ROM

		for (uint8_t i = 0; i < 8; i++)
		{ // Send the ROM Code MSB first again

			ROM_code_byte = ( ROM_codes_array[count] >> 8*(7-i) ) & 0xFF;
			DS18B20_writeData(sensor, ROM_code_byte);
		}

		DS18B20_writeData(sensor, READ_SCRATCHPAD); // Read data

		uint8_t Temperature_byte_1 = DS18B20_readByte(sensor); // First byte of data
		uint8_t Temperature_byte_2 = DS18B20_readByte(sensor); // Second byte of data

		// Temperature data is made of the concatenation of the two data bytes.
		// The sensor sends the data LSB first.
		// The DS18B20 encodes the value as a signed (two's complement) 16-bit integer
		// with 4 fractional bits, i.e. in units of 1/16 degC. Cast to int16_t before
		// storing so negative temperatures are preserved correctly.
		int16_t Temperature = (int16_t)((Temperature_byte_2 << 8) | Temperature_byte_1);
		temperature[count] = Temperature;

		count += 1; // Increment the sensor count

		// Display the temperature of the sensor through Serial (in 1/16 degC units)
		log_ds18b20("Temperature of sensor %i: %d\n\r", count, Temperature);
	}
}
