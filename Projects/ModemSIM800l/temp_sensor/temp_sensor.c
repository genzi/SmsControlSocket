#include "temp_sensor.h"
#include "stm32f0xx.h"


#define TS_CAL1 (*(unsigned short *)0x1FFFF7B8)
#define TS_CAL2 (*(unsigned short *)0x1FFFF7C2)
#define VREFINT_CAL (*(unsigned short *)0x1FFFF7BA)
	

static __IO uint16_t RegularConvData_Tab[2];
static int32_t correction;


void TemperatureSensorSetCorrection(float _correction) {
	correction = _correction * 1000;
}

void TemperatureSensorInit(float _correction) {
	int i;
	correction = _correction * 1000;
////////////////////////////////ADC Config//////////////////////////////////
  ADC_InitTypeDef     ADC_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;
  /* ADC1 DeInit */  
  ADC_DeInit(ADC1);
	
	  /* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  
   /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
   
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continuous mode withe a resolution equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
  ADC_Init(ADC1, &ADC_InitStructure); 
	
//	ADC_ChannelConfig(ADC1, ADC_Channel_10 , ADC_SampleTime_55_5Cycles); 
  
  /* Convert the ADC1 temperature sensor  with 55.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor , ADC_SampleTime_55_5Cycles);  
  ADC_TempSensorCmd(ENABLE);
  
  /* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint , ADC_SampleTime_55_5Cycles); 
  ADC_VrefintCmd(ENABLE);
  
//  /* Convert the ADC1 Vbat with 55.5 Cycles as sampling time */ 
//  ADC_ChannelConfig(ADC1, ADC_Channel_Vbat , ADC_SampleTime_55_5Cycles);  
//  ADC_VbatCmd(ENABLE);
  
  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
  
  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
  
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
  
  /* Enable the ADC peripheral */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADRDY flag */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
////////////////////////////////////////////////////////////////////////////
	
////////////////////////////////DMA Config//////////////////////////////////
  DMA_InitTypeDef   DMA_InitStructure;
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
  
  /* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(uint32_t)&ADC1->DR;;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RegularConvData_Tab;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 2;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
////////////////////////////////////////////////////////////////////////////
	for(i = 0; i < 10000; i++) {
		TemperatureSensorGetValueMiliCelsius();
	}
}

int32_t TemperatureSensorGetValueMiliCelsius(void) {
	int32_t temperature;	
	static int32_t averageTemperature;
	
	temperature = ((RegularConvData_Tab[1] * (VREFINT_CAL)) / RegularConvData_Tab[0]) - (int32_t)TS_CAL1;
	temperature *= (int32_t)(110000 - 30000);
	temperature = temperature / (int32_t)(TS_CAL2 - TS_CAL1);
	temperature += (30000 +  correction);
	
	averageTemperature = averageTemperature + ((temperature - averageTemperature) >> 5);
	
	return averageTemperature;
}
