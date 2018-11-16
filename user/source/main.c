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
//#define TEST

int main(void)
{
    ProtocolDef ProtocolType = none;
#ifdef TEST
    uint8_t tmpAPDU[MAX_APDU_LENGTH];
    uint8_t *ptmpAPDU = tmpAPDU;
#endif
    uint8_t *pAPDU = NULL;
    uint32_t dwLen = 0;
    uint8_t ucVoltageIdex;
    //uint8_t *pframe = NULL;
    uint32_t i;
    //uint32_t test;
    DLT698_FRAME dlt698Frame;

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
#ifdef TEST
    //response normal 85 01 2a 20 00 02 00 01 01 03 12 08 d6 12 00 00 12 00 00 00 00
    tmpAPDU[0] = 0x85;
    tmpAPDU[1] = 0x01;
    tmpAPDU[2] = 0x2a;
    tmpAPDU[3] = 0x20;
    tmpAPDU[4] = 0x00;
    tmpAPDU[5] = 0x02;
    tmpAPDU[6] = 0x00;
    tmpAPDU[7] = 0x01;
    tmpAPDU[8] = 0x01;
    tmpAPDU[9] = 0x03;
    tmpAPDU[10] = 0x12;
    tmpAPDU[11] = 0x08;
    tmpAPDU[12] = 0xd6;
    tmpAPDU[13] = 0x12;
    tmpAPDU[14] = 0x00;
    tmpAPDU[15] = 0x00;
    tmpAPDU[16] = 0x12;
    tmpAPDU[17] = 0x00;
    tmpAPDU[18] = 0x00;
    tmpAPDU[19] = 0x00;
    tmpAPDU[20] = 0x00;
//response record 85 03 27 50 04 02 00 02 00 20 21 02 00 00 20 00 02 00 01 01 1c 07 e2 0b 0f 00 00 00 00 00 00
#ifdef RECORD_TEST
    tmpAPDU[0] = 0x85;
    tmpAPDU[1] = 0x03;
    tmpAPDU[2] = 0x2d;
    tmpAPDU[3] = 0x50;
    tmpAPDU[4] = 0x04;
    tmpAPDU[5] = 0x02;
    tmpAPDU[6] = 0x00;
    tmpAPDU[7] = 0x02;
    tmpAPDU[8] = 0x00;
    tmpAPDU[9] = 0x20;
    tmpAPDU[10] = 0x21;
    tmpAPDU[11] = 0x02;
    tmpAPDU[12] = 0x00;
    tmpAPDU[13] = 0x00;
    tmpAPDU[14] = 0x20;
    tmpAPDU[15] = 0x00;
    tmpAPDU[16] = 0x02;
    tmpAPDU[17] = 0x00;
    tmpAPDU[18] = 0x01;
    tmpAPDU[19] = 0x01;
    tmpAPDU[20] = 0x1c;
    tmpAPDU[21] = 0x07;
    tmpAPDU[22] = 0xe2;
    tmpAPDU[23] = 0x0b;
    tmpAPDU[24] = 0x0f;
    tmpAPDU[25] = 0x00;
    tmpAPDU[26] = 0x00;
    tmpAPDU[27] = 0x00;
    tmpAPDU[28] = 0x00;
    tmpAPDU[29] = 0x00;
    tmpAPDU[30] = 0x00;
#endif
    if (dwAPduAnalyze(ptmpAPDU, 31, &stCollData) >= 0)
    {
        for (i = 0; i < stCollData.ucDataNum; i++)
        {
            if (stCollData.stDataUnit[i].stOAD.OI_date == 0x2000) /*��ѹOI*/
            {
                v698VoltageModify(&stCollData.stDataUnit[i]);
            }
        }
    }
#endif
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
                ProtocolType = GetProtocolType(ucApp_Buf_DB2ZD, dwLen); //У�����Ҳ�۹���
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
                dlt698Frame.pStart = NULL;
                //dwLen = 0;

                /*������֡*/
                if ((dwLen = Uart_Read(pUartDB, ucApp_Buf_DB2ZD, DRV_BUF_SIZE)) == 0)
                    break;

//if (Voltage_Change_State == Voltage_NOChange)
//{
//Uart_OnceWrite(pUartZD, ucApp_Buf_DB2ZD, dwLen, TICKS_1000MS);
//break;
//}

/*��֡����*/
//if ((pGetpFrame(ucApp_Buf_DB2ZD, dwLen, &dlt698Frame)) == NULL) //У�����Ͳ�͸����
//{
//Uart_OnceWrite(pUartZD, ucApp_Buf_DB2ZD, dwLen, TICKS_1000MS);
//break;
//}

/*��ѹ��ȡ����*/
//if ((dwLen = dwGet698Apdu(&dlt698Frame, &pAPDU)) <= 0)
//break;

//if (dwAPduAnalyze(pAPDU, dwLen, &stCollData) < 0)
#if 1
                if ((pGetpFrame(ucApp_Buf_DB2ZD, dwLen, &dlt698Frame)) == NULL) //У�����Ͳ�͸����
                {
                    Uart_OnceWrite(pUartZD, ucApp_Buf_DB2ZD, dwLen, TICKS_1000MS);
                }
                //if ((pGetpFrame(ucApp_Buf_DB2ZD, dwLen, &dlt698Frame)) != NULL) //У�����Ͳ�͸����
                else if (Voltage_Change_State != Voltage_NOChange)
                {
                    if ((dwLen = dwGet698Apdu(&dlt698Frame, &pAPDU)) > 0)
                    {
                        if (dwAPduAnalyze(pAPDU, dwLen, &stCollData) > 0)
                        {
#endif
                            for (i = 0; i < stCollData.ucDataNum; i++)
                            {
                                if (stCollData.stDataUnit[i].stOAD.OI_date == 0x2000) /*��ѹOI*/
                                {
                                    v698VoltageModify(&stCollData.stDataUnit[i]);
                                }
                            }
                            dwReCalculateFCS(&dlt698Frame);
                        }
                    }
                    Uart_OnceWrite(pUartZD, dlt698Frame.pStart, dlt698Frame.uwFramelen + 2, TICKS_500MS);
                }
                //frame -> UartBuf
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

static void RsvFrameHandle(uint8_t *pucBuffer)
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

static void VoltageTimeOutHandle(void)
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