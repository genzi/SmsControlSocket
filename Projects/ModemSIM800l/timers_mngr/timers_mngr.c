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
		TimersList[i].repeated = false;
		TimersList[i].counter = STOPPED;
		TimersList[i].reload = STOPPED;
		TimersList[i].callback = NULL;
	}
}

void TimersMngrConfigTimer(uint8_t number, struct Timer timerInitStructure) {
	TimersList[number].repeated = timerInitStructure.repeated;
	TimersList[number].counter = STOPPED;
	TimersList[number].reload = timerInitStructure.reload;
	TimersList[number].callback = timerInitStructure.callback;
}

void TimersMngrProcess(void) {
	uint8_t timerIndex;
	
	for(timerIndex = 0; timerIndex < gNumberOfTimers; timerIndex++) {
		if(TimersList[timerIndex].counter == INTERRUPT) {
			TimersList[timerIndex].counter = TimersList[timerIndex].repeated ? TimersList[timerIndex].reload : STOPPED;
			if(TimersList[timerIndex].callback != NULL) {
				TimersList[timerIndex].callback(NULL);
			}
		}
	}	
	
}

void TimersMngrDecrementCounters(void) {
	uint8_t timerIndex;
	
	for(timerIndex = 0; timerIndex < gNumberOfTimers; timerIndex++) {
		if(TimersList[timerIndex].counter > 0) {
			TimersList[timerIndex].counter--;
		}
	}
}

void TimersMngrTimerStart(uint8_t number) {
	TimersList[number].counter = TimersList[number].reload;
}

void TimersMngrTimerStop(uint8_t number) {
	TimersList[number].counter = STOPPED;
}

void TimersMngrSetReloadValue(uint8_t number, int32_t reload) {
	TimersList[number].reload = reload;
}
