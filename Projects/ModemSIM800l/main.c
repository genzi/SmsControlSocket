/**
  ******************************************************************************
  * @file    SysTick/SysTick_Example/main.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    24-July-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sim800l.h"
#include "log\logging.h"
#include "timers_mngr\timers_mngr.h"
#include "temp_sensor\temp_sensor.h"
#include "nv_config\nv_config.h"
#include "buttons_mngr\buttons_mngr.h"
#include <string.h>

/** @addtogroup STM32F0xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup SysTick_Example
  * @{
  */ 
	
/* Public variables ----------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern __IO uint8_t TxBuffer[];
extern __IO uint8_t RxBuffer[];
extern __IO uint8_t NbrOfDataToTransfer;
extern __IO uint8_t TxCount; 
extern __IO uint16_t RxCount;
extern __IO uint32_t SysTickCounter;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
static __IO uint32_t TimingDelay;
static unsigned short int transmitFlag = 0;
float gTemperature;
__IO uint32_t LsiFreq = 40000;

/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nTime);
void LEDs_Init(void);
static void NVIC_Config(void);
static void USART_Config(void);

void USART_Send(USART_TypeDef* USARTx, uint8_t size);

/* Private functions ---------------------------------------------------------*/

void WatchDogInit(void) {
	/* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_128);

  /* Set counter reload value to obtain 250ms IWDG TimeOut.
     Counter Reload Value = 250ms/IWDG counter clock period
                          = 250ms / (LSI/32)
                          = 0.25s / (LsiFreq/32)
                          = LsiFreq/(32 * 4)
                          = LsiFreq/128
   */
  IWDG_SetReload(LsiFreq/128);

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}

void WatchDogFeed(void *ptr) {
	IWDG_ReloadCounter();
}

void StatusLEDon(void *par)
{
	BitAction bit;
	
	if(gNVConfig->outputActive) {
		bit = Bit_RESET;
	} else {
		bit = Bit_SET;
	}
	GPIO_WriteBit(GPIOA, GPIO_Pin_3, bit);
	TimersMngrTimerStart(1);
}

void StatusLEDoff(void *par)
{
	BitAction bit;
	
	if(gNVConfig->outputActive) {
		bit = Bit_SET;
	} else {
		bit = Bit_RESET;
	}	
	GPIO_WriteBit(GPIOA, GPIO_Pin_3, bit);
}

void ReadTemperature(void *par)
{
	gTemperature = TemperatureSensorGetValueMiliCelsius() / 1000.0;
	(void)gTemperature;
}


/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{	
	struct sFirmwareVersion version;
	struct Timer timerInit;
		
	gLogData = LogInit(&SysTickCounter);
	
	version.majorVerion = 0;
	version.minorVersion = 1;
	version.buildIndicator = 69;
	
	LogSetOutputLevel(gLogData, eSubSystemSYSTEM, eInfoLogging);
	LogSetOutputLevel(gLogData, eSubSystemSIM800L, eInfoLogging);
	LogVersion(gLogData, &version);
	
	TimersMngrInit(4);
	
	timerInit.callback = StatusLEDon;
	timerInit.reload = timerInit.counter = 500;
	timerInit.repeated = true;
	TimersMngrConfigTimer(0, timerInit);
	
	timerInit.callback = StatusLEDoff;
	timerInit.reload = timerInit.counter = 100;
	timerInit.repeated = false;
	TimersMngrConfigTimer(1, timerInit);
	
	timerInit.callback = ReadTemperature;
	timerInit.reload = timerInit.counter = 5000;
	timerInit.repeated = true;
	TimersMngrConfigTimer(2, timerInit);
	
	timerInit.callback = WatchDogFeed;
	timerInit.reload = timerInit.counter = 100;
	timerInit.repeated = true;
	TimersMngrConfigTimer(3, timerInit);
  
	if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */
		Log(gLogData, eSubSystemSYSTEM, eFatalErrorLogging, "SysTick Config ERROR");
    while (1);
  }
	
	Log(gLogData, eSubSystemSYSTEM, eInfoLogging, "SysTick initialized");
	
	LEDs_Init();
	NVIC_Config();
	USART_Config();
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_3, (BitAction)0);
	
	ModuleGSMInit();
	
	Log(gLogData, eSubSystemSYSTEM, eInfoLogging, "Hardware initialized");
	
	TimersMngrTimerStart(0);	//status led
	TimersMngrTimerStart(2);	//read temperature
	TimersMngrTimerStart(3);	//watch dog feed
	
	if(NVConfigIsInitialised() == false) {
		NVConfigSave(pFactoryConfig);
	}
	
	if(gNVConfig->outputActive == true) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)1);
	} else {
		GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)0);
	}
	
	TemperatureSensorInit(gNVConfig->temperatureCorrection);
	ButtonsMngrInit(&SysTickCounter);
	
	WatchDogInit();
	
  while (1)
  {
		ModuleGSMMainProcess();
		TimersMngrProcess();
    
		if(transmitFlag)
		{
			Delay(100);
			transmitFlag = 0;
			strcat((char *)TxBuffer, "\r\n");
			USART_Send(USART1, strlen((char *)TxBuffer));
			
			Log(gLogData, eSubSystemSIM800L, eInfoLogging, "Command sended");
		}
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

/**
  * @brief  Initialise Leds on the board.
  * @param  None
  * @retval None
  */
void LEDs_Init(void)
{
  /* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* Configure PC10 and PC11 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
static void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USART Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief Configure the USART Device
  * @param  None
  * @retval None
  */
static void USART_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  /* USART1 Pins configuration ************************************************/
  /* Connect pin to Periph */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1); 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);    
  
  /* Configure pins as AF pushpull */
																	// TX        RX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
    
/* USARTx configured as follow:
  - BaudRate = 9600 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

/**
  * @brief  Send data to USART.
  * @param  None
  * @retval None
  */
void USART_Send(USART_TypeDef* USARTx, uint8_t size)
{
		TxCount = 0;
		NbrOfDataToTransfer = size;
	  USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
