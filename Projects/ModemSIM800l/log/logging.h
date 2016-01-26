/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LOGGING_H
#define __LOGGING_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif
	 
struct sFirmwareVersion{
	uint8_t majorVerion;
	uint8_t minorVersion;
	uint16_t buildIndicator;
};
typedef enum {
	FALSE,
	TRUE
} tBoolean;

enum eLogLevel {eNoLogging, eWarningLogging, eErrorLogging, eFatalErrorLogging};

#define NUM_LOG_SUBSYSTEMS 2
enum eLogSubSystem {eSubSystemSIM800L, eSubSystemGPIO};

struct sLogStruct{
tBoolean logOn;
enum eLogLevel outputLevel[NUM_LOG_SUBSYSTEMS];
};

static struct sLogStruct gLogData;
	 
struct sLogStruct* LogInit(void);
void Log(struct sLogStruct* logData, enum eLogSubSystem sys, enum eLogLevel level, char *msg);
void LogWithNum(struct sLogStruct* logData, enum eLogSubSystem sys, enum eLogLevel level, char *msg, int number);
void LogSetOutputLevel(struct sLogStruct* logData, enum eLogSubSystem sys, enum eLogLevel level);
void LogGlobalOn(struct sLogStruct* logData);
void LogGlobalOff(struct sLogStruct* logData);
void LogVersion(struct sFirmwareVersion *v);

	 
#ifdef __cplusplus
}
#endif

#endif /* __LOGGING_H */
