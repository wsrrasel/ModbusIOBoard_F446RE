/*
 * Flash_Address.h
 *
 *  Created on: May 25, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_FLASH_ADDRESS_H_
#define INC_FLASH_ADDRESS_H_


typedef enum FS_Addresses{
	FSA_NONE = 0,
	/*DIs---------------------------*/
	FSA_DI0_DEBOUNCE_DELAY,
	FSA_DI1_DEBOUNCE_DELAY,
	FSA_DI2_DEBOUNCE_DELAY,
	FSA_DI3_DEBOUNCE_DELAY,

	/*Modbus Serials-------------------*/
	FSA_MB_SERIAL_BAUDRATE,
	FSA_MB_SERIAL_DATABIT,
	FSA_MB_SERIAL_PARITY,
	FSA_MB_SERIAL_STOPBIT,

	/*Debug------------------------------*/
	FSA_DEBUG_ENABLE,
	FSA_MAX,
}FS_Addresses_te;

#endif /* INC_FLASH_ADDRESS_H_ */
