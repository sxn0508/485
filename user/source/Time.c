
#include "Time.h"

bool blWordsCounterStatus = false; //Ĭ�ϲ���ʱ
bool blFramesCounterStatus = false;
uint16_t uwTime2WordsCounter = 0;  //����ֵĬ��Ϊ0
uint16_t uwTime2FramesCounter = 0; //����ֵĬ��Ϊ0

/********************************************************************
* ��    �ܣ���ʱ����ʼ������
* ��    �룺None
* ��    ����None
*           
* �� д �ˣ�
* ��д���ڣ�2016.8.7
**********************************************************************/
void vTimer_Init(void)
{
    /*��ʱ��2����֡��ʱ�ж�*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruc;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    vIRQ_TIME_Config();

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    //TIM_TimeBaseInitStruc.TIM_Period = (1000 - 1); //����1000ʱ�����1ms
    TIM_TimeBaseInitStruc.TIM_Period = (50000 - 1); //����50000ʱ�����50ms __2017-3-21
    //TIM_TimeBaseInitStruc.TIM_Prescaler=16;//ϵͳƵ��16M��16��Ƶ��1Mhz��ʱƵ�ʣ�
    TIM_TimeBaseInitStruc.TIM_Prescaler = 64;                   //ϵͳƵ��64M��64��Ƶ��1Mhz��ʱƵ�ʣ�
    TIM_TimeBaseInitStruc.TIM_ClockDivision = 0;                //�����˲������ܹر�
    TIM_TimeBaseInitStruc.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
                                                                //	TIM_TimeBaseInitStruc.TIM_RepetitionCounter �����Զ���װ����TIM15, TIM16 and TIM17��Ч
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruc);
    TIM_Cmd(TIM2, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    /*��ʱ��5��������PWM��38KHZ���*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    TIM_TimeBaseInitStruc.TIM_Period = SystemCoreClock / 38400 - 1;
    TIM_TimeBaseInitStruc.TIM_Prescaler = 0;                    //ϵͳƵ��64M
    TIM_TimeBaseInitStruc.TIM_ClockDivision = 0;                //�����˲������ܹر�
    TIM_TimeBaseInitStruc.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruc);
    TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Disable);
    TIM_Cmd(TIM5, ENABLE); //ʹ�ܶ�ʱ��5
    //PWM����
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = uwCCR2_Val;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM5, &TIM_OCInitStructure);
    //TIM_ITConfig(TIM5, TIM_IT_CC2, ENABLE);//ʹ�ܶ�ʱ��5ͨ��2�ж�
    TIM_CCxCmd(TIM5, TIM_Channel_2, TIM_CCx_Enable);
}

/********************************************************************
* ��    �ܣ���ʱ���ж����ú���
* ��    �룺None
* ��    ����None
*           
* �� д �ˣ�
* ��д���ڣ�2016.8.7
**********************************************************************/
static void vIRQ_TIME_Config(void)
{
    /*��ʱ��2�ж�����*/
    NVIC_InitTypeDef NVIC_Initstruc;
    NVIC_Initstruc.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_Initstruc.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Initstruc.NVIC_IRQChannelSubPriority = 1;
    NVIC_Initstruc.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Initstruc);

    /*��ʱ��5�ж����ȼ�����*/
    NVIC_Initstruc.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_Initstruc.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Initstruc.NVIC_IRQChannelSubPriority = 0;
    NVIC_Initstruc.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Initstruc);
}

/********************************************************************
* ��    �ܣ���ʱ��2�Ƿ�ʱ
* ��    �룺None
* ��    ����true/false
*           
* �� д �ˣ�
* ��д���ڣ�2016.8.7
**********************************************************************/
bool blTime2_Update(uint16_t counter, uint16_t ms)
{
    if (counter >= ms)
        return true;
    else
        return false;
}
