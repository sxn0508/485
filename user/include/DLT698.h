#ifndef DLT698_H
#define DLT698_H
#include "BaseDef.h"
#include "DLT698_45.h"

#define MAX_APDU_LENGTH 2048
#define MAX_ROAD_NUM_PER_ROAD 32
#define MAX_COLL_OAD_SIZE 127

#define PPPINITFCS16 0xFFFF

typedef struct
{
    uint8_t *pStart;        //指向帧头
    uint8_t *pSecurityData; //指向安全数据
    uint16_t uwSA_len;      //SA长度
    uint16_t nHCSPos;
    uint16_t uwFramelen; //帧长度，不含头尾
} DLT698_FRAME;

typedef struct
{
    OAD stOAD;    // OAD
    UINT8 ucVal;  // store valid mark
    UINT16 uwLen; // Data length
    UINT8 *ucPtr; // Data buffer pointer
} DATA_UNIT;

typedef enum
{
    D_CLASS_NULL = 0,
    D_CLASS_REAL = 1,   // 实时数据
    D_CLASS_CUR_FREEZE, // 秒、分、小时冻结
    D_CLASS_DAY_FREEZE, // 日冻结数据
    D_CLASS_MON_FREEZE, // 月冻结数据

    D_CLASS_EVENT, // 事件类数据

    D_CLASS_DATA, // real + cur + day + mon
} DATA_CLASS;
typedef struct
{
    DATA_CLASS eDataCls;

    //UINT16 uwConfigNo; // meter config number

    //UINT8 ucMeterAddr[MAX_METER_ADDR_LEN];
    //UINT8 ucMeterLen;

    //time_t tCollStore;   // collect store time
    //time_t tCollStart;   // collect start time
    //time_t tCollSuccess; // collect success time
    //time_t tFreezeTag;   // collect data freeze time

    //UINT32 udwRecordNo;  // event record number
    //time_t tEventHappen; // event happen time
    //time_t tEventFinish; // event finish time

    OAD stMainOAD; // main OAD of ROAD

    UINT8 ucDataNum;

    DATA_UNIT stDataUnit[MAX_COLL_OAD_SIZE]; // OAD + Len + Data pointer to store
} COLL_STORE_DATA;
typedef enum
{
    GET_COMPLETE,
    GET_NOT_COMPLETE,
    GET_PARTLY_COMPLETE
} GET_COMPLETE_TYPE;

typedef struct
{
    OAD stOAD;
    UINT16 uwAttrIndexRead;
    GET_COMPLETE_TYPE eIsComplete;
} GET_NORMAL_INFO;

extern int dwGet698Apdu(DLT698_FRAME *pframe, UINT8 **pAPDU);
extern int dwGetApdu(UINT8 *pframe, UINT8 **pApdu);
extern UINT16 pppfcs16(UINT16 fcs, unsigned char *cp, int len);
extern int dw698LinkAnalyze(UINT8 *pframe, UINT8 *pAPDU);
extern int dwAPduAnalyze(UINT8 *pucBuf, UINT32 dwlen, COLL_STORE_DATA *pstCollStoreData);
extern int dwReCalculateFCS(DLT698_FRAME *pframe);
#endif