#include "sim800l.h"
#include "timers_mngr\timers_mngr.h"

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
	
	SMS *smsToSend = SMSCreate();
	if(strstr(smsReceived->message, "Zapal")) {
		GPIOC->BSRR |= GPIO_Pin_9;
		if(smsToSend) {strcpy(smsToSend->message, "Dioda zaswiecona");}
	} else if(strstr(smsReceived->message, "Zgas")) {
		GPIOC->BRR |= GPIO_Pin_9;
		if(smsToSend) {strcpy(smsToSend->message, "Dioda zgaszona");}
	}	else {
		if(smsToSend) {strcpy(smsToSend->message, "Nieznane polecenie");}
	}			
	if(smsToSend) {strcpy(smsToSend->telNumber, smsReceived->telNumber);}
	
	if(smsToSend) {
		ModuleGSMSMSSend(smsToSend);
	}
}

void ModuleGSMSMSSendedCallback(Response response) {
	
}

