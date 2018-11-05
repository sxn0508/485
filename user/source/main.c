#include "main.h"
#include "GPIO.h"
#include "Uart.h"
#include "DLT645.h"
#include "Flash.h"
#include "Time.h"

#define  SysTick_1000Ms  SystemCoreClock/ 8 

int main(void)
{
	/*中断向量初始化*/
	vNVIC_IRQ_Config();			
	SysTick_Config(SysTick_1000Ms);
	/*SysTick使用系统频率64M的8分频：8MHz*/
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	NVIC_SetPriority(SysTick_IRQn, -1);
	/*内部flash初始化*/
	vInterFlash_Init();			
	/*初始化喂狗引脚、指示灯引脚、串口1、串口3引脚*/
	vGPIO_Init();					
	/*上电喂狗*/
	vFeedExtWatchDog();			
	/*串口初始化*/
	vUart_Init();					
	/*定时器2初始化*/
	vTimer_Init();					
	/*电压修改的上下限值设为默认值；开启修改功能 */
	Voltage_Change_Init();		
	/*电流重过载参数设置*/
	vCurrent_Limit_DefaultSet();
	/*电量修改的上下限值设为默认值；开启修改功能 */
	vEnergy_Modify_Init();
	/*点亮运行灯*/
	vLed_Light();					
	while(1)
	{
		/*串口无收发时喂狗间隔为1ms左右*/
		vFeedExtWatchDog();
		/*集中器侧数据帧收帧处理*/
		if(blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart1_Rcv) == false)
		{
			RsvFrameHandle(ucApp_Buf_ZD2DB);
		}
		/*电表侧收帧处理*/
		/*未收到电表帧*/
		if(blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart3_Rcv) == true) 
		{
			VoltageTimeOutHandle();
		}
		/*收到电表帧*/
		else  
		{
			/*非法帧*/
			if(!blRecvFrame(&ucDrv_Buf_Uart3_Rcv,ucApp_Buf_DB2ZD))
			{
				/*电压回帧超时*/
				if((Voltage_Frame_Status == FrameAwaitingReply) &&
							(blTime2_Update(uwTime2FramesCounter,DLT645_FRAMES_DELAY_MAX)))
				{
					/*中继器使用上次的正确电压值替代回复*/
					if(blVoltage_Create(ucApp_Buf_ZD2DB))  
					{
						ucDrv_Buf_PutBytes(ucApp_Buf_ZD2DB,&ucDrv_Buf_Uart1_Snd,ucApp_Buf_ZD2DB[9]);
						/*发送电压帧*/
						if(!blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart1_Snd))   
						{
							Uart_DataPut(USART1);
							/*等待电压回帧状态清位*/
							Voltage_Frame_Status = FrameReplied;
							/*计时器复位*/
							blFramesCounterStatus = false;
							uwTime2FramesCounter = 0;
						}
					}
				}
			}
			/*合法帧*/
			else    
			{
				vDBDataID_Handle(ucApp_Buf_DB2ZD);
				ucDrv_Buf_PutBytes(ucApp_Buf_DB2ZD,&ucDrv_Buf_Uart1_Snd,uc645DataDomainLength+12);
				if(!blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart1_Snd))   
				{
					Uart_DataPut(USART1);
				}
			}
		}
		/*可选功能：红外设置参数*/
#ifdef ENABLE_INFR
		/*红外串口收帧处理 */
		if(blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart2_Rcv) == false)					
		{
			if(blRecvFrame(&ucDrv_Buf_Uart2_Rcv, ucApp_Buf_INFR))
			{
				if (ucApp_Buf_INFR[8] == 0X13) 
				{
					ucApp_Buf_INFR[1] = 0x01;
					ucApp_Buf_INFR[2] = 0x01;
					ucApp_Buf_INFR[3] = 0x01;
					ucApp_Buf_INFR[4] = 0x01;
					ucApp_Buf_INFR[5] = 0x01;
					ucApp_Buf_INFR[6] = 0x01;
					ucApp_Buf_INFR[8] = 0x93;
					ucApp_Buf_INFR[9] = 0x06;
					ucApp_Buf_INFR[10] = 0x34;
					ucApp_Buf_INFR[11] = 0x34;
					ucApp_Buf_INFR[12] = 0x34;
					ucApp_Buf_INFR[13] = 0x34;
					ucApp_Buf_INFR[14] = 0x34;
					ucApp_Buf_INFR[15] = 0x34;
					ucApp_Buf_INFR[16] = 0xA7;
					ucApp_Buf_INFR[17] = 0x16;
					ucDrv_Buf_PutBytes(ucApp_Buf_INFR, &ucDrv_Buf_Uart2_Snd, 18);
				}
				else
				{
					vINFRDataID_Handle(ucApp_Buf_INFR);
				}
				/*红外口回帧*/
				if (!blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart2_Snd))   
				{
					USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
					//TIM_CCxCmd(TIM5, TIM_Channel_2, TIM_CCx_Enable);
					USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
				}
			}
		}//end 红外收帧处理
#endif 
	}//end while
}//end main


void RsvFrameHandle(uint8_t * pucBuffer)
{
	/*帧合法性判断*/
	if(blRecvFrame(&ucDrv_Buf_Uart1_Rcv,pucBuffer))
	{
		/*是否召测电压*/
		if((pucBuffer[8]==0X11) && 
					(pucBuffer[10]==0x33) && 
					(pucBuffer[12]==0x34) && 
					(pucBuffer[13]==0x35))
		{
			/*等待电压回帧状态置位*/
			Voltage_Frame_Status = FrameAwaitingReply;
			/*开始计时*/
			blFramesCounterStatus = true;
		}
		/*转发召测帧*/
		ucDrv_Buf_PutBytes(pucBuffer,&ucDrv_Buf_Uart3_Snd,uc645DataDomainLength+12);
		if(!blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart3_Snd))   
		{
			Uart_DataPut(USART3);
		}
	}
}

void VoltageTimeOutHandle(void)
{
	/*召测电压超时*/
	if((Voltage_Frame_Status == FrameAwaitingReply) &&
				(blTime2_Update(uwTime2FramesCounter,DLT645_FRAMES_DELAY_MAX)))
	{
		/*中继器使用上次的正确电压值替代回复*/
		if(blVoltage_Create(ucApp_Buf_ZD2DB))  
		{
			ucDrv_Buf_PutBytes(ucApp_Buf_ZD2DB, &ucDrv_Buf_Uart1_Snd, ucApp_Buf_ZD2DB[9]+12);
			if(!blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart1_Snd))   
			{
				Uart_DataPut(USART1);
				/*等待电压回帧状态清位*/
				Voltage_Frame_Status = FrameReplied;
				/*计时器复位*/
				blFramesCounterStatus = false;
				uwTime2FramesCounter = 0;
			}
		}
	}
}


void vNVIC_IRQ_Config(void)
{
	/*配置中断向量组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
}







