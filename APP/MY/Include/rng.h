#ifndef __RNG_H
#define __RNG_H	 
#include "stm32f4xx.h"
	
u8  RNG_Init(void);			//RNG��ʼ�� 
u32 RNG_Get_RandomNum(void);//�õ������
int RNG_Get_RandomRange(int min,int max);//����[min,max]��Χ�������
#endif

















