#ifndef DLT645_H
#define DLT645_H

#include "stm32f37x.h"
#include "BaseDef.h"
#include "Uart.h"
#include "main.h"

/******************************************
程序版本控制参数，根据客户需求进行相应更改 
电压上上限
电压下下限
电压合格上限默认值
电压合格下限默认值
*******************************************/
#ifdef WORK_VOLTAGE_100V

#define WORK_VOLTAGE 1000        //电表工作电压：100V
#define Voltage_MAX_MAX 1100     //电压上上限
#define Voltage_MIN_MIN 900      //电压下下限
#define Voltage_MAX_Default 1030 //100V供电的电表；2017-10-25
#define Voltage_MIN_Default 1000 //电压合格下限默认值，flash未设置时
#else
#define WORK_VOLTAGE 2200        //电表工作电压:220V
#define Voltage_MAX_MAX 2800     //电压上上限
#define Voltage_MIN_MIN 1200     //电压下下限
#define Voltage_MAX_Default 2350 //电压合格上限默认值，flash未设置时 #define Voltage_MIN_Default  				1980						//电压合格下限默认值，flash未设置时
#define Voltage_MIN_Default 1980 //电压合格下限默认值，flash未设置时
#endif
/*电压上限调整相关参数*/
#define Voltage_Change 0xFF                 //电压需要修改状态 //STM32内部flash默认值为0XFF stragen_2017-03-15
#define DAYFROZEN_CHANGE 0xFF               //日冻结需要修改状态
#define DAYFROZEN_NOCHANGE 0x00             //屏蔽日冻结修改
#define Voltage_NOChange 0x00               //屏蔽电压修改
#define UCDAYFROZEN_CHANGE_DEFAULE_RATIO 90 //默认日冻结修改比例：90%
#define DLT645_WORDS_DELAY_MAX 20           //645帧内字节间超时时间，单位50毫秒
#define DLT645_FRAMES_DELAY_MAX 25          //645回帧超时时间，单位50毫秒，超时时间为1.25秒

/*电流各相差值调整相关参数*/
#define CURRENT_MAX_DVALUE 80 //最小电流值不能低于最大电流值的80%

/*枚举类型：电压替代回帧完成状态*/
typedef enum
{
    FrameAwaitingReply = 0,
    FrameReplied = !FrameAwaitingReply
} FrameHandleStatus;
/*结构体类型：电流值*/
typedef struct current
{
    uint32_t value; //电流值,扩大1000倍存储，例如123.456A，按照123456计算
    uint8_t count;  //电流更新计数
    bool isminus;   //负数标志位
} CURRENT;
/*结构体类型：变压器容量、变比和二次侧合格电流上限值*/
typedef struct
{
    /*容量*/
    uint32_t capacity;
    /*额定电流*/
    uint32_t rated_current;
    /*二次侧CT变比*/
    uint32_t CTratio;
    /*CT2次侧三相电流和上限*/
    uint32_t CT2_CurrentLimit;
} TransFormer_TypeDef;

/*结构体类型：数据标识与对应的处理函数*/
typedef struct
{
    uint8_t Data_ID[4];
    void (*Handle)(uint8_t *);
} DefDataIDHandle;

/*全局变量*/
/*645帧缓存结构体和帧数据域长度*/
extern uint8_t ucApp_Buf_DB2ZD[DATA_BUF_SIZE]; //应用层缓存
extern uint8_t ucApp_Buf_ZD2DB[DATA_BUF_SIZE]; //应用层缓存
extern uint8_t ucApp_Buf_INFR[DATA_BUF_SIZE];  //红外口应用缓存
extern uint8_t uc645DataDomainLength;          //接收到的帧数据区长度

/*参数：电压限值*/
extern FrameHandleStatus Voltage_Frame_Status;
extern int Voltage_MAX;              //电压合格上限
extern int Voltage_MIN;              //电压合格下限
extern uint8_t Voltage_Change_State; //电压是否修改标志位
/*电流重载、过载调整参数*/
extern CURRENT Acurrent, Bcurrent, Ccurrent; //电流值全局变量
extern TransFormer_TypeDef TransFormer;
//extern uint32_t			uwCT_Capacity;											//变压器容量，单位kVA
//extern uint32_t			uwCT_Ratio;												//变压器CT变比，一次侧/二次侧
//extern uint32_t			uwCurrent_Limit_Value;	  									//变压器过载电流，二次侧
//extern uint32_t			uwCT_HeavyLoad_Curents;									//变压器重载电流，二次侧

/*参数：电能量修改*/
#ifdef ENABLE_PPLUSENERGY_MODIFY
extern uint8_t ucDayfrozen_Modify_Ratio; //比例的整数位，BCD码
#endif

/*内部函数*/
static UINT8 GetLowByte(int Voltage);                  //将电压个位转化为低字节；
static UINT8 GetHighByte(int Voltage);                 //将电压十位转化为高字节；
static bool BCD_Verity(UINT8 LowByte, UINT8 HighByte); //BCD校验
static STATUS dwCheckFrame(uint8_t *pucBuffer);        //检查帧校验字节和结束字节
static int Get_Voltage(UINT8 LowByte, UINT8 HighByte); //从数据帧中读取电压值
static void vGet_Current(uint8_t LowByte, uint8_t MidByte, uint8_t HighByte, CURRENT *result);
static void vPut_Current(CURRENT current, uint8_t *pLowByte, uint8_t *pMidByte, uint8_t *pHighByte);
static uint16_t Get_Estatus(UINT8 LowByte, UINT8 HighByte); //从数据帧中读取电表运行状态字
static uint32_t Get_Energy(uint8_t FstByte, uint8_t ScdByte, uint8_t TirdByte, uint8_t ForthByte);
static void Fill_Energy(uint32_t energy, uint8_t *pFstByte, uint8_t *pScdByte, uint8_t *pTirdByte, uint8_t *pForthByte);
static bool blCurrentSingle_Modify_Method(CURRENT *ref1, CURRENT *ref2, CURRENT *result); //根据前两相电流修改第三相电流
static void vCurrentTriple_Modify_Method(CURRENT *current_max, CURRENT *current_mid, CURRENT *current_min);

/*对外函数接口*/
extern void vDBDataID_Handle(uint8_t *pucBuffer);   //根据数据标识选择处理函数
extern void vINFRDataID_Handle(uint8_t *pucBuffer); //根据数据标识选择处理函数
extern bool blRecvFrame(UCDRV_BUF *ucDrv_Buf, uint8_t *ucdata_buf);
extern void Voltage_Change_Init(void);             //使能电压修改功能，并从flash中读取电压上下限
extern void vEnergy_Modify_Init(void);             //日冻结修改功能初始化
extern void Set_Voltage_Limit(uint8_t *pucBuffer); //扩充帧：电压限值设置
extern void vVoltageHandle(uint8_t *pucBuffer);
extern void Voltage_Modifier(uint8_t *pucBuffer);                                                                  //修改电压帧的电压数据
extern void vDayfrozen_Modifier(uint8_t *pucBuffer);                                                               //修改日冻结数据函数
extern uint32_t vNow_Frozen_Modify_Mothod(uint32_t nowenergy, uint32_t RealValueAddr, uint32_t ModifiedValueAddr); //日冻结和当前电量修改方法
extern void vNowEnergy_Modifier(uint8_t *pucBuffer);                                                               //修改日冻结数据函数
extern bool blVoltage_Create(uint8_t *pucBuffer);                                                                  //创建电压帧
extern void Get_VChange_State(uint8_t *pucBuffer);                                                                 //扩充帧：更改电压修改标志位
extern void Query_Voltage_Limit(uint8_t *pucBuffer);                                                               //扩充帧：电压限值查询
extern void Default_Reset(uint8_t *pucBuffer);                                                                     //电压限值回复出厂
extern int Voltage_Modifier_Method(int Voltage);                                                                   //电压合格率调整策略
extern void Estatus_Modify_CurrentEvent(uint8_t *pucBuffer);                                                       //修改电表运行状态字
extern void vCurrentDirect_Modify(uint8_t *pucBuffer);                                                             //修改电流方向
extern void vCurrent_Modify(uint8_t *pucBuffer);                                                                   //电流各相差值修改
extern void vCurrent_Count_Clear(void);                                                                            //电流计数清零
extern void vEnergy_Modify_AbleSet(uint8_t *pucBuffer);                                                            //正向有功电能量使能/禁能设置
extern void vEnergy_Modify_RaitoSet(uint8_t *pucBuffer);                                                           //正向有功电能量使能/禁能设置
extern void vEnergy_Modify_Reset(uint8_t *pucBuffer);                                                              //电能量修改回复出厂设置
extern void vAccountEnergy_Modify(uint8_t *pucBuffer);                                                             //结算日正向有功电能量修改函数
extern void vVoltage_SetMaxValue(uint8_t *pucbuffer);
extern void vVoltage_SetMinValue(uint8_t *pucbuffer);
extern void vVoltage_SetMaxMaxValue(uint8_t *pucbuffer);
extern void vVoltage_SetMinMinValue(uint8_t *pucbuffer);
extern void vEnergy_Modify_ReadSet(uint8_t *pucbuffer);
extern void vEnergy_Modify_ReadValue(uint8_t *pucbuffer);
extern void vLoadProfile_Voltage_Modify(uint8_t *pucbuffer);
extern void vLoadProfile_Current_Modify(uint8_t *pucbuffer);

/*电流重过载参数设置*/
extern void vCurrent_Limit_DefaultSet(void);                           //电流重过载参数初始化
extern void vCurrent_Limit_Modify(uint8_t *pucBuffer);                 //电流上限值修改
extern void vCurrent_Limit_Set(uint8_t *pucbuffer);                    //电流重过载参数设置:电流上限
extern void vCurrent_Limit_ReadSet(uint8_t *pucbuffer);                //电流重过载参数读取:电流上限
extern void vCurrent_CTRatio_Set(uint8_t *pucbuffer);                  //电流重过载参数设置:CT变比
extern void vCurrent_CTRatio_ReadSet(uint8_t *pucbuffer);              //电流重过载参数读取:CT变比
extern void vCurrent_TransFormer_Capacity_ReadSet(uint8_t *pucbuffer); //电流重过载参数读取:CT变比

/*查询程序版本*/
extern void vRead_Version(uint8_t *pucbuffer);

#endif
;
