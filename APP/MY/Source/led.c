#include "led.h"

static inline void led_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void led_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    led_gpio_config();
    led_switch(off);
}

void led_switch(Led_Switch status)
{
    if(on == status)
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
    else if(off == status)
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
}
