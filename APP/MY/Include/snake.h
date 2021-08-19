#ifndef __SNAKE_H
#define __SNAKE_H

#include "stm32f4xx.h"
#include "lcd.h"
typedef struct Snake
{
	u8 x[1000];
	u8 y[1000];
	u8 x_food;
	u8 y_food;
	u8 x_head;
	u8 y_head;
	int length;
	char life;
	char direct;
	char direct_init;
	char mode;
	char level;
}SNAKE;
void snakeshow(void);
void Snake_Init(void);
void snake_move(void);
void food(void);
#endif

