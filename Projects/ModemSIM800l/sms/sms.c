#include "sms.h"
#include <stdlib.h>
#include <string.h>

SMS *SMSCreate(void) {
	SMS *sms = calloc(1, sizeof(SMS));
	memset(sms->telNumber, 0, 13);
	memset(sms->message, 0, 161);
	
  return sms;
}

void SMSDestroy(void **sms) {
        if (*sms != NULL) {
                free(*sms);
                *sms = NULL;
        }
}

bool SMSParse(SMS *sms, char *buffer) {
	char *pCMGR;
	char *pStrTok;
	
	if((pCMGR = strstr(buffer, "+CMGR:")) != NULL) {
		if(strstr(buffer, "OK\r\n")) {
			pStrTok = strtok(pCMGR, ",");
			pStrTok = strtok(NULL, ",");
			memcpy(sms->telNumber, pStrTok+1, 12);
			sms->telNumber[12] = 0;
			pStrTok = strtok(NULL, "\r\n");
			pStrTok = strtok(NULL, "\r\n");
			strncpy(sms->message, pStrTok, 160);
			return true;
		}
	}
	
	return false;
}
