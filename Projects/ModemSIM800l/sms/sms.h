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

typedef struct {
	char *pin;
	char *action;
	char *variable;
	char *value;
} MsgContent;
	 
SMS *SMSCreate(void);
void SMSDestroy(SMS **sms);
bool SMSParse(SMS *sms, char *buffer);

void SMSMessageParse(char *msg, MsgContent *msgContent);


#ifdef __cplusplus
}
#endif

#endif /* __SMS_H */
