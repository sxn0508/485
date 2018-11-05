/*********************************************************************** 
* Copyright (c) 2012,积成电子股份有限公司 All rights reserved.
*
* 文件名称： UART.c
* 描    述： 串口通信代码文件
* 
* 修改记录：
*
* V1.0，2016.06.19，
***********************************************************************/

#include "Uart.h"
#include "stm32f37x_it.h"


UCDRV_BUF ucDrv_Buf_Uart1_Rcv={{0},0,0};
UCDRV_BUF ucDrv_Buf_Uart1_Snd={{0},0,0};
UCDRV_BUF ucDrv_Buf_Uart2_Rcv={{0},0,0};
UCDRV_BUF ucDrv_Buf_Uart2_Snd={{0},0,0};
UCDRV_BUF ucDrv_Buf_Uart3_Rcv={{0},0,0};
UCDRV_BUF ucDrv_Buf_Uart3_Snd={{0},0,0};
/********************************************************************
* 功    能：串口初始化函数
* 输    入：None
* 输    出：None
*           
* 编 写 人：
* 编写日期：2016.6.23
**********************************************************************/
void vUart_Init(void)
{
	USART_InitTypeDef USART_Initstruc;
	/*USART1配置*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	USART_OverSampling8Cmd(USART1, ENABLE); 
	USART_Initstruc.USART_BaudRate=2400;
	USART_Initstruc.USART_Mode=(USART_Mode_Rx | USART_Mode_Tx);
	USART_Initstruc.USART_WordLength=USART_WordLength_9b;
	USART_Initstruc.USART_Parity=USART_Parity_Even;
	USART_Initstruc.USART_StopBits=USART_StopBits_1;
	USART_Initstruc.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART1,&USART_Initstruc);
	/*USART3配置*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);	
	USART_OverSampling8Cmd(USART3, ENABLE); 
	USART_Initstruc.USART_BaudRate=2400;
	USART_Initstruc.USART_Mode=(USART_Mode_Rx | USART_Mode_Tx);
	USART_Initstruc.USART_WordLength=USART_WordLength_9b;
	USART_Initstruc.USART_Parity=USART_Parity_Even;
	USART_Initstruc.USART_StopBits=USART_StopBits_1;
	USART_Initstruc.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART3,&USART_Initstruc);
	/*USART2配置为标准串口*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_OverSampling8Cmd(USART2, ENABLE); 
	USART_Initstruc.USART_BaudRate=1200;
	USART_Initstruc.USART_WordLength=USART_WordLength_9b;
	USART_Initstruc.USART_StopBits=USART_StopBits_1;
	USART_Initstruc.USART_Parity=USART_Parity_Even;
	USART_Initstruc.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Initstruc.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART2,&USART_Initstruc);
	vIRQ_UART_Config();   //中断配置,使能
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1,ENABLE);
	USART_Cmd(USART3,ENABLE);
	USART_Cmd(USART2,ENABLE);
}
/********************************************************************
  * @brief 串口中断初始化函数
  * @param  None
  * @retval None
  *******************************************************************/	
void vIRQ_UART_Config(void)
{
	/*定义中断结构体*/
	NVIC_InitTypeDef NVIC_Initstruc;
	/*配置串口1中断*/
	NVIC_Initstruc.NVIC_IRQChannel=USART1_IRQn;
	NVIC_Initstruc.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_Initstruc.NVIC_IRQChannelSubPriority=1;
	NVIC_Initstruc.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_Initstruc);
	/*配置串口3中断*/
	NVIC_Initstruc.NVIC_IRQChannel=USART3_IRQn;
	NVIC_Initstruc.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_Initstruc);
	/*配置串口2中断*/
	NVIC_Initstruc.NVIC_IRQChannel=USART2_IRQn;
	NVIC_Initstruc.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_Initstruc);
}
/********************************************************************
* 功    能：判断驱动层缓存是否为空
* 输    入：驱动层缓存结构体地址
* 输    出：true/false
*           
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
bool blDrv_Buf_IsEmpty(const UCDRV_BUF *ucdrv_buf)
{ 
	if(ucdrv_buf->RD_Index == ucdrv_buf->WR_Index)	
		return true;
	else
		return false;	
}
/********************************************************************
* 功    能：判断驱动层缓存是否满
* 输    入：驱动层缓存结构体地址
* 输    出：true/false
*           
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
bool blDrv_Buf_IsFull(const UCDRV_BUF *ucdrv_buf)
{ 
	if(ucdrv_buf->RD_Index == (ucdrv_buf->WR_Index+1)%DRV_BUF_SIZE)	
		return true;
	else
		return false;
}
/********************************************************************
* 功    能：查询驱动层缓存剩余空间
* 输    入：数据缓存结构体地址
* 输    出：uint16_t
*           
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
uint16_t ucDrv_Buf_EmpLen(const UCDRV_BUF *ucdrv_buf)
{ 
	uint16_t i= (ucdrv_buf->RD_Index - ucdrv_buf->WR_Index)%DRV_BUF_SIZE -1;
	return i;
}
/********************************************************************
* 功    能：查询缓存中未读的字节数
* 输    入：数据缓存结构体地址
* 输    出：uint16_t
*          
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
uint16_t ucDrv_Buf_DataLen(const UCDRV_BUF *ucdrv_buf)
{ 
	return (ucdrv_buf->WR_Index - ucdrv_buf->RD_Index + DRV_BUF_SIZE)%DRV_BUF_SIZE;
}
/********************************************************************
* 功    能：从驱动缓存读取指定数量的字节
* 输    入：驱动缓存结构体地址，数据缓存结构体地址，读取字节数
* 输    出：true/false
*          
* 编 写 人：王君龙
* 编写日期：2016.6.28
**********************************************************************/
bool ucDrv_Buf_GetBytes(UCDRV_BUF *ucdrv_buf,uint8_t *ucdata_buf,uint16_t length)
{ 
	uint16_t i;
	if(ucDrv_Buf_DataLen(ucdrv_buf) >= length)
	{
		for(i=0;i<length;i++)
		{
			*(ucdata_buf + i) = ucdrv_buf->ucBuf[ucdrv_buf->RD_Index];
			ucdrv_buf->RD_Index = (ucdrv_buf->RD_Index + 1)	% DRV_BUF_SIZE;
		}
		return true;
	}
	else 
		return false;
}
/********************************************************************
* 功    能：向驱动缓存写指定数量的字节
* 输    入：数据缓存结构体地址，驱动缓存结构体地址，写字节数
* 输    出：true/false
*          
* 编 写 人：王君龙
* 编写日期：2017.3.14
**********************************************************************/
bool ucDrv_Buf_PutBytes(uint8_t *ucdata_buf,UCDRV_BUF *ucdrv_buf,uint16_t length)
{ 
	uint16_t i;
	if(DRV_BUF_SIZE >= ucDrv_Buf_DataLen(ucdrv_buf) + length)
	{
		for(i=0;i<length;i++)
		{
			ucdrv_buf->ucBuf[ucdrv_buf->WR_Index] = *(ucdata_buf + i);
			ucdrv_buf->WR_Index = (ucdrv_buf->WR_Index + 1)	% DRV_BUF_SIZE;
		}
		return true;
	}
	else 
		return false;
}
/********************************************************************
* 功    能：串口发送（使用中断功能）
* 输    入：USART_X
* 输    出：void
*           
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
 void Uart_DataPut(USART_TypeDef* USART_X)
{
	 USART_ITConfig(USART_X,USART_IT_TXE,ENABLE);
}

