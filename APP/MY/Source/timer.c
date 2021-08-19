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
	//配置 TIM5 中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;					//	中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	// 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;				// 响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// 使能中断
	NVIC_Init(&NVIC_InitStructure);
}


void	TIM5_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  // 使能TIM4时钟
	
	TIM_TimeBaseInitStructure.TIM_Period 		  = arr; 								//	重载值
	TIM_TimeBaseInitStructure.TIM_Prescaler	  = psc;   //	分频系数
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseInitStructure.TIM_CounterMode	  = TIM_CounterMode_Up; 				//	向上计数
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);	// 初始化TIM4

	NVIC_TimConfig(); 	//中断配置
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);  // 使能更新中断	
	TIM_Cmd(TIM5,ENABLE); 							// 使能TIM4
}

/******************************************************************************************************
*	函 数 名: TIM2_GPIO_Config
*	入口参数: 无
*	返 回 值: 无
*	函数功能: 初始化TIM2的相关引脚
*	说    明: 
*******************************************************************************************************/

void  TIM2_GPIO_Config	(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); 	//IO口时钟配置

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;  		// 复用模式 	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  	// 推挽
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;		// 上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //	速度等级
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; 
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);								// 初始化 TIM2_CH1 引脚
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2);		// 设置复用
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2);		// 设置复用
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM2);		// 设置复用
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM2);		// 设置复用
	
}

/******************************************************************************************************
*	函 数 名: TIM2_PWMinit
*	入口参数: Period - 定时器重载值，frequency - 定时器计数频率
*	返 回 值: 无
*	函数功能: 初始化 TIM2 ，并进行PWM的设置
*	说    明: 由于TIM2挂载在APB1时钟线上，所以TIM2的时钟频率为90M，定时器的分频系数在 1-65536 之间，
*            所以定时器的时钟频率范围为 90M - 1373 Hz，在实际应用中最好取能被90M整除的频率值。
*******************************************************************************************************/

void	TIM2_PWMinit(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM2_GPIO_Config(); 	//初始化IO口
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  //使能时钟
	
	//定时器基本设置
	TIM_TimeBaseInitStructure.TIM_Period 			= arr; 								// 重载值
	TIM_TimeBaseInitStructure.TIM_Prescaler		= psc;  // 分频系数
	TIM_TimeBaseInitStructure.TIM_CounterMode		= TIM_CounterMode_Up; 				// 向上计数
	TIM_TimeBaseInitStructure.TIM_ClockDivision	= TIM_CKD_DIV1; 						// 数字滤波采样时钟分频设置
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);					// 初始化TIM2基本设置

	//PWM输出配置
	TIM_OCInitStructure.TIM_OCMode 		= TIM_OCMode_PWM1;			// PWM模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//	使能比较输出
	TIM_OCInitStructure.TIM_OCPolarity 	= TIM_OCPolarity_High;		// 小于跳变值输出高电平
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  			//初始化定时器比较输出通道 1
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //自动重载比较输出通道 1 的值

	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  			//初始化定时器比较输出通道 1
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //自动重载比较输出通道 1 的值

	TIM_OC3Init(TIM2, &TIM_OCInitStructure);  			//初始化定时器比较输出通道 1
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  //自动重载比较输出通道 1 的值

	TIM_OC4Init(TIM2, &TIM_OCInitStructure);  			//初始化定时器比较输出通道 1
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  //自动重载比较输出通道 1 的值

	TIM_ARRPreloadConfig(TIM2,ENABLE);	//	使能自动重载
	TIM_Cmd(TIM2,ENABLE); 					//	使能TIM2		
}

void TIM1_GPIO_Config(void)
{
  //PE 8 9 10 11 12 13 14输出
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

//TIM1做PWM输出
void Tim1_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
  //时基初始化
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //死区控制用。
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数器方向
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
//			printf("电机:%.1f,%.1f\r\n",right_m.speed,left_m.speed);
//			printf("电机:%lld,%lld\r\n",right_m.total_raw_angle,left_m.total_raw_angle);
		}
			Encoder_measure(&right_m,right);
			Encoder_measure(&left_m,left);	
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);       
}

//初始化ADC															   
void  Adc_Init(void)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

  //先初始化ADC1通道5 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA5 通道5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	
 
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器	

}				  
//获得ADC值
//ch: @ref ADC_channels 
//通道值 0~16取值范围为：ADC_Channel_0~ADC_Channel_16
//返回值:转换结果
u16 Get_Adc(u8 ch)   
{
	  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
  
	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}
//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
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
