/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SIM800L_H
#define __SIM800L_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include <string.h>
#include "queue\Queue.h"
	 
extern void USART_Send(USART_TypeDef* USARTx, uint8_t size);
extern uint8_t TxBuffer[];	 
	 
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
	READ_NEW_SMS,
	SEND_SMS_TO,
	SEND_SMS_ASNWER,
	DELAY,
	WAIT_FOR_RESPONSE,
	IDLE
} State;

typedef enum {
	RESP_OK,
	RESP_ERROR,
	RESP_WAIT
} Response;
	 
typedef struct {
	State currentState;
	State nextState;
} sim800l;

extern sim800l moduleGSM;
extern Queue *gQueueSimUsart;

void ModuleGSMInit(void);
void ModuleGSMProcess(void);
void ModuleGSMReset(void);
void ModuleGSMEnable(void);
void SendCommand(char *command);

void ModuleGSMDelaySetMs(int msDelay);
void ModuleGSMDelayDecrementMs(void);
Response ModuleGSMDelayCheckMs(void);


	 
	 
	 
#ifdef __cplusplus
}
#endif

#endif /* __SIM800L_H */
