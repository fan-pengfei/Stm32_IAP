#include "snake_task.h"
#include "snake.h"
#include "uart.h"
extern SNAKE snake;
void snake_task(void *pvParameters)
{
	while(1)
	{
		snake_move();
		snakeshow();
		vTaskDelay(100-10*(snake.level%10));
	}
}
