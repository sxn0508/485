#ifndef GPIO_H
#define GPIO_H

#include "stm32f37x.h"
#include "BaseDef.h"
#include "Time.h"

//extern bool LED_BLINK;
extern void vGPIO_Init(void);
extern void vFeedExtWatchDog(void);
extern void vLed_Light(void);
extern void vExti_Init(void);
#endif
