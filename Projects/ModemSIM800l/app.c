#include "sim800l.h"


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

