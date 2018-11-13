#include "main.h"
#include "GPIO.h"
#include "Uart.h"
#include "DLT645.h"
#include "Flash.h"
#include "Time.h"
#include "protocol.h"
#include "DLT698.h"
#include "DLT698_45.h"

/*698数据存储单元*/
COLL_STORE_DATA stCollData;
/*串口收帧标志*/
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

    /*配置中断向量组*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SysTick_Config(SysTick_100Ms);
    /*SysTick使用系统频率64M的8分频：8MHz*/
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    NVIC_SetPriority(SysTick_IRQn, -1);
    vInterFlash_Init();
    vGPIO_Init();
    vFeedExtWatchDog();
    vUart_Init(); //RXNE中断使能
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
    while (1)
    {
        switch (ProtocolType)
        {
        case none:
        {
            /*识别规约类型*/
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

                /*电表口收帧*/
                if ((dwLen = Uart_Read(pUartDB, ucApp_Buf_DB2ZD, DRV_BUF_SIZE)) == 0)
                    break;

                /*收帧处理*/
                if ((pframe = pGetpFrame(ucApp_Buf_DB2ZD, dwLen)) == NULL)
                    break;

                /*电压读取部分*/
                if ((dwLen = dwGetApdu(pframe, pAPDU)) > 0)
                //if ((dwLen = dwGetApdu(ucApp_Buf_DB2ZD, pAPDU)) > 0)
                {
                    if (1 == dwAPduAnalyze(pAPDU, dwLen, &stCollData))
                    {
                        for (i = 0; i < stCollData.ucDataNum; i++)
                        {
                            if (stCollData.stDataUnit[i].stOAD.OI_date == 0x2000) /*电压OI*/
                            {
                                /*uwLen:
                               ucPrt--->    01 —— 类型=1，表示数组  
                                            03 —— 数组元素个数=3
                                            12 09 6D —— 元素1：类型18：long-unsigned 241.3V A相
                                            12 09 6D —— 元素2：类型18：long-unsigned 241.3V B相
                                            12 09 6D —— 元素3：类型18：long-unsigned 241.3V C相
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
            /*帧转发*/
            break;
        }
        case dlt645:
        {
            /*串口无收发时喂狗间隔为1ms左右*/
            vFeedExtWatchDog();
            /*集中器侧数据帧收帧处理*/
            if (blDrv_Buf_IsEmpty(pUartZD->pRsvbuf) == false)
            {
                RsvFrameHandle(ucApp_Buf_ZD2DB);
            }
            /*电表侧收帧处理*/
            /*未收到电表帧*/
            if (blDrv_Buf_IsEmpty(pUartDB->pRsvbuf) == true)
            {
                VoltageTimeOutHandle();
            }
            /*收到电表帧*/
            else
            {
                /*非法帧*/
                if (!blRecvFrame(pUartDB->pRsvbuf, ucApp_Buf_DB2ZD))
                {
                    /*电压回帧超时*/
                    if ((Voltage_Frame_Status == FrameAwaitingReply) &&
                        (blTime2_Update(uwTime2FramesCounter, DLT645_FRAMES_DELAY_MAX)))
                    {
                        /*中继器使用上次的正确电压值替代回复*/
                        if (blVoltage_Create(ucApp_Buf_ZD2DB))
                        {
                            ucDrv_Buf_PutBytes(ucApp_Buf_ZD2DB, pUartZD->pSndbuf, ucApp_Buf_ZD2DB[9]);
                            /*发送电压帧*/
                            if (!blDrv_Buf_IsEmpty(pUartZD->pSndbuf))
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
                    ucDrv_Buf_PutBytes(ucApp_Buf_DB2ZD, pUartZD->pSndbuf, uc645DataDomainLength + 12);
                    if (!blDrv_Buf_IsEmpty(pUartZD->pSndbuf))
                    {
                        Uart_DataPut(pUartZD->handler);
                    }
                }
            }
            /*可选功能：红外设置参数*/
#ifdef ENABLE_INFR
            /*红外串口收帧处理 */
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
                    /*红外口回帧*/
                    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
                    {
                        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
                        //TIM_CCxCmd(TIM5, TIM_Channel_2, TIM_CCx_Enable);
                        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
                    }
                }
            } //end 红外收帧处理
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
    /*帧合法性判断*/
    if (blRecvFrame(pUartZD->pRsvbuf, pucBuffer))
    {
        /*是否召测电压*/
        if ((pucBuffer[8] == 0X11) &&
            (pucBuffer[10] == 0x33) &&
            (pucBuffer[12] == 0x34) &&
            (pucBuffer[13] == 0x35))
        {
            /*等待电压回帧状态置位*/
            Voltage_Frame_Status = FrameAwaitingReply;
            /*开始计时*/
            blFramesCounterStatus = true;
        }
        /*转发召测帧*/
        ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, uc645DataDomainLength + 12);
        if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
        {
            Uart_DataPut(pUartDB->handler);
        }
    }
}

void VoltageTimeOutHandle(void)
{
    /*召测电压超时*/
    if ((Voltage_Frame_Status == FrameAwaitingReply) &&
        (blTime2_Update(uwTime2FramesCounter, DLT645_FRAMES_DELAY_MAX)))
    {
        /*中继器使用上次的正确电压值替代回复*/
        if (blVoltage_Create(ucApp_Buf_ZD2DB))
        {
            ucDrv_Buf_PutBytes(ucApp_Buf_ZD2DB, pUartZD->pSndbuf, ucApp_Buf_ZD2DB[9] + 12);
            if (!blDrv_Buf_IsEmpty(pUartZD->pSndbuf))
            {
                Uart_DataPut(pUartZD->handler);
                /*等待电压回帧状态清位*/
                Voltage_Frame_Status = FrameReplied;
                /*计时器复位*/
                blFramesCounterStatus = false;
                uwTime2FramesCounter = 0;
            }
        }
    }
}
