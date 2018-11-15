
#include "Time.h"

bool blWordsCounterStatus = false; //默认不计时
bool blFramesCounterStatus = false;
uint16_t uwTime2WordsCounter = 0;  //计数值默认为0
uint16_t uwTime2FramesCounter = 0; //计数值默认为0

/********************************************************************
* 功    能：计时器初始化函数
* 输    入：None
* 输    出：None
*           
* 编 写 人：
* 编写日期：2016.8.7
**********************************************************************/
void vTimer_Init(void)
{
    /*定时器2用作帧超时判断*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruc;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    vIRQ_TIME_Config();

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    //TIM_TimeBaseInitStruc.TIM_Period = (1000 - 1); //计数1000时溢出，1ms
    TIM_TimeBaseInitStruc.TIM_Period = (50000 - 1); //计数50000时溢出，50ms __2017-3-21
    //TIM_TimeBaseInitStruc.TIM_Prescaler=16;//系统频率16M，16分频，1Mhz计时频率；
    TIM_TimeBaseInitStruc.TIM_Prescaler = 64;                   //系统频率64M，64分频，1Mhz计时频率；
    TIM_TimeBaseInitStruc.TIM_ClockDivision = 0;                //数字滤波器功能关闭
    TIM_TimeBaseInitStruc.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
                                                                //	TIM_TimeBaseInitStruc.TIM_RepetitionCounter 向下自动填装，对TIM15, TIM16 and TIM17有效
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruc);
    TIM_Cmd(TIM2, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    /*定时器5用作红外PWM波38KHZ输出*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    TIM_TimeBaseInitStruc.TIM_Period = SystemCoreClock / 38400 - 1;
    TIM_TimeBaseInitStruc.TIM_Prescaler = 0;                    //系统频率64M
    TIM_TimeBaseInitStruc.TIM_ClockDivision = 0;                //数字滤波器功能关闭
    TIM_TimeBaseInitStruc.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruc);
    TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Disable);
    TIM_Cmd(TIM5, ENABLE); //使能定时器5
    //PWM配置
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = uwCCR2_Val;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM5, &TIM_OCInitStructure);
    //TIM_ITConfig(TIM5, TIM_IT_CC2, ENABLE);//使能定时器5通道2中断
    TIM_CCxCmd(TIM5, TIM_Channel_2, TIM_CCx_Enable);
}

/********************************************************************
* 功    能：计时器中断配置函数
* 输    入：None
* 输    出：None
*           
* 编 写 人：
* 编写日期：2016.8.7
**********************************************************************/
static void vIRQ_TIME_Config(void)
{
    /*定时器2中断配置*/
    NVIC_InitTypeDef NVIC_Initstruc;
    NVIC_Initstruc.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_Initstruc.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Initstruc.NVIC_IRQChannelSubPriority = 1;
    NVIC_Initstruc.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Initstruc);

    /*定时器5中断优先级配置*/
    NVIC_Initstruc.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_Initstruc.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Initstruc.NVIC_IRQChannelSubPriority = 0;
    NVIC_Initstruc.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Initstruc);
}

/********************************************************************
* 功    能：计时器2是否超时
* 输    入：None
* 输    出：true/false
*           
* 编 写 人：
* 编写日期：2016.8.7
**********************************************************************/
bool blTime2_Update(uint16_t counter, uint16_t ms)
{
    if (counter >= ms)
        return true;
    else
        return false;
}
