/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMERS_MNGR_H
#define __TIMERS_MNGR_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif
	 
#define INTERRUPT 0
#define STOPPED -1

struct Timer {
	bool repeated;
	int32_t counter;
	int32_t reload;
	void (*callback)(void *);
};

void TimersMngrInit(uint8_t numberOfTimers);
void TimersMngrConfigTimer(uint8_t number, struct Timer timerInitStructure);
void TimersMngrProcess(void);
void TimersMngrDecrementCounters(void);
void TimersMngrTimerStart(uint8_t number);
void TimersMngrTimerStop(uint8_t number);
	 
#ifdef __cplusplus
}
#endif

#endif /* __TIMERS_MNGR_H */
