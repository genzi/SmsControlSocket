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
	 
typedef struct {
	State currentState;
	
} sim800l;

sim800l moduleGSM;

void ModuleGSMInit(void);
void ModuleGSMProcess(void);
void ModuleGSMReset(void);
void ModuleGSMEnable(void);
int SendCommand(char *command);
	 
	 
	 
#ifdef __cplusplus
}
#endif

#endif /* __SIM800L_H */
