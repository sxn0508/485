#include "DLT698.h"
//#include "DLT698_45.h"
#include "BaseDef.h"

static int dwDlt698DataLen(UINT8 *ucRcvBuf, int dwRcvLen);
inline int dwDlt698Len(UINT8 *ucRcvBuf, int *LenBytes);
static int dwGetResponseNormal(UINT8 *pucBuf, COLL_STORE_DATA *pstCollStoreData);
static int dwGetResponseRecord(UINT8 *pucBuf, COLL_STORE_DATA *stCollData);
static int dwDlt698RSDLen(UINT8 *ucRcvBuf, int dwRcvLen);
static int dwDlt698MsLen(UINT8 *ucRcvBuf, int dwRcvLen);

/*
* FCS lookup table as calculated by the table generator.
*/
const UINT16 fcstab[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78};

/*
* Calculate a new fcs given the current fcs and the new data.
*/
//extern UINT16 pppfcs16(fcs, cp, len)
extern UINT16 pppfcs16(register UINT16 fcs, register unsigned char *cp, register int len)
{
    //register UINT16 fcs;
    //register unsigned char *cp;
    //register int len;
    assert(sizeof(UINT16) == 2);
    assert(((UINT16)-1) > 0);
    while (len--)
        fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
    return (~fcs);
}

int dwReCalculateFCS(DLT698_FRAME *pframe)
{
    UINT16 uwframeLen;
    UINT32 nFCSPos;
    UINT16 uwfcs;

    uwframeLen = pframe->uwFramelen;

    nFCSPos = uwframeLen - 1;
    uwfcs = PPPINITFCS16;
    uwfcs = pppfcs16(uwfcs, pframe->pStart + 1, nFCSPos - 1);

    *(pframe->pStart + nFCSPos) = LSB(uwfcs);
    *(pframe->pStart + nFCSPos + 1) = MSB(uwfcs);

    return 1;
}

/**
  ************************************************************************
  * @brief    : unpack scurity APDU source code
  * @param    :
  * @return   :
  * @author   :
  * @version  : 0.1
  * @date     : 2017-8-29 23:11:13
  * @modify   :
  * @see also :
  ************************************************************************
  * @attention
  ************************************************************************
  */
//int dwUnpackSecurity(UINT8 *ucSecurityBuf, int dwSourceLen, UINT8 **clearAPDU, COLLECT_CHANNEL eChan, TSA *pTsa)
int dwUnpackSecurity(UINT8 *ucSecurityBuf, int dwSourceLen, UINT8 **clearAPDU)
{
    int Len = 0, Bytes = 0;

    assert(ucSecurityBuf);

    *clearAPDU = ucSecurityBuf;

    if (ucSecurityBuf[0] != SECURITY_Response)
    {
        return dwSourceLen;
    }
    else
    {
        switch (ucSecurityBuf[1])
        {
        /*应用数据单元安全类型：0明文，1密文*/
        case 0:
        {
            if ((Len = dwDlt698Len(&ucSecurityBuf[2], &Bytes)) < 0)
            {
                return -1;
            }
            //else if (eChan < COLLECT_PLC) //只有485通道进行Mac验证
            //{
            //进行mac验证
            //if ((ucSecurityBuf[2 + Bytes + Len] == 1) &&
            //(ucSecurityBuf[2 + Bytes + Len + 1] == 0) &&
            //(ucSecurityBuf[2 + Bytes + Len + 2] == 4)) //mac长度4
            //{
            //if (ERROR == dwValidateMAC(&ucSecurityBuf[2 + Bytes + Len + 1 + 1 + 1], &ucSecurityBuf[2 + Bytes], Len, pTsa, eChan))
            //{
            //校验错误返回-2
            //return -2;
            //}
            //else
            //{
            *clearAPDU = ucSecurityBuf + 2 + Bytes;
            return Len;
            //}
            //}
            //else
            //{
            //return -2;
            //}
            //}
//非485通道或不需验证
#if 0
            else
            {
                *clearAPDU = ucSecurityBuf + 2 + Bytes;
                return Len;
            }
#endif
        }
        break;
        default:
            break;
        }
    }

    return dwSourceLen;
}

int dwGet698Apdu(DLT698_FRAME *pframe, UINT8 **pAPDU)
{
    UINT16 dwLen; //clearApdu len
#if 1
    dwLen = pframe->uwFramelen - pframe->uwSA_len - 8;
    if ((dwLen = dwUnpackSecurity(pframe->pSecurityData, dwLen, pAPDU)) < 0)
    {
        return -1;
    }
#endif
    //*pAPDU = pframe->pStart + pframe->uwSA_len + 7;

    //return (pframe->uwFramelen - pframe->uwSA_len - 8);
    return dwLen;
}
/*
*********************************************************************
* 功    能：帧校验
* 输    入：
* 输    出：
* 作    者：ddy
* 编写日期：2017-05-24
* 修改日期: 
**********************************************************************
*/
int dwGetApdu(UINT8 *pframe, UINT8 **pAPDU)
{
    UINT16 uwframeLen;
    UINT32 nHCSPos = 0;
    UINT32 nFCSPos;
    UINT16 uwhcs;
    UINT16 uwfcs;
    UINT16 SA_Len;
    int dwAPDULen = 0;
    //帧头首字符为: 0x68
    if (pframe[0] != 0x68)
    {
        return -1;
    }

    //帧长占2byte，不含头尾
    uwframeLen = MAKE_WORD(pframe[2], pframe[1]);
    /*SA地址字节数，0代表1个字节*/
    SA_Len = (pframe[4] & 0x0F) + 2;
    nHCSPos += SA_Len + 5;

    //帧头校验hcs
    uwhcs = PPPINITFCS16;
    uwhcs = pppfcs16(uwhcs, pframe + 1, nHCSPos - 1);

    if (uwhcs != MAKE_WORD(pframe[nHCSPos + 1], pframe[nHCSPos]))
    {
        return -1;
    }

    //帧校验fcs
    nFCSPos = uwframeLen - 1;
    uwfcs = PPPINITFCS16;
    uwfcs = pppfcs16(uwfcs, pframe + 1, nFCSPos - 1);
    if (uwfcs != MAKE_WORD(pframe[nFCSPos + 1], pframe[nFCSPos]))
    {
        return -1;
    }

    dwAPDULen = uwframeLen - SA_Len - 8;

    //HardFault
    //memcpy(pAPDU, &pframe[nHCSPos + 2], dwAPDULen);
    *pAPDU = &pframe[nHCSPos + 2];
    return dwAPDULen;
}

int dwAPduAnalyze(UINT8 *pucBuf, UINT32 dwlen, COLL_STORE_DATA *pstCollStoreData)
{
    //int Len = 0, Bytes = 0;
    int Ret = -1;
    COLL_STORE_DATA *pstCollData = pstCollStoreData;

    if (GET_Response == pucBuf[0])
    {
        switch (pucBuf[1])
        {
        case GetResponseNormal:
        case GetResponseNormalList:
        {
            Ret = dwGetResponseNormal(pucBuf, pstCollData);
        }
        break;
        case GetResponseRecord:
        case GetResponseRecordList:
        {
            Ret = dwGetResponseRecord(pucBuf, pstCollData);
        }
        break;
        case GetResponseNext:
        {
            //	Ret = blGetResponseNext(pstChanTaskMsg, &pApduBuf[nPos], ucGetResponse);
        }
        break;
        default:
            break;
        }
    }
    else if (SECURITY_Response == pucBuf[0])
    {
        return -1;
    }

    return Ret;
}

/**
 * @note: dlt698 data length area
 * @author: 
 * @date: 2017-7-21 15:05:44
 *-----------------------------------------------------------------
 */
inline int dwDlt698Len(UINT8 *ucRcvBuf, int *LenBytes)
{
    int Bytes;

    assert(ucRcvBuf);

    if (0x00 == (ucRcvBuf[0] & 0x80))
    {
        *LenBytes = 1;
        return ucRcvBuf[0];
    }

    Bytes = ucRcvBuf[0] & 0x7F;

    if (Bytes == 1)
    {
        *LenBytes = 2;
        return (ucRcvBuf[1]);
    }
    else if (Bytes == 2)
    {
        *LenBytes = 3;
        return ((ucRcvBuf[1] << 8) | (ucRcvBuf[2]));
    }
    else if (Bytes == 3)
    {
        *LenBytes = 4;
        return ((ucRcvBuf[1] << 16) |
                (ucRcvBuf[2] << 8) & (ucRcvBuf[3]));
    }
    else if (Bytes == 4)
    {
        *LenBytes = 5;
        return ((ucRcvBuf[1] << 24) |
                (ucRcvBuf[2] << 16) & (ucRcvBuf[3] << 8) & (ucRcvBuf[4]));
    }

    return -1;
}

int dwDlt698DataLenByOAD(OAD stOAD, UINT8 *pucBuf)
{
    //UINT8 i;
    //int dwGetResult;
    //INT32 dwRetLen = 0;
    //UINT8 ucDataType = 0;
    //int nRetDataLen = 0;
    //UINT8 ucArrayCount = 0;
    UINT8 byOI1 = stOAD.OI_date >> 8;
    //UINT8 byOI2 = stOAD.OI_date;
    //UINT8 ucFeature = stOAD.attr_ID;
    //UINT8 ucIndex = stOAD.attr_index;

    if ((byOI1 >> 4) == 0)
        byOI1 = byOI1 & 0xF0;
    return dwDlt698DataLen(pucBuf, 256);
}

int dwGetOad(UINT8 *pucStr, OAD *pstOad)
{
    pstOad->OI_date = MAKE_WORD(*pucStr, *(pucStr + 1));
    pucStr += 2;

    pstOad->attr_ID = *pucStr++;
    pstOad->attr_index = *pucStr++;

    return 4;
}

DATA_CLASS eDataClass(OAD stMainOad)
{
    if (stMainOad.OI_date == OI_INSTANTFRZ ||
        stMainOad.OI_date == OI_SECFRZ ||
        stMainOad.OI_date == OI_MINUTEFRZ ||
        stMainOad.OI_date == OI_HOURFRZ)
    {
        return D_CLASS_CUR_FREEZE;
    }
    else if (stMainOad.OI_date == OI_DAY_FRZ)
    {
        return D_CLASS_DAY_FREEZE;
    }
    else if (stMainOad.OI_date == OI_MONTHFRZ)
    {
        return D_CLASS_MON_FREEZE;
    }
    else if (stMainOad.OI_date >= 0x3000 &&
             stMainOad.OI_date < 0x3300)
    {
        return D_CLASS_EVENT;
    }

    return D_CLASS_NULL;
}

int dwGetResponseNormal(UINT8 *pucBuf, COLL_STORE_DATA *pstCollStoreData) //, COLLECT_UNIT_ID stID)
{
    UINT8 i;
    UINT8 ucResultNum;
    int dwLen;
    //BOOL Ret = true;
    COLL_STORE_DATA *pstCollData = pstCollStoreData;
    UINT8 *Ptr = pucBuf;
    OAD stOADtmp;
    //extern BOOL blFollowReportSWord;
    //extern UINT8 ucFollowReportSWord[4];
    //UINT8 ucNum;

    Ptr++; // Get Request

    switch (*Ptr++)
    {
    case GetResponseNormal:
    {
        Ptr++;
        ucResultNum = 1;
    }
    break;
    case GetResponseNormalList:
    {
        //SEQUENCE OF ResultNormal
        Ptr++;
        if ((ucResultNum = *Ptr++) > 127)
        {
            return -1;
        }
    }
    break;
    default:
        return -1;
    }

    bzero(pstCollData, sizeof(COLL_STORE_DATA));

    //stCollData.uwConfigNo = uwConfigNo;
    pstCollData->eDataCls = D_CLASS_REAL;

    for (i = 0; i < ucResultNum; i++)
    {
        //Ptr += dwGetOad(Ptr, &pstCollData->stDataUnit[i].stOAD);
        Ptr += dwGetOad(Ptr, &stOADtmp);

        if (ERROR_INFO == *Ptr)
        {
            Ptr++; // Choice
            Ptr++; // DAR
        }
        else if (RES_DATA == *Ptr)
        {
            Ptr++; // Choice
            //if ((dwLen = dwDlt698DataLenByOAD(pstCollData->stDataUnit[i].stOAD, Ptr)) < 0)
            if ((dwLen = dwDlt698DataLenByOAD(stOADtmp, Ptr)) < 0)
            {
                return -1;
            }
#if 1
            if (stOADtmp.OI_date == OI_VOLTAGE)
            {
                pstCollData->stDataUnit[pstCollData->ucDataNum].stOAD.OI_date = OI_VOLTAGE;
                pstCollData->stDataUnit[pstCollData->ucDataNum].stOAD.attr_ID = stOADtmp.attr_ID;
                pstCollData->stDataUnit[pstCollData->ucDataNum].stOAD.attr_index = stOADtmp.attr_index;

                pstCollData->stDataUnit[pstCollData->ucDataNum].uwLen = dwLen;
                pstCollData->stDataUnit[pstCollData->ucDataNum].ucPtr = Ptr;
                pstCollData->stDataUnit[pstCollData->ucDataNum].ucVal = 1;
                pstCollData->ucDataNum++;
            }
#endif
            //pstCollData->stDataUnit[i].uwLen = dwLen;
            //pstCollData->stDataUnit[i].ucPtr = Ptr;
            //pstCollData->stDataUnit[i].ucVal = 1;
            //pstCollData->ucDataNum++;

            Ptr += dwLen;
        }
        else
        {
            return -1;
        }
    }

/*********跟随上报标志********/
#if 0 //stragen
    if (1 == *Ptr++) //跟随上报标志
    {
        //若干个对象属性及其数据  [1]  SEQUENCE OF A-ResultNormal
        if (1 == *Ptr++)
        {
            ucNum = *Ptr++; // 个数SEQUENCE OF A-ResultNormal

            for (i = 0; i < ucNum; i++)
            {
                Ptr += dwGetOad(Ptr, &stOAD);
                if ((0x3320 == stOAD.OI_date) && (0x02 == stOAD.attr_ID) && (0x01 == stOAD.attr_index))
                {
                    if (1 == *Ptr++) //结果类型
                    {
                        if (DL_OAD == *Ptr++)
                        {
                            stFollowRptInfo[uwConfigNo - 1].ucFollowRptFlag = 1;
                            stFollowRptInfo[uwConfigNo - 1].stFollowRptEventOAD.OI_date = MAKE_WORD(*Ptr, *(Ptr + 1));
                            Ptr += 2;
                            stFollowRptInfo[uwConfigNo - 1].stFollowRptEventOAD.attr_ID = *Ptr++;
                            stFollowRptInfo[uwConfigNo - 1].stFollowRptEventOAD.attr_index = *Ptr++;
                        }
                    }
                }
                else if (0x2015 == stOAD.OI_date)
                {
                    if (1 == *Ptr++)
                    {
                        if (*Ptr++ != DL_BIT_STR)
                        {
                            continue;
                        }
                        if (*Ptr++ != 32)
                        {
                            continue;
                        }
                        blFollowReportSWord = TRUE;
                        memcpy(ucFollowReportSWord, Ptr, 0x04);
                        Ptr += 4;
                    }
                }
            }
        }
    }
#endif

    //blMeterDataChange(stID.ucTaskID, &stCollData);

    //if (OK != dwDealDataStore(&stCollData, stID))
    //{
    //return -1;
    //}
    /*数据解析函数，待移植*/
    //dwDataRowInsertItem(rowArray[i], pstCollData->stDataUnit[k].stOAD, pstCollData->stDataUnit[k].ucPtr, pstCollData->stDataUnit[k].uwLen, MAX_DATA_ROW_LEN);
    return 1;
}
/*
*********************************************************************
*GetResponseRecord 
**********************************************************************
*/
//int dwGetResponseRecord(UINT16 uwConfigNo, UINT8 *pucBuf, COLLECT_UNIT_ID stID)
int dwGetResponseRecord(UINT8 *pucBuf, COLL_STORE_DATA *stCollData)
{
    UINT8 i, j, k;
    UINT8 ucNum;
    UINT8 ucCSDNum;
    UINT8 ucResultNum;
    int dwLen;
    //struct tm stTime;
    UINT8 *Ptr = pucBuf;

    //struct tm tmNow;
    //time_t timeNow;
    //OAD stOAD;

    //extern BOOL blFollowReportSWord;
    //extern UINT8 ucFollowReportSWord[4];

    Ptr++;
    switch (*Ptr++)
    {
    case GetResponseRecord:
    {
        Ptr++;
        ucResultNum = 1;
    }
    break;
    case GetResponseRecordList:
    {
        //SEQUENCE OF A-ResultRecord
        Ptr++;
        if ((ucResultNum = *pucBuf++) > 127)
        {
            return -1;
        }
    }
    break;
    default:
        return -1;
    }

    bzero(&stCollData, sizeof(COLL_STORE_DATA));
    //stCollData.uwConfigNo = uwConfigNo;

    for (i = 0; i < ucResultNum; i++)
    {
        Ptr += dwGetOad(Ptr, &stCollData->stMainOAD);

        stCollData->eDataCls = eDataClass(stCollData->stMainOAD);
        if (stCollData->eDataCls == D_CLASS_NULL)
        {
            return -1;
        }

        //SEQUENCE OF CSD
        if ((ucCSDNum = *Ptr++) > 127)
        {
            return -1;
        }

        for (j = 0; j < ucCSDNum; j++)
        {
            //OAD
            if (*Ptr == 0)
            {
                Ptr++;
                Ptr += dwGetOad(Ptr, &stCollData->stDataUnit[j].stOAD);
            }
            //ROAD 暂不考虑
            else
            {
                return -1;
            }
        }

        //记录数据或者错误信息
        if (ERROR_INFO == *Ptr) //错误信息
        {
            Ptr++; // Choice
            Ptr++; // DAR
        }
        else if (RES_DATA == *Ptr) //记录数据
        {
            Ptr++; // Choice
            if ((ucNum = *Ptr++) > 127)
            {
                return -1;
            }

            if (ucNum == 0)
            {
                return 0;
            }

            for (j = 0; j < ucNum; j++)
            {
                stCollData->ucDataNum = 0;
                for (k = 0; k < ucCSDNum; k++)
                {
                    if ((dwLen = dwDlt698DataLenByOAD(stCollData->stDataUnit[k].stOAD, Ptr)) < 0)
                    {
                        return -1;
                    }

                    stCollData->stDataUnit[k].uwLen = dwLen;
                    stCollData->stDataUnit[k].ucPtr = Ptr;
                    stCollData->stDataUnit[k].ucVal = 1;
                    stCollData->ucDataNum++;

                    Ptr += dwLen;

#if 0
                    switch (stCollData.stDataUnit[k].stOAD.OI_date)
                    {
                    case OI_DATA_FRZ_TIME:
                    {
                        if (FALSE == FormatDataToBinaryS(stCollData.stDataUnit[k].ucPtr + 1, stCollData.stDataUnit[k].ucPtr[0], &stTime, &stCollData.stDataUnit[k].uwLen))
                        {
                            return ERROR;
                        }

                        switch (stCollData.eDataCls)
                        {
                        case D_CLASS_CUR_FREEZE:
                        {
                            stTime.tm_sec = 0x00;
                        }
                        break;
                        case D_CLASS_DAY_FREEZE:
                        {
#ifdef SHANDONG

                            timeNow = time(NULL);
                            localtime_r(&timeNow, &tmNow);

                            if (tmNow.tm_mday != stTime.tm_mday || tmNow.tm_mon != stTime.tm_mon || tmNow.tm_year != stTime.tm_year)
                            {
                                return 0;
                            }
#endif

                            stTime.tm_hour = 0x00;
                            stTime.tm_min = 0x00;
                            stTime.tm_sec = 0x00;
                        }
                        break;
                        case D_CLASS_MON_FREEZE:
                        {
#ifdef SHANDONG

                            timeNow = time(NULL);
                            localtime_r(&timeNow, &tmNow);

                            if (tmNow.tm_mon != stTime.tm_mon || tmNow.tm_year != stTime.tm_year)
                            {
                                return 0;
                            }
#endif

                            stTime.tm_mday = 0x01;
                            stTime.tm_hour = 0x00;
                            stTime.tm_min = 0x00;
                            stTime.tm_sec = 0x00;
                        }
                        break;
                        default:
                            return ERROR;
                        }

                        stCollData.stDataUnit[k].ucVal = 0;
                        stCollData.tFreezeTag = mktime(&stTime);
                    }
                    break;
                    case OI_EVENT_RCRD_NUM:
                    {
                        if (DL_D_LONG_UNS != stCollData.stDataUnit[k].ucPtr[0])
                        {
                            return -1;
                        }

                        stCollData.stDataUnit[k].ucVal = 0;
                        stCollData.udwRecordNo = LONGSWAP(*(UINT32 *)(stCollData.stDataUnit[k].ucPtr + 1));
                    }
                    break;
                    case OI_OCCURRENCE_TIME:
                    {
                        if (FALSE == FormatDataToBinaryS(stCollData.stDataUnit[k].ucPtr + 1, stCollData.stDataUnit[k].ucPtr[0], &stTime, &stCollData.stDataUnit[k].uwLen))
                        {
                            return -1;
                        }

                        stCollData.stDataUnit[k].ucVal = 0;
                        stCollData.tEventHappen = mktime(&stTime);
                    }
                    break;
                    case OI_END_TIME:
                    {
                        if (FALSE == FormatDataToBinaryS(stCollData.stDataUnit[k].ucPtr + 1, stCollData.stDataUnit[k].ucPtr[0], &stTime, &stCollData.stDataUnit[k].uwLen))
                        {
                            stCollData.stDataUnit[k].ucVal = 0;
                            stCollData.tEventFinish = 0xFFFFFFFF;
                            //return -1;
                        }
                        else
                        {
                            stCollData.stDataUnit[k].ucVal = 0;
                            stCollData.tEventFinish = mktime(&stTime);
                        }
                    }
                    break;
                    default:
                        break;
                    }
#endif
                }

#if 0
                blMeterDataChange(stID.ucTaskID, &stCollData);

                if (0 == stID.ucLastNTimes) //补抄任务
                {
                    if (OK != dwDealDataStore(&stCollData, stID))
                    {
                        return -1;
                    }
                }
                else //补抄任务，抄一帧存一帧
                {
                    if (OK != dwCollRetryDataStore(&stCollData, stID))
                    {
                        return -1;
                    }
                }
#endif
            }
#if 0
            /*********跟随上报标志********/
            if (1 == *Ptr++) //跟随上报标志
            {
                if (1 == *Ptr++) //若干个对象属性及其数据 [1]  SEQUENCE OF A-ResultNormal
                {
                    ucNum = *Ptr++; // 个数 SEQUENCE OF A-ResultNormal

                    for (i = 0; i < ucNum; i++)
                    {
                        Ptr += dwGetOad(Ptr, &stOAD);
                        if ((0x3320 == stOAD.OI_date) && (0x02 == stOAD.attr_ID) && (0x01 == stOAD.attr_index))
                        {
                            if (1 == *Ptr++) //结果类型
                            {
                                if (DL_OAD == *Ptr++)
                                {
                                    stFollowRptInfo[uwConfigNo - 1].ucFollowRptFlag = 1;
                                    stFollowRptInfo[uwConfigNo - 1].stFollowRptEventOAD.OI_date = MAKE_WORD(*Ptr, *(Ptr + 1));
                                    Ptr += 2;
                                    stFollowRptInfo[uwConfigNo - 1].stFollowRptEventOAD.attr_ID = *Ptr++;
                                    stFollowRptInfo[uwConfigNo - 1].stFollowRptEventOAD.attr_index = *Ptr++;
                                }
                            }
                        }
                        else if (0x2015 == stOAD.OI_date)
                        {
                            if (1 == *Ptr++) //结果类型
                            {
                                if (*Ptr++ != DL_BIT_STR)
                                {
                                    continue;
                                }
                                if (*Ptr++ != 32)
                                {
                                    continue;
                                }
                                blFollowReportSWord = TRUE;
                                memcpy(ucFollowReportSWord, Ptr, 0x04);
                                Ptr += 4;
                            }
                        }
                    }
                }
            }
#endif
        }
        else
        {
            return -1;
        }
    }

    return 1;
}

/**
  ************************************************************************
  * @brief    : count data length of the frist data in ucRcvBuf
  * @param    : ucRcvBuf : source code (with data identifier)
  *             dwRcvLen : valid length of source code
  * @return   : -1   : error    
                else : data length of the frist data in the source code
  * @author   : Lixg
  * @version  : 0.1
  * @date     : 2017-7-21 13:37:37
  * @modify   : 
  * @see also : 
  ************************************************************************
  * @attention
  ************************************************************************
  */
int dwDlt698DataLen(UINT8 *ucRcvBuf, int dwRcvLen)
{
    assert(ucRcvBuf);

    UINT16 Len = 0;
    UINT8 *Ptr = ucRcvBuf;

    int i = 0, j = 0, Res = 0, varLen = 0, varLenB = 0, LenBytes = 0; // 可变长度

    /* data identify */
    Ptr++;
    Len++;

    switch (ucRcvBuf[0])
    {
    case DL_NULL:
    {
        /* data length */
        /* data contents */
    }
    break;
    case DL_ARRAY:
    {
        // data length
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        // data contents
        for (i = 0; i < varLen; i++)
        {
            if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) <= 0)
            {
                return -1;
            }

            Ptr += Res;
            Len += Res;
        }
    }
    break;
    case DL_STRUCT:
    {
        // data length
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        // data contents
        for (i = 0; i < varLen; i++)
        {
            if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) <= 0)
            {
                return -1;
            }

            Ptr += Res;
            Len += Res;
        }
    }
    break;
    case DL_BOOL:
    {
        /* data length */
        /* data contents */
        Ptr++;
        Len++;
    }
    break;
    case DL_BIT_STR:
    {
        /* data length */
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        /* data contents */
        if (0 == (varLen % 8))
        {
            Ptr += varLen / 8;
            Len += varLen / 8;
        }
        else
        {
            Ptr += (varLen / 8) + 1;
            Len += (varLen / 8) + 1;
        }
    }
    break;
    case DL_D_LONG:
    {
        /* data length */
        /* data contents */
        Ptr += 4;
        Len += 4;
    }
    break;
    case DL_D_LONG_UNS:
    {
        /* data length */
        /* data contents */
        Ptr += 4;
        Len += 4;
    }
    break;
    case DL_OCTET_STR:
    {
        /* data length */
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        /* data contents */
        Ptr += varLen;
        Len += varLen;
    }
    break;
    case DL_VISIBLE_STR:
    {
        /* data length */
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        /* data contents */
        Ptr += varLen;
        Len += varLen;
    }
    break;
    case DL_UTF8_STR:
    {
        /* data length */
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        /* data contents */
        Ptr += varLen;
        Len += varLen;
    }
    break;
    case DL_INTEGER:
    {
        /* data length */
        /* data contents */
        Ptr++;
        Len++;
    }
    break;
    case DL_LONG:
    {
        /* data length */
        /* data contents */
        Ptr += 2;
        Len += 2;
    }
    break;
    case DL_CHAR_UNS:
    {
        /* data length */
        /* data contents */
        Ptr += 1;
        Len += 1;
    }
    break;
    case DL_LONG_UNS:
    {
        /* data length */
        /* data contents */
        Ptr += 2;
        Len += 2;
    }
    break;
    case DL_LONG64:
    {
        /* data length */
        /* data contents */
        Ptr += 8;
        Len += 8;
    }
    break;
    case DL_LONG64_UNS:
    {
        /* data length */
        /* data contents */
        Ptr += 8;
        Len += 8;
    }
    break;
    case DL_ENUM:
    {
        /* data length */
        /* data contents */
        Ptr += 1;
        Len += 1;
    }
    break;
    case DL_FLOAT32:
    {
        /* data length */
        /* data contents */
        Ptr += 4;
        Len += 4;
    }
    break;
    case DL_FLOAT64:
    {
        /* data length */
        /* data contents */
        Ptr += 8;
        Len += 8;
    }
    break;
    case DL_DATE_TIME:
    {
        /* data length */
        /* data contents */
        Ptr += 10;
        Len += 10;
    }
    break;
    case DL_DATE:
    {
        /* data length */
        /* data contents */
        Ptr += 5;
        Len += 5;
    }
    break;
    case DL_TIME:
    {
        /* data length */
        /* data contents */
        Ptr += 3;
        Len += 3;
    }
    break;
    case DL_DATE_TIME_S:
    {
        /* data length */
        /* data contents */
        Ptr += 7;
        Len += 7;
    }
    break;
    case DL_OI:
    {
        /* data length */
        /* data contents */
        Ptr += 2;
        Len += 2;
    }
    break;
    case DL_OAD:
    {
        /* data length */
        /* data contents */
        Ptr += 4;
        Len += 4;
    }
    break;
    case DL_ROAD:
    {
        Ptr += 4;
        Len += 4;

        // OAD number
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        // relevance OAD
        for (i = 0; i < varLen; i++)
        {
            Ptr += 4;
            Len += 4;
        }
    }
    break;
    case DL_OMD:
    {
        /* data length */
        /* data contents */
        Ptr += 4;
        Len += 4;
    }
    break;
    case DL_TI:
    {
        /* data length */
        /* data contents */
        Ptr += 3;
        Len += 3;
    }
    break;
    case DL_TSA: // octet-string
    {
        /* data length */
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        /* data contents */
        Ptr += varLen;
        Len += varLen;
    }
    break;
    case DL_MAC: // octet-string
    case DL_RN:  // octet-string
    {
        /* data length */
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        /* data contents */
        Ptr += varLen;
        Len += varLen;
    }
    break;
    case DL_REGION:
    {
        /* data length */
        /* data contents */
        Ptr += 1;
        Len += 1; // ENUMERATED

        if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) < 0)
        { // start data
            return -1;
        }
        Ptr += Res;
        Len += Res;

        if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) < 0)
        { // end data
            return -1;
        }
        Ptr += Res;
        Len += Res;
    }
    break;
    case DL_SCALER_UNIT:
    {
        /* data length */
        /* data contents */
        Ptr += 1;
        Len += 1;

        Ptr += 1;
        Len += 1; // ENUMERATED
    }
    break;
    case DL_RSD:
    {
        if ((Res = dwDlt698RSDLen(Ptr, dwRcvLen - Len)) < 0)
        {
            return -1;
        }

        Ptr += Res;
        Len += Res;
    }
    break;
    case DL_CSD:
    {
        if (*Ptr == 0)
        {
            Ptr += 1; // choice
            Len += 1;

            Ptr += 4; // OAD
            Len += 4;
        }
        else if (*Ptr == 1)
        {
            Ptr += 1; // choice
            Len += 1;

            Ptr += 4; // OAD
            Len += 4;

            // relevance OAD number
            if ((varLenB = dwDlt698Len(Ptr, &LenBytes)) < 0)
            {
                return -1;
            }

            Len += LenBytes;
            Ptr += LenBytes;

            // relevance OAD
            for (j = 0; j < varLenB; j++)
            {
                if (Len > dwRcvLen)
                {
                    return -1;
                }

                Ptr += 4;
                Len += 4;
            }
        }
        else
        {
            return -1;
        }
    }
    break;
    case DL_MS:
    {
        if ((Res = dwDlt698MsLen(Ptr, dwRcvLen - Len)) < 0)
        {
            return -1;
        }

        Ptr += Res;
        Len += Res;
    }
    break;
    case DL_SID:
    {
        /* data length */
        /* data contents */
        Ptr += 4;
        Len += 4;

        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        Ptr += varLen;
        Len += varLen;
    }
    break;
    case DL_SID_MAC:
    {
        /* SID */
        Ptr += 4;
        Len += 4;

        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        Ptr += varLen;
        Len += varLen;

        /* MAC */
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        Ptr += varLen;
        Len += varLen;
    }
    break;
    case DL_COMDCB:
    {
        Ptr += 5;
        Len += 5;
    }
    break;
    case DL_RCSD:
    {
        // CSD number
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        // CSD
        for (i = 0; i < varLen; i++)
        {
            if (Len > dwRcvLen)
            {
                return -1;
            }

            if (*Ptr == 0)
            {
                Ptr += 1; // choice
                Len += 1;

                Ptr += 4; // OAD
                Len += 4;
            }
            else if (*Ptr == 1)
            {
                Ptr += 1; // choice
                Len += 1;

                Ptr += 4; // OAD
                Len += 4;

                // relevance OAD number
                if ((varLenB = dwDlt698Len(Ptr, &LenBytes)) < 0)
                {
                    return -1;
                }

                Len += LenBytes;
                Ptr += LenBytes;

                // relevance OAD
                for (j = 0; j < varLenB; j++)
                {
                    if (Len > dwRcvLen)
                    {
                        return -1;
                    }

                    Ptr += 4;
                    Len += 4;
                }
            }
            else
            {
                return -1;
            }
        }
    }
    break;
    default:
        return -1;
    }

    if (Len > dwRcvLen)
    {
        return -1;
    }

    return Len;
}
/**
  ************************************************************************
  * @brief    : count RSD length in the ucRcvBuf
  * @param    : ucRcvBuf : source code (do not contain data identifier)
  *             dwRcvLen : valid length of source code
  * @return   : -1   : error    
                else : data length of RSD in the source code
  * @author   : Lixg
  * @version  : 0.1
  * @date     : 2017-7-21 13:37:37
  * @modify   : 
  * @see also : 
  ************************************************************************
  * @attention
  ************************************************************************
  */
int dwDlt698RSDLen(UINT8 *ucRcvBuf, int dwRcvLen)
{
    assert(ucRcvBuf);

    UINT16 Len = 0;
    UINT8 *Ptr = ucRcvBuf;

    int i = 0, Res = 0, varLen = 0, LenBytes = 0; // 可变长度

    if (*Ptr == 0)
    {
        Ptr += 1;
        Len += 1; // choice

        //Ptr += 1;
        //Len += 1; // NULL
    }
    else if (*Ptr == 1)
    {
        Ptr += 1;
        Len += 1; // choice

        Ptr += 4;
        Len += 4; // OAD

        if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) < 0)
        { // data
            return -1;
        }
        Ptr += Res;
        Len += Res;
    }
    else if (*Ptr == 2)
    {
        Ptr += 1;
        Len += 1; // choice

        Ptr += 4;
        Len += 4; // OAD

        if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) < 0)
        { // start data
            return -1;
        }
        Ptr += Res;
        Len += Res;

        if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) < 0)
        { // end data
            return -1;
        }
        Ptr += Res;
        Len += Res;

        if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) < 0)
        { // TI
            return -1;
        }
        Ptr += Res;
        Len += Res;
    }
    else if (*Ptr == 3)
    {
        Ptr += 1;
        Len += 1; // choice

        // relevance OAD number
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        for (i = 0; i < varLen; i++)
        {
            Ptr += 4;
            Len += 4; // OAD

            if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) < 0)
            { // start data
                return -1;
            }
            Ptr += Res;
            Len += Res;

            if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) < 0)
            { // end data
                return -1;
            }
            Ptr += Res;
            Len += Res;

            if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) < 0)
            { // TI
                return -1;
            }
            Ptr += Res;
            Len += Res;
        }
    }
    else if (*Ptr == 4 || *Ptr == 5)
    {
        Ptr += 1;
        Len += 1; // choice

        Ptr += 7;
        Len += 7; // data_time_s

        if ((Res = dwDlt698MsLen(Ptr, dwRcvLen - Len)) < 0)
        {
            return -1;
        }

        Ptr += Res; // MS
        Len += Res;
    }
    else if (*Ptr == 6 || *Ptr == 7 || *Ptr == 8)
    {
        Ptr += 1;
        Len += 1; // choice

        Ptr += 7;
        Len += 7; // data_time_s

        Ptr += 7;
        Len += 7; // data_time_s

        Ptr += 3;
        Len += 3; // TI

        if ((Res = dwDlt698MsLen(Ptr, dwRcvLen - Len)) < 0)
        {
            return -1;
        }

        Ptr += Res; // MS
        Len += Res;
    }
    else if (*Ptr == 9)
    {
        Ptr += 1;
        Len += 1; // choice

        Ptr += 1;
        Len += 1; // last n
    }
    else if (*Ptr == 10)
    {
        Ptr += 1;
        Len += 1; // choice

        Ptr += 1;
        Len += 1; // last n

        if ((Res = dwDlt698MsLen(Ptr, dwRcvLen - Len)) < 0)
        {
            return -1;
        }

        Ptr += Res; // MS
        Len += Res;
    }
    else
    {
        return -1;
    }

    if (Len > dwRcvLen)
    {
        return -1;
    }

    return Len;
}

/**
  ************************************************************************
  * @brief    : count MS length in the ucRcvBuf
  * @param    : ucRcvBuf : source code (do not contain data identifier)
  *             dwRcvLen : valid length of source code
  * @return   : -1   : error    
                else : data length of MS in the source code
  * @author   : Lixg
  * @version  : 0.1
  * @date     : 2017-7-21 13:37:37
  * @modify   : 
  * @see also : 
  ************************************************************************
  * @attention
  ************************************************************************
  */
int dwDlt698MsLen(UINT8 *ucRcvBuf, int dwRcvLen)
{
    assert(ucRcvBuf);

    UINT16 Len = 0;
    UINT8 *Ptr = ucRcvBuf;

    int i = 0, Res = 0, varLen = 0, varLenB = 0, LenBytes = 0; // 可变长度

    if (*Ptr == 0)
    {
        Ptr += 1;
        Len += 1; // choice - no meter
    }
    else if (*Ptr == 1)
    {
        Ptr += 1;
        Len += 1; // choice - all meter
    }
    else if (*Ptr == 2)
    {
        Ptr += 1;
        Len += 1; // choice - sequence of user type

        // number of user type
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        for (i = 0; i < varLen; i++)
        {
            Len += 1;
            Ptr += 1;
        }
    }
    else if (*Ptr == 3)
    {
        Ptr += 1;
        Len += 1; // choice - sequence of tsa

        // number of user type
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        for (i = 0; i < varLen; i++)
        {
            if ((varLenB = dwDlt698Len(Ptr, &LenBytes)) < 0)
            {
                return -1;
            }

            Len += LenBytes;
            Ptr += LenBytes;

            /* data contents */
            Ptr += varLenB;
            Len += varLenB;
        }
    }
    else if (*Ptr == 4)
    {
        Ptr += 1;
        Len += 1; // choice - sequence of configno

        // number of user type
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        for (i = 0; i < varLen; i++)
        {
            Len += 2;
            Ptr += 2;
        }
    }
    else if (*Ptr == 5 || *Ptr == 6 || *Ptr == 7)
    {
        Ptr += 1;
        Len += 1; // choice - sequence of region

        // number of user type
        if ((varLen = dwDlt698Len(Ptr, &LenBytes)) < 0)
        {
            return -1;
        }

        Len += LenBytes;
        Ptr += LenBytes;

        for (i = 0; i < varLen; i++)
        {
            Ptr += 1;
            Len += 1; // ENUMERATED

            if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) < 0)
            { // start data
                return -1;
            }
            Ptr += Res;
            Len += Res;

            if ((Res = dwDlt698DataLen(Ptr, dwRcvLen - Len)) < 0)
            { // end data
                return -1;
            }
            Ptr += Res;
            Len += Res;
        }
    }
    else
    {
        return -1;
    }

    if (Len > dwRcvLen)
    {
        return -1;
    }

    return Len;
}
#if 0
/*******************************************************************
*通过OAD获取数据放入发送缓冲区
********************************************************************/
UINT16 uwGetDateThrOAD(GET_NORMAL_INFO *pstGetNormalInfo, UINT8 *pucSendBuf, UINT16 uwSendLength)
{
    UINT16 uwDataLen = 0;
    UINT16 uwOADDateLen = 0;
    UINT16 uwOI;

    uwOI = pstGetNormalInfo->stOAD.OI_date;

    *pucSendBuf++ = MSB(uwOI);
    *pucSendBuf++ = LSB(uwOI);
    *pucSendBuf++ = pstGetNormalInfo->stOAD.attr_ID;
    *pucSendBuf++ = pstGetNormalInfo->stOAD.attr_index;
    uwDataLen += 4;

    switch (pstGetNormalInfo->stOAD.OI_date)
    {
    case OI_DAY_FRZ: //0x5004
        uwOADDateLen = uwGetDayFRZ(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_MONTHFRZ: //0x5006
        uwOADDateLen = uwGetMonthFRZ(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_BRVERACT_HARMONIC_WAVE: //0x0222
        uwOADDateLen = uwGetBRveractHarmonicWave(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_DATETIME: //0x4000
        uwOADDateLen = uwGetDatetimeAttr(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_COMMADDR: //0x4001
        uwOADDateLen = uwGetCommAddr(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_CUSTOMER_NO: //0x4003
        uwOADDateLen = uwGetCustomerNo(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_POSITION: //0x4004
        uwOADDateLen = uwGetPosition(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_GROUPADDR: //0x4005
        uwOADDateLen = uwGetGroupAddr(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_CLKSOURCE: //0x4006
        uwOADDateLen = uwGetClockSource(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_LCDPARA: //0x4007
        uwOADDateLen = uwGetLCDPara(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_TZ_TI_NUM: //0x400C
        uwOADDateLen = uwGetTimeZone(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_METERING_NUM: //0x4010
        uwOADDateLen = uwGetMeteringNum(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_PRESET_TZ_TAB: //0x4014
        uwOADDateLen = uwGetCurTZTab(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_PRESENT_DAYTI_TAB: //0x4016
        uwOADDateLen = uwGetPresentDayTITab(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_PRESET_TARIFF_PRICE: //0x4018
        uwOADDateLen = uwGetTariffPrice(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_CURRENT_TRANS_RATIO: //0x401C
        uwOADDateLen = uwGetCurrentTransRatio(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_VOLTAGE_TRANS_RATIO: //0x401D
        uwOADDateLen = uwGetVoltageTransRatio(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_TERMINAL_ELIMINATE: //0x4024
        uwOADDateLen = uwGetEliminate(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_VOLTAGE_QUALIFIED_RATE: //0x4030
        uwOADDateLen = uwGetVoltageQualifiedRate(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_MAX_DEMAND_PERIOD: //0x4100
        uwOADDateLen = uwGetMaxDemandPeriod(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_SLIDER_TIME: //0x4101
        uwOADDateLen = uwGetSliderTime(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ASSET_MANAGEMENT_CODE: //0x4103
        uwOADDateLen = uwGetAssetCode(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_RATED_VOLTAGE: //0x4104
        uwOADDateLen = uwGetRatedVoltage(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_RATED_CURRENT: //0x4105
        uwOADDateLen = uwGetRatedCurrent(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_MAX_CURRENT: //0x4106
        uwOADDateLen = uwGetMaxCurrent(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_METER_ACTV_CONSTANT: //0x4109
        uwOADDateLen = uwGetMeterActvConstant(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_METER_REACTV_CONSTANT: //0x410A
        uwOADDateLen = uwGetMeterReactvConstant(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ABC_PHAS_CONDCT_COEFF: //0x410C
        uwOADDateLen = uwGetABCPhasConductCoeff(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ABC_PHAS_REACT_COEFF: //0x410D
        uwOADDateLen = uwGetABCPhasReactCoeff(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ABC_PHAS_RESIST_COEFF: //0x410E
        uwOADDateLen = uwGetABCPhasResistCoeff(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ABC_PHAS_SUSCEPT_COEFF: //0x410F
        uwOADDateLen = uwGetABCPhasSusceptCoeff(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_STLMNT_DATE: //0x4116
        uwOADDateLen = uwGetSettlementDate(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_CASCADE_COMM_PARAM: //0x4202
        uwOADDateLen = uwGetCascadeCommParam(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_BROADCAST_TIME: //0x4204
        uwOADDateLen = uwGetBroadcastTiming(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_DEVICE: //0x4300
        uwOADDateLen = uwGetDevice(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_APP_CONNECTION: //0x4400
        uwOADDateLen = uwGetAppConnection(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_APP_AUTHENTCTN_PWD: //0x4401
        uwOADDateLen = uwGetAppAuthentctnPwd(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_WIRELESS_NETWORK_COMM1: //0x4500
        uwOADDateLen = uwGetCommPara(pstGetNormalInfo, pucSendBuf, 0, uwSendLength + 4);
        break;
    case OI_WIRELESS_NETWORK_COMM2: //0x4501
        uwOADDateLen = uwGetCommPara(pstGetNormalInfo, pucSendBuf, 1, uwSendLength + 4);
        break;
    case OI_ETHERNET_COMM1: //0x4510
        uwOADDateLen = uwGetEthernetComm(pstGetNormalInfo, pucSendBuf, 0, uwSendLength + 4);
        break;
    case OI_ETHERNET_COMM2: //0x4511
        uwOADDateLen = uwGetEthernetComm(pstGetNormalInfo, pucSendBuf, 1, uwSendLength + 4);
        break;
    case OI_METER_CALI_TIME: //0x3016
        uwOADDateLen = uwGetMeterTiming(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_METER_VOLTAGE_RVER_PH: //0x300F
        uwOADDateLen = uwGetVoltageReversePhaseAttr(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_TER_INITIALIZATION: //0x3100
        uwOADDateLen = uwGetTerInitialize(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_TER_VERSION_CHANGE: //0x3101
        uwOADDateLen = uwGetTerVerChange(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_TER_STATUS_CHANGE: //0x3104(记录型对象不应通过GetNormal来读取,此处例外)
        uwOADDateLen = uwGetTerStatusChange(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_CLOCK_BEYOND: //0x3105
        uwOADDateLen = uwGetClockBeyond(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_TERMINAL_OUTAGE: //0x3106
        uwOADDateLen = uwGetTerminalOutage(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_DC_UPPER_LIMIT: //0x3107
        uwOADDateLen = uwGetDCUpperLimit(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_DC_LOWER_LIMIT: //0x3108
        uwOADDateLen = uwGetDCLowerLimit(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_TER_MSG_CERTIFY: //0x3109
        uwOADDateLen = uwGetTerMsgCertify(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_DEVICE_FAULT: //0x310A
        uwOADDateLen = uwGetDeviceFaultRecord(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_METER_DATA_FALL: //0x310B
        uwOADDateLen = uwGetMeterDataFall(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ENERGY_BEYOND: //0x310C
        uwOADDateLen = uwGetEnergyBeyond(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_METER_FLY: //0x310D
        uwOADDateLen = uwGetMeterFly(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_METER_STOP: //0x310E
        uwOADDateLen = uwGetMeterStop(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_COLLECT_FAIL: //0x310F
        uwOADDateLen = uwGetCollectFail(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_COMM_FLOW_BEYOND: //0x3110
        uwOADDateLen = uwGetCommFlowBeyond(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_FIND_UNKNOWN_METER: //0x3111
        uwOADDateLen = uwGetFindUnknownMeter(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_CROSS_REGION_METER: //0x3112
        uwOADDateLen = uwGetCrossAreaMeter(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_SET_CLOCK: //0x3114
        uwOADDateLen = uwGetTerTimingEvent(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_YK: //0x3115
        uwOADDateLen = uwGetRemoteTripRecord(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ACT_EXCEED_LIMIT_RCRD: //0x3116
        uwOADDateLen = uwGetActExceedLimitRcrd(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_OUTLOOP_STAT_CHANGE: //0x3117
        uwOADDateLen = uwGetOutputInsertStatusChange(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_TER_PROG_RCRD: //0x3118
        uwOADDateLen = uwGetTerProgramRecord(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_TER_CURRENTLOOP_ABNORMAL: //0x3119
        uwOADDateLen = uwGetTerCurrException(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_METER_NET_STATE_SWITCH: //0x311A
        uwOADDateLen = uwGetMeterNetStateSwitch(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_TER_METER_CALI_RCRD: //0x311A
        uwOADDateLen = uwGetTerTimingMeter(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_METER_DATA_CHANGE: //0x311C
        uwOADDateLen = uwGetMeterDataChange(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_PWR_CTRL_TRIP_RCRD: //0x3200
        uwOADDateLen = uwGetPowControlTrip(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ELECCTRL_TRIP_RCRD: //0x3201
        uwOADDateLen = uwGetElecControlTrip(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_PURCHASE_ELEC_PARA_SET_RCRD: //0x3202
        uwOADDateLen = uwGetPurchaseParaRecord(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ELEC_CTRL_ALARM_RCRD: //0x3203
        uwOADDateLen = uwGetElecControlAlarm(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_COLL_DOC_CONFIG_TAB: //0x6000
        uwOADDateLen = uwGetCollDocConfigTab(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
#ifdef SHANXI
    case OI_MICRO_POWER_COMMUNICATION: //0x6005
        uwOADDateLen = uwGetMicroPowerCom(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
#endif
#if defined(ZHEJIANG_698J)
    case OI_SEARCH_TAB: //0x6002
        uwOADDateLen = uwGetSearchMeterPara(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
#endif
    case OI_TASK_CONFIG_TABLE: //0x6012
        uwOADDateLen = uwGetTaskConfigTable(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_GENERAL_COLL_SCHEME_SET: //0x6014
        uwOADDateLen = uwGetGeneralCollSchemeSet(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_EVENT_COLL_SCHEME_SET: //0x6016
        uwOADDateLen = uwGetEventCollSchemeSet(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    //case OI_TRANSPARENT_SCHEME_SET:	//0x6018
    //uwOADDateLen = uwGetTransparentSchemeSet(pstGetNormalInfo, pucSendBuf, uwSendLength+4);
    //break;
    case OI_REPORT_SCHEME_SET: //0x601C
        uwOADDateLen = uwGetReportSchemeSet(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;

    case OI_COLLECT_RULE_LIB: //0x601E
        uwOADDateLen = uwGetCollectRuleLib(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;

    case OI_TG1: //0x2301
    case OI_TG2: //0x2302
    case OI_TG3: //0x2303
    case OI_TG4: //0x2304
    case OI_TG5: //0x2305
    case OI_TG6: //0x2306
    case OI_TG7: //0x2307
    case OI_TG8: //0x2308
        uwOADDateLen = uwGetTG1_8Data(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;

    case OI_PMETER1: //0x2401
    case OI_PMETER2: //0x2402
    case OI_PMETER3: //0x2403
    case OI_PMETER4: //0x2404
    case OI_PMETER5: //0x2405
    case OI_PMETER6: //0x2406
    case OI_PMETER7: //0x2407
    case OI_PMETER8: //0x2408
        uwOADDateLen = uwGetPM1_8Data(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_CMB_ACT:
    case OI_FWACT:                  //当前正向有功电能 0x0010
    case OI_RVERACT:                //当前反向有功电能 0x0020
    case OI_CMB_REACT_1:            //当前正向无功电能 0x0030
    case OI_CMB_REACT_2:            //当前反向无功电能 0x0040
    case OI_FSTQUAD_REACT:          //第一象限反向无功电能 0x0050
    case OI_SNDQUAD_REACT:          //第二象限反向无功电能 0x0060
    case OI_THDQUAD_REACT:          //第三象限反向无功电能 0x0070
    case OI_FUTHQUAD_REACT:         //第四象限反向无功电能 0x0080
    case OI_COPPERLOSS_ACT_COMPALL: //铜损有功电能 0x0300
    case OI_IRONLOSS_ACT_COMPALL:   //铁损有功电能 0x0400
    case OI_AFWACT:                 //A相正向有功电能 0x0011
    case OI_ARVERACT:               //A相反向有功电能 0x0021
    case OI_ACMB_REACT_1:           //A相组合无功1 0x0031
    case OI_ACMB_REACT_2:           //A相组合无功2 0x0041
    case OI_BFWACT:                 //B相正向有功电能 0x0012
    case OI_BRVERACT:               //B相反向有功电能 0x0022
    case OI_BCMB_REACT_1:           //当前B相组合无功1 0x0032
    case OI_BCMB_REACT_2:           //当前B相组合无功2 0x0042
    case OI_CFWACT:                 //当前C相正向有功电能 0x0013
    case OI_CRVERACT:               //当前C相反向有功电能 0x0023
    case OI_CCMB_REACT_1:           //当前C相组合无功1电能 0x0033
    case OI_CCMB_REACT_2:           //当前C相组合无功2电能 0x0043
    case OI_FWACT_MAXDEM:           //当前正向有功总最大需量 0x1010
    case OI_RVERACT_MAXDEM:         //当前反向有功总最大需量 0x1020
    case OI_CMB_REACT1_MAXDEM:      //当前正向无功总最大需量 0x1030
    case OI_CMB_REACT2_MAXDEM:      //当前反向无功总最大需量 0x1040
    case OI_ACTIVEPOWER:            //当前有功功率 0x2004
    case OI_REACTIVEPOWER:          //当前总及分相无功功率 0x2005
    case OI_APPARENTPOWER:          //当前总及分相视在功率 0x2006
    case OI_VOLTAGE:                //当前三相电压 0x2000
    case OI_CURRENT:                //当前三相电流 0x2001
    case OI_POWERFACTOR:            //当前总及分相功率因数 0x200A
    case OI_VOLPhANGLE:             //当前电压相角 0x2002
    case OI_CURRPhANGLE:            //当前电压电流相角 0x2003
    case OI_PHASEA_VOL_QUALIFICATION_RATE:
    case OI_PHASEB_VOL_QUALIFICATION_RATE:
    case OI_PHASEC_VOL_QUALIFICATION_RATE:
        uwOADDateLen = uwGetACMeasureData(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;

    case OI_COMMFLOW: //0x2200
        uwOADDateLen = uwGetCommFlowData(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_RUNTIME: //0x2203
        uwOADDateLen = uwGetPowOnTimeData(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_RESETTM: //0x2204
        uwOADDateLen = uwGetResetNumData(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ESAM: //0xF100
        uwOADDateLen = uwGetESAMINFO(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ESAMSAFEMODEPARA: //0xF101
        uwOADDateLen = uwGetESAMSAFEMODE(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_SWITCHINPUT: //0xF203 数据读取及属性4参数-开关量接入及属性标志读取
        uwOADDateLen = uwGetSwitchInput(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_RELAY_OUTPUT: //0xF205 继电器输出
#ifdef EMC_TEST //台体测试设置后立刻召测，终端数据未更新，故加延时
        taskDelay(sysClkRateGet() << 1);
#endif
        uwOADDateLen = uwGetRelayOutput(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ALARM_OUTPUT: //0xF206 继电器输出
        uwOADDateLen = uwGetAlarmOutput(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_FILE_BLOCK_TRANS_MANAGE: //0xF001
        uwOADDateLen = uwGetFileBlockTrans(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_REMOTE_CONTROL: //0x8000
        uwOADDateLen = uwGetRemoteControl(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_ENSURE_POWER_SERVING: //0x8001
#ifdef EMC_TEST //台体测试设置后立刻召测，终端数据未更新，故加延时
        taskDelay(sysClkRateGet() << 1);
#endif
        uwOADDateLen = uwGetEnsurePowerServing(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_DUN_ALARM: //0x8002
        uwOADDateLen = uwGetDunAlarm(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_SAFETY_VALUE: //0x8100
        uwOADDateLen = uwGetSafetyValue(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_CONTROL_PERIOD: //0x8101
        uwOADDateLen = uwGetPowerControlPeriod(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_PCONTROL_ALARM_TIME: //0x8102
        uwOADDateLen = uwGetPowerControlAlarmTime(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_TI_PWR_CTRL: //0x8103
        uwOADDateLen = uwGetTimePowerControl(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_FACTORY_REST_CTRL: //0x8104
        uwOADDateLen = uwGetFactoryRestCtrl(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_BUSINESS_STOP_CTRL: //0x8105
        uwOADDateLen = uwGetBusinessStopCtrl(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_PRE_PWR_FLOAT_DW_CTRL: //0x8106
        uwOADDateLen = uwGetPwrFloatDwCtrl(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_BUY_ELEC_CTRL: //0x8107
        uwOADDateLen = uwGetBuyElecCtrl(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_MONTH_ELEC_CTRL: //0x8108
        uwOADDateLen = uwGetMonthElecCtrl(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
#ifndef LCD_LM10T
    case OI_PLC: //0xF209
        uwOADDateLen = uwGetPLCInfo(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
#endif
    case OI_EVENT_FLAG: //0xFF65
        uwOADDateLen = uwGetEventFlag(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_FACTORY_TEST: //0xFF64 出厂测试
        uwOADDateLen = uwGetFactoryTest(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    case OI_EXTENDING_LOG: //0xFF66 报文记录
        uwOADDateLen = uwGetExtendLog(pstGetNormalInfo, pucSendBuf, uwSendLength + 4);
        break;
    default:
        *pucSendBuf++ = ERROR_INFO;
        if ((uwOI >= 0x9000) && (uwOI < 0xF000))
        {
            *pucSendBuf++ = DAR_OBJ_NOT_EXIST;
        }
        else
        {
            *pucSendBuf++ = DAR_OBJ_NOT_DEF;
        }
        uwOADDateLen = 2;
        pstGetNormalInfo->eIsComplete = GET_COMPLETE;
        break;
    }

    if (pstGetNormalInfo->eIsComplete == GET_NOT_COMPLETE)
    {
        uwDataLen = 0;
    }
    else
    {
        uwDataLen += uwOADDateLen;
    }

    return uwDataLen;
}
#endif

#if 0
/*
*********************************************************************
* 功    能：根据数据类型获取长度及时间
* 输    入：
* 输    出：void
* 作    者：ddy
* 编写日期：2017-04-25
* 修改日期: 
**********************************************************************
*/
BOOL FormatDataToBinaryS(UINT8 *pData, UINT8 byDataFormat, struct tm *pstTime, UINT16 *pwDataLen)
{
    BOOL blDataValid = TRUE;
    *pwDataLen = 0;

    switch (byDataFormat)
    {
    case DL_DATE_TIME:
    {
        if (pData[0] == 0xFF)
        {
            blDataValid = FALSE;
        }
        else
        {
            pstTime->tm_year = (pData[0] << 8) + pData[1] - 1900;
            pstTime->tm_mon = pData[2] - 1;
            pstTime->tm_mday = pData[3];
            //星期
            pstTime->tm_wday = pData[4];
            pstTime->tm_hour = pData[5];
            pstTime->tm_min = pData[6];
            pstTime->tm_sec = pData[7];
            *pwDataLen = 10;
        }
    }
    break;
    case DL_DATE:
    {
        if (pData[0] == 0xFF)
        {
            blDataValid = FALSE;
        }
        else
        {
            pstTime->tm_year = (pData[0] << 8) + pData[1] - 1900;
            pstTime->tm_mon = pData[2] - 1;
            pstTime->tm_mday = pData[3];
            //星期 pData[4]
            pstTime->tm_wday = pData[4];
            *pwDataLen = 5;
        }
    }
    break;
    case DL_TIME:
    {
        if (pData[0] == 0xFF)
        {
            blDataValid = FALSE;
        }
        else
        {
            pstTime->tm_hour = pData[0];
            pstTime->tm_min = pData[1];
            pstTime->tm_sec = pData[2];
            *pwDataLen = 3;
        }
    }
    break;
    case DL_DATE_TIME_S:
    {
        if (pData[0] == 0xFF)
        {
            blDataValid = FALSE;
        }
        else
        {
            pstTime->tm_year = (pData[0] << 8) + pData[1] - 1900;
            pstTime->tm_mon = pData[2] - 1;
            pstTime->tm_mday = pData[3];
            pstTime->tm_hour = pData[4];
            pstTime->tm_min = pData[5];
            pstTime->tm_sec = pData[6];
            *pwDataLen = 7;
        }
    }
    break;
    default:
        blDataValid = FALSE;
        break;
    }
    return blDataValid;
}
#endif