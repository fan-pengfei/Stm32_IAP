#ifndef __SPI_H
#define __SPI_H
#include "stm32f4xx.h"

void spi1_init(void);
void spi_set_speed(SPI_TypeDef *SPIx, uint16_t SPI_BaudRatePrescaler);
uint8_t spi_read_write_byte(SPI_TypeDef* spix, uint8_t tx_data);

#endif
