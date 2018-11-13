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

static void vIRQ_UART_Config(void); //�����жϳ�ʼ��
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
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    USART_OverSampling8Cmd(USART1, ENABLE);
    USART_Initstruc.USART_BaudRate = 2400;
    USART_Initstruc.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
    USART_Initstruc.USART_WordLength = USART_WordLength_9b;
    USART_Initstruc.USART_Parity = USART_Parity_Even;
    USART_Initstruc.USART_StopBits = USART_StopBits_1;
    USART_Initstruc.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_Initstruc);
    /*USART3����*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    USART_OverSampling8Cmd(USART3, ENABLE);
    USART_Initstruc.USART_BaudRate = 2400;
    USART_Initstruc.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
    USART_Initstruc.USART_WordLength = USART_WordLength_9b;
    USART_Initstruc.USART_Parity = USART_Parity_Even;
    USART_Initstruc.USART_StopBits = USART_StopBits_1;
    USART_Initstruc.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_Initstruc);
    /*USART2����Ϊ��׼����*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    USART_OverSampling8Cmd(USART2, ENABLE);
    USART_Initstruc.USART_BaudRate = 1200;
    USART_Initstruc.USART_WordLength = USART_WordLength_9b;
    USART_Initstruc.USART_StopBits = USART_StopBits_1;
    USART_Initstruc.USART_Parity = USART_Parity_Even;
    USART_Initstruc.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Initstruc.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_Initstruc);
    vIRQ_UART_Config(); //�ж�����,ʹ��
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
    USART_Cmd(USART3, ENABLE);
    USART_Cmd(USART2, ENABLE);
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
    NVIC_Initstruc.NVIC_IRQChannel = USART1_IRQn;
    NVIC_Initstruc.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Initstruc.NVIC_IRQChannelSubPriority = 1;
    NVIC_Initstruc.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Initstruc);
    /*���ô���3�ж�*/
    NVIC_Initstruc.NVIC_IRQChannel = USART3_IRQn;
    NVIC_Initstruc.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_Initstruc);
    /*���ô���2�ж�*/
    NVIC_Initstruc.NVIC_IRQChannel = USART2_IRQn;
    NVIC_Initstruc.NVIC_IRQChannelSubPriority = 2;
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
bool blDrv_Buf_IsEmpty(const struct ucbuf *ucdrv_buf)
{
    if (ucdrv_buf->rd == ucdrv_buf->wr)
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
bool blDrv_Buf_IsFull(const struct ucbuf *ucdrv_buf)
{
    if (ucdrv_buf->rd == (ucdrv_buf->wr + 1) % DRV_BUF_SIZE)
        return true;
    else
        return false;
}
/********************************************************************
* ��    �ܣ������������ȡָ���������ֽ�
* ��    �룺��������ṹ���ַ�����ݻ���ṹ���ַ����ȡ�ֽ���
* ��    ����true/false
*          
* �� д �ˣ�������
* ��д���ڣ�2016.6.28
**********************************************************************/
bool ucDrv_Buf_GetBytes(struct ucbuf *ucdrv_buf, uint8_t *ucdata_buf, uint16_t length)
{
    uint16_t i;
    if (uwBuf_UnReadLen(ucdrv_buf) >= length)
    {
        for (i = 0; i < length; i++)
        {
            *(ucdata_buf + i) = ucdrv_buf->data[ucdrv_buf->rd];
            ucdrv_buf->rd = (ucdrv_buf->rd + 1) % DRV_BUF_SIZE;
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
bool ucDrv_Buf_PutBytes(uint8_t *ucdata_buf, struct ucbuf *ucdrv_buf, uint16_t length)
{
    uint16_t i;
    if (DRV_BUF_SIZE >= uwBuf_UnReadLen(ucdrv_buf) + length)
    {
        for (i = 0; i < length; i++)
        {
            ucdrv_buf->data[ucdrv_buf->wr] = *(ucdata_buf + i);
            ucdrv_buf->wr = (ucdrv_buf->wr + 1) % DRV_BUF_SIZE;
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
void Uart_DataPut(USART_TypeDef *USART_X)
{
    USART_ITConfig(USART_X, USART_IT_TXE, ENABLE);
}

#if 1
/***************************************************************
* ��������: Uart_Write
* ��������: д���ڣ��жϷ��ͣ������ʱ��delay
* �������: uart��ϵͳ����  buf:�洢λ��   count����������
* �������: ��
* �� �� ֵ: > 0��ʵ�ʷ��͵�����   -1������ʧ��
****************************************************************/
int32_t Uart_OnceWrite(UartDef *puart, const uint8_t *pbuf, uint32_t count, uint32_t ticks)
{
    uint32_t sendlen = 0;
    uint16_t i;
    //��֤count��Ϊ0�������ٿ���һ�η����ж�
    if (!count || count > uwBuf_EmpLen(puart->pSndbuf))
        return -1;
    for (i = 0; i < count; i++)
    {
        puart->pSndbuf->data[puart->pSndbuf->wr] = *(pbuf + i);
        puart->pSndbuf->wr = (++puart->pSndbuf->wr) % DRV_BUF_SIZE;
    }
    sendlen = puart->pSndbuf->rd;
    //LL_USART_EnableIT_TXE(puart->handler);
    USART_ITConfig(puart->handler, USART_IT_TXE, ENABLE);
    while (ticks)
    {
        /*�������*/
        if (!uwBuf_UnReadLen(puart->pSndbuf))
            break;
        if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
        {
            ticks--;
        }
    }
    /*�涨ʱ����δ������ɣ�ҲҪ�ر��ж�*/
    //LL_USART_DisableIT_TXE(puart->handler);
    USART_ITConfig(puart->handler, USART_IT_TXE, DISABLE);
    return (puart->pSndbuf->rd - sendlen + DRV_BUF_SIZE) % DRV_BUF_SIZE;
}

/**********************************************
    * ��������: Uart_IdleRead
    * ��������: ������,�ֽڼ������idleMs�����߶�ȡ�����ﵽcountʱ��������
    * �������: puart�����ڽṹ��ָ��  buf:�洢λ��   count����ȡ����  idle���ֽڳ�ʱʱ��ms������Ϊ0
    * �������: ��
    * �� �� ֵ: > 0��ʵ�ʶ�ȡ������   0�������ݻ�count==0����� 
    ****************************************************************/
uint32_t Uart_IdleRead(UartDef *puart, uint8_t *buf, uint32_t count, uint32_t idleMs)
{
    volatile uint32_t readlen = 0;
    volatile uint32_t readlenow = 0;
    volatile idleMsCount;
    uint16_t i;
    /*��ȡ����> 0��<���泤��*/
    if (!count || count > DRV_BUF_SIZE)
    {
        return 0;
    }
    /*��֤��С��ʱ*/
    if (idleMs < 0xFFFFFFFF)
    {
        idleMsCount = ++idleMs;
    }
    USART_ITConfig(puart->handler, USART_IT_RXNE, ENABLE);
    readlenow = uwBuf_UnReadLen(puart->pRsvbuf);
    while (readlenow < count && idleMsCount > 0)
    {
        /*idleMsCount��ʱ�ۼ�*/
        if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
        {
            idleMsCount--;
        }
        readlenow = uwBuf_UnReadLen(puart->pRsvbuf);
        /*readlennow �б仯��idleMsCount����*/
        if (readlenow > readlen)
        {
            idleMsCount = idleMs;
            readlen = readlenow;
        }
    }
    USART_ITConfig(puart->handler, USART_IT_RXNE, DISABLE);
    if (readlen)
    {
        /*ʵ�ʶ����ĳ���*/
        readlen = (readlen < count ? readlen : count);
        for (i = 0; i < readlen; i++)
        {
            *(buf + i) = puart->pRsvbuf->data[puart->pRsvbuf->rd];
            puart->pRsvbuf->rd = (++puart->pRsvbuf->rd) % DRV_BUF_SIZE;
        }
    }
    return readlen;
}

/**********************************************
    * ��������: Uart_OnceRead
    * ��������: ������,����ָ���������ȡʱ�䳬ʱ���˳�
    * �������: puart�����ڽṹ��ָ��  buf:�洢λ��   count����ȡ����  delay���ֽڳ�ʱʱ��ms������Ϊ0
    * �������: ��
    * �� �� ֵ: > 0��ʵ�ʶ�ȡ������   0�������ݻ�count==0����� 
    ****************************************************************/
uint32_t Uart_OnceRead(UartDef *puart, uint8_t *buf, uint32_t count, uint32_t delay)
{
    volatile uint32_t readlen = 0;
    uint16_t i;
    /*��ȡ����> 0��<���泤��*/
    if (!count || count > DRV_BUF_SIZE)
    {
        return 0;
    }
    USART_ITConfig(puart->handler, USART_IT_RXNE, ENABLE);
    readlen = uwBuf_UnReadLen(puart->pRsvbuf);
    while (delay)
    {
        readlen = uwBuf_UnReadLen(puart->pRsvbuf);
        /*������ֽڱ��ζ�����������BufClear��ֹ�����ٶ�ȡ*/
        if (readlen >= count)
            break;
        if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
        {
            delay--;
        }
    }
    USART_ITConfig(puart->handler, USART_IT_RXNE, DISABLE);
    if (readlen)
    {
        /*ʵ�ʶ����ĳ���*/
        readlen = (readlen < count ? readlen : count);
        for (i = 0; i < readlen; i++)
        {
            *(buf + i) = puart->pRsvbuf->data[puart->pRsvbuf->rd];
            puart->pRsvbuf->rd = (++puart->pRsvbuf->rd) % DRV_BUF_SIZE;
        }
    }
    return readlen;
}

/***************************************************************
* ��������: Uart_Write
* ��������: д����
* �������: uart��ϵͳ����  buf:�洢λ��   count����������
* �������: ��
* �� �� ֵ: > 0��ʵ�ʷ��͵�����   -1������ʧ��
****************************************************************/

void Uart_Write(UartDef *puart, const uint8_t *pbuf, uint32_t count)
{
    uint32_t sendlen = 0;
    uint16_t i;
    //��֤count��Ϊ0�������ٿ���һ�η����ж�
    if (!count || count > uwBuf_EmpLen(puart->pSndbuf))
        return;
    for (i = 0; i < count; i++)
    {
        puart->pSndbuf->data[puart->pSndbuf->wr] = *(pbuf + i);
        puart->pSndbuf->wr = (++puart->pSndbuf->wr) % DRV_BUF_SIZE;
    }
    sendlen = puart->pSndbuf->rd;
    USART_ITConfig(puart->handler, USART_IT_TXE, ENABLE);
}

/**********************************************
    * ��������: Uart_Read
    * ��������: �����ڣ�ֱ�Ӵӻ����ȡ�������Զ�����
    * �������: puart�����ڽṹ��ָ��  buf:�洢λ��   count����ȡ����
    * �������: ��
    * �� �� ֵ: > 0��ʵ�ʶ�ȡ������   0�������ݻ�count==0����� 
    ****************************************************************/
uint32_t Uart_Read(UartDef *puart, uint8_t *buf, uint32_t count)
{
    volatile uint32_t readlen = 0;
    uint16_t i;
    if (!count || count > 0xFFFF)
        return 0;
    readlen = uwBuf_UnReadLen(puart->pRsvbuf);
    if (readlen)
    {
        /*ʵ�ʶ����ĳ���*/
        readlen = (readlen < count ? readlen : count);
        for (i = 0; i < readlen; i++)
        {
            *(buf + i) = puart->pRsvbuf->data[puart->pRsvbuf->rd];
            puart->pRsvbuf->rd = (++puart->pRsvbuf->rd) % DRV_BUF_SIZE;
        }
    }
    return readlen;
}
/**********************************************
* ��������: Uart_ReadITEnable
* ��������: ����Idle�жϷ�ʽ���������Ƿ�ʹ��
* �������: puart�����ڽṹ��ָ�� cmd:EN/DEN 
* �������: 
* �� �� ֵ: 
****************************************************************/
void Uart_idleReadEnable(UartDef *puart)
{
    uint32_t regClear;
    //USART_ITConfig(puart->handler, USART_IT_RXNE, ENABLE);
    regClear = puart->handler->ISR;
    USART_ClearITPendingBit(puart->handler, USART_IT_IDLE);
    USART_ITConfig(puart->handler, USART_IT_IDLE, ENABLE);
}
/**********************************************
* ��������: Uart_ReadITdisable
* ��������: ����Idle�жϷ�ʽ���������Ƿ�ʹ��
* �������: puart�����ڽṹ��ָ�� cmd:EN/DEN 
* �������: 
* �� �� ֵ: 
****************************************************************/
void Uart_idleReadDisable(UartDef *puart)
{
    uint32_t regClear;
    //USART_ITConfig(puart->handler, USART_IT_RXNE, DISABLE);
    USART_ITConfig(puart->handler, USART_IT_IDLE, DISABLE);
    USART_ClearITPendingBit(puart->handler, USART_IT_IDLE);
    regClear = puart->handler->ISR;
}

/*******************************************************
* ��    �ܣ���ѯ�����㻺��ʣ��ռ�
* ��    �룺���ݻ���ṹ���ַ
* ��    ����uint32_t
*           
* �� д �ˣ�
* ��д���ڣ�2018��9��7��11:04:55
**********************************************************************/
uint32_t uwBuf_EmpLen(const struct ucbuf *buf)
{
    /*return ((buf->rd + BUF_SIZE - buf->wr) % BUF_SIZE -1);*/
    return ((buf->rd + DRV_BUF_SIZE - buf->wr - 1) % DRV_BUF_SIZE);
}
/********************************************************************
* ��    �ܣ���ѯ������δ�����ֽ���
* ��    �룺���ݻ���ṹ���ַ
* ��    ����uint32_t
*          
* �� д �ˣ�
* ��д���ڣ�2016.6.28
**********************************************************************/
uint32_t uwBuf_UnReadLen(const struct ucbuf *buf)
{
    return (buf->wr - buf->rd + DRV_BUF_SIZE) % DRV_BUF_SIZE;
}
/********************************************************************
* ��    �ܣ���ջ���
* ��    �룺���ݻ���ṹ���ַ
* ��    ����
*          
* �� д �ˣ�
* ��д���ڣ�2016.6.28
**********************************************************************/
void vBuf_Clear(struct ucbuf *buf)
{
    buf->rd = buf->wr;
}

/********************************************************************
* ��    �ܣ����һ������ض��ַ�
* ��    �룺����ṹ��ָ��uc_buf���ض��ַ�chr,�������ֽ���ln
* ��    ����true/false
* ˵    ������ָ��ָ����ַ�           
* �� д �ˣ�stragen
* ��д���ڣ�2016.8.25
**********************************************************************/
bool blBufchr(struct ucbuf *uc_buf, uint8_t chr, uint16_t ln)
{
    while (ln-- && (uc_buf->data[uc_buf->rd] != chr)) // &&(uc_buf->RD_Index != uc_buf->WR_Index))
    {
        uc_buf->rd = (uc_buf->rd + 1) % DRV_BUF_SIZE;
    }
    //return(ln ? true : false);						//��ָ�������0
    if (ln)
        return false;
    else
        return true;
}

/********************************************************************
* ��    �ܣ��жϻ������ַ����Ƿ����ض��ַ���
* ��    �룺����ṹ���ַuc_buf���ַ���ָ��ptchr���ַ�������ln
* ��    ����true/false
* ˵    ������ָ�벻�ı�
* �� д �ˣ�stragen
* ��д���ڣ�2016.8.25
**********************************************************************/
bool blBufcmp(struct ucbuf *uc_buf, char *ptchr, uint8_t ln)
{
    while (ln-- && (uc_buf->data[uc_buf->rd] == *(ptchr++)) && (uc_buf->rd != uc_buf->wr)) //��ֹ��ȡԽ��
    {
        uc_buf->rd = (uc_buf->rd + 1) % DRV_BUF_SIZE;
    }
    return (ln ? false : true);
}

#endif
