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

enum eLogLevel {eInfoLogging, eWarningLogging, eErrorLogging, eFatalErrorLogging, eNoLogging};

#define NUM_LOG_SUBSYSTEMS 3
enum eLogSubSystem {eSubSystemSYSTEM, eSubSystemSIM800L, eSubSystemGPIO};

struct sLogStruct{
tBoolean logOn;
enum eLogLevel outputLevel[NUM_LOG_SUBSYSTEMS];
};

static struct sLogStruct gLogData;
	 
struct sLogStruct* LogInit(void);
void Log(struct sLogStruct* logData, enum eLogSubSystem sys, enum eLogLevel level, char *msg);
void LogSetOutputLevel(struct sLogStruct* logData, enum eLogSubSystem sys, enum eLogLevel level);
void LogGlobalOn(struct sLogStruct* logData);
void LogGlobalOff(struct sLogStruct* logData);
void LogVersion(struct sLogStruct* logData, struct sFirmwareVersion *v);

	 
#ifdef __cplusplus
}
#endif

#endif /* __LOGGING_H */
