#ifndef _MYIIC_H
#define _MYIIC_H
#include "stm32f4xx.h"

//IO��������
#define SDA_IN()  {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=0<<12*2;}	//PH5����ģʽ
#define SDA_OUT() {GPIOB->MODER&=~(3<<(12*2));GPIOB->MODER|=1<<12*2;} //PH5���ģʽ
//IO����
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
#define IIC_SCL   BIT_ADDR(GPIOB_ODR_Addr,13) //SCL
#define IIC_SDA   BIT_ADDR(GPIOB_ODR_Addr,12) //SDA 
#define READ_SDA  BIT_ADDR(GPIOB_IDR_Addr,12)  //����SDA
#define I2C_INFO(fmt,arg...)           printf("<<-I2C-INFO->> "fmt"\n",##arg)
#define I2C_ERROR(fmt,arg...)          printf("<<-I2C-ERROR->> "fmt"\n",##arg)

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 
#endif

