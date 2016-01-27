#include "logging.h"
#include <stdlib.h>
#include <stdio.h>

char LogBuffer[NUMBER_OF_LOGS][LOG_STRING_LENGTH];
struct sLogStruct *gLogData;

struct sLogStruct* LogInit(volatile uint32_t *SysTimestamp){
	int i;
	
	if(gLogData != NULL) {
		return gLogData;
	}
	struct sLogStruct *logData = (struct sLogStruct *)malloc(sizeof(struct sLogStruct));
	logData->pTimestamp = SysTimestamp;
	logData->logOn = TRUE;
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

void LogSetOutputLevel(struct sLogStruct* logData, enum eLogSubSystem sys, enum eLogLevel level) {
	logData->outputLevel[sys] = level;
}

void Log(struct sLogStruct* logData, enum eLogSubSystem sys, enum eLogLevel level, char *msg) {
	static int index = 0;
	uint32_t timestamp;
	
	if(logData->logOn == FALSE) {
		return;
	}
	
	timestamp = logData->pTimestamp == NULL ? 0 : *logData->pTimestamp;
	if(level >= logData->outputLevel[sys]) {
		snprintf(&LogBuffer[index++%NUMBER_OF_LOGS][0], LOG_STRING_LENGTH, "{%Xh}[%d]<%d>%s", timestamp, sys, level, msg);
	}
}

void LogWithNum(struct sLogStruct* logData, enum eLogSubSystem sys, enum eLogLevel level, char *msg, int num) {
	char tempString[LOG_STRING_LENGTH];
	
	snprintf(tempString, LOG_STRING_LENGTH, msg, num);
	Log(logData, sys, level, tempString);	
}

void LogVersion(struct sLogStruct* logData, struct sFirmwareVersion *v) {
	char version[LOG_STRING_LENGTH];
	
	snprintf(version, LOG_STRING_LENGTH, "SoftVer%d.%d.%d", v->majorVerion, v->minorVersion, v->buildIndicator);
	Log(logData, eSubSystemSYSTEM, eInfoLogging, version);
}

