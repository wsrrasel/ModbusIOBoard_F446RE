/*
 * MB_Handler.h
 *
 *  Created on: Jan 18, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_MB_HANDLER_H_
#define INC_MB_HANDLER_H_

#include "main.h"
#include "MB_CRC.h"
#include "MB_Addresses.h"
#include "string.h"
#include "stdlib.h"
#include "MB_RTUSlave.h"
#include "MB_Config.h"



//#define mb_res_max_size 255						// maximum data unit size for the response


#define MB_CO_ADDR_OFFSET 	 			1U						// coil address offset by 1
#define MB_DI_ADDR_OFFSET 	 			10001U					// discrete input address offset by 10001
#define MB_HR_ADDR_OFFSET 	 			40001U					// holding register address offset by 40001
#define MB_IR_ADDR_OFFSET 	 			30001U					// input register address offset by 30001

#define MB_MAX_CR_ADDR 		 			10000U					// max address for coils and registers
#define MB_MAX_COILS 					2000U					// max coils in the Modbus protocol is 2000
#define MB_MAX_WM_COILS 				1968U					// max multiple coils to write in the Modbus protocol is 1968
#define MB_MAX_REGISTERS 				125U		// max registers in the Modbus protocol is 2000

#define MB_MAX_REGISTER_VALUE 			0xFFFF	// max registers value


#define MB_FC_MAX						127U		// max supported function code
#define MB_EFC_OFFSET					0x80U		// exception function code offset for te exception


#define MB_AND_MASK_16T8				0x00FF		// AND mask to get the lower 8 bits



typedef enum MB_ErrorStatus{
	MB_ERROR = 0U,
	MB_OK,
}MB_ErrorStatus;




/*Coil state value*/
typedef enum{
	LOW = 0x0000,
	HIGH = 0xFF00,
}MB_CoilValue;




/* Function code list */
typedef enum MB_FuncCode{
	MB_FC_RC_01 = 1U,		//01 (0x01), RC-Read Coils
	MB_FC_RDI_02,			//02 (0x02), RDI-Read Discrete Inputs
	MB_FC_RHR_03,			//03 (0x03), RHG-Read Holding Registers
	MB_FC_RIR_04,			//04 (0x04), RIG-Read Input Registers
	MB_FC_WSC_05,			//05 (0x05), WSC-Write Single Coil
	MB_FC_WSR_06,			//06 (0x06), WSR-Write Single Register
	MB_FC_RES_07,			//07 (0x07), RES-Read Exception Status (Serial Line only)
	MB_FC_DIAG_08,			//08 (0x08), DIAG-Diagnostics (Serial Line only)
	MB_FC_GCEC_11 = 11U,		//11 (0x0B), GCEC-Get Communication Event Counter (Serial Line only)
	//MB_FC_GCEL_12,			//12 (0x0C), GCEL-Get Communication Event Log (Serial Line only)
	MB_FC_WMC_15 = 15U,		//15 (0x0F), WMC-Write Multiple Coils
	MB_FC_WMR_16,			//16 (0x10), WMR-Write Multiple registers
	//MB_FC_RSI_17,			//17 (0x11), RSI-Report Server ID (Serial Line only)
	//MB_FC_RFR_20 = 20U,		//20 (0x14), RFR-Read File Record
	//MB_FC_WFR_21,			//21 (0x15), WFR-Write File Record
	MB_FC_MWR_22 = 22U,			//22 (0x16), MWR-Mask Write Register
	MB_FC_RWMR_23,			//23 (0x17), RWMR-Read/Write Multiple registers
	//MB_FC_RFQ_24,			//24 (0x18), RFQ-Read FIFO Queue
//	MB_FC_EIT_43 = 43U,		//43 (0x2B), EIT-Encapsulated Interface Transport

}MB_FuncCode;




/*
 * Exception Code enumerator
 */
typedef enum MB_ExcepCode{
	MB_EC_NONE_0 = 0,				//0, NE - NO EXCEPTION
	MB_EC_ILF_1,				//01, ILF - ILLEGAL FUNCTION
	MB_EC_ILDA_2,				//02, ILDA - ILLEGAL DATA ADDRESS
	MB_EC_ILDV_3,				//03, ILDV - ILLEGAL DATA VALUE
	MB_EC_SDV_4,				//04, SDV - SERVER DEVICE FAILURE
	MB_EC_ACK_5,				//05, ACK - ACKNOWLEDGE
	MB_EC_SDB_6,				//06, SDB - SERVER DEVICE BUSY
	MB_EC_MPE_8 = 8,			//08, MPE - MEMORY PARITY ERROR
	MB_EC_GPU_10 = 10,			//10, GPU - GATEWAY PATH UNAVAILABLE
	MB_EC_GTDFR_11,				//11, GTDFR - GATEWAY TARGET DEVICE	FAILED TO RESPOND

}MB_ExcepCode;

typedef enum {
	MB_NRS_254 = 254u,			//254, modbus no response status
	MB_RS_255,					//255, modbus response status
}RespStatus;



/*
 *To make the PDU buffer byte index readable
 */
typedef enum{
	MB_SA_INDEX = 0,				// 0, Slave address index
	MB_FC_INDEX,					// 1, Function code index

	MB_RWCR_STA_HB_INDEX,		// 2, Read-write-Coil-Register, starting address High byte index
	MB_RWCR_STA_LB_INDEX,			// 3, Read-write-Coil-Register, starting address Low byte index
	MB_RCR_RQ_HB_INDEX,				// 4, Read-Coil-Register, requested quantity high byte index
	MB_RCR_RQ_LB_INDEX,				// 5, Read-Coil-Register, requested quantity Low byte index
	MB_WSCR_OV_HB_INDEX = 5,		// 5, write-Single-Coil-Register, output value high byte index
	MB_WSCR_OV_LB_INDEX,			// 6, write-Single-Coil-Register, output value low byte index
	MB_WMCR_RQ_HB_INDEX = 5,		// 5, Read-Multiple-Coil-Register, requested quantity high byte index
	MB_WMCR_RQ_LB_INDEX,			// 6, write-Multiple-Coil-Register, requested quantity low byte index
}MB_PDUBufferIndex;




/*Modbus handler for modbus RTU
 * */
typedef struct MB_RTUHandler{
	uint8_t *rxBuffer;				// pDU (application data unit) buffer for the request
	uint8_t *txBuffer;				// pDU buffer for the response
	uint8_t rxBufferSize;			// received pdu buffer size
	uint8_t txBufferSize;			//response pdu buffer size
	uint8_t is_req_cpy_to_resp;
}MB_RTUHandler;




void MB_init(MB_RTUHandler *_mbHandle);												// to initialize the Modbus RTU slave

void MB_SetHandlerInstance(MB_RTUHandler *_mbHandle);
MB_RTUHandler *MB_GetHandlerInstance(void);

void MB_ClearBuffer(void);

void MB_appendToRespPDU(uint8_t data);
void MB_appendToRespPDU_SAFC(void);
void MB_appendToRespPDU_CRC(void);

void MB_CopyReqToRespPDU(void);
uint8_t MB_getDI(uint16_t address, uint8_t *value);
uint8_t MB_getDIInByte(uint8_t *byteArray, uint8_t size, uint16_t startAddress, uint16_t endAddress);


void MB_setReqBuff(uint8_t *buff, uint8_t size);
void MB_setRespBuff(uint8_t *buff);
uint8_t MB_getRespBuffSize(void);
uint8_t MB_getSlaveAddress(void);
uint8_t MB_getFunctionCode(void);								// to get the function code
uint16_t MB_getSCRAddress(void);								// to get the diagnostics sub function
uint16_t MB_getStartAddress(uint8_t firstByteIndex);			// to get the start address
uint16_t MB_get2byteData(uint8_t firstByteIndex);					// to get 2 byte of data from PDU buffer

uint8_t MB_getMCValueInByte(uint8_t index);

uint16_t MB_getMRValueIn2Byte(uint8_t index);

uint8_t MB_setCoil(uint16_t address, uint16_t value);

uint8_t MB_isFunctionCodeValid(uint8_t functionCode);				// to check the function code is valid or not
uint8_t MB_executeRequest(uint8_t functionCode);				// to execute the request
void MB_excepRespBuilder(uint8_t excepCode);						// to build up the exception response PDU





/*-------Callback function--------*/
void MB_attachDICallback(uint8_t (*callbackReadFunc)(MB_DIAddrEnum address, uint8_t *value));
void MB_attachCOCallback(uint8_t (*callbackWriteFunc)(MB_CoilAddrEnum address, uint8_t value), uint8_t (*callbackReadFunc)(MB_CoilAddrEnum address, uint8_t *value));
void MB_attachIRCallback(uint8_t (*callbackReadFunc)(MB_IRAddrEnum address, uint16_t *value));
void MB_attachHRCallback(uint8_t (*callbackWriteFunc)(MB_HRAddrEnum address, uint16_t value), uint8_t (*callbackReadFunc)(MB_HRAddrEnum address, uint16_t *value));




#endif /* INC_MB_HANDLER_H_ */
