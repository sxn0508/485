#ifndef GPIO_H
#define GPIO_H

#include "stm32f37x.h"
#include "BaseDef.h"

extern void		vGPIO_Init(void);
extern void		vFeedExtWatchDog(void);
extern void		vLed_Light(void);

#endif

