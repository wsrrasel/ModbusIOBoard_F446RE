/*
 * MB_Serial.c
 *
 *  Created on: Feb 24, 2022
 *      Author: RASEL_EEE
 */

#include "MB_Handler.h"
#include "MB_Serial.h"
#include "main.h"
#include "stdio.h"
#include "Config.h"
#include "Debug.h"

extern Debug_ts debug;


/*-------private variables--------*/
UART_HandleTypeDef *Uart;
TIM_HandleTypeDef *Timer_1P5_CHAR;				// used as a 1.5char timer for slave and reply timeout timer for the master
TIM_HandleTypeDef *Timer_3P5_CHAR;		// used as a 3.5char timer for slave and broadcast turnaround delay timeout timer for the master


static MBS_Serial *serl;

//volatile uint16_t mb_recv_buf_size;
//volatile uint8_t frameOkFlag;

/*
 * 0 position of  MBS_BaudRate[] is for user define baud rate. it is readable/writable
 * */
static uint32_t MBS_BaudRate[] = {
		2000000U,		//This is user define baud rate. it is readable/writable
		1200U,
		1800U,
		2400U,
		4800U,
		9600U,
		14400U,
		19200U,
		28800U,
		38400U,
		57600U,
		115200U,
		230400U,
		460800U,
		576000U,
		921600U
};

/*-------private functions--------*/
uint8_t (*MBS_CmpltTxCallback)(MBS_Serial *mbSerial, uint16_t address, uint16_t value);
uint8_t (*MBS_CmpltRxCallback)(MBS_Serial *mbSerial);



static void MBS_Timer_Init(TIM_TypeDef *Timer_Inst);
static float MBS_GetByteTxTime(uint32_t baudRate);


/** @brief Modbus serial Initialization Function
  * @param Timer_1P5_CHAR, Timer_3P5_CHAR, GPIOx and RS485_RE_DE_Pin;
  * where, 	Timer_1P5_CHAR timer pinter point 1.5 char timer,
  * 		Timer_3P5_CHAR timer pinter point 3.5 char timer,
  * @retval None
  *
  */
void MBS_Init(MBS_Serial *serial, UART_HandleTypeDef *UART, TIM_HandleTypeDef *TIMER_1P5_CHAR, TIM_HandleTypeDef *TIMER_3P5_CHAR){
	serl = serial;
	Uart = UART;
	Timer_1P5_CHAR = TIMER_1P5_CHAR;
	Timer_3P5_CHAR = TIMER_3P5_CHAR;


	MBS_Timer_Init(Timer_1P5_CHAR->Instance);					// initialize 1.5 char timer
	MBS_Timer_Init(Timer_3P5_CHAR->Instance);					// initialize 3.5 char timer
	serial->frameOkFlag = 0U;


	MBS_STOP_TIMER(Timer_1P5_CHAR);		//start 1.5 char timer
	MBS_STOP_TIMER(Timer_3P5_CHAR);		//start 3.5 char timer

	/*Set the serial receiving mode always*/
	serial->setDownstream();

}




/**
  * @brief Initialize timer
  * @param none
  * @retval None
  */
static void MBS_Timer_Init(TIM_TypeDef *Timer_Inst){
	Timer_Inst->CR1 &= ~TIM_CR1_CEN;		// Initially disable timer4 counter
	Timer_Inst->EGR |= TIM_EGR_UG;			// Re-initialize the counter and generates an update of the registers.
	Timer_Inst->DIER |= TIM_DIER_UIE;		// enable Update interrupt
}


/**
  * @brief gets the modbus serial instance
  * @param none
  * @retval None
  */
MBS_Serial *MBS_GetInstance(void){
	return serl;
}



/**
  * @brief configures the modbus serial port
  * @param serialPort: modbus serial port struct
  * @retval error status
  */
MB_ErrorStatus MBS_Config(MBS_Serial *serial){
	MB_ErrorStatus errorStatus = MB_OK;

	/*---------stop the uart interrupt, and the 3.5 & 1.5 char time------------*/
	__HAL_UART_DISABLE_IT(Uart, UART_IT_RXNE);		// enable uart receive interrupt
	MBS_STOP_TIMER(Timer_1P5_CHAR); 					//stop 1.5 char timer
	MBS_STOP_TIMER(Timer_3P5_CHAR); 					//stop 3.5 char timer

//	MBS_DecodeConfigValue(serial);
	// sets the baudrate for modbus serial
	if((serial->baudRate <= 65535U && serial->baudRate > 0) ){
		Uart->Init.BaudRate = serial->baudRate;
	}else{
		errorStatus = MB_ERROR;
		goto MBS_SERIAL_CONFIG_END;
	}

	/*---------configure the uart------------*/
	// sets the data bits for modbus serial
	switch (serial->dataBits) {
		case MBS_DB_7:
			Uart->Init.WordLength = UART_WORDLENGTH_8B;		// wordlength 8 bits = 7 data bits  + 1 parity bits
			break;
		case MBS_DB_8:
			Uart->Init.WordLength = UART_WORDLENGTH_9B;		// wordlength 9 bits = 8 data bits + 1 parity bits
			break;
		default:
			errorStatus = MB_ERROR;
			goto MBS_SERIAL_CONFIG_END;
			break;

	}


	// sets the parity bit
	switch (serial->parityBit) {
		case MBS_PB_NONE:
			Uart->Init.Parity = UART_PARITY_NONE;
			break;
		case MBS_PB_ODD:
			Uart->Init.Parity = UART_PARITY_ODD;
			break;
		case MBS_PB_EVEN:
			Uart->Init.Parity = UART_PARITY_EVEN;
			break;
		default:
			errorStatus = MB_ERROR;
			goto MBS_SERIAL_CONFIG_END;
			break;
	}

	// sets the stop bit
	switch (serial->stopBit) {
		case MBS_SB_1:
			Uart->Init.StopBits = UART_STOPBITS_1;
			break;
		case MBS_SB_2:
			Uart->Init.StopBits = UART_STOPBITS_2;
			break;
		default:
			errorStatus = MB_ERROR;
			goto MBS_SERIAL_CONFIG_END;
			break;
	}

	/*---------configure uart, 1.5 and 3.5 char Timer------------*/
	if(errorStatus == MB_OK){

		Timer_1P5_CHAR->Init.Period = MBS_GetCharTimeCluster(Uart->Init.BaudRate, MBS_1P5_CHAR);	// set the 1.5 char timer period according to the new baud rate
		Timer_3P5_CHAR->Init.Period = MBS_GetCharTimeCluster(Uart->Init.BaudRate, MBS_3P5_CHAR);	// set the 3.5 char timer period according to the new baud rat

		if (HAL_TIM_Base_Init(Timer_1P5_CHAR) != HAL_OK) {
			errorStatus = MB_ERROR;
			goto MBS_SERIAL_CONFIG_END;
		}					// configure 1.5 char timer
		if (HAL_TIM_Base_Init(Timer_3P5_CHAR) != HAL_OK) {
			errorStatus = MB_ERROR;
			goto MBS_SERIAL_CONFIG_END;
		}					// configure 3.5 char timer
		if (HAL_UART_Init(Uart) != HAL_OK) {
			errorStatus = MB_ERROR;
			goto MBS_SERIAL_CONFIG_END;
		}								// configure uart
	}

	/*---------start uart interrupt and the 3.5 & 1.5 char timer------------*/
	MBS_SERIAL_CONFIG_END:							// goto label

	__HAL_UART_ENABLE_IT(Uart, UART_IT_RXNE);		// enable uart receive interrupt
//	MBS_START_TIMER(Timer_1P5_CHAR-); 				//start 1.5 char timer
//	MBS_START_TIMER(Timer_3P5_CHAR); 				//start 3.5 char timer

	return errorStatus;
}


MB_ErrorStatus MBS_GetSerialConfig(MBS_Serial *serial){
	MB_ErrorStatus errorStatus = MB_OK;

	serial->baudRate = Uart->Init.BaudRate;
	switch (Uart->Init.WordLength) {
		case UART_WORDLENGTH_8B:
			serial->dataBits = MBS_DB_7 ;
			break;
		case UART_WORDLENGTH_9B:
			serial->dataBits = MBS_DB_8 ;
			break;
		default:
			errorStatus = MB_ERROR;
			break;
	}



	// sets the parity bit
	switch (Uart->Init.Parity) {
		case UART_PARITY_NONE:
			serial->parityBit = MBS_PB_NONE ;
			break;
		case UART_PARITY_ODD:
			serial->parityBit = MBS_PB_ODD ;
			break;
		case UART_PARITY_EVEN:
			serial->parityBit = MBS_PB_EVEN ;
			break;
		default:
			errorStatus = MB_ERROR;
			break;
	}

	// sets the stop bit
	switch (Uart->Init.StopBits) {
		case UART_STOPBITS_1:
			serial->stopBit= MBS_SB_1 ;
			break;
		case UART_STOPBITS_2:
			serial->stopBit = MBS_SB_2 ;
			break;
		default:
			errorStatus = MB_ERROR;
			break;
	}


	for(uint8_t i = MBS_BR_MIN_INDEX; i < (MBS_BR_MAX_INDEX); i++){
		if(MBS_BaudRate[i] == serial->baudRate){
			serial->baudRateIndex = i;
			break;
		}
	}

//	MBS_EncodeConfigValue(serial);
	return errorStatus;
}

/**
  * @brief encode modbus configuration value
  *
  * @param serialPort: modbus serial port struct
  * @retval None
  */
void MBS_EncodeConfigValue(MBS_Serial *serial){
	serial->serialConfigValue = 0x0000;
	serial->serialConfigValue |=  (uint16_t)((serial->baudRateIndex&0x000F) << 12U);
	serial->serialConfigValue |=  (uint16_t)((serial->dataBits&0x000F) << 8U);
	serial->serialConfigValue |=  (uint16_t)((serial->parityBit&0x000F) << 4U);
	serial->serialConfigValue |=  (uint16_t)(serial->stopBit&0x000F);
}


/**
  * @brief decode modbus configuration value
  *
  * @param serialPort: modbus serial port struct
  * @retval None
  */
void MBS_DecodeConfigValue(MBS_Serial *serial){
	serial->baudRateIndex = (serial->serialConfigValue&0xF000)>>12U;
	serial->dataBits 		= (serial->serialConfigValue&0x0F00)>>8U;
	serial->parityBit		= (serial->serialConfigValue&0x00F0)>>4U;
	serial->stopBit 		= (serial->serialConfigValue&0x000F);
	if((serial->baudRateIndex >= MBS_BR_MIN_INDEX && serial->baudRateIndex < MBS_BR_MAX_INDEX) ){
		serial->baudRate = MBS_BaudRate[serial->baudRateIndex];
	}
}


uint8_t MBS_CheckSerialConfigValidity(MBS_Serial *serial){
	MB_ErrorStatus errorStatus = MB_OK;
//	MBS_DecodeConfigValue(serial);		// decode the serial config value
	// check the baud rate is in the list
	if(!(serial->baudRate <= 65535U && serial->baudRate > 0)){

		errorStatus = MB_ERROR;
	}
	// check databits
	if(!(serial->dataBits >= MBS_DB_7 && serial->dataBits <= MBS_DB_8)){

		errorStatus = MB_ERROR;
	}
	// check parity bits
	if(!(serial->parityBit >= MBS_PB_NONE && serial->parityBit <= MBS_PB_EVEN)){

		errorStatus = MB_ERROR;
	}
	// check stop bit
	if(!(serial->stopBit >= MBS_SB_1 && serial->stopBit <= MBS_SB_2)){

		errorStatus = MB_ERROR;
	}
	return errorStatus;
}



/**
  * @brief send serial data packet
  *
  * @param none
  * @retval None
  */
MB_ErrorStatus MBS_Send(MBS_Serial *serial){

	MB_ErrorStatus errorStatus = MB_OK;
	if(serial->txBuffSize > 0u){
		if(!serial->isSending){
			serial->rxBuffSize = 0;
			HAL_GPIO_WritePin(RS485_TX_LED_GPIO_Port, RS485_TX_LED_Pin, GPIO_PIN_SET);
			serial->setUpstream();
			if(serial->send(serial->txBuff, serial->txBuffSize) != HAL_OK){
				errorStatus = MB_ERROR;
			}else{
				serial->isSending = 1;
				MBS_START_TIMER(Timer_3P5_CHAR);		//start 3.5 char timer
			}
			/*Set the serial receiving mode always*/
			serial->setDownstream();
			HAL_GPIO_WritePin(RS485_TX_LED_GPIO_Port, RS485_TX_LED_Pin, GPIO_PIN_RESET);
		}else{
			errorStatus = MB_ERROR;
		}
	}else{
		errorStatus = MB_ERROR;
	}
	return errorStatus;
}


/**
  * @brief receives serial data byte
  *
  * @param none
  * @retval None
  */
void MBS_Receive(MBS_Serial *serial){

		serial->isReceiving = 1;



		MBS_STOP_TIMER(Timer_1P5_CHAR); //stop 1.5 char timer
		MBS_STOP_TIMER(Timer_3P5_CHAR); //stop 3.5 char timer

		// copy data to the receive buffer
		if(serial->rxBuffSize < MB_PDU_MAX_SIZE){
			serial->rxBuff[serial->rxBuffSize++] = serial->rcvByte();//Uart->Instance->DR;//Uart->Instance->RDR;
		}else{
			serial->rxBuffSize = MB_PDU_MAX_SIZE;
		}

		MBS_START_TIMER(Timer_1P5_CHAR);		//start 1.5 char timer
		MBS_START_TIMER(Timer_3P5_CHAR);		//start 3.5 char timer


		HAL_GPIO_TogglePin(RS485_RX_LED_GPIO_Port, RS485_RX_LED_Pin);
}


void MBS_StopReceiving(MBS_Serial *serial){

	HAL_GPIO_WritePin(RS485_RX_LED_GPIO_Port, RS485_RX_LED_Pin, GPIO_PIN_RESET);
	serial->isSending = 0;
	if(Timer_3P5_CHAR->Instance->SR & TIM_SR_UIF){
		MBS_STOP_TIMER(Timer_3P5_CHAR);		//stop the 3.5 char timer, once a data packet is received
		serial->isDataReceived = 1U;

		if(serial->rxBuffSize <= MB_PDU_MIN_SIZE || serial->rxBuffSize >= MB_PDU_MAX_SIZE){
			serial->rxBuffSize = 0 ;
			serial->isDataReceived = 0U;
		}else{
			MBS_ProcessRcvdFrame(serial);
			serial->isReceiving = 0;
			serial->rxBuffSize = 0 ;
		}
		serial->isReceiving = 0;

	}
}

/**
  * @brief check received data frame
  *
  * @param none
  * @retval None
  */
void MBS_ChackRcvdFrame(MBS_Serial *serial, MB_RTUSlave *mbSlave){

	if(Timer_1P5_CHAR->Instance->SR & TIM_SR_UIF){
		MBS_STOP_TIMER(Timer_1P5_CHAR);		// stop 1.5 char timer
		serial->frameOkFlag = 0U;
		if(serial->rxBuffSize >= (MBS_RTU_PDU_MIN_SIZE-1U)){		// if the buffer size is less than the max buffer size
			if(serial->rxBuff[MB_SA_INDEX] == mbSlave->slave_address
				|| serial->rxBuff[MB_SA_INDEX] == MB_BROADCAST_ADDR){
				uint16_t receivedCRC16 = (serial->rxBuff[serial->rxBuffSize-1] << 8) | serial->rxBuff[serial->rxBuffSize-2];
				uint16_t calculatedCRC16 = MB_CalcCRC16(serial->rxBuff, serial->rxBuffSize-2);
				if(receivedCRC16 == calculatedCRC16){
					mbSlave->rxBuffer = &serial->rxBuff[0];
					mbSlave->txBuffer = &serial->txBuff[0];
					mbSlave->rxBufferSize = serial->rxBuffSize;
					mbSlave->txBufferSize = serial->txBuffSize;
					serial->frameOkFlag=1;
				}else{
					serial->frameOkFlag = 0;
				}
			}
		}
	}

}



/**
  * @brief processes received data packet and response according to the command
  *
  * @param none
  * @retval None
  */
void MBS_ProcessRcvdFrame(MBS_Serial *serial){
	if(serial->isDataReceived > 0U){
		serial->isDataReceived = 0U;

		if(serial->frameOkFlag){	// if the buffer size is less than the max buffer size
			MB_RTUSlave *mbSlave = MBRS_GetInstance();
			MB_ErrorStatus errorStatus = MB_ERROR;
			mbSlave->rxBuffer = &serial->rxBuff[0];
//			memcpy(	mbSlave->rxBuffer, serial->rxBuff, serial->rxBuffSize);
			mbSlave->txBuffer = &serial->txBuff[0];


			mbSlave->rxBufferSize = serial->rxBuffSize;
			mbSlave->txBufferSize = serial->txBuffSize;

			uint8_t statusRet = MBRS_processRequest(mbSlave);

			serial->rxBuffSize = mbSlave->rxBufferSize;
			serial->txBuffSize = mbSlave->txBufferSize;


			if(statusRet>0){
				errorStatus = MBS_Send(serial);
				if(errorStatus != MB_OK){
					DEBUG_SPRINT_NL("Modbus: Transmission Failed!");
				}

				serial->rxBuffSize = 0;
				serial->txBuffSize = 0;
				mbSlave->rxBufferSize = 0;
				mbSlave->txBufferSize = 0;
				MB_ClearBuffer();
			}
			serial->rxBuffSize = 0;
			serial->txBuffSize = 0;
			mbSlave->rxBufferSize = 0;
			mbSlave->txBufferSize = 0;
//			MBRS_PostProcess(mbSlave, errorStatus); // to process some modbus related task after sending the response
		}else{
			MB_ClearBuffer();
		}

		serial->rxBuffSize = 0;
		serial->frameOkFlag = 0U;
	}
}





void MBS_ClearTXBuffer(MBS_Serial *serial){
//	memset(serial->txBuff,0U,serial->txBuffSize);
	serial->txBuffSize = 0U;
}

void MBS_ClearRXBuffer(MBS_Serial *serial){
//	memset(serial->rxBuff,0U,serial->rxBuffSize);
	serial->rxBuffSize = 0U;
}


/**
  * @brief update the modbus serial changes
  * @param serialPort: modbus serial port struct
  * @retval error status
  */
MB_ErrorStatus MBS_UpdateSerialConfig(MBS_Serial *serial){
//	MBS_DecodeConfigValue(serial);
	return MBS_Config(serial);
}




/**
  * @brief to get the 1.5 7 3.5 char time for slave mode
  * @param none
  * @retval return timeout in us
  */
uint32_t MBS_GetCharTimeCluster(uint32_t baudRate, float charType){
	return ceil(1000.0f * charType * MBS_GetByteTxTime(baudRate));
}

/**
  * @brief to get the transmission rate according to the number of byte to be sent
  *
  * @param none
  * @retval return timeout in ms
  */
uint16_t MBS_GetTransTimeout(uint8_t size, uint32_t baudRate){
		return (ceil((float)size*MBS_GetByteTxTime(baudRate)*1.1f)+1U);		// calculate serial transmission timeout

}

/**
  * @brief to get the actual byte transition time.
  * fo 1 byte tx, we actually need to send 11bits in 1 frame
  * 1 frame = 11 bits = 1 stop bit + 8 bits data + 1 parity bit + 1 stop bit
  * @param none
  * @retval return timeout in ms
  */
static float MBS_GetByteTxTime(uint32_t baudRate){
	return (baudRate > 0 ? (11000.0f/(float)baudRate) : MBS_1CHAR_TX_TIMEOUT_MAX);
}



