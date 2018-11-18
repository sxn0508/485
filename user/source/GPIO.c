
#include "GPIO.h"
#include "Time.h"

//bool LED_BLINK;
/**
  * @brief GPIO��ʼ��������
  * @param  none
  * @retval none
	* B6��B7����Ϊ����1��B8��B9����Ϊ����3��A1���Ź������A0����Ϊι����PC13����Ϊ���еƣ�
  */
void vGPIO_Init(void)
{
    /*PB6 PB7��Ϊ����*/
    GPIO_InitTypeDef GPIO_Initstruc;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_7);
    GPIO_Initstruc.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Initstruc.GPIO_OType = GPIO_OType_PP;
    GPIO_Initstruc.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Initstruc.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Initstruc.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_Initstruc);
    /*PB8 PB9��Ϊ����*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_7);
    GPIO_Initstruc.GPIO_OType = GPIO_OType_PP;
    GPIO_Initstruc.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_Initstruc);
    /*PA0 ��Ϊ���Ź����*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_Initstruc.GPIO_Pin = GPIO_Pin_0;
    GPIO_Initstruc.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Initstruc.GPIO_OType = GPIO_OType_OD; //��©
    GPIO_Initstruc.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Initstruc.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_Initstruc);
    /*PC13 ��Ϊָʾ�ƣ�Ĭ�ϲ���*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    GPIO_Initstruc.GPIO_Pin = GPIO_Pin_13;
    GPIO_Initstruc.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Initstruc.GPIO_OType = GPIO_OType_PP; //��©
    GPIO_Initstruc.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Initstruc.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Initstruc.GPIO_PuPd = GPIO_PuPd_DOWN; //����Ϊ����
    GPIO_Init(GPIOC, &GPIO_Initstruc);
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET); //����ߵ�ƽ��LED��
    /*PA1_����ΪPWM���*/
    GPIO_Initstruc.GPIO_Pin = GPIO_Pin_1;
    GPIO_Initstruc.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Initstruc.GPIO_OType = GPIO_OType_PP;
    GPIO_Initstruc.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Initstruc.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_Initstruc);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_2); //���ù���:TIM5
    /*PA2 PA3��Ϊ����2������*/
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);
    GPIO_Initstruc.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Initstruc.GPIO_OType = GPIO_OType_PP;
    GPIO_Initstruc.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Initstruc.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Initstruc.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_Initstruc);
}

/********************************************************************
* ��    �ܣ��ⲿ�жϳ�ʼ��
* ��    �룺void
* ��    ����void
* ˵    ����PB7����1����
* �� д �ˣ�stragen
* ��д���ڣ�2018��9��4��15:47:13
**********************************************************************/
void vExti_Init(void)
{
    GPIO_InitTypeDef GPIOInitStruct;
    EXTI_InitTypeDef ExtiInitStruct;
    NVIC_InitTypeDef NvicInitStruct;

    /*����GPIO����*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIOInitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIOInitStruct.GPIO_Mode = GPIO_Mode_IN;
    //GPIOInitStruct.GPIO_OType = GPIO_OType_PP;
    GPIOInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIOInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIOInitStruct);

    /*�����ⲿ�ж�ģʽ*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource7);
    ExtiInitStruct.EXTI_Line = EXTI_Line7;
    ExtiInitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    ExtiInitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    ExtiInitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&ExtiInitStruct);

    /*�����ж����ȼ�*/
    NvicInitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    NvicInitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NvicInitStruct.NVIC_IRQChannelSubPriority = 1;
    NvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NvicInitStruct);
}
/********************************************************************
* ��    �ܣ�ι������
* ��    �룺void
* ��    ����	void
* �� д �ˣ�������
* ��д���ڣ�2017.3.21
**********************************************************************/
void vFeedExtWatchDog(void)
{
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
    }
    else
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
    }
}
/********************************************************************
* ��    �ܣ�����ָʾ��
* ��    �룺void
* ��    ����	void
* �� д �ˣ�stragen
* ��д���ڣ�2017.3.21
**********************************************************************/
void vLed_Light(void)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET); //����PC13
}
