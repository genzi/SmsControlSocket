/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SIM800L_H
#define __SIM800L_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "queue\Queue.h"
#include <string.h>
#include <stdbool.h>
#include "sms\sms.h"
	 
	 
extern void USART_Send(USART_TypeDef* USARTx, uint8_t size);
extern __IO uint8_t TxBuffer[];	 
extern __IO uint8_t RxBuffer[];
extern __IO uint16_t RxCount;
	 
typedef enum {
	RESETING,
	STARTING,
	AT,
	AT_RESPONSE,
	CHECK_PIN,
	CHECK_PIN_RESPONSE,
	CHECK_CREG,
	CHECK_CREG_RESPONSE,
	WRITE_PIN,
	CHECK_SMS_MODE,
	SET_SMS_TXT_MODE,
	SET_SMS_TXT_MODE_RESPONSE,
	READY,
	CHECK_NEW_SMS,
	CHECK_NEW_SMS_RESPONSE,
	READ_NEW_SMS,
	READ_NEW_SMS_RESPONSE,
	DELETE_ALL_SMS,
	SEND_SMS,
	SEND_SMS_PROMPT,
	SEND_SMS_CONTENT,
	SEND_SMS_RESPONSE,
	DELAY,
	WAIT_FOR_RESPONSE,
	IDLE
} State;

typedef enum {
	RESP_OK,
	RESP_ERROR,
	RESP_WAIT,
	RESP_TIMEOUT
} Response;
	 
typedef struct {
	State currentState;
	State nextState;
} sim800l;

extern sim800l moduleGSM;
extern Queue *gQueueSimUsart;
extern __IO bool newDataUSART1Flag;

void ModuleGSMInit(void);
void ModuleGSMMainProcess(void);
void ModuleGSMStateMachineProcess(void);
void ModuleGSMRxBufferAnalyzeProcess(volatile uint8_t *RxBuffer, volatile uint16_t RxCount, volatile bool newDataFlag);
void ModuleGSMReset(void);
void ModuleGSMEnable(void);
void SendCommand(char *command);
void SendCommandWithNum(char *command, int num);
void SendCommandWithStr(char *command, char *str);
void SendSMSContent(char *content);

void ModuleGSMDelaySetMs(int timer, int msDelay);
void ModuleGSMDelayDecrementMs(void);
void ModuleGSMDelayCancel(int timer);
Response ModuleGSMDelayCheckMs(int timer);

void ModuleGSMSMSSend(SMS *lSmsToSend);

//Callback functions
//void ModuleGSMSMSReceivedCallBack(SMS *smsReceived);
	 
	 
	 
#ifdef __cplusplus
}
#endif

#endif /* __SIM800L_H */
