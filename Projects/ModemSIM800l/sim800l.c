#include "sim800l.h"
#include "log\\logging.h"


sim800l moduleGSM;

static Response response; //powinno byc typu enum OK, ERROR, WAIT
static volatile int ModuleGSMDelayCounter;

static void ModuleGSMSetWaitForNextState(int msDelay, State nextSate)
{
	ModuleGSMDelaySetMs(msDelay);
	moduleGSM.nextState = nextSate;
	moduleGSM.currentState = WAIT;
	Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Sim800l goes to WAIT");
}

void ModuleGSMProcess(void)
{
	switch(moduleGSM.currentState)
	{
		case WAIT:
			if(ModuleGSMDelayCheckMs() != RESP_WAIT)
			{
				moduleGSM.currentState = moduleGSM.nextState;
				LogWithNum(gLogData, eSubSystemSIM800L, eInfoLogging, "Sim800l goes to State: %d", moduleGSM.nextState);
			}
		break;
		
		case RESETING:
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Sim800l RESETING");
			ModuleGSMReset();
			ModuleGSMSetWaitForNextState(500, STARTING);
		break;
		
		case STARTING:
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Sim800l STARTING");
			ModuleGSMEnable();
			ModuleGSMSetWaitForNextState(3000, CHECK_ALIVE);
		break;
			
		case CHECK_ALIVE:
			response = SendCommand("AT\r\n");
		
			if(response == RESP_OK)
			{
				moduleGSM.currentState = READY;
			}
			else if(response == RESP_ERROR)
			{
				moduleGSM.currentState = READY;
			}
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Sim800l goes to READY");
		break;
			
		case READY:
			
		break;
		
		default:
			break;
	}
}

void ModuleGSMInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
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

Response SendCommand(char *command)
{
	strcpy((char *)TxBuffer, command);
	USART_Send(USART1, strlen((char *)TxBuffer));	
	return RESP_OK;
}

/**
	Delay functions
		void ModuleGSMDelaySetMs(void);
		void ModuleGSMDelayDecrementMs(void);
		Response ModuleGSMDelayCheckMs(void);
*/

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
