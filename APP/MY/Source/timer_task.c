#include "timer_task.h"
#include "uart.h"
#include "iwdg.h"
void timer_task(void *pvParameters)
{
	while(1)
	{		
		vTaskDelay(5);
		IWDG_Feed();//Î¹¹·
	}
}

