#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f4xx.h"
void TIM5_Init(u16 arr,u16 psc);
void	TIM2_PWMinit(u16 arr,u16 psc);
void Encoder_Init_TIM4(u16 arr,u16 psc);
void Encoder_Init_TIM3(u16 arr,u16 psc);
void TIM1_GPIO_Config(void);
void Tim1_Config(void);
void  Adc_Init(void);
u16 Get_Adc(u8 ch) ;
typedef struct
{
	int raw_angle;
	int last_raw_angle;
	long long total_raw_angle;
	float angle;
	double total_angle;
	int round_cnt;
	float speed;
}motor_data;
enum{
	right=1,
	left
};
#endif
