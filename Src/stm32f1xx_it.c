/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"

/* USER CODE BEGIN 0 */
#include <string.h>
#include "et_stm32f_arm_kit_lcd.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;
extern RTC_TimeTypeDef alarm[8],wakeTime,sleepTime;
extern RTC_HandleTypeDef hrtc;

extern uint32_t kg,age;
extern char name[100];
extern uint8_t cmd[2],mode,open;
extern float closePWM,openPWM;
extern char recieve[100];
extern void openValve(void);
extern void closeValve(void);
extern uint8_t* bmp;
extern long bmpSize;
extern void ShowPIXEL(uint16_t);
extern void ShowBMP(uint16_t*);
extern int inStartMenu,day,pass,isOpen;
extern int statistic[7][8];
extern void setTreeStat(int treeNo);
extern void calculate();
extern char disp[21];
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart1;

/******************************************************************************/
/*            Cortex-M3 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles EXTI line0 interrupt.
*/
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */
	//if(!isOpen) {
		openValve();
	//} else
	//	closeValve();
  /* USER CODE END EXTI0_IRQn 1 */
}

/**
* @brief This function handles DMA1 channel5 global interrupt.
*/
void DMA1_Channel5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */

  /* USER CODE END DMA1_Channel5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA1_Channel5_IRQn 1 */
	//LCD_DisplayStringLine(Line4, (uint8_t *) recieve);
	
	if(recieve[0] == 'O') {
		openValve();
	}
	else if(recieve[0] == 'C') {
		closeValve();
	}
	else if(recieve[0] == 'M') {
		//int cm = -1;
		//memmove(recieve, recieve+1, strlen(recieve));
		//sscanf(recieve,"%d", &cm);
		//sprintf(disp,"CM:%02d",cm);
		//LCD_ClearLine(Line2);
		//LCD_DisplayStringLine(Line2, (uint8_t *) disp);
	}
	else if(recieve[0] == 'V') {
		int left = recieve[1]-'0';
		int tpass = recieve[2]-'0';
		statistic[(tpass==7)? day-1:day][tpass] = left;
		if(!inStartMenu) {
			sprintf(disp,"WATER DRANK: %d%% ",statistic[(tpass==7)? day-1:day][tpass]*10);
			LCD_SetTextColor(Black); 
			LCD_DisplayStringLine(Line4, (uint8_t *) disp);
		}
		
		setTreeStat(day);
	}
	else if(recieve[0] == 'N') {
			memmove(recieve, recieve+1, strlen(recieve));
			strcpy(name,recieve);
			sprintf(disp,"NAME:%s",name);
		
			if(!inStartMenu) {
				LCD_ClearLine(Line0);
				LCD_DisplayStringLine(Line0, (uint8_t *) disp);
			} else {
				LCD_ClearLine(Line2);
				LCD_SetTextColor(White); 
				LCD_DisplayStringLine(Line2, (uint8_t *) disp);
			}
			
	}
	else if(recieve[0] == 'K') {
			memmove(recieve, recieve+1, strlen(recieve));
			sscanf(recieve,"%d", &kg);
			
			if(!inStartMenu) {
				sprintf(disp,"AGE:%02d KG:%02d",age,kg);
				LCD_ClearLine(Line1);
				LCD_DisplayStringLine(Line1, (uint8_t *) disp);
			} else {
				LCD_DisplayChar(Line4,320-180,kg%10+'0');
				LCD_DisplayChar(Line4,320-165,kg/10+'0');
			}
			sprintf (disp, "%c", 'a'+kg);
			HAL_UART_Transmit_IT(&huart1, (uint8_t *) disp, 1);
	}
	else if(recieve[0] == 'A') {
			memmove(recieve, recieve+1, strlen(recieve));
			sscanf(recieve,"%d", &age);
		
		if(!inStartMenu) {
			sprintf(disp,"AGE:%02d KG:%02d",age,kg);
			LCD_ClearLine(Line1);
			LCD_DisplayStringLine(Line1, (uint8_t *) disp);
		} else {
			LCD_DisplayChar(Line3,320-130,age%10+'0');
			LCD_DisplayChar(Line3,320-115,age/10+'0');
		}
			
	}
	else if(recieve[0] == 'T') {
			memmove(recieve, recieve+1, strlen(recieve));
		
			int h=0,m=0;
		
			sscanf(recieve,"%d:%d", &h,&m);
			sTime.Hours = (uint8_t)h;
			sTime.Minutes = (uint8_t)m;
			sTime.Seconds = 0;
			
			HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
			sprintf (disp, "Time-> [%02d:%02d]", sTime.Hours,sTime.Minutes);
			
		
			if(!inStartMenu) {
				
			} else {
				LCD_DisplayChar(Line7,320-130,sTime.Hours%10+'0');
				LCD_DisplayChar(Line7,320-115,sTime.Hours/10+'0');
				LCD_DisplayChar(Line7,320-240,sTime.Minutes%10+'0');
				LCD_DisplayChar(Line7,320-225,sTime.Minutes/10+'0');
			}
			calculate();
	}
	else if(recieve[0] == 'W') {
			memmove(recieve, recieve+1, strlen(recieve));
		
			int h=0,m=0;
			sscanf(recieve,"%d:%d", &h,&m);
			wakeTime.Hours = (uint8_t)h;
			wakeTime.Minutes = (uint8_t)m;
			wakeTime.Seconds = 0;
		
			sprintf (disp, "Wake Time-> [%02d:%02d]", wakeTime.Hours,wakeTime.Minutes);
			
		
			if(!inStartMenu) {

			} else {
				LCD_DisplayChar(Line5,320-130,wakeTime.Hours%10+'0');
				LCD_DisplayChar(Line5,320-115,wakeTime.Hours/10+'0');
				LCD_DisplayChar(Line5,320-240,wakeTime.Minutes%10+'0');
				LCD_DisplayChar(Line5,320-225,wakeTime.Minutes/10+'0');
			}
			calculate();
	}
	else if(recieve[0] == 'Z') {
			memmove(recieve, recieve+1, strlen(recieve));
		
			int h=0,m=0;
			sscanf(recieve,"%d:%d", &h,&m);
			sleepTime.Hours = (uint8_t)h;
			sleepTime.Minutes = (uint8_t)m;
			sleepTime.Seconds = 0;
		
			sprintf (disp, "Sleep Time-> [%02d:%02d]", sleepTime.Hours,sleepTime.Minutes);
			
		
			if(!inStartMenu) {

			} else {
				LCD_DisplayChar(Line6,320-140,sleepTime.Hours%10+'0');
				LCD_DisplayChar(Line6,320-125,sleepTime.Hours/10+'0');
				LCD_DisplayChar(Line6,320-255,sleepTime.Minutes%10+'0');
				LCD_DisplayChar(Line6,320-240,sleepTime.Minutes/10+'0');
			}
			calculate();
	}
	else if(recieve[0] == 'D') {
			memmove(recieve, recieve+1, strlen(recieve));
		
			int d=0,m=0;
		
			sscanf(recieve,"%d/%d", &d,&m);
			sDate.Date = (uint8_t)d;
			sDate.Month = (uint8_t)m;
			
			HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
			sprintf (disp, "Date-> [%02d/%02d]", sDate.Date,sDate.Month);
			
	}
  /* USER CODE END DMA1_Channel5_IRQn 1 */
}

/**
* @brief This function handles USART1 global interrupt.
*/
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
* @brief This function handles EXTI line[15:10] interrupts.
*/
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */
	if(!isOpen) {
		openValve();
	} else
		closeValve();
  /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
