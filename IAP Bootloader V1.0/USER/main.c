#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"   
#include "stmflash.h" 
#include "iap.h"   
#include "crc.h"
#include "iwdg.h"
u32 crc32_temp=0;
int main(void)
{ 
	u8 crc32_true=0;   
	u32 i;
	u16 oldcount=0;	//�ϵĴ��ڽ�������ֵ
	u16 applenth=0;	//���յ���app���볤��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();					//��ʼ��LED 
	IWDG_Config(IWDG_Prescaler_64 ,325); 
	printf("*****�ȴ�����bin�ļ�!*****!\r\n");
	//��ʾ��ʾ��Ϣ
	while(1)
	{
		IWDG_Feed();//ι��
		GPIO_ToggleBits(GPIOC,GPIO_Pin_13);
	 	if(USART_RX_CNT)//�յ�������
		{
			if(oldcount==USART_RX_CNT)//��������,û���յ��κ�����,��Ϊ�������ݽ������.
			{
				applenth=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("�û�����������!\r\n");
				printf("���볤��:%dBytes\r\n",applenth);
				crc32_temp=USART_RX_BUF[0x1c]*256*256*256+USART_RX_BUF[0x1c+1]*256*256+USART_RX_BUF[0x1c+2]*256+USART_RX_BUF[0x1c+3];//��ȡcrc32��ֵ
				for(i=0;i<4;i++)//���ԭcrc32
				{
					USART_RX_BUF[0x1c+i]=0x00;
				}
				if(crc32_temp==crc32(USART_RX_BUF,applenth))//�ж�CRC32У��ֵ�Ƿ����
				{
					crc32_true=1;
//                    for(i=0x1c;i<applenth-4;i++)
//                    {
//                        USART_RX_BUF[i]=USART_RX_BUF[i+4];
//                    }
//                    applenth=applenth-4;
					printf("%x,%x\r\nCRCУ��ɹ���\r\n",crc32_temp,crc32(USART_RX_BUF,applenth));
				}
                else//������ȣ����µȴ��µ�bin�ļ�
                {
                    printf("CRC32 fail��\r\n���������س���\r\n");
					printf("*****�ȴ�����bin�ļ�!*****\r\n");
                    USART_RX_CNT=0;
                    crc32_true=0;
                 }
			}
			else 
			{
				oldcount=USART_RX_CNT;
			}			
		}
		delay_ms(70);  	 
		if(crc32_true)	//������ȷ
		{
			if(applenth)//bin�ļ���Ϊ��
			{
				printf("��ʼ���¹̼�...\r\n");	
 				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
				{	  
					iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//����FLASH����   
					printf("�̼��������!\r\n");	
					printf("��ʼִ��FLASH�û�����!!\r\n");
					if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
					{	 
						iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
						crc32_true=0;
					}
					else 
					{
						printf("��FLASHӦ�ó���,�޷�ִ��!\r\n");	   
					}	
				}
				else 
				{ 
					printf("��FLASHӦ�ó���!\r\n");
				}
 			}
			else 
			{
				printf("û�п��Ը��µĹ̼�!\r\n");
			}
		}
	}  
}

