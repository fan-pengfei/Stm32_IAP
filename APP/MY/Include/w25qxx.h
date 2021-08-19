#ifndef __W25QXX_H
#define __W25QXX_H
#include "stm32f4xx.h"
#include "delay.h"
#include "uart.h"
#include "spi.h"

//W25X系列/Q系列芯片列表
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16
//W25Q128 ID  0XEF17
#define W25Q80 0XEF13
#define W25Q16 0XEF14
#define W25Q32 0XEF15
#define W25Q64 0XEF16
#define W25Q128 0XEF17

//////////////////////////////////////////////////////////////////////////////////
//指令表
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg 0x05
#define W25X_WriteStatusReg 0x01
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_PageProgram 0x02
#define W25X_BlockErase 0xD8
#define W25X_SectorErase 0x20
#define W25X_ChipErase 0xC7
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F

///////////////////////////////////////////////////////////////////////////////////
//引脚定义
#define W25QXX_CS_RCC RCC_AHB1Periph_GPIOA
#define W25QXX_CS_PORT GPIOA
#define W25QXX_CS_PIN GPIO_Pin_4
#define W25QXX_CS(n) GPIO_WriteBit(W25QXX_CS_PORT, W25QXX_CS_PIN, (BitAction)n)

#define SPI_DELAY(n) delay_mus(n)

void w25qxx_cs_init(void);
void w25qxx_init(void);
uint16_t w25qxx_read_id(void);     //读取FLASH ID
uint8_t w25qxx_read_sr(void);      //读取状态寄存器
void w25qxx_write_sr(uint8_t sr); //写状态寄存器
void w25qxx_write_enable(void);   //写使能
void w25qxx_write_disable(void);  //写保护
void w25qxx_write_nocheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void w25qxx_read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);    //读取flash
void w25qxx_write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite); //写入flash
void w25qxx_erase_chip(void);                                                     //整片擦除
void w25qxx_erase_sector(uint32_t Dst_Addr);                                      //扇区擦除
void w25qxx_wait_busy(void);                                                      //等待空闲
void w25qxx_powerdown(void);                                                      //进入掉电模式
void w25qxx_wakeup(void);                                                         //唤醒

#endif
