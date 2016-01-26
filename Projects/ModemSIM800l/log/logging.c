#include "logging.h"
#include <stdlib.h>
#include <stdio.h>

char LogBuffer[10][30];

struct sLogStruct* LogInit(){
	int i;
	struct sLogStruct *logData = malloc(sizeof(*logData));
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
	if(level >= logData->outputLevel[sys]) {
		snprintf(&LogBuffer[index++%10][0], 30, "[%d]<%d>%s", sys, level, msg);
	}
}

void LogVersion(struct sLogStruct* logData, struct sFirmwareVersion *v) {
	char version[20];
	
	snprintf(version, 20, "SoftVer%d.%d.%d", v->majorVerion, v->minorVersion, v->buildIndicator);
	Log(logData, eSubSystemSYSTEM, eInfoLogging, version);
}
