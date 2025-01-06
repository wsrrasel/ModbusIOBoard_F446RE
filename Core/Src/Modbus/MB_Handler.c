/*
 * MB_Handler.c
 *
 *  Created on: Jan 18, 2022
 *      Author: RASEL_EEE
 */

#include "MB_Handler.h"
#include "MB_Addresses.h"
#include "MB_Config.h"



MB_RTUHandler mbHandle;



uint8_t (*MB_readDICallback)(MB_DIAddrEnum address, uint8_t *value);
uint8_t (*MB_writeCOCallback)(MB_CoilAddrEnum address, uint8_t value);
uint8_t (*MB_readCOCallback)(MB_CoilAddrEnum address, uint8_t *value);
uint8_t (*MB_readIRCallback)(MB_IRAddrEnum address, uint16_t *value);
uint8_t (*MB_writeHRCallback)(MB_HRAddrEnum address, uint16_t value);
uint8_t (*MB_readHRCallback)(MB_HRAddrEnum address, uint16_t *value);


uint16_t MB_getQuantity(uint8_t firstByteAddress);				// to get the quantity
uint16_t MB_getSCRValue(void);									// to get the output value for single coil or register
uint8_t MB_checkCoilDataQtyAddr(uint16_t maxAddress);							// to check the coil data quantity and address are valid or not
uint8_t MB_checkMCQtyAddrByteCount(void);			   			// to check the multiple coils data quantity, address and byte count for the write multiple output coils
uint8_t MB_checkMRQtyAddrByteCount(uint16_t maxAddress);
uint8_t MB_checkDataQtyAddr(uint16_t startingAddress,uint16_t maxAddress, uint16_t quantity, uint16_t maxQuantity);	// to check the data quantity and address for the coils and registers
uint8_t MB_checkDataQuantity(uint16_t quantity, uint16_t maxQuantity);	// to check requested data quantity
uint8_t MB_checkDataAddress(uint16_t startAddress, uint16_t maxAddress, uint16_t quantity);	// to check requested data address							// to check the single register value
uint8_t MB_checkSCRAddr(uint16_t address, uint16_t maxAddress);
uint8_t MB_checkSRValue(uint16_t value);
uint8_t MB_checkSRValueAddr(void);
uint16_t MB_getSCRAddress(void);




uint8_t MB_getCoil( uint16_t address, uint8_t *value);
uint8_t MB_getCoilsInByte(uint8_t *byteArray, uint8_t size, uint16_t startAddress, uint16_t endAddress);



uint8_t MB_getIRValue(uint16_t address, uint16_t *value);
uint8_t MB_getIRsValue(uint16_t *value, uint8_t size, uint16_t startAddress, uint16_t lastAddresss);
uint8_t MB_getHRsValue(uint16_t *pData, uint16_t startAddress, uint16_t lastAddresss);
uint8_t MB_setHRValue(uint16_t address, uint16_t data);


uint8_t MB_readDI(void);											// to read discrete input status
uint8_t MB_readCoils(void);											// to read coils status
uint8_t MB_writeSC(void);											// to write single coil
uint8_t MB_writeMC(void);											// to write multiple coils
uint8_t MB_readIR(void);
uint8_t MB_readHR(void);
uint8_t MB_writeSHR(void);
uint8_t MB_writeMHR(void);

/**
  * @brief To initialize the modbus settings,
  * @param none
  * @retval none
  */
void MB_init(MB_RTUHandler *_mbHandle){
	/*
	 * Slave address should be configurable,
	 * Default slave address will be 01 for all the devices
	 * */
	mbHandle.is_req_cpy_to_resp = 0U;
}


void MB_SetHandlerInstance(MB_RTUHandler *_mbHandle){
	mbHandle = *_mbHandle;
}

MB_RTUHandler *MB_GetHandlerInstance(void){
	return &mbHandle;
}


/**
  * @brief sets requested modbus pdu buffer
  * @param buff: pdu buffer pointer
  * @param size: pdu buffer size
  * @retval none
  */
void MB_setReqBuff(uint8_t *buff, uint8_t size){
	mbHandle.rxBuffer = buff;
	mbHandle.rxBufferSize = size;

}

/**
  * @brief gets response of the requested modbus pdu buffer
  * @param buff: pdu buffer pointer
  * @param size: pdu buffer size
  * @retval none
  */

void MB_setRespBuff(uint8_t *buff){
	mbHandle.txBuffer = buff;
}
/*
 * get response buffer size
 * */
uint8_t MB_getRespBuffSize(void){
	return mbHandle.txBufferSize;
}

/**
  * @brief To execute the request which is received from the master
  * @param functionCode = the request function code
  * @retval 2-6,8,10-11, and 255; where 2-6,8 & 10-11 = exception code, and 255 = no exception occurred
  */
uint8_t MB_executeRequest(uint8_t functionCode){
	uint8_t exeStatus = MB_EC_ILF_1;
	mbHandle.txBufferSize = 0u;
	switch (functionCode) {
		case MB_FC_RC_01:
			exeStatus = MB_readCoils();
			break;
		case MB_FC_RDI_02:
			exeStatus = MB_readDI();
			break;
		case MB_FC_RHR_03:
			exeStatus = MB_readHR();
			break;
		case MB_FC_RIR_04:
			exeStatus = MB_readIR();
			break;
		case MB_FC_WSC_05:
			exeStatus = MB_writeSC();
			break;
		case MB_FC_WSR_06:
			exeStatus = MB_writeSHR();
			break;
		case MB_FC_RES_07:

			break;

		case MB_FC_WMC_15:
			exeStatus = MB_writeMC();
			break;
		case MB_FC_WMR_16:
			exeStatus = MB_writeMHR();
			break;
		case MB_FC_MWR_22:

			break;
		case MB_FC_RWMR_23:

			break;
		default:
			exeStatus = MB_EC_ILF_1;
			break;
	}
	return exeStatus;
}


/**
  * @brief To clear the modbus PDU buffer,
  * @param none
  * @retval none
  */
void MB_ClearBuffer(void){
	memset(mbHandle.rxBuffer,0U,mbHandle.rxBufferSize);
	memset(mbHandle.txBuffer,0U,mbHandle.txBufferSize);
	mbHandle.rxBufferSize = 0U;
	mbHandle.txBufferSize = 0U;
}

/**
  * @brief It append data to response PDU
  * @param data, data byte to append to response PDU
  * @retval none
  */
void MB_appendToRespPDU(uint8_t data){
	mbHandle.txBuffer[mbHandle.txBufferSize++] = data;
	mbHandle.is_req_cpy_to_resp = 0U;
}


/**
  * @brief It append slave address and function code to response PDU
  * @param none
  * @retval none
  */
void MB_appendToRespPDU_SAFC(void){
	MB_appendToRespPDU(MB_getSlaveAddress());
	MB_appendToRespPDU(MB_getFunctionCode());
}






/**
  * @brief It appends CRC16 to response PDU
  * @param none
  * @retval none
  */
void MB_appendToRespPDU_CRC(void){
	if(!mbHandle.is_req_cpy_to_resp){
		uint16_t crc16 = MB_CalcCRC16(mbHandle.txBuffer, mbHandle.txBufferSize);			// calculates CRC
		MB_appendToRespPDU((uint8_t)(crc16 & 0x00FF));
		MB_appendToRespPDU((uint8_t)(crc16>>8));
	}
	mbHandle.is_req_cpy_to_resp = 0U;
}

/**
  * @brief It copies request PDU to response PDU
  * @param none
  * @retval none
  */
void MB_CopyReqToRespPDU(void){
	memcpy(mbHandle.txBuffer, mbHandle.rxBuffer, mbHandle.rxBufferSize);
//	mbHandle.txBuffer = mbHandle.rxBuffer;
	mbHandle.txBufferSize = mbHandle.rxBufferSize;
	mbHandle.is_req_cpy_to_resp = 1U;
}



/**
  * @brief To build exception response
  * @param None
  * @retval 1 or 0, if calculated CRC is matched with received CRC it will return 1, otherwise it will return 0
  */
void MB_excepRespBuilder(uint8_t excepCode){
	MB_appendToRespPDU(MB_getSlaveAddress());							// appends the slave address to the response PDU
	MB_appendToRespPDU(MB_EFC_OFFSET + MB_getFunctionCode());			// appends the exception function code
	MB_appendToRespPDU(excepCode);										// appends exception code


}


/**
  * @brief To check the function code
  * @param functionCode - received function code
  * @retval 1U or 0U, returns 1U for valid function code, or return 0U for invalid function code
  */
uint8_t MB_isFunctionCodeValid(uint8_t functionCode){
	// 1-227 is the range of supported function code public + user define function code
	return ((functionCode >= MB_FC_RC_01 && functionCode <= MB_FC_DIAG_08)
	|| (functionCode == MB_FC_GCEC_11 )
	|| (functionCode >= MB_FC_WMC_15 && functionCode <= MB_FC_WMR_16)
	|| (functionCode >= MB_FC_MWR_22 && functionCode <= MB_FC_RWMR_23)
	|| (functionCode == 66u)
	);
}

/**
  * @brief To check the valid data quantity and address for the coils
  * @param none
  * @retval 0, 2 & 3, where 0 = no exception, 2 = illegal data address exception, 3 = illegal data value exception
  */
uint8_t MB_checkCoilDataQtyAddr(uint16_t maxAddress){
	return MB_checkDataQtyAddr(MB_getStartAddress(MB_FC_INDEX+1), maxAddress, MB_getQuantity(MB_FC_INDEX+3), MB_MAX_COILS);
}



/**
  * @brief To check the valid data quantity and address for the coils
  * @param none
  * @retval 0, 2 & 3, where 0 = no exception, 2 = illegal data address, 3 = illegal data value
  *
  */

uint8_t MB_checkMCQtyAddrByteCount(void){
	uint8_t clc_byte_count = (MB_getQuantity(MB_FC_INDEX+3)/8)+(MB_getQuantity(MB_FC_INDEX+3)%8>0);		// to calculate byte count from the output quantity
	if(MB_checkDataQuantity(MB_getQuantity(MB_FC_INDEX+3), (MB_COA_MAX-MB_CO_ADDR_OFFSET))
			&& (clc_byte_count == mbHandle.rxBuffer[MB_FC_INDEX+5])){				//multiple coils write checks valid quantity of coils
		if(MB_checkDataAddress(MB_getStartAddress(MB_FC_INDEX+1), (MB_COA_MAX-MB_CO_ADDR_OFFSET), MB_getQuantity(MB_FC_INDEX+3))){		// MAX coils/registers address 10000, checks valid data address and address+quantity of coils, Max value will not exceed 9999
			return 0;
		}else{
			return MB_EC_ILDA_2;										// returns exception code - 2 for illegal data address and address+quantity
		}
	}else{
		return MB_EC_ILDV_3;											// returns exception code - 3 for illegal data value/quantity
	}

}


/**
 * @brief Checks the validity of multiple register quantity and address.
 *
 * This function verifies that the quantity of registers requested for writing
 * is within the allowed limits and that the specified start address is valid.
 * It also checks if the byte count provided in the received buffer matches
 * the expected count based on the quantity.
 *
 * @param[in] maxAddress The maximum valid address for the registers.
 *
 * @return uint8_t Returns 0 if the quantity and address are valid;
 *         otherwise, returns an exception code:
 *         - MB_EC_ILDA_2: Illegal data address (invalid start address or
 *           quantity exceeds range).
 *         - MB_EC_ILDV_3: Illegal data value or quantity (exceeds limits).
 *
 * @note The maximum number of registers that can be written is defined by
 *       the `maxAddress` parameter, with a defined limit for the quantity
 *       of registers. The byte count is calculated as twice the quantity
 *       of registers, as each register is 2 bytes.
 */
uint8_t MB_checkMRQtyAddrByteCount(uint16_t maxAddress){
	uint8_t clc_byte_count = MB_getQuantity(MB_FC_INDEX+3)*2;		// to calculate byte count from the output quantity
	if(MB_checkDataQuantity(MB_getQuantity(MB_FC_INDEX+3), (maxAddress-MB_HR_ADDR_OFFSET))
			&& (clc_byte_count == mbHandle.rxBuffer[MB_FC_INDEX+5])){				// Max quantity- 1968 for multiple register write checks valid quantity of coils
		if(MB_checkDataAddress(MB_getStartAddress(MB_FC_INDEX+1), (maxAddress-MB_HR_ADDR_OFFSET), MB_getQuantity(MB_FC_INDEX+3))){		// MAX coils/registers address 10000, checks valid data address and address+quantity of register, Max value will not exceed 9999
			return 0;
		}else{
			return MB_EC_ILDA_2;										// returns exception code - 2 for illegal data address and address+quantity
		}
	}else{
		return MB_EC_ILDV_3;											// returns exception code - 3 for illegal data value/quantity
	}

}



/**
  * @brief To check the valid data quantity and address for the coils and registers
  *
  * @param startAddress, quantity, maxQuantity, where startAddress is the starting address
  * of the requested coils or registers, quantity is the number of requested coils or register
  * and maxQuantity is the max number of coils or registers
  *
  * @retval 0, 2 & 3, where 0 = no exception, 2 = illegal data address, 3 = illegal data value
  */
uint8_t MB_checkDataQtyAddr(uint16_t startAddress, uint16_t maxAddress, uint16_t quantity, uint16_t maxQuantity){
	if(MB_checkDataQuantity(quantity, maxQuantity)){						// Max quantity- 2000 for coils and 125 for registers, checks valid quantity of coils
		if(MB_checkDataAddress(startAddress, maxAddress, quantity)){		// MAX coils/registers address 10000, checks valid data address and address+quantity of coils, Max value will not exceed 9999
			return 0;
		}else{
			return MB_EC_ILDA_2;										// returns exception code - 2 for illegal data address and address+quantity
		}
	}else{
		return MB_EC_ILDV_3;											// returns exception code - 3 for illegal data value/quantity
	}
}




/**
  * @brief To check the valid data quantity for the coils and registers
  *
  * @param quantity, maxQuantity, where, quantity is the number of requested coils or register
  * and maxQuantity is the max number of coils or registers
  *
  * @retval 0, 1 0 = invalid, 1 = valid
  */
uint8_t MB_checkDataQuantity(uint16_t quantity, uint16_t maxQuantity){
	return (quantity >= 1 && quantity <= maxQuantity);
}

/**
  * @brief To check the valid data address for the coils and registers
  *
  * @param startAddress and quantity, where, startAddress is the starting address
  * of the requested coils or registers, quantity is the number of requested coils or register
  *
  * @retval 0, 1 0 = invalid, 1 = valid
  */
uint8_t MB_checkDataAddress(uint16_t startAddress, uint16_t maxAddress, uint16_t quantity){
	uint16_t lastAddress = startAddress + quantity;
	return((startAddress >= 0 && startAddress <= maxAddress)
				&& (lastAddress >= 1 && lastAddress <= maxAddress));
}



/**
  * @brief To check the output value for the coils
  *
  * @param value, it is the output coil value, output coil can be 0x0000 or 0xFFFF
  *
  * @retval 0 or 1, where 1 = no exception,  0 = exception
  */
uint8_t MB_checkSCValue(uint16_t value){
	return (value==LOW || value==HIGH);
}




/**
  * @brief To check the single coil or register address
  *
  * @param address, for the target output coil or register
  *
  * @retval 0 or 1, where 1 = no exception,  0 = exception
  */
uint8_t MB_checkSCRAddr(uint16_t address, uint16_t maxAddress){
	return (address>=0U && address<maxAddress);
}


/**
  * @brief To check the single coil output address and value
  *
  * @param address, for the target output coil or register
  *
  * @retval 0,2& 3, where 0 = no exception, 2 = illegal data address,  3 = illegal data value
  */
uint8_t MB_checkSCValueAddr(void){
	if(MB_checkSCValue(MB_getSCRValue())){
		if(MB_checkSCRAddr(MB_getSCRAddress(), (MB_COA_MAX-MB_CO_ADDR_OFFSET))){
			return 0;
		}else{
			return MB_EC_ILDA_2;
		}
	}else{
		return MB_EC_ILDV_3;
	}

}

/**
  * @brief To check the output value for the coils
  *
  * @param value, it is the output coil value, output coil can be 0x0000 or 0xFFFF
  *
  * @retval 0 or 1, where 1 = no exception,  0 = exception
  */
uint8_t MB_checkSRValue(uint16_t value){
	return (value>=0x0000 && value<=MB_MAX_REGISTER_VALUE);
}


/**
 * @brief Checks the validity of the single register (SR) value address.
 *
 * This function validates the SR value and its address to ensure they
 * conform to the defined Modbus standards. It first checks if the SR
 * value is valid, and then verifies whether the SR address is within
 * the permissible range.
 *
 * @return uint8_t
 * - 0 if the SR value and address are valid.
 * - MB_EC_ILDA_2 if the address is invalid (illegal data address).
 * - MB_EC_ILDV_3 if the SR value is invalid (illegal data value).
 *
 * @note The function makes use of `MB_checkSRValue()` and
 *       `MB_checkSCRAddr()` to perform the necessary checks.
 */
uint8_t MB_checkSRValueAddr(void){
	if(MB_checkSRValue(MB_getSCRValue())){
		if(MB_checkSCRAddr(MB_getSCRAddress(), (MB_HRA_MAX-MB_HR_ADDR_OFFSET))>0u){
			return 0U;
		}else{
			return MB_EC_ILDA_2;
		}
	}else{
		return MB_EC_ILDV_3;
	}

}



uint16_t MB_get2byteData(uint8_t firstByteIndex){
	return ((uint16_t)mbHandle.rxBuffer[firstByteIndex]<<8u | (uint16_t)mbHandle.rxBuffer[firstByteIndex+1u]);
}

/**
  * @brief To get slave address
  * @param none
  * @retval 0-127
  */
uint8_t MB_getSlaveAddress(void){
	return mbHandle.rxBuffer[MB_SA_INDEX];
}

/**
  * @brief To get the function code from request PDU buffer
  * @param none
  * @retval 0-127
  */
uint8_t MB_getFunctionCode(void){
	return mbHandle.rxBuffer[MB_FC_INDEX];
}



/**
  * @brief To get the data start address (without offset) from request PDU buffer
  * @param none
  * @retval 0-9999
  */
uint16_t MB_getStartAddress(uint8_t firstByteIndex){
	return MB_get2byteData(firstByteIndex);
}

uint16_t MB_getSCRAddress(void){
	return MB_get2byteData(MB_FC_INDEX+1);
}





/**
  * @brief To get the requested data quantity from request PDU buffer
  * @param none
  * @retval 1-2000 or 1-125, 1-2000 for coils and discrete input and 1-125 for registers
  */
uint16_t MB_getQuantity(uint8_t firstByteIndex){
	return MB_get2byteData(firstByteIndex);
}


/**
  * @brief To get the requested data output value from the request PDU buffer
  * @param none
  * @retval the output value for single coil or register
  */
uint16_t MB_getSCRValue(void){
	return MB_get2byteData(MB_FC_INDEX+3);//(mbHandle.rxBuffer[MB_FC_INDEX+3]<<8 | mbHandle.rxBuffer[MB_FC_INDEX+4]);
}

/**
  * @brief To get the requested data output value from the request PDU buffer
  * @param none
  * @retval the output value for single coil or register
  */
uint8_t MB_getMCValueInByte(uint8_t index){
	return mbHandle.rxBuffer[index];//(mbHandle.rxBuffer[MB_FC_INDEX+3]<<8 | mbHandle.rxBuffer[MB_FC_INDEX+4]);
}


/**
 * @brief Retrieves a 2-byte value from the input buffer based on the given index.
 *
 * This function extracts a 16-bit (2-byte) value from the receive buffer
 * using the specified index. It is assumed that the value is stored
 * in a specific format compatible with Modbus communication.
 *
 * @param[in] index The index of the data to be retrieved from the buffer.
 *
 * @return uint16_t The 2-byte value extracted from the buffer.
 *
 * @note The function calls `MB_get2byteData()` to obtain the value,
 *       which is expected to handle the necessary logic for retrieving
 *       and combining the bytes correctly.
 */
uint16_t MB_getMRValueIn2Byte(uint8_t index){
	return MB_get2byteData(index);//(mbHandle.rxBuffer[MB_FC_INDEX+3]<<8 | mbHandle.rxBuffer[MB_FC_INDEX+4])
}



/*Input register-----------------*/



/**
 * @brief Retrieves the value of a specific input register.
 *
 * This function reads the value of an input register specified by the given
 * address. It uses a callback function to perform the read operation.
 *
 * @param[in] address The address of the input register to read.
 * @param[out] value Pointer to a variable where the retrieved value will be stored.
 *
 * @return uint8_t Returns the status of the read operation:
 *         - `MB_OK` on success.
 *         - `MB_ERROR` if the read operation fails.
 *
 * @note Ensure that the address provided is valid and within the range of
 *       defined input registers to avoid unexpected behavior.
 */
uint8_t MB_getIRValue(uint16_t address, uint16_t *value){
	return MB_readIRCallback(address, value);
}


/**
 * @brief Retrieves the values of multiple input registers.
 *
 * This function reads values from input registers starting from the specified
 * start address up to the last address. It checks the validity of the retrieved
 * data and populates the provided array with the values of the input registers.
 *
 * @param[out] value Pointer to an array where the retrieved values will be stored.
 * @param[in] size The number of input registers to read.
 * @param[in] startAddress The starting address of the input registers to read.
 * @param[in] lastAddresss The last address of the input registers to read.
 *
 * @return uint8_t Returns the status of the operation:
 *         - `MB_OK` on successful retrieval of all values.
 *         - `MB_ERROR` if there is an error retrieving any of the values or if
 *           the retrieved data is out of the valid range (0x0000 to 0xFFFF).
 *
 * @note The size parameter should match the number of registers being read
 *       to avoid out-of-bounds access in the value array.
 */
uint8_t MB_getIRsValue(uint16_t *value, uint8_t size, uint16_t startAddress, uint16_t lastAddresss){
	for(uint16_t i = startAddress; i <= lastAddresss; i++){
		uint16_t data;
		if(MB_getIRValue(i, &data)){
			if(data >= 0x0000 && data <= 0xFFFF){
				value[i-startAddress] = data;
			}else{
				return MB_ERROR;
			}
		}else{
			return MB_ERROR;
		}
	}

	return MB_OK;
}

/*Holding register-----------------------*/

/**
 * @brief Retrieves the values of multiple holding registers.
 *
 * This function reads values from holding registers starting from the specified
 * start address up to the last address. It checks the validity of the retrieved
 * data and populates the provided array with the values of the holding registers.
 *
 * @param[out] pData Pointer to an array where the retrieved values will be stored.
 * @param[in] startAddress The starting address of the holding registers to read.
 * @param[in] lastAddresss The last address of the holding registers to read.
 *
 * @return uint8_t Returns the status of the operation:
 *         - `MB_OK` on successful retrieval of all values.
 *         - `MB_ERROR` if there is an error retrieving any of the values or if
 *           the retrieved data is out of the valid range (0x0000 to 0xFFFF).
 *
 * @note The function assumes that the size of the pData array is sufficient to
 *       store the values of the registers being read.
 */
uint8_t MB_getHRsValue(uint16_t *pData, uint16_t startAddress, uint16_t lastAddresss){
	//DEBUG_PRINT("getHR_ok\r\n");

	for(uint16_t address = startAddress; address <= lastAddresss; address++){
		uint16_t data = 0U;
		//DEBUG_PRINT_INT("address: ", address, ",\r\n ");
		if(MB_readHRCallback(address, &data)){
			//DEBUG_PRINT_INT(" OK address: ", address, ",\r\n ");
			if(data >= 0x0000 && data <= 0xFFFF){
				pData[address-startAddress] = data;
			}else{
				return MB_ERROR;
			}
		}else{
			//DEBUG_PRINT_INT(" ERROR address: ", address, ",\r\n ");
			return MB_ERROR;
		}
	}

	return MB_OK;
}


/**
 * @brief Sets the value of a specified holding register.
 *
 * This function writes a specified data value to a holding register at the
 * given address. The actual writing is performed through a callback function
 * that handles the underlying implementation details.
 *
 * @param[in] address The address of the holding register to be set.
 * @param[in] data The value to write to the holding register.
 *
 * @return uint8_t Returns the status of the write operation:
 *         - Non-zero value indicates a successful write operation.
 *         - Zero indicates a failure in writing to the holding register.
 *
 * @note This function assumes that the provided address is valid and within
 *       the range of holding registers. The behavior is undefined if the
 *       address is out of bounds.
 */
uint8_t MB_setHRValue(uint16_t address, uint16_t data){
	//DEBUG_PRINT("SHR_OK\r\n");
	return MB_writeHRCallback(address, data);
	 //return (mbRegister.HR[address - MB_HR_ADDR_OFFSET] == data);
}



/**
 * @brief Reads multiple coils in Modbus communication.
 *
 * This function processes the Modbus request to read multiple coils. It validates
 * the start address and the number of coils requested, retrieves the coil data,
 * and constructs the response message containing the coil states.
 *
 * @return uint8_t Returns the status or exception code:
 *         - `MB_RS_255` on success.
 *         - `MB_EC_SDV_4` if there is a service device failure during coil data retrieval.
 *         - Other exception codes as returned by `MB_checkDataQtyAddr()`.
 *
 * @note The function reads the coil data in bytes and appends the response PDU,
 *       handling cases where the number of coils is not a multiple of 8.
 */
uint8_t MB_readCoils(void){
	uint8_t excepCode = MB_checkDataQtyAddr( MB_getStartAddress(MB_FC_INDEX+1), (MB_COA_MAX-MB_CO_ADDR_OFFSET),
			MB_getQuantity(MB_FC_INDEX+3), (MB_COA_MAX-MB_CO_ADDR_OFFSET) );//MB_checkCoilDataQtyAddr(MB_CA_MAX);
	if(excepCode == 0){										// if there is no exception

		uint8_t byteCount = (MB_getQuantity(MB_FC_INDEX+3)/8)+((MB_getQuantity(MB_FC_INDEX+3)%8)>0);
		uint16_t startAddress = MB_getStartAddress(MB_FC_INDEX+1)+MB_CO_ADDR_OFFSET;
		uint16_t endAddress = startAddress + MB_getQuantity(MB_FC_INDEX+3);
		uint8_t data[byteCount];

		if(MB_getCoilsInByte(data, byteCount, startAddress, endAddress)){
			MB_appendToRespPDU_SAFC();							// to append slave address and function code to response PDU
			MB_appendToRespPDU(byteCount);  // total response byte
			for(uint8_t i = 0; i < byteCount; i++){
				MB_appendToRespPDU(data[i]);
			}
			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;								// returns exception code 04
		}
	}else{													// if exception occurred
		return excepCode;
	}
}

/**
 * @brief Reads discrete inputs in Modbus communication.
 *
 * This function processes the Modbus request to read multiple discrete inputs.
 * It validates the start address and quantity of inputs, retrieves the input
 * data, and constructs the response message with the retrieved data.
 *
 * @return uint8_t Returns the status or exception code:
 *         - `MB_RS_255` on success.
 *         - `MB_EC_SDV_4` if there is a service device failure during input retrieval.
 *         - Other exception codes as returned by `MB_checkDataQtyAddr()`.
 *
 * @note The function handles the discrete inputs byte-by-byte and appends the response PDU.
 */
uint8_t MB_readDI(void){
	uint8_t excepCode = MB_checkDataQtyAddr( MB_getStartAddress(MB_FC_INDEX+1), (MB_DIA_MAX-MB_DI_ADDR_OFFSET),
						MB_getQuantity(MB_FC_INDEX+3), (MB_DIA_MAX-MB_DI_ADDR_OFFSET) );//MB_checkCoilDataQtyAddr(MB_DIA_MAX);
	if(excepCode == 0){										// if there is no exception

		/*------- Test start---------*/
		uint8_t byteCount = (MB_getQuantity(MB_FC_INDEX+3)/8)+((MB_getQuantity(MB_FC_INDEX+3)%8)>0);
		uint16_t startAddress = MB_getStartAddress(MB_FC_INDEX+1)+MB_DI_ADDR_OFFSET;
		uint16_t endAddress = startAddress + MB_getQuantity(MB_FC_INDEX+3);
		uint8_t data[byteCount];
		if(MB_getDIInByte(data, byteCount, startAddress, endAddress)){
			MB_appendToRespPDU_SAFC();							// to append slave address and function code to response PDU
			MB_appendToRespPDU(byteCount);  // total response byte

			for(uint8_t i = 0; i < byteCount; i++){
				MB_appendToRespPDU(data[i]);
			}
			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;								// returns exception code 04
		}

	}else{													// if exception occurred
		return excepCode;
	}

}



/**
 * @brief Writes a single coil in Modbus communication.
 *
 * This function processes the Modbus request to write a single coil. It validates
 * the request, retrieves the coil address and value, and updates the specified coil.
 * The response message is copied from the request message and prepared for transmission.
 *
 * @return uint8_t Returns the status or exception code:
 *         - `MB_RS_255` on success.
 *         - `MB_EC_SDV_4` if there is a service device failure during coil writing.
 *         - Other exception codes as returned by `MB_checkSCValueAddr()`.
 *
 * @note The function constructs the response by copying the request PDU, and it handles
 *       any errors during the coil writing process.
 */
uint8_t MB_writeSC(void){
	uint8_t excepCode = MB_checkSCValueAddr();
	if(excepCode == 0){										// if there is no exception
		if(MB_setCoil(MB_getSCRAddress()+MB_CO_ADDR_OFFSET, MB_getSCRValue())){
			memcpy(mbHandle.txBuffer, mbHandle.rxBuffer, mbHandle.rxBufferSize); // 2 is subtracted, case of 2 byte crc will add at the end of the processing
			mbHandle.txBufferSize = mbHandle.rxBufferSize; // 2 is subtracted, case of 2 byte crc will add at the end of the processing

			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;
		}
	}else{													// if exception occurred
		return excepCode;
	}
}







/**
 * @brief Writes multiple coils in Modbus communication.
 *
 * This function processes the Modbus request to write multiple coils. It
 * validates the request, retrieves the coil values from the request PDU,
 * and writes the values to the specified coil addresses. The response PDU
 * is then constructed and returned.
 *
 * @return uint8_t Returns the status or exception code:
 *         - `MB_RS_255` on success.
 *         - `MB_EC_SDV_4` if there is a service device failure during coil writing.
 *         - Other exception codes as returned by `MB_checkMCQtyAddrByteCount()`.
 *
 * @note The function processes coils byte-by-byte and bit-by-bit, handling
 *       any errors that occur during the writing process.
 */
uint8_t MB_writeMC(void){

	uint8_t excepCode = MB_checkMCQtyAddrByteCount();		//TODO: an error is found there, have to solve it
	if(excepCode == 0){										// if there is no exception

		/*------- Test start---------*/


		uint16_t startAddress = MB_getStartAddress(MB_FC_INDEX+1)+MB_CO_ADDR_OFFSET;
		uint16_t endAddress = startAddress + MB_getQuantity(MB_FC_INDEX+3);
		uint8_t byteCount = (MB_getQuantity(MB_FC_INDEX+3)/8)+((MB_getQuantity(MB_FC_INDEX+3)%8)>0);
		uint16_t startByteIndex = (MB_FC_INDEX+6);
		//uint16_t lastByteIndex = (startByteIndex + byteCount-1);

		for(uint8_t byteIndex = 0; byteIndex < byteCount; byteIndex++){
			uint8_t data = MB_getMCValueInByte((startByteIndex+byteIndex));
			for(uint8_t bitIndex = 0; bitIndex < 8U; bitIndex++){
				//uint8_t bitValue = ((data>>bitIndex)&0x01);
				uint16_t coilAddress = startAddress+(byteIndex*8U)+bitIndex;
				if(coilAddress>=startAddress && coilAddress < endAddress){
					if(MB_setCoil(coilAddress, ((data>>bitIndex)&0x01)) == MB_ERROR){
						return MB_EC_SDV_4;
					}
				}else{
					break;
				}
			}
		}


			uint16_t andMask = 0x00FF;
			MB_appendToRespPDU_SAFC();
			MB_appendToRespPDU((uint8_t)(MB_getStartAddress(MB_FC_INDEX+1)>>8));
			MB_appendToRespPDU((uint8_t)(MB_getStartAddress(MB_FC_INDEX+1) & andMask));
			MB_appendToRespPDU((uint8_t)(MB_getQuantity(MB_FC_INDEX+3)>>8));
			MB_appendToRespPDU((uint8_t)(MB_getQuantity(MB_FC_INDEX+3) & andMask));
//			MB_appendToRespPDU_CRC();
		return MB_RS_255;
	}else{													// if exception occurred
		return excepCode;
	}
}



/**
 * @brief Reads input registers in Modbus communication.
 *
 * This function processes the Modbus request to read multiple input registers.
 * It validates the start address and quantity of registers requested, retrieves the register values,
 * and constructs the response message with the data from the input registers.
 *
 * @return uint8_t Returns the status or exception code:
 *         - `MB_RS_255` on success.
 *         - `MB_EC_SDV_4` if there is a service device failure during register value retrieval.
 *         - Other exception codes as returned by `MB_checkDataQtyAddr()`.
 *
 * @note The function retrieves the register values as 16-bit data, appends the response PDU,
 *       and sends the result byte by byte.
 */
uint8_t MB_readIR(void){
	uint8_t excepCode = MB_checkDataQtyAddr(MB_getStartAddress(MB_FC_INDEX+1), (MB_IRA_MAX-MB_IR_ADDR_OFFSET),
						MB_getQuantity(MB_FC_INDEX+3), (MB_IRA_MAX-MB_IR_ADDR_OFFSET));//MB_checkRegisterDataQtyAddr(MB_IRA_MAX);
	if(excepCode == 0){										// if there is no exception

		uint8_t size = MB_getQuantity(MB_FC_INDEX+3);
		uint16_t data[size];
		uint16_t startAddress =  MB_getStartAddress(MB_FC_INDEX+1)+MB_IR_ADDR_OFFSET;
		uint16_t lastAddress = startAddress+size-1;

		if(MB_getIRsValue(data, size,  startAddress, lastAddress)){


			uint8_t byteCount = size*2;
			MB_appendToRespPDU_SAFC();
			MB_appendToRespPDU(byteCount);
			for(uint8_t i = 0; i < size; i++){
				MB_appendToRespPDU((uint8_t)(data[i]>>8));
				MB_appendToRespPDU((uint8_t)(data[i] & MB_AND_MASK_16T8));
			}
			return MB_RS_255;
		}else{

			return MB_EC_SDV_4;									// exception - 4 ,  service device failure
		}

	}else{													// if exception occurred
		return excepCode;
	}
}


/**
 * @brief Reads holding registers for Modbus communication.
 *
 * This function handles the Modbus request for reading multiple
 * holding registers. It validates the request data such as start
 * address and quantity, retrieves the register values, and constructs
 * the response message.
 *
 * @return uint8_t Returns the status or exception code:
 *         - `MB_RS_255` on success.
 *         - `MB_EC_SDV_4` if there is a service device failure.
 *         - Other exception codes as returned by `MB_checkDataQtyAddr()`.
 *
 * @note This function uses Modbus PDU to append the response.
 */
uint8_t MB_readHR(void){
	uint8_t excepCode =  MB_checkDataQtyAddr(MB_getStartAddress(MB_FC_INDEX+1), (MB_HRA_MAX-MB_HR_ADDR_OFFSET),
						 MB_getQuantity(MB_FC_INDEX+3), (MB_HRA_MAX-MB_HR_ADDR_OFFSET));//MB_checkRegisterDataQtyAddr(MB_HRA_MAX);
	if(excepCode == 0){										// if there is no exception

		/*------- Test start---------*/

		uint8_t size = MB_getQuantity(MB_FC_INDEX+3);		// get the requested quantity
		uint16_t data[size];
		uint16_t startAddress =  MB_getStartAddress(MB_FC_INDEX+1)+MB_HR_ADDR_OFFSET;
		uint16_t lastAddress = startAddress+size-1;
		// retrieve registers value and make the response

		if(MB_getHRsValue(data, startAddress, lastAddress)){

			uint16_t andMask = 0x00FF;
			uint8_t byteCount = size*2;
			MB_appendToRespPDU_SAFC();
			MB_appendToRespPDU(byteCount);
			for(uint8_t i = 0; i < size; i++){
				MB_appendToRespPDU((uint8_t)(data[i]>>8));
				MB_appendToRespPDU((uint8_t)(data[i] & andMask));

			}

//			MB_appendToRespPDU_CRC();

			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;									// exception - 4 ,  service device failure
		}

	}else{													// if exception occurred
		return excepCode;
	}
}


/**
 * @brief Writes a single holding register in Modbus communication.
 *
 * This function processes the Modbus request to write a value to a single holding register.
 * It validates the register address and value before setting the register value.
 * The function then prepares a response message based on the write operation.
 *
 * @return uint8_t Returns the status or exception code:
 *         - `MB_RS_255` on success.
 *         - `MB_EC_SDV_4` if there is a service device failure during register value setting.
 *         - Other exception codes as returned by `MB_checkSRValueAddr()`.
 *
 * @note The function handles the case where the register being set is the slave address,
 *       and updates the response buffer accordingly. It also ensures the response size
 *       accounts for the addition of a CRC at the end of the processing.
 */
uint8_t MB_writeSHR(void){
	uint8_t excepCode = MB_checkSRValueAddr();
	if(excepCode == 0U){										// if there is no exception
		if(MB_setHRValue(MB_getSCRAddress()+MB_HR_ADDR_OFFSET, MB_getSCRValue()) == MB_OK){//setSingleRegisterValue(MB_getSCRAddress()+MB_HR_ADDR_OFFSET, MB_getSCRValue())){
			memcpy(mbHandle.txBuffer, mbHandle.rxBuffer, mbHandle.rxBufferSize);  // 2 is subtracted, case of 2 byte crc will add at the end of the processing
			if((MB_getSCRAddress()+MB_HR_ADDR_OFFSET) == MB_IRA_SLAVE_ADDRESS){
				mbHandle.txBuffer[MB_SA_INDEX] = MB_getSlaveAddress();
			}
			mbHandle.txBufferSize = mbHandle.rxBufferSize; // 2 is subtracted, case of 2 byte crc will add at the end of the processing
			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;
		}
	}else{													// if exception occurred
		return excepCode;
	}
}


/**
 * @brief Writes multiple holding registers in Modbus communication.
 *
 * This function processes the Modbus request to write values to multiple holding registers.
 * It validates the register address and quantity before writing the values retrieved
 * from the request to the specified registers. The function constructs the response message
 * based on the write operation.
 *
 * @return uint8_t Returns the status or exception code:
 *         - `MB_RS_255` on success.
 *         - `MB_EC_SDV_4` if there is a service device failure during register value setting.
 *         - Other exception codes as returned by `MB_checkMRQtyAddrByteCount()`.
 *
 * @note The function processes the input data in 2-byte values and updates the
 *       response message to include the start address and quantity of registers written.
 *       Additionally, it updates the slave address in the response if the address being
 *       set corresponds to the slave address.
 */
uint8_t MB_writeMHR(void){
	uint8_t excepCode = MB_checkMRQtyAddrByteCount(MB_HRA_MAX);
	if(excepCode == 0){										// if there is no exception

		uint16_t startAddress = MB_getStartAddress(MB_FC_INDEX+1)+MB_HR_ADDR_OFFSET;
		uint16_t endAddress = startAddress + MB_getQuantity(MB_FC_INDEX+3)-1;
//		uint8_t byteCount = MB_getQuantity(MB_FC_INDEX+3);
		//uint16_t startByteIndex = (MB_FC_INDEX+6);
		//uint16_t lastByteIndex = (startByteIndex + byteCount-1);
		uint8_t byteIndex = (MB_FC_INDEX+6);
		for(uint16_t regAddress = startAddress; regAddress <= endAddress; regAddress++){
			uint16_t data = MB_getMRValueIn2Byte((byteIndex));
			byteIndex += 2;
			if(MB_setHRValue(regAddress, data) == MB_ERROR){
				return MB_EC_SDV_4;
			}
		}

		uint16_t andMask = 0x00FF;
		MB_appendToRespPDU_SAFC();
		MB_appendToRespPDU((uint8_t)(MB_getStartAddress(MB_FC_INDEX+1)>>8));
		MB_appendToRespPDU((uint8_t)(MB_getStartAddress(MB_FC_INDEX+1) & andMask));
		MB_appendToRespPDU((uint8_t)(MB_getQuantity(MB_FC_INDEX+3)>>8));
		MB_appendToRespPDU((uint8_t)(MB_getQuantity(MB_FC_INDEX+3) & andMask));
//		MB_appendToRespPDU_CRC();

		if((MB_getSCRAddress()+MB_HR_ADDR_OFFSET) == MB_IRA_SLAVE_ADDRESS){
			mbHandle.txBuffer[MB_SA_INDEX] = MB_getSlaveAddress();
		}

		return MB_RS_255;
	}else{													// if exception occurred
		return excepCode;
	}
}





/*--------- callback function ------------*/

uint8_t MB_getDI(uint16_t address, uint8_t *value){
	return MB_readDICallback(address, value);
}

uint8_t MB_getDIInByte(uint8_t *byteArray, uint8_t size, uint16_t startAddress, uint16_t endAddress){
	volatile uint8_t byteCount = 0;
	volatile uint8_t dataByte = 0;
	volatile uint8_t bitCount = 0;

	for(uint16_t i = startAddress; i < endAddress; i++){
		uint8_t data = 0;
		if(MB_getDI(i, &data)){
			if(data >=0 && data <=1){
				dataByte |= data<<(bitCount++);
				if(bitCount>=8){
					if(byteCount < size){
						byteArray[byteCount++] = dataByte;
						dataByte = 0;
						bitCount=0;
					}else{
						return MB_ERROR;
					}
				}
			}else{
				return MB_ERROR;
			}
		}else{
			return MB_ERROR;
		}
	}
	if( bitCount > 0 && bitCount < 8){
		if(byteCount < size){
			byteArray[byteCount++] = dataByte;
		}else{
			return MB_ERROR;
		}
		//byteCount++;
	}

	return MB_OK;
}




/**
  * @brief sets coil output
  * @param address: address of the holding register
  * @retval holding register value
  */
uint8_t MB_setCoil(uint16_t address, uint16_t value){

	return MB_writeCOCallback(address, (value > 0));
	//return (address-MB_C_ADDR_OFFSET >= 0 && address-MB_C_ADDR_OFFSET <= MB_MAX_CR_ADDR);
}


uint8_t MB_getCoil( uint16_t address, uint8_t *value){
	return MB_readCOCallback(address, value);
}
uint8_t MB_getCoilsInByte(uint8_t *byteArray, uint8_t size, uint16_t startAddress, uint16_t endAddress){
	uint8_t byteCount = 0;
	uint8_t dataByte = 0;
	uint8_t bitCount = 0;

	for(uint16_t i = startAddress; i < endAddress; i++){
		uint8_t data=0;
		if(MB_getCoil(i, &data)){
			if(data >=0 && data <=1){
				dataByte |= data<<(bitCount++);
				if(bitCount>=8){
					if(byteCount < size){
						byteArray[byteCount++] = dataByte;
						dataByte = 0;
						bitCount=0;
					}else{
						return MB_ERROR;
					}
				}
			}else{
				return MB_ERROR;
			}
		}else{
			return MB_ERROR;
		}
	}
	if( bitCount > 0 && bitCount < 8){
		if(byteCount < size){
			byteArray[byteCount++] = dataByte;
		}else{
			return MB_ERROR;
		}
		//byteCount++;
	}

	return MB_OK;
}







/*------ attach callback----------*/
/**
 * @brief Attaches a callback function for reading discrete inputs.
 *
 * This function allows the user to set a custom callback function
 * that will be invoked to read discrete input values from specified
 * addresses.
 *
 * @param callbackReadFunc Pointer to the function that reads discrete
 *                         inputs. The function should match the
 *                         signature:
 *                         `uint8_t callback(MB_DIAddrEnum address, uint8_t *value)`.
 */
void MB_attachDICallback(uint8_t (*callbackReadFunc)( MB_DIAddrEnum address, uint8_t *value)){
	MB_readDICallback = callbackReadFunc;
}

/**
 * @brief Attaches callback functions for coil outputs.
 *
 * This function allows the user to set custom callback functions for
 * reading and writing coil outputs. The provided functions will be
 * invoked when performing operations on coil addresses.
 *
 * @param callbackWriteFunc Pointer to the function that writes coil
 *                          outputs. The function should match the
 *                          signature:
 *                          `uint8_t callback(MB_CoilAddrEnum address, uint8_t value)`.
 * @param callbackReadFunc Pointer to the function that reads coil
 *                         outputs. The function should match the
 *                         signature:
 *                         `uint8_t callback(MB_CoilAddrEnum address, uint8_t *value)`.
 */
void MB_attachCOCallback(uint8_t (*callbackWriteFunc)(MB_CoilAddrEnum address, uint8_t value), uint8_t (*callbackReadFunc)(MB_CoilAddrEnum address, uint8_t *value)){
	MB_writeCOCallback = callbackWriteFunc;
	MB_readCOCallback = callbackReadFunc;
}


/**
 * @brief Attaches a callback function for reading input registers.
 *
 * This function allows the user to set a custom callback function
 * that will be invoked to read input register values from specified
 * addresses.
 *
 * @param callbackReadFunc Pointer to the function that reads input
 *                         registers. The function should match the
 *                         signature:
 *                         `uint8_t callback(MB_IRAddrEnum address, uint16_t *value)`.
 */
void MB_attachIRCallback(uint8_t (*callbackReadFunc)(MB_IRAddrEnum address, uint16_t *value)){
	MB_readIRCallback = callbackReadFunc;
}

/**
 * @brief Attaches callback functions for holding registers.
 *
 * This function allows the user to set custom callback functions for
 * reading and writing holding register values. The provided functions
 * will be invoked when performing operations on holding register
 * addresses.
 *
 * @param callbackWriteFunc Pointer to the function that writes holding
 *                          registers. The function should match the
 *                          signature:
 *                          `uint8_t callback(MB_HRAddrEnum address, uint16_t value)`.
 * @param callbackReadFunc Pointer to the function that reads holding
 *                         registers. The function should match the
 *                         signature:
 *                         `uint8_t callback(MB_HRAddrEnum address, uint16_t *value)`.
 */
void MB_attachHRCallback(uint8_t (*callbackWriteFunc)(MB_HRAddrEnum address, uint16_t value), uint8_t (*callbackReadFunc)(MB_HRAddrEnum address, uint16_t *value)){
	MB_writeHRCallback = callbackWriteFunc;
	MB_readHRCallback = callbackReadFunc;
}

