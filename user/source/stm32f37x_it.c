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
#include "Uart.h"
#include "Time.h"


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
	if (Bit_SET == GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13))
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);    //拉低PC13
	}
	else	
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	}
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
* 功    能：USART1_IRQHandler中断函数
            将串口数据写到Uart1缓存结构体中的数组
* 输    入：None
* 输    出：None
*           
* 编 写 人：王君龙
* 编写日期：2016.6.23
**********************************************************************/
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1,USART_IT_RXNE) != RESET )
	{
		if(blDrv_Buf_IsFull(&ucDrv_Buf_Uart1_Rcv) != true) //if串口1接收缓存区不满
		{
			ucDrv_Buf_Uart1_Rcv.ucBuf[ucDrv_Buf_Uart1_Rcv.WR_Index]=(uint8_t)USART_ReceiveData(USART1);
			ucDrv_Buf_Uart1_Rcv.WR_Index=(ucDrv_Buf_Uart1_Rcv.WR_Index+1)%DRV_BUF_SIZE;
		}
	};

	//从串口1的发送缓存向外发一个字节
	if (USART_GetITStatus(USART1,USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_TXE);
		USART_SendData(USART1,(uint16_t)ucDrv_Buf_Uart1_Snd.ucBuf[ucDrv_Buf_Uart1_Snd.RD_Index]);
		ucDrv_Buf_Uart1_Snd.RD_Index = (ucDrv_Buf_Uart1_Snd.RD_Index + 1)% DRV_BUF_SIZE;
		if(blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart1_Snd) == true)
		{
			USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
		}
	}
}

/********************************************************************
* 功    能：USART2_IRQHandler中断函数
            将串口数据读取到Uart2缓存结构体中的数组
* 输    入：None
* 输    出：None
*           
* 编 写 人：王君龙
* 编写日期：2018.4.17
**********************************************************************/
void USART2_IRQHandler(void)
{
#ifdef ENABLE_INFR
	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE)== SET)
		USART_ClearFlag(USART2, USART_FLAG_ORE);
	if (USART_GetITStatus(USART2,USART_IT_RXNE) != RESET )
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		if(blDrv_Buf_IsFull(&ucDrv_Buf_Uart2_Rcv) != true) //if串口2接收缓存区不满
		{
			ucDrv_Buf_Uart2_Rcv.ucBuf[ucDrv_Buf_Uart2_Rcv.WR_Index]=(uint8_t)USART_ReceiveData(USART2);
			ucDrv_Buf_Uart2_Rcv.WR_Index=(ucDrv_Buf_Uart2_Rcv.WR_Index+1)%DRV_BUF_SIZE;
		}
	};
	if (USART_GetITStatus(USART2,USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART2,USART_IT_TXE);
		USART_SendData(USART2,(uint16_t)ucDrv_Buf_Uart2_Snd.ucBuf[ucDrv_Buf_Uart2_Snd.RD_Index]);
		ucDrv_Buf_Uart2_Snd.RD_Index = (ucDrv_Buf_Uart2_Snd.RD_Index + 1)% DRV_BUF_SIZE;
		if(blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart2_Snd) == true)
		{
			USART_ITConfig(USART2,USART_IT_TXE,DISABLE);
			//TIM_CCxCmd(TIM5, TIM_Channel_2, TIM_CCx_Disable);
			while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TC))
			{
			}
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		}
	}
#endif
}

/********************************************************************
* 功    能：USART3_IRQHandler中断函数
            将串口数据读取到Uart3缓存结构体中的数组
* 输    入：None
* 输    出：None
*           
* 编 写 人：王君龙
* 编写日期：2016.6.23
**********************************************************************/
void USART3_IRQHandler(void)
{
	if (RESET != USART_GetITStatus(USART3,USART_IT_RXNE)) //读一个字节入串口3接收缓存
	{
		if(blDrv_Buf_IsFull(&ucDrv_Buf_Uart3_Rcv) != true) //if串口3接收缓存区不满
		{
			ucDrv_Buf_Uart3_Rcv.ucBuf[ucDrv_Buf_Uart3_Rcv.WR_Index] = (uint8_t)USART_ReceiveData(USART3);
			ucDrv_Buf_Uart3_Rcv.WR_Index = (ucDrv_Buf_Uart3_Rcv.WR_Index+1) % DRV_BUF_SIZE;	
		}
	}
	if (USART_GetITStatus(USART3,USART_IT_TXE) != RESET) 
	{
		//USART_ClearITPendingBit(USART3,USART_IT_TXE);
		USART_SendData(USART3,(uint16_t)ucDrv_Buf_Uart3_Snd.ucBuf[ucDrv_Buf_Uart3_Snd.RD_Index]);
		ucDrv_Buf_Uart3_Snd.RD_Index = (ucDrv_Buf_Uart3_Snd.RD_Index + 1) % DRV_BUF_SIZE;
		if(blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart3_Snd) == true)
			USART_ITConfig(USART3,USART_IT_TXE,DISABLE);
	}
}

/********************************************************************
* 功    能：TIM2_IRQHandler断函数
            计时累加
* 输    入：None
* 输    出：None
*           
* 编 写 人：王君龙
* 编写日期：2016.6.23
**********************************************************************/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		if(blWordsCounterStatus == true)
		{
			uwTime2WordsCounter++;
		}
		if(blFramesCounterStatus == true)
		{
			uwTime2FramesCounter++;
		}
	}
}

/********************************************************************
* 功    能：TIM5_IRQHandler断函数
				PWM输出
* 输    入：None
* 输    出：None
*
* 编 写 人：王君龙
* 编写日期：2017.9.11
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
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
