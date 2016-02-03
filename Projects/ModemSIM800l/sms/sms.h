/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SMS_H
#define __SMS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
	 
typedef struct {
	char telNumber[13];
	char message[161];
} SMS;
	 
SMS *SMSCreate(void);
void SMSDestroy(void **sms);
bool SMSParse(SMS *sms, char *buffer);


#ifdef __cplusplus
}
#endif

#endif /* __SMS_H */
