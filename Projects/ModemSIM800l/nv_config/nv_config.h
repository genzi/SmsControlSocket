/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NV_CONFIG_H
#define __NV_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_flash.h"
#include <stdbool.h>
	 
#define CONFIG_ADDR 0x0800FC00

typedef struct {
	char pin[5];
	float temperatureCorrection;
	bool sendAnswer;
	bool outputActive;
} Config;

extern const Config *gNVConfig;
//extern const Config gFactoryConfig;
extern const Config *pFactoryConfig;

void NVConfigSave(const Config *config);
bool NVConfigIsInitialised(void);

#ifdef __cplusplus
}
#endif

#endif /* __NV_CONFIG_H */
