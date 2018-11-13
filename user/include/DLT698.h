#ifndef DLT698_H
#define DLT698_H
#include "BaseDef.h"
#include "DLT698_45.h"

#define MAX_APDU_LENGTH 2048
#define MAX_ROAD_NUM_PER_ROAD 32
#define MAX_COLL_OAD_SIZE 127

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

extern int dwGetApdu(UINT8 *pucPtr, UINT8 *pucStr);
extern UINT16 pppfcs16(UINT16 fcs, unsigned char *cp, int len);
extern int dw698LinkAnalyze(UINT8 *pframe, UINT8 *pAPDU);
//extern int dwAPduAnalyze(UINT8 *pucBuf, UINT32 dwlen);
extern int dwAPduAnalyze(UINT8 *pucBuf, UINT32 dwlen, COLL_STORE_DATA *pstCollStoreData);
#endif