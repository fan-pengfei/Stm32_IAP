#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx.h"


/*---------------------- 按键配置宏 ------------------------*/

#define KEY_PIN           GPIO_Pin_0        		 // KEY 引脚      
#define KEY_PORT          GPIOF                     // KEY GPIO端口     
#define KEY_CLK           RCC_AHB1Periph_GPIOF	    // KEY GPIO端口时钟

/*---------------------- 函数声明 ----------------------------*/

void 	KEY_Init(void);	//按键IO口初始化函数

#endif //__KEY_H
