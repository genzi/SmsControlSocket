/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTONS_MNGR_H
#define __BUTTONS_MNGR_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif
	 
typedef enum {USER_BTN} Button;
	 
void ButtonsMngrInit(volatile uint32_t *SysTimestamp);
	 
#ifdef __cplusplus
}
#endif

#endif /* __BUTTONS_MNGR_H */
