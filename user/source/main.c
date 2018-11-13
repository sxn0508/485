#include "main.h"
#include "GPIO.h"
#include "Uart.h"
#include "DLT645.h"
#include "Flash.h"
#include "Time.h"
#include "protocol.h"
#include "DLT698.h"
#include "DLT698_45.h"

/*698���ݴ洢��Ԫ*/
COLL_STORE_DATA stCollData;
/*������֡��־*/
bool FLAG_UartZD_HasData = false;
bool FLAG_UartDB_HasData = false;

int main(void)
{
    ProtocolDef ProtocolType = none;
    uint8_t tmpAPDU[MAX_APDU_LENGTH];
    uint8_t *pAPDU = tmpAPDU;
    uint32_t dwLen = 0;
    uint8_t *pframe = NULL;
    uint32_t i;
    uint32_t test;

    /*�����ж�������*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SysTick_Config(SysTick_100Ms);
    /*SysTickʹ��ϵͳƵ��64M��8��Ƶ��8MHz*/
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    NVIC_SetPriority(SysTick_IRQn, -1);
    vInterFlash_Init();
    vGPIO_Init();
    vFeedExtWatchDog();
    vUart_Init(); //RXNE�ж�ʹ��
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
    while (1)
    {
        switch (ProtocolType)
        {
        case none:
        {
            /*ʶ���Լ����*/
            Uart_idleReadEnable(pUartZD);
            Uart_idleReadEnable(pUartDB);
            if (FLAG_UartZD_HasData)
            {
                Uart_idleReadDisable(pUartZD);
                FLAG_UartZD_HasData = false;
                dwLen = dwUartCopy(pUartZD, ucApp_Buf_ZD2DB, pUartDB);
                ProtocolType = GetProtocolType(ucApp_Buf_ZD2DB, dwLen);
            }
            if (FLAG_UartDB_HasData)
            {
                Uart_idleReadDisable(pUartDB);
                FLAG_UartDB_HasData = false;
                dwLen = dwUartCopy(pUartDB, ucApp_Buf_DB2ZD, pUartZD);
                ProtocolType = GetProtocolType(ucApp_Buf_DB2ZD, dwLen);
            }
            break;
        }
        case dlt698:
        {
            Uart_idleReadEnable(pUartZD);
            Uart_idleReadEnable(pUartDB);
            if (FLAG_UartZD_HasData)
            {
                Uart_idleReadDisable(pUartZD);
                FLAG_UartZD_HasData = false;
                dwLen = dwUartCopy(pUartZD, ucApp_Buf_ZD2DB, pUartDB);
                //if (pframe = pGetpFrame(ucApp_Buf_ZD2DB, dwLen) == NULL)
                //break;
            }
            if (FLAG_UartDB_HasData)
            {
                Uart_idleReadDisable(pUartDB);
                FLAG_UartDB_HasData = false;
                pframe = NULL;

                /*������֡*/
                if ((dwLen = Uart_Read(pUartDB, ucApp_Buf_DB2ZD, DRV_BUF_SIZE)) == 0)
                    break;

                /*��֡����*/
                if ((pframe = pGetpFrame(ucApp_Buf_DB2ZD, dwLen)) == NULL)
                    break;

                /*��ѹ��ȡ����*/
                if ((dwLen = dwGetApdu(pframe, pAPDU)) > 0)
                //if ((dwLen = dwGetApdu(ucApp_Buf_DB2ZD, pAPDU)) > 0)
                {
                    if (1 == dwAPduAnalyze(pAPDU, dwLen, &stCollData))
                    {
                        for (i = 0; i < stCollData.ucDataNum; i++)
                        {
                            if (stCollData.stDataUnit[i].stOAD.OI_date == 0x2000) /*��ѹOI*/
                            {
                                /*uwLen:
                               ucPrt--->    01 ���� ����=1����ʾ����  
                                            03 ���� ����Ԫ�ظ���=3
                                            12 09 6D ���� Ԫ��1������18��long-unsigned 241.3V A��
                                            12 09 6D ���� Ԫ��2������18��long-unsigned 241.3V B��
                                            12 09 6D ���� Ԫ��3������18��long-unsigned 241.3V C��
                                */
                                A_Voltage = MAKE_WORD(*(stCollData.stDataUnit[i].ucPtr + 3), *(stCollData.stDataUnit[i].ucPtr + 4));
                                B_Voltage = MAKE_WORD(*(stCollData.stDataUnit[i].ucPtr + 6), *(stCollData.stDataUnit[i].ucPtr + 7));
                                A_Voltage = MAKE_WORD(*(stCollData.stDataUnit[i].ucPtr + 9), *(stCollData.stDataUnit[i].ucPtr + 10));
                                //stCollData.stDataUnit[i].uwLen;
                                //stCollData.stDataUnit[i].ucVal;
                            }
                        }
                    }
                }
                //voltage -> apdu
                //apdu -> frame
                //frame -> UartBuf
                //Uart_Write(DestUart, pdata, dwLen);
            }
            /*֡ת��*/
            break;
        }
        case dlt645:
        {
            /*�������շ�ʱι�����Ϊ1ms����*/
            vFeedExtWatchDog();
            /*������������֡��֡����*/
            if (blDrv_Buf_IsEmpty(pUartZD->pRsvbuf) == false)
            {
                RsvFrameHandle(ucApp_Buf_ZD2DB);
            }
            /*������֡����*/
            /*δ�յ����֡*/
            if (blDrv_Buf_IsEmpty(pUartDB->pRsvbuf) == true)
            {
                VoltageTimeOutHandle();
            }
            /*�յ����֡*/
            else
            {
                /*�Ƿ�֡*/
                if (!blRecvFrame(pUartDB->pRsvbuf, ucApp_Buf_DB2ZD))
                {
                    /*��ѹ��֡��ʱ*/
                    if ((Voltage_Frame_Status == FrameAwaitingReply) &&
                        (blTime2_Update(uwTime2FramesCounter, DLT645_FRAMES_DELAY_MAX)))
                    {
                        /*�м���ʹ���ϴε���ȷ��ѹֵ����ظ�*/
                        if (blVoltage_Create(ucApp_Buf_ZD2DB))
                        {
                            ucDrv_Buf_PutBytes(ucApp_Buf_ZD2DB, pUartZD->pSndbuf, ucApp_Buf_ZD2DB[9]);
                            /*���͵�ѹ֡*/
                            if (!blDrv_Buf_IsEmpty(pUartZD->pSndbuf))
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
                    ucDrv_Buf_PutBytes(ucApp_Buf_DB2ZD, pUartZD->pSndbuf, uc645DataDomainLength + 12);
                    if (!blDrv_Buf_IsEmpty(pUartZD->pSndbuf))
                    {
                        Uart_DataPut(pUartZD->handler);
                    }
                }
            }
            /*��ѡ���ܣ��������ò���*/
#ifdef ENABLE_INFR
            /*���⴮����֡���� */
            if (blDrv_Buf_IsEmpty(pUartIR->pRsvbuf) == false)
            {
                if (blRecvFrame(pUartIR->pRsvbuf, ucApp_Buf_INFR))
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
                        ucDrv_Buf_PutBytes(ucApp_Buf_INFR, pUartIR->pSndbuf, 18);
                    }
                    else
                    {
                        vINFRDataID_Handle(ucApp_Buf_INFR);
                    }
                    /*����ڻ�֡*/
                    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
                    {
                        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
                        //TIM_CCxCmd(TIM5, TIM_Channel_2, TIM_CCx_Enable);
                        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
                    }
                }
            } //end ������֡����
#endif
            break;
        } //end while
        default:
        {
            break;
        }
        } //end switch
    }     //end while
} //end main

void RsvFrameHandle(uint8_t *pucBuffer)
{
    /*֡�Ϸ����ж�*/
    if (blRecvFrame(pUartZD->pRsvbuf, pucBuffer))
    {
        /*�Ƿ��ٲ��ѹ*/
        if ((pucBuffer[8] == 0X11) &&
            (pucBuffer[10] == 0x33) &&
            (pucBuffer[12] == 0x34) &&
            (pucBuffer[13] == 0x35))
        {
            /*�ȴ���ѹ��֡״̬��λ*/
            Voltage_Frame_Status = FrameAwaitingReply;
            /*��ʼ��ʱ*/
            blFramesCounterStatus = true;
        }
        /*ת���ٲ�֡*/
        ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, uc645DataDomainLength + 12);
        if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
        {
            Uart_DataPut(pUartDB->handler);
        }
    }
}

void VoltageTimeOutHandle(void)
{
    /*�ٲ��ѹ��ʱ*/
    if ((Voltage_Frame_Status == FrameAwaitingReply) &&
        (blTime2_Update(uwTime2FramesCounter, DLT645_FRAMES_DELAY_MAX)))
    {
        /*�м���ʹ���ϴε���ȷ��ѹֵ����ظ�*/
        if (blVoltage_Create(ucApp_Buf_ZD2DB))
        {
            ucDrv_Buf_PutBytes(ucApp_Buf_ZD2DB, pUartZD->pSndbuf, ucApp_Buf_ZD2DB[9] + 12);
            if (!blDrv_Buf_IsEmpty(pUartZD->pSndbuf))
            {
                Uart_DataPut(pUartZD->handler);
                /*�ȴ���ѹ��֡״̬��λ*/
                Voltage_Frame_Status = FrameReplied;
                /*��ʱ����λ*/
                blFramesCounterStatus = false;
                uwTime2FramesCounter = 0;
            }
        }
    }
}
