#include "snake.h"
#include "uart.h"
#include "snake_task.h"
#include "rng.h"
#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
#include "lcd_task.h"
SNAKE snake;
extern u8 text[100];
extern u32 key_flag;
int flag_direct=1,flag_dead=1,flag_food=0;
void snakeshow(void)
{	
	int i;
	if(snake.life)
	{
		for(i=0;i<snake.length;i++)
		{
			LCD_DrawPoint_big(snake.x[i],snake.y[i],RED);
		}
		LCD_DrawPoint_big(snake.x_food,snake.y_food,RED);
  }
	sprintf((char *)&text,"Score:%d",snake.length-1);
	LCD_ShowString(0,0,text,RED);
	for(i=0;i<2;i++)
	{
		LCD_DrawLine(2,16+i,238,16+i,RED);
		LCD_DrawLine(2,132+i,238,132+i,RED);
		LCD_DrawLine(1+i,16,1+i,134,RED);
		LCD_DrawLine(236+i,16,236+i,134,RED);
	}
}

void Snake_Init(void)
{
	snake.direct=2;
	snake.direct_init=0;
	snake.life=1;
	snake.length=1;
	snake.x[0]=120;
	snake.y[0]=67;
	snake.x_head=120;
	snake.y_head=67;
	snake.x_food=RNG_Get_RandomRange(5,235);
	snake.y_food=RNG_Get_RandomRange(17,130);
}
void snake_move(void)
{
	int i;
	for(i=0;i<snake.length;i++)
	{
		if(snake.x[i]<=4||snake.x[i]>=235||snake.y[i]<=19||snake.y[i]>=131)
		{
			if(flag_dead)
			{
				LCD_Clear(WHITE);
				flag_dead=0;
			}
		  if(snake.mode==0)
			{
				snake.life=0;
			}
		}
	}
	if(snake.life)
	{
		if(snake.mode==0&&!(snake.direct+flag_direct==3||snake.direct+flag_direct==7))
	  {
			snake.direct=flag_direct;
		}
		else
		{
			if(snake.direct==1)
			{
				if(abs(snake.x_head-snake.x_food)<=2)
				{

					snake.direct=snake.direct;
				}
				else if(snake.x_head-snake.x_food>2)
				{
					snake.direct=3;
				}				
				else
				{
					snake.direct=4;
				}
			}
			else if(snake.direct==2)
			{
				if(abs(snake.x_head-snake.x_food)<=2)
				{
						snake.direct=snake.direct;
				}
				else if(snake.x_head-snake.x_food>2)
				{
					snake.direct=3;
				}
				else
				{
					snake.direct=4;
				}
			}
			else if(snake.direct==3)
			{
				if(abs(snake.y_head-snake.y_food)<=2)
				{
					snake.direct=snake.direct;
				}
				else if(snake.y_head-snake.y_food>2)
				{
					snake.direct=1;
				}
				else
				{
					snake.direct=2;
				}
			}
			if(snake.direct==4)
			{
				if(abs(snake.y_head-snake.y_food)<=2)
				{
					snake.direct=snake.direct;
				}
				else if(snake.y_head-snake.y_food>2)
				{
					snake.direct=1;
				}
				else
				{
					snake.direct=2;
				}
			}
		}
		switch(snake.direct)
		{
			case 1:
				snake.y_head=snake.y_head-4;
			break;
			case 2:
				snake.y_head=snake.y_head+4;
			break;
			case 3:
				snake.x_head=snake.x_head-4;
			break;
			case 4:
				snake.x_head=snake.x_head+4;
			break;	
		}
	  LCD_DrawPoint_big(snake.x[snake.length-1],snake.y[snake.length-1],WHITE);
		for(i=1;i<snake.length;i++)
		{
			snake.x[i]=snake.x[i]%240;
			snake.y[i]=snake.y[i]%135;
			snake.x_head=snake.x_head%240;
			snake.y_head=snake.y_head%135;
			snake.x[snake.length-i]=snake.x[snake.length-i-1];
			snake.y[snake.length-i]=snake.y[snake.length-i-1];
		}		
		for(i=0;i<snake.length;i++)
		{
			
			if(snake.x[i]==snake.x_head&&snake.y[i]==snake.y_head)
			{
				if(snake.mode==0)
				{
					snake.life=0;
				}
			}
		}		
		snake.x[0]=snake.x_head;
		snake.y[0]=snake.y_head;

		if((abs(snake.x_head-snake.x_food)<=4)&&(abs(snake.y_head-snake.y_food)<=4))
		{			
			snake.length++;			
			for(i=1;i<snake.length;i++)
			{
				snake.x[snake.length-i]=snake.x[snake.length-i-1];
				snake.y[snake.length-i]=snake.y[snake.length-i-1];
			}
			LCD_DrawPoint_big(snake.x_food,snake.y_food,WHITE);
			LCD_DrawPoint_big(snake.x[snake.length],snake.y[snake.length],WHITE);
			food();
		}
	}
	
	else
	{
		sprintf((char *)&text,"You are dead!!");
		LCD_ShowString(70,50,text,RED);
	}
}
	
void food(void)
{
	int i;
	flag_food=0;
	while(flag_food==0)
	{	
		flag_food=1;
		snake.x_food=RNG_Get_RandomRange(5,235);
	  snake.y_food=RNG_Get_RandomRange(23,130);
		for(i=0;i<snake.length;i++)
		{
			if(abs(snake.x[i]-snake.x_food)<3&&abs(snake.y[i]-snake.y_food)<3)
			{
				flag_food=0;
			}
		}
		
  }

}
