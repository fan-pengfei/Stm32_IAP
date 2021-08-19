#include "key.h"
void KEY_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体
	RCC_AHB1PeriphClockCmd ( KEY_CLK, ENABLE); 	//初始化KEY时钟	
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;   //输出模式
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	//上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //速度选择
	GPIO_InitStructure.GPIO_Pin   = KEY_PIN;	 
	
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOF,GPIO_Pin_0);
}

