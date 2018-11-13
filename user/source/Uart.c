/*********************************************************************** 
* Copyright (c) 2012,积成电子股份有限公司 All rights reserved.
*
* 文件名称： UART.c
* 描    述： 串口通信代码文件
* 
* 修改记录：
*
* V1.0，2016.06.19，
***********************************************************************/

#include "Uart.h"
#include "stm32f37x_it.h"

static void vIRQ_UART_Config(void); //串口中断初始化
/********************************************************************
* 功    能：串口初始化函数
* 输    入：None
* 输    出：None
*           
* 编 写 人：
* 编写日期：2016.6.23
**********************************************************************/
void vUart_Init(void)
{
    USART_InitTypeDef USART_Initstruc;
    /*USART1配置*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    USART_OverSampling8Cmd(USART1, ENABLE);
    USART_Initstruc.USART_BaudRate = 2400;
    USART_Initstruc.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
    USART_Initstruc.USART_WordLength = USART_WordLength_9b;
    USART_Initstruc.USART_Parity = USART_Parity_Even;
    USART_Initstruc.USART_StopBits = USART_StopBits_1;
    USART_Initstruc.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_Initstruc);
    /*USART3配置*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    USART_OverSampling8Cmd(USART3, ENABLE);
    USART_Initstruc.USART_BaudRate = 2400;
    USART_Initstruc.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
    USART_Initstruc.USART_WordLength = USART_WordLength_9b;
    USART_Initstruc.USART_Parity = USART_Parity_Even;
    USART_Initstruc.USART_StopBits = USART_StopBits_1;
    USART_Initstruc.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_Initstruc);
    /*USART2配置为标准串口*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    USART_OverSampling8Cmd(USART2, ENABLE);
    USART_Initstruc.USART_BaudRate = 1200;
    USART_Initstruc.USART_WordLength = USART_WordLength_9b;
    USART_Initstruc.USART_StopBits = USART_StopBits_1;
    USART_Initstruc.USART_Parity = USART_Parity_Even;
    USART_Initstruc.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Initstruc.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_Initstruc);
    vIRQ_UART_Config(); //中断配置,使能
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
    USART_Cmd(USART3, ENABLE);
    USART_Cmd(USART2, ENABLE);
}
/********************************************************************
  * @brief 串口中断初始化函数
  * @param  None
  * @retval None
  *******************************************************************/
void vIRQ_UART_Config(void)
{
    /*定义中断结构体*/
    NVIC_InitTypeDef NVIC_Initstruc;
    /*配置串口1中断*/
    NVIC_Initstruc.NVIC_IRQChannel = USART1_IRQn;
    NVIC_Initstruc.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Initstruc.NVIC_IRQChannelSubPriority = 1;
    NVIC_Initstruc.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Initstruc);
    /*配置串口3中断*/
    NVIC_Initstruc.NVIC_IRQChannel = USART3_IRQn;
    NVIC_Initstruc.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_Initstruc);
    /*配置串口2中断*/
    NVIC_Initstruc.NVIC_IRQChannel = USART2_IRQn;
    NVIC_Initstruc.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_Initstruc);
}
/********************************************************************
* 功    能：判断驱动层缓存是否为空
* 输    入：驱动层缓存结构体地址
* 输    出：true/false
*           
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
bool blDrv_Buf_IsEmpty(const struct ucbuf *ucdrv_buf)
{
    if (ucdrv_buf->rd == ucdrv_buf->wr)
        return true;
    else
        return false;
}
/********************************************************************
* 功    能：判断驱动层缓存是否满
* 输    入：驱动层缓存结构体地址
* 输    出：true/false
*           
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
bool blDrv_Buf_IsFull(const struct ucbuf *ucdrv_buf)
{
    if (ucdrv_buf->rd == (ucdrv_buf->wr + 1) % DRV_BUF_SIZE)
        return true;
    else
        return false;
}
/********************************************************************
* 功    能：从驱动缓存读取指定数量的字节
* 输    入：驱动缓存结构体地址，数据缓存结构体地址，读取字节数
* 输    出：true/false
*          
* 编 写 人：王君龙
* 编写日期：2016.6.28
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
* 功    能：向驱动缓存写指定数量的字节
* 输    入：数据缓存结构体地址，驱动缓存结构体地址，写字节数
* 输    出：true/false
*          
* 编 写 人：王君龙
* 编写日期：2017.3.14
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
* 功    能：串口发送（使用中断功能）
* 输    入：USART_X
* 输    出：void
*           
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
void Uart_DataPut(USART_TypeDef *USART_X)
{
    USART_ITConfig(USART_X, USART_IT_TXE, ENABLE);
}

#if 1
/***************************************************************
* 函数名称: Uart_Write
* 功能描述: 写串口，中断发送，最大发送时长delay
* 输入参数: uart：系统串口  buf:存储位置   count：发送数量
* 输出参数: 无
* 返 回 值: > 0：实际发送的数量   -1：发送失败
****************************************************************/
int32_t Uart_OnceWrite(UartDef *puart, const uint8_t *pbuf, uint32_t count, uint32_t ticks)
{
    uint32_t sendlen = 0;
    uint16_t i;
    //保证count不为0，可至少开启一次发送中断
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
        /*发送完成*/
        if (!uwBuf_UnReadLen(puart->pSndbuf))
            break;
        if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
        {
            ticks--;
        }
    }
    /*规定时间内未发送完成，也要关闭中断*/
    //LL_USART_DisableIT_TXE(puart->handler);
    USART_ITConfig(puart->handler, USART_IT_TXE, DISABLE);
    return (puart->pSndbuf->rd - sendlen + DRV_BUF_SIZE) % DRV_BUF_SIZE;
}

/**********************************************
    * 函数名称: Uart_IdleRead
    * 功能描述: 读串口,字节间隔大于idleMs，或者读取数量达到count时结束接收
    * 输入参数: puart：串口结构体指针  buf:存储位置   count：读取数量  idle：字节超时时间ms，可以为0
    * 输出参数: 无
    * 返 回 值: > 0：实际读取的数量   0：无数据或count==0或错误 
    ****************************************************************/
uint32_t Uart_IdleRead(UartDef *puart, uint8_t *buf, uint32_t count, uint32_t idleMs)
{
    volatile uint32_t readlen = 0;
    volatile uint32_t readlenow = 0;
    volatile idleMsCount;
    uint16_t i;
    /*读取数量> 0，<缓存长度*/
    if (!count || count > DRV_BUF_SIZE)
    {
        return 0;
    }
    /*保证最小延时*/
    if (idleMs < 0xFFFFFFFF)
    {
        idleMsCount = ++idleMs;
    }
    USART_ITConfig(puart->handler, USART_IT_RXNE, ENABLE);
    readlenow = uwBuf_UnReadLen(puart->pRsvbuf);
    while (readlenow < count && idleMsCount > 0)
    {
        /*idleMsCount计时累减*/
        if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
        {
            idleMsCount--;
        }
        readlenow = uwBuf_UnReadLen(puart->pRsvbuf);
        /*readlennow 有变化，idleMsCount重载*/
        if (readlenow > readlen)
        {
            idleMsCount = idleMs;
            readlen = readlenow;
        }
    }
    USART_ITConfig(puart->handler, USART_IT_RXNE, DISABLE);
    if (readlen)
    {
        /*实际读到的长度*/
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
    * 函数名称: Uart_OnceRead
    * 功能描述: 读串口,读到指定数量或读取时间超时后退出
    * 输入参数: puart：串口结构体指针  buf:存储位置   count：读取数量  delay：字节超时时间ms，可以为0
    * 输出参数: 无
    * 返 回 值: > 0：实际读取的数量   0：无数据或count==0或错误 
    ****************************************************************/
uint32_t Uart_OnceRead(UartDef *puart, uint8_t *buf, uint32_t count, uint32_t delay)
{
    volatile uint32_t readlen = 0;
    uint16_t i;
    /*读取数量> 0，<缓存长度*/
    if (!count || count > DRV_BUF_SIZE)
    {
        return 0;
    }
    USART_ITConfig(puart->handler, USART_IT_RXNE, ENABLE);
    readlen = uwBuf_UnReadLen(puart->pRsvbuf);
    while (delay)
    {
        readlen = uwBuf_UnReadLen(puart->pRsvbuf);
        /*多余的字节本次丢弃，并调用BufClear防止后面再读取*/
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
        /*实际读到的长度*/
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
* 函数名称: Uart_Write
* 功能描述: 写串口
* 输入参数: uart：系统串口  buf:存储位置   count：发送数量
* 输出参数: 无
* 返 回 值: > 0：实际发送的数量   -1：发送失败
****************************************************************/

void Uart_Write(UartDef *puart, const uint8_t *pbuf, uint32_t count)
{
    uint32_t sendlen = 0;
    uint16_t i;
    //保证count不为0，可至少开启一次发送中断
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
    * 函数名称: Uart_Read
    * 功能描述: 读串口，直接从缓存读取，缓存自动接收
    * 输入参数: puart：串口结构体指针  buf:存储位置   count：读取数量
    * 输出参数: 无
    * 返 回 值: > 0：实际读取的数量   0：无数据或count==0或错误 
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
        /*实际读到的长度*/
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
* 函数名称: Uart_ReadITEnable
* 功能描述: 串口Idle中断方式连续接收是否使能
* 输入参数: puart：串口结构体指针 cmd:EN/DEN 
* 输出参数: 
* 返 回 值: 
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
* 函数名称: Uart_ReadITdisable
* 功能描述: 串口Idle中断方式连续接收是否使能
* 输入参数: puart：串口结构体指针 cmd:EN/DEN 
* 输出参数: 
* 返 回 值: 
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
* 功    能：查询驱动层缓存剩余空间
* 输    入：数据缓存结构体地址
* 输    出：uint32_t
*           
* 编 写 人：
* 编写日期：2018年9月7日11:04:55
**********************************************************************/
uint32_t uwBuf_EmpLen(const struct ucbuf *buf)
{
    /*return ((buf->rd + BUF_SIZE - buf->wr) % BUF_SIZE -1);*/
    return ((buf->rd + DRV_BUF_SIZE - buf->wr - 1) % DRV_BUF_SIZE);
}
/********************************************************************
* 功    能：查询缓存中未读的字节数
* 输    入：数据缓存结构体地址
* 输    出：uint32_t
*          
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
uint32_t uwBuf_UnReadLen(const struct ucbuf *buf)
{
    return (buf->wr - buf->rd + DRV_BUF_SIZE) % DRV_BUF_SIZE;
}
/********************************************************************
* 功    能：清空缓存
* 输    入：数据缓存结构体地址
* 输    出：
*          
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
void vBuf_Clear(struct ucbuf *buf)
{
    buf->rd = buf->wr;
}

/********************************************************************
* 功    能：查找缓存中特定字符
* 输    入：缓存结构体指针uc_buf，特定字符chr,缓存内字节数ln
* 输    出：true/false
* 说    明：读指针指向该字符           
* 编 写 人：stragen
* 编写日期：2016.8.25
**********************************************************************/
bool blBufchr(struct ucbuf *uc_buf, uint8_t chr, uint16_t ln)
{
    while (ln-- && (uc_buf->data[uc_buf->rd] != chr)) // &&(uc_buf->RD_Index != uc_buf->WR_Index))
    {
        uc_buf->rd = (uc_buf->rd + 1) % DRV_BUF_SIZE;
    }
    //return(ln ? true : false);						//读指针可能是0
    if (ln)
        return false;
    else
        return true;
}

/********************************************************************
* 功    能：判断缓存中字符串是否是特定字符串
* 输    入：缓存结构体地址uc_buf，字符串指针ptchr，字符串长度ln
* 输    出：true/false
* 说    明：读指针不改变
* 编 写 人：stragen
* 编写日期：2016.8.25
**********************************************************************/
bool blBufcmp(struct ucbuf *uc_buf, char *ptchr, uint8_t ln)
{
    while (ln-- && (uc_buf->data[uc_buf->rd] == *(ptchr++)) && (uc_buf->rd != uc_buf->wr)) //防止读取越界
    {
        uc_buf->rd = (uc_buf->rd + 1) % DRV_BUF_SIZE;
    }
    return (ln ? false : true);
}

#endif
