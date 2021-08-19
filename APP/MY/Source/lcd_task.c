#include "lcd_task.h"
#include "lcd.h"
#include "stdio.h"
#include "timer_task.h"
#include "snake.h"
#include "start_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
#include "timer_task.h"
#include "adc.h"
#include "snake_task.h"
#include "key.h"
#include "key_task.h"
#include "snake.h"

#define TIMER_TASK_PRIO 5
#define TIMER_TASK_STK_SIZE 128
TaskHandle_t Timer_Task_Handler;

#define SNAKE_TASK_PRIO 4
#define SNAKE_TASK_STK_SIZE 512
TaskHandle_t Snake_Task_Handler;


#define KEY_TASK_PRIO 3
#define KEY_TASK_STK_SIZE 128
TaskHandle_t Key_Task_Handler;

u8 flag_start=1,flag_line1=1,flag_line2=0,flag_line3=0;
u8 text[100];
u16 s,s_,ms_,ms;
extern SNAKE snake;;
extern u32 key_flag,key_num;
extern int flag_direct;
u32 num_1,num_2,num_4,num_3;
u8 begin=0;
void lcd_task(void *pvParameters)
{
	u32 i;
	while(1)
	{
		if(flag_start)
		{
			sprintf((char *)&text,"SNAKE");
			LCD_ShowString(100,0,text,RED);
			sprintf((char *)&text,"GAME BEGIN");
			LCD_ShowString(82,40,text,RED);
			sprintf((char *)&text,"GAME LEVEL");
			LCD_ShowString(82,60,text,RED);
			sprintf((char *)&text,"GAME MODE");
			LCD_ShowString(82,80,text,RED);
			if(flag_line1)
			{
				LCD_DrawPoint_big(180,48,BLACK);
				LCD_DrawPoint_big(180,48+20,WHITE);	
				LCD_DrawPoint_big(180,48+40,WHITE);	
				sprintf((char *)&text,"       ");
				LCD_ShowString(185,60,text,RED);	
				sprintf((char *)&text,"       ");
				LCD_ShowString(185,80,text,RED);					
			}
			else if(flag_line2)
			{
				LCD_DrawPoint_big(180,48+20,BLACK);	
				LCD_DrawPoint_big(180,48,WHITE);
				LCD_DrawPoint_big(180,48+40,WHITE);	
				sprintf((char *)&text,"%d",snake.level%10);
				LCD_ShowString(185,60,text,RED);
				sprintf((char *)&text,"       ");
				LCD_ShowString(185,80,text,RED);	
			}
			else if(flag_line3)
			{
				LCD_DrawPoint_big(180,48+40,BLACK);	
				LCD_DrawPoint_big(180,48,WHITE);
				LCD_DrawPoint_big(180,48+20,WHITE);
				sprintf((char *)&text,"%d",snake.mode%2);
				LCD_ShowString(185,80,text,RED);				
				sprintf((char *)&text,"       ");
				LCD_ShowString(185,60,text,RED);				
			}
			if(Get_Adc(5)>=3500)
			{
				num_2++;
				if(num_2==3)
				{
					i++;
				}
			}
			if(Get_Adc(5)>=2000&&Get_Adc(5)<=3000)
			{
				num_2=0;
			}
			if(Get_Adc(4)<500&&flag_line2)
			{
				num_4++;
				if(num_4==3&&flag_line2)
				{
					snake.level++;
				}
			}
			if(Get_Adc(4)>=2000&&Get_Adc(4)<=3000&&flag_line2)
			{
				num_4=0;
			}
			if(Get_Adc(4)<500&&flag_line3)
			{
				num_3++;
				if(num_3==3&&flag_line3)
				{
					snake.mode++;
				}
			}
			if(Get_Adc(4)>=2000&&Get_Adc(4)<=3000&&flag_line3)
			{
				num_3=0;
			}
			if(Get_Adc(4)<500&&flag_line1)
			{
				num_1++;
				if(num_1==3&&flag_line1)
				{
					begin++;
				}
			}
			if(Get_Adc(4)>=2000&&Get_Adc(4)<=3000&&flag_line1)
			{
				num_1=0;
			}
			switch(i%3)
			{
				case 0:
					flag_line1=1;
					flag_line2=0;
					flag_line3=0;
				break;
				case 1:
					flag_line1=0;
					flag_line2=1;
					flag_line3=0;
				break;
				case 2:
					flag_line1=0;
					flag_line2=0;
					flag_line3=1;
				break;
			}
			if(flag_line1==1&&begin)
			{
				LCD_Clear(WHITE);
				flag_line1=0;
				key_flag=0;
				flag_start=0;
				taskENTER_CRITICAL();//进入临界区
				xTaskCreate((TaskFunction_t)timer_task,
						(char*         )"timer_task",
						(uint16_t      )TIMER_TASK_STK_SIZE,
						(void *        )NULL,
						(UBaseType_t   )TIMER_TASK_PRIO,
						(TaskHandle_t*)&Timer_Task_Handler);	
						
				xTaskCreate((TaskFunction_t)snake_task,
						(char*         )"snake_task",
						(uint16_t      )SNAKE_TASK_STK_SIZE,
						(void *        )NULL,
						(UBaseType_t   )SNAKE_TASK_PRIO,
						(TaskHandle_t*)&Snake_Task_Handler);							
				xTaskCreate((TaskFunction_t)key_task,
						(char*         )"key_task",
						(uint16_t      )KEY_TASK_STK_SIZE,
						(void *        )NULL,
						(UBaseType_t   )KEY_TASK_PRIO,
						(TaskHandle_t*)&Key_Task_Handler);
	      taskEXIT_CRITICAL();//退出临界区
			}
		}
		else 
		{
			if(snake.life)
			{
				sprintf((char *)&text,"Time:%02d:%02d:%02d:%d",s/3600,s%3600/60,s%60,ms/100);
				LCD_ShowString(110,0,text,RED);
				s_=s;
				ms_=ms;
				vTaskDelay(20);
			}
			else
			{
				sprintf((char *)&text,"Time:%02d:%02d:%02d:%d",s_/3600,s_%3600/60,s_%60,ms_/100);
				LCD_ShowString(110,0,text,RED);
				vTaskDelay(20);
			}
	  }
	}
}

