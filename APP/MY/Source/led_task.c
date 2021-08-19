#include "led_task.h"
#include "led.h"
#include "uart.h"
#include "iwdg.h"
void led_task(void *pvParameters)
{
	while(1)
	{
		led_switch(on);
		vTaskDelay(500);
		led_switch(off);
		vTaskDelay(500);
		
	}
}

