/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
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
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include <math.h>
#include "et_stm32f_arm_kit_lcd.h"
#define White 0xFFFF
#define Black 0x0000
#define Grey 0xF7DE
#define Blue 0x001F
#define Blue2 0x051F
#define Red 0xF800
#define Magenta 0xF81F
#define Green 0x07E0
#define Cyan 0x7FFF
#define Yellow 0xFFE0


/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI3_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
int isOpen = 1;
int inStartMenu = 0;

void openValve(void) {
	if(!isOpen) {
		isOpen = 1;
		LCD_SetBackColor(Green);
		LCD_SetTextColor(White);
  	LCD_DisplayStringLine(Line0,"        OPEN        ");
		LCD_SetBackColor(White);
		
		float duty = 0;
		while(duty<1.0) {
			duty += 0.05;
			htim2.Instance -> CCR3 = (1000-1) * duty;
			HAL_Delay(100);
		}
	}
	
}
void closeValve(void) {
	if(isOpen) {
		isOpen = 0;
		LCD_SetBackColor(Blue);
		LCD_SetTextColor(White);
  	LCD_DisplayStringLine(Line0,"     TreeMinder     ");
		LCD_SetBackColor(White);
		
		float duty = 1.0;
		while(duty>0) {
			duty -= 0.05;
			htim2.Instance -> CCR3 = (1000-1) * duty;
			HAL_Delay(100);
		}
	}
}
char* toDateString(uint8_t  date) {
		switch(date) {
			case RTC_WEEKDAY_MONDAY: return "Mon";
			case RTC_WEEKDAY_TUESDAY: return "Tue";
			case RTC_WEEKDAY_WEDNESDAY: return "Wed";
			case RTC_WEEKDAY_THURSDAY: return "Thu";
			case RTC_WEEKDAY_FRIDAY: return "Fri";
			case RTC_WEEKDAY_SATURDAY: return "Sat";
			case RTC_WEEKDAY_SUNDAY: return "Sun";
			default: return "Day";
		}
}
int compareTime(RTC_TimeTypeDef a,RTC_TimeTypeDef b) {
		if(a.Hours==b.Hours && a.Minutes==b.Minutes && a.Seconds>>2==b.Seconds>>2)
			return 1;
		else
			return 0;
}


/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
RTC_HandleTypeDef hrtc;

RTC_TimeTypeDef alarm[8] = {
	{0,0,0},
	{0,0,0},
	{11,30,0},
	{13,00,0},
	{15,00,0},
	{17,00,0},
	{20,30,0},
	{21,30,0}
};
RTC_TimeTypeDef wakeTime = {8,0,0};
RTC_TimeTypeDef sleepTime = {21,0,0};

int statistic[7][8] = {
	{5,9,2,1,5,4,2,0},
	{1,2,1,9,0,0,0,0},
	{5,8,7,5,9,0,0,0},
	{6,4,9,0,0,0,0,0},
	{0,8,5,5,5,9,0,0},
	{3,7,8,9,1,5,4,9},
	{0,0,0,0,0,0,0,0}
};

uint32_t kg=65,age=30;
int pass=-1,day=6;
char name[21]="User";
uint8_t cmd[2],mode=0,open=0;
char recieve[21];
char disp[21],*str,lastSec=0;

//variables need to be declared at the beginning 
/* USER CODE BEGIN 4 */
void calculate() {
	
	
	int sleepHrFixed = (sleepTime.Hours==0)? 24:sleepTime.Hours;
	int gap = (sleepHrFixed*60+sleepTime.Minutes) - (wakeTime.Hours*60+wakeTime.Hours);
	if(gap<0) gap *= -1;
	
	
	for(int i=0;i<8;i++) {
		int minnow = round(i/8.0*gap);
		alarm[i].Hours = (wakeTime.Hours + minnow/60)%24;
		minnow /= 60;
		if(minnow+wakeTime.Minutes>=60) alarm[i].Hours++;
		alarm[i].Minutes = (wakeTime.Minutes + minnow)%60;
	}
	
	while(sTime.Hours>alarm[pass+1].Hours || (sTime.Hours==alarm[pass+1].Hours&&sTime.Minutes>alarm[pass+1].Minutes)) {
		pass++;
		if(pass==8) {
			pass = -1;
			break;
		}
	}

	
}
void startMenu() {
		inStartMenu = 1;
		uint16_t x,y;
	
		LCD_Clear(0x674D);  //light green
		LCD_SetTextColor(0x6A62); //brown
		LCD_SetBackColor(0x674D); //light green 
	
		LCD_DisplayStringLine(Line0,"     TreeMinder  ");
		LCD_DrawLine(29,320-50,220,Horizontal);
		LCD_DrawLine(30,320-50,220,Horizontal);
		
		LCD_SetTextColor(White); 
		LCD_DisplayStringLine(Line2,"NAME: User");
		
		LCD_DisplayStringLine(Line3,"AGE: <    >");
		LCD_DrawRect(73,320-77,20,20); //--
		LCD_DrawRect(73,320-155,20,20); //++
		
		LCD_DisplayStringLine(Line4,"WEIGHT: <    >");
		LCD_DrawRect(97,320-123,20,20);
		LCD_DrawRect(97,320-203,20,20);
		
		LCD_DisplayStringLine(Line5,"WAKE:<    >:<    >");
		LCD_DrawRect(120,320-80,20,20);
		LCD_DrawRect(120,320-158,20,20);
		LCD_DrawRect(120,320-190,20,20);
		LCD_DrawRect(120,320-270,20,20);
		
		LCD_DisplayStringLine(Line6,"SLEEP:<    >:<    >");
		LCD_DrawRect(145,320-93,20,20);
		LCD_DrawRect(145,320-171,20,20);
		LCD_DrawRect(145,320-203,20,20);
		LCD_DrawRect(145,320-283,20,20);
		
		LCD_DisplayStringLine(Line7,"TIME:<    >:<    >");
		LCD_DrawRect(170,320-80,20,20);
		LCD_DrawRect(170,320-158,20,20);
		LCD_DrawRect(170,320-190,20,20);
		LCD_DrawRect(170,320-270,20,20);
		
		LCD_SetTextColor(Yellow);
		LCD_DisplayStringLine(Line9,"        DONE        ");
		LCD_DrawRect(215,320-115,25,90);
	
		LCD_DisplayChar(Line3,320-130,age%10+'0');
		LCD_DisplayChar(Line3,320-115,age/10+'0');
		
		LCD_DisplayChar(Line4,320-180,kg%10+'0');
		LCD_DisplayChar(Line4,320-165,kg/10+'0');
		
		LCD_DisplayChar(Line5,320-130,wakeTime.Hours%10+'0');
		LCD_DisplayChar(Line5,320-115,wakeTime.Hours/10+'0');
		LCD_DisplayChar(Line5,320-240,wakeTime.Minutes%10+'0');
		LCD_DisplayChar(Line5,320-225,wakeTime.Minutes/10+'0');
		
		LCD_DisplayChar(Line6,320-140,sleepTime.Hours%10+'0');
		LCD_DisplayChar(Line6,320-125,sleepTime.Hours/10+'0');
		LCD_DisplayChar(Line6,320-255,sleepTime.Minutes%10+'0');
		LCD_DisplayChar(Line6,320-240,sleepTime.Minutes/10+'0');
		
		LCD_DisplayChar(Line7,320-130,sTime.Hours%10+'0');
		LCD_DisplayChar(Line7,320-115,sTime.Hours/10+'0');
		LCD_DisplayChar(Line7,320-240,sTime.Minutes%10+'0');
		LCD_DisplayChar(Line7,320-225,sTime.Minutes/10+'0');
	
	while(inStartMenu) {
		HAL_UART_Receive_DMA(&huart1,(uint8_t *)recieve,20);
		
		x = TCS_Read_X();
		y = TCS_Read_Y();
		
			sprintf (disp, "[%02d:%02d]\n\r", x,y);
			HAL_UART_Transmit(&huart2,(uint8_t *)disp,strlen(disp),1000);
		
		
			// AGE //
		if (x >= 150 && x <= 160 && y >=240-160 && y <= 240-150 ){ //++ age
			if (age < 99 && age>1 ){
					age++;
					LCD_DisplayChar(Line3,320-130,age%10+'0');
					LCD_DisplayChar(Line3,320-115,age/10+'0');
					HAL_Delay(200);
				}
			}
		if (x >= 220 && x <= 230 && y >=240-160 && y <= 240-150 ){ //-- age
			if (age <= 99 && age>1 ){
				age--;
				LCD_DisplayChar(Line3,320-130,age%10+'0');
				LCD_DisplayChar(Line3,320-115,age/10+'0');
				HAL_Delay(200);
			}
		}
			
		// WEIGHT //
		if (x >= 100 && x <= 110 && y >= 240-135 && y <= 240-125){  //++weight
			if (kg < 99 && kg>1 ){
					kg++;
					LCD_DisplayChar(Line4,320-180,kg%10+'0');
					LCD_DisplayChar(Line4,320-165,kg/10+'0');
					HAL_Delay(200);
				}
		}
		
		if (x >= 180 && x <= 190 && y >= 240-135 && y <= 240-125){  //--weight
			if (kg <= 99 && kg >1 ){
					kg--;
					LCD_DisplayChar(Line4,320-180,kg%10+'0');
					LCD_DisplayChar(Line4,320-165,kg/10+'0');
					HAL_Delay(200);
				}
		}
		
		// WAKE HR //
		if (x >= 150 && x <= 160 && y >=240-110 && y <= 240-100 ){ //++ wakehr
					wakeTime.Hours = (wakeTime.Hours+1)%24;
					LCD_DisplayChar(Line5,320-130,wakeTime.Hours%10+'0');
					LCD_DisplayChar(Line5,320-115,wakeTime.Hours/10+'0');
					HAL_Delay(200);
		}
		
		if (x >= 220 && x <= 230 && y >=240-110 && y <= 240-100 ){ //-- wakehr
				if (wakeTime.Hours > 0 ){
					wakeTime.Hours--;
					LCD_DisplayChar(Line5,320-130,wakeTime.Hours%10+'0');
					LCD_DisplayChar(Line5,320-115,wakeTime.Hours/10+'0');
					HAL_Delay(200);
				}
		}
		
		// WAKE MIN //
		if (x >= 30 && x <= 40 && y >=240-110 && y <= 240-100 ){ //++wake min
			wakeTime.Minutes = (wakeTime.Minutes+1)%60;
					LCD_DisplayChar(Line5,320-240,wakeTime.Minutes%10+'0');
					LCD_DisplayChar(Line5,320-225,wakeTime.Minutes/10+'0');
			HAL_Delay(200);
				
		}
		if (x >= 110 && x <= 120 && y >=240-110 && y <= 240-100 ){ //-- wakemin
				if (wakeTime.Minutes > 0 ){
					wakeTime.Minutes--;
					LCD_DisplayChar(Line5,320-240,wakeTime.Minutes%10+'0');
					LCD_DisplayChar(Line5,320-225,wakeTime.Minutes/10+'0');
					HAL_Delay(200);
				}
		}
		
		// SLEEP HR //
		if (x >= 130 && x <= 140 && y >=240-85 && y <= 240-75 ){ //++ slphr
					sleepTime.Hours = (sleepTime.Hours+1)%24;
					LCD_DisplayChar(Line6,320-140,sleepTime.Hours%10+'0');
					LCD_DisplayChar(Line6,320-125,sleepTime.Hours/10+'0');
			HAL_Delay(200);
		}
		
		if (x >= 210 && x <= 220 && y >=240-85 && y <= 240-75 ){ //-- slphr
				if (sleepTime.Hours > 0 ){
					sleepTime.Hours--;
					LCD_DisplayChar(Line6,320-140,sleepTime.Hours%10+'0');
					LCD_DisplayChar(Line6,320-125,sleepTime.Hours/10+'0');
					HAL_Delay(200);
				}
				
		}
		
		// SLEEP MIN //
		if (x >= 20 && x <= 30 && y >=240-85 && y <= 240-75 ){ //++slp min
			sleepTime.Minutes = (sleepTime.Minutes+1)%60;
			LCD_DisplayChar(Line6,320-255,sleepTime.Minutes%10+'0');
			LCD_DisplayChar(Line6,320-240,sleepTime.Minutes/10+'0');
			HAL_Delay(200);
			
		}
		if (x >= 100 && x <= 110 && y >=240-85 && y <= 240-75 ){ //-- slpmin
			if (sleepTime.Minutes > 0 ){
					sleepTime.Minutes--;
					LCD_DisplayChar(Line6,320-255,sleepTime.Minutes%10+'0');
					LCD_DisplayChar(Line6,320-240,sleepTime.Minutes/10+'0');
				HAL_Delay(200);
				}
		}
		
		
		// TIME HR //
		if (x >= 150 && x <= 160 && y >=240-60 && y <= 240-50 ){ //++ tmhr
					sTime.Hours = (sTime.Hours+1)%24;
					LCD_DisplayChar(Line7,320-130,sTime.Hours%10+'0');
					LCD_DisplayChar(Line7,320-115,sTime.Hours/10+'0');
			HAL_Delay(200);
		}
		
		if (x >= 220 && x <= 230 && y >=240-60 && y <= 240-50 ){ //-- tmhr
				if (sTime.Hours > 0 ){
					sTime.Hours--;
					LCD_DisplayChar(Line7,320-130,sTime.Hours%10+'0');
					LCD_DisplayChar(Line7,320-115,sTime.Hours/10+'0');
					HAL_Delay(200);
				}
		}
		

		// TIME MIN //
		if (x >= 30 && x <= 40 && y >=240-60 && y <= 240-50 ){ //++tmmin
			sTime.Minutes = (sTime.Minutes+1)%60;
			LCD_DisplayChar(Line7,320-240,sTime.Minutes%10+'0');
			LCD_DisplayChar(Line7,320-225,sTime.Minutes/10+'0');
			HAL_Delay(200);
				
		}
		if (x >= 110 && x <= 120 && y >=240-60 && y <= 240-50 ){ //-- tmmin
				if (sTime.Minutes > 0 ){
					sTime.Minutes--;
					LCD_DisplayChar(Line7,320-240,sTime.Minutes%10+'0');
					LCD_DisplayChar(Line7,320-225,sTime.Minutes/10+'0');
					HAL_Delay(200);
				}
		}
		
		// DONE //
		if (x >= 100 && x <= 200 && y >=240-10 && y <= 240-0 ){ //-- DONE
				calculate();
			
			
			sprintf (disp, "%c", 'a'+kg);
			HAL_UART_Transmit_IT(&huart1, (uint8_t *) disp, 1);
			
				inStartMenu = 0;
		} 
		
	}
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}


void setTree(int treeNo,int step1,int step2, int step3, int step4, int step5, int step6, int step7, int step8){
		int num_25 = 320-25;
		int num_24 = 320-24;
		int num_10 = 320-10;
		int num_40 = 320-40;
		int num_26 = 320-26;
		
				
		LCD_SetTextColor(0x6A62); //brown
		LCD_DrawCircle(239,num_25-44*(treeNo),10); //ground
		LCD_DrawCircle(239,num_25-44*(treeNo),9); //ground
		LCD_DrawCircle(239,num_25-44*(treeNo),8); //ground
	
		if(step2+step3+step4+step5+step6+step7 && step1==0) {
					LCD_SetTextColor(Grey); 
					LCD_DrawLine(195,num_25-44*(treeNo),35,Vertical); //first stem
		}
		if(step5+step6+step7 && step4==0) {
					LCD_SetTextColor(Grey); 
					LCD_DrawLine(155,num_25-44*(treeNo),40,Vertical); //stem grows
		}
		if(step8 && step7==0) {
					LCD_SetTextColor(Grey); 
					LCD_DrawLine(120,num_25-44*(treeNo),45,Vertical); //stem grows
		}
			
				if (step1 == 0){
						;
				}
				if (step1 == 1){   
					LCD_SetTextColor(0x6A62); //brown
					LCD_DrawLine(195,num_25-44*(treeNo),35,Vertical); //first stem
				}
				
				if (step1 == 2){   
					LCD_SetTextColor(Yellow); 
					LCD_DrawLine(195,num_25-44*(treeNo),35,Vertical); //first stem
				}
				
				if (step2 == 0){
						
						;
				}
				if (step2 == 1){
					LCD_SetTextColor(0x6A62); //brown
					LCD_DrawLine(215,num_24-44*(treeNo),10,Left);	
					LCD_SetTextColor(0x674D); // light green
					LCD_DrawCircle(200,num_10-44*(treeNo),8);  //fresh leaf
					LCD_DrawCircle(200,num_10-44*(treeNo),7);  //fresh leaf
					LCD_DrawCircle(200,num_10-44*(treeNo),6);  //fresh leaf
				}
				if (step2 == 2){
					LCD_SetTextColor(0x6A62); //brown
					LCD_DrawLine(215,num_24-44*(treeNo),10,Left);	
					LCD_SetTextColor(0x674D); // light green
					LCD_DrawCircle(200,num_10-44*(treeNo),5);  //wither leaf
				}
				
				if (step3 == 0){
						;
				}
				if (step3 == 1){
					LCD_SetTextColor(0x6A62); //brown
					LCD_DrawLine(205,num_26-44*(treeNo),10,Right);
					LCD_SetTextColor(0x674D); // light green
					LCD_DrawCircle(190,num_40-44*(treeNo),8);  //fresh leaf
					LCD_DrawCircle(190,num_40-44*(treeNo),7);  //fresh leaf
					LCD_DrawCircle(190,num_40-44*(treeNo),6);  //fresh leaf
				
				}
				if (step3 == 2){
					LCD_SetTextColor(0x6A62); //brown
					LCD_DrawLine(205,num_26-44*(treeNo),10,Right);
					LCD_SetTextColor(0x674D); // light green
					LCD_DrawCircle(190,num_40-44*(treeNo),5);  //wither leaf
				}
				
				if (step4 == 0){
						;
				}
				if (step4 == 1){   
					LCD_SetTextColor(0x6A62); //brown
					LCD_DrawLine(155,num_25-44*(treeNo),40,Vertical); //stem grows
				}
				if (step4 == 2){   
					LCD_SetTextColor(Yellow); 
					LCD_DrawLine(155,num_25-44*(treeNo),40,Vertical); //stem grows
				}
				
				if (step5 == 0){
					;
				}
				if (step5 == 1){
					LCD_SetTextColor(0x6A62); //brown
					LCD_DrawLine(175,num_24-44*(treeNo),10,Left);
					LCD_SetTextColor(0x674D); // light green
					LCD_DrawCircle(160,num_10-44*(treeNo),8);  //fresh leaf
					LCD_DrawCircle(160,num_10-44*(treeNo),7);  //fresh leaf	
					LCD_DrawCircle(160,num_10-44*(treeNo),6);  //fresh leaf		
						
				}
				if (step5 == 2){
					LCD_SetTextColor(0x6A62); //brown
					LCD_DrawLine(175,num_24-44*(treeNo),10,Left);
					LCD_SetTextColor(0x674D); // light green			
					LCD_DrawCircle(160,num_10-44*(treeNo),5);  //wither leaf	
				}
				
				if (step6 == 0){
					;
				}
				if (step6 == 1){
					LCD_SetTextColor(0x6A62); //brown
					LCD_DrawLine(155,num_26-44*(treeNo),10,Right);
					LCD_SetTextColor(0x674D); // light green
					LCD_DrawCircle(140,num_40-44*(treeNo),8);  //fresh leaf 
					LCD_DrawCircle(140,num_40-44*(treeNo),7);  //fresh leaf 
					LCD_DrawCircle(140,num_40-44*(treeNo),6);  //fresh leaf 
					
				}
				if (step6 == 2){
					LCD_SetTextColor(0x6A62); //brown
					LCD_DrawLine(155,num_26-44*(treeNo),10,Right);
					LCD_SetTextColor(0x674D); // light green
					LCD_DrawCircle(140,num_40-44*(treeNo),5);  //wither leaf
				}
				
				if (step7 == 0){
					;
				}
				if (step7 == 1){ 
					LCD_SetTextColor(0x6A62); //brown
					LCD_DrawLine(120,num_25-44*(treeNo),45,Vertical); //stem grows
				}
				if (step7 == 2){ 
					LCD_SetTextColor(Yellow); 
					LCD_DrawLine(120,num_25-44*(treeNo),45,Vertical); //stem grows
				}
				
				if (step8 == 0){
					;
				}
				if (step8 == 1 ){
					LCD_SetTextColor(0x674D); // light greecn
					LCD_DrawCircle(115,num_25-44*(treeNo),15);  //fresh leaf		
					LCD_DrawCircle(115,num_25-44*(treeNo),14);  //fresh leaf
					LCD_DrawCircle(115,num_25-44*(treeNo),13);  //fresh leaf					
				}
				if (step8 == 2 && step7 != 0){
					LCD_SetTextColor(0x674D); // light green			
					LCD_DrawCircle(115,num_25-44*(treeNo),10);  //wither leaf	
				}






}


void setTreeStat(int treeNo) {
	

	int toSet[8];
	for(int i=0;i<8;i++) {
		if(statistic[treeNo][i]<3)
			toSet[i] = 0;
		else if(statistic[treeNo][i]<7)
			toSet[i] = 2;
		else
			toSet[i] = 1;
		
	}
	setTree(treeNo,toSet[0],toSet[1],toSet[2],toSet[3],toSet[4],toSet[5],toSet[6],toSet[7]);
}

void callMenu() {
	
			LCD_Clear(White);  //light green
	
			LCD_SetBackColor(Blue);
			LCD_SetTextColor(White);
  		LCD_DisplayStringLine(Line0,"     TreeMinder     ");
			LCD_SetBackColor(White);
			
			LCD_SetTextColor(Black);
			HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
			sprintf (disp, " YOU   %02d:%02d   TIME ", sTime.Hours,sTime.Minutes);
			LCD_DisplayStringLine(Line1,(uint8_t *)disp);
			LCD_DisplayStringLine(Line2,"       CHECK       ");
			
			LCD_SetTextColor(Yellow);
			LCD_DrawRect(18,315,30,80); //you button
			LCD_DrawRect(18,90,30,80); //time button
			LCD_DrawRect(42,220,30,110); //check button
	
			setTreeStat(0);
			setTreeStat(1);
			setTreeStat(2);
			setTreeStat(3);
			setTreeStat(4);
			setTreeStat(5);
			setTreeStat(6);


}
	


void callYou() {
	uint16_t x,y;
	inStartMenu = 2;
	
	LCD_Clear(0x674D);  //light green
	
	LCD_SetTextColor(0x6A62); //brown
	LCD_SetBackColor(0x674D); //light green 		
	
	sprintf(disp,"NAME:%s",name);
	LCD_DisplayStringLine(Line0, (uint8_t *) disp);
	sprintf(disp,"AGE:%02d KG:%02d",age,kg);
	LCD_DisplayStringLine(Line1, (uint8_t *) disp);
	sprintf (disp, "Wake Time:  [%02d:%02d]", wakeTime.Hours,wakeTime.Minutes);
	LCD_DisplayStringLine(Line2, (uint8_t *) disp);
	sprintf (disp, "Sleep Time:  [%02d:%02d]", sleepTime.Hours,sleepTime.Minutes);
	LCD_DisplayStringLine(Line3, (uint8_t *) disp);
	sprintf (disp, "Next Time: [%02d:%02d]", alarm[pass+1].Hours,alarm[pass+1].Minutes);
	LCD_DisplayStringLine(Line4,(uint8_t *)disp);
	
	
	
	LCD_SetTextColor(Yellow);
	LCD_DisplayStringLine(Line9,"        DONE        ");
	LCD_DrawRect(215,320-115,25,90);
	
	while(inStartMenu) {
		x = TCS_Read_X();
		y = TCS_Read_Y();
		if(x!=0 && y != 0) {//if (x >= 100 && x <= 200 && y >=240-50 && y <= 240-0 ){ //-- DONE
			
				inStartMenu = 0;
		} 
		HAL_Delay(100);
	}
	callMenu();
}

void callTime() {
	uint16_t x,y;
	inStartMenu = 3;
	
	
	
	LCD_Clear(0x674D);  //light green
	LCD_SetTextColor(White); //brown
	LCD_SetBackColor(0x674D); //light green 
	
	sprintf (disp, "Water poured at:    ");
	LCD_DisplayStringLine(Line0,(uint8_t *)disp);
	for(int i=0;i<8;i++) {
		sprintf (disp, " : [%02d:%02d]          ",alarm[i].Hours,alarm[i].Minutes);
		disp[0] = i+'0';
		if(i==pass+1) LCD_SetTextColor(Yellow);
		else LCD_SetTextColor(White);
		LCD_DisplayStringLine(Line1+24*i,(uint8_t *)disp);
	}
	
	LCD_SetTextColor(Yellow);
	LCD_DisplayStringLine(Line9,"        DONE        ");
	LCD_DrawRect(215,320-115,25,90);
	
	while(inStartMenu) {
		x = TCS_Read_X();
		y = TCS_Read_Y();
		if(x!=0 && y != 0) {//if (x >= 100 && x <= 200 && y >=240-50 && y <= 240-0 ){ //-- DONE
				inStartMenu = 0;
		}
		HAL_Delay(100);		
	}
	callMenu();
}
void callStat(int num) {
	uint16_t x,y;
	inStartMenu = 4;
	
	
	
	LCD_Clear(0x674D);  //light green
	LCD_SetTextColor(White); //brown
	LCD_SetBackColor(0x674D); //light green 
	
	sprintf (disp, "--- Day %d ---",num+1);
	LCD_DisplayStringLine(Line0,(uint8_t *)disp);
	
	for(int i=0;i<8;i++) {
		sprintf (disp, "Glass %d: %d%%", i+1,statistic[num][i]*10);
		LCD_DisplayStringLine(Line1+24*i,(uint8_t *)disp);
	}
	
	LCD_SetTextColor(Yellow);
	LCD_DisplayStringLine(Line9,"        DONE        ");
	LCD_DrawRect(215,320-115,25,90);
	
	while(inStartMenu) {
		x = TCS_Read_X();
		y = TCS_Read_Y();
		if(x!=0 && y != 0) {//if (x >= 100 && x <= 200 && y >=240-50 && y <= 240-0 ){ //-- DONE
				inStartMenu = 0;
		}
		HAL_Delay(100);		
	}
	callMenu();
	
}
//Interrupt callback routine

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI3_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_RTC_Init();
  MX_SPI1_Init();

  /* USER CODE BEGIN 2 */
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_TC);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3) ;
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	HAL_DMA_Init(&hdma_usart1_rx);
	
	
	
	LCD_Setup();
	

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	int X, Y;
	
	uint16_t posX, posY,maxX=320,maxY=240;
	char pos[50];
	
	closeValve();

		
		startMenu();
		
		callTime();

	int dateOrTime = 0;
	
  while (1)
  {
		
			X = TCS_Read_X();
			Y = TCS_Read_Y();
			
			
			if (Y >= 24 && Y <= 48 && X >= 240 && X <= 320){ //call You button
					callYou();
			}
			else if (Y >= 24 && Y <= 48 && X >= 0 && X <= 80){ //call Time Button
					callTime();
			}
			else if (Y >= 24 && Y <= 48){ //call check Button
				dateOrTime = (dateOrTime+1)%2;
				LCD_SetTextColor(Black);
				if(dateOrTime) {
					HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
					sprintf (disp, " YOU %s %02d/%02d TIME ", toDateString(sDate.WeekDay),sDate.Date,sDate.Month);
					LCD_DisplayStringLine(Line1,(uint8_t *)disp);
				} else {
					HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
					sprintf (disp, " YOU   %02d:%02d   TIME ", sTime.Hours,sTime.Minutes);
					LCD_DisplayStringLine(Line1,(uint8_t *)disp);
					
				}
				
				LCD_SetTextColor(Yellow);
				LCD_DrawRect(18,320-5,30,80); //you button
				LCD_DrawRect(18,320-230,30,80); //time button
				
			}
			
			else if (Y > 48 && Y < 115 && X >= 100 && X <= 220){ //call check Button
					sprintf (disp, "%c", 'V'+((pass==-1)? 7:pass) );
					HAL_UART_Transmit_IT(&huart1, (uint8_t *) disp, 1);
					LCD_SetTextColor(Red);
					LCD_DisplayStringLine(Line2,"       WAIT.       ");
					LCD_DrawRect(42,220,30,110); //check button
					HAL_Delay(100);
					LCD_SetTextColor(Black);
					LCD_DisplayStringLine(Line2,"       CHECK       ");
					LCD_SetTextColor(Yellow);
					LCD_DrawRect(42,220,30,110); //check button
			}

			else if (Y >= 115 && Y <= 240 && X >= 320-44 && X <= 320-1){ 
					callStat(0);
			}
				else if (Y >= 115 && Y <= 240 && X >= 320-99 && X <= 320-45){ 
					callStat(1);
			}
					else if (Y >= 115 && Y <= 240 && X >= 320-144 && X <= 320-100){ 
					callStat(2);
			}
						else if (Y >= 115 && Y <= 240 && X >= 320-189 && X <= 320-145){ 
					callStat(3);
			}
							else if (Y >= 115 && Y <= 240 && X >= 320-234 && X <= 320-190){ 
					callStat(4);
			}
								else if (Y >= 115 && Y <= 240 && X >= 320-279 && X <= 320-235){ 
					callStat(5);
			}
									else if (Y >= 115 && Y <= 240 && X >= 320-319 && X <= 320-280){
					callStat(6);
			}
		
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		if(sTime.Seconds == 0) {
			LCD_SetTextColor(Black);
			sprintf (disp, " YOU   %02d:%02d   TIME ", sTime.Hours,sTime.Minutes);
			LCD_DisplayStringLine(Line1,(uint8_t *)disp);
			
			LCD_SetTextColor(Yellow);
			LCD_DrawRect(18,320-5,30,80); //you button
			LCD_DrawRect(18,320-230,30,80); //time button

		}
		
		if(compareTime(sTime,alarm[pass+1])) {
			pass++;
			if(pass>=7) {
				pass = -1;
				day++;
				if(day>=7)
					day = 0;
				  for(int i=0;i<7;i++)
						for(int j=0;j<8;j++)
							statistic[i][j] = 0;

			}
			
			
			
			
			//strcpy(str,"It's time to drink !");
			//HAL_UART_Transmit(&huart2, (uint8_t *) str, strlen(str), 1000);
			
			HAL_UART_Transmit_IT(&huart1, (uint8_t *) "C", 1);
			HAL_Delay(1000);
			//closeValve();
			sprintf (disp, "Next Time: [%02d:%02d]", alarm[pass+1].Hours,alarm[pass+1].Minutes);
			LCD_DisplayStringLine(Line3,(uint8_t *)disp);
		}
		HAL_UART_Receive_DMA(&huart1,(uint8_t *)recieve,20);


		
		
		
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	
  }
	
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV5;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_PLL2;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL2_ON;
  RCC_OscInitStruct.PLL2.PLL2MUL = RCC_PLL2_MUL8;
  RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /**Configure the Systick interrupt time 
    */
  __HAL_RCC_PLLI2S_ENABLE();

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* RTC init function */
static void MX_RTC_Init(void)
{

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef DateToUpdate;

    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initialize RTC and set the Time and Date 
    */
  sTime.Hours = 1;
  sTime.Minutes = 0;
  sTime.Seconds = 0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 1;
  DateToUpdate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }

}

/* SPI3 init function */
static void MX_SPI3_Init(void)
{

  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi3.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }

}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 72-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = (1000-1)/2;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim2);

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PE3 PE4 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PE5 PE6 PE7 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PC5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
