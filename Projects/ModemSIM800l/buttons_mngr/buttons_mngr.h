/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTONS_MNGR_H
#define __BUTTONS_MNGR_H

#include <stdint.h>
#include "stm32f0xx.h"

#ifdef __cplusplus
 extern "C" {
#endif
	 
#define LONG_PRESS 1000
	 
typedef enum {USER_BTN} Button;
	 
extern __IO uint32_t *ButtonsMngrSysTimestamp;
	 
void ButtonsMngrInit(volatile uint32_t *SysTimestamp);

void ButtonShortPressedCallback(Button button);
void ButtonLongPressedCallback(Button button);
	
#ifdef __cplusplus
}
#endif

#endif /* __BUTTONS_MNGR_H */
