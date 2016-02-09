#include "nv_config.h"
#include <string.h>

const Config *gNVConfig = (Config *)CONFIG_ADDR;
static const Config gFactoryConfig = {"1111", -6.4};
const Config *pFactoryConfig = &gFactoryConfig;

static char __attribute__ ((aligned (4))) buffer[sizeof(Config)];

void NVConfigSave(const Config *config) {
	int i;
	uint32_t *bufuint32;
	volatile FLASH_Status FLASH_Status_it = FLASH_COMPLETE;
	
//	memcpy(buffer, (char *)CONFIG_ADDR, sizeof(Config));
	FLASH_SetLatency(FLASH_Latency_1);

	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR | FLASH_FLAG_BSY);	
	FLASH_Status_it = FLASH_ErasePage(CONFIG_ADDR);
	memcpy(buffer, (char *)config, sizeof(Config));
	for(i = 0; i < sizeof(Config); i += 4)
	{
		bufuint32 = (uint32_t *)&buffer[i];
		FLASH_Status_it = FLASH_ProgramWord(CONFIG_ADDR+i, *bufuint32);
	}
	FLASH_Lock();
}

bool NVConfigIsInitialised(void) {
	char tabFF[sizeof(Config)];
	
	memset(tabFF, 0xFF, sizeof(Config));
	
	return (memcmp(gNVConfig, tabFF, sizeof(Config)) == 0) ? false : true;
}
