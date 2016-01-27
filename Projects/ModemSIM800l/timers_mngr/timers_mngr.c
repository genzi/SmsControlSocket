#include "timers_mngr.h"
#include <stdlib.h>
#include <string.h>

static struct Timer *TimersList;
static uint8_t gNumberOfTimers;

void TimersMngrInit(uint8_t numberOfTimers) {
	uint8_t i;
	gNumberOfTimers = numberOfTimers;
	TimersList = malloc((sizeof(struct Timer)) * gNumberOfTimers);
	for(i = 0; i < gNumberOfTimers; i++)
	{
		TimersList[i].repeated = FALSE;
		TimersList[i].counter = STOPPED;
		TimersList[i].callback = NULL;
	}
}

void TimerMngrConfigTimer(uint8_t number, struct Timer timerInitStructure) {
	TimersList[number].repeated = timerInitStructure.repeated;
	TimersList[number].counter = STOPPED;
	TimersList[number].value = timerInitStructure.value;
	TimersList[number].callback = timerInitStructure.callback;
}

void TimersMngrProcess(void) {
	uint8_t timerIndex;
	
	for(timerIndex = 0; timerIndex < gNumberOfTimers; timerIndex++)
	{

	}	
	
}
