#ifndef TIME_H
#define TIME_H

#include "stm32f37x.h"
#include "BaseDef.h"

/*PWM参数*/
//#define uwCCR2_Val  1684   // 64mhz/1684=38khz
#define uwCCR2_Val 842 // 64mhz/1684=38khz

extern bool blWordsCounterStatus;     //字节间间隔定时器开关
extern bool blFramesCounterStatus;    //回帧间隔定时器开关
extern uint16_t uwTime2WordsCounter;  //time2定时器计数值,用来记录字节间超时
extern uint16_t uwTime2FramesCounter; //time2定时器计数值,用来记录帧间超时

//extern uint32_t ticks; //SysTick计数器，用于指示灯闪烁

extern uint32_t TICKS;
extern uint32_t gSystickAccuracy; //1秒内的SysTick数
#define TICK_1US (gSystickAccuracy / 1000000)
#define TICK_1MS (gSystickAccuracy / 1000)
#define TICK_500MS (gSystickAccuracy / 2)
#define TICK_200MS (gSystickAccuracy / 5)
#define TICK_1S (gSystickAccuracy)
extern void vSystickSetAccuracy(uint32_t accuracy);
extern void vSystickIntCmd(FunctionalState value);

static void vIRQ_TIME_Config(void); //定时器终端配置

extern bool blTime2_Update(uint16_t counter, uint16_t ms); //定时器2是否超时
extern void vTimer_Init(void);                             //定时器初始化

#endif
