#include "Flash.h"
#include "DLT698.h"
#include "BaseDef.h"
#include "DLT645.h"

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
        /*?????????????????????????????????0?????????1??????*/
        case 0:
        {
            if ((Len = dwDlt698Len(&ucSecurityBuf[2], &Bytes)) < 0)
            {
                return -1;
            }
            //else if (eChan < COLLECT_PLC) //??????485????????????Mac??????
            //{
            //??????mac??????
            //if ((ucSecurityBuf[2 + Bytes + Len] == 1) &&
            //(ucSecurityBuf[2 + Bytes + Len + 1] == 0) &&
            //(ucSecurityBuf[2 + Bytes + Len + 2] == 4)) //mac??????4
            //{
            //if (ERROR == dwValidateMAC(&ucSecurityBuf[2 + Bytes + Len + 1 + 1 + 1], &ucSecurityBuf[2 + Bytes], Len, pTsa, eChan))
            //{
            //??????????????????-2
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
//???485?????????????????????
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
    UINT16 dwLen;
#if 1
    /*security response len*/
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
* ???    ???????????????
* ???    ??????
* ???    ??????
* ???    ??????ddy
* ???????????????2017-05-24
* ????????????: 
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
    //??????????????????: 0x68
    if (pframe[0] != 0x68)
    {
        return -1;
    }

    //?????????2byte???????????????
    uwframeLen = MAKE_WORD(pframe[2], pframe[1]);
    /*SA??????????????????0??????1?????????*/
    SA_Len = (pframe[4] & 0x0F) + 2;
    nHCSPos += SA_Len + 5;

    //????????????hcs
    uwhcs = PPPINITFCS16;
    uwhcs = pppfcs16(uwhcs, pframe + 1, nHCSPos - 1);

    if (uwhcs != MAKE_WORD(pframe[nHCSPos + 1], pframe[nHCSPos]))
    {
        return -1;
    }

    //?????????fcs
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

    pstOad->attr_ID = *pucStr;
    pucStr++;
    pstOad->attr_index = *pucStr;
    pucStr++;

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

/*********??????????????????********/
#if 0 //stragen
    if (1 == *Ptr++) //??????????????????
    {
        //?????????????????????????????????  [1]  SEQUENCE OF A-ResultNormal
        if (1 == *Ptr++)
        {
            ucNum = *Ptr++; // ??????SEQUENCE OF A-ResultNormal

            for (i = 0; i < ucNum; i++)
            {
                Ptr += dwGetOad(Ptr, &stOAD);
                if ((0x3320 == stOAD.OI_date) && (0x02 == stOAD.attr_ID) && (0x01 == stOAD.attr_index))
                {
                    if (1 == *Ptr++) //????????????
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
    /*??????????????????????????????*/
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

    bzero(stCollData, sizeof(COLL_STORE_DATA));
    //stCollData.uwConfigNo = uwConfigNo;

    for (i = 0; i < ucResultNum; i++)
    {
        Ptr += dwGetOad(Ptr, &stCollData->stMainOAD);
#if 0
        stCollData->stMainOAD.OI_date = MAKE_WORD(*Ptr, *(Ptr + 1));
        Ptr += 2;
        stCollData->stMainOAD.attr_ID = *Ptr++;
        stCollData->stMainOAD.attr_index = *Ptr++;
        Ptr += 4;
#endif

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
            //ROAD ????????????
            else
            {
                return -1;
            }
        }

        //??????????????????????????????
        if (ERROR_INFO == *Ptr) //????????????
        {
            Ptr++; // Choice
            Ptr++; // DAR
        }
        else if (RES_DATA == *Ptr) //????????????
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

                if (0 == stID.ucLastNTimes) //????????????
                {
                    if (OK != dwDealDataStore(&stCollData, stID))
                    {
                        return -1;
                    }
                }
                else //?????????????????????????????????
                {
                    if (OK != dwCollRetryDataStore(&stCollData, stID))
                    {
                        return -1;
                    }
                }
#endif
            }
#if 0
            /*********??????????????????********/
            if (1 == *Ptr++) //??????????????????
            {
                if (1 == *Ptr++) //????????????????????????????????? [1]  SEQUENCE OF A-ResultNormal
                {
                    ucNum = *Ptr++; // ?????? SEQUENCE OF A-ResultNormal

                    for (i = 0; i < ucNum; i++)
                    {
                        Ptr += dwGetOad(Ptr, &stOAD);
                        if ((0x3320 == stOAD.OI_date) && (0x02 == stOAD.attr_ID) && (0x01 == stOAD.attr_index))
                        {
                            if (1 == *Ptr++) //????????????
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
                            if (1 == *Ptr++) //????????????
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

    int i = 0, j = 0, Res = 0, varLen = 0, varLenB = 0, LenBytes = 0; // ????????????

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

    int i = 0, Res = 0, varLen = 0, LenBytes = 0; // ????????????

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

    int i = 0, Res = 0, varLen = 0, varLenB = 0, LenBytes = 0; // ????????????

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

void v698VoltageModify(DATA_UNIT *pDataUnit)
{
    DATA_UNIT *pdata = pDataUnit;
    UINT8 i;
    /*Ptr?????????APDU???????????????????????????????????????APDU?????????*/
    UINT8 *Ptr = pdata->ucPtr;
    UINT8 ucArrays;
    int dwValue;
    int dwVoltageValue[3];

    if (pdata->ucVal == 0)
        return;
    if (*Ptr == 0) //NULL
    {
        return;
    }
    else if (*Ptr++ == 1) //array
    {
        ucArrays = *Ptr++; //????????????????????????A/B/C???
        Ptr++;
        for (i = 0; i < ucArrays; i++)
        {
            dwValue = MAKE_WORD(*Ptr, *(Ptr + 1));
            if ((dwValue > Voltage_MAX) || (dwValue < Voltage_MIN))
            {
                dwVoltageValue[i] = Voltage_Modifier_Method(dwValue);
                *Ptr = MSB(dwVoltageValue[i]);
                *(Ptr + 1) = LSB(dwVoltageValue[i]);
            }
            Ptr += 3;
        }
    }
}

void v698EnergyModify(DATA_UNIT *pDataUnit)
{
    DATA_UNIT *pdata = pDataUnit;
    UINT8 i;
    /*Ptr?????????APDU???????????????????????????????????????APDU?????????*/
    UINT8 *Ptr = pdata->ucPtr;
    UINT8 ucArrays;
    uint32_t dwEnergyValue[5];
    bzero(dwEnergyValue, sizeof(dwEnergyValue));

    if (pdata->ucVal == 0)
        return;
    if (*Ptr == 0) //NULL
    {
        return;
    }
    else if (*Ptr++ == 1) //array
    {
        ucArrays = *Ptr++; //???????????????????????????????????????????????????
        Ptr++;             //data
        Ptr += 5;          //??????????????????????????????????????????
        for (i = 1; i < ucArrays; i++)
        {
            dwEnergyValue[i] = MAKE_LONG(MAKE_WORD(*Ptr, *(Ptr + 1)), MAKE_WORD(*(Ptr + 2), *(Ptr + 3)));
            dwEnergyValue[i] = vNow_Frozen_Modify_Mothod(dwEnergyValue[i],
                                                         FLASH_REAL_1DAYFROZEN_ADDR + 4 * i,
                                                         FLASH_MODIFIED_1DAYFROZEN_ADDR + 4 * i);
            *Ptr = MSB(MSW(dwEnergyValue[i]));
            *(Ptr + 1) = LSB(MSW(dwEnergyValue[i]));
            *(Ptr + 2) = MSB(LSW(dwEnergyValue[i]));
            *(Ptr + 3) = LSB(LSW(dwEnergyValue[i]));
            Ptr += 5; //1????????????4?????????
            dwEnergyValue[0] += dwEnergyValue[i];
        }
        Ptr -= 5 * ucArrays; //???
        *Ptr = MSB(MSW(dwEnergyValue[0]));
        *(Ptr + 1) = LSB(MSW(dwEnergyValue[0]));
        *(Ptr + 2) = MSB(LSW(dwEnergyValue[0]));
        *(Ptr + 3) = LSB(LSW(dwEnergyValue[0]));
    }
}