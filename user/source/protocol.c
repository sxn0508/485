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

/********************************************************************
* 功    能：获取帧指针
* 输    入：应用层缓存指针，数据长度
* 输    出：帧指针
*           
* 编 写 人：
* 编写日期：2018-9-9 08:01:18
**********************************************************************/
uint8_t *pGetpFrame(uint8_t *pframe, uint32_t dwLen, DLT698_FRAME *p698Frame)
{
    uint8_t *p;
    UINT16 uwframeLen;
    UINT32 nHCSPos;
    UINT32 nFCSPos;
    UINT16 uwhcs;
    UINT16 uwfcs;
    UINT16 SA_Len;
    int dwAPDULen = 0;
    //if (len > DRV_BUF_SIZE)
    //return NULL;
    p = (uint8_t *)strchr((char *)pframe, 0x68);

    if (p == NULL)
        return NULL;

    //帧长占2byte，不含头尾
    uwframeLen = MAKE_WORD(*(p + 2), *(p + 1));

    if (dwLen + 2 > uwframeLen)
        return NULL;

    /*SA地址字节数，0代表1个字节*/
    SA_Len = (*(p + 4) & 0x0F) + 2;
    nHCSPos += SA_Len + 5;

    //帧头校验hcs
    uwhcs = PPPINITFCS16;
    uwhcs = pppfcs16(uwhcs, p + 1, nHCSPos - 1);

    if (uwhcs != MAKE_WORD(*(p + nHCSPos + 1), *(p + nHCSPos)))
    {
        return NULL;
    }

    //帧校验fcs
    nFCSPos = uwframeLen - 1;
    uwfcs = PPPINITFCS16;
    uwfcs = pppfcs16(uwfcs, p + 1, nFCSPos - 1);

    if (uwfcs != MAKE_WORD(*(p + nFCSPos + 1), *(p + nFCSPos)))
    {
        return NULL;
    }

    p698Frame->uwFramelen = uwframeLen;
    p698Frame->uwSA_len = SA_Len;
    p698Frame->pStart = p;
    p698Frame->pSecurityData = p + nHCSPos + 2;

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