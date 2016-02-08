/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TEMP_SENSOR_H
#define __TEMP_SENSOR_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif
	 
void TemperatureSensorInit(float);
int32_t TemperatureSensorGetValueMiliCelsius(void);
void TemperatureSensorSetCorrection(float _correction);
	 
#ifdef __cplusplus
}
#endif

#endif /* __TEMP_SENSOR_H */
