/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMERS_MNGR_H
#define __TIMERS_MNGR_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif
	 
#define STOPPED 0xFFFFFFFF
	 
typedef enum {
	FALSE,
	TRUE
} tBoolean;
	 
struct Timer {
	tBoolean repeated;
	uint32_t counter;
	uint32_t value;
	void (*callback)(void *);
};

void TimersMngrInit(uint8_t numberOfTimers);
void TimerMngrConfigTimer(uint8_t number, struct Timer timerInitStructure);
void TimersMngrProcess(void);

	 
#ifdef __cplusplus
}
#endif

#endif /* __TIMERS_MNGR_H */
