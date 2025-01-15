/*
 * MB_Addresses.h
 *
 *  Created on: May 12, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_MB_ADDRESSES_H_
#define INC_MB_ADDRESSES_H_



/* Coils addresses
 * FC: 02 read discrete inputs
*/
typedef enum MB_DIAddrEnum{
	/*Digital outputs------------*/
	MB_DIA_DI1_STATUS = 10001U,
	MB_DIA_DI2_STATUS,
	MB_DIA_DI3_STATUS,
	MB_DIA_DI4_STATUS,
	MB_DIA_DI5_STATUS,
	MB_DIA_DI6_STATUS,
	MB_DIA_DI7_STATUS,
	MB_DIA_DI8_STATUS,
	MB_DIA_MAX,
}MB_DIAddrEnum;



// Coils addresses write coil: FC-05, readCOil: 01
typedef enum MB_CoilAddrEnum{
	/*Digital outputs------------*/
	MB_COA_DO1_ENABLE = 1U,
	MB_COA_DO2_ENABLE,
	MB_COA_DO3_ENABLE,
	MB_COA_DO4_ENABLE,
	MB_COA_DO5_ENABLE,
	MB_COA_DO6_ENABLE,
	MB_COA_DO7_ENABLE,
	MB_COA_DO8_ENABLE,

	/*Modbus------------*/
	MB_COA_MB_SERIAL_SAVE_CONFIG = 21,
	MB_COA_MB_KEEP_ALIVE_ENABLE,

	/*Debug------------*/
	MB_COA_DEBUG_ENABLE = 31,
	MB_COA_MAX,
}MB_CoilAddrEnum;




// Input register addresses
typedef enum MB_IRAddrEnum{
	/*Modbus------------*/
	MB_IRA_SLAVE_ADDRESS = 30001U,

	MB_IRA_MAX,
}MB_IRAddrEnum;



// Holding register addresses
typedef enum MB_HRAddrEnum{
	 /*DIs------------------------------*/
	MB_HRA_DI0_DEBOUNCE_DELAY = 40001U,
	MB_HRA_DI1_DEBOUNCE_DELAY,
	MB_HRA_DI2_DEBOUNCE_DELAY,
	MB_HRA_DI3_DEBOUNCE_DELAY,
	MB_HRA_DI4_DEBOUNCE_DELAY,
	MB_HRA_DI5_DEBOUNCE_DELAY,
	MB_HRA_DI6_DEBOUNCE_DELAY,
	MB_HRA_DI7_DEBOUNCE_DELAY,

	/*Modbus Serial---------------------*/
	MB_HRA_MB_SERIAL_BAUDRATE = 40021U,
	MB_HRA_MB_SERIAL_DATABIT,
	MB_HRA_MB_SERIAL_PARITY,
	MB_HRA_MB_SERIAL_STOPBIT,
	MB_HRA_MB_KEEP_ALIVE_TIMEOUT,
	MB_HRA_MB_KEEP_ALIVE,

	MB_HRA_MAX,
}MB_HRAddrEnum;


#endif /* INC_MB_ADDRESSES_H_ */
