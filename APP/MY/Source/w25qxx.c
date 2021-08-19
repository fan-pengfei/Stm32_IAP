#include "w25qxx.h"

static inline void w25qxx_gpio_config(void)
{
    RCC_AHB1PeriphClockCmd(W25QXX_CS_RCC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = W25QXX_CS_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(W25QXX_CS_PORT, &GPIO_InitStruct);
}

void w25qxx_cs_init(void)
{
    w25qxx_gpio_config();
    W25QXX_CS(1);
}

void w25qxx_init(void)
{
    uint16_t id = w25qxx_read_id();
    printf("id = %2x\r\n", id);
    if (W25Q64 != id)
    {
        while (1)
        {
        }
    }
}

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q128
//容量为16M字节,共有128个Block,4096个Sector

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
uint8_t w25qxx_read_sr(void)
{
    uint8_t byte = 0;
    W25QXX_CS(0);                                  //使能器件
    spi_read_write_byte(SPI1, W25X_ReadStatusReg); //发送读取状态寄存器命令
    byte = spi_read_write_byte(SPI1, 0Xff);        //读取一个字节
    W25QXX_CS(1);                                  //取消片选
    return byte;
}

//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void w25qxx_write_sr(uint8_t sr)
{
    W25QXX_CS(0);                                   //使能器件
    spi_read_write_byte(SPI1, W25X_WriteStatusReg); //发送写取状态寄存器命令
    spi_read_write_byte(SPI1, sr);                  //写入一个字节
    W25QXX_CS(1);                                   //取消片选
}

//W25QXX写使能
//将WEL置位
void w25qxx_write_enable(void)
{
    W25QXX_CS(0);                                //使能器件
    spi_read_write_byte(SPI1, W25X_WriteEnable); //发送写使能
    W25QXX_CS(1);                                //取消片选
}

//W25QXX写禁止
//将WEL清零
void w25qxx_write_disable(void)
{
    W25QXX_CS(0);                                 //使能器件
    spi_read_write_byte(SPI1, W25X_WriteDisable); //发送写禁止指令
    W25QXX_CS(1);                                 //取消片选
}

//读取芯片ID
//返回值如下:
//0XEF13,表示芯片型号为W25Q80
//0XEF14,表示芯片型号为W25Q16
//0XEF15,表示芯片型号为W25Q32
//0XEF16,表示芯片型号为W25Q64
//0XEF17,表示芯片型号为W25Q128
uint16_t w25qxx_read_id(void)
{
    uint16_t temp = 0;
    W25QXX_CS(0);
    spi_read_write_byte(SPI1, 0x90); //发送读取ID命令
    spi_read_write_byte(SPI1, 0x00);
    spi_read_write_byte(SPI1, 0x00);
    spi_read_write_byte(SPI1, 0x00);
    temp |= spi_read_write_byte(SPI1, 0xFF) << 8;
    temp |= spi_read_write_byte(SPI1, 0xFF);
    W25QXX_CS(1);
    return temp;
}
//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void w25qxx_read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    uint16_t i;
    W25QXX_CS(0);                                           //使能器件
    spi_read_write_byte(SPI1, W25X_ReadData);               //发送读取命令
    spi_read_write_byte(SPI1, (uint8_t)((ReadAddr) >> 16)); //发送24bit地址
    spi_read_write_byte(SPI1, (uint8_t)((ReadAddr) >> 8));
    spi_read_write_byte(SPI1, (uint8_t)ReadAddr);
    for (i = 0; i < NumByteToRead; i++)
        pBuffer[i] = spi_read_write_byte(SPI1, 0XFF); //循环读数
    W25QXX_CS(1);
}
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
void w25qxx_write_page(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t i;
    w25qxx_write_enable();                                   //SET WEL
    W25QXX_CS(0);                                            //使能器件
    spi_read_write_byte(SPI1, W25X_PageProgram);             //发送写页命令
    spi_read_write_byte(SPI1, (uint8_t)((WriteAddr) >> 16)); //发送24bit地址
    spi_read_write_byte(SPI1, (uint8_t)((WriteAddr) >> 8));
    spi_read_write_byte(SPI1, (uint8_t)WriteAddr);
    for (i = 0; i < NumByteToWrite; i++)
        spi_read_write_byte(SPI1, pBuffer[i]); //循环写数
    W25QXX_CS(1);                              //取消片选
    w25qxx_wait_busy();                        //等待写入结束
}
//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void w25qxx_write_nocheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t pageremain;
    pageremain = 256 - WriteAddr % 256; //单页剩余的字节数
    if (NumByteToWrite <= pageremain)
        pageremain = NumByteToWrite; //不大于256个字节
    while (1)
    {
        w25qxx_write_page(pBuffer, WriteAddr, pageremain);
        if (NumByteToWrite == pageremain)
            break; //写入结束了
        else       //NumByteToWrite>pageremain
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;

            NumByteToWrite -= pageremain; //减去已经写入了的字节数
            if (NumByteToWrite > 256)
                pageremain = 256; //一次可以写入256个字节
            else
                pageremain = NumByteToWrite; //不够256个字节了
        }
    };
}
//写SPI FLASH
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
uint8_t W25QXX_BUFFER[4096];
void w25qxx_write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t *W25QXX_BUF;
    W25QXX_BUF = W25QXX_BUFFER;
    secpos = WriteAddr / 4096; //扇区地址
    secoff = WriteAddr % 4096; //在扇区内的偏移
    secremain = 4096 - secoff; //扇区剩余空间大小
    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
    if (NumByteToWrite <= secremain)
        secremain = NumByteToWrite; //不大于4096个字节
    while (1)
    {
        w25qxx_read(W25QXX_BUF, secpos * 4096, 4096); //读出整个扇区的内容
        for (i = 0; i < secremain; i++)               //校验数据
        {
            if (W25QXX_BUF[secoff + i] != 0XFF)
                break; //需要擦除
        }
        if (i < secremain) //需要擦除
        {
            w25qxx_erase_sector(secpos);    //擦除这个扇区
            for (i = 0; i < secremain; i++) //复制
                W25QXX_BUF[i + secoff] = pBuffer[i];
            w25qxx_write_nocheck(W25QXX_BUF, secpos * 4096, 4096); //写入整个扇区
        }
        else
            w25qxx_write_nocheck(pBuffer, WriteAddr, secremain); //写已经擦除了的,直接写入扇区剩余区间.
        if (NumByteToWrite == secremain)
            break; //写入结束了
        else       //写入未结束
        {
            secpos++;   //扇区地址增1
            secoff = 0; //偏移位置为0

            pBuffer += secremain;        //指针偏移
            WriteAddr += secremain;      //写地址偏移
            NumByteToWrite -= secremain; //字节数递减
            if (NumByteToWrite > 4096)
                secremain = 4096; //下一个扇区还是写不完
            else
                secremain = NumByteToWrite; //下一个扇区可以写完了
        }
    };
}
//擦除整个芯片
//等待时间超长...
void w25qxx_erase_chip(void)
{
    w25qxx_write_enable(); //SET WEL
    w25qxx_wait_busy();
    W25QXX_CS(0);                              //使能器件
    spi_read_write_byte(SPI1, W25X_ChipErase); //发送片擦除命令
    W25QXX_CS(1);                              //取消片选
    w25qxx_wait_busy();                        //等待芯片擦除结束
}
//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void w25qxx_erase_sector(uint32_t Dst_Addr)
{
    //监视falsh擦除情况,测试用
    //printf("fe:%x\r\n", Dst_Addr);
    Dst_Addr *= 4096;
    w25qxx_write_enable(); //SET WEL
    w25qxx_wait_busy();
    W25QXX_CS(0);                                           //使能器件
    spi_read_write_byte(SPI1, W25X_SectorErase);            //发送扇区擦除指令
    spi_read_write_byte(SPI1, (uint8_t)((Dst_Addr) >> 16)); //发送24bit地址
    spi_read_write_byte(SPI1, (uint8_t)((Dst_Addr) >> 8));
    spi_read_write_byte(SPI1, (uint8_t)Dst_Addr);
    W25QXX_CS(1);       //取消片选
    w25qxx_wait_busy(); //等待擦除完成
}
//等待空闲
void w25qxx_wait_busy(void)
{
    while ((w25qxx_read_sr() & 0x01) == 0x01); // 等待BUSY位清空
}
//进入掉电模式
void w25qxx_powerdown(void)
{
    W25QXX_CS(0);                              //使能器件
    spi_read_write_byte(SPI1, W25X_PowerDown); //发送掉电命令
    W25QXX_CS(1);                              //取消片选
    SPI_DELAY(3);                               //等待TPD
}
//唤醒
void w25qxx_wakeup(void)
{
    W25QXX_CS(0);                                     //使能器件
    spi_read_write_byte(SPI1, W25X_ReleasePowerDown); //  send W25X_PowerDown command 0xAB
    W25QXX_CS(1);                                     //取消片选
    SPI_DELAY(3);                                      //等待TRES1
}
