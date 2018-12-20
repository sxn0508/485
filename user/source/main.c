#include "main.h"
#include "GPIO.h"
#include "Uart.h"
#include "DLT645.h"
#include "Flash.h"
#include "Time.h"
#include "protocol.h"
#include "DLT698.h"
#include "DLT698_45.h"

uint32_t gSystickAccuracy; //1秒内的SysTick数

/*698数据存储单元*/
COLL_STORE_DATA stCollData;
/*串口收帧标志*/
bool FLAG_UartZD_HasData = false;
bool FLAG_UartDB_HasData = false;
/*自动波特率识别相关参数*/
struct AUTO_BAUD AutoBaud485 =
    {
        .irqCount = 100,
        .uwBaudelay = 0xFFFFFFFF,
        .Uart_BaudRate = 0,
};
/*上电后延时启动参数*/
uint32_t uwBootDelay = 2000000;

static void vLedRun(uint32_t delay);
static void VoltageTimeOutHandle(void);
static void RsvFrameHandle(uint8_t *pucBuffer);
static void vAutoSetBaudrate(void);
static uint32_t uwFindBaudRate(uint32_t baudDelay);

int main(void)
{
    /*485口波特率类型*/
    ProtocolDef ProtocolType = none;
    /*698帧APDU数据段指针*/
    uint8_t *pAPDU = NULL;
    uint32_t dwLen = 0;
    uint32_t i;
    /*698帧链路层参数*/
    DLT698_FRAME dlt698Frame;
    vGPIO_Init();
    /*集中器上电过程中485总线有乱码，此时不能识别波特率*/
    while (uwBootDelay--)
    {
        vFeedExtWatchDog();
    }
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    vInterFlash_Init();
    vFeedExtWatchDog();
    vTimer_Init();
    vLed_Light();
    vSystickSetAccuracy(1000000); //SysTick = 1us
    vSystickIntCmd(ENABLE);
    /*TICK使能后才能初始化外部中断*/
    vExti_Init(); //
    vUart_EnableBaudrate(pUartIR, 1200);
    /*电压修改的上下限值设为默认值；开启修改功能 */
    Voltage_Change_Init();
    /*电流重过载参数设置*/
    vCurrent_Limit_DefaultSet();
    /*电量修改的上下限值设为默认值；开启修改功能 */
    vEnergy_Modify_Init();
    while (1)
    {
        vFeedExtWatchDog();
        /*红外串口收帧处理 */
        if (blDrv_Buf_IsEmpty(pUartIR->pRsvbuf) == false)
        {
            if (blRecvFrame(pUartIR->pRsvbuf, ucApp_Buf_INFR))
            {
                vINFRDataID_Handle(ucApp_Buf_INFR);
                /*红外口回帧*/
                if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
                {
                    USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
                    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
                }
            }
        } //end 红外收帧处理
        /*485口收帧处理*/
        switch (ProtocolType)
        {
        case none:
        {
            vFeedExtWatchDog();
            vLedRun(TICK_200MS);
            if (AutoBaud485.Uart_BaudRate == 0)
            {
                if (AutoBaud485.irqCount > 0)
                {
                    break;
                }
                else
                {
                    EXTI_DeInit();
                    AutoBaud485.Uart_BaudRate = uwFindBaudRate(AutoBaud485.uwBaudelay);
                    vSystickSetAccuracy(1000); //systick=1ms
                    vSystickIntCmd(ENABLE);
                    vUart_EnableBaudrate(pUartZD, AutoBaud485.Uart_BaudRate); //使能接收中断
                    vUart_EnableBaudrate(pUartDB, AutoBaud485.Uart_BaudRate);
                    /*接收后续字节并忽略*/
                    vFeedExtWatchDog();
                    Uart_IdleRead(pUartZD, ucApp_Buf_ZD2DB, DRV_BUF_SIZE, TICK_500MS);
                    vBuf_Clear(pUartZD->pRsvbuf);
                    Uart_idleReadEnable(pUartZD);
                    Uart_idleReadEnable(pUartDB);
                }
            }
            else
            {
                /*识别规约类型*/
                if (FLAG_UartZD_HasData)
                {
                    Uart_idleReadDisable(pUartZD);
                    FLAG_UartZD_HasData = false;
                    vFeedExtWatchDog();
                    dwLen = dwUartCopy(pUartZD, ucApp_Buf_ZD2DB, pUartDB);
                    ProtocolType = GetProtocolType(ucApp_Buf_ZD2DB, dwLen);
                    Uart_idleReadEnable(pUartZD);
                }
                if (FLAG_UartDB_HasData)
                {
                    Uart_idleReadDisable(pUartDB);
                    FLAG_UartDB_HasData = false;
                    vFeedExtWatchDog();
                    dwLen = dwUartCopy(pUartDB, ucApp_Buf_DB2ZD, pUartZD);
                    ProtocolType = GetProtocolType(ucApp_Buf_DB2ZD, dwLen); //校验错误也嫩过？
                    Uart_idleReadEnable(pUartDB);
                }
            }
            break;
        }
        case dlt698:
        {
            vFeedExtWatchDog();
            vLedRun(TICK_1S);
            Uart_idleReadEnable(pUartZD);
            Uart_idleReadEnable(pUartDB);
            if (FLAG_UartZD_HasData)
            {
                Uart_idleReadDisable(pUartZD);
                FLAG_UartZD_HasData = false;
                vFeedExtWatchDog();
                dwLen = dwUartCopy(pUartZD, ucApp_Buf_ZD2DB, pUartDB);
                Uart_idleReadEnable(pUartZD);
                break;
            }
            if (FLAG_UartDB_HasData)
            {
                vFeedExtWatchDog();
                Uart_idleReadDisable(pUartDB);
                FLAG_UartDB_HasData = false;
                dlt698Frame.pStart = NULL;

                /*电表口收帧*/
                if ((dwLen = Uart_Read(pUartDB, ucApp_Buf_DB2ZD, DRV_BUF_SIZE)) == 0)
                    break;
                if ((pGetpFrame(ucApp_Buf_DB2ZD, dwLen, &dlt698Frame)) == NULL ||
                    Voltage_Change_State == Voltage_NOChange)
                {
                    vFeedExtWatchDog();
                    Uart_OnceWrite(pUartZD, ucApp_Buf_DB2ZD, dwLen, 1000 * TICK_1MS);
                }
                else
                {
                    if ((dwLen = dwGet698Apdu(&dlt698Frame, &pAPDU)) > 0)
                    {
                        if (dwAPduAnalyze(pAPDU, dwLen, &stCollData) > 0)
                        {
                            for (i = 0; i < stCollData.ucDataNum; i++)
                            {
                                if (stCollData.stDataUnit[i].stOAD.OI_date == 0x2000) /*电压OI*/
                                {
                                    v698VoltageModify(&stCollData.stDataUnit[i]);
                                }
                            }
                            dwReCalculateFCS(&dlt698Frame);
                        }
                    }
                    vFeedExtWatchDog();
                    Uart_OnceWrite(pUartZD, dlt698Frame.pStart, dlt698Frame.uwFramelen + 2, TICK_500MS);
                }
            }
            Uart_idleReadEnable(pUartDB);
            break;
        }
        case dlt645:
        {
            vLedRun(TICK_1S);
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
            /*该部分移植到规约状态机之外，与698规约共用*/
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

static void VoltageTimeOutHandle(void)
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
/********************************************************************
* 功    能：根据串口1位数据电平持续时间计算波特率
* 输    入：电平持续时间
* 输    出：对应波特率
* 说    名：
* 编 写 人：
* 编写日期：2018年9月10日16:01:21
**********************************************************************/
uint32_t uwFindBaudRate(uint32_t baudDelay)
{
    /* Baud 1200 :1666us*/
    //if (baudDelay > 1500 && baudDelay < 1800)
    //   return 1200;
    /* Baud 1200:833us*/
    if (baudDelay > 820 && baudDelay < 850)
        return 1200;
    /* Baud 2400:417us*/
    else if (baudDelay > 400 && baudDelay < 430)
        return 2400;
    /* Baud 4800:208us*/
    else if (baudDelay > 190 && baudDelay < 220)
        return 4800;
    /* Baud 9600 :104us*/
    else if (baudDelay > 90 && baudDelay < 120)
        return 9600;
    else
        /*default baudrate*/
        return 2400;
}

/********************************************************************
* 功    能：计算串口1位数据电平持续时间
* 输    入：持续时间存储位置
* 输    出：None
* 说    明：中断服务函数中记录n个中断的时标
*           相邻时标计算脉冲持续时间
*           查找最短持续时间
* 编 写 人：
* 编写日期：2018年9月5日15:08:58
**********************************************************************/
void vUartZD_BaudelayCal(volatile uint32_t *pBaudelay)
{

    static uint32_t LAST_TICK = 0xFFFFFFFF;
    uint32_t nowtick = 0;
    uint32_t tmpdelay = 0;
    AutoBaud485.irqCount--;
    /*取当前tick数，0~0xFFFF循环*/
    nowtick = TICKS;
    /*初次收到串口数据，系统tick最大65535*/
    if (LAST_TICK != 0xFFFFFFFF)
    {
        /*与lastick相减得到本次delay*/
        tmpdelay = (nowtick + 0xFFFF - LAST_TICK) & 0xFFFF;
        /*当前tick数存为count*/
        LAST_TICK = nowtick;
        /*与上次delay比较，取较小值存为delay*/
        if (tmpdelay < *pBaudelay)
        {
            *pBaudelay = tmpdelay;
        }
    }
    else //之前已有边沿中断
    {
        /*当前tick数存为lastick*/
        /*delay= 0xFFFFFFFF*/
        LAST_TICK = nowtick;
    }
}

void vAutoSetBaudrate(void)
{
    vSystickSetAccuracy(1000000); //SysTick = 1us
    vSystickIntCmd(ENABLE);
    /*TICK使能后才能初始化外部中断*/
    vExti_Init(); //
    AutoBaud485.irqCount = 100;
    AutoBaud485.uwBaudelay = 0xFFFFFFFF;
    while (AutoBaud485.irqCount)
    {
        vFeedExtWatchDog();
    }
    EXTI_DeInit();
    AutoBaud485.Uart_BaudRate = uwFindBaudRate(AutoBaud485.uwBaudelay);
    vSystickSetAccuracy(1000); //systick=1ms
    vSystickIntCmd(ENABLE);
    vUart_EnableBaudrate(pUartZD, AutoBaud485.Uart_BaudRate); //使能接收中断
    vUart_EnableBaudrate(pUartDB, AutoBaud485.Uart_BaudRate);
    /*接收后续字节并忽略*/
    vFeedExtWatchDog();
    Uart_IdleRead(pUartZD, ucApp_Buf_ZD2DB, DRV_BUF_SIZE, TICK_500MS);
    vBuf_Clear(pUartZD->pRsvbuf);

#if 0
    Uart_ITReadEnable(pUartZD);
    /*last loop do not clear FLAG,second loop do not receive second frame*/
    while (!FLAG_UartZD_HasData)
    {
        vFeedExtWatchDog();
    }
    Uart_ITReadDisable(pUartZD);
    FLAG_UartZD_HasData = false;
    pdataDown->len = Uart_Read(pUartZD, (uint8_t *)pdataDown, BUF_SIZE);
    if (pdataDown->len == 0)
    {
        UartDB_BaudRate = 0;
        continue;
    }
    do
    {
        vFeedExtWatchDog();
        vUart_EnableBaudrate(pUartDB, UartDB_BaudRate);
        Uart_OnceWrite(pUartDB, (uint8_t *)pdataDown, pdataDown->len, 1000);
        pdataUp->len = Uart_IdleRead(pUartDB, (uint8_t *)pdataUp, BUF_SIZE, 500);
        pframe = pGetpFrame((uint8_t *)pdataUp, pdataUp->len);
        /*9600 -> 4800 -> 2400 -> 1200*/
        UartDB_BaudRate = UartDB_BaudRate >> 1;
    } while (pframe == NULL && UartDB_BaudRate > 600);
    /*if no reply from DB,UartDB_BaudRate always == 600*/
    //} while (UartDB_BaudRate < 1200); //波特率匹配完成
#endif
}
void vLedRun(uint32_t delay)
{
    static uint32_t last_tick = 0;
    if (TICKS - last_tick > delay)
    {
        last_tick = TICKS;
        if (Bit_SET == GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13))
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET); //拉低PC13
        }
        else
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
        }
    }
}