#include "master.h"
//discoveryf303, 
// ledS blinks -"im working"
// ledNE blinks -"master received OK
// ledE blinks -"master received ERROR
// ledSE blinks -"master not received 

volatile uint8_t dmaTxBufMaster[dmaBufSize];
volatile uint8_t dmaRxBufMaster[dmaBufSize];
extern volatile uint8_t dmaRxBufSlave[];//!!!
//volatile uint8_t masterMsg[masterMsgSize];

uint8_t simple=0;
uint16_t masterState=0x02;
//0x01 - tick timeout
//0x02 - tick next
uint16_t masterTimeout;
uint16_t masterNextSend;

//launches from SysTick_Handler,1ms 
void masterTasks(){
	if(masterState&0x01){
		if(masterTimeout--==0){
			//UART masterDMA HALT;!!!s
			HAL_GPIO_TogglePin(GPIOE,ledSE_Pin);
			masterState&=~0x01;
		}		
	}
	if(masterState&0x02){
		if(masterNextSend--==0){
			masterState|=0x01;
			masterNextSend=constPeriod;
			masterTimeout=constTimeout;
			sendMessageMaster();
		}
	}
	if(simple--==0){
		HAL_GPIO_TogglePin(GPIOE,ledS_Pin);		
	}
}


void sendMessageMaster(){
	HAL_UART_Receive_DMA(&huart2,(uint8_t*)dmaRxBufSlave,masterMsgSize);//!!!
	HAL_UART_Transmit_DMA(&huart1,(uint8_t*)dmaTxBufMaster,masterMsgSize);
}

//launches from USART1 or his DMA channel
void receiveMessageMaster(DMA_HandleTypeDef *hdma){
	//
	HAL_GPIO_TogglePin(GPIOE,ledNW_Pin);		
	if(1){//!!!
		DW_Type crc;
		uint8_t receivedLen=dmaRxBufMaster[0];
		if(receivedLen>=4)
			crc.dw = HAL_CRC_Calculate(&hcrc,(uint32_t*)(dmaRxBufMaster+3),receivedLen-3);
		if((crc.b[0]==dmaRxBufMaster[1]) &&
			(crc.b[1]==dmaRxBufMaster[2])){
			if(dmaRxBufMaster[3]==HSK_OK)
				HAL_GPIO_TogglePin(GPIOE,ledNE_Pin);
			else if(dmaRxBufMaster[3]==HSK_ERROR)
				HAL_GPIO_TogglePin(GPIOE,ledE_Pin);
		}	
	}
}

void updateMsg(){
	DW_Type crc;
	//incrementing message
	for(int i=0;i<(masterMsgSize-3);i++){
		dmaTxBufMaster[i+3]++;
		if(dmaTxBufMaster[i+3]==0){
			break;
		}
	}

	crc.dw=HAL_CRC_Calculate(&hcrc,(uint32_t*)(dmaTxBufMaster+3), masterMsgSize-3);
	dmaTxBufMaster[0]=masterMsgSize-1;
	dmaTxBufMaster[1]=crc.b[0];
	dmaTxBufMaster[2]=crc.b[1];
}

void makeMaster(){
	//1)init	masterMsg
	for(int i=0;i<(masterMsgSize-3);i++){
		dmaTxBufMaster[i+3]=i+1;
	}	
	//init callbacks
	hdma_usart1_rx.XferCpltCallback = receiveMessageMaster;
	hdma_usart2_rx.XferCpltCallback = receiveMessageSlave;
	while(1){
		HAL_Delay(100);
	}
}

