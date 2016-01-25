#include "sim800l.h"

static Response response; //powinno byc typu enum OK, ERROR, WAIT

void ModuleGSMProcess(void)
{
	switch(moduleGSM.currentState)
	{
		case RESETING:
			ModuleGSMReset();
		#if 0
			if(ModuleGSMDelay)
				moduleGSM.currentState = STARTING;
		#endif
		break;
			
		case STARTING:
			ModuleGSMEnable();
		#if 0
			if(ModuleGSMDelay)
				moduleGSM.currentState = CHECK_ALIVE;
		#endif
		break;
			
		case CHECK_ALIVE:
			response = SendCommand("AT\r\n");
		
			if(response == RESP_OK)
			{
				
			}
			else if(response == RESP_ERROR)
			{
				
			}
		break;
		
		default:
			break;
	}
}

void ModuleGSMInit(void)
{
	moduleGSM.currentState = RESETING;
	
}

void ModuleGSMReset(void)
{
	
}

void ModuleGSMEnable(void)
{
	
}

Response SendCommand(char *command)
{
	
	return RESP_ERROR;
}
