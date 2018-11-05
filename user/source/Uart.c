/*********************************************************************** 
* Copyright (c) 2012,���ɵ��ӹɷ����޹�˾ All rights reserved.
*
* �ļ����ƣ� UART.c
* ��    ���� ����ͨ�Ŵ����ļ�
* 
* �޸ļ�¼��
*
* V1.0��2016.06.19��
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
* ��    �ܣ����ڳ�ʼ������
* ��    �룺None
* ��    ����None
*           
* �� д �ˣ�
* ��д���ڣ�2016.6.23
**********************************************************************/
void vUart_Init(void)
{
	USART_InitTypeDef USART_Initstruc;
	/*USART1����*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	USART_OverSampling8Cmd(USART1, ENABLE); 
	USART_Initstruc.USART_BaudRate=2400;
	USART_Initstruc.USART_Mode=(USART_Mode_Rx | USART_Mode_Tx);
	USART_Initstruc.USART_WordLength=USART_WordLength_9b;
	USART_Initstruc.USART_Parity=USART_Parity_Even;
	USART_Initstruc.USART_StopBits=USART_StopBits_1;
	USART_Initstruc.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART1,&USART_Initstruc);
	/*USART3����*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);	
	USART_OverSampling8Cmd(USART3, ENABLE); 
	USART_Initstruc.USART_BaudRate=2400;
	USART_Initstruc.USART_Mode=(USART_Mode_Rx | USART_Mode_Tx);
	USART_Initstruc.USART_WordLength=USART_WordLength_9b;
	USART_Initstruc.USART_Parity=USART_Parity_Even;
	USART_Initstruc.USART_StopBits=USART_StopBits_1;
	USART_Initstruc.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART3,&USART_Initstruc);
	/*USART2����Ϊ��׼����*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_OverSampling8Cmd(USART2, ENABLE); 
	USART_Initstruc.USART_BaudRate=1200;
	USART_Initstruc.USART_WordLength=USART_WordLength_9b;
	USART_Initstruc.USART_StopBits=USART_StopBits_1;
	USART_Initstruc.USART_Parity=USART_Parity_Even;
	USART_Initstruc.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Initstruc.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART2,&USART_Initstruc);
	vIRQ_UART_Config();   //�ж�����,ʹ��
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1,ENABLE);
	USART_Cmd(USART3,ENABLE);
	USART_Cmd(USART2,ENABLE);
}
/********************************************************************
  * @brief �����жϳ�ʼ������
  * @param  None
  * @retval None
  *******************************************************************/	
void vIRQ_UART_Config(void)
{
	/*�����жϽṹ��*/
	NVIC_InitTypeDef NVIC_Initstruc;
	/*���ô���1�ж�*/
	NVIC_Initstruc.NVIC_IRQChannel=USART1_IRQn;
	NVIC_Initstruc.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_Initstruc.NVIC_IRQChannelSubPriority=1;
	NVIC_Initstruc.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_Initstruc);
	/*���ô���3�ж�*/
	NVIC_Initstruc.NVIC_IRQChannel=USART3_IRQn;
	NVIC_Initstruc.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_Initstruc);
	/*���ô���2�ж�*/
	NVIC_Initstruc.NVIC_IRQChannel=USART2_IRQn;
	NVIC_Initstruc.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_Initstruc);
}
/********************************************************************
* ��    �ܣ��ж������㻺���Ƿ�Ϊ��
* ��    �룺�����㻺��ṹ���ַ
* ��    ����true/false
*           
* �� д �ˣ�
* ��д���ڣ�2016.6.28
**********************************************************************/
bool blDrv_Buf_IsEmpty(const UCDRV_BUF *ucdrv_buf)
{ 
	if(ucdrv_buf->RD_Index == ucdrv_buf->WR_Index)	
		return true;
	else
		return false;	
}
/********************************************************************
* ��    �ܣ��ж������㻺���Ƿ���
* ��    �룺�����㻺��ṹ���ַ
* ��    ����true/false
*           
* �� д �ˣ�
* ��д���ڣ�2016.6.28
**********************************************************************/
bool blDrv_Buf_IsFull(const UCDRV_BUF *ucdrv_buf)
{ 
	if(ucdrv_buf->RD_Index == (ucdrv_buf->WR_Index+1)%DRV_BUF_SIZE)	
		return true;
	else
		return false;
}
/********************************************************************
* ��    �ܣ���ѯ�����㻺��ʣ��ռ�
* ��    �룺���ݻ���ṹ���ַ
* ��    ����uint16_t
*           
* �� д �ˣ�
* ��д���ڣ�2016.6.28
**********************************************************************/
uint16_t ucDrv_Buf_EmpLen(const UCDRV_BUF *ucdrv_buf)
{ 
	uint16_t i= (ucdrv_buf->RD_Index - ucdrv_buf->WR_Index)%DRV_BUF_SIZE -1;
	return i;
}
/********************************************************************
* ��    �ܣ���ѯ������δ�����ֽ���
* ��    �룺���ݻ���ṹ���ַ
* ��    ����uint16_t
*          
* �� д �ˣ�
* ��д���ڣ�2016.6.28
**********************************************************************/
uint16_t ucDrv_Buf_DataLen(const UCDRV_BUF *ucdrv_buf)
{ 
	return (ucdrv_buf->WR_Index - ucdrv_buf->RD_Index + DRV_BUF_SIZE)%DRV_BUF_SIZE;
}
/********************************************************************
* ��    �ܣ������������ȡָ���������ֽ�
* ��    �룺��������ṹ���ַ�����ݻ���ṹ���ַ����ȡ�ֽ���
* ��    ����true/false
*          
* �� д �ˣ�������
* ��д���ڣ�2016.6.28
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
* ��    �ܣ�����������дָ���������ֽ�
* ��    �룺���ݻ���ṹ���ַ����������ṹ���ַ��д�ֽ���
* ��    ����true/false
*          
* �� д �ˣ�������
* ��д���ڣ�2017.3.14
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
* ��    �ܣ����ڷ��ͣ�ʹ���жϹ��ܣ�
* ��    �룺USART_X
* ��    ����void
*           
* �� д �ˣ�
* ��д���ڣ�2016.6.28
**********************************************************************/
 void Uart_DataPut(USART_TypeDef* USART_X)
{
	 USART_ITConfig(USART_X,USART_IT_TXE,ENABLE);
}

