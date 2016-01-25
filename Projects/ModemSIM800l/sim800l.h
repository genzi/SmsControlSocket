/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SIM800L_H
#define __SIM800L_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
	 
typedef enum {
	RESETING,
	STARTING,
	CHECK_ALIVE,
	CHECK_PIN,
	WRITE_PIN,
	CHECK_SMS_MODE,
	SET_SMS_TXT_MODE,
	CHECK_NET_REG,
	READY,
	CHECK_NEW_SMS,
	READ_NEW_SMS,
	SEND_SMS_TO,
	SEND_SMS_ASNWER
} State;

typedef enum {
	RESP_OK,
	RESP_ERROR,
	RESP_WAIT
} Response;
	 
typedef struct {
	State currentState;
	
} sim800l;

sim800l moduleGSM;
int ModuleGSMDelayCounter;

void ModuleGSMInit(void);
void ModuleGSMProcess(void);
void ModuleGSMReset(void);
void ModuleGSMEnable(void);
Response SendCommand(char *command);

void ModuleGSMDelaySet(void);
void ModuleGSMDelayDecrement(void);
Response ModuleGSMDelayCheck(void);


	 
	 
	 
#ifdef __cplusplus
}
#endif

#endif /* __SIM800L_H */
