#include "sim800l.h"
#include "log\\logging.h"
#include "timers_mngr\timers_mngr.h"

#define DELAY_BTW_CMDS 100

/* Public variables ----------------------------------------------------------*/
sim800l moduleGSM;
Queue *gQueueSimUsart;
Queue *gQueueSimURC;

/* Private variables ----------------------------------------------------------*/
static volatile int ModuleGSMDelayCounter[10];
static char ResponseBuffer[512];
static SMS *smsReceived;
static SMS *smsToSend;

#define MODULE_OK 2000
#define MODULE_RESET 500

/**
	Callback functions
*/

__weak void ModuleGSMSMSStartedCallback(void) {
	
}

__weak void ModuleGSMSMSConnectedToNetworkCallback(void) {
	
}

__weak void ModuleGSMSMSReceivedCallback(SMS *smsReceived) {
	
}

__weak void ModuleGSMSMSSendedCallback(Response response) {
	
}

///////////////////////////////////////////////////////////////////////////////////////////

static void StatusLEDBlinkRate(uint16_t rate){
	TimersMngrSetReloadValue(0, rate);
}


static bool ModuleGSMResponseOK() {
	return strstr(ResponseBuffer, "\r\nOK\r\n") ? true : false;
}

//static bool ModuleGSMResponseERROR() {
//	return strstr(ResponseBuffer, "\r\nERROR\r\n") ? true : false;
//}

static void ModuleGSMSetDelayToNextState(int msDelay, State nextSate) {
	ModuleGSMDelaySetMs(0, msDelay);
	moduleGSM.nextState = nextSate;
	moduleGSM.currentState = DELAY;
	Log(gLogData, eSubSystemSIM800L, eInfoLogging, "goes to DELAY");
}

static void ModuleGSMWaitForResponse(int msDelay, State nextSate) {
	ModuleGSMDelaySetMs(0, msDelay);
	moduleGSM.nextState = nextSate;
	moduleGSM.currentState = WAIT_FOR_RESPONSE;
	Log(gLogData, eSubSystemSIM800L, eInfoLogging, "goes to WAIT_FOR_RESPONSE");
}

static void ClearRxBufferAndCounter(void) {
	memset((void *)RxBuffer, 0, RxCount);
	RxCount = 0;
}

void ModuleGSMStateMachineProcess(void)
{
	char *pStr[3];
	char *pStrTok[3];
	static uint8_t smsNumber;
	
	switch(moduleGSM.currentState)
	{
		case DELAY:
		case WAIT_FOR_RESPONSE:
			if(ModuleGSMDelayCheckMs(0) != RESP_WAIT)
			{
				moduleGSM.currentState = moduleGSM.nextState;
				LogWithNum(gLogData, eSubSystemSIM800L, eInfoLogging, "goes to State: %d", moduleGSM.nextState);
			}
		break;
			
		case RESETING:
			StatusLEDBlinkRate(MODULE_RESET);
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "RESETING");
			ModuleGSMReset();
			ModuleGSMSetDelayToNextState(500, STARTING);
		break;
		
		case STARTING:
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "STARTING");
			ModuleGSMEnable();
			ModuleGSMSetDelayToNextState(3000, AT);
		break;
			
		case AT:
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Sending AT");
			ModuleGSMWaitForResponse(100, AT_RESPONSE);
			SendCommand("AT\r\n");
		break;
		
		case AT_RESPONSE:			
			if(Queue_read(gQueueSimUsart, ResponseBuffer) != -1) {
				if(ModuleGSMResponseOK()) {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "AT_RESPONSE OK");
					ModuleGSMSMSStartedCallback();
					ModuleGSMSetDelayToNextState(3000, CHECK_PIN);				
				} else {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "AT_RESPONSE ERROR");
					ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, IDLE);					
				}
			} else {
				Log(gLogData, eSubSystemSIM800L, eErrorLogging, "AT_RESPONSE TIMEOUT");
				ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, IDLE);				
			}					
		break;
			
		case CHECK_PIN:
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Sending AT+CPIN?");
			ModuleGSMWaitForResponse(100, CHECK_PIN_RESPONSE);
			SendCommand("AT+CPIN?\r\n");			
		break;
		
		case CHECK_PIN_RESPONSE:
			if(Queue_read(gQueueSimUsart, ResponseBuffer) != -1) {
				if(strstr(ResponseBuffer, "+CPIN: READY")) {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "CHECK_PIN_RESPONSE OK");
					ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, SET_SMS_TXT_MODE);				
				} else {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "CHECK_PIN_RESPONSE ERROR");
					ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, IDLE);					
				}
			} else {
				Log(gLogData, eSubSystemSIM800L, eErrorLogging, "CHECK_PIN_RESPONSE TIMEOUT");
				ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, IDLE);				
			}				
		break;
			
		case SET_SMS_TXT_MODE:
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Sending AT+CMGF=1");
			ModuleGSMWaitForResponse(100, SET_SMS_TXT_MODE_RESPONSE);
			SendCommand("AT+CMGF=1\r\n");			
		break;
		
		case SET_SMS_TXT_MODE_RESPONSE:
			if(Queue_read(gQueueSimUsart, ResponseBuffer) != -1) {
				if(ModuleGSMResponseOK()) {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "CMGF=1_RESPONSE OK");
					ModuleGSMSetDelayToNextState(100, CHECK_CREG);				
				} else {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "CMGF=1_RESPONSE ERROR");
					ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, IDLE);					
				}
			} else {
				Log(gLogData, eSubSystemSIM800L, eErrorLogging, "CMGF=1_RESPONSE TIMEOUT");
				ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, IDLE);				
			}				
		break;
			
		case 	CHECK_CREG:
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Sending AT+CREG?");
			ModuleGSMWaitForResponse(100, CHECK_CREG_RESPONSE);
			SendCommand("AT+CREG?\r\n");			
		break;
		
		case CHECK_CREG_RESPONSE:
			if(Queue_read(gQueueSimUsart, ResponseBuffer) != -1) {
				if(strstr(ResponseBuffer, "+CREG: 0,5") ||
					 strstr(ResponseBuffer, "+CREG: 0,1")) {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Registered in network");
					StatusLEDBlinkRate(MODULE_OK);
					ModuleGSMSMSConnectedToNetworkCallback();
					ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, READY);				
				} else if(strstr(ResponseBuffer, "+CREG: 0,2")) {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "searching network");
					ModuleGSMSetDelayToNextState(3000, CHECK_CREG);
				} else {
					Log(gLogData, eSubSystemSIM800L, eErrorLogging, "error cannot register");
					ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, IDLE);					
				}
			} else {
				Log(gLogData, eSubSystemSIM800L, eErrorLogging, "CHECK_CREG_RESPONSE TIMEOUT");
				ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, IDLE);				
			}				
		break;
			
		case READY:
			//TODO have to check CREG registrartion in network; I need use timer
			//but without callback, only read here actual timer counter
			//analyze all changes state in reaction to ok, error etc
			if(Queue_read(gQueueSimURC, ResponseBuffer) != -1) {
				if((pStr[0] = strstr((char *)ResponseBuffer, "+CMTI:")) != NULL) {
					if((pStr[1] = strstr(pStr[0], "\r\n")) != NULL) {
						pStrTok[0] = strtok(pStr[0], ",");
						pStrTok[1] = strtok(NULL, ",");
						smsNumber = atoi(pStrTok[1]);
						moduleGSM.currentState = READ_NEW_SMS;
					}
				} else if((pStr[0] = strstr((char *)ResponseBuffer, "RING")) != NULL) {
					//for now do nothing
				}
			} else if(smsToSend) {
				ModuleGSMSetDelayToNextState(1000, SEND_SMS);
			} else if(RESP_OK == ModuleGSMDelayCheckMs(1)) {
				ModuleGSMDelaySetMs(1, 60000);
				ModuleGSMSetDelayToNextState(20, CHECK_CREG);
			}
		break;
		
		case READ_NEW_SMS:
			LogWithNum(gLogData, eSubSystemSIM800L, eInfoLogging, "Send AT+CMGR=%d", smsNumber);
			ModuleGSMWaitForResponse(1000, READ_NEW_SMS_RESPONSE);
			SendCommandWithNum("AT+CMGR=%d\r\n", smsNumber);
		break;
			
		case READ_NEW_SMS_RESPONSE:
			if(Queue_read(gQueueSimUsart, ResponseBuffer) != -1) {
				Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Message readed");
				
				if(SMSParse(smsReceived, ResponseBuffer)) {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Message parsed");
					ModuleGSMSMSReceivedCallback(smsReceived);
				} else {
					Log(gLogData, eSubSystemSIM800L, eErrorLogging, "Message parse err");
				}
				ModuleGSMSetDelayToNextState(100, DELETE_ALL_SMS);
			} else {
				Log(gLogData, eSubSystemSIM800L, eErrorLogging, "Message read err");
				ModuleGSMSetDelayToNextState(100, READY);
			}			
		break;
			
		case DELETE_ALL_SMS:
			ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, READY);
			SendCommand("AT+CMGD=1,4\r\n");
		break;
		
		case SEND_SMS:
			ClearRxBufferAndCounter();
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Send AT+CMGS=telnumber");
			ModuleGSMWaitForResponse(1000, SEND_SMS_PROMPT);
			SendCommandWithStr("AT+CMGS=\"%s\"\r\n", smsToSend->telNumber);
		break;
		
		case SEND_SMS_PROMPT:
			if(Queue_read(gQueueSimUsart, ResponseBuffer) != -1) {
				if(strstr(ResponseBuffer, ">")) {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "sms send received > ");
					ModuleGSMSetDelayToNextState(100, SEND_SMS_CONTENT);				
				} else {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "CMGS prompt err");
					SMSDestroy(&smsToSend);
					ModuleGSMSetDelayToNextState(1000, READY);					
				}
			} else {
				Log(gLogData, eSubSystemSIM800L, eErrorLogging, "CMGS TIMEOUT");
				SMSDestroy(&smsToSend);
				ModuleGSMSetDelayToNextState(1000, READY);				
			}			
		break;
			
		case SEND_SMS_CONTENT:
			ClearRxBufferAndCounter();
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "sending sms content");
			ModuleGSMWaitForResponse(60000, SEND_SMS_RESPONSE);
			SendSMSContent(smsToSend->message);			
		break;
		
		case SEND_SMS_RESPONSE:
			if(Queue_read(gQueueSimUsart, ResponseBuffer) != -1) {
				if(ModuleGSMResponseOK()) {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "SEND_SMS_RESPONSE OK");
					SMSDestroy(&smsToSend);
					ModuleGSMSMSSendedCallback(RESP_OK);
					ModuleGSMSetDelayToNextState(100, READY);				
				} else {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "SEND_SMS_RESPONSE ERROR");
					SMSDestroy(&smsToSend);
					ModuleGSMSMSSendedCallback(RESP_ERROR);
					ModuleGSMSetDelayToNextState(1000, READY);					
				}
			} else {
				Log(gLogData, eSubSystemSIM800L, eErrorLogging, "SEND_SMS_RESPONSE TIMEOUT");
				SMSDestroy(&smsToSend);
				ModuleGSMSMSSendedCallback(RESP_TIMEOUT);
				ModuleGSMSetDelayToNextState(1000, READY);				
			}			
		break;
		
		case IDLE:
			LogWithNum(gLogData, eSubSystemSIM800L, eErrorLogging, "IDLE state", moduleGSM.currentState);
			ModuleGSMSetDelayToNextState(5000, RESETING);			
		break;

		default:
			LogWithNum(gLogData, eSubSystemSIM800L, eErrorLogging, "UNDEFINED state:%d", moduleGSM.currentState);
			ModuleGSMSetDelayToNextState(5000, RESETING);
		break;
	}
}

void ModuleGSMRxBufferAnalyzeProcess(volatile uint8_t *RxBuffer, volatile uint16_t RxCount, volatile bool newDataFlag) {
		
	char *pStr[3];
	
	if(newDataFlag == false) {
		return;
	}
	newDataFlag = false;
	
	//Handle URC
	if((pStr[0] = strstr((char *)RxBuffer, "+CMTI:")) != NULL) {
		if((pStr[1] = strstr(pStr[0], "\r\n")) != NULL) {
			if(Queue_write(gQueueSimURC, pStr[0], (pStr[1] - pStr[0] + 2)) != -1) {
				ClearRxBufferAndCounter();
			}
		}
	} else if((pStr[0] = strstr((char *)RxBuffer, "RING")) != NULL) {
		if(Queue_write(gQueueSimURC, pStr[0], 4) != -1) {
			ClearRxBufferAndCounter();
		}
	}
			
	//Handle responses
	if(moduleGSM.currentState == WAIT_FOR_RESPONSE) {
		if((pStr[0] = strstr((char *)RxBuffer, "+CMGR:")) != NULL) {
			if((pStr[1] = strstr(pStr[0], "OK\r\n")) != NULL) {
				if(Queue_write(gQueueSimUsart, pStr[0], (pStr[1] - pStr[0] + 4)) != -1) {
					ClearRxBufferAndCounter();
					moduleGSM.currentState = READ_NEW_SMS_RESPONSE;			
				}					
			}
		} else if(strstr((char *)RxBuffer, "\r\nOK\r\n") ||
						  strstr((char *)RxBuffer, "\r\nERROR\r\n")) {
			if(Queue_write(gQueueSimUsart, (char *)RxBuffer, RxCount+1) != -1) {
				ClearRxBufferAndCounter();
				moduleGSM.currentState = moduleGSM.nextState;
			}
		} else if(strstr((char *)RxBuffer, ">")) {
			if(Queue_write(gQueueSimUsart, ">", 1) != -1) {
				ClearRxBufferAndCounter();
				moduleGSM.currentState = moduleGSM.nextState;
			}				
		}
	}
}

void ModuleGSMInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	gQueueSimUsart = Queue_create(1,512);
	gQueueSimURC = Queue_create(1,40);
	
	if(gQueueSimUsart) {
		Log(gLogData, eSubSystemSIM800L, eInfoLogging, "QueueSimUsart created");
	} else {
		Log(gLogData, eSubSystemSIM800L, eFatalErrorLogging, "QueueSimUsart init error");
		while(true){}	//TODO how to handle this error
	}
	
	if(gQueueSimURC) {
		Log(gLogData, eSubSystemSIM800L, eInfoLogging, "QueueSimURC created");
	} else {
		Log(gLogData, eSubSystemSIM800L, eFatalErrorLogging, "QueueSimURC init error");
		while(true){}	//TODO how to handle this error
	}
	
	smsReceived = SMSCreate();
	
	if(smsReceived) {
		Log(gLogData, eSubSystemSIM800L, eInfoLogging, "smsReceived created");
	} else {
		Log(gLogData, eSubSystemSIM800L, eFatalErrorLogging, "smsReceived init error");
		while(true){}	//TODO how to handle this error
	}
	
  /* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  /* Configure PC10 and PC11 in output pushpull mode */
																//RESET				//DTR
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7);

	moduleGSM.currentState = RESETING;
	
}

/**
	Main process executes RxBufferAnalyzeProcess which analyzes buffer that is filling in uart irq
	and executes StateMachineProcess which depends of actual modem state and data arrived
*/

void ModuleGSMMainProcess() {
	ModuleGSMRxBufferAnalyzeProcess(RxBuffer, RxCount, newDataUSART1Flag);
	ModuleGSMStateMachineProcess();
}

void ModuleGSMReset(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
}

void ModuleGSMEnable(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
}

void SendCommand(char *command)
{
	strcpy((char *)TxBuffer, command);
	USART_Send(USART1, strlen((char *)TxBuffer));	
}

void SendCommandWithNum(char *command, int num)
{
	sprintf((char *)TxBuffer, command, num);
	USART_Send(USART1, strlen((char *)TxBuffer));	
}

void SendCommandWithStr(char *command, char *str)
{
	sprintf((char *)TxBuffer, command, str);
	USART_Send(USART1, strlen((char *)TxBuffer));	
}

void SendSMSContent(char *content)
{
	sprintf((char *)TxBuffer, "%s%c", content, '\x1A');
	USART_Send(USART1, strlen((char *)TxBuffer));	
}

/**
	Delay functions
		void ModuleGSMDelayCancel(int msDelay);
		void ModuleGSMDelaySetMs(int, void);
		void ModuleGSMDelayDecrementMs(void);
		Response ModuleGSMDelayCheckMs(int timer);
*/

void ModuleGSMDelayCancel(int timer, int msDelay)
{
	if(timer > 9) {
		return;
	}
	ModuleGSMDelayCounter[timer] = 0;
}

void ModuleGSMDelaySetMs(int timer, int msDelay)
{
	if(timer > 9) {
		return;
	}
	ModuleGSMDelayCounter[timer] = msDelay;
}

void ModuleGSMDelayDecrementMs(void)
{
	int i;
	for(i = 0; i < 10; i++) {
		if (ModuleGSMDelayCounter[i] != 0x00) { 
			ModuleGSMDelayCounter[i]--;
		}
	}
}

Response ModuleGSMDelayCheckMs(int timer)
{
	if(timer > 9) {
		return RESP_ERROR;
	}
	if(ModuleGSMDelayCounter[timer] == 0)
	{
		return RESP_OK;
	}
	
	return RESP_WAIT;
}

void ModuleGSMSMSSend(SMS *lSmsToSend) {
	smsToSend = lSmsToSend;
}

