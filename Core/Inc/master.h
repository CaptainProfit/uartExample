#ifndef __MASTER_H
#define __MASTER_H
#include "main.h"

#define constPeriod 300

#define constTimeout (constPeriod/2)
#define dmaBufSize 32
#define masterMsgSize 23
#define slaveMsgSize 4
extern CRC_HandleTypeDef hcrc;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern void sendMessageMaster(void);
extern void receiveMessageMaster(DMA_HandleTypeDef *hdma);
extern void makeMaster(void);
extern void masterTasks(void);

extern void receiveMessageSlave(DMA_HandleTypeDef *hdma);
extern void sendMessageSlave(void);

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;
typedef union{
	uint8_t b[4];
	uint16_t w[4];
	uint32_t dw;
} DW_Type;
typedef enum{HSK_OK=1,HSK_ERROR=2} HSK_Type;

#endif 
