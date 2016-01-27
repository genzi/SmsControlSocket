/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LOGGING_H
#define __LOGGING_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif
	 
#define LOG_STRING_LENGTH 40
#define NUMBER_OF_LOGS 10
	 
struct sFirmwareVersion{
	uint8_t majorVerion;
	uint8_t minorVersion;
	uint16_t buildIndicator;
};

enum eLogLevel {eInfoLogging, eWarningLogging, eErrorLogging, eFatalErrorLogging, eNoLogging};

#define NUM_LOG_SUBSYSTEMS 3
enum eLogSubSystem {eSubSystemSYSTEM, eSubSystemSIM800L, eSubSystemGPIO};

struct sLogStruct{
bool logOn;
enum eLogLevel outputLevel[NUM_LOG_SUBSYSTEMS];
volatile uint32_t *pTimestamp;
};

extern struct sLogStruct *gLogData;
	 
struct sLogStruct* LogInit(volatile uint32_t *SysTimestamp);
void Log(struct sLogStruct* logData, enum eLogSubSystem sys, enum eLogLevel level, char *msg);
void LogWithNum(struct sLogStruct* logData, enum eLogSubSystem sys, enum eLogLevel level, char *msg, int num);
void LogSetOutputLevel(struct sLogStruct* logData, enum eLogSubSystem sys, enum eLogLevel level);
void LogGlobalOn(struct sLogStruct* logData);
void LogGlobalOff(struct sLogStruct* logData);
void LogVersion(struct sLogStruct* logData, struct sFirmwareVersion *v);
	 
#ifdef __cplusplus
}
#endif

#endif /* __LOGGING_H */
