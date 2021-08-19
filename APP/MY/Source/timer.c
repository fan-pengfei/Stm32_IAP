#include "timer.h"
#include "uart.h"
motor_data left_m,right_m;
void Encoder_measure(motor_data *p,char tim)
{
		p->last_raw_angle=p->raw_angle;
		if(tim==right)
		{
			p->raw_angle = TIM3->CNT; 
		}			
		else
		{
			p->raw_angle = TIM4->CNT; 
		}
    p->angle=360.0*p->raw_angle/512.0;
    if(p->raw_angle - p->last_raw_angle > 256)
    {
        p->round_cnt --;
    }
    else if (p->raw_angle - p->last_raw_angle < -256)
    {
        p->round_cnt ++;
    }
		p->total_angle=p->round_cnt*360.0+p->angle;
		p->total_raw_angle=p->round_cnt*512+p->raw_angle;
}
long long Encoder_Timer_Overflow_left,Encoder_Timer_Overflow_right;
u32 Previous_Count_left,Previous_Count_right;
u32 ENCODER_PPR=100;
void NVIC_TimConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	//���� TIM5 �ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;					//	�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	// ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;				// ��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);
}


void	TIM5_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  // ʹ��TIM4ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period 		  = arr; 								//	����ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler	  = psc;   //	��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseInitStructure.TIM_CounterMode	  = TIM_CounterMode_Up; 				//	���ϼ���
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);	// ��ʼ��TIM4

	NVIC_TimConfig(); 	//�ж�����
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);  // ʹ�ܸ����ж�	
	TIM_Cmd(TIM5,ENABLE); 							// ʹ��TIM4
}

/******************************************************************************************************
*	�� �� ��: TIM2_GPIO_Config
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: ��ʼ��TIM2���������
*	˵    ��: 
*******************************************************************************************************/

void  TIM2_GPIO_Config	(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); 	//IO��ʱ������

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;  		// ����ģʽ 	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  	// ����
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;		// ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //	�ٶȵȼ�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; 
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);								// ��ʼ�� TIM2_CH1 ����
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2);		// ���ø���
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2);		// ���ø���
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM2);		// ���ø���
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM2);		// ���ø���
	
}

/******************************************************************************************************
*	�� �� ��: TIM2_PWMinit
*	��ڲ���: Period - ��ʱ������ֵ��frequency - ��ʱ������Ƶ��
*	�� �� ֵ: ��
*	��������: ��ʼ�� TIM2 ��������PWM������
*	˵    ��: ����TIM2������APB1ʱ�����ϣ�����TIM2��ʱ��Ƶ��Ϊ90M����ʱ���ķ�Ƶϵ���� 1-65536 ֮�䣬
*            ���Զ�ʱ����ʱ��Ƶ�ʷ�ΧΪ 90M - 1373 Hz����ʵ��Ӧ�������ȡ�ܱ�90M������Ƶ��ֵ��
*******************************************************************************************************/

void	TIM2_PWMinit(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM2_GPIO_Config(); 	//��ʼ��IO��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  //ʹ��ʱ��
	
	//��ʱ����������
	TIM_TimeBaseInitStructure.TIM_Period 			= arr; 								// ����ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler		= psc;  // ��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_CounterMode		= TIM_CounterMode_Up; 				// ���ϼ���
	TIM_TimeBaseInitStructure.TIM_ClockDivision	= TIM_CKD_DIV1; 						// �����˲�����ʱ�ӷ�Ƶ����
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);					// ��ʼ��TIM2��������

	//PWM�������
	TIM_OCInitStructure.TIM_OCMode 		= TIM_OCMode_PWM1;			// PWMģʽ1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//	ʹ�ܱȽ����
	TIM_OCInitStructure.TIM_OCPolarity 	= TIM_OCPolarity_High;		// С������ֵ����ߵ�ƽ
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  			//��ʼ����ʱ���Ƚ����ͨ�� 1
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //�Զ����رȽ����ͨ�� 1 ��ֵ

	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  			//��ʼ����ʱ���Ƚ����ͨ�� 1
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //�Զ����رȽ����ͨ�� 1 ��ֵ

	TIM_OC3Init(TIM2, &TIM_OCInitStructure);  			//��ʼ����ʱ���Ƚ����ͨ�� 1
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  //�Զ����رȽ����ͨ�� 1 ��ֵ

	TIM_OC4Init(TIM2, &TIM_OCInitStructure);  			//��ʼ����ʱ���Ƚ����ͨ�� 1
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  //�Զ����رȽ����ͨ�� 1 ��ֵ

	TIM_ARRPreloadConfig(TIM2,ENABLE);	//	ʹ���Զ�����
	TIM_Cmd(TIM2,ENABLE); 					//	ʹ��TIM2		
}

void TIM1_GPIO_Config(void)
{
  //PE 8 9 10 11 12 13 14���
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOE,&GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_TIM1);
}

//TIM1��PWM���
void Tim1_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
  //ʱ����ʼ��
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //���������á�
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //����������
  TIM_TimeBaseInitStructure.TIM_Prescaler = 1000-1;   //Timer clock = sysclock /(TIM_Prescaler+1) = 168M
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInitStructure.TIM_Period = 168 - 1;    //Period = (TIM counter clock / TIM output clock) - 1 = 20K
  TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);

 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
 

  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC2Init(TIM1,&TIM_OCInitStructure);
	TIM_OC3Init(TIM1,&TIM_OCInitStructure);
	TIM_OC4Init(TIM1,&TIM_OCInitStructure);
  TIM_Cmd(TIM1,ENABLE);
  TIM_CtrlPWMOutputs(TIM1,ENABLE);
}
void Encoder_Init_TIM4(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;    

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);    
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);  
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;          
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;              
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;                
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                  
  GPIO_Init(GPIOD, &GPIO_InitStructure);                          

  GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);           
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4);           
	
  TIM_TimeBaseStructure.TIM_Period = arr; 	                      
	TIM_TimeBaseStructure.TIM_Prescaler=psc;                        
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;       
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;           
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);                  
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;                  
  TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;	      
  TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;   
  TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;	            
  TIM_ICInitStructure.TIM_ICFilter =0;                            
  TIM_ICInit(TIM4,&TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_2;                  
  TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;	      
  TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;   
  TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;	            
  TIM_ICInitStructure.TIM_ICFilter=0;                             
  TIM_ICInit(TIM4,&TIM_ICInitStructure);
	
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising );
		
  NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;                   
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;      
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0x02;            
	NVIC_Init(&NVIC_InitStructure);                                 
		
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);                        
	TIM_Cmd(TIM4,ENABLE);   
}
long long right_cnt;
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET)                    
	{   
//		Encoder_measure(&right_m,right);
		Encoder_Timer_Overflow_right++;     		
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update); 
}


u32 Read_Encoder_right(void)
{
  u32 Count;                                                      
  u16 Current_Count;                                              
	u16 Enc_Timer_Overflow_one;	                                   

  Enc_Timer_Overflow_one=Encoder_Timer_Overflow_right;                  
  Current_Count = TIM_GetCounter(TIM4);                           
  Encoder_Timer_Overflow_right=0;                                       
	if((TIM4->CR1&0x0010) == 0x0010)                                
    Count = (u32)((-1*Enc_Timer_Overflow_one)*(4*ENCODER_PPR-4) + (Current_Count - Previous_Count_right));  
	else                                                            
		Count = (u32)(Current_Count - Previous_Count_right + (Enc_Timer_Overflow_one) * (4*ENCODER_PPR-4)); 
		     
  Previous_Count_right = Current_Count;  
  return(Count);
}
u32 Read_Encoder_left(void)
{
  u32 Count;                                                      
  u16 Current_Count;                                              
	u16 Enc_Timer_Overflow_one;	                                   

  Enc_Timer_Overflow_one=Encoder_Timer_Overflow_left;                  
  Current_Count = TIM_GetCounter(TIM3);                           
  Encoder_Timer_Overflow_left=0; 
	
	if((TIM3->CR1&0x0010) == 0x0010)                                
    Count = (u32)((-1*Enc_Timer_Overflow_one)*(4*ENCODER_PPR-4) + (Current_Count - Previous_Count_left));  
	else                                                            
		Count = (u32)(Current_Count - Previous_Count_left + (Enc_Timer_Overflow_one) * (4*ENCODER_PPR-4)); 
		     
  Previous_Count_left = Current_Count;  
  return(Count);
}
void Encoder_Init_TIM3(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;    

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6| GPIO_Pin_7;          
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;              
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;                
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                  
  GPIO_Init(GPIOA, &GPIO_InitStructure);                          

  GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3);           
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_TIM3);           
	
  TIM_TimeBaseStructure.TIM_Period = arr; 	                      
	TIM_TimeBaseStructure.TIM_Prescaler=psc;                        
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;       
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;           
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);                  
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;                  
  TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;	      
  TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;   
  TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;	            
  TIM_ICInitStructure.TIM_ICFilter =0;                            
  TIM_ICInit(TIM3,&TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_2;                  
  TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;	      
  TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;   
  TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;	            
  TIM_ICInitStructure.TIM_ICFilter=0;                             
  TIM_ICInit(TIM3,&TIM_ICInitStructure);
	
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising );
		
  NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;                   
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;      
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0x02;            
	NVIC_Init(&NVIC_InitStructure);                                 
		
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                        
	TIM_Cmd(TIM3,ENABLE);   
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)                    
	{   
		Encoder_Timer_Overflow_left++;     		
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  
}
int encode_right,encode_left;
float speed_right,speed_left;
int us_100=0;
void TIM5_IRQHandler(void)                         
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET)     
	{  
		us_100++;
		if(us_100>=10000)
		{
			us_100=0;
		}
		if(us_100%50==0)
		{
			encode_right=Read_Encoder_right();                          
			speed_right=(float)encode_right*0.1f;  
			encode_left=Read_Encoder_left();                          
			speed_left=(float)encode_left*0.1f;
			right_m.speed=speed_right;
			left_m.speed=speed_left;
		}
		if(us_100%500==0)
		{
//			printf("���:%.1f,%.1f\r\n",right_m.speed,left_m.speed);
//			printf("���:%lld,%lld\r\n",right_m.total_raw_angle,left_m.total_raw_angle);
		}
			Encoder_measure(&right_m,right);
			Encoder_measure(&left_m,left);	
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);       
}

//��ʼ��ADC															   
void  Adc_Init(void)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��

  //�ȳ�ʼ��ADC1ͨ��5 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA5 ͨ��5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
	
 
	ADC_Cmd(ADC1, ENABLE);//����ADת����	

}				  
//���ADCֵ
//ch: @ref ADC_channels 
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_16
//����ֵ:ת�����
u16 Get_Adc(u8 ch)   
{
	  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    
  
	ADC_SoftwareStartConv(ADC1);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}
//��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//ch:ͨ�����
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
	}
	return temp_val/times;
} 
