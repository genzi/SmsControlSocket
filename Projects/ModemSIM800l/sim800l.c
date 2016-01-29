#include "sim800l.h"
#include "log\\logging.h"
#include "timers_mngr\timers_mngr.h"

#define DELAY_BTW_CMDS 100

/* Public variables ----------------------------------------------------------*/
sim800l moduleGSM;
Queue *gQueueSimUsart;

/* Private variables ----------------------------------------------------------*/
static volatile int ModuleGSMDelayCounter;
static char ResponseBuffer[512];

#define MODULE_OK 2000
#define MODULE_RESET 500

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
	ModuleGSMDelaySetMs(msDelay);
	moduleGSM.nextState = nextSate;
	moduleGSM.currentState = DELAY;
	Log(gLogData, eSubSystemSIM800L, eInfoLogging, "goes to DELAY");
}

static void ModuleGSMWaitForResponse(int msDelay, State nextSate) {
	ModuleGSMDelaySetMs(msDelay);
	moduleGSM.nextState = nextSate;
	moduleGSM.currentState = WAIT_FOR_RESPONSE;
	Log(gLogData, eSubSystemSIM800L, eInfoLogging, "goes to WAIT_FOR_RESPONSE");
}

void ModuleGSMProcess(void)
{
	switch(moduleGSM.currentState)
	{
		case DELAY:
		case WAIT_FOR_RESPONSE:
			if(ModuleGSMDelayCheckMs() != RESP_WAIT)
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
					ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, READY);				
					StatusLEDBlinkRate(MODULE_OK);
				} else {
					Log(gLogData, eSubSystemSIM800L, eInfoLogging, "AT_RESPONSE ERROR");
					ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, READY);					
				}
			} else {
				Log(gLogData, eSubSystemSIM800L, eErrorLogging, "AT_RESPONSE TIMEOUT");
				ModuleGSMSetDelayToNextState(DELAY_BTW_CMDS, IDLE);				
			}					
		break;
			
		case READY:
			
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

void ModuleGSMInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	gQueueSimUsart = Queue_create(1,512);
	
	if(gQueueSimUsart) {
		Log(gLogData, eSubSystemSIM800L, eInfoLogging, "QueueSimUsart created");
	} else {
		Log(gLogData, eSubSystemSIM800L, eFatalErrorLogging, "QueueSimUsart init error");
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

/**
	Delay functions
		void ModuleGSMDelayCancel(int msDelay);
		void ModuleGSMDelaySetMs(void);
		void ModuleGSMDelayDecrementMs(void);
		Response ModuleGSMDelayCheckMs(void);
*/

void ModuleGSMDelayCancel(int msDelay)
{
	ModuleGSMDelayCounter = 0;
}

void ModuleGSMDelaySetMs(int msDelay)
{
	ModuleGSMDelayCounter = msDelay;
}

void ModuleGSMDelayDecrementMs(void)
{
	if (ModuleGSMDelayCounter != 0x00)
  { 
    ModuleGSMDelayCounter--;
  }
}

Response ModuleGSMDelayCheckMs(void)
{
	if(ModuleGSMDelayCounter == 0)
	{
		return RESP_OK;
	}
	
	return RESP_WAIT;
}
