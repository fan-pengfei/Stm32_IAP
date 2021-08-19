#include "start_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"

#include "led_task.h"
#include "lcd_task.h"
#include "usart_task.h"
#include "timer_task.h"
#include "key_task.h"

#include "snake_task.h"
#define START_TASK_PRIO 1
#define START_TASK_STK_SIZE 128
TaskHandle_t Start_Task_Handler;

#define LED_TASK_PRIO 2
#define LED_TASK_STK_SIZE 128
TaskHandle_t Led_Task_Handler;


#define USART_TASK_PRIO 3
#define USART_TASK_STK_SIZE 256
TaskHandle_t Usart_Task_Handler;

#define TIMER_TASK_PRIO 4
#define TIMER_TASK_STK_SIZE 128
TaskHandle_t Timer_Task_Handler;

#define KEY_TASK_PRIO 5
#define KEY_TASK_STK_SIZE 128
TaskHandle_t Key_Task_Handler;


void create_start_task(void)
{
			xTaskCreate((TaskFunction_t)start_task,
                (char*         )"start_task",
                (uint16_t      )START_TASK_STK_SIZE,
                (void *        )NULL,
                (UBaseType_t   )START_TASK_PRIO,
                (TaskHandle_t*)&Start_Task_Handler);
}

void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();//进入临界区
	
	xTaskCreate((TaskFunction_t)led_task,
                (char*         )"led_task",
                (uint16_t      )LED_TASK_STK_SIZE,
                (void *        )NULL,
                (UBaseType_t   )LED_TASK_PRIO,
                (TaskHandle_t*)&Led_Task_Handler);
								
								
	xTaskCreate((TaskFunction_t)usart_task,
							(char*         )"usart_task",
							(uint16_t      )USART_TASK_STK_SIZE,
							(void *        )NULL,
							(UBaseType_t   )USART_TASK_PRIO,
							(TaskHandle_t*)&Usart_Task_Handler);
							
	xTaskCreate((TaskFunction_t)timer_task,
							(char*         )"timer_task",
							(uint16_t      )TIMER_TASK_STK_SIZE,
							(void *        )NULL,
							(UBaseType_t   )TIMER_TASK_PRIO,
							(TaskHandle_t*)&Timer_Task_Handler);		

	xTaskCreate((TaskFunction_t)key_task,
							(char*         )"key_task",
							(uint16_t      )KEY_TASK_STK_SIZE,
							(void *        )NULL,
							(UBaseType_t   )KEY_TASK_PRIO,
							(TaskHandle_t*)&Key_Task_Handler);	

							
	vTaskDelete(Start_Task_Handler);//删除开始任务
	taskEXIT_CRITICAL();//退出临界区
}
