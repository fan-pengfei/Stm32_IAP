#ifndef __UART_H
#define __UART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART6_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����  	
typedef struct
{
	float a[3],w[3],angle[3];
	short Tem;
}mpu6050;
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
extern u8  USART6_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART6_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void uart3_init(u32 bound);
void uart6_init(u32 bound);
void usart2_init(void);
void get_data_mpu(void);
#endif

