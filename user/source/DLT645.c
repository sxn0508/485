#include "DLT645.h"
#include "Time.h"
#include "Flash.h"

/*串口驱动缓存*/
static struct ucbuf Uart1_Rsvbuf, Uart1_Sndbuf, Uart3_Rsvbuf, Uart3_Sndbuf, Uart2_Rsvbuf, Uart2_Sndbuf;
/*应用层串口结构体*/
static UartDef UartZD = {USART1, &Uart1_Rsvbuf, &Uart1_Sndbuf};
static UartDef UartDB = {USART3, &Uart3_Rsvbuf, &Uart3_Sndbuf};
static UartDef UartIR = {USART2, &Uart2_Rsvbuf, &Uart2_Sndbuf};
/*应用层串口结构体指针*/
UartDef *pUartZD = &UartZD;
UartDef *pUartDB = &UartDB;
UartDef *pUartIR = &UartIR;
/*规约帧缓存*/
uint8_t ucApp_Buf_DB2ZD[DATA_BUF_SIZE]; //电表上行帧应用缓存
uint8_t ucApp_Buf_ZD2DB[DATA_BUF_SIZE]; //终端下行帧应用缓存
uint8_t ucApp_Buf_INFR[DATA_BUF_SIZE];  //红外口应用缓存
/*
#define ENABLE_ESTATUS						//电表状态字修改功能 
#define ENABLE_CURRENT_DEVIATION			//电流各相差值修改功能
#define ENABLE_CURRENT_LIMIT				//电流最大值修改功能
#define ENABLE_ENERGY_MODIFY				//修改正向有功
#define ENABLE_INFR							//修改正向有功
*/
const DefDataIDHandle INFRDataID_Handle_List[] = {
    {{0X33, 0X37, 0X34, 0X35}, Get_VChange_State},       //扩充：电压修改使能/禁能ID_02010400,
    {{0X33, 0X39, 0X34, 0X35}, Query_Voltage_Limit},     //扩充：电压限值查询ID_02010600,
    {{0X33, 0X3A, 0X34, 0X35}, Default_Reset},           //扩充：电压修改功能恢复出厂设置ID_02010700,
    {{0X33, 0X3B, 0X34, 0X35}, vVoltage_SetMaxValue},    //扩充：电压设置合格上限
    {{0X33, 0X3C, 0X34, 0X35}, vVoltage_SetMinValue},    //扩充：电压设置合格下限
    {{0X33, 0X3D, 0X34, 0X35}, vVoltage_SetMaxMaxValue}, //扩充：电压设置修改上限
    {{0X33, 0X3E, 0X34, 0X35}, vVoltage_SetMinMinValue}, //扩充：电压设置修改下限
    {{0X33, 0X73, 0X34, 0X33}, vEnergy_Modify_AbleSet},  //扩充：正向有功电能量修改使能/禁能ID_00014000,
    {{0X33, 0X74, 0X34, 0X33}, vEnergy_Modify_RaitoSet}, //扩充：正向有功电能量修改比例设置ID_00014100,
    {{0X33, 0X75, 0X34, 0X33}, vEnergy_Modify_Reset},    //扩充：正向有功电能量修改恢复出厂设置ID_00014200,
    {{0X33, 0X76, 0X34, 0X33}, vEnergy_Modify_ReadSet},  //扩充：正向有功电能量修改读取设：置
    /*电流重过载*/
    {{0X33, 0X37, 0X35, 0X35}, vCurrent_Limit_Set},                    //扩充：设置电流重过载参数:电流上限
    {{0X33, 0X38, 0X35, 0X35}, vCurrent_Limit_ReadSet},                //扩充：设置电流重过载参数:电流上限读取
    {{0X33, 0X39, 0X35, 0X35}, vCurrent_CTRatio_Set},                  //扩充：设置电流重过载参数:二次侧CT变比
    {{0X33, 0X3A, 0X35, 0X35}, vCurrent_CTRatio_ReadSet},              //扩充：读取电流重过载参数:二次侧CT变比
    {{0X33, 0X3B, 0X35, 0X35}, vCurrent_TransFormer_Capacity_ReadSet}, //扩充：读取电流重过载参数:变压器容量
    {{0X32, 0X32, 0X32, 0X32}, vRead_Version},                         //扩充：读取程序版本
};
const DefDataIDHandle DBDataID_Handle_List[] = {
    {{0X33, 0X34, 0X34, 0X35}, vVoltageHandle}, //A相电压ID_02010100,
    {{0X33, 0X35, 0X34, 0X35}, vVoltageHandle}, //B相电压ID_02010200,
    {{0X33, 0X36, 0X34, 0X35}, vVoltageHandle}, //C相电压ID_02010300,
    {{0X33, 0X32, 0X34, 0X35}, vVoltageHandle}, //电压数据块ID_0201FF00,
#ifdef ENABLE_CURRENT_DEVIATION
    {{0X33, 0X34, 0X35, 0X35}, vCurrent_Modify}, //A相电流ID_02020100,
    {{0X33, 0X35, 0X35, 0X35}, vCurrent_Modify}, //B相电流ID_02020200,
    {{0X33, 0X36, 0X35, 0X35}, vCurrent_Modify}, //C相电流ID_02020300,
    {{0X33, 0X32, 0X35, 0X35}, vCurrent_Modify}, //电流数据块ID_0202FF00,
#else
#ifdef ENABLE_CURRENT_LIMIT
    {{0X33, 0X34, 0X35, 0X35}, vCurrent_Limit_Modify}, //A相电流ID_02020100,
    {{0X33, 0X35, 0X35, 0X35}, vCurrent_Limit_Modify}, //B相电流ID_02020200,
    {{0X33, 0X36, 0X35, 0X35}, vCurrent_Limit_Modify}, //C相电流ID_02020300,
    {{0X33, 0X32, 0X35, 0X35}, vCurrent_Limit_Modify}, //电流数据块ID_0202FF00,
#endif
#endif
    {{0X37, 0X38, 0X33, 0X38}, Estatus_Modify_CurrentEvent}, //电表运行状态字4ID_04000504,
    {{0X38, 0X38, 0X33, 0X38}, Estatus_Modify_CurrentEvent}, //电表运行状态字5ID_04000505,
    {{0X39, 0X38, 0X33, 0X38}, Estatus_Modify_CurrentEvent}, //电表运行状态字6ID_04000506,
    {{0X3A, 0X38, 0X33, 0X38}, Estatus_Modify_CurrentEvent}, //电表运行状态字7ID_04000507,
    {{0X32, 0X38, 0X33, 0X38}, Estatus_Modify_CurrentEvent}, //电表运行状态字数据块ID_040005FF,
    {{0X33, 0X33, 0X34, 0X33}, vNowEnergy_Modifier},         //当前正向有功总电能ID_00010000,
    {{0X33, 0X34, 0X34, 0X33}, vNowEnergy_Modifier},         //当前正向有功费率1电能ID_00010100,
    {{0X33, 0X35, 0X34, 0X33}, vNowEnergy_Modifier},         //当前正向有功费率2电能ID_00010200,
    {{0X33, 0X36, 0X34, 0X33}, vNowEnergy_Modifier},         //当前正向有功费率3电能ID_00010300,
    {{0X33, 0X37, 0X34, 0X33}, vNowEnergy_Modifier},         //当前正向有功费率4电能ID_00010400,
    {{0X33, 0X32, 0X34, 0X33}, vNowEnergy_Modifier},         //当前正向有功电能数据块ID_0001FF00,
    {{0X33, 0X33, 0X48, 0X33}, vNowEnergy_Modifier},         //当前A相正向有功ID_00150000,
    {{0X33, 0X33, 0X5C, 0X33}, vNowEnergy_Modifier},         //当前B相正向有功ID_00290000,
    {{0X33, 0X33, 0X70, 0X33}, vNowEnergy_Modifier},         //当前C相正向有功ID_003D0000,
    {{0X34, 0X34, 0X39, 0X38}, vDayfrozen_Modifier},         //上1日冻结正向有功电能数据（费率1~4）ID_05060101,
    {{0X35, 0X34, 0X39, 0X38}, vDayfrozen_Modifier},         //上2日冻结正向有功电能数据（费率1~4）ID_05060102,
    {{0X36, 0X34, 0X39, 0X38}, vDayfrozen_Modifier},         //上3日冻结正向有功电能数据（费率1~4） ID_05060103,
    {{0X34, 0X33, 0X34, 0X33}, vAccountEnergy_Modify},       //上1结算日正向有功总电能ID_00010001,
    {{0X34, 0X34, 0X34, 0X33}, vAccountEnergy_Modify},       //上1结算日正向有功费率1电能ID_00010101,
    {{0X34, 0X35, 0X34, 0X33}, vAccountEnergy_Modify},       //上1结算日正向有功费率1电能 ID_00010201,
    {{0X34, 0X36, 0X34, 0X33}, vAccountEnergy_Modify},       //上1结算日正向有功费率1电能ID_00010301,
    {{0X34, 0X37, 0X34, 0X33}, vAccountEnergy_Modify},       //上1结算日正向有功费率1电能ID_00010401,
    {{0X34, 0X32, 0X34, 0X33}, vAccountEnergy_Modify},       //上1结算日正向有功电能数据块ID_0001FF01,
    {{0X34, 0X33, 0X48, 0X33}, vAccountEnergy_Modify},       //上1结算日A相正向有功电能ID_00150001,
    {{0X34, 0X33, 0X5C, 0X33}, vAccountEnergy_Modify},       //上1结算日B相正向有功电能ID_00290001,
    {{0X34, 0X33, 0X70, 0X33}, vAccountEnergy_Modify},       //上1结算日C相正向有功电能ID_003D0001,
    {{0X33, 0X37, 0X34, 0X35}, Get_VChange_State},           //扩充：电压修改使能/禁能ID_02010400,
    {{0X33, 0X38, 0X34, 0X35}, Set_Voltage_Limit},           //扩充：电压限值设置ID_02010500,
    {{0X33, 0X39, 0X34, 0X35}, Query_Voltage_Limit},         //扩充：电压限值查询ID_02010600,
    {{0X33, 0X3A, 0X34, 0X35}, Default_Reset},               //扩充：电压修改功能恢复出厂设置ID_02010700,

    /*电流重过载*/
    {{0X33, 0X37, 0X35, 0X35}, vCurrent_Limit_Set},                    //扩充：设置电流重过载参数:电流上限
    {{0X33, 0X38, 0X35, 0X35}, vCurrent_Limit_ReadSet},                //扩充：设置电流重过载参数:电流上限读取
    {{0X33, 0X39, 0X35, 0X35}, vCurrent_CTRatio_Set},                  //扩充：设置电流重过载参数:二次侧CT变比
    {{0X33, 0X3A, 0X35, 0X35}, vCurrent_CTRatio_ReadSet},              //扩充：读取电流重过载参数:二次侧CT变比
    {{0X33, 0X3B, 0X35, 0X35}, vCurrent_TransFormer_Capacity_ReadSet}, //扩充：读取电流重过载参数:变压器容量

    {{0X33, 0X73, 0X34, 0X33}, vEnergy_Modify_AbleSet},   //扩充：正向有功电能量修改使能/禁能ID_00014000,
    {{0X33, 0X74, 0X34, 0X33}, vEnergy_Modify_RaitoSet},  //扩充：正向有功电能量修改比例设置ID_00014100,
    {{0X33, 0X75, 0X34, 0X33}, vEnergy_Modify_Reset},     //扩充：正向有功电能量回复出厂设置ID_00014200,
    {{0X33, 0X76, 0X34, 0X33}, vEnergy_Modify_ReadSet},   //扩充：正向有功电能量修改读取设置
    {{0X33, 0X77, 0X34, 0X33}, vEnergy_Modify_ReadValue}, //扩充：正向有功电能量读取存储的真实值和修改值ID_00014200,
    /*4号补遗负荷曲线连续抄读部分中的电压曲线、电流曲线修改,2018年8月7日20:53:4：0*/
    {{0X33, 0X34, 0X43, 0X39}, vLoadProfile_Voltage_Modify}, //A相电压曲线连续抄读ID_06100101,
    {{0X34, 0X34, 0X43, 0X39}, vLoadProfile_Voltage_Modify}, //B电压数据块曲线连续抄读ID_06100102,
    {{0X35, 0X34, 0X43, 0X39}, vLoadProfile_Voltage_Modify}, //C电压数据块曲线连续抄读ID_06100103,
    {{0X32, 0X34, 0X43, 0X39}, vLoadProfile_Voltage_Modify}, //电压数据块曲线连续抄读ID_061001FF,
    {{0X33, 0X35, 0X43, 0X39}, vLoadProfile_Current_Modify}, //A相电流曲线连续抄读ID_06100201,
    {{0X34, 0X35, 0X43, 0X39}, vLoadProfile_Current_Modify}, //B电流数据块曲线连续抄读ID_06100202,
    {{0X35, 0X35, 0X43, 0X39}, vLoadProfile_Current_Modify}, //C电流数据块曲线连续抄读ID_06100203,
    {{0X32, 0X35, 0X43, 0X39}, vLoadProfile_Current_Modify}, //电流数据块曲线连续抄读ID_061002FF,
    {{0X32, 0X32, 0X32, 0X32}, vRead_Version},               //扩充：读取程序版本
};

FrameHandleStatus Voltage_Frame_Status;
uint8_t ucRcvStatus = 0;
uint8_t uc645DataDomainLength = 0;                 //645帧数据域长度
uint8_t Voltage_Change_State = Voltage_NOChange;   //电压是否修改标志位，默认修改状态
uint8_t Dayfrozen_Change_State = DAYFROZEN_CHANGE; //电量是否修改，默认修改
uint8_t ucDayfrozen_Modify_Ratio;                  //电量调整比例
int Voltage_MAX;                                   //电压合格上限
int Voltage_MIN;                                   //电压合格下限
int A_Voltage = WORK_VOLTAGE;                      //最后一次修改后的A相电压值
int B_Voltage = WORK_VOLTAGE;                      //最后一次修改后的B相电压值
int C_Voltage = WORK_VOLTAGE;                      //最后一次修改后的C相电压值
CURRENT Acurrent = {0, 0, false};
CURRENT Bcurrent = {0, 0, false};
CURRENT Ccurrent = {0, 0, false};

/*变压器结构体定义*/
TransFormer_TypeDef TransFormer;

/*电流重载、过载调整相关参数
#ifdef	  TRANSFORMER_CAPACITY_50KVA
	#define TransFormer.CT2_CurrentLimit					2700						//50KVA变压器的CT二次侧三相电流和的最大值，扩大1000倍，实际值是2.7A
#endif
#ifdef	  TRANSFORMER_CAPACITY_100KVA
	#define TransFormer.CT2_CurrentLimit					5400						//100KVA变压器的CT二次侧三相电流和的最大值，扩大1000倍，实际值是5.4A	
#endif
#ifdef	  TRANSFORMER_CAPACITY_200KVA
	#define TransFormer.CT2_CurrentLimit					10800						//200KVA变压器的CT二次侧三相电流和的最大值，扩大1000倍，实际值是10.8A
#endif
#ifdef	  TRANSFORMER_CAPACITY_400KVA
	#define TransFormer.CT2_CurrentLimit					21600						//400KVA变压器的CT二次侧三相电流和的最大值，扩大1000倍，实际值是21.6A
#endif
*/

/********************************************************************
 * 功    能：将报文存到应用层缓存，并判断是否为645帧
 * 输    入：应用层缓存指针，驱动层缓存指针
 * 输    出：如果都合法返回true
 否则返回false
 *           
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/

bool blRecvFrame(struct ucbuf *ucDrv_Buf, uint8_t *ucdata_buf)
{
    /*收帧状态*/
    switch (ucRcvStatus)
    {
    /*帧头68*/
    case SEARCH_SOH:
    {
        /*未读到指定字节数，跳出分支，返回false*/
        if (ucDrv_Buf_GetBytes(ucDrv_Buf, ucdata_buf, 1) == false)
        {
            break;
        }
        /*读到一个字节*/
        else
        {
            /*帧头不合法，返回false*/
            if (*(ucdata_buf + 0) != 0X68)
            {
                break;
            }
            else
            {
                /*帧头合法，收帧状改为RCV_DATA1*/
                ucRcvStatus = RCV_DATA1;
                blWordsCounterStatus = true;
            }
        }
    }
    case RCV_DATA1:
    {
        /*读9个字节失败，跳出分支，返回false*/
        if (ucDrv_Buf_GetBytes(ucDrv_Buf, ucdata_buf + 1, 9) == false)
        {
            /*DLT645_WORDS_DELAY_MAX，字节接收超时*/
            if (blTime2_Update(uwTime2WordsCounter, DLT645_WORDS_DELAY_MAX))
            {
                /*超时则重新判断帧头68，不超时则继续找长度；*/
                ucRcvStatus = SEARCH_SOH;
                blWordsCounterStatus = false;
                uwTime2WordsCounter = 0;
            }
            break;
        }
        else //读68之后9个字节成功
        {
            if ((*(ucdata_buf + 7) != 0X68) || ((*(ucdata_buf + 9) + 12) >= 255))
            {
                ucRcvStatus = SEARCH_SOH; //第七个字节不为68 ,跳出，返回false，重新找68
                blWordsCounterStatus = false;
                uwTime2WordsCounter = 0;
                break;
            }
            else
            {                                              //第七个字节是68
                uc645DataDomainLength = *(ucdata_buf + 9); //读出645帧长度L
                ucRcvStatus = RCV_DATA2;                   //指向下一部分
                blWordsCounterStatus = false;
                uwTime2WordsCounter = 0;
                blWordsCounterStatus = true;
            }
        }
    }
    case RCV_DATA2:
    {
        if (ucDrv_Buf_GetBytes(ucDrv_Buf, ucdata_buf + 10, uc645DataDomainLength + 2) == false)
        {                                                                    //读后续字节失败，跳出分支，返回false；
            if (blTime2_Update(uwTime2WordsCounter, DLT645_WORDS_DELAY_MAX)) //DLT645_WORDS_DELAY_MAX如果字节接收超时
            {                                                                //如果超时，则重新找68，否则重新接收后续字节；跳出分支
                ucRcvStatus = SEARCH_SOH;
                blWordsCounterStatus = false;
                uwTime2WordsCounter = 0;
            }
            break;
        }
        else //读出指定字节数的一帧
        {
            ucRcvStatus = SEARCH_SOH; //状态改为查找68；
            blWordsCounterStatus = false;
            uwTime2WordsCounter = 0;
            if (dwCheckFrame(ucdata_buf) != OK_1)
            {
                //	if (blTime2_Update(uwTime2WordsCounter,DLT645_WORDS_DELAY_MAX)) //DLT645_WORDS_DELAY_MAX如果字节接收超时
                //{ //如果超时，则重新找68，否则重新接收后续字节；跳出分支
                //ucRcvStatus = SEARCH_SOH;
                //}//帧校验错误
                return false;
            }
            else
            {
                //ucRcvStatus = SEARCH_SOH;//状态改为查找68；
                //blWordsCounterStatus = false;
                return true; //判断收帧是否合格，合格返回true，否则返回false
            }
        }
    }
    break;
    default:
        break;
    }
    return false;
}

/********************************************************************
 * 功    能：判断帧的校验和、结束字节是否合法
 * 输    入：应用层缓存指针
 * 输    出：如果都合法返回true
 否则返回false
 *           
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
STATUS dwCheckFrame(uint8_t *pucBuffer)
{
    uint8_t ucCheckData = 0;
    uint8_t i;
    //求取校验和
    for (i = 0; i < uc645DataDomainLength + 10; i++)
    {
        ucCheckData += pucBuffer[i];
    }
    //判断校验和和结束符
    if ((ucCheckData == pucBuffer[uc645DataDomainLength + 10]) && (pucBuffer[uc645DataDomainLength + 11] == 0x16))
    {
        return OK_1;
    }
    else
    {
        return ERROR_1;
    }
}

/********************************************************************
 * 功    能：电表：侧收帧数据标识分析处理
 * 输    入：应用层缓存指针
 * 输    出：数据标识
 * 编 写 人：王君龙
 * 编写日期：2018.5.8
 **********************************************************************/
void vDBDataID_Handle(uint8_t *pucBuffer)
{
    uint8_t i;
    uint16_t idcount;
    idcount = sizeof(DBDataID_Handle_List) / sizeof(DBDataID_Handle_List[0]);
    for (i = 0; i < idcount; i++)
    {
        if ((DBDataID_Handle_List[i].Data_ID[0] == pucBuffer[10]) &&
            (DBDataID_Handle_List[i].Data_ID[1] == pucBuffer[11]) &&
            (DBDataID_Handle_List[i].Data_ID[2] == pucBuffer[12]) &&
            (DBDataID_Handle_List[i].Data_ID[3] == pucBuffer[13]))
            break;
    }
    if (i < idcount)
    {
#ifndef ENABLE_CURRENT_DEVIATION
        if (DBDataID_Handle_List[i].Handle == vCurrent_Modify)
            return;
#endif
#ifndef ENABLE_CURRENT_LIMIT
        if (DBDataID_Handle_List[i].Handle == vCurrent_Limit_Modify)
            return;
#endif
#ifndef ENABLE_ESTATUS
        if (DBDataID_Handle_List[i].Handle == Estatus_Modify_CurrentEvent)
            return;
#endif
#ifndef ENABLE_ENERGY_MODIFY
        if ((DBDataID_Handle_List[i].Handle == vNowEnergy_Modifier) ||
            ((DBDataID_Handle_List[i].Handle == vDayfrozen_Modifier)) ||
            ((DBDataID_Handle_List[i].Handle == vAccountEnergy_Modify)) ||
            ((DBDataID_Handle_List[i].Handle == vEnergy_Modify_AbleSet)) ||
            ((DBDataID_Handle_List[i].Handle == vEnergy_Modify_RaitoSet)) ||
            ((DBDataID_Handle_List[i].Handle == vEnergy_Modify_Reset)))
            return;
#endif
        DBDataID_Handle_List[i].Handle(pucBuffer);
    }
    else
        return;
}

/********************************************************************
 * 功    能：红外侧收帧数据标识分析处理
 * 输    入：应用层缓存指针
 * 输    出：数据标识
 * 编 写 人：王君龙
 * 编写日期：2018.5.8
 **********************************************************************/
void vINFRDataID_Handle(uint8_t *pucBuffer)
{
    uint8_t i;
    uint16_t idcount;
    idcount = sizeof(INFRDataID_Handle_List) / sizeof(INFRDataID_Handle_List[0]);
    for (i = 0; i < idcount; i++)
    {
        if ((INFRDataID_Handle_List[i].Data_ID[0] == pucBuffer[10]) &&
            (INFRDataID_Handle_List[i].Data_ID[1] == pucBuffer[11]) &&
            (INFRDataID_Handle_List[i].Data_ID[2] == pucBuffer[12]) &&
            (INFRDataID_Handle_List[i].Data_ID[3] == pucBuffer[13]))
            break;
    }
    if (i < idcount)
    {
#ifndef ENABLE_ENERGY_MODIFY
        if (((DBDataID_Handle_List[i].Handle == vEnergy_Modify_AbleSet)) ||
            ((DBDataID_Handle_List[i].Handle == vEnergy_Modify_RaitoSet)) ||
            ((DBDataID_Handle_List[i].Handle == vEnergy_Modify_Reset)) ||
            ((DBDataID_Handle_List[i].Handle == vEnergy_Modify_ReadSet)) ||
            ((DBDataID_Handle_List[i].Handle == vEnergy_Modify_ReadValue)))
            return;
#endif
        INFRDataID_Handle_List[i].Handle(pucBuffer);
    }
    else
        return;
}

/********************************************************************
 * 功    能：扩充帧：更改电压修改标志位
 * 输    入：应用层缓存指针
 * 输    出：none
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
void Get_VChange_State(uint8_t *pucBuffer)
{
    uint8_t i;
    if (pucBuffer[14] == 0x34)
    {
        Voltage_Change_State = Voltage_NOChange;
    }
    if (pucBuffer[14] == 0x33)
    {
        Voltage_Change_State = Voltage_Change;
    }
    if (ERROR == Flash_Write_Word(FLASH_VChangeState_Addr, Voltage_Change_State))
    {
        pucBuffer[8] = 0xD4;
    }
    else
        pucBuffer[8] = 0x94;
    pucBuffer[9] = 0x00;
    pucBuffer[10] = 0x00;
    for (i = 0; i < 10; i++)
    {
        pucBuffer[10] += pucBuffer[i];
    }
    pucBuffer[11] = 0x16;
    /*电表口回复*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}
/********************************************************************
 * 功    能：扩充帧：电压限值设置
 * 输    入：应用层缓存指针
 * 输    出：none
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
void Set_Voltage_Limit(uint8_t *pucBuffer)
{
    int VMAX = 0;
    int VMIN = 0;
    int i = 0;
    VMAX = Get_Voltage(pucBuffer[14], pucBuffer[15]);
    VMIN = Get_Voltage(pucBuffer[16], pucBuffer[17]);
    if ((VMAX > VMIN) && (VMAX < Voltage_MAX_MAX) //电压限值设置条件，上限大于下限且小于上上限
        && (VMIN > Voltage_MIN_MIN))              //电压下限大于下下限
    {
        Voltage_MAX = VMAX;
        Voltage_MIN = VMIN;
        Flash_Write_Word(FLASH_VMAX1_Addr, pucBuffer[14]);
        Flash_Write_Word(FLASH_VMAX2_Addr, pucBuffer[15]);
        Flash_Write_Word(FLASH_VMIN1_Addr, pucBuffer[16]);
        Flash_Write_Word(FLASH_VMIN2_Addr, pucBuffer[17]);
        pucBuffer[8] = 0x94; //设置成功标志
    }
    else
    {
        pucBuffer[8] = 0xD4; //设置异常标志
    }
    pucBuffer[9] = 0x00;
    pucBuffer[10] = 0x00; //求取校验和
    for (i = 0; i < 10; i++)
    {
        pucBuffer[10] += pucBuffer[i];
    }
    pucBuffer[11] = 0x16; //结束符
    /*电表口回复*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}

/********************************************************************
 * 功    能：扩充帧：电压限值查询
 * 输    入：应用层缓存指针
 * 输    出：none
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
void Query_Voltage_Limit(uint8_t *pucBuffer)
{
    int i = 0;
    pucBuffer[8] = 0x91;
    pucBuffer[9] = 0x0C;
    pucBuffer[14] = GetLowByte(Voltage_MAX_MAX);
    pucBuffer[15] = GetHighByte(Voltage_MAX_MAX); //电压上上限
    pucBuffer[16] = GetLowByte(Voltage_MIN_MIN);
    pucBuffer[17] = GetHighByte(Voltage_MIN_MIN); //电压下下限
    pucBuffer[18] = GetLowByte(Voltage_MAX);
    pucBuffer[19] = GetHighByte(Voltage_MAX); //电压上限值
    pucBuffer[20] = GetLowByte(Voltage_MIN);
    pucBuffer[21] = GetHighByte(Voltage_MIN); //电压下限值
    pucBuffer[22] = 0x00;                     //求取校验和
    for (i = 0; i < 24 - 2; i++)              //报文长度24
    {
        pucBuffer[22] += pucBuffer[i];
    }
    pucBuffer[23] = 0x16; //结束符
    uc645DataDomainLength = 12;
    /*电表口回复*/
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 24);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}

/********************************************************************
 * 功    能：电压限值恢复出厂
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
void Default_Reset(uint8_t *pucBuffer)
{
    int i = 0;
    A_Voltage = WORK_VOLTAGE;                                        //A相电压值改为标准值
    B_Voltage = WORK_VOLTAGE;                                        //B相电压值改为标准值
    C_Voltage = WORK_VOLTAGE;                                        //C相电压值改为标准值
    Voltage_Change_State = Voltage_Change;                           //电压修改状态改为“有效”
    Flash_Write_Word(FLASH_VChangeState_Addr, Voltage_Change_State); //存储到FLASH中
    Voltage_MAX = Voltage_MAX_Default;                               //电压合格限值恢复默认并且存档到FLASH中
    Voltage_MIN = Voltage_MIN_Default;
    Flash_Write_Word(FLASH_VMAX1_Addr, GetLowByte(Voltage_MAX));
    Flash_Write_Word(FLASH_VMAX2_Addr, GetHighByte(Voltage_MAX));
    Flash_Write_Word(FLASH_VMIN1_Addr, GetLowByte(Voltage_MIN));
    Flash_Write_Word(FLASH_VMIN2_Addr, GetHighByte(Voltage_MIN));
    pucBuffer[8] = 0x94;  //求取校验和
    pucBuffer[9] = 0x00;  //求取校验和
    pucBuffer[10] = 0x00; //求取校验和
    for (i = 0; i < 10; i++)
    {
        pucBuffer[10] += pucBuffer[i];
    }
    pucBuffer[11] = 0x16; //结束符
    uc645DataDomainLength = 0;
    /*电表口回复*/
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口发送*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}

/********************************************************************
 * 功    能：电压帧处理函数
 * 输    入：应用层缓存指针，驱动层缓存指针
 * 输    出：
 *           
 * 编 写 人：王君龙
 * 编写日期：2018.5.10
 **********************************************************************/
void vVoltageHandle(uint8_t *pucBuffer)
{
    /*等待电压回帧状态清位*/
    Voltage_Frame_Status = FrameReplied;
    /*计时器复位*/
    blFramesCounterStatus = false;
    uwTime2FramesCounter = 0;
    /*电压帧判断处理*/
    Voltage_Modifier(pucBuffer);
}

/********************************************************************
 * 功    能：电压合格率判断调整
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
void Voltage_Modifier(uint8_t *pucBuffer)
{

    uint8_t ucCheckData = 0;
    uint8_t i = 0;
    //IWDG_ReloadCounter();
    if (Voltage_Change_State == Voltage_NOChange)
        return;
    if ((pucBuffer[8] == 0X91) && (pucBuffer[10] == 0x33) && (pucBuffer[12] == 0x34) && (pucBuffer[13] == 0x35))
    {
        if (pucBuffer[11] == 0x32) //三相电压数据块
        {
            if (BCD_Verity(pucBuffer[14], pucBuffer[15])) //BCD校验
            {
                A_Voltage = Get_Voltage(pucBuffer[14], pucBuffer[15]);
                if ((A_Voltage > Voltage_MAX) || (A_Voltage < Voltage_MIN)) //修改A相电压
                {
                    A_Voltage = Voltage_Modifier_Method(A_Voltage);
                    pucBuffer[14] = GetLowByte(A_Voltage);
                    pucBuffer[15] = GetHighByte(A_Voltage);
                }
            }
            if (BCD_Verity(pucBuffer[16], pucBuffer[17])) //BCD校验
            {
                B_Voltage = Get_Voltage(pucBuffer[16], pucBuffer[17]);
                if ((B_Voltage > Voltage_MAX) || (B_Voltage < Voltage_MIN)) //修改B相电压
                {
                    B_Voltage = Voltage_Modifier_Method(B_Voltage);
                    pucBuffer[16] = GetLowByte(B_Voltage);
                    pucBuffer[17] = GetHighByte(B_Voltage);
                }
            }
            if (BCD_Verity(pucBuffer[18], pucBuffer[19])) //BCD校验
            {
                C_Voltage = Get_Voltage(pucBuffer[18], pucBuffer[19]);
                if ((C_Voltage > Voltage_MAX) || (C_Voltage < Voltage_MIN)) //修改C相电压
                {
                    C_Voltage = Voltage_Modifier_Method(C_Voltage);
                    pucBuffer[18] = GetLowByte(C_Voltage);
                    pucBuffer[19] = GetHighByte(C_Voltage);
                }
            }
        }
        if (pucBuffer[11] == 0x34) //A相电压数据
        {
            if (BCD_Verity(pucBuffer[14], pucBuffer[15])) //BCD校验
            {
                A_Voltage = Get_Voltage(pucBuffer[14], pucBuffer[15]);
                if ((A_Voltage > Voltage_MAX) || (A_Voltage < Voltage_MIN)) //修改A相电压
                {
                    A_Voltage = Voltage_Modifier_Method(A_Voltage);
                    pucBuffer[14] = GetLowByte(A_Voltage);
                    pucBuffer[15] = GetHighByte(A_Voltage);
                }
            }
        }
        if (pucBuffer[11] == 0x35) //B相电压数据
        {
            if (BCD_Verity(pucBuffer[14], pucBuffer[15])) //BCD校验
            {
                B_Voltage = Get_Voltage(pucBuffer[14], pucBuffer[15]);
                if ((B_Voltage > Voltage_MAX) || (B_Voltage < Voltage_MIN)) //修改B相电压
                {
                    B_Voltage = Voltage_Modifier_Method(B_Voltage);
                    pucBuffer[14] = GetLowByte(B_Voltage);
                    pucBuffer[15] = GetHighByte(B_Voltage);
                }
            }
        }
        if (pucBuffer[11] == 0x36) //C相电压数据
        {
            if (BCD_Verity(pucBuffer[14], pucBuffer[15])) //BCD校验
            {
                C_Voltage = Get_Voltage(pucBuffer[14], pucBuffer[15]);
                if ((C_Voltage > Voltage_MAX) || (C_Voltage < Voltage_MIN)) //修改C相电压
                {
                    C_Voltage = Voltage_Modifier_Method(C_Voltage);
                    pucBuffer[14] = GetLowByte(C_Voltage);
                    pucBuffer[15] = GetHighByte(C_Voltage);
                }
            }
        }
        //求取校验和
        for (i = 0; i < uc645DataDomainLength + 10; i++)
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[uc645DataDomainLength + 10] = ucCheckData;
    }
    return;
}
/********************************************************************
 * 功    能：回复电压帧组帧函数
 * 输    入：应用层缓存指针
 * 输    出：	true/false
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
bool blVoltage_Create(uint8_t *pucBuffer)
{
    uint8_t ucCheckData = 0;
    uint8_t i = 0;
    switch (pucBuffer[11])
    {
    case 0x32: //三相电压数据块
    {
        pucBuffer[8] = 0x91;
        pucBuffer[9] = 0x0A;
        //使用上次修改的合法电压值或1000 2017-10-25:
        A_Voltage = ((A_Voltage <= Voltage_MAX) && (A_Voltage >= Voltage_MIN) ? A_Voltage : WORK_VOLTAGE);
        pucBuffer[14] = GetLowByte(A_Voltage);
        pucBuffer[15] = GetHighByte(A_Voltage);
        B_Voltage = ((B_Voltage <= Voltage_MAX) && (B_Voltage >= Voltage_MIN) ? B_Voltage : WORK_VOLTAGE);
        pucBuffer[16] = GetLowByte(B_Voltage);
        pucBuffer[17] = GetHighByte(B_Voltage);
        C_Voltage = ((C_Voltage <= Voltage_MAX) && (C_Voltage >= Voltage_MIN) ? C_Voltage : WORK_VOLTAGE);
        pucBuffer[18] = GetLowByte(C_Voltage);
        pucBuffer[19] = GetHighByte(C_Voltage);
        for (i = 0; i < 20; i++) //固定帧长22
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[20] = ucCheckData;
        pucBuffer[21] = 0x16;
        return true;
    }
    case 0x34: //A相电压数据
    {
        A_Voltage = ((A_Voltage <= Voltage_MAX) && (A_Voltage >= Voltage_MIN) ? A_Voltage : WORK_VOLTAGE);
        pucBuffer[8] = 0x91;
        pucBuffer[9] = 0x06;
        pucBuffer[14] = GetLowByte(A_Voltage);
        pucBuffer[15] = GetHighByte(A_Voltage);
        for (i = 0; i < 16; i++) //固定帧长18
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[16] = ucCheckData;
        pucBuffer[17] = 0x16;
        return true;
    }
    case 0x35: //B相电压数据
    {
        B_Voltage = ((B_Voltage <= Voltage_MAX) && (B_Voltage >= Voltage_MIN) ? B_Voltage : WORK_VOLTAGE);
        pucBuffer[8] = 0x91;
        pucBuffer[9] = 0x06;
        pucBuffer[14] = GetLowByte(B_Voltage);
        pucBuffer[15] = GetHighByte(B_Voltage);
        for (i = 0; i < 16; i++) //固定帧长18
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[16] = ucCheckData;
        pucBuffer[17] = 0x16;
        return true;
    }
    case 0x36: //C相电压数据
    {
        C_Voltage = ((C_Voltage <= Voltage_MAX) && (C_Voltage >= Voltage_MIN) ? C_Voltage : WORK_VOLTAGE); //固定帧长18
        pucBuffer[8] = 0x91;
        pucBuffer[9] = 0x06;
        pucBuffer[14] = GetLowByte(C_Voltage);
        pucBuffer[15] = GetHighByte(C_Voltage);
        for (i = 0; i < 16; i++)
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[16] = ucCheckData;
        pucBuffer[17] = 0x16;
        return true;
    }
    default:
        return false; //如果收到的电压帧不是召测A/B/C相，则收帧错误，即使电表回复超时，也不能替代回复
    }
}
/********************************************************************
 * 功    能：BCD码校验
 * 输    入：电压数据的低字节和高字节
 * 输    出：true或false
 *           
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
bool BCD_Verity(uint8_t LowByte, uint8_t HighByte)
{
    uint8_t hundreds; //电压百位
    uint8_t tens;     //电压十位
    uint8_t ones;     //电压个位
    uint8_t decimals; //电压小数位
    hundreds = (HighByte & 0xF0) >> 4;
    tens = HighByte & 0x0F;
    ones = (LowByte & 0xF0) >> 4;
    decimals = LowByte & 0x0F;
    if ((hundreds >= 0x03) && (hundreds <= 0x0C) && (tens >= 0x03) && (tens <= 0x0C) && (ones >= 0x03) && (ones <= 0x0C) && (decimals >= 0x03) && (decimals <= 0x0C))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/********************************************************************
 * 功    能：从数据帧中读取电压值
 * 输    入：电压值低字节、高字节
 * 输    出：十进制的电压值
 *           
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
int Get_Voltage(uint8_t LowByte, uint8_t HighByte)
{
    int Voltage = 0;
    uint8_t hundreds; //电压百位
    uint8_t tens;     //电压十位
    uint8_t ones;     //电压个位
    uint8_t decimals; //电压小数位
    hundreds = ((HighByte - 0x33) & 0xF0) >> 4;
    tens = (HighByte - 0x33) & 0x0F;
    ones = ((LowByte - 0x33) & 0xF0) >> 4;
    decimals = (LowByte - 0x33) & 0x0F;
    Voltage = hundreds * 1000 + tens * 100 + ones * 10 + decimals; //为表示小数位，将数整体增大10倍
    return Voltage;
}

/********************************************************************
 * 功    能：从数据帧中读取电流值
 * 输    入：电流值低字节、中字节、高字节
 * 输    出：十进制的电流值
 * 要    求：
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
void vGet_Current(uint8_t LowByte, uint8_t MidByte, uint8_t HighByte, CURRENT *pcurrent)
{
    uint8_t million;
    uint8_t tenkillo;
    uint8_t killo;
    uint8_t hundreds;
    uint8_t tens;
    uint8_t ones;
    million = ((HighByte - 0x33) & 0x70) >> 4; //取bit7~bit5，最高位是符号位,忽略
    tenkillo = (HighByte - 0x33) & 0x0F;
    killo = ((MidByte - 0x33) & 0xF0) >> 4;
    hundreds = (MidByte - 0x33) & 0x0F;
    tens = ((LowByte - 0x33) & 0xF0) >> 4;
    ones = (LowByte - 0x33) & 0x0F;
    pcurrent->value = million * 100000 + tenkillo * 10000 + killo * 1000 + hundreds * 100 + tens * 10 + ones;
    pcurrent->count++;
    pcurrent->isminus = ((((HighByte - 0x33) & 0x80) != 0) ? true : false);
}
/********************************************************************
 * 功    能：将电流值写入帧字节
 * 输    入：电流值低字节、中字节、高字节，电流值
 * 输    出：
 * 要    求：电流值不为负值
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
void vPut_Current(CURRENT pcurrent, uint8_t *pLowByte, uint8_t *pMidByte, uint8_t *pHighByte)
{
    *pLowByte = 0x33 + ((uint8_t)(((pcurrent.value / 10) % 10) << 4) | (uint8_t)(pcurrent.value % 10));
    *pMidByte = 0x33 + ((uint8_t)(((pcurrent.value / 1000) % 10) << 4) | (uint8_t)((pcurrent.value / 100) % 10));
    *pHighByte = 0x33 + ((uint8_t)((pcurrent.value / 100000) << 4) | (uint8_t)((pcurrent.value / 10000) % 10));
    if (pcurrent.isminus == true)
    {
        *pHighByte = *pHighByte | 0x80;
    }
}

/********************************************************************
 * 功    能：电压修改初始化：
 * 输    入：	none
 * 输    出：	none
 * 编 写 人：王君龙
 * 编写日期：2017.10.25
 **********************************************************************/
void Voltage_Change_Init(void)
{
    uint8_t Vmax1;
    uint8_t Vmax2;
    uint8_t Vmin1;
    uint8_t Vmin2;
    /*电压修改使能位初始化*/
    Voltage_Change_State = EEProm_Read(FLASH_VChangeState_Addr); //从flash中读取电压修改使能字节
    /*电压修改上下限初始化*/
    Vmax1 = EEProm_Read(FLASH_VMAX1_Addr);
    Vmax2 = EEProm_Read(FLASH_VMAX2_Addr);
    Vmin1 = EEProm_Read(FLASH_VMIN1_Addr);
    Vmin2 = EEProm_Read(FLASH_VMIN2_Addr);

    if ((Vmax1 == 0xff) && (Vmax2 == 0xff) && (Vmin1 == 0xff) && (Vmin2 == 0xff))
    {
        Voltage_MAX = Voltage_MAX_Default; //设置电压合格上下限默认值
        Voltage_MIN = Voltage_MIN_Default;
    }
    else
    {
        Voltage_MAX = Get_Voltage(Vmax1, Vmax2); //flash中存储的是654对应帧字节，两个字节
        Voltage_MIN = Get_Voltage(Vmin1, Vmin2);
    }
}

/*****************************************************
 * 功    能：电压合格率调整策略
 * 输    入：
 * 输    出：
 * 说    明：电压偏小需调节时，以MIN为基准，将Voltage镜像，若镜像点超过MAX，依次减小有效区间值（MAX-MIN），直到Voltage处在此区间内;           
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 *******************************************************/
int Voltage_Modifier_Method(int Voltage)
{
    int n = 0;
    int m = 0;
    int i = 0;
    n = (Voltage_MIN - Voltage_MIN_MIN) / (Voltage_MAX - Voltage_MIN) + 1; //(198-120)/(235-198)+1或者(100-90)/(103-100)+1
    m = (Voltage_MAX_MAX - Voltage_MAX) / (Voltage_MAX - Voltage_MIN) + 1; //(260-235)/(235-198)+1或者(110-103)/(103-100)+1
    if ((Voltage > Voltage_MAX) && (Voltage <= Voltage_MAX_MAX))
    {
        if (Voltage <= (Voltage_MAX + (Voltage_MAX - Voltage_MIN)))
        {
            Voltage = Voltage - (Voltage - Voltage_MAX) * 2;
            return Voltage;
        }
        else
        {
            for (i = 1; i < m; i++)
            {
                if (Voltage <= (Voltage_MAX + (Voltage_MAX - Voltage_MIN) * (i + 1)))
                {
                    Voltage = Voltage - (Voltage_MAX - Voltage_MIN) * (i + 1);
                    return Voltage;
                }
            }
        }
    }
    if ((Voltage < Voltage_MIN) && (Voltage >= Voltage_MIN_MIN))
    {
        if (Voltage >= (Voltage_MIN - (Voltage_MAX - Voltage_MIN)))
        {
            Voltage = Voltage + (Voltage_MIN - Voltage) * 2;
            return Voltage;
        }
        else
        {
            for (i = 1; i < n; i++)
            {
                if (Voltage >= (Voltage_MIN - (Voltage_MAX - Voltage_MIN) * (i + 1)))
                {
                    Voltage = Voltage + (Voltage_MAX - Voltage_MIN) * (i + 1);
                    return Voltage;
                }
            }
        }
    }
    return Voltage;
}
/*****************************************************
 * 功    能：修改电表状态字中的电流反向、电流短路、电流断路状态字
 * 输    入：应用层缓存指针
 * 输    出：
 *           
 * 编 写 人：王君龙
 * 编写日期：2017.5.23
 *******************************************************/
void Estatus_Modify_CurrentEvent(uint8_t *pucBuffer)
{
    uint16_t estatus = 0x0000;
    int i = 0;
    uint8_t ucCheckData = 0x0;
    switch (pucBuffer[10])
    {
    case 0x37: //状态字4,A相状态字
    {
        //BIT4,BIT6,bit8 置0->11111110 10101111:
        estatus = 0xFEAF & Get_Estatus(pucBuffer[14], pucBuffer[15]);
        pucBuffer[14] = (uint8_t)(estatus & 0x00FF) + 0X33;        //状态字低字节
        pucBuffer[15] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33; //状态字高字节
        break;
    }
    case 0x38: //状态字5,B相状态字
    {
        estatus = 0xFEAF & Get_Estatus(pucBuffer[14], pucBuffer[15]); //[14][15]转换为二进制
        pucBuffer[14] = (uint8_t)(estatus & 0x00FF) + 0X33;           //状态字低字节
        pucBuffer[15] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33;    //状态字高字节
        break;
    }
    case 0x39: //状态字6,C相状态字
    {
        estatus = 0xFEAF & Get_Estatus(pucBuffer[14], pucBuffer[15]); //[14][15]转换为二进制
        pucBuffer[14] = (uint8_t)(estatus & 0x00FF) + 0X33;           //状态字低字节
        pucBuffer[15] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33;    //状态字高字节
        break;
    }
    case 0x3A: //状态字7，合相状态字
    {
        estatus = 0xFFF7 & Get_Estatus(pucBuffer[14], pucBuffer[15]); //BIT3=0   11111111 11110111
        pucBuffer[14] = (uint8_t)(estatus & 0x00FF) + 0X33;           //状态字低字节
        pucBuffer[15] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33;    //状态字高字节
        break;
    }
    case 0x32: //状态字数据块
    {
        //1:[14][15] 2:[16][17] 3:[18][19] 4:[20][21] 5:[22][23] 6:[24][25] 7:[26][27]
        //状态字4,A相状态字
        estatus = 0xFEAF & Get_Estatus(pucBuffer[20], pucBuffer[21]); //BIT4,BIT6,bit8 =0->11111110 10101111
        pucBuffer[20] = (uint8_t)(estatus & 0x00FF) + 0X33;           //状态字低字节
        pucBuffer[21] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33;    //状态字高字节
        //状态字5,B相状态字
        estatus = 0xFEAF & Get_Estatus(pucBuffer[22], pucBuffer[23]);
        pucBuffer[22] = (uint8_t)(estatus & 0x00FF) + 0X33;        //状态字低字节
        pucBuffer[23] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33; //状态字高字节
        //状态字6,C相状态字
        estatus = 0xFEAF & Get_Estatus(pucBuffer[24], pucBuffer[25]);
        pucBuffer[24] = (uint8_t)(estatus & 0x00FF) + 0X33;        //状态字低字节
        pucBuffer[25] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33; //状态字高字节
        //状态字7，合相状态字
        estatus = 0xFFF7 & Get_Estatus(pucBuffer[26], pucBuffer[27]);
        pucBuffer[26] = (uint8_t)(estatus & 0x00FF) + 0X33;        //状态字低字节
        pucBuffer[27] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33; //状态字高字节
        break;
    }
    default:
        break;
    }
    for (i = 0; i < uc645DataDomainLength + 10; i++)
    {
        ucCheckData += pucBuffer[i];
    }
    pucBuffer[uc645DataDomainLength + 10] = ucCheckData;
}

/*****************************************************
 * 功    能：单相电流修改方法
 * 输    入：两相参考电流，一相需修改电流
 * 输    出：电流不需要修改，输出false
 *           电流已修改，输出true
 * 说	  明：
 * 编 写 人：王君龙
 * 编写日期：2017.12.13
 *******************************************************/
bool blCurrentSingle_Modify_Method(CURRENT *ref1, CURRENT *ref2, CURRENT *result) //注释中根据B相和C相电流修改A相电流
{
    //if Acurrent->count>Bcurrent->count && Acurrent->count>Ccurrent->count,先A，不修改
    //if Bcurrent->count=Acurrent->count && Acurrent->count>Ccurrent->count,先B，后A，根据B修改A
    //if Ccurrent->count=Acurrent->count && Acurrent->count>Bcurrent->count,先C，后A，根据C修改A
    //if Bcurrent->count=Ccurrent->count && Acurrent->count<Ccurrent->count,先BC，后A，根据BC修改A
    if ((result->count > ref1->count) &&
        (result->count > ref2->count)) //先result相，不修改
        return false;
    else if ((result->count == ref1->count) &&
             (result->count > ref2->count)) //先ref1相，后result相,根据ref1修改result
    {
        if (ref1->value == 0 || result->value == 0 || ref1->isminus || result->isminus)
            return false; //已经收到的两相电流中，有0值，或者有负值，不做修改
        if (result->value < ref1->value)
        {
            //若result小，result修改为ref1的固定比例：CURRENT_MAX_DVALUE/100
            result->value = ((result->value < (CURRENT_MAX_DVALUE * ref1->value / 100))
                                 ? CURRENT_MAX_DVALUE * ref1->value / 100
                                 : result->value);
        }
        else
        {
            //若result大，result修改为ref1的固定比例：100/CURRENT_MAX_DVALUE；不超过范围或者ref1等于0时，不修改
            result->value = ((CURRENT_MAX_DVALUE * result->value / 100 > ref1->value)
                                 ? 100 * ref1->value / CURRENT_MAX_DVALUE
                                 : result->value);
        }
        return true;
    }
    else if ((result->count == ref2->count) && (result->count > ref1->count)) //先ref2相，后result相，根据ref2修改result
    {
        if (ref2->value == 0 || result->value == 0 || ref2->isminus || result->isminus)
            return false;
        if (result->value < ref2->value)
        {
            result->value = ((result->value < (CURRENT_MAX_DVALUE * ref2->value / 100))
                                 ? CURRENT_MAX_DVALUE * ref2->value / 100
                                 : result->value);
        }
        else
        {
            result->value = ((CURRENT_MAX_DVALUE * result->value / 100 > ref2->value)
                                 ? 100 * ref2->value / CURRENT_MAX_DVALUE
                                 : result->value);
        }
        return true;
    }
    else if ((result->count == ref1->count) && (result->count == ref2->count)) //前两相是ref1和ref2，最后是result相
    {
        if ((ref1->value == 0) || (ref2->value == 0) || (result->value == 0) || ref1->isminus || ref2->isminus || result->isminus)
            return false;
        if (result->value > MAX(ref1->value, ref2->value)) //result比前两相都大，result修改为较小值的固定比例
        {
            result->value = (CURRENT_MAX_DVALUE * result->value / 100 >= MIN(ref1->value, ref2->value)
                                 ? 100 * MIN(ref1->value, ref2->value) / CURRENT_MAX_DVALUE
                                 : result->value);
        }
        else if (result->value < MIN(ref1->value, ref2->value)) //result比前两相都小，result修改为较大值的固定比例
        {
            result->value = ((result->value < CURRENT_MAX_DVALUE * MAX(ref1->value, ref2->value) / 100)
                                 ? CURRENT_MAX_DVALUE * MAX(ref1->value, ref2->value) / 100
                                 : result->value);
        }
        else //result在ref1和ref2之间，因为前两帧差值已保证不超出范围，所以result不需要修改
        {
            return false;
        }
        return true;
    }
    return true;
}

/*****************************************************
 * 功    能：三相电流修改方法
 * 输    入：三相参考电流
 * 输    出：无
 * 说	  明：最大电流值改为平均值的1.1倍，中间值改为平均值，最小值改为平均值的90%，
 *				此时最大电流值和最电流值的差值是最大值的18.18%
 *				
 * 编 写 人：王君龙
 * 编写日期：2017.12.13
 *******************************************************/
void vCurrentTriple_Modify_Method(CURRENT *current_max, CURRENT *current_mid, CURRENT *current_min)
{
    uint32_t current_average;
    current_average = (current_max->value + current_mid->value + current_min->value) / 3;
    current_max->value = current_average * 11 / 10; //最大值改为平均值的110%
    current_mid->value = current_average;           //中间值改为平均值
    current_min->value = current_average * 9 / 10;  //最小值改为平均值的90%；
}

/*****************************************************
 * 功    能：修改电流各相差值
 * 输    入：应用层缓存指针
 * 输    出：无
 * 说	  明：单相电流：若任意两相差值大于20%的较大值，则将较小值改为较大值的80%          
 *				三相电流：按照总和不变来修改
 * 编 写 人：王君龙
 * 编写日期：2017.12.13
 *******************************************************/
void vCurrent_Modify(uint8_t *pucBuffer)
{
    uint8_t i;
    uint8_t ucCheckData = 0;
    switch (pucBuffer[11])
    {
    case 0x32: //三相电流数据块
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Acurrent);
        /*电流为负值,不修改电流*/
        if (Acurrent.isminus || Bcurrent.isminus || Ccurrent.isminus)
            return;
        /*电流为0，不修改电流*/
        if ((Acurrent.value == 0) || (Bcurrent.value == 0) || (Ccurrent.value == 0))
            return;
        /*电流相互之间不超差，不修改电流*/
        if (CURRENT_MAX_DVALUE * MAX(Acurrent.value, MAX(Bcurrent.value, Ccurrent.value)) / 100 <=
            MIN(Acurrent.value, MIN(Bcurrent.value, Ccurrent.value)))
            return;
        else //需要修改，最大值减小，最小值增大，剩下的是中间值
        {
            if ((Acurrent.value >= Bcurrent.value) && (Bcurrent.value >= Ccurrent.value))
                vCurrentTriple_Modify_Method(&Acurrent, &Bcurrent, &Ccurrent);
            else if ((Acurrent.value >= Ccurrent.value) && (Ccurrent.value >= Bcurrent.value))
                vCurrentTriple_Modify_Method(&Acurrent, &Ccurrent, &Bcurrent);
            else if ((Bcurrent.value >= Acurrent.value) && (Acurrent.value >= Ccurrent.value))
                vCurrentTriple_Modify_Method(&Bcurrent, &Acurrent, &Ccurrent);
            else if ((Bcurrent.value >= Ccurrent.value) && (Ccurrent.value >= Acurrent.value))
                vCurrentTriple_Modify_Method(&Bcurrent, &Ccurrent, &Acurrent);
            else if ((Ccurrent.value >= Acurrent.value) && (Acurrent.value >= Bcurrent.value))
                vCurrentTriple_Modify_Method(&Ccurrent, &Acurrent, &Bcurrent);
            else if ((Ccurrent.value >= Bcurrent.value) && (Bcurrent.value >= Acurrent.value))
                vCurrentTriple_Modify_Method(&Ccurrent, &Bcurrent, &Acurrent);
            //A相电流,十进制数转BCD码
            vPut_Current(Acurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
            //B相电流
            vPut_Current(Acurrent, &pucBuffer[17], &pucBuffer[18], &pucBuffer[19]);
            //C相电流
            vPut_Current(Acurrent, &pucBuffer[20], &pucBuffer[21], &pucBuffer[22]);
            for (i = 0; i < 23; i++) //数据域长度23
            {
                ucCheckData += pucBuffer[i];
            }
            pucBuffer[23] = ucCheckData;
            pucBuffer[24] = 0x16;
        }
        break;
    }
    case 0x34: //A相电流
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Acurrent);
        if (((pucBuffer[16] - 0x33) & 0x80) != 0)
        {
            return;
        }
        if (blCurrentSingle_Modify_Method(&Bcurrent, &Ccurrent, &Acurrent)) //根据B相和C相电流修改A相
        {
            //A相电流,十进制数转BCD码
            vPut_Current(Acurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
            for (i = 0; i < 17; i++) //数据域长度17
            {
                ucCheckData += pucBuffer[i];
            }
            pucBuffer[17] = ucCheckData;
            pucBuffer[18] = 0x16;
        }
        break;
    }
    case 0x35: //B相电流
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Bcurrent);

        if (((pucBuffer[16] - 0x33) & 0x80) != 0) //电流是负值
        {
            return;
        }
        if (blCurrentSingle_Modify_Method(&Acurrent, &Ccurrent, &Bcurrent)) //根据A相和C相修改B相电流
        {
            //B相电流,十进制数转BCD码
            vPut_Current(Bcurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
            for (i = 0; i < 17; i++) //数据域长度17
            {
                ucCheckData += pucBuffer[i];
            }
            pucBuffer[17] = ucCheckData;
            pucBuffer[18] = 0x16;
        }
        break;
    }
    case 0x36: //C相电流
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Ccurrent);

        if (((pucBuffer[16] - 0x33) & 0x80) != 0)
        {
            return;
        }
        if (blCurrentSingle_Modify_Method(&Acurrent, &Bcurrent, &Ccurrent)) //根据A相和B相修改C相电流
        {
            //B相电流,十进制数转BCD码
            vPut_Current(Ccurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
            for (i = 0; i < 17; i++) //数据域长度16
            {
                ucCheckData += pucBuffer[i];
            }
            pucBuffer[17] = ucCheckData;
            pucBuffer[18] = 0x16;
        }
        break;
    }
    }
}

/********************************************************************
 * 功    能：电流重过载参数初始化
 * 输    入：电流帧数组指针
 * 输    出：
 *           
 * 编 写 人：王君龙
 * 编写日期：2018年8月13日16:13:30
 **********************************************************************/
void vCurrent_Limit_DefaultSet(void)
{
    uint32_t uwLimit;
    uint32_t uwCTratio;
/*变压器容量和额定电流出厂固化，不允许修改*/
#ifdef TRANSFORMER_CAPACITY_50KVA
    TransFormer.capacity = 50;
    TransFormer.rated_current = 72000;
#endif

#ifdef TRANSFORMER_CAPACITY_100KVA
    TransFormer.capacity = 100;
    TransFormer.rated_current = 144000;
#endif

#ifdef TRANSFORMER_CAPACITY_200KVA
    TransFormer.capacity = 200;
    TransFormer.rated_current = 288000;
#endif

#ifdef TRANSFORMER_CAPACITY_400KVA
    TransFormer.capacity = 400;
    TransFormer.rated_current = 577000;
#endif
    /*从FLASH读取变压器二次侧CT变比*/
    uwCTratio = Flash_Read_Word(FLASH_CURRENT_LIMIT_RATIO_ADDR);
    /*变比无效 ，则为初次上电*/
    if (uwCTratio == 0xFFFFFFFF)
    {
        /*CT设置为80*/
        TransFormer.CTratio = 80;
        /*存储CT值*/
        Flash_Write_Word(FLASH_CURRENT_LIMIT_RATIO_ADDR, TransFormer.CTratio);
    }
    else
    {
        /*数据有效，初始化全局变量*/
        TransFormer.CTratio = uwCTratio;
    }
    /*读取电流上限参数*/
    uwLimit = Flash_Read_Word(FLASH_CURRENT_LIMIT_VALUE_ADDR);
    /*参数无效，初次上电*/
    if (uwLimit == 0xFFFFFFFF)
    {
        /*根据额定电流和CT变比计算电流上限值*/
        TransFormer.CT2_CurrentLimit = (TransFormer.rated_current / TransFormer.CTratio) * 3 * 8 / 10;
        /*存储电流上限值*/
        Flash_Write_Word(FLASH_CURRENT_LIMIT_VALUE_ADDR, TransFormer.CT2_CurrentLimit);
    }
    else
    {
        /*参数有效，初始化全局变量*/
        TransFormer.CT2_CurrentLimit = uwLimit;
    }
}

/********************************************************************
 * 功    能：各相电流值上限修改函数
 * 输    入：电流帧数组指针
 * 输    出：
 *           
 * 编 写 人：王君龙
 * 编写日期：2018.1.30
 **********************************************************************/
void vCurrent_Limit_Modify(uint8_t *pucBuffer)
{
    uint32_t ABCcurrent;
    uint8_t i;
    uint8_t ucCheckData = 0;
#ifndef ENABLE_CURRENT_LIMIT
    return;
#endif

    switch (pucBuffer[11])
    {
    case 0x32: //三相电流数据块
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Acurrent);
        vGet_Current(pucBuffer[17], pucBuffer[18], pucBuffer[19], &Bcurrent);
        vGet_Current(pucBuffer[20], pucBuffer[21], pucBuffer[22], &Ccurrent);
        if (Acurrent.isminus || Bcurrent.isminus || Ccurrent.isminus)
            return;
        ABCcurrent = Acurrent.value + Bcurrent.value + Ccurrent.value;
        if (ABCcurrent > TransFormer.CT2_CurrentLimit)
        {
            Acurrent.value = TransFormer.CT2_CurrentLimit * Acurrent.value / ABCcurrent;
            Bcurrent.value = TransFormer.CT2_CurrentLimit * Bcurrent.value / ABCcurrent;
            Ccurrent.value = TransFormer.CT2_CurrentLimit * Ccurrent.value / ABCcurrent;
        }
        Acurrent.value = (Acurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Acurrent.value);
        Bcurrent.value = (Bcurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Bcurrent.value);
        Ccurrent.value = (Ccurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Ccurrent.value);
        //A相电流,十进制数转BCD码
        vPut_Current(Acurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
        //B相电流
        vPut_Current(Bcurrent, &pucBuffer[17], &pucBuffer[18], &pucBuffer[19]);
        //C相电流
        vPut_Current(Ccurrent, &pucBuffer[20], &pucBuffer[21], &pucBuffer[22]);
        for (i = 0; i < 23; i++) //数据域长度23
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[23] = ucCheckData;
        pucBuffer[24] = 0x16;
        break;
    }
    case 0x34: //A相电流
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Acurrent);
        Acurrent.value = (Acurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Acurrent.value);
        vPut_Current(Acurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
        for (i = 0; i < 17; i++) //数据域长度17
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[17] = ucCheckData;
        pucBuffer[18] = 0x16;
        break;
    }
    case 0x35: //B相电流
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Bcurrent);
        Bcurrent.value = (Bcurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Bcurrent.value);
        vPut_Current(Bcurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
        for (i = 0; i < 17; i++) //数据域长度17
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[17] = ucCheckData;
        pucBuffer[18] = 0x16;

        break;
    }
    case 0x36: //C相电流
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Ccurrent);
        Ccurrent.value = (Ccurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Ccurrent.value);
        vPut_Current(Bcurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
        for (i = 0; i < 17; i++) //数据域长度17
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[17] = ucCheckData;
        pucBuffer[18] = 0x16;
        break;
    }
    }
}

/*****************************************************
 * 功    能：修改电流方向帧
 * 输    入：应用层缓存指针
 * 输    出：
 *           
 * 编 写 人：王君龙
 * 编写日期：2017.12.13
 *******************************************************/
void vCurrentDirect_Modify(uint8_t *pucBuffer)
{
    uint8_t i = 0;
    uint8_t ucCheckData;
    switch (pucBuffer[11])
    {
    case 0x32: //三相电流数据块
    {
        pucBuffer[16] = (pucBuffer[16] - 0x33) & 0xEF + 0x33; //最高bit位为1，表示为负值，将最高位屏蔽为1
        pucBuffer[19] = (pucBuffer[19] - 0x33) & 0xEF + 0x33; //最高bit位为1，表示为负值，将最高位屏蔽为1
        pucBuffer[22] = (pucBuffer[22] - 0x33) & 0xEF + 0x33; //最高bit位为1，表示为负值，将最高位屏蔽为1
        for (i = 0; i < 23; i++)                              //数据域长度23
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[23] = ucCheckData;
        pucBuffer[24] = 0x16;
        break;
    }
    case 0x34: //A相电流
    {
        pucBuffer[16] = (pucBuffer[16] - 0x33) & 0xEF + 0x33; //最高bit位为1，表示为负值，将最高位屏蔽为1
        for (i = 0; i < 17; i++)                              //数据域长度17
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[17] = ucCheckData;
        pucBuffer[18] = 0x16;
        break;
    }
    case 0x35: //B相电流
    {
        pucBuffer[16] = (pucBuffer[16] - 0x33) & 0xEF + 0x33; //最高bit位为1，表示为负值，将最高位屏蔽为1
        for (i = 0; i < 17; i++)                              //数据域长度17
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[17] = ucCheckData;
        pucBuffer[18] = 0x16;
        break;
    }
    case 0x36: //C相电流
    {
        pucBuffer[16] = (pucBuffer[16] - 0x33) & 0xEF + 0x33; //最高bit位为1，表示为负值，将最高位屏蔽为1
        for (i = 0; i < 17; i++)                              //数据域长度16
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[17] = ucCheckData;
        pucBuffer[18] = 0x16;
        break;
    }
    }
}

/********************************************************************
 * 功    能：电流重过载参数：电流上限值设置
 * 输    入：维护帧数组指针
 * 输    出：
 *           
 * 编 写 人：王君龙
 * 编写日期：2018年8月13日09:39:18
 * 接受：   68 AA AA AA AA AA AA 68 14 07 33 37 35 35 56 34 12 XX 16(123.456A)
 *成功回复：68 AA AA AA AA AA AA 68 94 00 XX 16
 *失败回复：68 AA AA AA AA AA AA 68 D4 00 XX 16
 **********************************************************************/
void vCurrent_Limit_Set(uint8_t *pucBuffer)
{
    uint8_t i;
    CURRENT limit;
    pucBuffer[8] = 0xD4;
    /*非重过载调整版本时，回复否认帧*/
#ifdef ENABLE_CURRENT_LIMIT
    /*输入的字节减去33后不是BCD码怎么办？*/
    vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &limit);
    if (!limit.isminus && (limit.value > 0) && (limit.value <= 999999))
    {
        if (SUCCESS == Flash_Write_Word(FLASH_CURRENT_LIMIT_VALUE_ADDR, limit.value))
        {
            TransFormer.CT2_CurrentLimit = limit.value;
            //limit.value = TransFormer.rated_current / TransFormer.CTratio;
            //limit.value = limit.value * 8 / 10;
            //limit.value = limit.value * 3;
            TransFormer.CTratio = TransFormer.rated_current /
                                  (TransFormer.CT2_CurrentLimit / 3 * 10 / 8);
            if (SUCCESS == Flash_Write_Word(FLASH_CURRENT_LIMIT_RATIO_ADDR, TransFormer.CTratio))
            {
                pucBuffer[8] = 0x94;
            }
        }
    }
#endif

    pucBuffer[9] = 0x00;
    pucBuffer[10] = 0;
    for (i = 0; i < 10; i++)
    {
        pucBuffer[10] += pucBuffer[i];
    }
    pucBuffer[11] = 0x16;
    /*电表口回复*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}

/********************************************************************
 * 功    能：电流重过载参数：CT变比设置
 * 输    入：维护帧数组指针
 * 输    出：
 *           
 * 编 写 人：王君龙
 * 编写日期：2018年8月13日09:39:18
 * 接受：   68 AA AA AA AA AA AA 68 14 06 33 39 35 35 00 01 XX 16(变比100)
 *成功回复：68 AA AA AA AA AA AA 68 94 00 XX 16
 *失败回复：68 AA AA AA AA AA AA 68 D4 00 XX 16
 **********************************************************************/
void vCurrent_CTRatio_Set(uint8_t *pucBuffer)
{
    uint8_t i;
    uint32_t uwdata = 0;
    pucBuffer[8] = 0xD4;
/*输入的字节减去33后不是BCD码怎么办？*/
/*不支持重过载修改功能时，回复否认帧*/
#ifdef ENABLE_CURRENT_LIMIT
    uwdata = Get_Voltage(pucBuffer[14], pucBuffer[15]);
    if ((uwdata > 0) && (uwdata <= 9999))
    {
        if (SUCCESS == Flash_Write_Word(FLASH_CURRENT_LIMIT_RATIO_ADDR, uwdata))
        {
            TransFormer.CTratio = uwdata;
            TransFormer.CT2_CurrentLimit = (TransFormer.rated_current / TransFormer.CTratio) * 24 / 10;
            if (SUCCESS == Flash_Write_Word(FLASH_CURRENT_LIMIT_VALUE_ADDR, TransFormer.CT2_CurrentLimit))
            {
                pucBuffer[8] = 0x94;
            }
        }
    }
#endif
    pucBuffer[9] = 0x00;
    pucBuffer[10] = 0;
    for (i = 0; i < 10; i++)
    {
        pucBuffer[10] += pucBuffer[i];
    }
    pucBuffer[11] = 0x16;
    /*电表口回复*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}

/********************************************************************
 * 功    能：电流重过载参数查询：电流上限值
 * 输    入：维护帧数组指针
 * 输    出：
 *           
 * 编 写 人：王君龙
 * 编写日期：2018年8月13日09:39:18
 * 接收：   68 AA AA AA AA AA AA 68 11 04 33 38 35 35 XX 16
 *成功回复：68 AA AA AA AA AA AA 68 91 07 33 38 35 35 56 34 12 XX 16(123.456)
 **********************************************************************/
void vCurrent_Limit_ReadSet(uint8_t *pucBuffer)
{
    uint8_t i;
    CURRENT limit;
    limit.value = TransFormer.CT2_CurrentLimit;
    limit.isminus = false;
    vPut_Current(limit, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
    pucBuffer[8] = 0x91;
    pucBuffer[9] = 0x07;
    pucBuffer[17] = 0;
    for (i = 0; i < 17; i++)
    {
        pucBuffer[17] += pucBuffer[i];
    }
    pucBuffer[18] = 0x16;
    /*电表口回复*/
    uc645DataDomainLength = 7;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 19);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 19);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}

/********************************************************************
 * 功    能：电流重过载参数查询：CT变比
 * 输    入：维护帧数组指针
 * 输    出：
 *           
 * 编 写 人：王君龙
 * 编写日期：2018年8月13日09:39:18
 * 接收：   68 AA AA AA AA AA AA 68 11 04 33 3A 35 35 XX 16
 *成功回复：68 AA AA AA AA AA AA 68 91 06 33 3A 35 35 00 01 XX 16(变比100)
 **********************************************************************/
void vCurrent_CTRatio_ReadSet(uint8_t *pucBuffer)
{

    uint8_t i;
    pucBuffer[8] = 0x91;
    pucBuffer[9] = 0x06;
    pucBuffer[14] = GetLowByte(TransFormer.CTratio);
    pucBuffer[15] = GetHighByte(TransFormer.CTratio);
    pucBuffer[16] = 0;
    for (i = 0; i < 16; i++)
    {
        pucBuffer[16] += pucBuffer[i];
    }
    pucBuffer[17] = 0x16;
    /*电表口回复*/
    uc645DataDomainLength = 6;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}

/********************************************************************
 * 功    能：电流重过载参数查询：变压器容量
 * 输    入：维护帧数组指针
 * 输    出：
 *           
 * 编 写 人：王君龙
 * 编写日期：2018年8月13日09:39:18
 * 接收：   68 AA AA AA AA AA AA 68 11 04 33 3B 35 35 XX 16
 *成功回复：68 AA AA AA AA AA AA 68 91 06 33 3B 35 35 00 01 XX 16(变比100)
 **********************************************************************/
void vCurrent_TransFormer_Capacity_ReadSet(uint8_t *pucBuffer)
{
    uint8_t i;
    pucBuffer[8] = 0x91;
    pucBuffer[9] = 0x06;
    pucBuffer[14] = GetLowByte(TransFormer.capacity);
    pucBuffer[15] = GetHighByte(TransFormer.capacity);
    pucBuffer[16] = 0;
    for (i = 0; i < 16; i++)
    {
        pucBuffer[16] += pucBuffer[i];
    }
    pucBuffer[17] = 0x16;
    /*电表口回复*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}

/*****************************************************
 * 功    能：电流帧计数清零
 * 输    入：
 * 输    出：无
 * 说	  明：收到电压回复帧后，将电流帧计数清零
 * 编 写 人：王君龙
 * 编写日期：2017.12.13
 *******************************************************/
void vCurrent_Count_Clear(void)
{
    Acurrent.count = 0;
    Bcurrent.count = 0;
    Ccurrent.count = 0;
}
/********************************************************************
 * 功    能：从数据帧中读取状态字
 * 输    入：状态字低字节、高字节
 * 输    出：二进制的状态字
 *           
 * 编 写 人：王君龙
 * 编写日期：2017.5.23
 **********************************************************************/
uint16_t Get_Estatus(uint8_t LowByte, uint8_t HighByte)
{
    uint16_t estatus;
    estatus = ((HighByte - 0x33) & 0x00FF) << 8; //00010000 00000000
    estatus |= (LowByte - 0X33) & 0X00FF;
    return estatus;
}
/********************************************************************
 * 功    能：将电压个位转换为低字节
 * 输    入：十进制电压值
 * 输    出：	BCD码
 *           
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
uint8_t GetLowByte(int Voltage)
{
    uint8_t LowByte = 0x00;
    LowByte = (((((Voltage % 1000) % 100) / 10) << 4) & 0xF0) + ((((Voltage % 1000) % 100) % 10) & 0x0F) + 0x33;
    return LowByte;
}

/********************************************************************
 * 功    能：将电压百位、十位转换为低字节
 * 输    入：十进制电压值
 * 输    出：	BCD码
 *           
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
uint8_t GetHighByte(int Voltage)
{
    uint8_t HighByte = 0x00;
    HighByte = (((uint8_t)(Voltage / 1000) << 4) & 0xF0) + ((uint8_t)((Voltage % 1000) / 100) & 0x0F) + 0x33;
    return HighByte;
}

/********************************************************************
 * 功    能：日冻结数据修改功能初始化：默认修改；
 * 输    入：
 * 输    出：
 *
 * 编 写 人：王君龙
 * 编写日期：2017.9.6
 **********************************************************************/
//void vDayfrozen_Change_Init(void)
void vEnergy_Modify_Init(void)
{
    uint8_t tmp;
    /*日冻结修改使能位初始化*/
    Dayfrozen_Change_State = EEProm_Read(FLASH_DAYFROZENCHANGESTATE_ADDR);
    tmp = EEProm_Read(FLASH_DAYFROZEN_MODIFY_RATIO_ADDR);
    /*判断flash中是否已存储电压修改比例*/
    if (tmp == 0xFF)
    {
        /*使用默认修改比例*/
        ucDayfrozen_Modify_Ratio = UCDAYFROZEN_CHANGE_DEFAULE_RATIO;
    }
    else
    {
        ucDayfrozen_Modify_Ratio = tmp;
    }
}

/********************************************************************
 * 功    能：从数据帧中读取电能量值
 * 输    入：电能量值从低到高四个字节
 * 输    出：十进制的电能量值
 * 要    求：输出值扩大100倍
 * 编 写 人：王君龙
 * 编写日期：2016.6.23
 **********************************************************************/
uint32_t Get_Energy(uint8_t FstByte, uint8_t ScdByte, uint8_t TirdByte, uint8_t ForthByte)
{
    uint32_t Energy = 0;
    Energy = BCD2DEC(ForthByte - 0x33) * 1000000 +
             BCD2DEC(TirdByte - 0x33) * 10000 +
             BCD2DEC(ScdByte - 0x33) * 100 +
             BCD2DEC(FstByte - 0x33);
    return Energy;
}

/********************************************************************
 * 功    能：将电能量值转化为645帧字节
 * 输    入：电能量值，四字节按传输顺序输入
 * 输    出：none
 * 要    求：输出值扩大100倍
 * 编 写 人：王君龙
 * 编写日期：2018.5.10
 **********************************************************************/
void Fill_Energy(uint32_t energy, uint8_t *pFstByte, uint8_t *pScdByte, uint8_t *pTirdByte, uint8_t *pForthByte)
{
    *pFstByte = DEC2BCD(energy % 100) + 0x33;
    *pScdByte = DEC2BCD(energy / 100 % 100) + 0x33;
    *pTirdByte = DEC2BCD(energy / 10000 % 100) + 0x33;
    *pForthByte = DEC2BCD(energy / 1000000 % 100) + 0x33;
}

/********************************************************************
 * 功    能：当前/日冻结电能量调整策略
 * 输    入：
 * 输    出：
 *           
 * 编 写 人：王君龙
 * 编写日期：2018.5.8
 **********************************************************************/
uint32_t vNow_Frozen_Modify_Mothod(uint32_t nowenergy, uint32_t RealValueAddr, uint32_t ModifiedValueAddr)
{
    uint32_t RealValue;
    uint32_t Modified_Value;
    uint64_t i;
    /*读取存储的上次真实值和修改值*/
    RealValue = Flash_Read_Word(RealValueAddr);
    Modified_Value = Flash_Read_Word(ModifiedValueAddr);
    /*终端第一次使用*/
    if ((RealValue == 0XFFFFFFFF) || (Modified_Value == 0XFFFFFFFF))
    {
        /*上送值存储为真实值和修改值*/
        Flash_Write_Word(RealValueAddr, nowenergy);
        Flash_Write_Word(ModifiedValueAddr, nowenergy);
        /*不修改*/
        return nowenergy;
    }
    else
    {
        /*计算真实的增量，打折后累加到上次的修改值*/
        /*电表值溢出后归零*/
        if (nowenergy < RealValue)
        {
            //i= ((uint64_t)(nowenergy+ 99999999- RealValue)* ucDayfrozen_Modify_Ratio)/100;
            /*2018-6-27 11:04:38：增加四舍五入，单次增加0.05kwh以下时，不修改*/
            i = ((uint64_t)(nowenergy + 99999999 - RealValue) * ucDayfrozen_Modify_Ratio + 50) / 100;
        }
        else
        {
            i = ((uint64_t)(nowenergy - RealValue) * ucDayfrozen_Modify_Ratio + 50) / 100;
        }
        Modified_Value += (uint32_t)i;
        /*累计值溢出后归零*/
        Modified_Value = (Modified_Value > 99999999 ? Modified_Value - 99999999 : Modified_Value);
        /*将上送值存储为真实值*/
        Flash_Write_Word(RealValueAddr, nowenergy);
        /*新的修改值覆盖旧值存储*/
        Flash_Write_Word(ModifiedValueAddr, Modified_Value);
        /*返回修改值*/
        return Modified_Value;
    }
}

/********************************************************************
 * 功    能：当前电量数据修改函数
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 编写日期：2018.5.9
 case ID_00010000:	//当前正向有功总电能
 case ID_00010100:	//当前正向有功费率1电能
 case ID_00010200:	//当前正向有功费率2电能
 case ID_00010300:	//当前正向有功费率3电能
 case ID_00010400:	//当前正向有功费率4电能
 case ID_0001FF00:	//当前正向有功电能数据块
 case ID_00150000:	//当前A相正向有功
 case ID_00290000:	//当前B相正向有功
 case ID_003D0000:	//当前C相正向有功
 **********************************************************************/
void vNowEnergy_Modifier(uint8_t *pucBuffer)
{
    uint16_t i;
    uint32_t energy;    //数据域的第一个电量值
    uint32_t energy_F1; //费率1电量专用变量
    uint32_t energy_F2; //费率2电量专用变量
    uint32_t energy_F3; //费率3电量专用变量
    uint32_t energy_F4; //费率4电量专用变量
    if (Dayfrozen_Change_State == 0x00)
        return;
    energy = Get_Energy(pucBuffer[14], pucBuffer[15], pucBuffer[16], pucBuffer[17]);
    switch (pucBuffer[11])
    {
    case 0x33:
    {
        switch (pucBuffer[12])
        {
        /*正向有功总*/
        case 0x34:
        {
            energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_ALL_NOWENERGY_ADDR,
                                               FLASH_MODIFIED_ALL_NOWENERGY_ADDR);
            break;
        }
        /*A相正向有功*/
        case 0x48:
        {
            energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_A_NOWENERGY_ADDR,
                                               FLASH_MODIFIED_A_NOWENERGY_ADDR);
            break;
        }
        /*B相正向有功*/
        case 0x5c:
        {
            energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_B_NOWENERGY_ADDR,
                                               FLASH_MODIFIED_B_NOWENERGY_ADDR);
            break;
        }
        /*C相正向有功*/
        case 0x70:
        {
            energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_C_NOWENERGY_ADDR,
                                               FLASH_MODIFIED_C_NOWENERGY_ADDR);
            break;
        }
        default:
            break;
        }
        break;
    }
    /*正向有功总费率1*/
    case 0x34:
    {
        energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_F1_NOWENERGY_ADDR,
                                           FLASH_MODIFIED_F1_NOWENERGY_ADDR);
        break;
    }
    /*正向有功总费率2*/
    case 0x35:
    {
        energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_F2_NOWENERGY_ADDR,
                                           FLASH_MODIFIED_F2_NOWENERGY_ADDR);
        break;
    }
    /*正向有功总费率3*/
    case 0x36:
    {
        energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_F3_NOWENERGY_ADDR,
                                           FLASH_MODIFIED_F3_NOWENERGY_ADDR);
        break;
    }
    /*正向有功总费率4*/
    case 0x37:
    {
        energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_F4_NOWENERGY_ADDR,
                                           FLASH_MODIFIED_F4_NOWENERGY_ADDR);
        break;
    }
    /*正向有功总数据块*/
    case 0x32:
    {
        /*			2018-6-27 09:07:02：重庆召测日冻结数据块发现总和四费率之和有差异，
	 *			原因为乘以0.9后小数点后第三位被忽略，总电量大于分费率电量之和，
	 *			将总电量计算方法改为将四费率分电量相加
	 *			energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_ALL_NOWENERGY_ADDR,FLASH_MODIFIED_ALL_NOWENERGY_ADDR);
	 *			*/
        energy_F1 = Get_Energy(pucBuffer[18], pucBuffer[19], pucBuffer[20], pucBuffer[21]);
        energy_F1 = vNow_Frozen_Modify_Mothod(energy_F1, FLASH_REAL_F1_NOWENERGY_ADDR,
                                              FLASH_MODIFIED_F1_NOWENERGY_ADDR);
        energy_F2 = Get_Energy(pucBuffer[22], pucBuffer[23], pucBuffer[24], pucBuffer[25]);
        energy_F2 = vNow_Frozen_Modify_Mothod(energy_F2, FLASH_REAL_F2_NOWENERGY_ADDR,
                                              FLASH_MODIFIED_F2_NOWENERGY_ADDR);
        energy_F3 = Get_Energy(pucBuffer[26], pucBuffer[27], pucBuffer[28], pucBuffer[29]);
        energy_F3 = vNow_Frozen_Modify_Mothod(energy_F3, FLASH_REAL_F3_NOWENERGY_ADDR,
                                              FLASH_MODIFIED_F3_NOWENERGY_ADDR);

        energy_F4 = Get_Energy(pucBuffer[30], pucBuffer[31], pucBuffer[32], pucBuffer[33]);
        energy_F4 = vNow_Frozen_Modify_Mothod(energy_F4, FLASH_REAL_F4_NOWENERGY_ADDR,
                                              FLASH_MODIFIED_F4_NOWENERGY_ADDR);
        /*读取总电量真实值*/
        energy = Get_Energy(pucBuffer[14], pucBuffer[15], pucBuffer[16], pucBuffer[17]);
        /*保存总电量真实值，单抄总电量时使用*/
        Flash_Write_Word(FLASH_REAL_ALL_NOWENERGY_ADDR, energy);
        /*根据分费率电量计算修改后的总电量*/
        energy = energy_F1 + energy_F2 + energy_F3 + energy_F4;
        /*保存修改值，单抄总电量时使用*/
        Flash_Write_Word(FLASH_MODIFIED_ALL_NOWENERGY_ADDR, energy);
        break;
    }
    default:
        break;
    }
    switch (pucBuffer[11])
    {
    /*正向有功总，或A/B/C相正向有功*/
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    {
        Fill_Energy(energy, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16], &pucBuffer[17]);
        pucBuffer[18] = 0;
        for (i = 0; i < 18; i++)
        {
            pucBuffer[18] += pucBuffer[i];
        }
        pucBuffer[19] = 0x16;
        break;
    }
    /*正向有功总数据块*/
    case 0x32:
    {
        /*总*/
        Fill_Energy(energy, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16], &pucBuffer[17]);
        /*尖*/
        Fill_Energy(energy_F1, &pucBuffer[18], &pucBuffer[19], &pucBuffer[20], &pucBuffer[21]);
        /*峰*/
        Fill_Energy(energy_F2, &pucBuffer[22], &pucBuffer[23], &pucBuffer[24], &pucBuffer[25]);
        /*平*/
        Fill_Energy(energy_F3, &pucBuffer[26], &pucBuffer[27], &pucBuffer[28], &pucBuffer[29]);
        /*谷*/
        Fill_Energy(energy_F4, &pucBuffer[30], &pucBuffer[31], &pucBuffer[32], &pucBuffer[33]);
        pucBuffer[34] = 0;
        for (i = 0; i < 34; i++)
        {
            pucBuffer[34] += pucBuffer[i];
        }
        pucBuffer[35] = 0x16;
        break;
    }
    default:
        break;
    }
}

/********************************************************************
 * 功    能：日冻结数据修改函数
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 前1日冻结第二天召测开始改，
 * 前2日冻结第三天召测开始改，
 * 前3日东结第四天召测开始改
 * 若上电3日内，前1日、前2日、前3日冻结每天召测的次数不全是一次，则无法
 * 实现，故不修改前2日和前3日冻结
 *
	case ID_05060101:	//上1日冻结正向有功电能数据（费率1~4）
	case ID_05060102:	//上2日冻结正向有功电能数据（费率1~4）
	case ID_05060103:	//上3日冻结正向有功电能数据（费率1~4） 
 * 编写日期：2018.5.9
 **********************************************************************/
void vDayfrozen_Modifier(uint8_t *pucBuffer) //修改日冻结数据函数
{
    uint32_t energy, energy_F1, energy_F2, energy_F3, energy_F4;
    uint16_t i;
    /*上1次、上2次、上3次日冻结电能数：据*/
    if (Dayfrozen_Change_State == 0x00)
        return;
    /*2018-6-27 13:27:07：
	 *重庆反馈上1日冻结正向有功总与四费率之和不一致，
	 *原因为乘以0.9时小数点后第三位被忽略，
	 *正向有功总不再乘以0.9，直接累加四费率修改后的值
	 *energy = Get_Energy(pucBuffer[14], pucBuffer[15], pucBuffer[16], pucBuffer[17]);	
	 *energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_1DAYFROZEN_ADDR, FLASH_MODIFIED_1DAYFROZEN_ADDR);
	 *Fill_Energy(energy, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16], &pucBuffer[17]);
	*/

    /*正向有功尖费率电量*/
    energy_F1 = Get_Energy(pucBuffer[18], pucBuffer[19], pucBuffer[20], pucBuffer[21]);
    energy_F1 = vNow_Frozen_Modify_Mothod(energy_F1, FLASH_REAL_1DAYFROZEN_ADDR + 4, FLASH_MODIFIED_1DAYFROZEN_ADDR + 4);
    Fill_Energy(energy_F1, &pucBuffer[18], &pucBuffer[19], &pucBuffer[20], &pucBuffer[21]);
    /*正向有功峰费率电量*/
    energy_F2 = Get_Energy(pucBuffer[22], pucBuffer[23], pucBuffer[24], pucBuffer[25]);
    energy_F2 = vNow_Frozen_Modify_Mothod(energy_F2, FLASH_REAL_1DAYFROZEN_ADDR + 8, FLASH_MODIFIED_1DAYFROZEN_ADDR + 8);
    Fill_Energy(energy_F2, &pucBuffer[22], &pucBuffer[23], &pucBuffer[24], &pucBuffer[25]);
    /*正向有功平费率电量*/
    energy_F3 = Get_Energy(pucBuffer[26], pucBuffer[27], pucBuffer[28], pucBuffer[29]);
    energy_F3 = vNow_Frozen_Modify_Mothod(energy_F3, FLASH_REAL_1DAYFROZEN_ADDR + 12, FLASH_MODIFIED_1DAYFROZEN_ADDR + 12);
    Fill_Energy(energy_F3, &pucBuffer[26], &pucBuffer[27], &pucBuffer[28], &pucBuffer[29]);
    /*正向有功谷费率电量*/
    energy_F4 = Get_Energy(pucBuffer[30], pucBuffer[31], pucBuffer[32], pucBuffer[33]);
    energy_F4 = vNow_Frozen_Modify_Mothod(energy_F4, FLASH_REAL_1DAYFROZEN_ADDR + 16, FLASH_MODIFIED_1DAYFROZEN_ADDR + 16);
    Fill_Energy(energy_F4, &pucBuffer[30], &pucBuffer[31], &pucBuffer[32], &pucBuffer[33]);
    /*正向有功总电量*/
    /*获取上一日真实值*/
    energy = Get_Energy(pucBuffer[14], pucBuffer[15], pucBuffer[16], pucBuffer[17]);
    /*存储真实值*/
    Flash_Write_Word(FLASH_REAL_1DAYFROZEN_ADDR, energy);
    /*计算修改后的值*/
    energy = energy_F1 + energy_F2 + energy_F3 + energy_F4;
    /*存储修改后的值*/
    Flash_Write_Word(FLASH_MODIFIED_1DAYFROZEN_ADDR, energy);
    /*组帧*/
    Fill_Energy(energy, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16], &pucBuffer[17]);

    pucBuffer[34] = 0;
    for (i = 0; i < 34; i++)
    {
        pucBuffer[34] += pucBuffer[i];
    }
    pucBuffer[35] = 0x16;
}

/********************************************************************
 * 功    能：结算日正向有功电量数据调整策略
 * 输    入：
 * 输    出：
 * 说    明：结算日电量为增加值，可以直接按比例缩小
 * 编 写 人：王君龙
 * 编写日期：2017.9.6
 **********************************************************************/
uint32_t vAccountEnergy_Modify_Mothod(uint32_t dayfrozen)
{
    uint64_t i;
    i = (uint64_t)dayfrozen * ucDayfrozen_Modify_Ratio;
    return (uint32_t)(i / 100);
}

/********************************************************************
 * 功    能：结算日正向有功电量数据修改函数
 * 输    入：应用层缓存指针
 * 输    出：none
 * 编 写 人：王君龙
 *
		case ID_00010001:	//上1结算日正向有功总电能
		case ID_00010101:	//上1结算日正向有功费率1电能
		case ID_00010201:	//上1结算日正向有功费率2电能 
		case ID_00010301:	//上1结算日正向有功费率3电能
		case ID_00010401:	//上1结算日正向有功费率4电能
		case ID_0001FF01:	//上1结算日正向有功电能数据块
		case ID_00150001:	//上1结算日A相正向有功电能
		case ID_00290001:	//上1结算日B相正向有功电能
		case ID_003D0001:	//上1结算日C相正向有功电能
 * 编写日期：2017.9.6
 **********************************************************************/
void vAccountEnergy_Modify(uint8_t *pucBuffer)
{
    int energy;
    uint16_t i;
    if (Dayfrozen_Change_State == 0x00)
        return;
    if (pucBuffer[11] == 0x32)
    {
        /*正向有功总电量*/
        energy = Get_Energy(pucBuffer[14], pucBuffer[15], pucBuffer[16], pucBuffer[17]);
        energy = vAccountEnergy_Modify_Mothod(energy);
        Fill_Energy(energy, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16], &pucBuffer[17]);
        /*正向有功尖费率电量*/
        energy = Get_Energy(pucBuffer[18], pucBuffer[19], pucBuffer[20], pucBuffer[21]);
        energy = vAccountEnergy_Modify_Mothod(energy);
        Fill_Energy(energy, &pucBuffer[18], &pucBuffer[19], &pucBuffer[20], &pucBuffer[21]);
        /*正向有功峰费率电量*/
        energy = Get_Energy(pucBuffer[22], pucBuffer[23], pucBuffer[24], pucBuffer[25]);
        energy = vAccountEnergy_Modify_Mothod(energy);
        Fill_Energy(energy, &pucBuffer[22], &pucBuffer[23], &pucBuffer[24], &pucBuffer[25]);
        /*正向有功平费率电量*/
        energy = Get_Energy(pucBuffer[26], pucBuffer[27], pucBuffer[28], pucBuffer[29]);
        energy = vAccountEnergy_Modify_Mothod(energy);
        Fill_Energy(energy, &pucBuffer[26], &pucBuffer[27], &pucBuffer[28], &pucBuffer[29]);
        /*正向有功谷费率电量*/
        energy = Get_Energy(pucBuffer[30], pucBuffer[31], pucBuffer[32], pucBuffer[33]);
        energy = vAccountEnergy_Modify_Mothod(energy);
        Fill_Energy(energy, &pucBuffer[30], &pucBuffer[31], &pucBuffer[32], &pucBuffer[33]);

        pucBuffer[34] = 0;
        for (i = 0; i < 34; i++)
        {
            pucBuffer[34] += pucBuffer[i];
        }
        pucBuffer[35] = 0x16;
    }
    else
    {
        energy = Get_Energy(pucBuffer[14], pucBuffer[15], pucBuffer[16], pucBuffer[17]);
        energy = vAccountEnergy_Modify_Mothod(energy);
        Fill_Energy(energy, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16], &pucBuffer[17]);

        pucBuffer[18] = 0;
        for (i = 0; i < 18; i++)
        {
            pucBuffer[18] += pucBuffer[i];
        }
        pucBuffer[19] = 0x16;
    }
}

/********************************************************************
 * 功    能：正向有功电能量修改禁能/使能
 * 输    入：应用层缓存指针
 * 输    出：none
 * 编 写 人：王君龙
 * 编写日期：2018.5.6
 **********************************************************************/
void vEnergy_Modify_AbleSet(uint8_t *pucBuffer)
{
    uint16_t i;
    if (pucBuffer[14] == 0xFF) //使能
    {
        Dayfrozen_Change_State = pucBuffer[14];
        Flash_Write_Word(FLASH_DAYFROZENCHANGESTATE_ADDR, Dayfrozen_Change_State);
        pucBuffer[8] = (EEProm_Read(FLASH_DAYFROZENCHANGESTATE_ADDR) == Dayfrozen_Change_State
                            ? 0x94
                            : 0xD4);
    }
    else if (pucBuffer[14] == 0x00) //禁：能
    {
        /*清空存储的电量值*/
        FLASH_Unlock();
        FLASH_ErasePage(FLASH_ENERGY_PAGE);
        FLASH_Lock();
        Dayfrozen_Change_State = pucBuffer[14];
        Flash_Write_Word(FLASH_DAYFROZENCHANGESTATE_ADDR, Dayfrozen_Change_State);
        pucBuffer[8] = (EEProm_Read(FLASH_DAYFROZENCHANGESTATE_ADDR) == Dayfrozen_Change_State
                            ? 0x94
                            : 0xD4);
    }
    else
    {
        pucBuffer[8] = 0xD4;
    }
    pucBuffer[9] = 0x00;
    pucBuffer[10] = 0;
    for (i = 0; i < 10; i++)
    {
        pucBuffer[10] += pucBuffer[i];
    }
    pucBuffer[11] = 0x16;
    /*电表口回复*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}

/********************************************************************
 * 功    能：正向有功电能量修改比例设置
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 修改记录：修改比例改为用两个字节表示，BCD码，低字节在前，高字节在后
 * 编写日期：2018.5.6
 **********************************************************************/
void vEnergy_Modify_RaitoSet(uint8_t *pucBuffer)
{
    uint8_t ratio;
    bool ratioValid;
    uint16_t i;

    /*ratio = BCD2DEC(pucBuffer[14]);*/
    //68 AA AA AA AA AA AA 68 91 05 33 74 34 33 20 01 00 16  120%
    ratio = BCD2DEC(pucBuffer[14]) + BCD2DEC(pucBuffer[15]) * 100;
    ratioValid = (ratio > 0 && ratio <= 255);
    if (ratioValid)
    {
        Flash_Write_Word(FLASH_DAYFROZEN_MODIFY_RATIO_ADDR, ratio);
        ucDayfrozen_Modify_Ratio = ratio;
        if (EEProm_Read(FLASH_DAYFROZEN_MODIFY_RATIO_ADDR) == ratio)
        {
            pucBuffer[8] = 0x94;
        }
        else
        {
            pucBuffer[8] = 0xD4;
        }
    }
    else
    {
        pucBuffer[8] = 0xD4;
    }
    pucBuffer[9] = 0x00;
    pucBuffer[10] = 0;
    for (i = 0; i < 10; i++)
    {
        pucBuffer[10] += pucBuffer[i];
    }
    pucBuffer[11] = 0x16;
    /*电表口回复*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}

/********************************************************************
 * 功    能：正向有功电能量恢复出厂设置
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 修改记录：程序需支持电能量调高，所以复位时日冻结修改比例，不再复位:2018年11月5日16:11:34
 * 编写日期：2018.5.6
 **********************************************************************/
void vEnergy_Modify_Reset(uint8_t *pucBuffer)
{
    bool cmdValid;
    uint16_t i;

    cmdValid = (pucBuffer[14] == 0x00);
    if (cmdValid)
    {
        /*电能修改使能*/
        Flash_Write_Word(FLASH_DAYFROZENCHANGESTATE_ADDR, 0xff);
        Dayfrozen_Change_State = 0xff;
        /*日冻结修改比例设置为90*/
        /*Flash_Write_Word(FLASH_DAYFROZEN_MODIFY_RATIO_ADDR, 90);*/
        /*ucDayfrozen_Modify_Ratio = 90;*/
        /*删除存储的原始和修改后的电量值（擦除存储页）*/
        FLASH_Unlock();
        FLASH_ErasePage(FLASH_ENERGY_PAGE);
        FLASH_Lock();
        if ( //(EEProm_Read(FLASH_DAYFROZEN_MODIFY_RATIO_ADDR) == 90) &&
            (EEProm_Read(FLASH_DAYFROZENCHANGESTATE_ADDR) == 0xff))
        {
            pucBuffer[8] = 0x94;
        }
        else
        {
            pucBuffer[8] = 0xD4;
        }
    }
    else
    {
        pucBuffer[8] = 0xD4;
    }
    pucBuffer[9] = 0x00;
    pucBuffer[10] = 0;
    for (i = 0; i < 10; i++)
    {
        pucBuffer[10] += pucBuffer[i];
    }
    pucBuffer[11] = 0x16;
    /*电表口回复*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}
#if 1
/********************************************************************
 * 功    能：正向有功电能量查询修改比例
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 修改记录：
 * 编写日期：2018-11-5 16:12:03
 **********************************************************************/
void vEnergy_Modify_ReadSet(uint8_t *pucbuffer)
{
    //68 AA AA AA AA AA AA 68 91 04 33 76 34 33 71 16
    uint8_t ratio;
    uint32_t i;
    if (pucbuffer[8] != 0x91)
        return;
    //pucbuffer[8] = 0x11;
    pucbuffer[9] = 0x06;
    //ratio = EEProm_Read(FLASH_DAYFROZEN_MODIFY_RATIO_ADDR);
    ratio = ucDayfrozen_Modify_Ratio;

    pucbuffer[14] = DEC2BCD(ratio % 100); //低位在前
    pucbuffer[15] = DEC2BCD(ratio / 100); //高位在后
    pucbuffer[16] = 0;
    for (i = 0; i < 16; i++)
    {
        pucbuffer[16] += pucbuffer[i];
    }
    pucbuffer[17] = 0x16;
    /*电表口回复*/
    uc645DataDomainLength = 4;
    ucDrv_Buf_PutBytes(pucbuffer, pUartDB->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
#ifdef ENABLE_INFR
    /*红外口回复*/
    ucDrv_Buf_PutBytes(pucbuffer, pUartIR->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}
#endif
/********************************************************************
 * 功    能：查询日冻结正向有功总当前真实值和修改值
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 修改记录：
 * 编写日期：2018-11-5 16:12:03
 **********************************************************************/
void vEnergy_Modify_ReadValue(uint8_t *pucbuffer)
{
    uint8_t i;
    int dayFrozen;
    uint32_t realValue;
    uint32_t modifiedValue;

    pucbuffer[8] = 0x91;
    pucbuffer[9] = 0x0C;

    /*读取存储的上次真实值和修改值*/
    realValue = Flash_Read_Word(FLASH_REAL_1DAYFROZEN_ADDR + 4);
    modifiedValue = Flash_Read_Word(FLASH_MODIFIED_1DAYFROZEN_ADDR + 4);
    /*终端第一次使用*/
    if (realValue == 0XFFFFFFFF)
    {
        realValue = 0;
    }
    Fill_Energy(realValue, &pucbuffer[14], &pucbuffer[15], &pucbuffer[16], &pucbuffer[17]);
    if (modifiedValue == 0XFFFFFFFF)
    {
        modifiedValue = 0;
    }
    Fill_Energy(modifiedValue, &pucbuffer[18], &pucbuffer[19], &pucbuffer[20], &pucbuffer[21]);
    pucbuffer[22] = 0;
    for (i = 0; i < 22; i++)
    {
        pucbuffer[22] += pucbuffer[i];
    }
    pucbuffer[23] = 0x16;
    /*电表口回复*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucbuffer, pUartDB->pSndbuf, 24);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucbuffer, pUartIR->pSndbuf, 24);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}

/********************************************************************
 * 功    能：电压合格上限修改函数（红外口收帧处理）
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 编写日期：2018.5.6
 **********************************************************************/
void vVoltage_SetMaxValue(uint8_t *pucBuffer)
{
    int VMAX = 0;
    uint16_t i = 0;
    VMAX = Get_Voltage(pucBuffer[14], pucBuffer[15]);
    if (VMAX > 2200)
    {
        Voltage_MAX = VMAX;
        Flash_Write_Word(FLASH_VMAX1_Addr, pucBuffer[14]);
        Flash_Write_Word(FLASH_VMAX2_Addr, pucBuffer[15]);
        if (((uint8_t)Flash_Read_Word(FLASH_VMAX1_Addr) == pucBuffer[14]) &&
            ((uint8_t)Flash_Read_Word(FLASH_VMAX2_Addr) == pucBuffer[15]))
        {
            pucBuffer[8] = 0x94; //设置成功标志
        }
        else
        {
            pucBuffer[8] = 0xD4;
        }
    }
    else
    {
        pucBuffer[8] = 0xD4;
    }
    pucBuffer[9] = 0x00;
    pucBuffer[10] = 0;
    for (i = 0; i < 10; i++)
    {
        pucBuffer[10] += pucBuffer[i];
    }
    pucBuffer[11] = 0x16; //结束符
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}
/********************************************************************
 * 功    能：电压合格下限修改函数（红外口收帧处理：）
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 编写日期：2018.5.6
 **********************************************************************/
void vVoltage_SetMinValue(uint8_t *pucBuffer)
{
    int VMIN = 0;
    uint16_t i = 0;
    VMIN = Get_Voltage(pucBuffer[14], pucBuffer[15]);
    if (VMIN < 2200)
    {
        Voltage_MIN = VMIN;
        Flash_Write_Word(FLASH_VMIN1_Addr, pucBuffer[14]);
        Flash_Write_Word(FLASH_VMIN2_Addr, pucBuffer[15]);
        if ((EEProm_Read(FLASH_VMIN1_Addr) == pucBuffer[14]) &&
            (EEProm_Read(FLASH_VMIN2_Addr) == pucBuffer[15]))
        {
            pucBuffer[8] = 0x94; //设置成功标志
        }
        else
        {
            pucBuffer[8] = 0xD4;
        }
    }
    else
    {
        pucBuffer[8] = 0xD4;
    }
    pucBuffer[9] = 0x00;
    pucBuffer[10] = 0;
    for (i = 0; i < 10; i++)
    {
        pucBuffer[10] += pucBuffer[i];
    }
    pucBuffer[11] = 0x16;
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}

/********************************************************************
 * 功    能：电压修改上限修改函数（红外口收帧处理：）
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 编写日期：2018.5.6
 **********************************************************************/
void vVoltage_SetMaxMaxValue(uint8_t *pucBuffer)
{
}
/********************************************************************
 * 功    能：电压修改下限修改函数（红外口收帧处理：）
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 编写日期：2018.5.6
 **********************************************************************/
void vVoltage_SetMinMinValue(uint8_t *pucBuffer)
{
}
#if 0
/********************************************************************
 * 功    能：读取电量修改参数：是否修改/修改比例
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 编写日期：2018.5.6
 **********************************************************************/
void vEnergy_Modify_ReadSet(uint8_t *pucBuffer)
{
    uint16_t i;
    uint8_t tRatio;
    pucBuffer[8] = 0x91;
    pucBuffer[9] = 0x06;
    pucBuffer[14] = (uint8_t)Flash_Read_Word(FLASH_DAYFROZENCHANGESTATE_ADDR);
    tRatio = (uint8_t)Flash_Read_Word(FLASH_DAYFROZEN_MODIFY_RATIO_ADDR);
    pucBuffer[15] = DEC2BCD(tRatio);
    pucBuffer[16] = 0;
    for (i = 0; i < 16; i++)
    {
        pucBuffer[16] += pucBuffer[i];
    }
    pucBuffer[17] = 0x16;
    /*电表口回复*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
    /*红外口回复*/
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
}
#endif

/********************************************************************
 * 功    能：负荷记录连续抄读电压数据修改函数
 * 输    入：应用层缓存指针
 * 输    出：none
 * 编 写 人：王君龙
 * 编写日期：2018年8月7日21:08:05
 * 第一帧：68 AA AA AA AA AA AA 68 91 0X 32 34 43 39 mm hh DD MM YY 11 11 11 11 11 11 22 22 22 22 22 22 33 33 33  33 33 33 44 44 44 44 44 44 55 55 55 55
 *                                 B1
 * 后续帧：68 AA AA AA AA AA AA 68 92 0X 32 34 43 39 11 11 11 11 22 22 22 22 33 33 33 33 44 44 44 44 55 55 55 55 55 55 SEQ
 *                                 B2 04             cs 16
 **********************************************************************/
void vLoadProfile_Voltage_Modify(uint8_t *pucbuffer)
{
    uint16_t n = 0; //数据块个数
    uint16_t length = pucbuffer[9];
    uint16_t ID_length = 0; //数据域中数据标识长度
    uint32_t voltage = 0;
    uint16_t i = 0;

    if (pucbuffer[8] == 0x91 || pucbuffer[8] == 0xB1) //无后续帧，或分帧的第一帧
    {
        ID_length = 9; //数据标识*4 + 时标*5
    }
    if (pucbuffer[8] == 0x92 || pucbuffer[8] == 0xB2)
    {
        ID_length = 4; //数据标识*4 ，忽略后面的SEQ*1，原因为整除2后结果一样
    }
    n = (length - ID_length) / 2; //电压数据的个数，分相和块数据都适用
    while (n--)
    {
        voltage = Get_Voltage(pucbuffer[10 + ID_length + 2 * n], pucbuffer[11 + ID_length + 2 * n]);
        voltage = Voltage_Modifier_Method(voltage);
        pucbuffer[10 + ID_length + 2 * n] = GetLowByte(A_Voltage);
        pucbuffer[11 + ID_length + 2 * n] = GetHighByte(A_Voltage);
    }
    pucbuffer[length + 10] = 0;
    for (i = 0; i < (length + 10); i++)
    {
        pucbuffer[length + 10] += pucbuffer[i];
    }
    pucbuffer[length + 11] = 16;
}

/********************************************************************
 * 功    能：负荷记录连续抄读电流数据修改函数
 * 输    入：应用层缓存指针
 * 输    出：	none
 * 编 写 人：王君龙
 * 编写日期：2018年8月7日21:08:05
 **********************************************************************/
void vLoadProfile_Current_Modify(uint8_t *pucbuffer)
{
    uint16_t n = 0; //数据块个数
    uint16_t length = pucbuffer[9];
    uint16_t ID_length = 0;
    uint32_t current = 0;
    uint16_t i = 0;
    uint32_t ABCcurrent = 0;

    if (pucbuffer[8] == 0x91 || pucbuffer[8] == 0xB1) //无后续帧，或分帧的第一帧
    {
        ID_length = 9; //数据标识*4 + 时标*5
    }
    if (pucbuffer[8] == 0x92 || pucbuffer[8] == 0xB2)
    {
        ID_length = 4; //数据标识*4 ，忽略后面的SEQ*1，原因为整除2后结果一样
    }

    switch (pucbuffer[10])
    {
    /*A相电流*/
    case 0x34:
#ifdef ENABLE_CURRENT_LIMIT
        n = (length - ID_length) / 3;
        while (n--)
        {
            if (pucbuffer[10 + ID_length + 3 * n] == 0x32 || //是3不是9
                pucbuffer[11 + ID_length + 3 * n] == 0x32 ||
                pucbuffer[12 + ID_length + 3 * n] == 0x32)
                continue;
            /*读取电流*/
            vGet_Current(pucbuffer[10 + ID_length + 3 * n],
                         pucbuffer[11 + ID_length + 3 * n],
                         pucbuffer[12 + ID_length + 3 * n],
                         &Acurrent);
            /*修改电流*/
            Acurrent.value = (Acurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Acurrent.value);
            /*回写电流*/
            vPut_Current(Acurrent,
                         &pucbuffer[10 + ID_length + 3 * n],
                         &pucbuffer[11 + ID_length + 3 * n],
                         &pucbuffer[12 + ID_length + 3 * n]);
        }
        pucbuffer[length + 10] = 0;
        for (i = 0; i < (length + 10); i++)
        {
            pucbuffer[length + 10] += pucbuffer[i];
        }
        pucbuffer[length + 11] = 16;
#endif
        break;
        /*B相电流*/
    case 0x35:
#ifdef ENABLE_CURRENT_LIMIT
        n = (length - ID_length) / 3;
        while (n--)
        {
            if (pucbuffer[10 + ID_length + 3 * n] == 0x32 ||
                pucbuffer[11 + ID_length + 3 * n] == 0x32 ||
                pucbuffer[12 + ID_length + 3 * n] == 0x32)
                continue;
            /*读取电流*/
            vGet_Current(pucbuffer[10 + ID_length + 3 * n],
                         pucbuffer[11 + ID_length + 3 * n],
                         pucbuffer[12 + ID_length + 3 * n],
                         &Bcurrent);
            /*修改电流*/
            Bcurrent.value = (Bcurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3)
                                                                                  : Bcurrent.value);
            /*回写电流*/
            vPut_Current(Bcurrent,
                         &pucbuffer[10 + ID_length + 3 * n],
                         &pucbuffer[11 + ID_length + 3 * n],
                         &pucbuffer[12 + ID_length + 3 * n]);
        }
        pucbuffer[length + 10] = 0;
        for (i = 0; i < (length + 10); i++)
        {
            pucbuffer[length + 10] += pucbuffer[i];
        }
        pucbuffer[length + 11] = 16;
#endif
        break;
    case 0x36: //C相电流
#ifdef ENABLE_CURRENT_LIMIT
        n = (length - ID_length) / 3;
        while (n--)
        {
            if (pucbuffer[10 + ID_length + 9 * n] == 0x32 ||
                pucbuffer[11 + ID_length + 9 * n] == 0x32 ||
                pucbuffer[12 + ID_length + 9 * n] == 0x32)
                continue;
            /*读取电流*/
            vGet_Current(pucbuffer[10 + ID_length + 3 * n],
                         pucbuffer[11 + ID_length + 3 * n],
                         pucbuffer[12 + ID_length + 3 * n],
                         &Ccurrent);
            /*修改电流*/
            Ccurrent.value = (Ccurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Ccurrent.value);
            /*回写电流*/
            vPut_Current(Ccurrent,
                         &pucbuffer[10 + ID_length + 3 * n],
                         &pucbuffer[11 + ID_length + 3 * n],
                         &pucbuffer[12 + ID_length + 3 * n]);
        }
        pucbuffer[length + 10] = 0;
        for (i = 0; i < (length + 10); i++)
        {
            pucbuffer[length + 10] += pucbuffer[i];
        }
        pucbuffer[length + 11] = 16;
#endif
        break;
    case 0x32: //块电流
#ifdef ENABLE_CURRENT_DEVIATION
        n = (length - ID_length) / 9; //数据点个数
        while (n--)
        {
            vGet_Current(pucbuffer[10 + ID_length + 9 * n],
                         pucbuffer[11 + ID_length + 9 * n],
                         pucbuffer[12 + ID_length + 9 * n],
                         &Acurrent);

            vGet_Current(pucbuffer[13 + ID_length + 9 * n],
                         pucbuffer[14 + ID_length + 9 * n],
                         pucbuffer[15 + ID_length + 9 * n],
                         &Bcurrent);

            vGet_Current(pucbuffer[16 + ID_length + 9 * n],
                         pucbuffer[17 + ID_length + 9 * n],
                         pucbuffer[18 + ID_length + 9 * n],
                         &Ccurrent);

            //电流为负值,不修改电流
            if (Acurrent.isminus || Bcurrent.isminus || Ccurrent.isminus)
                continue;
            //电流为0，不修改电流
            if ((Acurrent.value == 0) || (Bcurrent.value == 0) || (Ccurrent.value == 0))
                continue;
            //电流相互之间不超差，不修改电流
            if (CURRENT_MAX_DVALUE * MAX(Acurrent.value, MAX(Bcurrent.value, Ccurrent.value)) / 100 <=
                MIN(Acurrent.value, MIN(Bcurrent.value, Ccurrent.value)))
                continue;
            else //需要修改，最大值减小，最小值增大，剩下的是中间值
            {
                if ((Acurrent.value >= Bcurrent.value) && (Bcurrent.value >= Ccurrent.value))
                    vCurrentTriple_Modify_Method(&Acurrent, &Bcurrent, &Ccurrent);
                else if ((Acurrent.value >= Ccurrent.value) && (Ccurrent.value >= Bcurrent.value))
                    vCurrentTriple_Modify_Method(&Acurrent, &Ccurrent, &Bcurrent);
                else if ((Bcurrent.value >= Acurrent.value) && (Acurrent.value >= Ccurrent.value))
                    vCurrentTriple_Modify_Method(&Bcurrent, &Acurrent, &Ccurrent);
                else if ((Bcurrent.value >= Ccurrent.value) && (Ccurrent.value >= Acurrent.value))
                    vCurrentTriple_Modify_Method(&Bcurrent, &Ccurrent, &Acurrent);
                else if ((Ccurrent.value >= Acurrent.value) && (Acurrent.value >= Bcurrent.value))
                    vCurrentTriple_Modify_Method(&Ccurrent, &Acurrent, &Bcurrent);
                else if ((Ccurrent.value >= Bcurrent.value) && (Bcurrent.value >= Acurrent.value))
                    vCurrentTriple_Modify_Method(&Ccurrent, &Bcurrent, &Acurrent);
                /*回写电流*/
                vPut_Current(Acurrent,
                             &pucbuffer[10 + ID_length + 9 * n],
                             &pucbuffer[11 + ID_length + 9 * n],
                             &pucbuffer[12 + ID_length + 9 * n]);

                vPut_Current(Bcurrent,
                             &pucbuffer[14 + ID_length + 9 * n],
                             &pucbuffer[15 + ID_length + 9 * n],
                             &pucbuffer[16 + ID_length + 9 * n]);

                vPut_Current(Ccurrent,
                             &pucbuffer[17 + ID_length + 9 * n],
                             &pucbuffer[18 + ID_length + 9 * n],
                             &pucbuffer[19 + ID_length + 9 * n]);
            }
        }
        pucbuffer[length + 10] = 0;
        for (i = 0; i < length + 10; i++)
        {
            pucbuffer[length + 10] += pucbuffer[i];
        }
        pucbuffer[length + 11] = 0x16;
#endif
#ifdef ENABLE_CURRENT_LIMIT
        n = (length - ID_length) / 9; //数据点个数
        while (n--)
        {
            /*FF无效数据*/
            if (pucbuffer[10 + ID_length + 9 * n] == 0x32 ||
                pucbuffer[11 + ID_length + 9 * n] == 0x32 ||
                pucbuffer[12 + ID_length + 9 * n] == 0x32 ||
                pucbuffer[13 + ID_length + 9 * n] == 0x32 ||
                pucbuffer[14 + ID_length + 9 * n] == 0x32 ||
                pucbuffer[15 + ID_length + 9 * n] == 0x32 ||
                pucbuffer[16 + ID_length + 9 * n] == 0x32 ||
                pucbuffer[17 + ID_length + 9 * n] == 0x32 ||
                pucbuffer[18 + ID_length + 9 * n] == 0x32)
                continue;

            /*读取电流*/
            vGet_Current(pucbuffer[10 + ID_length + 9 * n],
                         pucbuffer[11 + ID_length + 9 * n],
                         pucbuffer[12 + ID_length + 9 * n],
                         &Acurrent);
            vGet_Current(pucbuffer[13 + ID_length + 9 * n],
                         pucbuffer[14 + ID_length + 9 * n],
                         pucbuffer[15 + ID_length + 9 * n],
                         &Bcurrent);
            vGet_Current(pucbuffer[16 + ID_length + 9 * n],
                         pucbuffer[17 + ID_length + 9 * n],
                         pucbuffer[18 + ID_length + 9 * n],
                         &Ccurrent);

            ABCcurrent = Acurrent.value + Bcurrent.value + Ccurrent.value;
            if (ABCcurrent > TransFormer.CT2_CurrentLimit)
            {
                Acurrent.value = TransFormer.CT2_CurrentLimit * Acurrent.value / ABCcurrent;
                Bcurrent.value = TransFormer.CT2_CurrentLimit * Bcurrent.value / ABCcurrent;
                Ccurrent.value = TransFormer.CT2_CurrentLimit * Ccurrent.value / ABCcurrent;
            }
            /*保证每单相电流不重载*/
            Acurrent.value = (Acurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Acurrent.value);
            Bcurrent.value = (Bcurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Bcurrent.value);
            Ccurrent.value = (Ccurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Ccurrent.value);
            /*回写电流*/
            vPut_Current(Acurrent,
                         &pucbuffer[10 + ID_length + 9 * n],
                         &pucbuffer[11 + ID_length + 9 * n],
                         &pucbuffer[12 + ID_length + 9 * n]);
            vPut_Current(Bcurrent,
                         &pucbuffer[13 + ID_length + 9 * n],
                         &pucbuffer[14 + ID_length + 9 * n],
                         &pucbuffer[15 + ID_length + 9 * n]);
            vPut_Current(Ccurrent,
                         &pucbuffer[16 + ID_length + 9 * n],
                         &pucbuffer[17 + ID_length + 9 * n],
                         &pucbuffer[18 + ID_length + 9 * n]);
        }
        pucbuffer[length + 10] = 0;
        for (i = 0; i < length + 10; i++)
        {
            pucbuffer[length + 10] += pucbuffer[i];
        }
        pucbuffer[length + 11] = 0x16;
#endif
        break;
    default:
        break;
    }
}

/*
江西：01
山东：02
重庆：03
*/
void vRead_Version(uint8_t *pucbuffer)
{
    uint8_t i;
    pucbuffer[8] = 0x91;
    pucbuffer[9] = 0x08;
    pucbuffer[14] = 0x03; //地区
    pucbuffer[15] = 0x05; //日月年
    pucbuffer[16] = 0x11;
    pucbuffer[17] = 0x18;
    pucbuffer[18] = 0;
    for (i = 0; i < 18; i++)
    {
        pucbuffer[18] += pucbuffer[i];
    }
    pucbuffer[19] = 0x16;
    uc645DataDomainLength = 8;
    ucDrv_Buf_PutBytes(pucbuffer, pUartDB->pSndbuf, 20);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*红外口回复*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucbuffer, pUartIR->pSndbuf, 20);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}
