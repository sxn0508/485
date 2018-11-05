#include "main.h"
#include "GPIO.h"
#include "Uart.h"
#include "DLT645.h"
#include "Flash.h"
#include "Time.h"

#define  SysTick_1000Ms  SystemCoreClock/ 8 

int main(void)
{
	/*�ж�������ʼ��*/
	vNVIC_IRQ_Config();			
	SysTick_Config(SysTick_1000Ms);
	/*SysTickʹ��ϵͳƵ��64M��8��Ƶ��8MHz*/
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	NVIC_SetPriority(SysTick_IRQn, -1);
	/*�ڲ�flash��ʼ��*/
	vInterFlash_Init();			
	/*��ʼ��ι�����š�ָʾ�����š�����1������3����*/
	vGPIO_Init();					
	/*�ϵ�ι��*/
	vFeedExtWatchDog();			
	/*���ڳ�ʼ��*/
	vUart_Init();					
	/*��ʱ��2��ʼ��*/
	vTimer_Init();					
	/*��ѹ�޸ĵ�������ֵ��ΪĬ��ֵ�������޸Ĺ��� */
	Voltage_Change_Init();		
	/*�����ع��ز�������*/
	vCurrent_Limit_DefaultSet();
	/*�����޸ĵ�������ֵ��ΪĬ��ֵ�������޸Ĺ��� */
	vEnergy_Modify_Init();
	/*�������е�*/
	vLed_Light();					
	while(1)
	{
		/*�������շ�ʱι�����Ϊ1ms����*/
		vFeedExtWatchDog();
		/*������������֡��֡����*/
		if(blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart1_Rcv) == false)
		{
			RsvFrameHandle(ucApp_Buf_ZD2DB);
		}
		/*������֡����*/
		/*δ�յ����֡*/
		if(blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart3_Rcv) == true) 
		{
			VoltageTimeOutHandle();
		}
		/*�յ����֡*/
		else  
		{
			/*�Ƿ�֡*/
			if(!blRecvFrame(&ucDrv_Buf_Uart3_Rcv,ucApp_Buf_DB2ZD))
			{
				/*��ѹ��֡��ʱ*/
				if((Voltage_Frame_Status == FrameAwaitingReply) &&
							(blTime2_Update(uwTime2FramesCounter,DLT645_FRAMES_DELAY_MAX)))
				{
					/*�м���ʹ���ϴε���ȷ��ѹֵ����ظ�*/
					if(blVoltage_Create(ucApp_Buf_ZD2DB))  
					{
						ucDrv_Buf_PutBytes(ucApp_Buf_ZD2DB,&ucDrv_Buf_Uart1_Snd,ucApp_Buf_ZD2DB[9]);
						/*���͵�ѹ֡*/
						if(!blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart1_Snd))   
						{
							Uart_DataPut(USART1);
							/*�ȴ���ѹ��֡״̬��λ*/
							Voltage_Frame_Status = FrameReplied;
							/*��ʱ����λ*/
							blFramesCounterStatus = false;
							uwTime2FramesCounter = 0;
						}
					}
				}
			}
			/*�Ϸ�֡*/
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
		/*��ѡ���ܣ��������ò���*/
#ifdef ENABLE_INFR
		/*���⴮����֡���� */
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
				/*����ڻ�֡*/
				if (!blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart2_Snd))   
				{
					USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
					//TIM_CCxCmd(TIM5, TIM_Channel_2, TIM_CCx_Enable);
					USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
				}
			}
		}//end ������֡����
#endif 
	}//end while
}//end main


void RsvFrameHandle(uint8_t * pucBuffer)
{
	/*֡�Ϸ����ж�*/
	if(blRecvFrame(&ucDrv_Buf_Uart1_Rcv,pucBuffer))
	{
		/*�Ƿ��ٲ��ѹ*/
		if((pucBuffer[8]==0X11) && 
					(pucBuffer[10]==0x33) && 
					(pucBuffer[12]==0x34) && 
					(pucBuffer[13]==0x35))
		{
			/*�ȴ���ѹ��֡״̬��λ*/
			Voltage_Frame_Status = FrameAwaitingReply;
			/*��ʼ��ʱ*/
			blFramesCounterStatus = true;
		}
		/*ת���ٲ�֡*/
		ucDrv_Buf_PutBytes(pucBuffer,&ucDrv_Buf_Uart3_Snd,uc645DataDomainLength+12);
		if(!blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart3_Snd))   
		{
			Uart_DataPut(USART3);
		}
	}
}

void VoltageTimeOutHandle(void)
{
	/*�ٲ��ѹ��ʱ*/
	if((Voltage_Frame_Status == FrameAwaitingReply) &&
				(blTime2_Update(uwTime2FramesCounter,DLT645_FRAMES_DELAY_MAX)))
	{
		/*�м���ʹ���ϴε���ȷ��ѹֵ����ظ�*/
		if(blVoltage_Create(ucApp_Buf_ZD2DB))  
		{
			ucDrv_Buf_PutBytes(ucApp_Buf_ZD2DB, &ucDrv_Buf_Uart1_Snd, ucApp_Buf_ZD2DB[9]+12);
			if(!blDrv_Buf_IsEmpty(&ucDrv_Buf_Uart1_Snd))   
			{
				Uart_DataPut(USART1);
				/*�ȴ���ѹ��֡״̬��λ*/
				Voltage_Frame_Status = FrameReplied;
				/*��ʱ����λ*/
				blFramesCounterStatus = false;
				uwTime2FramesCounter = 0;
			}
		}
	}
}


void vNVIC_IRQ_Config(void)
{
	/*�����ж�������*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
}







