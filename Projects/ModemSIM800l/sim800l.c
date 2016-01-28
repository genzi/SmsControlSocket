#include "sim800l.h"
#include "log\\logging.h"


sim800l moduleGSM;

static Response response; 
static volatile int ModuleGSMDelayCounter;

static void ModuleGSMSetDelayToNextState(int msDelay, State nextSate)
{
	ModuleGSMDelaySetMs(msDelay);
	moduleGSM.nextState = nextSate;
	moduleGSM.currentState = DELAY;
	Log(gLogData, eSubSystemSIM800L, eInfoLogging, "goes to DELAY");
}

static void ModuleGSMWaitForResponse(int msDelay, State nextSate)
{
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
			/*TODO
			-	dodac stan WAIT_FOR_RESPONSE: przed przejsciem do tego stanu ustawic flage czekania na odpowiedz i timeout
				inny dla kazdego zapytania. Jesli dostaniemy na uarcie prawidlowa odpiwiedz (badanie \r\n albo OK\r\n) to 
				zmieniamy flage i w WAIT_FOR_RESPONSE decydujemy co zrobic dalej (moze skoczyc do innego stanu lub od razu jakas akcja)
			- do wysylania i zapisywania odpowiedzi uzyc kolejki
			*/
			ModuleGSMWaitForResponse(50, AT_RESPONSE);
			SendCommand("AT\r\n");
			
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "goes to READY");
		break;
		
		case AT_RESPONSE:
			
		break;
			
		case READY:
			
		break;
		
		default:
			LogWithNum(gLogData, eSubSystemSIM800L, eErrorLogging, "UNDEFINED state:%d", moduleGSM.currentState);
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
