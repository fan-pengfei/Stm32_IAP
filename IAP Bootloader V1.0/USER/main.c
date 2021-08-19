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
	u16 oldcount=0;	//老的串口接收数据值
	u16 applenth=0;	//接收到的app代码长度
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED 
	IWDG_Config(IWDG_Prescaler_64 ,325); 
	printf("*****等待接受bin文件!*****!\r\n");
	//显示提示信息
	while(1)
	{
		IWDG_Feed();//喂狗
		GPIO_ToggleBits(GPIOC,GPIO_Pin_13);
	 	if(USART_RX_CNT)//收到了数据
		{
			if(oldcount==USART_RX_CNT)//新周期内,没有收到任何数据,认为本次数据接收完成.
			{
				applenth=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("用户程序接收完成!\r\n");
				printf("代码长度:%dBytes\r\n",applenth);
				crc32_temp=USART_RX_BUF[0x1c]*256*256*256+USART_RX_BUF[0x1c+1]*256*256+USART_RX_BUF[0x1c+2]*256+USART_RX_BUF[0x1c+3];//读取crc32的值
				for(i=0;i<4;i++)//清空原crc32
				{
					USART_RX_BUF[0x1c+i]=0x00;
				}
				if(crc32_temp==crc32(USART_RX_BUF,applenth))//判断CRC32校验值是否相等
				{
					crc32_true=1;
//                    for(i=0x1c;i<applenth-4;i++)
//                    {
//                        USART_RX_BUF[i]=USART_RX_BUF[i+4];
//                    }
//                    applenth=applenth-4;
					printf("%x,%x\r\nCRC校验成功！\r\n",crc32_temp,crc32(USART_RX_BUF,applenth));
				}
                else//若不相等，重新等待新的bin文件
                {
                    printf("CRC32 fail！\r\n请重新下载程序！\r\n");
					printf("*****等待接受bin文件!*****\r\n");
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
		if(crc32_true)	//检验正确
		{
			if(applenth)//bin文件不为空
			{
				printf("开始更新固件...\r\n");	
 				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
				{	  
					iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//更新FLASH代码   
					printf("固件更新完成!\r\n");	
					printf("开始执行FLASH用户代码!!\r\n");
					if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
					{	 
						iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
						crc32_true=0;
					}
					else 
					{
						printf("非FLASH应用程序,无法执行!\r\n");	   
					}	
				}
				else 
				{ 
					printf("非FLASH应用程序!\r\n");
				}
 			}
			else 
			{
				printf("没有可以更新的固件!\r\n");
			}
		}
	}  
}

