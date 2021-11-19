#include "master.h"
//discoveryf303, 
// ledS blinks -"im working"
// ledN blinks -"master received 

volatile uint8_t dmaTxBufSlave[dmaBufSize];
volatile uint8_t dmaRxBufSlave[dmaBufSize];
extern volatile uint8_t dmaRxBufMaster[];//!!!
volatile uint8_t receivedLen;
//volatile uint8_t slaveMsg[slaveMsgSize];

void sendMessageSlave(){
	HAL_UART_Receive_DMA(&huart1,(uint8_t*)dmaRxBufMaster,slaveMsgSize);//!!!
	HAL_UART_Transmit_DMA(&huart2,(uint8_t*)dmaTxBufSlave,slaveMsgSize);
}
void createMessageSlave(HSK_Type hsk){
	DW_Type crc;
	dmaTxBufSlave[0]=3;
	crc.dw=HAL_CRC_Calculate(&hcrc,(uint32_t*)(dmaTxBufSlave+2), slaveMsgSize-3);
	dmaTxBufSlave[1]=crc.b[0];
	dmaTxBufSlave[2]=crc.b[1];
	dmaTxBufSlave[3]=(uint8_t)hsk;
}
//launches from USART2 or his DMA channel
void receiveMessageSlave(DMA_HandleTypeDef *hdma){
	HAL_GPIO_TogglePin(GPIOE,ledW_Pin);		
	if(1){//!!!
		DW_Type crc;
		receivedLen=masterMsgSize-1;
		//1 check size
		if(dmaRxBufSlave[0]==receivedLen){
				crc.dw=HAL_CRC_Calculate(&hcrc,(uint32_t*)(dmaRxBufSlave+2), receivedLen-2);
			//check CRC
			if((crc.b[0]==dmaRxBufSlave[1]) &&
				(crc.b[1]==dmaRxBufSlave[2])){
					createMessageSlave(HSK_OK);
				}
			else createMessageSlave(HSK_ERROR);
		}		
		else createMessageSlave(HSK_ERROR);
		sendMessageSlave();
	}
}

