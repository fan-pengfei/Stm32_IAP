#ifndef __LED_H
#define __LED_H
#include "stm32f4xx.h"

typedef enum
{
    on, 
    off
} Led_Switch;

void led_init(void);
void led_switch(Led_Switch status);

#endif
