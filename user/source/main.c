#include "main.h"
#include "GPIO.h"
#include "Uart.h"
#include "DLT645.h"
#include "Flash.h"
#include "Time.h"
#include "protocol.h"
#include "DLT698.h"
#include "DLT698_45.h"

uint32_t gSystickAccuracy; //1���ڵ�SysTick��

/*698���ݴ洢��Ԫ*/
COLL_STORE_DATA stCollData;
/*������֡��־*/
bool FLAG_UartZD_HasData = false;
bool FLAG_UartDB_HasData = false;
/*�Զ�������ʶ����ز���*/
struct AUTO_BAUD AutoBaud485 =
    {
        .irqCount = 100,
        .uwBaudelay = 0xFFFFFFFF,
        .Uart_BaudRate = 0,
};
/*�ϵ����ʱ��������*/
uint32_t uwBootDelay = 2000000;

static void vLedRun(uint32_t delay);
static void VoltageTimeOutHandle(void);
static void RsvFrameHandle(uint8_t *pucBuffer);
static void vAutoSetBaudrate(void);
static uint32_t uwFindBaudRate(uint32_t baudDelay);

int main(void)
{
    /*485�ڲ���������*/
    ProtocolDef ProtocolType = none;
    /*698֡APDU���ݶ�ָ��*/
    uint8_t *pAPDU = NULL;
    uint32_t dwLen = 0;
    uint32_t i;
    /*698֡��·�����*/
    DLT698_FRAME dlt698Frame;
    vGPIO_Init();
    /*�������ϵ������485���������룬��ʱ����ʶ������*/
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
    /*TICKʹ�ܺ���ܳ�ʼ���ⲿ�ж�*/
    vExti_Init(); //
    vUart_EnableBaudrate(pUartIR, 1200);
    /*��ѹ�޸ĵ�������ֵ��ΪĬ��ֵ�������޸Ĺ��� */
    Voltage_Change_Init();
    /*�����ع��ز�������*/
    vCurrent_Limit_DefaultSet();
    /*�����޸ĵ�������ֵ��ΪĬ��ֵ�������޸Ĺ��� */
    vEnergy_Modify_Init();
    while (1)
    {
        vFeedExtWatchDog();
        /*���⴮����֡���� */
        if (blDrv_Buf_IsEmpty(pUartIR->pRsvbuf) == false)
        {
            if (blRecvFrame(pUartIR->pRsvbuf, ucApp_Buf_INFR))
            {
                vINFRDataID_Handle(ucApp_Buf_INFR);
                /*����ڻ�֡*/
                if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
                {
                    USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
                    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
                }
            }
        } //end ������֡����
        /*485����֡����*/
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
                    vUart_EnableBaudrate(pUartZD, AutoBaud485.Uart_BaudRate); //ʹ�ܽ����ж�
                    vUart_EnableBaudrate(pUartDB, AutoBaud485.Uart_BaudRate);
                    /*���պ����ֽڲ�����*/
                    vFeedExtWatchDog();
                    Uart_IdleRead(pUartZD, ucApp_Buf_ZD2DB, DRV_BUF_SIZE, TICK_500MS);
                    vBuf_Clear(pUartZD->pRsvbuf);
                    Uart_idleReadEnable(pUartZD);
                    Uart_idleReadEnable(pUartDB);
                }
            }
            else
            {
                /*ʶ���Լ����*/
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
                    ProtocolType = GetProtocolType(ucApp_Buf_DB2ZD, dwLen); //У�����Ҳ�۹���
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

                /*������֡*/
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
                                if (stCollData.stDataUnit[i].stOAD.OI_date == 0x2000) /*��ѹOI*/
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
            /*�ò�����ֲ����Լ״̬��֮�⣬��698��Լ����*/
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
/********************************************************************
* ��    �ܣ����ݴ���1λ���ݵ�ƽ����ʱ����㲨����
* ��    �룺��ƽ����ʱ��
* ��    ������Ӧ������
* ˵    ����
* �� д �ˣ�
* ��д���ڣ�2018��9��10��16:01:21
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
* ��    �ܣ����㴮��1λ���ݵ�ƽ����ʱ��
* ��    �룺����ʱ��洢λ��
* ��    ����None
* ˵    �����жϷ������м�¼n���жϵ�ʱ��
*           ����ʱ������������ʱ��
*           ������̳���ʱ��
* �� д �ˣ�
* ��д���ڣ�2018��9��5��15:08:58
**********************************************************************/
void vUartZD_BaudelayCal(volatile uint32_t *pBaudelay)
{

    static uint32_t LAST_TICK = 0xFFFFFFFF;
    uint32_t nowtick = 0;
    uint32_t tmpdelay = 0;
    AutoBaud485.irqCount--;
    /*ȡ��ǰtick����0~0xFFFFѭ��*/
    nowtick = TICKS;
    /*�����յ��������ݣ�ϵͳtick���65535*/
    if (LAST_TICK != 0xFFFFFFFF)
    {
        /*��lastick����õ�����delay*/
        tmpdelay = (nowtick + 0xFFFF - LAST_TICK) & 0xFFFF;
        /*��ǰtick����Ϊcount*/
        LAST_TICK = nowtick;
        /*���ϴ�delay�Ƚϣ�ȡ��Сֵ��Ϊdelay*/
        if (tmpdelay < *pBaudelay)
        {
            *pBaudelay = tmpdelay;
        }
    }
    else //֮ǰ���б����ж�
    {
        /*��ǰtick����Ϊlastick*/
        /*delay= 0xFFFFFFFF*/
        LAST_TICK = nowtick;
    }
}

void vAutoSetBaudrate(void)
{
    vSystickSetAccuracy(1000000); //SysTick = 1us
    vSystickIntCmd(ENABLE);
    /*TICKʹ�ܺ���ܳ�ʼ���ⲿ�ж�*/
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
    vUart_EnableBaudrate(pUartZD, AutoBaud485.Uart_BaudRate); //ʹ�ܽ����ж�
    vUart_EnableBaudrate(pUartDB, AutoBaud485.Uart_BaudRate);
    /*���պ����ֽڲ�����*/
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
    //} while (UartDB_BaudRate < 1200); //������ƥ�����
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
            GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET); //����PC13
        }
        else
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
        }
    }
}