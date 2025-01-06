/*
 * MB_RTUSlave.c
 *
 *  Created on: Oct 25, 2022
 *      Author: wsrra
 */

#include "MB_Handler.h"
#include "MB_RTUSlave.h"
#include "MB_CRC.h"
#include "MB_Serial.h"


static MB_RTUSlave *slv;
void MBRS_Init(MB_RTUSlave *mbSlave,uint8_t slaveAddress){
	slv = mbSlave;
	slv->slave_address = slaveAddress;
}

MB_RTUSlave *MBRS_GetInstance(void){
	return slv;
}
void MBRS_SetInstance(MB_RTUSlave *mbSlave){
	slv = mbSlave;
}


/**
  * @brief To check the slave address. slave address 0 for broadcast,
  * 1-247 valid slave address, 248-255 reserved slave address
  *
  * @param none
  *
  * @retval 0 & 1, if returns 0 means error, 1 means no error
  */
uint8_t MBRS_CheckSAnCRC(MB_RTUSlave *mbSlave){
	if((mbSlave->rxBuffer[MB_SA_INDEX] >= MB_BROADCAST_ADDR)
		&& (mbSlave->rxBuffer[MB_SA_INDEX] <= MB_SLAVE_ADDR_MAX)){				// is the slave address is valid

		if(mbSlave->rxBuffer[MB_SA_INDEX] == mbSlave->slave_address
			|| mbSlave->rxBuffer[MB_SA_INDEX] == MB_BROADCAST_ADDR){			// if slave address is matched

			if(MB_CRCCheck(mbSlave->rxBuffer, mbSlave->rxBufferSize)){
				return 1U;
			}
		}

	}

	return 0U;
}



/**
  * @brief To process the request.
  *	Basically, it checks CRC error, function code, data quantity and data address
  *	and if any exception occurs, it returns the corresponding exception code
  *
  * @param none
  *
  * @retval 0 & 1, if returns 0 do nothing, else send the response to the master
  */
uint8_t MBRS_processRequest(MB_RTUSlave *mbSlave){

		/*
		 * When the remote device enters its Listen Only Mode,
		 * all active communication controls are turned off
		 * and  any MODBUS messages addressed to it or broadcast are monitored,
		 * but no actions will be taken and no responses will be sent
		 * */

	MB_setReqBuff(mbSlave->rxBuffer, mbSlave->rxBufferSize);
	MB_setRespBuff(mbSlave->txBuffer);
	uint8_t processStatus = 0u;
	uint8_t funCode = MB_getFunctionCode();


		// check function code

		if( MB_isFunctionCodeValid(funCode)>0U){									// if the function code is valid
			processStatus = MB_executeRequest(funCode);						//  MB_executeRequest() will return 0x04 and 0xFF, 0x04 = exception occured and 0xFF = no exception

		}else{
			processStatus = MB_EC_ILF_1;									// exception code 1 for illegal function code
		}
		if (processStatus != 0u){
			if(processStatus != MB_RS_255 && processStatus != MB_NRS_254){	// for exception response

				MB_excepRespBuilder(processStatus);						// processStatus = 255 means, no exception and the response has been built
			}else{															// for normal response
				if(processStatus == MB_NRS_254){
					return 0U;
				}
			}
			if((mbSlave->rxBuffer[MB_SA_INDEX] != MB_BROADCAST_ADDR)){
				MB_appendToRespPDU_CRC();
				mbSlave->txBufferSize = MB_getRespBuffSize();
				if(funCode == MB_FC_WSC_05 || funCode == MB_FC_WSR_06) {mbSlave->txBufferSize -= 2;}
				return 1U;
			}
		}

	return 0U;
}




