#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
#include "led.h"
#include "uart.h"
#include "timer.h"
#include "start_task.h"
#include "iwdg.h"
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();	
	uart_init(115200); 
	led_init();
	printf("\r\n");
	printf("APP运行OK!\r\n");
	create_start_task();//创建开始任务
	vTaskStartScheduler();//开启任务调度
	while(1){}
}

