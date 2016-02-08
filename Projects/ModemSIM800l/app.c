#include "sim800l.h"
#include "timers_mngr\timers_mngr.h"
#include <ctype.h>

extern float gTemperature;

#define MODULE_OK 2000
#define MODULE_RESET 500

static void StatusLEDBlinkRate(uint16_t rate){
	TimersMngrSetReloadValue(0, rate);
}

void ModuleGSMResetCallback(void) {
	StatusLEDBlinkRate(MODULE_RESET);
}

void ModuleGSMConnectedToNetworkCallback(void) {
	StatusLEDBlinkRate(MODULE_OK);
}

void ModuleGSMSMSReceivedCallback(SMS *smsReceived) {
	
	MsgContent content;
	char *p = smsReceived->message;
	SMS *smsToSend = SMSCreate();
	
	if(smsToSend == NULL) {
		return;
	}
	
	for ( ; *p; ++p) *p = tolower(*p);
	
	SMSMessageParse(smsReceived->message, &content);
	
	if(strstr(content.password, "1111")) {
		strcpy(smsToSend->telNumber, smsReceived->telNumber);
		if(strstr(content.action, "set")) {
			if(strstr(content.variable, "output")) {
				if(strstr(content.value, "high") || strstr(content.value, "1")) {
					strcpy(smsToSend->message, "Output set to high");
					GPIOC->BSRR |= GPIO_Pin_9;
				} else {
					strcpy(smsToSend->message, "Output set to low");
					GPIOC->BRR |= GPIO_Pin_9;
				}
			} else {
				strcpy(smsToSend->message, "Unknown set variable");
			}		
		} else if(strstr(content.action, "get")) {
			if(strstr(content.variable, "temperature")) {
				sprintf(smsToSend->message, "Actual temperature: %.1f", gTemperature);
			} else {
				strcpy(smsToSend->message, "Unknown get variable");
			}
		} else {
			strcpy(smsToSend->message, "Unknown action");
		}
		
		ModuleGSMSMSSend(smsToSend);
	}
	
}

void ModuleGSMSMSSendedCallback(Response response) {
	
}

