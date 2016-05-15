#include "sim800l.h"
#include "timers_mngr\timers_mngr.h"
#include "nv_config\nv_config.h"
#include "buttons_mngr\buttons_mngr.h"
#include <ctype.h>

#define MODULE_OK 2000
#define MODULE_RESET 500
#define MODULE_CONFIG_MODE 250

extern float gTemperature;

bool configMode = false;

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
	Config newConfig;
	
	if(smsToSend == NULL) {
		return;
	}
	
	for ( ; *p; ++p) *p = tolower(*p);
	
	SMSMessageParse(smsReceived->message, &content);
	
	if(strstr(content.pin, gNVConfig->pin)) {
		strcpy(smsToSend->telNumber, smsReceived->telNumber);
		if(strstr(content.action, "set")) {
			if(strstr(content.variable, "output")) {
				if(strstr(content.value, "high") || strstr(content.value, "1") || strstr(content.value, "on")) {
					strcpy(smsToSend->message, "Output set to high");
					GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)1);
					memcpy(&newConfig, gNVConfig, sizeof(Config));
					newConfig.outputActive = true;
					NVConfigSave(&newConfig);					
				} else {
					strcpy(smsToSend->message, "Output set to low");
					GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)0);
					memcpy(&newConfig, gNVConfig, sizeof(Config));
					newConfig.outputActive = false;
					NVConfigSave(&newConfig);
				}
			} else if(strstr(content.variable, "answer")) {
				if(strstr(content.value, "yes") || strstr(content.value, "true")) {
					strcpy(smsToSend->message, "Answers will be send");
					memcpy(&newConfig, gNVConfig, sizeof(Config));
					newConfig.sendAnswer = true;
					NVConfigSave(&newConfig);					
				} else {
					strcpy(smsToSend->message, "Answers will not be send");
					memcpy(&newConfig, gNVConfig, sizeof(Config));
					newConfig.sendAnswer = false;
					NVConfigSave(&newConfig);						
				}				
			} else if(strstr(content.variable, "pin")) {
				if(true == configMode) {
					configMode = false;
					StatusLEDBlinkRate(MODULE_OK);
					memcpy(&newConfig, gNVConfig, sizeof(Config));
					strncpy(newConfig.pin, content.value, 4);
					NVConfigSave(&newConfig);
					strcpy(smsToSend->message, "pin has been changed");
				}
			} else {
				strcpy(smsToSend->message, "Unknown set variable");
			}		
		} else if(strstr(content.action, "get")) {
			if(strstr(content.variable, "temperature")) {
				sprintf(smsToSend->message, "Actual temperature: %.1f", gTemperature);
			} else if(strstr(content.variable, "output")) {
				if(gNVConfig->outputActive) {
					strcpy(smsToSend->message, "Output state high");
				} else {
					strcpy(smsToSend->message, "Output state low");
				}
			} else {
				strcpy(smsToSend->message, "Unknown get variable");
			}
		} else {
			strcpy(smsToSend->message, "Unknown action");
		}
		
		if(gNVConfig->sendAnswer == true) {
			ModuleGSMSMSSend(smsToSend);
		}
	}
	
}

void ModuleGSMSMSSendedCallback(Response response) {
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void ButtonShortPressedCallback(Button button) {
	
}

void ButtonLongPressedCallback(Button button) {
	if(false == configMode) {
		configMode = true;
		StatusLEDBlinkRate(MODULE_CONFIG_MODE);
	} else {
		configMode = false;
		StatusLEDBlinkRate(MODULE_OK);		
	}
}

