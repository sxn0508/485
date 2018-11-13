/**
  ******************************************************************************
  * @file     : DLT698_45.h
  * @author   : 
  * @version  : 0.1
  * @date     : 2017-4-26 22:03:11
  * @brief    : This file contains all the ...  
  ******************************************************************************
  * @attention
  *
  * FILE FOR ...
  *
  * Copyright (C), 2017-2027, ieslab
  ******************************************************************************
  */

#ifndef __DLT698_45_H__
#define __DLT698_45_H__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "BaseDef.h"
#include "stm32f37x.h"

/**
 * @note: max value define for parameter
 * @author: 
 * @date: 2017-4-26 21:38:58
 *---------------------------------------------------------------------------------------------
 */
#define MAX_APDU_LENGTH 2048
#define MAX_ROAD_NUM_PER_ROAD 32
#define MAX_TSA_SIZE 16
#define MAX_MAC_SIZE 100
#define MAX_RN_SIZE 100
#define SID_ADDITIONAL_DATA_SIZE 16
#define MAX_RCSD_CSD_NUM 20
#define MAX_SIGNINFO_SIZE 128

    //***************************Э���������Ͷ���*************************************

    //Data
    typedef enum
    {
        DL_NULL = 0,
        DL_ARRAY,
        DL_STRUCT,
        DL_BOOL,
        DL_BIT_STR,
        DL_D_LONG,
        DL_D_LONG_UNS,
        DL_OCTET_STR = 9,
        DL_VISIBLE_STR,
        DL_UTF8_STR = 12,
        DL_INTEGER = 15,
        DL_LONG,
        DL_CHAR_UNS,
        DL_LONG_UNS,
        DL_LONG64 = 20,
        DL_LONG64_UNS,
        DL_ENUM,
        DL_FLOAT32,
        DL_FLOAT64,
        DL_DATE_TIME,
        DL_DATE,
        DL_TIME,
        DL_DATE_TIME_S,
        DL_OI = 80,
        DL_OAD,
        DL_ROAD,
        DL_OMD,
        DL_TI,
        DL_TSA,
        DL_MAC,
        DL_RN,
        DL_REGION,
        DL_SCALER_UNIT,
        DL_RSD,
        DL_CSD,
        DL_MS,
        DL_SID,
        DL_SID_MAC,
        DL_COMDCB,
        DL_RCSD
    } DLT_DATA;

    /**
 * @note: Data typedef - 6.3.3
 * @author: 
 * @date: 2017-4-26 21:39:27
 *----------------------------------------------------------------------------------------------
 */

    //date_time 6.3.3.25
    typedef struct
    {
        UINT16 uwYear;
        UINT8 ucMonth;
        UINT8 ucDayOfMonth;
        UINT8 ucDayOfWeek;
        UINT8 ucHour;
        UINT8 ucMinute;
        UINT8 ucSecond;
        UINT16 ucMilliseconds;
    } DATE_TIME;

    //date 6.3.3.27
    typedef struct
    {
        UINT16 year;
        UINT8 month;
        UINT8 day_of_month;
        UINT8 day_of_week;
    } DATE;

    //time 6.3.3.28
    typedef struct
    {
        UINT8 ucHour;
        UINT8 ucMinute;
        UINT8 ucSecond;
    } TIME;

    //date_time_s 6.3.3.26
    typedef struct
    {
        UINT16 year;
        UINT8 month;
        UINT8 day;
        UINT8 hour;
        UINT8 minute;
        UINT8 second;
    } DATE_TIME_S;

    //OAD 6.3.3.5
    typedef struct
    {
        UINT16 OI_date;
        UINT8 attr_ID;
        UINT8 attr_index;
    } OAD;

    //ROAD 6.3.3.6
    typedef struct
    {
        OAD stOAD;
        UINT8 ucAssociateOADNum;
        OAD stAssociateOAD[MAX_ROAD_NUM_PER_ROAD];
    } ROAD;

    //OMD 6.3.3.12
    typedef struct
    {
        UINT16 OI_date;
        UINT8 attr_Func;
        UINT8 attr_Operator;
    } OMD;

    //TI 6.3.3.15
    typedef enum
    {
        TI_SECOND = 0,
        TI_MINUTE,
        TI_HOUR,
        TI_DAY,
        TI_MONTH,
        TI_YEAR
    } TI_UNIT;

    typedef struct
    {
        TI_UNIT eTimeUnit;
        UINT16 uwInterval;
    } TI;

    //TSA::=octet-string(SIZE(2...17)) 6.3.3.22
    typedef struct
    {
        UINT8 ucAddrHead; // �洢SA�����ֽ�,bit0-bit3��ʾ��ַ����
        UINT8 ucAddress[MAX_TSA_SIZE];
    } TSA;

    //MAC::=octet-string 6.3.3.15
    typedef struct
    {
        UINT8 ucMACSize; // MAC����
        UINT8 ucMACData[MAX_MAC_SIZE];
    } MAC;

    //RN::=octet-string 6.3.3.18
    typedef struct
    {
        UINT8 ucRNNum;
        UINT8 ucRN[MAX_RN_SIZE];
    } RN;

    //Region 6.3.3.23
    typedef enum
    {
        BEGIN_CLOSE_END_OPEN = 0, //ǰ�պ�
        BEGIN_OPEN_END_CLOSE,     //ǰ�����
        BEGIN_CLOSE_END_CLOSE,    //ǰ�պ��
        BEGIN_OPEN_END_OPEN       //ǰ����
    } REGION_UNIT;

    //PHYSICAL_UNIT ����λ��DLT698.45�ο���¼B
    typedef enum
    {
        PHY_YEAR = 1,                        // ʱ��---��
        PHY_MONTH,                           // ʱ��---��
        PHY_WEEK,                            // ʱ��---��
        PHY_DATE,                            // ʱ��---��
        PHY_HOUR,                            // ʱ��---Сʱ
        PHY_MINUTE,                          // ʱ��---��
        PHY_SECOND,                          // ʱ��---��
        PHY_ANGLE,                           //���ࣩ��---��
        PHY_DEGREE_CELSIUS,                  // ���϶�--- ��C
        PHY_RMB_YUAN,                        // ����ҵ�λ---Ԫ
        PHY_METRE,                           // ��---m
        PHY_METRE_PER_SECOND,                // ��ÿ��---m/s
        PHY_CUBIC_METRE,                     // rV���Ǳ���������ֵ(�ݻ�)---m3
        PHY_CUBIC_METRE_CORRECT,             // ���������---m3
        PHY_CUBIC_METRE_PER_HOUR,            // ������λ-������/Сʱ---m3/(60*60s)
        PHY_CUBIC_METRE_PER_HOUR_CORRECT,    //������������λ-������/Сʱ---m3/(60*60s)
        PHY_CUBIC_METRE_PER_24HOURS,         // ������λ-������/24Сʱ---m3/(24*60*60s)
        PHY_CUBIC_METRE_PER_24HOURS_CORRECT, // ������������λ-������/24Сʱ---m3/(24*60*60s)
        PHY_LITRE,                           // �ݻ���λ-��---L=10-3m3
        PHY_KILOGRAM,                        // ������λ-ǧ��---kg
        PHY_NEWTON,                          // ���ĵ�λ-ţ��---N
        PHY_NEWTON_METRE,                    // ������λ-ţ����---Nm J=Nm=Ws
        PHY_PASCAL,                          // ѹ����λ-��˹��---N/m2
        PHY_BAR,                             // ѹ����λ-��---10-5N/m2
        PHY_JOULE,                           // ������λ-����---J=Nm=Ws
        PHY_JOULE_PER_HOUR,                  // �ȹ���λ-��ÿСʱ---J/(60*60s)
        PHY_WATT,                            // �й����ʵ�λ-��---W=J/s
        PHY_KILOWATT,                        // �й����ʵ�λ-ǧ��---kW=J/(s*1000)
        PHY_VOLT_AMPERE,                     // ���ڹ��ʵ�λ-����---VA
        PHY_KILOVOLT_AMPERE,                 // ���ڹ��ʵ�λ-ǧ����---kVA
        PHY_VAR,                             // �޹����ʵ�λ-��---var
        PHY_KILOVAR,                         // �޹����ʵ�λ-ǧ��---kvar
        PHY_KILOWATT_HOUR,                   // �й����� rw���й����ܱ���������ֵ-ǧ��-ʱ---kWh=kW*(60*60s)
        PHY_KILOVOLT_AMPERE_HOUR,            // �������� rS�����ڵ��ܱ���������ֵ-ǧ��-��-Сʱ---kVAh=kVA*(60*60s)
        PHY_KILOVAR_HOUR,                    // �޹����� rB���޹����ܱ���������ֵ-ǧ��-ʱ---kvarh=kvar*(60*60s)
        PHY_AMPERE,                          // ������λ-����---A
        PHY_COULOMB,                         // ������λ-����---C=As
        PHY_VOLT,                            // ��ѹ��λ-����---V
        PHY_VOLT_PER_METRE,                  // �糡ǿ�ȵ�λ-��ÿ��---V/m
        PHY_FARAD,                           // ���ݵ�λ-����---F=C/V=As/V
        PHY_OHM,                             // ���赥λ-ŷķ---OHM=V/A
        PHY_OHM_METRE,                       // ����ϵ����λ---OHMm2/m=OHMm
        PHY_WEBER,                           // ��ͨ����λ-Τ��---Wb=Vs
        PHY_TESLA,                           // ��ͨ�ܶȵ�λ-��˹��---T=Wb/m2
        PHY_AMPERE_PER_METRE,                // �ų�ǿ�ȵ�λ-����ÿ��---A/m
        PHY_HENRY,                           // ��е�λ-����---H=Wb/A
        PHY_HERTZ,                           // Ƶ�ʵ�λ-����---Hz=1/s
        PHY_FRACTION_OF_WATT_HOUR,           // �й���������������ֵ��λ-��ʱ��֮һ---1/(Wh)
        PHY_FRACTION_OF_VAR_HOUR,            // �޹���������������ֵ��λ-��ʱ��֮һ---1/(varh)
        PHY_FRACTION_OF_VOLT_AMPERE_HOUR,    // ������������������ֵ��λ-����ʱ��֮һ---1/(VAh)
        PHY_PERCENT,                         // �ٷֱ� %
        PHY_BYTE,                            // �ֽ�
        PHY_dBm,                             // �ֱ����� dBm
        PHY_RMB_YUAN_PER_KILOVAR_HOUR,       // ��� Ԫ/kWh
        PHY_AMPERE_HOUR,                     // ��ʱ Ah
        PHY_MILLISECOND,                     // ���� ms

        PHY_OTHER_UNIT = 254, // ������λ
        PHY_MISS_UNIT = 255   // �޵�λ��ȱ��λ������
    } PHYSICAL_UNIT;

    // Scaler_Unit 6.3.3.14
    typedef struct
    {
        UINT8 ucConvertFactor;       // ��������
        PHYSICAL_UNIT ePhysicalUnit; // ����λ
    } SCALER_UNIT;

    // RSD 6.3.3.7
    typedef enum
    {
        RSD_NULL = 0,
        RSD_SELECTOR1,
        RSD_SELECTOR2,
        RSD_SELECTOR3,
        RSD_SELECTOR4,
        RSD_SELECTOR5,
        RSD_SELECTOR6,
        RSD_SELECTOR7,
        RSD_SELECTOR8,
        RSD_SELECTOR9,
        RSD_SELECTOR10
    } RSD;

    // CSD 6.3.3.9
    typedef struct
    {
        UINT8 ucIsROAD;
        OAD stOAD;
        UINT8 ucOADNum;
        OAD stAssociateOAD[MAX_ROAD_NUM_PER_ROAD];
    } CSD;

    //MS 6.3.3.10
    typedef enum
    {
        MS_NONE_METER = 0,            // �޵��ܱ�
        MS_ALL_USER_ADDRESS,          // ȫ���û���ַ
        MS_GROUP_USER_TYPE,           // һ���û�����
        MS_GROUP_USER_ADDRESS,        // һ���û���ַ
        MS_GROUP_CONFIG_NO,           // һ���������
        MS_GROUP_USER_TYPE_REGION,    // һ���û���������
        MS_GROUP_USER_ADDRESS_REGION, // һ���û���ַ����
        MS_GROUP_CONFIG_NO_REGION     // һ�������������
    } MS;

    //SID 6.3.3.16
    typedef struct
    {
        UINT32 udwID;
        UINT8 ucAdditionalSize;
        UINT8 ucAdditionalData[SID_ADDITIONAL_DATA_SIZE]; //�������� octet-string
    } SID;

    //SID_MAC 6.3.3.17
    typedef struct
    {
        SID stSID;
        MAC stMAC;
    } SID_MAC;

    //COMDCB 6.3.3.29
    typedef enum
    {
        BR_300bps = 0,
        BR_600bps,
        BR_1200bps,
        BR_2400bps,
        BR_4800bps,
        BR_7200bps,
        BR_9600bps,
        BR_19200bps,
        BR_38400bps,
        BR_57600bps,
        BR_115200bps,
        BR_Adaptive = 255
    } BAUD_RATE;

    typedef enum
    {
        CB_NULL = 0,
        CB_ODD,
        CB_EVEN
    } CHECK_BIT;

    typedef enum
    {
        DB_5BITS = 5,
        DB_6BITS,
        DB_7BITS,
        DB_8BITS
    } DATA_BIT;

    typedef enum
    {
        SB_1BIT = 1,
        SB_2BITS,
    } STOP_BIT;

    typedef enum
    {
        FC_NULL = 0,
        FC_HARDWARE,
        FC_SOFTWARE
    } FLOW_CONTROL;

    typedef struct
    {
        BAUD_RATE eBaudRate;       // ������
        CHECK_BIT eCheckBit;       // У��λ
        DATA_BIT eDataBit;         // ����λ
        STOP_BIT eStopBit;         // ֹͣλ
        FLOW_CONTROL eFlowControl; // ����
    } COMDCB_STRUCT;

    //RCSD 6.3.3.8
    typedef struct
    {
        UINT8 ucCSDNum;
        CSD stCSD[MAX_RCSD_CSD_NUM];
    } RCSD;

    // PIID 6.3.3.4
    typedef UINT8 PIID;

    // PIID-ACD 6.3.3.4
    typedef UINT8 PIID_ACD;

    //DAR 6.3.3.11
    typedef enum
    {
        //SUCCESS = 0,
        D_SUCCESS = 0, //stragen
        HW_INVALID,
        INVALID_T,
        REFUSE_RW,
        OBJ_NOT_DEF,
        CLASS_NOT_FIT,
        OBJ_NOT_EXIST,
        TYPE_NOT_MATCH,
        OUT_OF_RANGE,
        DTBLK_NOT_AVAILABLE,
        FRAMETRAS_CANCELL,
        FRAME_TRAS_NSTAT,
        BLKWR_CANCELL,
        BLKWR_STATE_NEXIST,
        DTBLKNUM_INVILID,
        PW_ERROR,
        BAUD_NOCHANG,
        YEAR_OOR,
        DAY_OOR,
        TARIFF_OOR,
        SECURITY_AUTH_NMATCH,
        RECHARGE_REPEAT,
        ESAM_ERROR,
        SECURITY_AUTH_FALSE,
        CUSTOMCODE_NMATCH,
        RECHARGE_TIMES_ERROR,
        HOARD_EXCESS,
        ADDR_ERROR,
        SYMMETRY_DEC_ERROR,
        NONSYMMETRY_DEC_ERROR,
        SIGNATURE_ERROR,
        METER_HANG,
        TIMELAB_INVALID,
        REQ_TIMEOUT,
        ESAM_P1P2_ERROR,
        ESAM_LC_ERROR,
        ELSE_ERROR = 255
    } DAR;
    typedef enum
    {
        DAR_SUCCEED = 0,              // �ɹ�
        DAR_HW_INVALID,               // Ӳ��ʧЧ
        DAR_TMP_INVALID,              // ��ʱʧЧ
        DAR_REFUSE_RW,                // �ܾ���д
        DAR_OBJ_NOT_DEF,              // ����δ����
        DAR_CLASS_NOT_FIT,            // ����ӿ��಻����
        DAR_OBJ_NOT_EXIST,            // ���󲻴���
        DAR_TYPE_NOT_MATCH,           // ���Ͳ�ƥ��
        DAR_OUT_OF_RANGE,             // Խ��
        DAR_DTBLK_NOT_AVAILABLE,      // ���ݿ鲻����
        DAR_SPLIT_FRAME_TRAS_CANCELL, // ��֡������ȡ��
        DAR_SPLIT_FRAME_TRAS_NSTAT,   // �����ڷ�֡����״̬
        DAR_BLKWR_CANCELL,            // ��дȡ��
        DAR_BLKWR_STATE_NEXIST,       // �����ڿ�д״̬
        DAR_DTBLKNUM_INVILID,         // ���ݿ������Ч
        DAR_PWD_ERROR,                // �����/δ��Ȩ
        DAR_BAUD_NOCHANG,             // ͨ�����ʲ��ܸ���
        DAR_YEAR_OOR,                 // ��ʱ������
        DAR_DAY_OOR,                  // ��ʱ������
        DAR_TARIFF_OOR,               // ��������
        DAR_SECURITY_AUTH_NMATCH,     // ��ȫ��֤��ƥ��
        DAR_RECHARGE_REPEAT,          // �ظ���ֵ
        DAR_ESAM_ERROR,               // ESAM��֤ʧ��
        DAR_SECURITY_AUTH_FALSE,      // ��ȫ��֤ʧ��
        DAR_CUSTOMERCODE_NMATCH,      // �ͻ����ʧ��
        DAR_RECHARGE_TIMES_ERROR,     // ��ֵ��������
        DAR_HOARD_EXCESS,             // ���糬�ڻ�
        DAR_ADDR_ERROR,               // ��ַ�쳣
        DAR_SYMMETRY_DEC_ERROR,       // �Գƽ��ܴ���
        DAR_NONSYMMETRY_DEC_ERROR,    // �ǶԳƽ��ܴ���
        DAR_SIGNATURE_ERROR,          // ǩ������
        DAR_METER_HANG,               // ���ܱ����
        DAR_TIMELAB_INVALID,          // ʱ���ǩ��Ч
        DAR_REQ_TIMEOUT,              // ����ʱ
        DAR_ESAM_P1P2_ERROR,          // ESAM��P1P2����
        DAR_ESAM_LC_ERROR,            // ESAM��LC����
        DAR_OTHER_ERROR = 255         // ����
    } DAR_NEW;

    /**
 * @note: Ӧ�ò�Э�����ݵ�Ԫ
 * @author: 
 * @date: 2017-4-26 22:37:13
 *---------------------------------------------------------------------------------------
 */

    typedef enum
    {
        LINK_Request = 1, // 1
        CONNECT_Request,  // 2
        RELEASE_Request,  // 3

        GET_Request = 5, // 5
        SET_Request,
        ACTION_Request,
        REPORT_Response,
        PROXY_Request,

        SECURITY_Request = 16,

        LINK_Response = 129,
        CONNECT_Response,
        RELEASE_Response,
        RELEASE_Notification,
        GET_Response,
        SET_Response,
        ACTION_Response,
        REPORT_Notification,
        PROXY_Response,

        SECURITY_Response = 144
    } APDU_ID;

    typedef enum
    {
        APDU_LOGIN = 0, // ��¼
        APDU_HEARTBEAT, // ����
        APDU_LOGOUT     // �˳���¼
    } APDU_LINK;

    typedef enum
    {
        GetRequestNormal = 1, // ��ȡһ��������������
        GetRequestNormalList, // ��ȡ���ɸ�������������
        GetRequestRecord,     // ��ȡһ����¼�Ͷ�����������
        GetRequestRecordList, // ��ȡ���ɸ���¼�Ͷ�����������
        GetRequestNext        // ��ȡ��֡��Ӧ����һ�����ݿ�����
    } APDU_GET_REQUEST;

    typedef enum
    {
        GetResponseNormal = 1, // ��Ӧһ��������������
        GetResponseNormalList, // ��Ӧ���ɸ�������������
        GetResponseRecord,     // ��Ӧһ����¼�Ͷ�����������
        GetResponseRecordList, // ��Ӧ���ɸ���¼�Ͷ�����������
        GetResponseNext        // ��Ӧ��֡��Ӧ����һ�����ݿ�����
    } APDU_GET_RESPONSE;

    typedef enum
    {
        SetRequestNormal = 1,       // ����һ��������������
        SetRequestNormalList,       // �������ɸ�������������
        SetThenGetRequestNormalList // ���ú��ȡ���ɸ�������������
    } APDU_SET;

    typedef enum
    {
        ActionRequest = 1,             // ����һ�����󷽷�����
        ActionRequestList,             // �������ɸ����󷽷�����
        ActionThenGetRequestNormalList // �������ɸ����󷽷����ȡ���ɸ�������������
    } APDU_ACTION;

    typedef enum
    {
        ReportNotificationList = 1,   // �ϱ����ɸ��������� (���ݹ�Լһ��ֻ���ϱ�һ����������)
        ReportNotificationRecordList, // �ϱ����ɸ���¼�Ͷ�������
        ReportNotificationTransData   // �ϱ�͸������
    } APDU_REPORT;

    typedef enum
    {
        ProxyGetRequestList = 1,       // �����ȡ���ɸ������������ɸ�������������
        ProxyGetRequestRecord,         // �����ȡһ����������һ����¼�Ͷ�����������
        ProxySetRequestList,           // �����������ɸ������������ɸ�������������
        ProxySetThenGetRequestList,    // �������ú��ȡ���ɸ������������ɸ�������������
        ProxyActionRequestList,        // ����������ɸ������������ɸ����󷽷�����
        ProxyActionThenGetRequestList, // ����������ȡ���ɸ������������ɸ����󷽷�����������
        ProxyTransCommandRequest       // ����͸��ת����������
    } APDU_PROXY;

    typedef struct
    {
        DATE_TIME_S stTransTime;
        TI stTransDelay;
    } TIME_TAG;

    //Get response: GET_RESULT
    typedef enum
    {
        ERROR_INFO = 0,
        RES_DATA
    } GET_RESULT;

    typedef struct
    {
        BOOL blIsSplited;
        BOOL blIsTheLastFrame;
        UINT8 ucCField;
        UINT16 uwFrameNum; //ȡֵ��Χ0~4095��ѭ��ʹ��,0xFFFF�������һ֡
    } ASSEMBLE_LINK_FRAME_PARA;

    /*ConnectResult*/
    typedef enum
    {
        ALLOW_APP_CONNECT = 0,
        PASSWORD_ERROR,
        SYMMETRIC_DECRYPTION_ERROR,
        RESYMMETRIC_DECRYPTION_ERROR,
        SIGN_ERROR,
        PROTOCOL_VER_ERROR,
        OTHER_ERROR = 255
    } CONNECT_RESULT;

    ///ConnectResponseInfo
    typedef struct
    {
        UINT8 ucSignInfoNum;
        UINT8 ucSignInfo[MAX_SIGNINFO_SIZE];
    } SIGNINFO;

    typedef struct
    {
        RN stServerRN;
        SIGNINFO stServerSignInfo;
    } SECURITYDATA;

    typedef struct
    {
        UINT8 ucRNSize;
        UINT8 ucRNData[100];
        UINT8 ucMACSize;
        UINT8 ucMACData[100];
    } RN_MAC;

    typedef struct
    {
        UINT16 OI;
        UINT16 SAFEMODE;
    } SAFE_MODE_PARA;

#if defined(ZHEJIANG_698J)
    typedef enum
    {
        AUTO_REPORTEVENT_TER = 0,
        AUTO_REPORTEVENT_METER
    } AUTO_REPORTEVENT_TYPE;

    typedef struct
    {
        UINT32 udwRecordNo;
        UINT16 uwConfigNo;
        AUTO_REPORTEVENT_TYPE eAutoPeportEveType;
    } STAUTO_REPORTEVENT_CONTENT;
#endif

/**
 * @note: Attribute and Attribute Index and Method define
 * @author:
 * @date: 2017-4-26 22:41:32
 *-----------------------------------------------------------------------------------------
 */
#define ATTRIBUTE_1 1
#define ATTRIBUTE_2 2
#define ATTRIBUTE_3 3
#define ATTRIBUTE_4 4
#define ATTRIBUTE_5 5
#define ATTRIBUTE_6 6
#define ATTRIBUTE_7 7
#define ATTRIBUTE_8 8
#define ATTRIBUTE_9 9
#define ATTRIBUTE_10 10
#define ATTRIBUTE_11 11
#define ATTRIBUTE_12 12
#define ATTRIBUTE_13 13
#define ATTRIBUTE_14 14
#define ATTRIBUTE_15 15
#define ATTRIBUTE_16 16
#define ATTRIBUTE_17 17
#define ATTRIBUTE_18 18
#define ATTRIBUTE_19 19
#define ATTRIBUTE_100 100

#define ATTR_INDEX_0 0
#define ATTR_INDEX_1 1
#define ATTR_INDEX_2 2
#define ATTR_INDEX_3 3
#define ATTR_INDEX_4 4
#define ATTR_INDEX_5 5
#define ATTR_INDEX_6 6
#define ATTR_INDEX_7 7
#define ATTR_INDEX_8 8
#define ATTR_INDEX_9 9
#define ATTR_INDEX_10 10
#define ATTR_INDEX_11 11
#define ATTR_INDEX_12 12
#define ATTR_INDEX_13 13
#define ATTR_INDEX_14 14

#define METHOD_1 1
#define METHOD_2 2
#define METHOD_3 3
#define METHOD_4 4
#define METHOD_5 5
#define METHOD_6 6
#define METHOD_7 7
#define METHOD_8 8
#define METHOD_127 127
#define METHOD_128 128
#define METHOD_129 129
#define METHOD_130 130
#define METHOD_131 131
#define METHOD_132 132
#define METHOD_133 133
#define METHOD_134 134
#ifdef SHANXI
#define METHOD_141 141
#define METHOD_142 142
#define METHOD_143 143
#endif
#define METHOD_171 171
/**
 * @note: OI define
 * @author: 
 * @date: 2017-4-26 22:41:32
 *-----------------------------------------------------------------------------------------
 */
// A.1 ������������ʶ
#define OI_CMB_ACT 0x0000                  // ����й�����
#define OI_FWACT 0x0010                    // �����й�����
#define OI_AFWACT 0x0011                   // A�������й�����
#define OI_BFWACT 0x0012                   // B�������й�����
#define OI_CFWACT 0x0013                   // C�������й�����
#define OI_RVERACT 0x0020                  // �����й�����
#define OI_ARVERACT 0x0021                 // A�෴���й�����
#define OI_BRVERACT 0x0022                 // B�෴���й�����
#define OI_CRVERACT 0x0023                 // C�෴���й�����
#define OI_CMB_REACT_1 0x0030              // ����޹�1����
#define OI_ACMB_REACT_1 0x0031             // A������޹�1����
#define OI_BCMB_REACT_1 0x0032             // B������޹�1����
#define OI_CCMB_REACT_1 0x0033             // C������޹�1����
#define OI_CMB_REACT_2 0x0040              // ����޹�2����
#define OI_ACMB_REACT_2 0x0041             // A������޹�2����
#define OI_BCMB_REACT_2 0x0042             // B������޹�2����
#define OI_CCMB_REACT_2 0x0043             // C������޹�2����
#define OI_FSTQUAD_REACT 0x0050            // ��һ�����޹�����
#define OI_AFSTQUAD_REACT 0x0051           // A���һ�����޹�����
#define OI_BFSTQUAD_REACT 0x0052           // B���һ�����޹�����
#define OI_CFSTQUAD_REACT 0x0053           // C���һ�����޹�����
#define OI_SNDQUAD_REACT 0x0060            // �ڶ������޹�����
#define OI_ASNDQUAD_REACT 0x0061           // A��ڶ������޹�����
#define OI_BSNDQUAD_REACT 0x0062           // B��ڶ������޹�����
#define OI_CSNDQUAD_REACT 0x0063           // C��ڶ������޹�����
#define OI_THDQUAD_REACT 0x0070            // ���������޹�����
#define OI_ATHDQUAD_REACT 0x0071           // A����������޹�����
#define OI_BTHDQUAD_REACT 0x0072           // B����������޹�����
#define OI_CTHDQUAD_REACT 0x0073           // C����������޹�����
#define OI_FUTHQUAD_REACT 0x0080           // ���������޹�����
#define OI_AFUTHQUAD_REACT 0x0081          // A����������޹�����
#define OI_BFUTHQUAD_REACT 0x0082          // B����������޹�����
#define OI_CFUTHQUAD_REACT 0x0083          // C����������޹�����
#define OI_FW_APARNT 0x0090                // �������ڵ���
#define OI_AFW_APARNT 0x0091               // A���������ڵ���
#define OI_BFW_APARNT 0x0092               // B���������ڵ���
#define OI_CFW_APARNT 0x0093               // C���������ڵ���
#define OI_RVER_APARNT 0x00A0              // �������ڵ���
#define OI_ARVER_APARNT 0x00A1             // A�෴�����ڵ���
#define OI_BRVER_APARNT 0x00A2             // B�෴�����ڵ���
#define OI_CRVER_APARNT 0x00A3             // C�෴�����ڵ���
#define OI_FWACT_FUDMTAL_WAVEALL 0x0110    // �����й������ܵ���
#define OI_AFWACT_FUDMTAL_WAVE 0x0111      // A�������й���������
#define OI_BFWACT_FUDMTAL_WAVE 0x0112      // B�������й���������
#define OI_CFWACT_FUDMTAL_WAVE 0x0113      // C�������й���������
#define OI_RVERACT_FUDMTAL_WAVEALL 0x0120  // �����й������ܵ���
#define OI_ARVERACT_FUDMTAL_WAVE 0x0121    // A�෴���й���������
#define OI_BRVERACT_FUDMTAL_WAVE 0x0122    // B�෴���й���������
#define OI_CRVERACT_FUDMTAL_WAVE 0x0123    // C�෴���й���������
#define OI_FWACT_HARMONIC_WAVEALL 0x0210   // �����й�г���ܵ���
#define OI_AFWACT_HARMONIC_WAVE 0x0211     // A�������й�г������
#define OI_BFWACT_HARMONIC_WAVE 0x0212     // B�������й�г������
#define OI_CFWACT_HARMONIC_WAVE 0x0213     // C�������й�г������
#define OI_RVERACT_HARMONIC_WAVEALL 0x0220 // �����й�г���ܵ���
#define OI_ARVERACT_HARMONIC_WAVE 0x0221   // A�෴���й�г������
#define OI_BRVERACT_HARMONIC_WAVE 0x0222   // B�෴���й�г������
#define OI_CRVERACT_HARMONIC_WAVE 0x0223   // C�෴���й�г������
#define OI_COPPERLOSS_ACT_COMPALL 0x0300   // ͭ���й��ܵ��ܲ�����
#define OI_ACOPPERLOSS_ACT_COMP 0x0301     // A��ͭ���й����ܲ�����
#define OI_BCOPPERLOSS_ACT_COMP 0x0302     // B��ͭ���й����ܲ�����
#define OI_CCOPPERLOSS_ACT_COMP 0x0303     // C��ͭ���й����ܲ�����
#define OI_IRONLOSS_ACT_COMPALL 0x0400     // �����й��ܵ��ܲ�����
#define OI_AIRONLOSS_ACT_COMP 0x0401       // A�������й����ܲ�����
#define OI_BIRONLOSS_ACT_COMP 0x0402       // B�������й����ܲ�����
#define OI_CIRONLOSS_ACT_COMP 0x0403       // C�������й����ܲ�����
#define OI_RELEVANT_ALL 0x0500             // �����ܵ���
#define OI_ARELEVANT 0x0501                // A���������
#define OI_BRELEVANT 0x0502                // B���������
#define OI_CRELEVANT 0x0503                // C���������
// A.2 �������������ʶ
#define OI_FWACT_MAXDEM 0x1010            // �����й��������
#define OI_AFWACT_MAXDEM 0x1011           // A�������й��������
#define OI_BFWACT_MAXDEM 0x1012           // B�������й��������
#define OI_CFWACT_MAXDEM 0x1013           // C�������й��������
#define OI_RVERACT_MAXDEM 0x1020          // �����й��������
#define OI_ARVERACT_MAXDEM 0x1021         // A�෴���й��������
#define OI_BRVERACT_MAXDEM 0x1022         // B�෴���й��������
#define OI_CRVERACT_MAXDEM 0x1023         // C�෴���й��������
#define OI_CMB_REACT1_MAXDEM 0x1030       // ����޹�1�������
#define OI_ACMB_REACT1_MAXDEM 0x1031      // A������޹�1�������
#define OI_BCMB_REACT1_MAXDEM 0x1032      // B������޹�1�������
#define OI_CCMB_REACT1_MAXDEM 0x1033      // C������޹�1�������
#define OI_CMB_REACT2_MAXDEM 0x1040       // ����޹�2�������
#define OI_ACMB_REACT2_MAXDEM 0x1041      // A������޹�2�������
#define OI_BCMB_REACT2_MAXDEM 0x1042      // B������޹�2�������
#define OI_CCMB_REACT2_MAXDEM 0x1043      // C������޹�2�������
#define OI_FSTQUAD_MAXDEM 0x1050          // ��һ�����������
#define OI_AFSTQUAD_MAXDEM 0x1051         // A���һ�����������
#define OI_BFSTQUAD_MAXDEM 0x1052         // B���һ�����������
#define OI_CFSTQUAD_MAXDEM 0x1053         // C���һ�����������
#define OI_SNDQUAD_MAXDEM 0x1060          // �ڶ������������
#define OI_ASNDQUAD_MAXDEM 0x1061         // A��ڶ������������
#define OI_BSNDQUAD_MAXDEM 0x1062         // B��ڶ������������
#define OI_CSNDQUAD_MAXDEM 0x1063         // C��ڶ������������
#define OI_THDQUAD_MAXDEM 0x1070          // ���������������
#define OI_ATHDQUAD_MAXDEM 0x1071         // A����������������
#define OI_BTHDQUAD_MAXDEM 0x1072         // B����������������
#define OI_CTHDQUAD_MAXDEM 0x1073         // C����������������
#define OI_FUTHQUAD_MAXDEM 0x1080         // ���������������
#define OI_AFUTHQUAD_MAXDEM 0x1081        // A����������������
#define OI_BFUTHQUAD_MAXDEM 0x1082        // B����������������
#define OI_CFUTHQUAD_MAXDEM 0x1083        // C����������������
#define OI_FW_APARNT_MAXDEM 0x1090        // ���������������
#define OI_AFW_APARNT_MAXDEM 0x1091       // A�����������������
#define OI_BFW_APARNT_MAXDEM 0x1092       // B�����������������
#define OI_CFW_APARNT_MAXDEM 0x1093       // C�����������������
#define OI_RVER_APARNT_MAXDEM 0x10A0      // ���������������
#define OI_ARVER_APARNT_MAXDEM 0x10A1     // A�෴�������������
#define OI_BRVER_APARNT_MAXDEM 0x10A2     // B�෴�������������
#define OI_CRVER_APARNT_MAXDEM 0x10A3     // C�෴�������������
#define OI_FRZ_FWACT_MAXDEM 0x1110        // ���������������й��������
#define OI_FRZ_AFWACT_MAXDEM 0x1111       // ����������A�������й��������
#define OI_FRZ_BFWACT_MAXDEM 0x1112       // ����������B�������й��������
#define OI_FRZ_CFWACT_MAXDEM 0x1113       // ����������C�������й��������
#define OI_FRZ_RVERACT_MAXDEM 0x1120      // ���������ڷ����й��������
#define OI_FRZ_ARVERACT_MAXDEM 0x1121     // ����������A�෴���й��������
#define OI_FRZ_BRVERACT_MAXDEM 0x1122     // ����������B�෴���й��������
#define OI_FRZ_CRVERACT_MAXDEM 0x1123     // ����������C�෴���й��������
#define OI_FRZ_CMB_REACT1_MAXDEM 0x1130   // ��������������޹�1�������
#define OI_FRZ_ACMB_REACT1_MAXDEM 0x1131  // ����������A������޹�1�������
#define OI_FRZ_BCMB_REACT1_MAXDEM 0x1132  // ����������B������޹�1�������
#define OI_FRZ_CCMB_REACT1_MAXDEM 0x1133  // ����������C������޹�1�������
#define OI_FRZ_CMB_REACT2_MAXDEM 0x1140   // ��������������޹�2�������
#define OI_FRZ_ACMB_REACT2_MAXDEM 0x1141  // ����������A������޹�2�������
#define OI_FRZ_BCMB_REACT2_MAXDEM 0x1142  // ����������B������޹�2�������
#define OI_FRZ_CCMB_REACT2_MAXDEM 0x1143  // ����������C������޹�2�������
#define OI_FRZ_FSTQUAD_MAXDEM 0x1150      // ���������ڵ�һ�����������
#define OI_FRZ_AFSTQUAD_MAXDEM 0x1151     // ����������A���һ�����������
#define OI_FRZ_BFSTQUAD_MAXDEM 0x1152     // ����������B���һ�����������
#define OI_FRZ_CFSTQUAD_MAXDEM 0x1153     // ����������C���һ�����������
#define OI_FRZ_SNDQUAD_MAXDEM 0x1160      // ���������ڵڶ������������
#define OI_FRZ_ASNDQUAD_MAXDEM 0x1161     // ����������A��ڶ������������
#define OI_FRZ_BSNDQUAD_MAXDEM 0x1162     // ����������B��ڶ������������
#define OI_FRZ_CSNDQUAD_MAXDEM 0x1163     // ����������C��ڶ������������
#define OI_FRZ_THDQUAD_MAXDEM 0x1170      // ���������ڵ��������������
#define OI_FRZ_ATHDQUAD_MAXDEM 0x1171     // ����������A����������������
#define OI_FRZ_BTHDQUAD_MAXDEM 0x1172     // ����������B����������������
#define OI_FRZ_CTHDQUAD_MAXDEM 0x1173     // ����������C����������������
#define OI_FRZ_FUTHQUAD_MAXDEM 0x1180     // ���������ڵ��������������
#define OI_FRZ_AFUTHQUAD_MAXDEM 0x1181    // ����������A����������������
#define OI_FRZ_BFUTHQUAD_MAXDEM 0x1182    // ����������B����������������
#define OI_FRZ_CFUTHQUAD_MAXDEM 0x1183    // ����������C����������������
#define OI_FRZ_FW_APARNT_MAXDEM 0x1190    // �������������������������
#define OI_FRZ_AFW_APARNT_MAXDEM 0x1191   // ����������A�����������������
#define OI_FRZ_BFW_APARNT_MAXDEM 0x1192   // ����������B�����������������
#define OI_FRZ_CFW_APARNT_MAXDEM 0x1193   // ����������C�����������������
#define OI_FRZ_RVER_APARNT_MAXDEM 0x11A0  // ���������ڷ��������������
#define OI_FRZ_ARVER_APARNT_MAXDEM 0x11A1 // ����������A�෴�������������
#define OI_FRZ_BRVER_APARNT_MAXDEM 0x11A2 // ����������B�෴�������������
#define OI_FRZ_CRVER_APARNT_MAXDEM 0x11A3 // ����������C�෴�������������

#define OI_VOLTAGE 0x2000       // ��ѹ
#define OI_CURRENT 0x2001       // ����
#define OI_VOLPhANGLE 0x2002    // ��ѹ���
#define OI_CURRPhANGLE 0x2003   // ��ѹ�������
#define OI_ACTIVEPOWER 0x2004   // �й�����
#define OI_REACTIVEPOWER 0x2005 // �޹�����
#define OI_APPARENTPOWER 0x2006 // ���ڹ���

#define OI_1MIN_AVERAGEACTPWR 0x2007        // һ����ƽ���й�����
#define OI_1MIN_AVERAGEREACTPWR 0x2008      // һ����ƽ���޹�����
#define OI_1MIN_AVERAGEAPARNTPWR 0x2009     // һ����ƽ�����ڹ���
#define OI_POWERFACTOR 0x200A               // ��������
#define OI_VOLTAGEWAVEDISTORTION 0x200B     // ��ѹ����ʧ���
#define OI_CURRENTWAVEDISTORTION 0x200C     // ��������ʧ���
#define OI_VOLTAGE_HARMONICCONTENT 0x200D   // ��ѹг��������
#define OI_PWR_GRID_FREQUENCY 0x200F        // ����Ƶ��
#define OI_METER_INSIDE_TEMPER 0x2010       // �����¶�
#define OI_CLOCK_BAT_VOLT 0x2011            // ʱ�ӵ�ص�ѹ
#define OI_PWR_OFF_READING_BATVOLT 0x2012   // ͣ�糭���ص�ѹ
#define OI_CLOCK_BAT_WORKING_TIME 0x2013    // ʱ�ӵ�ع���ʱ��
#define OI_METER_RUN_SWORD 0x2014           // ���ܱ�����״̬��
#define OI_METER_FOLLOW_REPORT_SWORD 0x2015 // ���ܱ�����ϱ�״̬��
#define OI_PRE_ACT_DEM 0x2017               // ��ǰ�й�����
#define OI_PRE_REACT_DEM 0x2018             // ��ǰ�޹�����
#define OI_PRE_APARNT_DEM 0x2019            // ��ǰ��������
#define OI_PRE_ELECTRPRICE 0x201A           // ��ǰ���
#define OI_PRE_TARIFF_ELECPRICE 0x201B      // ��ǰ���ʵ��
#define OI_PRE_STEP_ELECPRICE 0x201C        // ��ǰ���ݵ��

#define OI_OCCURRENCE_TIME 0x201E // �¼�����ʱ��

#define OI_END_TIME 0x2020                      // �¼�����ʱ��
#define OI_DATA_FRZ_TIME 0x2021                 // ���ݶ���ʱ��
#define OI_EVENT_RCRD_NUM 0x2022                // ʱ���¼���
#define OI_FRZ_RCRD_NUM 0x2023                  // �����¼���
#define OI_EVENT_SOURCE 0x2024                  // �¼�����Դ
#define OI_EVENT_PRE_VALUE 0x2025               // �¼���ǰֵ
#define OI_VOLT_UNBALANCE_RATE 0x2026           // ��ѹ��ƽ����
#define OI_CURRENT_UNBALANCE_RATE 0x2027        // ������ƽ����
#define OI_LOAD_RATE 0x2028                     // ������
#define OI_AH_VALUE 0x2029                      // ��ʱֵ
#define OI_TARGET_SERVER_ADDR 0x202A            // Ŀ���������ַ
#define OI_PRE_WALLET_FILE 0x202C               // ��ǰǮ���ļ�
#define OI_PRE_OVERDRAFT_AMOUNT 0x202D          // ��ǰ͸֧���
#define OI_CUMULATIVE_AMOUNT 0x202E             // �ۼƹ�����
#define OI_MONTHLY_ELECTR 0x2031                // �¶��õ���
#define OI_STEP_STLMNT_ELECTR 0x2032            // ���ݽ����õ���
#define OI_CTRL_CMD_EXE_SWORD 0x2040            // ��������ִ��״̬��
#define OI_CTRL_CMD_ERROR_SWORD 0x2041          // �����������״̬��
#define OI_MINUTEREGION_STATISTICS 0x2100       // ��������ͳ��
#define OI_HOURREGION_STATISTICS 0x2101         // Сʱ����ͳ��
#define OI_DAYREGION_STATISTICS 0x2102          // ������ͳ��
#define OI_MONTHREGION_STATISTICS 0x2103        // ������ͳ��
#define OI_YEARREGION_STATISTICS 0x2104         // ������ͳ��
#define OI_MINUTE_AVERAGE 0x2110                // ��������ƽ��
#define OI_HOUR_AVERAGE 0x2111                  // Сʱƽ��
#define OI_DAY_AVERAGE 0x2112                   // ��ƽ��
#define OI_MONTH_AVERAGE 0x2113                 // ��ƽ��
#define OI_YEAR_AVERAGE 0x2114                  // ��ƽ��
#define OI_MINUTE_EXTREMUM 0x2120               // ���Ӽ�ֵ
#define OI_HOUR_EXTREMUM 0x2121                 // Сʱ��ֵ
#define OI_DAY_EXTREMUM 0x2122                  // �ռ�ֵ
#define OI_MONTH_EXTREMUM 0x2123                // �¼�ֵ
#define OI_YEAR_EXTREMUM 0x2124                 // �꼫ֵ
#define OI_PHASEA_VOL_QUALIFICATION_RATE 0x2131 //A���ѹ�ϸ���
#define OI_PHASEB_VOL_QUALIFICATION_RATE 0x2132 //B���ѹ�ϸ���
#define OI_PHASEC_VOL_QUALIFICATION_RATE 0x2133 //C���ѹ�ϸ���
#define OI_DAY_MAXACTPWR_TIME 0x2140            // ������й����ʼ�����ʱ��
#define OI_MONTH_MAXACTPWR_TIME 0x2141          // ������й����ʼ�����ʱ��

#define OI_COMMFLOW 0x2200
#define OI_RUNTIME 0x2203
#define OI_RESETTM 0x2204
#define OI_ESAM 0xF100
#define OI_ESAMSAFEMODEPARA 0xF101
#define OI_TG1 0x2301
#define OI_TG2 0x2302
#define OI_TG3 0x2303
#define OI_TG4 0x2304
#define OI_TG5 0x2305
#define OI_TG6 0x2306
#define OI_TG7 0x2307
#define OI_TG8 0x2308
#define OI_REMOTE_CONTROL 0x8000
#define OI_ENSURE_POWER_SERVING 0x8001
#define OI_DUN_ALARM 0x8002
#define OI_SAFETY_VALUE 0x8100
#define OI_CONTROL_PERIOD 0x8101
#define OI_PCONTROL_ALARM_TIME 0x8102
#define OI_PMETER1 0x2401
#define OI_PMETER2 0x2402
#define OI_PMETER3 0x2403
#define OI_PMETER4 0x2404
#define OI_PMETER5 0x2405
#define OI_PMETER6 0x2406
#define OI_PMETER7 0x2407
#define OI_PMETER8 0x2408

//	�ı��һ
#ifdef FOUR_METER
#define OI_FOUR_TOTAL_WATER 0x2500       // �ۼ�ˮ����
#define OI_FOUR_TOTAL_GAS 0x2501         // �ۼ�������
#define OI_FOUR_TOTAL_HEAT 0x2502        // �ۼ�����
#define OI_FOUR_THERMAL_POWER 0x2503     // �ȹ���
#define OI_FOUR_TOTAL_WORK_TIME 0x2504   // �ۼƹ���ʱ��
#define OI_FOUR_WATER_TEMPERATURE 0x2505 // ˮ��
#define OI_FOUR_INSTRUMENT_STATUS 0x2506 // �Ǳ�״̬
#endif

//  A.4 �¼�������ʶ
#define OI_METER_LOSS_VOLT 0x3000             // ���ܱ�ʧѹ�¼�
#define OI_METER_UNDER_VOLT 0x3001            // ���ܱ�Ƿѹ�¼�
#define OI_METER_OVER_VOLT 0x3002             // ���ܱ��ѹ�¼�
#define OI_METER_PHASE_BREAKING 0x3003        // ���ܱ�����¼�
#define OI_METER_LOSS_CURRENT 0x3004          // ���ܱ�ʧ���¼�
#define OI_METER_OVER_CURRENT 0x3005          // ���ܱ�����¼�
#define OI_METER_CURRENT_BREAK 0x3006         // ���ܱ�����¼�
#define OI_METER_PWR_RVER 0x3007              // ���ܱ��ʷ����¼�
#define OI_METER_OVERLOAD 0x3008              // ���ܱ�����¼�
#define OI_METER_FWACT_DEM_BEYOND 0x3009      // ���ܱ������й����������¼�
#define OI_METER_RVERACT_DEM_BEYOND 0x300A    // ���ܱ����й����������¼�
#define OI_METER_REACT_DEM_BEYOND 0x300B      // ���ܱ��޹����������¼�
#define OI_METER_PWRFACTOR_LOWERLIMIT 0x300C  // ���ܱ��������������¼�
#define OI_METER_ALL_LOSS_VOLTAGE 0x300D      // ���ܱ�ȫʧѹ�¼�
#define OI_METER_AUXILIARY_PWR_DOWN 0x300E    // ���ܱ�����Դ�����¼�
#define OI_METER_VOLTAGE_RVER_PH 0x300F       // ���ܱ��ѹ�������¼�
#define OI_METER_CURRENT_RVER_PH 0x3010       // ���ܱ�����������¼�
#define OI_METER_PWR_DOWN 0x3011              // ���ܱ�����¼�
#define OI_METER_PROG 0x3012                  // ���ܱ����¼�
#define OI_METER_RESET 0x3013                 // ���ܱ������¼�
#define OI_METER_DEM_RESET 0x3014             // ���ܱ����������¼�
#define OI_METER_EVENT_RESET 0x3015           // ���ܱ��¼������¼�
#define OI_METER_CALI_TIME 0x3016             // ���ܱ�Уʱ�¼�
#define OI_METER_TI_TAB_PROG 0x3017           // ���ܱ�ʱ�α����¼�
#define OI_METER_TZ_TAB_PROG 0x3018           // ���ܱ�ʱ�������¼�
#define OI_METER_WEEKEND_PROG 0x3019          // ���ܱ������ձ���¼�
#define OI_METER_STLMNT_DATE_PROG 0x301A      // ���ܱ�����ձ���¼�
#define OI_METER_UNCAP 0x301B                 // ���ܱ����¼�
#define OI_METER_OPEN_BUTTON_BOX 0x301C       // ���ܱ���ť���¼�
#define OI_METER_VOLT_UNBALANCE 0x301D        // ���ܱ��ѹ��ƽ���¼�
#define OI_METER_CURRENT_UNBALANCE 0x301E     // ���ܱ������ƽ���¼�
#define OI_METER_TRIPPING_OPERATION 0x301F    // ���ܱ���բ�¼�
#define OI_METER_SWITCH_ON 0x3020             // ���ܱ��բ�¼�
#define OI_METER_HOLIDAY_PROG 0x3021          // ���ܱ�ڼ��ձ��
#define OI_METER_ACT_CMB_PROG 0x3022          // ���ܱ��й���Ϸ�ʽ����¼�
#define OI_METER_REACT_CMB_PROG 0x3023        // ���ܱ��޹���Ϸ�ʽ����¼�
#define OI_METER_TARIFFPARA_PROG 0x3024       // ���ܱ���ʲ��������¼�
#define OI_METER_STEP_TAB_PROG 0x3025         // ���ܱ���ݱ����¼�
#define OI_METER_SECRET_KEY_UPDATE 0x3026     // ���ܱ���Կ�����¼�
#define OI_METER_ABNORMAL_CARD 0x3027         // ���ܱ��쳣�忨�¼�
#define OI_METER_PURCHASE_ELECTR_RCRD 0x3028  // ���ܱ����¼
#define OI_METER_REFUND_RCRD 0x3029           // ���ܱ��˷Ѽ�¼
#define OI_METER_MAGNETIC_INTERFERENCE 0x302A // ���ܱ�㶨�ų������¼�
#define OI_METER_LOADSWITCH_MISOPERATE 0x302B // ���ܱ��ɿ��������¼�
#define OI_METER_PWR_ABNORMAL 0x302C          // ���ܱ��Դ�쳣�¼�
#define OI_METER_CURRENT_SERUNBALANCE 0x302D  // ���ܱ�������ز�ƽ���¼�
#define OI_METER_CLOCK_FAULT 0x302E           // ���ܱ�ʱ�ӹ����¼�
#define OI_METER_METERING_CHIP_FAILURE 0x302F // ���ܱ����оƬ�����¼�
#define OI_COMM_MODULE_CHANGE 0x3030          // ͨ��ģ�����¼�

#define OI_TER_INITIALIZATION 0x3100            // �ն˳�ʼ���¼�
#define OI_TER_VERSION_CHANGE 0x3101            // �ն˰汾����¼�
#define OI_TER_STATUS_CHANGE 0x3104             // �ն�״̬����λ�¼�
#define OI_CLOCK_BEYOND 0x3105                  // ���ܱ�ʱ�ӳ����¼�
#define OI_TERMINAL_OUTAGE 0x3106               // �ն�ͣ/�ϵ��¼�
#define OI_DC_UPPER_LIMIT 0x3107                // �ն�ֱ��ģ����Խ�����¼�
#define OI_DC_LOWER_LIMIT 0x3108                // �ն�ֱ��ģ����Խ�����¼�
#define OI_TER_MSG_CERTIFY 0x3109               // �ն���Ϣ��֤�����¼�
#define OI_DEVICE_FAULT 0x310A                  // �豸���ϼ�¼
#define OI_METER_DATA_FALL 0x310B               // ���ܱ�ʾ���½��¼�
#define OI_ENERGY_BEYOND 0x310C                 // �����������¼�
#define OI_METER_FLY 0x310D                     // ���ܱ�����¼�
#define OI_METER_STOP 0x310E                    // ���ܱ�ͣ���¼�
#define OI_COLLECT_FAIL 0x310F                  // �ն˳���ʧ���¼�
#define OI_COMM_FLOW_BEYOND 0x3110              // ��ͨ�����������¼�
#define OI_FIND_UNKNOWN_METER 0x3111            // ����δ֪���ܱ��¼�
#define OI_CROSS_REGION_METER 0x3112            // ��̨�����ܱ��¼�
#define OI_SET_CLOCK 0x3114                     // �ն˶�ʱ�¼�
#define OI_YK 0x3115                            // ң����բ��¼
#define OI_ACT_EXCEED_LIMIT_RCRD 0x3116         // �й��ܵ��ܲԽ���¼���¼
#define OI_OUTLOOP_STAT_CHANGE 0x3117           // �����·����״̬��λ�¼���¼
#define OI_TER_PROG_RCRD 0x3118                 // �ն˱�̼�¼
#define OI_TER_CURRENTLOOP_ABNORMAL 0x3119      // �ն˵�����·�쳣�¼�
#define OI_METER_NET_STATE_SWITCH 0x311A        // ���ܱ�����״̬�л��¼�
#define OI_TER_METER_CALI_RCRD 0x311B           // �ն˶Ե��Уʱ��¼
#define OI_METER_DATA_CHANGE 0x311C             // ���ܱ����ݱ����ؼ�¼
#define OI_PWR_CTRL_TRIP_RCRD 0x3200            // ������բ��¼
#define OI_ELECCTRL_TRIP_RCRD 0x3201            // �����բ��¼
#define OI_PURCHASE_ELEC_PARA_SET_RCRD 0x3202   // ����������ü�¼
#define OI_ELEC_CTRL_ALARM_RCRD 0x3203          // ��ظ澯�¼���¼
#define OI_CHANNELREPORTINGSTATUS 0x3300        // ͨ���ϱ�״̬
#define OI_STANDARD_RCRD_UNIT 0x3301            // ��׼�¼���¼��Ԫ
#define OI_PROG_RCRD_UNIT 0x3302                // ��̼�¼�¼���Ԫ
#define OI_FIND_UNKNOWNMETER_UNIT 0x3303        // ����δ֪���ܱ��¼���Ԫ
#define OI_CROSS_REGIONMETER_UNIT 0x3304        // ��̨�����ܱ��¼���Ԫ
#define OI_PWRCTRL_TRIP_RCRDUNIT 0x3305         // ������բ��¼��Ԫ
#define OI_ELECTRONICCTRL_TRIPRCRDUNIT 0x3306   // �����բ��¼��Ԫ
#define OI_ELECTRONICCTRL_ALARMRCRDUNIT 0x3307  // ��ظ澯�¼���Ԫ
#define OI_METER_DEM_EXCEEDLIMITUNIT 0x3208     // ���ܱ����������¼���Ԫ
#define OI_PWR_ON_OFF_RCRDUNIT 0x3209           // ͣ/�ϵ��¼���¼��Ԫ
#define OI_REMOTE_RCRDUNIT 0x330A               // ң���¼���¼��Ԫ
#define OI_ACTALLDIFFEXCEED_RCRDUNIT 0x330B     // �й��ܵ������Խ���¼���¼��Ԫ
#define OI_EVENT_CLEAR_EVENT_RCRDUNIT 0x330C    // �¼������¼���¼��Ԫ
#define OI_TER_METER_CALI_UNIT 0x330D           // �ն˶Ե��Уʱ��¼��Ԫ
#define OI_METER_NET_STATESWITCH_UNIT 0x330E    // ���ܱ�����״̬�л��¼���Ԫ
#define OI_METER_DATACHANGEMONI_RCRDUNIT 0x330F // ���ܱ����ݱ����ؼ�¼��Ԫ
#define OI_METER_ABNORMALCARDRCRD_UNIT 0x3310   // ���ܱ��쳣�忨��¼��Ԫ
#define OI_METER_REFUNDRCRD_UNIT 0x3311         // �˷��¼���¼��Ԫ
#define OI_COMM_MODULECHANGE_UNIT 0x3312        // ͨ��ģ�����¼���Ԫ
#define OI_METER_CLOCKBEYONDRCRD_UNIT 0x3313    // ���ܱ�ʱ�ӳ����¼���¼��Ԫ
#define OI_EVENT_LIST_OBJ 0x3320                // �¼��б����

#define OI_DATETIME 0x4000                     // ����ʱ��
#define OI_COMMADDR 0x4001                     // ͨ�ŵ�ַ
#define OI_METER_NO 0X4002                     // ���
#define OI_CUSTOMER_NO 0X4003                  // �ͻ����
#define OI_POSITION 0x4004                     // �豸����λ��
#define OI_GROUPADDR 0x4005                    // ���ַ
#define OI_CLKSOURCE 0x4006                    // ʱ��Դ
#define OI_LCDPARA 0x4007                      // LCD����
#define OI_TWO_TZ_TAB_SWITCHTIME 0x4008        // ����ʱ�����л�ʱ��
#define OI_TWOSETS_DAYTI_SWITCHTIME 0x4009     // ������ʱ���л�ʱ��
#define OI_TWOSETS_TITARIFF_SWITCHTIME 0x400A  // ���׷�ʱ�����л�ʱ��
#define OI_TWOSETS_STEPPRICE_SWITCHTIME 0x400B // ���׽��ݵ���л��¼�
#define OI_TZ_TI_NUM 0x400C                    // ʱ��ʱ����
#define OI_STEP_NUM 0x400D                     // ������
#define OI_HARMONIC_ANALYSIS_TIMES 0x400E      // г����������
#define OI_SECRET_KEY_ALL_NUM 0x400F           // ��Կ������
#define OI_METERING_NUM 0x4010                 // ����Ԫ����
#define OI_PUBLIC_HOLIDAY_TAB 0x4011           // �������ձ�
#define OI_WEEKEND_FWORD 0x4012                // ������������
#define OI_WEEKEND_ADOPTED_DAYSCHED 0x4013     // �����ղ��õ���ʱ�α�
#define OI_PRESET_TZ_TAB 0x4014                // ��ǰ��ʱ����
#define OI_BACKUPSET_TZ_TAB 0x4015             // ������ʱ����
#define OI_PRESENT_DAYTI_TAB 0x4016            // ��ǰ����ʱ�α�
#define OI_BACKUPSET_DAILY_TI_TAB 0x4017       // ��������ʱ�α�
#define OI_PRESET_TARIFF_PRICE 0x4018          // ��ǰ�׷��ʵ��
#define OI_BACKUPSET_TARIFF_PRICE 0x4019       // �����׷��ʵ��
#define OI_PRESET_STEP_PRICE 0x401A            // ��ǰ�׽��ݵ��
#define OI_BACKUPSET_STEP_PRICE 0x401B         // �����׽��ݵ��
#define OI_CURRENT_TRANS_RATIO 0x401C          // �������������
#define OI_VOLTAGE_TRANS_RATIO 0x401D          // ��ѹ���������
#define OI_ALARM_AMOUNT_LIMITVALUE 0x401E      // ���������ֵ
#define OI_OTHER_AMOUNT_LIMITVALUE 0x401F      // ���������ֵ
#define OI_ALARM_ELEC_LIMITVALUE 0x4020        // ����������ֵ
#define OI_OTHER_ELEC_LIMITVALUE 0x4021        // ����������ֵ
#define OI_CARD_STATUS_WORD 0x4022             // �忨״̬��
#define OI_TERMINAL_ELIMINATE 0x4024           // �޳�״̬��
#define OI_COLL_UPG_RESULT_TAB 0x4025          // �ɼ���Զ�����������
#define OI_COLL_UPG_RESULT 0x4026              // �ɼ����������
#define OI_VOLTAGE_QUALIFIED_RATE 0x4030       // ��ѹ�ϸ��ʲ���
#define OI_MAX_DEMAND_PERIOD 0x4100            // �����������
#define OI_SLIDER_TIME 0x4101                  // ����ʱ��
#define OI_CALI_METER_PULSE_WIDTH 0x4102       // У��������
#define OI_ASSET_MANAGEMENT_CODE 0x4103        // �ʲ��������
#define OI_RATED_VOLTAGE 0x4104                // ���ѹ
#define OI_RATED_CURRENT 0x4105                // �����
#define OI_MAX_CURRENT 0x4106                  // ������
#define OI_ACTV_ACCURACY_GRADE 0x4107          // �й�׼ȷ�ȵȼ�
#define OI_REACTV_ACCURACY_GRADE 0x4108        // �޹�׼ȷ�ȵȼ�
#define OI_METER_ACTV_CONSTANT 0x4109          // ���ܱ��й�����
#define OI_METER_REACTV_CONSTANT 0x410A        // ���ܱ��޹�����
#define OI_METER_MODEL 0x410B                  // ���ܱ��ͺ�
#define OI_ABC_PHAS_CONDCT_COEFF 0x410C        // ABC����絼ϵ��
#define OI_ABC_PHAS_REACT_COEFF 0x410D         // ABC����翹ϵ��
#define OI_ABC_PHAS_RESIST_COEFF 0x410E        // ABC�������ϵ��
#define OI_ABC_PHAS_SUSCEPT_COEFF 0x410F       // ABC�������ϵ��
#define OI_METER_RUNNING_FWORD1 0x4110         // ���ܱ���������״̬��1
#define OI_SOFTWARE_RCRD_NUM 0x4111            // ���������
#define OI_ACT_CMB_MODE_FWORD 0x4112           // �й���Ϸ�ʽ������
#define OI_REACT_CMB1_FWORD 0x4113             // �޹���Ϸ�ʽ1������
#define OI_REACT_CMB2_FWORD 0x4114             // �޹���Ϸ�ʽ2������
#define OI_STLMNT_DATE 0x4116                  // ������
#define OI_TI_DEM_FRZ_PERI 0x4117              // �ڼ�������������
#define OI_CASCADE_COMM_PARAM 0x4202           // ����ͨ�Ų���
#define OI_BROADCAST_TIME 0x4204               // �ն˹㲥Уʱ
#define OI_DEVICE 0x4300                       // �����豸
#define OI_APP_CONNECTION 0x4400               // Ӧ������
#define OI_APP_AUTHENTCTN_PWD 0x4401           // Ӧ��������֤����
#define OI_WIRELESS_NETWORK_COMM1 0x4500       // ����ͨ��ģ��1
#define OI_WIRELESS_NETWORK_COMM2 0x4501       // ����ͨ��ģ��2
#define OI_ETHERNET_COMM1 0x4510               // ��̫��ͨ��ģ��1
#define OI_ETHERNET_COMM2 0x4511               // ��̫��ͨ��ģ��2

//  A.6 ����������ʶ
#define OI_INSTANTFRZ 0x5000            // ˲ʱ����
#define OI_SECFRZ 0x5001                // �붳��
#define OI_MINUTEFRZ 0x5002             // ���Ӷ���
#define OI_HOURFRZ 0x5003               // Сʱ����
#define OI_DAY_FRZ 0x5004               // �ն���
#define OI_STLMNTFRZ 0x5005             // �����ն���
#define OI_MONTHFRZ 0x5006              // �¶���
#define OI_YEARFRZ 0x5007               // �궳��
#define OI_TZ_TAB_SWITCHFRZ 0x5008      // ʱ�����л�����
#define OI_DAYTI_TAB_SWITCHFRZ 0x5009   // ��ʱ�α��л��ն���
#define OI_TARIFFPRICE_SWITCHFRZ 0x500A // ���ʵ���л�����
#define OI_STEP_SWITCH_FRZ 0x500B       // �����л�����
#define OI_STEP_STLMNT_FRZ 0x5011       // ���ݽ��㶳��

//  A.7 �ɼ����������ʶ
#define OI_COLL_DOC_CONFIG_TAB 0x6000    // �ɼ��������ñ�
#define OI_COLLECTDOC_CONFIG_UNIT 0x6001 // �ɼ��������õ�Ԫ
#define OI_SEARCH_TAB 0x6002             // �ѱ�
#define OI_SEARCH_TAB_RESULT 0x6003      // һ���ѱ���
#define OI_CROSS_PLATFORM_RESULT 0x6004  // һ����̨�����
#ifdef SHANXI
#define OI_MICRO_POWER_COMMUNICATION 0x6005 // ΢��������ͨ��
#endif
#define OI_TASK_CONFIG_TABLE 0x6012            // �������ñ�
#define OI_TASK_CONFIG_UNIT 0x6013             // �������õ�Ԫ
#define OI_GENERAL_COLL_SCHEME_SET 0x6014      // ��ͨ�ɼ�������
#define OI_GENERALCOLL_SCHEME 0x6015           // ��ͨ�ɼ�����
#define OI_EVENT_COLL_SCHEME_SET 0x6016        // �¼��ɼ�������
#define OI_EVENTCOLL_SCHEME 0x6017             // �¼��ɼ�����
#define OI_TRANSPARENT_SCHEME_SET 0x6018       // ͸��������
#define OI_TRANSPARENT_SCHEME 0x6019           // ͸������
#define OI_TRANSPARENT_SCHEME_RESULTSET 0x601A // ͸�����������
#define OI_TRANSPARENT_SCHEME_RESULT 0x601B    // һ��͸���������
#define OI_REPORT_SCHEME_SET 0x601C            // �ϱ�������
#define OI_REPORT_SCHEME 0x601D                // �ϱ�����
#define OI_COLLECT_RULE_LIB 0x601E             // �ɼ������
#define OI_COLLECT_RULE 0x601F                 // �ɼ�����
#define OI_COLLECT_STATESET 0x6032             // �ɼ�״̬��
#define OI_COLLECT_STATE 0x6033                // �ɼ�״̬
#define OI_COLLECT_TASK_MONITORSET 0x6034      // �ɼ������ؼ�
#define OI_COLLECT_TASK_MONITOR_UNIT 0x6035    // �ɼ������ص�Ԫ
#define OI_COLLECT_STARTUP_TIMESTAMP 0x6040    // �ɼ�����ʱ��
#define OI_COLLECT_SUCCEED_TIMESTAMP 0x6041    // �ɼ��ɹ�ʱ��
#define OI_COLLECT_STORAGE_TIMESTAMP 0x6042    // �ɼ��洢ʱ��
//  A.9 ����������ʶ
#define OI_REMOTE_CONTROL 0x8000               // ң��
#define OI_ENSURE_POWER_SERVING 0x8001         // ����
#define OI_DUN_ALARM 0x8002                    // �߷Ѹ澯
#define OI_GENERAL_CHI_INFO 0x8003             // һ��������Ϣ
#define OI_IMPORTANT_CHI_INFO 0x8004           // ��Ҫ������Ϣ
#define OI_SAFETY_VALUE 0x8100                 // �ն˱�����ֵ
#define OI_CONTROL_PERIOD 0x8101               // �ն˹���ʱ��
#define OI_PCONTROL_ALARM_TIME 0x8102          // ���ظ澯ʱ��
#define OI_TI_PWR_CTRL 0x8103                  // ʱ�ι���
#define OI_FACTORY_REST_CTRL 0x8104            // ���ݿ���
#define OI_BUSINESS_STOP_CTRL 0x8105           // Ӫҵ��ͣ����
#define OI_PRE_PWR_FLOAT_DW_CTRL 0x8106        // ��ǰ�����¸���
#define OI_BUY_ELEC_CTRL 0x8107                // �����
#define OI_MONTH_ELEC_CTRL 0x8108              // �µ��
#define OI_TI_PWRCTRL_CONFIG_UNIT 0x8109       // ʱ�ι������õ�Ԫ
#define OI_FACTORY_RESTCTRL_CONFIG_UNIT 0x810A // ���ݿ����õ�Ԫ
#define OI_BUSINESSSTOPCTRL_CONFIG_UNIT 0x810B // Ӫҵ��ͣ�����õ�Ԫ
#define OI_BUY_ELECCTRL_CONFIG_UNIT 0x810C     // ��������õ�Ԫ
#define OI_MONTH_ELECCTRL_CONFIG_UNIT 0x810D   // �µ�����õ�Ԫ
#define OI_CTRL_OBJ 0x810E                     // ���ƶ���
#define OI_TRIP_ROUND 0x810F                   // ��բ�ִ�
#define OI_ELEC_CTRL_FIXED_VALUE 0x8110        // ��ض�ֵ

//  A.10���ļ����������
#define OI_FILE_BLOCK_TRANS_MANAGE 0xF001 //�ļ��ֿ鴫�����

//  A.11 ESAM�ӿ������
#define OI_ESAM 0xF100                // ESAM
#define OI_ESAM_SAFE_MODE_PARA 0xF101 // ��ȫģʽ����
//  A.12 ��������豸�����
#define OI_RS232 0xF200    // RS232
#define OI_RS485 0xF201    // RS485
#define OI_INFRARED 0xF202 // ����
#define OI_SWITCHINPUT 0xF203
#define OI_DC_ANALOG 0xF204         // ֱ��ģ����
#define OI_RELAY_OUTPUT 0xF205      // �̵������
#define OI_ALARM_OUTPUT 0xF206      // �澯���
#define OI_MULTI_FUNC_TERM 0xF207   // �๦�ܶ���
#define OI_AC_SAMPLING 0xF208       // ���ɽӿ�
#define OI_PLC 0xF209               // �ز�/΢�������߽ӿ�
#define OI_PINPUTDEVICE 0xF20A      // ���������豸
#define OI_BLUETOOTH 0xF20B         // ����
#define OI_SLAVENODE_ELEMENT 0xF210 // �ӽڵ㵥Ԫ
//  A.13 ��ʾ�����
#define OI_AUTO_ROUND_DISPLAY 0xF300    // �Զ�����
#define OI_KEYDOWN_ROUND_DISPLAY 0xF301 // ��������
//
#define OI_EVENT_FLAG 0xFF65 // ieslab��չ ȫ�¼���־
//	���������
#define OI_FACTORY_TEST 0xFF64  // ��������
#define OI_EXTENDING_LOG 0xFF66 // ���ļ�¼

    /**
 * @note: address type
 * @author: 
 * @date: 2017-8-18 10:27:12
 *-----------------------------------------------------------------
 */
    typedef enum
    {
        ADDR_SINGLE = 0, // single address
        ADDR_WILD = 1,   // wildcard address
        ADDR_GROUP = 2,  // group address
        ADDR_BROAD = 3,  // broadcast address
    } ADDR_TYPE;

    typedef struct
    {
        UINT8 ucScramble : 1;  // scramble code
        UINT8 ucAddrLogic : 2; // logic Address
        UINT8 ucAddrType : 2;  // see ADDR_TYPE
        UINT8 ucAddrClient;    // client address
    } DLT_LINK_PARA;

    /**
 * @note: frame 
 * @author: 
 * @date: 2017-4-26 22:49:11
 *-----------------------------------------------------------------
 */

    typedef struct
    {
        // ֡ͷ
        UINT8 ucBegin; // ��ʼ�ַ�68H
        // ������L
        UINT8 ucLengthLow;
        UINT8 ucLengthHigh;
        // ������C
        unsigned ucFunctionCode : 3; // ������
        unsigned ucScrambleCode : 1; // �����־
        unsigned ucNull : 1;         // ����
        unsigned ucSplitSign : 1;    // ��֡��־λ
        unsigned ucPRM : 1;          // ������־λ
        unsigned ucDIR : 1;          // ���䷽��λ
        // ��ַ��A
        // ��������ַSA
        unsigned ucServerAddrLen : 4;  // ��������ַ����N
        unsigned ucLogicAddr : 2;      // �߼���ַ
        unsigned ucServerAddrType : 2; // ��������ַ���
        /******* ��������ַ���� *******/
        // �ͻ�����ַCA
        UINT8 ucClientAddr;
        // ֡ͷУ��HCS
        UINT8 ucHCRCLow;
        UINT8 ucHCRCHigh;
        /******* APDU Ӧ�ò� *******/
        // ֡У��FCS
        UINT8 ucFCRCLow;
        UINT8 ucFCRCHigh;
        // �����ַ�
        UINT8 ucEnd; // �����ַ�16H
    } FRAME_LINK_OBJ_ORIENTED;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* __DLT698_45_H__ */
