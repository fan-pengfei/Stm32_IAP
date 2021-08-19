#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx.h"


/*---------------------- �������ú� ------------------------*/

#define KEY_PIN           GPIO_Pin_0        		 // KEY ����      
#define KEY_PORT          GPIOF                     // KEY GPIO�˿�     
#define KEY_CLK           RCC_AHB1Periph_GPIOF	    // KEY GPIO�˿�ʱ��

/*---------------------- �������� ----------------------------*/

void 	KEY_Init(void);	//����IO�ڳ�ʼ������

#endif //__KEY_H
