#ifndef TIME_H
#define TIME_H

#include "stm32f37x.h"
#include "BaseDef.h"

/*PWM����*/
//#define uwCCR2_Val  1684   // 64mhz/1684=38khz
#define uwCCR2_Val 842 // 64mhz/1684=38khz

extern bool blWordsCounterStatus;     //�ֽڼ�����ʱ������
extern bool blFramesCounterStatus;    //��֡�����ʱ������
extern uint16_t uwTime2WordsCounter;  //time2��ʱ������ֵ,������¼�ֽڼ䳬ʱ
extern uint16_t uwTime2FramesCounter; //time2��ʱ������ֵ,������¼֡�䳬ʱ

//extern uint32_t ticks; //SysTick������������ָʾ����˸

extern uint32_t TICKS;
extern uint32_t gSystickAccuracy; //1���ڵ�SysTick��
#define TICK_1US (gSystickAccuracy / 1000000)
#define TICK_1MS (gSystickAccuracy / 1000)
#define TICK_500MS (gSystickAccuracy / 2)
#define TICK_200MS (gSystickAccuracy / 5)
#define TICK_1S (gSystickAccuracy)
extern void vSystickSetAccuracy(uint32_t accuracy);
extern void vSystickIntCmd(FunctionalState value);

static void vIRQ_TIME_Config(void); //��ʱ���ն�����

extern bool blTime2_Update(uint16_t counter, uint16_t ms); //��ʱ��2�Ƿ�ʱ
extern void vTimer_Init(void);                             //��ʱ����ʼ��

#endif
