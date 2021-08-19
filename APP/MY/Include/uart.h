#ifndef __UART_H
#define __UART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
#define EN_USART6_RX 			1		//使能（1）/禁止（0）串口1接收  	
typedef struct
{
	float a[3],w[3],angle[3];
	short Tem;
}mpu6050;
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern u8  USART6_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART6_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void uart3_init(u32 bound);
void uart6_init(u32 bound);
void usart2_init(void);
void get_data_mpu(void);
#endif

