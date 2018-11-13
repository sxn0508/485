/*********************************************************************** 
* Copyright (c) 2012, All rights reserved.
*
* 文件名称： UART.c
* 描    述： 串口通信代码文件
* 
* 修改记录：
*
* V1.0，2016.06.19，
***********************************************************************/
#include "protocol.h"
#include "DLT698.h"

#define TICKS_500MS 6
/********************************************************************
* 功    能：获取帧指针
* 输    入：应用层缓存指针，数据长度
* 输    出：帧指针
*           
* 编 写 人：
* 编写日期：2018-9-9 08:01:18
**********************************************************************/
void *pGetpFrame(uint8_t *framebuf, uint32_t len)
{
    uint8_t *p;
    if (len > DRV_BUF_SIZE)
        return NULL;
    p = (uint8_t *)strchr((char *)framebuf, 0x68);
    //if (*(framebuf + len - 1) != 0x16)
    //{
    //p = NULL;
    //}
    return p;
}

/********************************************************************
* 功    能：查询应用层缓存的帧格式
* 输    入：应用层缓存指针，数据长度
* 输    出：规约类型枚举值
*           
* 编 写 人：
* 编写日期：2018-9-9 08:01:18
**********************************************************************/
ProtocolDef GetProtocolType(uint8_t *pbuf, uint32_t len)
{
    ProtocolDef ptype = none;
    uint8_t *pframe;
    pframe = (uint8_t *)strchr((char *)pbuf, 0x68);
    uint32_t dlt645_L = *(pframe + 9) + 12;
    //uint32_t dlt698_L = *(pframe + 1) | (*(pframe + 2) & 0x3F) << 8 + 2;
    uint32_t dlt698_L = MAKE_WORD(*(pframe + 2) & 0x3F, *(pframe + 1)) + 2;

    /*8th byte==68*/
    /*last byte== 16*/
    /* CS correct*/
    /*97-645 do not support autobaud*/
    if (*(pframe + 7) == 0x68 &&
        len >= dlt645_L &&
        *(pframe + dlt645_L - 2) == dlt645_CalCS(pframe, dlt645_L - 2) &&
        *(pframe + dlt645_L - 1) == 0x16)
        ptype = dlt645;
    /*last byte== 16*/
    else if (*(pframe + dlt698_L - 1) == 0x16 &&
             len >= dlt698_L)
        ptype = dlt698;
    return ptype;
}
/********************************************************************
* 功    能：dlt645_CS计算`
* 输    入：帧结构指针
* 输    出：CS值
*           
* 编 写 人：
* 编写日期：2018-9-9 08:01:18
**********************************************************************/
uint8_t dlt645_CalCS(uint8_t *pframe, uint32_t cspos)
{
    uint8_t *pbuf;
    uint8_t CS = 0;
    uint8_t i;

    pbuf = (uint8_t *)pframe;
    //求取校验和
    for (i = 0; i < cspos; i++)
    {
        CS += *(pbuf + i);
    }
    return CS;
}
uint32_t dwUartCopy(UartDef *SrcUart, uint8_t *pdata, UartDef *DestUart)
{
    uint32_t dwLen;
    dwLen = Uart_Read(SrcUart, pdata, DRV_BUF_SIZE);
    if (dwLen == 0)
        return 0;
    /*向下透传*/
    //Uart_Write(DestUart, pdata, dwLen);
    Uart_OnceWrite(DestUart, pdata, dwLen, TICKS_500MS);
    return dwLen;
}