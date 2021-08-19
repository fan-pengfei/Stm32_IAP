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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init();	
	uart_init(115200); 
	led_init();
	printf("\r\n");
	printf("APP����OK!\r\n");
	create_start_task();//������ʼ����
	vTaskStartScheduler();//�����������
	while(1){}
}

