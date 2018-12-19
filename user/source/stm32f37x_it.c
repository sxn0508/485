/**
  ******************************************************************************
  * @file    Project/STM32F37x_StdPeriph_Templates/stm32f37x_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
//#include "stdlib.h"
#include "stm32f37x_it.h"
#include "main.h"
#include "GPIO.h"
#include "Uart.h"
#include "Time.h"
#include "DLT645.h"

uint16_t capture = 0;
/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    //	if (CoreDebug->DHCSR & 1) {  //check C_DEBUGEN == 1 -> Debugger Connected
    //      __breakpoint(0);  // halt program execution here
    //  }

    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    TICKS++;
}

/******************************************************************************/
/*                 STM32F37x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f37x.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/********************************************************************
* ��    �ܣ�USART1_IRQHandler�жϺ���
            ����������д��Uart1����ṹ���е�����
* ��    �룺None
* ��    ����None
*           
* �� д �ˣ�������
* ��д���ڣ�2016.6.23
**********************************************************************/
void USART1_IRQHandler(void)
{
    if (USART_GetFlagStatus(pUartZD->handler, USART_FLAG_ORE))
    {
        USART_ClearFlag(pUartZD->handler, USART_FLAG_ORE);
    }

    if (USART_GetITStatus(pUartZD->handler, USART_IT_RXNE) != RESET)
    {
        if (blDrv_Buf_IsFull(pUartZD->pRsvbuf) != true) //if����1���ջ���������
        {
            pUartZD->pRsvbuf->data[pUartZD->pRsvbuf->wr] = (uint8_t)USART_ReceiveData(pUartZD->handler);
            //ucDrv_Buf_Uart1_Rcv.ucBuf[ucDrv_Buf_Uart1_Rcv.WR_Index] = (uint8_t)USART_ReceiveData(pUartZD->handler);
            //ucDrv_Buf_Uart1_Rcv.WR_Index = (ucDrv_Buf_Uart1_Rcv.WR_Index + 1) % DRV_BUF_SIZE;
            pUartZD->pRsvbuf->wr = (pUartZD->pRsvbuf->wr + 1) % DRV_BUF_SIZE;
        }
    };

    //�Ӵ���1�ķ��ͻ������ⷢһ���ֽ�
    if (USART_GetITStatus(pUartZD->handler, USART_IT_TXE) != RESET)
    {
        USART_ClearITPendingBit(pUartZD->handler, USART_IT_TXE);
        //USART_SendData(pUartZD->handler, (uint16_t)ucDrv_Buf_Uart1_Snd.ucBuf[ucDrv_Buf_Uart1_Snd.RD_Index]);
        USART_SendData(pUartZD->handler, (uint16_t)pUartZD->pSndbuf->data[pUartZD->pSndbuf->rd]);
        //ucDrv_Buf_Uart1_Snd.RD_Index = (ucDrv_Buf_Uart1_Snd.RD_Index + 1) % DRV_BUF_SIZE;
        pUartZD->pSndbuf->rd = (pUartZD->pSndbuf->rd + 1) % DRV_BUF_SIZE;
        if (blDrv_Buf_IsEmpty(pUartZD->pSndbuf) == true)
        {
            USART_ITConfig(pUartZD->handler, USART_IT_TXE, DISABLE);
        }
    }
    /*���ճ�ʱһ���ֽ�ʱ����δ�յ����ݣ���Լ1ms*/
    /*������DLT645��Լ�涨�ֽڳ�ʱʱ��Ϊ500ms*/
    if (USART_GetITStatus(pUartZD->handler, USART_IT_IDLE) != RESET)
    {
        USART_ClearITPendingBit(pUartZD->handler, USART_IT_IDLE);
        FLAG_UartZD_HasData = true;
    }
}

/********************************************************************
* ��    �ܣ�USART2_IRQHandler�жϺ���
            ���������ݶ�ȡ��Uart2����ṹ���е�����
* ��    �룺None
* ��    ����None
*           
* �� д �ˣ�������
* ��д���ڣ�2018.4.17
**********************************************************************/
void USART2_IRQHandler(void)
{
#ifdef ENABLE_INFR
    if (USART_GetFlagStatus(pUartIR->handler, USART_FLAG_ORE) == SET)
        USART_ClearFlag(pUartIR->handler, USART_FLAG_ORE);
    if (USART_GetITStatus(pUartIR->handler, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(pUartIR->handler, USART_IT_RXNE);
        //if (blDrv_Buf_IsFull(&ucDrv_Buf_Uart2_Rcv) != true) //if����2���ջ���������
        if (blDrv_Buf_IsFull(pUartIR->pRsvbuf) != true) //if����2���ջ���������
        {
            //ucDrv_Buf_Uart2_Rcv.ucBuf[ucDrv_Buf_Uart2_Rcv.WR_Index] = (uint8_t)USART_ReceiveData(pUartIR->handler);
            pUartIR->pRsvbuf->data[pUartIR->pRsvbuf->wr] = (uint8_t)USART_ReceiveData(pUartIR->handler);
            //ucDrv_Buf_Uart2_Rcv.WR_Index = (ucDrv_Buf_Uart2_Rcv.WR_Index + 1) % DRV_BUF_SIZE;
            pUartIR->pRsvbuf->wr = (pUartIR->pRsvbuf->wr + 1) % DRV_BUF_SIZE;
        }
    };
    if (USART_GetITStatus(pUartIR->handler, USART_IT_TXE) != RESET)
    {
        USART_ClearITPendingBit(pUartIR->handler, USART_IT_TXE);
        //USART_SendData(pUartIR->handler, (uint16_t)ucDrv_Buf_Uart2_Snd.ucBuf[ucDrv_Buf_Uart2_Snd.RD_Index]);
        USART_SendData(pUartIR->handler, (uint16_t)pUartIR->pSndbuf->data[pUartIR->pSndbuf->rd]);
        //ucDrv_Buf_Uart2_Snd.RD_Index = (ucDrv_Buf_Uart2_Snd.RD_Index + 1) % DRV_BUF_SIZE;
        pUartIR->pSndbuf->rd = (pUartIR->pSndbuf->rd + 1) % DRV_BUF_SIZE;
        if (blDrv_Buf_IsEmpty(pUartIR->pSndbuf) == true)
        {
            USART_ITConfig(pUartIR->handler, USART_IT_TXE, DISABLE);
            //TIM_CCxCmd(TIM5, TIM_Channel_2, TIM_CCx_Disable);
            while (RESET == USART_GetFlagStatus(pUartIR->handler, USART_FLAG_TC))
            {
            }
            USART_ITConfig(pUartIR->handler, USART_IT_RXNE, ENABLE);
        }
    }
#endif
}

/********************************************************************
* ��    �ܣ�USART3_IRQHandler�жϺ���
            ���������ݶ�ȡ��Uart3����ṹ���е�����
* ��    �룺None
* ��    ����None
*           
* �� д �ˣ�������
* ��д���ڣ�2016.6.23
**********************************************************************/
void USART3_IRQHandler(void)
{
    if (USART_GetFlagStatus(pUartDB->handler, USART_FLAG_ORE))
    {
        USART_ClearFlag(pUartDB->handler, USART_FLAG_ORE);
    }
    if (RESET != USART_GetITStatus(pUartDB->handler, USART_IT_RXNE)) //��һ���ֽ��봮��3���ջ���
    {
        if (blDrv_Buf_IsFull(pUartDB->pRsvbuf) != true) //if����3���ջ���������
        {
            //ucDrv_Buf_Uart3_Rcv.ucBuf[ucDrv_Buf_Uart3_Rcv.WR_Index] = (uint8_t)USART_ReceiveData(pUartDB->handler);
            pUartDB->pRsvbuf->data[pUartDB->pRsvbuf->wr] = (uint8_t)USART_ReceiveData(pUartDB->handler);
            //ucDrv_Buf_Uart3_Rcv.WR_Index = (ucDrv_Buf_Uart3_Rcv.WR_Index + 1) % DRV_BUF_SIZE;
            pUartDB->pRsvbuf->wr = (pUartDB->pRsvbuf->wr + 1) % DRV_BUF_SIZE;
        }
    }
    if (USART_GetITStatus(pUartDB->handler, USART_IT_TXE) != RESET)
    {
        //USART_ClearITPendingBit(pUartDB->handler,USART_IT_TXE);
        //USART_SendData(pUartDB->handler, (uint16_t)ucDrv_Buf_Uart3_Snd.ucBuf[ucDrv_Buf_Uart3_Snd.RD_Index]);
        USART_SendData(pUartDB->handler, (uint16_t)pUartDB->pSndbuf->data[pUartDB->pSndbuf->rd]);
        //ucDrv_Buf_Uart3_Snd.RD_Index = (ucDrv_Buf_Uart3_Snd.RD_Index + 1) % DRV_BUF_SIZE;
        pUartDB->pSndbuf->rd = (pUartDB->pSndbuf->rd + 1) % DRV_BUF_SIZE;
        if (blDrv_Buf_IsEmpty(pUartDB->pSndbuf) == true)
            USART_ITConfig(pUartDB->handler, USART_IT_TXE, DISABLE);
    }
    /*���ճ�ʱһ���ֽ�ʱ����δ�յ����ݣ���Լ1ms*/
    /*������DLT645��Լ�涨�ֽڳ�ʱʱ��Ϊ500ms*/
    if (USART_GetITStatus(pUartDB->handler, USART_IT_IDLE) != RESET)
    {
        USART_ClearITPendingBit(pUartDB->handler, USART_IT_IDLE);
        FLAG_UartDB_HasData = true;
    }
}

/********************************************************************
* ��    �ܣ�TIM2_IRQHandler�Ϻ���
            ��ʱ�ۼ�
* ��    �룺None
* ��    ����None
*           
* �� д �ˣ�������
* ��д���ڣ�2016.6.23
**********************************************************************/
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        if (blWordsCounterStatus == true)
        {
            uwTime2WordsCounter++;
        }
        if (blFramesCounterStatus == true)
        {
            uwTime2FramesCounter++;
        }
    }
}

/********************************************************************
* ��    �ܣ�TIM5_IRQHandler�Ϻ���
				PWM���
* ��    �룺None
* ��    ����None
*
* �� д �ˣ�������
* ��д���ڣ�2017.9.11
**********************************************************************/
void TIM5_IRQHandler(void)
{
    /*
	if (TIM_GetITStatus(TIM5, TIM_IT_CC2) != RESET)
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC2);
		capture = TIM_GetCapture1(TIM5);
		TIM_SetCompare1(TIM5, capture + uwCCR2_Val);
	}
 */
}

/********************************************************************
* ��    �ܣ��ⲿ�ж�5~9�жϷ�����
* ��    �룺None
* ��    ����None
*           
* �� д �ˣ�stragen
* ��д���ڣ�2018��9��5��08:41:59
**********************************************************************/
void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        vUartZD_BaudelayCal(&AutoBaud485.uwBaudelay);
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
