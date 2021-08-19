#include "key.h"
void KEY_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ��
	RCC_AHB1PeriphClockCmd ( KEY_CLK, ENABLE); 	//��ʼ��KEYʱ��	
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;   //���ģʽ
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //�ٶ�ѡ��
	GPIO_InitStructure.GPIO_Pin   = KEY_PIN;	 
	
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOF,GPIO_Pin_0);
}

