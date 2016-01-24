#include "sim800l.h"

static int response; //powinno byc typu enum OK, ERROR, WAIT

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

int SendCommand(char *command)
{
	
}
