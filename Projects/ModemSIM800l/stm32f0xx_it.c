/**
  ******************************************************************************
  * @file    SysTick/SysTick_Example/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    24-July-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "stm32f0xx_it.h"
#include "sim800l.h"
#include "timers_mngr\timers_mngr.h"
#include "log\logging.h"
#include "buttons_mngr\buttons_mngr.h"
#include <string.h>

/** @addtogroup STM32F0xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup SysTick_Example
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TXBUFFERSIZE 255
#define RXBUFFERSIZE 511
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t SysTickCounter;
__IO uint8_t TxBuffer[TXBUFFERSIZE];
__IO uint8_t RxBuffer[RXBUFFERSIZE];
__IO uint8_t NbrOfDataToTransfer = 0;
__IO uint8_t TxCount = 0; 
__IO uint16_t RxCount = 0;
__IO bool newDataUSART1Flag;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
	Log(gLogData, eSubSystemSYSTEM, eFatalErrorLogging, "HardFault");
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  SysTickCounter++;
	
	TimingDelay_Decrement();
	ModuleGSMDelayDecrementMs();
	TimersMngrDecrementCounters();
}

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */

void USART1_IRQHandler(void)
{
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    RxBuffer[RxCount++] = (USART_ReceiveData(USART1) & 0x7F);
				
		if(RxCount == RXBUFFERSIZE)
		{
			RxCount = 0;
		}
		
		newDataUSART1Flag = true;
  }

  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {   
    /* Write one byte to the transmit data register */
    USART_SendData(USART1, TxBuffer[TxCount++]);

    if(TxCount == NbrOfDataToTransfer)
    {
      /* Disable the USART1 Transmit interrupt */
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			memset((void *)TxBuffer, 0, TXBUFFERSIZE);
    }
  }
	
	if(USART_GetITStatus(USART1, USART_IT_PE) != RESET) {   
		USART_ClearITPendingBit(USART1, USART_IT_PE);
	}
	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET) {   
		USART_ClearITPendingBit(USART1, USART_IT_TC);
	}
	if(USART_GetITStatus(USART1, USART_IT_LBD) != RESET) {   
		USART_ClearITPendingBit(USART1, USART_IT_LBD);
	}
	if(USART_GetITStatus(USART1, USART_IT_ERR) != RESET) {   
		USART_ClearITPendingBit(USART1, USART_IT_ERR);
	}
	if(USART_GetITStatus(USART1, USART_IT_ORE) != RESET) {   
		USART_ClearITPendingBit(USART1, USART_IT_ORE);
	}
	if(USART_GetITStatus(USART1, USART_IT_NE) != RESET) {   
		USART_ClearITPendingBit(USART1, USART_IT_NE);
	}
	if(USART_GetITStatus(USART1, USART_IT_FE) != RESET) {   
		USART_ClearITPendingBit(USART1, USART_IT_FE);
	}
	
	if(USART_GetFlagStatus(USART1, USART_FLAG_PE) != RESET) {
		USART_ClearFlag(USART1, USART_FLAG_PE);
	}	
	if(USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET) {
		USART_ClearFlag(USART1, USART_FLAG_TC);
	}
	if(USART_GetFlagStatus(USART1, USART_FLAG_LBD) != RESET) {
		USART_ClearFlag(USART1, USART_FLAG_LBD);
	}
	if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET) {
		USART_ClearFlag(USART1, USART_FLAG_ORE);
	}
	if(USART_GetFlagStatus(USART1, USART_FLAG_NE) != RESET) {
		USART_ClearFlag(USART1, USART_FLAG_NE);
	}
	if(USART_GetFlagStatus(USART1, USART_FLAG_FE) != RESET) {
		USART_ClearFlag(USART1, USART_FLAG_FE);
	}

}

/**
  * @brief  This function handles External lines 4 to 15 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
	static uint32_t pressedTimestamp;
	
	if(EXTI_GetITStatus(EXTI_Line7) != RESET)
  {
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == RESET) {
			pressedTimestamp = *ButtonsMngrSysTimestamp;
		} else {
			if((*ButtonsMngrSysTimestamp - pressedTimestamp) >= LONG_PRESS) {
				ButtonLongPressedCallback(USER_BTN);
			} else {
				ButtonShortPressedCallback(USER_BTN);
			}
		}
    
    /* Clear the EXTI line 7 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line7);
  }
   
}


/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
