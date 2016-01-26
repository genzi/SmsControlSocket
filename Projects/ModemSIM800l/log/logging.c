#include "logging.h"
#include <stdlib.h>

struct sLogStruct* LogInit(){
	int i;
	struct sLogStruct *logData = malloc(sizeof(*logData));
	logData->logOn = FALSE;
	for (i=0; i < NUM_LOG_SUBSYSTEMS; i++) {
		logData->outputLevel[i] = eNoLogging;
	}
	return logData;
}

void LogGlobalOn(struct sLogStruct* logData) {
	logData->logOn = TRUE;
}
void LogGlobalOff(struct sLogStruct* logData) {
	logData->logOn = FALSE;
}
