
#include "GPIO.h"
#include "Time.h"


/**
  * @brief GPIO初始化函数；
  * @param  none
  * @retval none
	* B6和B7配置为串口1；B8和B9配置为串口3；A1看门狗输出；A0配置为喂狗；PC13配置为运行灯；
  */
void vGPIO_Init(void)
{
	/*PB6 PB7作为串口*/
	GPIO_InitTypeDef GPIO_Initstruc;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_7);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_7);
	GPIO_Initstruc.GPIO_Mode=GPIO_Mode_AF;
	GPIO_Initstruc.GPIO_OType=GPIO_OType_PP;
	GPIO_Initstruc.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Initstruc.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Initstruc.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOB,&GPIO_Initstruc);
	/*PB8 PB9作为串口*/
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_7);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_7);
	GPIO_Initstruc.GPIO_OType=GPIO_OType_PP;	
	GPIO_Initstruc.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;	
	GPIO_Init(GPIOB,&GPIO_Initstruc);
	/*PA0 作为看门狗输出*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	GPIO_Initstruc.GPIO_Pin=GPIO_Pin_0;
	GPIO_Initstruc.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_Initstruc.GPIO_OType=GPIO_OType_OD;//开漏
	GPIO_Initstruc.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Initstruc.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_Initstruc);
	/*PC13 作为指示灯，默认不亮*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
	GPIO_Initstruc.GPIO_Pin=GPIO_Pin_13;
	GPIO_Initstruc.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_Initstruc.GPIO_OType=GPIO_OType_OD;//开漏
	GPIO_Initstruc.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Initstruc.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Initstruc.GPIO_PuPd=GPIO_PuPd_DOWN;//配置为下拉
	GPIO_Init(GPIOC,&GPIO_Initstruc);
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);//输出高电平，LED灭
	/*PA1_配置为PWM输出*/
	GPIO_Initstruc.GPIO_Pin=GPIO_Pin_1;
	GPIO_Initstruc.GPIO_Mode=GPIO_Mode_AF;
	GPIO_Initstruc.GPIO_OType=GPIO_OType_PP;
	GPIO_Initstruc.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Initstruc.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_Initstruc);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_2);//复用功能:TIM5
	/*PA2 PA3作为串口2发、收*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_7);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_7);
	GPIO_Initstruc.GPIO_Mode=GPIO_Mode_AF;
	GPIO_Initstruc.GPIO_OType=GPIO_OType_PP;
	GPIO_Initstruc.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Initstruc.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Initstruc.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3;
	GPIO_Init(GPIOA,&GPIO_Initstruc);
}
/********************************************************************
* 功    能：喂狗函数
* 输    入：void
* 输    出：	void
* 编 写 人：王君龙
* 编写日期：2017.3.21
**********************************************************************/
void vFeedExtWatchDog(void)
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET); 	
	}
	else
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET); 	
	}
}
/********************************************************************
* 功    能：点亮指示灯
* 输    入：void
* 输    出：	void
* 编 写 人：stragen
* 编写日期：2017.3.21
**********************************************************************/
void vLed_Light(void)
{
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET); //拉低PC13
}
