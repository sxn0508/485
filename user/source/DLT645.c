#include "DLT645.h"
#include "Time.h"
#include "Flash.h"

/*������������*/
static struct ucbuf Uart1_Rsvbuf, Uart1_Sndbuf, Uart3_Rsvbuf, Uart3_Sndbuf, Uart2_Rsvbuf, Uart2_Sndbuf;
/*Ӧ�ò㴮�ڽṹ��*/
static UartDef UartZD = {USART1, &Uart1_Rsvbuf, &Uart1_Sndbuf};
static UartDef UartDB = {USART3, &Uart3_Rsvbuf, &Uart3_Sndbuf};
static UartDef UartIR = {USART2, &Uart2_Rsvbuf, &Uart2_Sndbuf};
/*Ӧ�ò㴮�ڽṹ��ָ��*/
UartDef *pUartZD = &UartZD;
UartDef *pUartDB = &UartDB;
UartDef *pUartIR = &UartIR;
/*��Լ֡����*/
uint8_t ucApp_Buf_DB2ZD[DATA_BUF_SIZE]; //�������֡Ӧ�û���
uint8_t ucApp_Buf_ZD2DB[DATA_BUF_SIZE]; //�ն�����֡Ӧ�û���
uint8_t ucApp_Buf_INFR[DATA_BUF_SIZE];  //�����Ӧ�û���
/*
#define ENABLE_ESTATUS						//���״̬���޸Ĺ��� 
#define ENABLE_CURRENT_DEVIATION			//���������ֵ�޸Ĺ���
#define ENABLE_CURRENT_LIMIT				//�������ֵ�޸Ĺ���
#define ENABLE_ENERGY_MODIFY				//�޸������й�
#define ENABLE_INFR							//�޸������й�
*/
const DefDataIDHandle INFRDataID_Handle_List[] = {
    {{0X33, 0X37, 0X34, 0X35}, Get_VChange_State},       //���䣺��ѹ�޸�ʹ��/����ID_02010400,
    {{0X33, 0X39, 0X34, 0X35}, Query_Voltage_Limit},     //���䣺��ѹ��ֵ��ѯID_02010600,
    {{0X33, 0X3A, 0X34, 0X35}, Default_Reset},           //���䣺��ѹ�޸Ĺ��ָܻ���������ID_02010700,
    {{0X33, 0X3B, 0X34, 0X35}, vVoltage_SetMaxValue},    //���䣺��ѹ���úϸ�����
    {{0X33, 0X3C, 0X34, 0X35}, vVoltage_SetMinValue},    //���䣺��ѹ���úϸ�����
    {{0X33, 0X3D, 0X34, 0X35}, vVoltage_SetMaxMaxValue}, //���䣺��ѹ�����޸�����
    {{0X33, 0X3E, 0X34, 0X35}, vVoltage_SetMinMinValue}, //���䣺��ѹ�����޸�����
    {{0X33, 0X73, 0X34, 0X33}, vEnergy_Modify_AbleSet},  //���䣺�����й��������޸�ʹ��/����ID_00014000,
    {{0X33, 0X74, 0X34, 0X33}, vEnergy_Modify_RaitoSet}, //���䣺�����й��������޸ı�������ID_00014100,
    {{0X33, 0X75, 0X34, 0X33}, vEnergy_Modify_Reset},    //���䣺�����й��������޸Ļָ���������ID_00014200,
    {{0X33, 0X76, 0X34, 0X33}, vEnergy_Modify_ReadSet},  //���䣺�����й��������޸Ķ�ȡ�裺��
    /*�����ع���*/
    {{0X33, 0X37, 0X35, 0X35}, vCurrent_Limit_Set},                    //���䣺���õ����ع��ز���:��������
    {{0X33, 0X38, 0X35, 0X35}, vCurrent_Limit_ReadSet},                //���䣺���õ����ع��ز���:�������޶�ȡ
    {{0X33, 0X39, 0X35, 0X35}, vCurrent_CTRatio_Set},                  //���䣺���õ����ع��ز���:���β�CT���
    {{0X33, 0X3A, 0X35, 0X35}, vCurrent_CTRatio_ReadSet},              //���䣺��ȡ�����ع��ز���:���β�CT���
    {{0X33, 0X3B, 0X35, 0X35}, vCurrent_TransFormer_Capacity_ReadSet}, //���䣺��ȡ�����ع��ز���:��ѹ������
    {{0X32, 0X32, 0X32, 0X32}, vRead_Version},                         //���䣺��ȡ����汾
};
const DefDataIDHandle DBDataID_Handle_List[] = {
    {{0X33, 0X34, 0X34, 0X35}, vVoltageHandle}, //A���ѹID_02010100,
    {{0X33, 0X35, 0X34, 0X35}, vVoltageHandle}, //B���ѹID_02010200,
    {{0X33, 0X36, 0X34, 0X35}, vVoltageHandle}, //C���ѹID_02010300,
    {{0X33, 0X32, 0X34, 0X35}, vVoltageHandle}, //��ѹ���ݿ�ID_0201FF00,
#ifdef ENABLE_CURRENT_DEVIATION
    {{0X33, 0X34, 0X35, 0X35}, vCurrent_Modify}, //A�����ID_02020100,
    {{0X33, 0X35, 0X35, 0X35}, vCurrent_Modify}, //B�����ID_02020200,
    {{0X33, 0X36, 0X35, 0X35}, vCurrent_Modify}, //C�����ID_02020300,
    {{0X33, 0X32, 0X35, 0X35}, vCurrent_Modify}, //�������ݿ�ID_0202FF00,
#else
#ifdef ENABLE_CURRENT_LIMIT
    {{0X33, 0X34, 0X35, 0X35}, vCurrent_Limit_Modify}, //A�����ID_02020100,
    {{0X33, 0X35, 0X35, 0X35}, vCurrent_Limit_Modify}, //B�����ID_02020200,
    {{0X33, 0X36, 0X35, 0X35}, vCurrent_Limit_Modify}, //C�����ID_02020300,
    {{0X33, 0X32, 0X35, 0X35}, vCurrent_Limit_Modify}, //�������ݿ�ID_0202FF00,
#endif
#endif
    {{0X37, 0X38, 0X33, 0X38}, Estatus_Modify_CurrentEvent}, //�������״̬��4ID_04000504,
    {{0X38, 0X38, 0X33, 0X38}, Estatus_Modify_CurrentEvent}, //�������״̬��5ID_04000505,
    {{0X39, 0X38, 0X33, 0X38}, Estatus_Modify_CurrentEvent}, //�������״̬��6ID_04000506,
    {{0X3A, 0X38, 0X33, 0X38}, Estatus_Modify_CurrentEvent}, //�������״̬��7ID_04000507,
    {{0X32, 0X38, 0X33, 0X38}, Estatus_Modify_CurrentEvent}, //�������״̬�����ݿ�ID_040005FF,
    {{0X33, 0X33, 0X34, 0X33}, vNowEnergy_Modifier},         //��ǰ�����й��ܵ���ID_00010000,
    {{0X33, 0X34, 0X34, 0X33}, vNowEnergy_Modifier},         //��ǰ�����й�����1����ID_00010100,
    {{0X33, 0X35, 0X34, 0X33}, vNowEnergy_Modifier},         //��ǰ�����й�����2����ID_00010200,
    {{0X33, 0X36, 0X34, 0X33}, vNowEnergy_Modifier},         //��ǰ�����й�����3����ID_00010300,
    {{0X33, 0X37, 0X34, 0X33}, vNowEnergy_Modifier},         //��ǰ�����й�����4����ID_00010400,
    {{0X33, 0X32, 0X34, 0X33}, vNowEnergy_Modifier},         //��ǰ�����й��������ݿ�ID_0001FF00,
    {{0X33, 0X33, 0X48, 0X33}, vNowEnergy_Modifier},         //��ǰA�������й�ID_00150000,
    {{0X33, 0X33, 0X5C, 0X33}, vNowEnergy_Modifier},         //��ǰB�������й�ID_00290000,
    {{0X33, 0X33, 0X70, 0X33}, vNowEnergy_Modifier},         //��ǰC�������й�ID_003D0000,
    {{0X34, 0X34, 0X39, 0X38}, vDayfrozen_Modifier},         //��1�ն��������й��������ݣ�����1~4��ID_05060101,
    {{0X35, 0X34, 0X39, 0X38}, vDayfrozen_Modifier},         //��2�ն��������й��������ݣ�����1~4��ID_05060102,
    {{0X36, 0X34, 0X39, 0X38}, vDayfrozen_Modifier},         //��3�ն��������й��������ݣ�����1~4�� ID_05060103,
    {{0X34, 0X33, 0X34, 0X33}, vAccountEnergy_Modify},       //��1�����������й��ܵ���ID_00010001,
    {{0X34, 0X34, 0X34, 0X33}, vAccountEnergy_Modify},       //��1�����������й�����1����ID_00010101,
    {{0X34, 0X35, 0X34, 0X33}, vAccountEnergy_Modify},       //��1�����������й�����1���� ID_00010201,
    {{0X34, 0X36, 0X34, 0X33}, vAccountEnergy_Modify},       //��1�����������й�����1����ID_00010301,
    {{0X34, 0X37, 0X34, 0X33}, vAccountEnergy_Modify},       //��1�����������й�����1����ID_00010401,
    {{0X34, 0X32, 0X34, 0X33}, vAccountEnergy_Modify},       //��1�����������й��������ݿ�ID_0001FF01,
    {{0X34, 0X33, 0X48, 0X33}, vAccountEnergy_Modify},       //��1������A�������й�����ID_00150001,
    {{0X34, 0X33, 0X5C, 0X33}, vAccountEnergy_Modify},       //��1������B�������й�����ID_00290001,
    {{0X34, 0X33, 0X70, 0X33}, vAccountEnergy_Modify},       //��1������C�������й�����ID_003D0001,
    {{0X33, 0X37, 0X34, 0X35}, Get_VChange_State},           //���䣺��ѹ�޸�ʹ��/����ID_02010400,
    {{0X33, 0X38, 0X34, 0X35}, Set_Voltage_Limit},           //���䣺��ѹ��ֵ����ID_02010500,
    {{0X33, 0X39, 0X34, 0X35}, Query_Voltage_Limit},         //���䣺��ѹ��ֵ��ѯID_02010600,
    {{0X33, 0X3A, 0X34, 0X35}, Default_Reset},               //���䣺��ѹ�޸Ĺ��ָܻ���������ID_02010700,

    /*�����ع���*/
    {{0X33, 0X37, 0X35, 0X35}, vCurrent_Limit_Set},                    //���䣺���õ����ع��ز���:��������
    {{0X33, 0X38, 0X35, 0X35}, vCurrent_Limit_ReadSet},                //���䣺���õ����ع��ز���:�������޶�ȡ
    {{0X33, 0X39, 0X35, 0X35}, vCurrent_CTRatio_Set},                  //���䣺���õ����ع��ز���:���β�CT���
    {{0X33, 0X3A, 0X35, 0X35}, vCurrent_CTRatio_ReadSet},              //���䣺��ȡ�����ع��ز���:���β�CT���
    {{0X33, 0X3B, 0X35, 0X35}, vCurrent_TransFormer_Capacity_ReadSet}, //���䣺��ȡ�����ع��ز���:��ѹ������

    {{0X33, 0X73, 0X34, 0X33}, vEnergy_Modify_AbleSet},   //���䣺�����й��������޸�ʹ��/����ID_00014000,
    {{0X33, 0X74, 0X34, 0X33}, vEnergy_Modify_RaitoSet},  //���䣺�����й��������޸ı�������ID_00014100,
    {{0X33, 0X75, 0X34, 0X33}, vEnergy_Modify_Reset},     //���䣺�����й��������ظ���������ID_00014200,
    {{0X33, 0X76, 0X34, 0X33}, vEnergy_Modify_ReadSet},   //���䣺�����й��������޸Ķ�ȡ����
    {{0X33, 0X77, 0X34, 0X33}, vEnergy_Modify_ReadValue}, //���䣺�����й���������ȡ�洢����ʵֵ���޸�ֵID_00014200,
    /*4�Ų��Ÿ��������������������еĵ�ѹ���ߡ����������޸�,2018��8��7��20:53:4��0*/
    {{0X33, 0X34, 0X43, 0X39}, vLoadProfile_Voltage_Modify}, //A���ѹ������������ID_06100101,
    {{0X34, 0X34, 0X43, 0X39}, vLoadProfile_Voltage_Modify}, //B��ѹ���ݿ�������������ID_06100102,
    {{0X35, 0X34, 0X43, 0X39}, vLoadProfile_Voltage_Modify}, //C��ѹ���ݿ�������������ID_06100103,
    {{0X32, 0X34, 0X43, 0X39}, vLoadProfile_Voltage_Modify}, //��ѹ���ݿ�������������ID_061001FF,
    {{0X33, 0X35, 0X43, 0X39}, vLoadProfile_Current_Modify}, //A�����������������ID_06100201,
    {{0X34, 0X35, 0X43, 0X39}, vLoadProfile_Current_Modify}, //B�������ݿ�������������ID_06100202,
    {{0X35, 0X35, 0X43, 0X39}, vLoadProfile_Current_Modify}, //C�������ݿ�������������ID_06100203,
    {{0X32, 0X35, 0X43, 0X39}, vLoadProfile_Current_Modify}, //�������ݿ�������������ID_061002FF,
    {{0X32, 0X32, 0X32, 0X32}, vRead_Version},               //���䣺��ȡ����汾
};

FrameHandleStatus Voltage_Frame_Status;
uint8_t ucRcvStatus = 0;
uint8_t uc645DataDomainLength = 0;                 //645֡�����򳤶�
uint8_t Voltage_Change_State = Voltage_NOChange;   //��ѹ�Ƿ��޸ı�־λ��Ĭ���޸�״̬
uint8_t Dayfrozen_Change_State = DAYFROZEN_CHANGE; //�����Ƿ��޸ģ�Ĭ���޸�
uint8_t ucDayfrozen_Modify_Ratio;                  //������������
int Voltage_MAX;                                   //��ѹ�ϸ�����
int Voltage_MIN;                                   //��ѹ�ϸ�����
int A_Voltage = WORK_VOLTAGE;                      //���һ���޸ĺ��A���ѹֵ
int B_Voltage = WORK_VOLTAGE;                      //���һ���޸ĺ��B���ѹֵ
int C_Voltage = WORK_VOLTAGE;                      //���һ���޸ĺ��C���ѹֵ
CURRENT Acurrent = {0, 0, false};
CURRENT Bcurrent = {0, 0, false};
CURRENT Ccurrent = {0, 0, false};

/*��ѹ���ṹ�嶨��*/
TransFormer_TypeDef TransFormer;

/*�������ء����ص�����ز���
#ifdef	  TRANSFORMER_CAPACITY_50KVA
	#define TransFormer.CT2_CurrentLimit					2700						//50KVA��ѹ����CT���β���������͵����ֵ������1000����ʵ��ֵ��2.7A
#endif
#ifdef	  TRANSFORMER_CAPACITY_100KVA
	#define TransFormer.CT2_CurrentLimit					5400						//100KVA��ѹ����CT���β���������͵����ֵ������1000����ʵ��ֵ��5.4A	
#endif
#ifdef	  TRANSFORMER_CAPACITY_200KVA
	#define TransFormer.CT2_CurrentLimit					10800						//200KVA��ѹ����CT���β���������͵����ֵ������1000����ʵ��ֵ��10.8A
#endif
#ifdef	  TRANSFORMER_CAPACITY_400KVA
	#define TransFormer.CT2_CurrentLimit					21600						//400KVA��ѹ����CT���β���������͵����ֵ������1000����ʵ��ֵ��21.6A
#endif
*/

/********************************************************************
 * ��    �ܣ������Ĵ浽Ӧ�ò㻺�棬���ж��Ƿ�Ϊ645֡
 * ��    �룺Ӧ�ò㻺��ָ�룬�����㻺��ָ��
 * ��    ����������Ϸ�����true
 ���򷵻�false
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
 **********************************************************************/

bool blRecvFrame(struct ucbuf *ucDrv_Buf, uint8_t *ucdata_buf)
{
    /*��֡״̬*/
    switch (ucRcvStatus)
    {
    /*֡ͷ68*/
    case SEARCH_SOH:
    {
        /*δ����ָ���ֽ�����������֧������false*/
        if (ucDrv_Buf_GetBytes(ucDrv_Buf, ucdata_buf, 1) == false)
        {
            break;
        }
        /*����һ���ֽ�*/
        else
        {
            /*֡ͷ���Ϸ�������false*/
            if (*(ucdata_buf + 0) != 0X68)
            {
                break;
            }
            else
            {
                /*֡ͷ�Ϸ�����֡״��ΪRCV_DATA1*/
                ucRcvStatus = RCV_DATA1;
                blWordsCounterStatus = true;
            }
        }
    }
    case RCV_DATA1:
    {
        /*��9���ֽ�ʧ�ܣ�������֧������false*/
        if (ucDrv_Buf_GetBytes(ucDrv_Buf, ucdata_buf + 1, 9) == false)
        {
            /*DLT645_WORDS_DELAY_MAX���ֽڽ��ճ�ʱ*/
            if (blTime2_Update(uwTime2WordsCounter, DLT645_WORDS_DELAY_MAX))
            {
                /*��ʱ�������ж�֡ͷ68������ʱ������ҳ��ȣ�*/
                ucRcvStatus = SEARCH_SOH;
                blWordsCounterStatus = false;
                uwTime2WordsCounter = 0;
            }
            break;
        }
        else //��68֮��9���ֽڳɹ�
        {
            if ((*(ucdata_buf + 7) != 0X68) || ((*(ucdata_buf + 9) + 12) >= 255))
            {
                ucRcvStatus = SEARCH_SOH; //���߸��ֽڲ�Ϊ68 ,����������false��������68
                blWordsCounterStatus = false;
                uwTime2WordsCounter = 0;
                break;
            }
            else
            {                                              //���߸��ֽ���68
                uc645DataDomainLength = *(ucdata_buf + 9); //����645֡����L
                ucRcvStatus = RCV_DATA2;                   //ָ����һ����
                blWordsCounterStatus = false;
                uwTime2WordsCounter = 0;
                blWordsCounterStatus = true;
            }
        }
    }
    case RCV_DATA2:
    {
        if (ucDrv_Buf_GetBytes(ucDrv_Buf, ucdata_buf + 10, uc645DataDomainLength + 2) == false)
        {                                                                    //�������ֽ�ʧ�ܣ�������֧������false��
            if (blTime2_Update(uwTime2WordsCounter, DLT645_WORDS_DELAY_MAX)) //DLT645_WORDS_DELAY_MAX����ֽڽ��ճ�ʱ
            {                                                                //�����ʱ����������68���������½��պ����ֽڣ�������֧
                ucRcvStatus = SEARCH_SOH;
                blWordsCounterStatus = false;
                uwTime2WordsCounter = 0;
            }
            break;
        }
        else //����ָ���ֽ�����һ֡
        {
            ucRcvStatus = SEARCH_SOH; //״̬��Ϊ����68��
            blWordsCounterStatus = false;
            uwTime2WordsCounter = 0;
            if (dwCheckFrame(ucdata_buf) != OK_1)
            {
                //	if (blTime2_Update(uwTime2WordsCounter,DLT645_WORDS_DELAY_MAX)) //DLT645_WORDS_DELAY_MAX����ֽڽ��ճ�ʱ
                //{ //�����ʱ����������68���������½��պ����ֽڣ�������֧
                //ucRcvStatus = SEARCH_SOH;
                //}//֡У�����
                return false;
            }
            else
            {
                //ucRcvStatus = SEARCH_SOH;//״̬��Ϊ����68��
                //blWordsCounterStatus = false;
                return true; //�ж���֡�Ƿ�ϸ񣬺ϸ񷵻�true�����򷵻�false
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
 * ��    �ܣ��ж�֡��У��͡������ֽ��Ƿ�Ϸ�
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����������Ϸ�����true
 ���򷵻�false
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
 **********************************************************************/
STATUS dwCheckFrame(uint8_t *pucBuffer)
{
    uint8_t ucCheckData = 0;
    uint8_t i;
    //��ȡУ���
    for (i = 0; i < uc645DataDomainLength + 10; i++)
    {
        ucCheckData += pucBuffer[i];
    }
    //�ж�У��ͺͽ�����
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
 * ��    �ܣ��������֡���ݱ�ʶ��������
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    �������ݱ�ʶ
 * �� д �ˣ�������
 * ��д���ڣ�2018.5.8
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
 * ��    �ܣ��������֡���ݱ�ʶ��������
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    �������ݱ�ʶ
 * �� д �ˣ�������
 * ��д���ڣ�2018.5.8
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
 * ��    �ܣ�����֡�����ĵ�ѹ�޸ı�־λ
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����none
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
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
    /*���ڻظ�*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڻظ�*/
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
 * ��    �ܣ�����֡����ѹ��ֵ����
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����none
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
 **********************************************************************/
void Set_Voltage_Limit(uint8_t *pucBuffer)
{
    int VMAX = 0;
    int VMIN = 0;
    int i = 0;
    VMAX = Get_Voltage(pucBuffer[14], pucBuffer[15]);
    VMIN = Get_Voltage(pucBuffer[16], pucBuffer[17]);
    if ((VMAX > VMIN) && (VMAX < Voltage_MAX_MAX) //��ѹ��ֵ�������������޴���������С��������
        && (VMIN > Voltage_MIN_MIN))              //��ѹ���޴���������
    {
        Voltage_MAX = VMAX;
        Voltage_MIN = VMIN;
        Flash_Write_Word(FLASH_VMAX1_Addr, pucBuffer[14]);
        Flash_Write_Word(FLASH_VMAX2_Addr, pucBuffer[15]);
        Flash_Write_Word(FLASH_VMIN1_Addr, pucBuffer[16]);
        Flash_Write_Word(FLASH_VMIN2_Addr, pucBuffer[17]);
        pucBuffer[8] = 0x94; //���óɹ���־
    }
    else
    {
        pucBuffer[8] = 0xD4; //�����쳣��־
    }
    pucBuffer[9] = 0x00;
    pucBuffer[10] = 0x00; //��ȡУ���
    for (i = 0; i < 10; i++)
    {
        pucBuffer[10] += pucBuffer[i];
    }
    pucBuffer[11] = 0x16; //������
    /*���ڻظ�*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڻظ�*/
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
 * ��    �ܣ�����֡����ѹ��ֵ��ѯ
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����none
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
 **********************************************************************/
void Query_Voltage_Limit(uint8_t *pucBuffer)
{
    int i = 0;
    pucBuffer[8] = 0x91;
    pucBuffer[9] = 0x0C;
    pucBuffer[14] = GetLowByte(Voltage_MAX_MAX);
    pucBuffer[15] = GetHighByte(Voltage_MAX_MAX); //��ѹ������
    pucBuffer[16] = GetLowByte(Voltage_MIN_MIN);
    pucBuffer[17] = GetHighByte(Voltage_MIN_MIN); //��ѹ������
    pucBuffer[18] = GetLowByte(Voltage_MAX);
    pucBuffer[19] = GetHighByte(Voltage_MAX); //��ѹ����ֵ
    pucBuffer[20] = GetLowByte(Voltage_MIN);
    pucBuffer[21] = GetHighByte(Voltage_MIN); //��ѹ����ֵ
    pucBuffer[22] = 0x00;                     //��ȡУ���
    for (i = 0; i < 24 - 2; i++)              //���ĳ���24
    {
        pucBuffer[22] += pucBuffer[i];
    }
    pucBuffer[23] = 0x16; //������
    uc645DataDomainLength = 12;
    /*���ڻظ�*/
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڻظ�*/
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
 * ��    �ܣ���ѹ��ֵ�ָ�����
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
 **********************************************************************/
void Default_Reset(uint8_t *pucBuffer)
{
    int i = 0;
    A_Voltage = WORK_VOLTAGE;                                        //A���ѹֵ��Ϊ��׼ֵ
    B_Voltage = WORK_VOLTAGE;                                        //B���ѹֵ��Ϊ��׼ֵ
    C_Voltage = WORK_VOLTAGE;                                        //C���ѹֵ��Ϊ��׼ֵ
    Voltage_Change_State = Voltage_Change;                           //��ѹ�޸�״̬��Ϊ����Ч��
    Flash_Write_Word(FLASH_VChangeState_Addr, Voltage_Change_State); //�洢��FLASH��
    Voltage_MAX = Voltage_MAX_Default;                               //��ѹ�ϸ���ֵ�ָ�Ĭ�ϲ��Ҵ浵��FLASH��
    Voltage_MIN = Voltage_MIN_Default;
    Flash_Write_Word(FLASH_VMAX1_Addr, GetLowByte(Voltage_MAX));
    Flash_Write_Word(FLASH_VMAX2_Addr, GetHighByte(Voltage_MAX));
    Flash_Write_Word(FLASH_VMIN1_Addr, GetLowByte(Voltage_MIN));
    Flash_Write_Word(FLASH_VMIN2_Addr, GetHighByte(Voltage_MIN));
    pucBuffer[8] = 0x94;  //��ȡУ���
    pucBuffer[9] = 0x00;  //��ȡУ���
    pucBuffer[10] = 0x00; //��ȡУ���
    for (i = 0; i < 10; i++)
    {
        pucBuffer[10] += pucBuffer[i];
    }
    pucBuffer[11] = 0x16; //������
    uc645DataDomainLength = 0;
    /*���ڻظ�*/
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڷ���*/
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
 * ��    �ܣ���ѹ֡������
 * ��    �룺Ӧ�ò㻺��ָ�룬�����㻺��ָ��
 * ��    ����
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2018.5.10
 **********************************************************************/
void vVoltageHandle(uint8_t *pucBuffer)
{
    /*�ȴ���ѹ��֡״̬��λ*/
    Voltage_Frame_Status = FrameReplied;
    /*��ʱ����λ*/
    blFramesCounterStatus = false;
    uwTime2FramesCounter = 0;
    /*��ѹ֡�жϴ���*/
    Voltage_Modifier(pucBuffer);
}

/********************************************************************
 * ��    �ܣ���ѹ�ϸ����жϵ���
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
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
        if (pucBuffer[11] == 0x32) //�����ѹ���ݿ�
        {
            if (BCD_Verity(pucBuffer[14], pucBuffer[15])) //BCDУ��
            {
                A_Voltage = Get_Voltage(pucBuffer[14], pucBuffer[15]);
                if ((A_Voltage > Voltage_MAX) || (A_Voltage < Voltage_MIN)) //�޸�A���ѹ
                {
                    A_Voltage = Voltage_Modifier_Method(A_Voltage);
                    pucBuffer[14] = GetLowByte(A_Voltage);
                    pucBuffer[15] = GetHighByte(A_Voltage);
                }
            }
            if (BCD_Verity(pucBuffer[16], pucBuffer[17])) //BCDУ��
            {
                B_Voltage = Get_Voltage(pucBuffer[16], pucBuffer[17]);
                if ((B_Voltage > Voltage_MAX) || (B_Voltage < Voltage_MIN)) //�޸�B���ѹ
                {
                    B_Voltage = Voltage_Modifier_Method(B_Voltage);
                    pucBuffer[16] = GetLowByte(B_Voltage);
                    pucBuffer[17] = GetHighByte(B_Voltage);
                }
            }
            if (BCD_Verity(pucBuffer[18], pucBuffer[19])) //BCDУ��
            {
                C_Voltage = Get_Voltage(pucBuffer[18], pucBuffer[19]);
                if ((C_Voltage > Voltage_MAX) || (C_Voltage < Voltage_MIN)) //�޸�C���ѹ
                {
                    C_Voltage = Voltage_Modifier_Method(C_Voltage);
                    pucBuffer[18] = GetLowByte(C_Voltage);
                    pucBuffer[19] = GetHighByte(C_Voltage);
                }
            }
        }
        if (pucBuffer[11] == 0x34) //A���ѹ����
        {
            if (BCD_Verity(pucBuffer[14], pucBuffer[15])) //BCDУ��
            {
                A_Voltage = Get_Voltage(pucBuffer[14], pucBuffer[15]);
                if ((A_Voltage > Voltage_MAX) || (A_Voltage < Voltage_MIN)) //�޸�A���ѹ
                {
                    A_Voltage = Voltage_Modifier_Method(A_Voltage);
                    pucBuffer[14] = GetLowByte(A_Voltage);
                    pucBuffer[15] = GetHighByte(A_Voltage);
                }
            }
        }
        if (pucBuffer[11] == 0x35) //B���ѹ����
        {
            if (BCD_Verity(pucBuffer[14], pucBuffer[15])) //BCDУ��
            {
                B_Voltage = Get_Voltage(pucBuffer[14], pucBuffer[15]);
                if ((B_Voltage > Voltage_MAX) || (B_Voltage < Voltage_MIN)) //�޸�B���ѹ
                {
                    B_Voltage = Voltage_Modifier_Method(B_Voltage);
                    pucBuffer[14] = GetLowByte(B_Voltage);
                    pucBuffer[15] = GetHighByte(B_Voltage);
                }
            }
        }
        if (pucBuffer[11] == 0x36) //C���ѹ����
        {
            if (BCD_Verity(pucBuffer[14], pucBuffer[15])) //BCDУ��
            {
                C_Voltage = Get_Voltage(pucBuffer[14], pucBuffer[15]);
                if ((C_Voltage > Voltage_MAX) || (C_Voltage < Voltage_MIN)) //�޸�C���ѹ
                {
                    C_Voltage = Voltage_Modifier_Method(C_Voltage);
                    pucBuffer[14] = GetLowByte(C_Voltage);
                    pucBuffer[15] = GetHighByte(C_Voltage);
                }
            }
        }
        //��ȡУ���
        for (i = 0; i < uc645DataDomainLength + 10; i++)
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[uc645DataDomainLength + 10] = ucCheckData;
    }
    return;
}
/********************************************************************
 * ��    �ܣ��ظ���ѹ֡��֡����
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	true/false
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
 **********************************************************************/
bool blVoltage_Create(uint8_t *pucBuffer)
{
    uint8_t ucCheckData = 0;
    uint8_t i = 0;
    switch (pucBuffer[11])
    {
    case 0x32: //�����ѹ���ݿ�
    {
        pucBuffer[8] = 0x91;
        pucBuffer[9] = 0x0A;
        //ʹ���ϴ��޸ĵĺϷ���ѹֵ��1000 2017-10-25:
        A_Voltage = ((A_Voltage <= Voltage_MAX) && (A_Voltage >= Voltage_MIN) ? A_Voltage : WORK_VOLTAGE);
        pucBuffer[14] = GetLowByte(A_Voltage);
        pucBuffer[15] = GetHighByte(A_Voltage);
        B_Voltage = ((B_Voltage <= Voltage_MAX) && (B_Voltage >= Voltage_MIN) ? B_Voltage : WORK_VOLTAGE);
        pucBuffer[16] = GetLowByte(B_Voltage);
        pucBuffer[17] = GetHighByte(B_Voltage);
        C_Voltage = ((C_Voltage <= Voltage_MAX) && (C_Voltage >= Voltage_MIN) ? C_Voltage : WORK_VOLTAGE);
        pucBuffer[18] = GetLowByte(C_Voltage);
        pucBuffer[19] = GetHighByte(C_Voltage);
        for (i = 0; i < 20; i++) //�̶�֡��22
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[20] = ucCheckData;
        pucBuffer[21] = 0x16;
        return true;
    }
    case 0x34: //A���ѹ����
    {
        A_Voltage = ((A_Voltage <= Voltage_MAX) && (A_Voltage >= Voltage_MIN) ? A_Voltage : WORK_VOLTAGE);
        pucBuffer[8] = 0x91;
        pucBuffer[9] = 0x06;
        pucBuffer[14] = GetLowByte(A_Voltage);
        pucBuffer[15] = GetHighByte(A_Voltage);
        for (i = 0; i < 16; i++) //�̶�֡��18
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[16] = ucCheckData;
        pucBuffer[17] = 0x16;
        return true;
    }
    case 0x35: //B���ѹ����
    {
        B_Voltage = ((B_Voltage <= Voltage_MAX) && (B_Voltage >= Voltage_MIN) ? B_Voltage : WORK_VOLTAGE);
        pucBuffer[8] = 0x91;
        pucBuffer[9] = 0x06;
        pucBuffer[14] = GetLowByte(B_Voltage);
        pucBuffer[15] = GetHighByte(B_Voltage);
        for (i = 0; i < 16; i++) //�̶�֡��18
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[16] = ucCheckData;
        pucBuffer[17] = 0x16;
        return true;
    }
    case 0x36: //C���ѹ����
    {
        C_Voltage = ((C_Voltage <= Voltage_MAX) && (C_Voltage >= Voltage_MIN) ? C_Voltage : WORK_VOLTAGE); //�̶�֡��18
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
        return false; //����յ��ĵ�ѹ֡�����ٲ�A/B/C�࣬����֡���󣬼�ʹ���ظ���ʱ��Ҳ��������ظ�
    }
}
/********************************************************************
 * ��    �ܣ�BCD��У��
 * ��    �룺��ѹ���ݵĵ��ֽں͸��ֽ�
 * ��    ����true��false
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
 **********************************************************************/
bool BCD_Verity(uint8_t LowByte, uint8_t HighByte)
{
    uint8_t hundreds; //��ѹ��λ
    uint8_t tens;     //��ѹʮλ
    uint8_t ones;     //��ѹ��λ
    uint8_t decimals; //��ѹС��λ
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
 * ��    �ܣ�������֡�ж�ȡ��ѹֵ
 * ��    �룺��ѹֵ���ֽڡ����ֽ�
 * ��    ����ʮ���Ƶĵ�ѹֵ
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
 **********************************************************************/
int Get_Voltage(uint8_t LowByte, uint8_t HighByte)
{
    int Voltage = 0;
    uint8_t hundreds; //��ѹ��λ
    uint8_t tens;     //��ѹʮλ
    uint8_t ones;     //��ѹ��λ
    uint8_t decimals; //��ѹС��λ
    hundreds = ((HighByte - 0x33) & 0xF0) >> 4;
    tens = (HighByte - 0x33) & 0x0F;
    ones = ((LowByte - 0x33) & 0xF0) >> 4;
    decimals = (LowByte - 0x33) & 0x0F;
    Voltage = hundreds * 1000 + tens * 100 + ones * 10 + decimals; //Ϊ��ʾС��λ��������������10��
    return Voltage;
}

/********************************************************************
 * ��    �ܣ�������֡�ж�ȡ����ֵ
 * ��    �룺����ֵ���ֽڡ����ֽڡ����ֽ�
 * ��    ����ʮ���Ƶĵ���ֵ
 * Ҫ    ��
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
 **********************************************************************/
void vGet_Current(uint8_t LowByte, uint8_t MidByte, uint8_t HighByte, CURRENT *pcurrent)
{
    uint8_t million;
    uint8_t tenkillo;
    uint8_t killo;
    uint8_t hundreds;
    uint8_t tens;
    uint8_t ones;
    million = ((HighByte - 0x33) & 0x70) >> 4; //ȡbit7~bit5�����λ�Ƿ���λ,����
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
 * ��    �ܣ�������ֵд��֡�ֽ�
 * ��    �룺����ֵ���ֽڡ����ֽڡ����ֽڣ�����ֵ
 * ��    ����
 * Ҫ    �󣺵���ֵ��Ϊ��ֵ
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
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
 * ��    �ܣ���ѹ�޸ĳ�ʼ����
 * ��    �룺	none
 * ��    ����	none
 * �� д �ˣ�������
 * ��д���ڣ�2017.10.25
 **********************************************************************/
void Voltage_Change_Init(void)
{
    uint8_t Vmax1;
    uint8_t Vmax2;
    uint8_t Vmin1;
    uint8_t Vmin2;
    /*��ѹ�޸�ʹ��λ��ʼ��*/
    Voltage_Change_State = EEProm_Read(FLASH_VChangeState_Addr); //��flash�ж�ȡ��ѹ�޸�ʹ���ֽ�
    /*��ѹ�޸������޳�ʼ��*/
    Vmax1 = EEProm_Read(FLASH_VMAX1_Addr);
    Vmax2 = EEProm_Read(FLASH_VMAX2_Addr);
    Vmin1 = EEProm_Read(FLASH_VMIN1_Addr);
    Vmin2 = EEProm_Read(FLASH_VMIN2_Addr);

    if ((Vmax1 == 0xff) && (Vmax2 == 0xff) && (Vmin1 == 0xff) && (Vmin2 == 0xff))
    {
        Voltage_MAX = Voltage_MAX_Default; //���õ�ѹ�ϸ�������Ĭ��ֵ
        Voltage_MIN = Voltage_MIN_Default;
    }
    else
    {
        Voltage_MAX = Get_Voltage(Vmax1, Vmax2); //flash�д洢����654��Ӧ֡�ֽڣ������ֽ�
        Voltage_MIN = Get_Voltage(Vmin1, Vmin2);
    }
}

/*****************************************************
 * ��    �ܣ���ѹ�ϸ��ʵ�������
 * ��    �룺
 * ��    ����
 * ˵    ������ѹƫС�����ʱ����MINΪ��׼����Voltage����������㳬��MAX�����μ�С��Ч����ֵ��MAX-MIN����ֱ��Voltage���ڴ�������;           
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
 *******************************************************/
int Voltage_Modifier_Method(int Voltage)
{
    int n = 0;
    int m = 0;
    int i = 0;
    n = (Voltage_MIN - Voltage_MIN_MIN) / (Voltage_MAX - Voltage_MIN) + 1; //(198-120)/(235-198)+1����(100-90)/(103-100)+1
    m = (Voltage_MAX_MAX - Voltage_MAX) / (Voltage_MAX - Voltage_MIN) + 1; //(260-235)/(235-198)+1����(110-103)/(103-100)+1
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
 * ��    �ܣ��޸ĵ��״̬���еĵ������򡢵�����·��������·״̬��
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2017.5.23
 *******************************************************/
void Estatus_Modify_CurrentEvent(uint8_t *pucBuffer)
{
    uint16_t estatus = 0x0000;
    int i = 0;
    uint8_t ucCheckData = 0x0;
    switch (pucBuffer[10])
    {
    case 0x37: //״̬��4,A��״̬��
    {
        //BIT4,BIT6,bit8 ��0->11111110 10101111:
        estatus = 0xFEAF & Get_Estatus(pucBuffer[14], pucBuffer[15]);
        pucBuffer[14] = (uint8_t)(estatus & 0x00FF) + 0X33;        //״̬�ֵ��ֽ�
        pucBuffer[15] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33; //״̬�ָ��ֽ�
        break;
    }
    case 0x38: //״̬��5,B��״̬��
    {
        estatus = 0xFEAF & Get_Estatus(pucBuffer[14], pucBuffer[15]); //[14][15]ת��Ϊ������
        pucBuffer[14] = (uint8_t)(estatus & 0x00FF) + 0X33;           //״̬�ֵ��ֽ�
        pucBuffer[15] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33;    //״̬�ָ��ֽ�
        break;
    }
    case 0x39: //״̬��6,C��״̬��
    {
        estatus = 0xFEAF & Get_Estatus(pucBuffer[14], pucBuffer[15]); //[14][15]ת��Ϊ������
        pucBuffer[14] = (uint8_t)(estatus & 0x00FF) + 0X33;           //״̬�ֵ��ֽ�
        pucBuffer[15] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33;    //״̬�ָ��ֽ�
        break;
    }
    case 0x3A: //״̬��7������״̬��
    {
        estatus = 0xFFF7 & Get_Estatus(pucBuffer[14], pucBuffer[15]); //BIT3=0   11111111 11110111
        pucBuffer[14] = (uint8_t)(estatus & 0x00FF) + 0X33;           //״̬�ֵ��ֽ�
        pucBuffer[15] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33;    //״̬�ָ��ֽ�
        break;
    }
    case 0x32: //״̬�����ݿ�
    {
        //1:[14][15] 2:[16][17] 3:[18][19] 4:[20][21] 5:[22][23] 6:[24][25] 7:[26][27]
        //״̬��4,A��״̬��
        estatus = 0xFEAF & Get_Estatus(pucBuffer[20], pucBuffer[21]); //BIT4,BIT6,bit8 =0->11111110 10101111
        pucBuffer[20] = (uint8_t)(estatus & 0x00FF) + 0X33;           //״̬�ֵ��ֽ�
        pucBuffer[21] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33;    //״̬�ָ��ֽ�
        //״̬��5,B��״̬��
        estatus = 0xFEAF & Get_Estatus(pucBuffer[22], pucBuffer[23]);
        pucBuffer[22] = (uint8_t)(estatus & 0x00FF) + 0X33;        //״̬�ֵ��ֽ�
        pucBuffer[23] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33; //״̬�ָ��ֽ�
        //״̬��6,C��״̬��
        estatus = 0xFEAF & Get_Estatus(pucBuffer[24], pucBuffer[25]);
        pucBuffer[24] = (uint8_t)(estatus & 0x00FF) + 0X33;        //״̬�ֵ��ֽ�
        pucBuffer[25] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33; //״̬�ָ��ֽ�
        //״̬��7������״̬��
        estatus = 0xFFF7 & Get_Estatus(pucBuffer[26], pucBuffer[27]);
        pucBuffer[26] = (uint8_t)(estatus & 0x00FF) + 0X33;        //״̬�ֵ��ֽ�
        pucBuffer[27] = (uint8_t)((estatus & 0xFF00) >> 8) + 0X33; //״̬�ָ��ֽ�
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
 * ��    �ܣ���������޸ķ���
 * ��    �룺����ο�������һ�����޸ĵ���
 * ��    ������������Ҫ�޸ģ����false
 *           �������޸ģ����true
 * ˵	  ����
 * �� д �ˣ�������
 * ��д���ڣ�2017.12.13
 *******************************************************/
bool blCurrentSingle_Modify_Method(CURRENT *ref1, CURRENT *ref2, CURRENT *result) //ע���и���B���C������޸�A�����
{
    //if Acurrent->count>Bcurrent->count && Acurrent->count>Ccurrent->count,��A�����޸�
    //if Bcurrent->count=Acurrent->count && Acurrent->count>Ccurrent->count,��B����A������B�޸�A
    //if Ccurrent->count=Acurrent->count && Acurrent->count>Bcurrent->count,��C����A������C�޸�A
    //if Bcurrent->count=Ccurrent->count && Acurrent->count<Ccurrent->count,��BC����A������BC�޸�A
    if ((result->count > ref1->count) &&
        (result->count > ref2->count)) //��result�࣬���޸�
        return false;
    else if ((result->count == ref1->count) &&
             (result->count > ref2->count)) //��ref1�࣬��result��,����ref1�޸�result
    {
        if (ref1->value == 0 || result->value == 0 || ref1->isminus || result->isminus)
            return false; //�Ѿ��յ�����������У���0ֵ�������и�ֵ�������޸�
        if (result->value < ref1->value)
        {
            //��resultС��result�޸�Ϊref1�Ĺ̶�������CURRENT_MAX_DVALUE/100
            result->value = ((result->value < (CURRENT_MAX_DVALUE * ref1->value / 100))
                                 ? CURRENT_MAX_DVALUE * ref1->value / 100
                                 : result->value);
        }
        else
        {
            //��result��result�޸�Ϊref1�Ĺ̶�������100/CURRENT_MAX_DVALUE����������Χ����ref1����0ʱ�����޸�
            result->value = ((CURRENT_MAX_DVALUE * result->value / 100 > ref1->value)
                                 ? 100 * ref1->value / CURRENT_MAX_DVALUE
                                 : result->value);
        }
        return true;
    }
    else if ((result->count == ref2->count) && (result->count > ref1->count)) //��ref2�࣬��result�࣬����ref2�޸�result
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
    else if ((result->count == ref1->count) && (result->count == ref2->count)) //ǰ������ref1��ref2�������result��
    {
        if ((ref1->value == 0) || (ref2->value == 0) || (result->value == 0) || ref1->isminus || ref2->isminus || result->isminus)
            return false;
        if (result->value > MAX(ref1->value, ref2->value)) //result��ǰ���඼��result�޸�Ϊ��Сֵ�Ĺ̶�����
        {
            result->value = (CURRENT_MAX_DVALUE * result->value / 100 >= MIN(ref1->value, ref2->value)
                                 ? 100 * MIN(ref1->value, ref2->value) / CURRENT_MAX_DVALUE
                                 : result->value);
        }
        else if (result->value < MIN(ref1->value, ref2->value)) //result��ǰ���඼С��result�޸�Ϊ�ϴ�ֵ�Ĺ̶�����
        {
            result->value = ((result->value < CURRENT_MAX_DVALUE * MAX(ref1->value, ref2->value) / 100)
                                 ? CURRENT_MAX_DVALUE * MAX(ref1->value, ref2->value) / 100
                                 : result->value);
        }
        else //result��ref1��ref2֮�䣬��Ϊǰ��֡��ֵ�ѱ�֤��������Χ������result����Ҫ�޸�
        {
            return false;
        }
        return true;
    }
    return true;
}

/*****************************************************
 * ��    �ܣ���������޸ķ���
 * ��    �룺����ο�����
 * ��    ������
 * ˵	  ����������ֵ��Ϊƽ��ֵ��1.1�����м�ֵ��Ϊƽ��ֵ����Сֵ��Ϊƽ��ֵ��90%��
 *				��ʱ������ֵ�������ֵ�Ĳ�ֵ�����ֵ��18.18%
 *				
 * �� д �ˣ�������
 * ��д���ڣ�2017.12.13
 *******************************************************/
void vCurrentTriple_Modify_Method(CURRENT *current_max, CURRENT *current_mid, CURRENT *current_min)
{
    uint32_t current_average;
    current_average = (current_max->value + current_mid->value + current_min->value) / 3;
    current_max->value = current_average * 11 / 10; //���ֵ��Ϊƽ��ֵ��110%
    current_mid->value = current_average;           //�м�ֵ��Ϊƽ��ֵ
    current_min->value = current_average * 9 / 10;  //��Сֵ��Ϊƽ��ֵ��90%��
}

/*****************************************************
 * ��    �ܣ��޸ĵ��������ֵ
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ������
 * ˵	  ������������������������ֵ����20%�Ľϴ�ֵ���򽫽�Сֵ��Ϊ�ϴ�ֵ��80%          
 *				��������������ܺͲ������޸�
 * �� д �ˣ�������
 * ��д���ڣ�2017.12.13
 *******************************************************/
void vCurrent_Modify(uint8_t *pucBuffer)
{
    uint8_t i;
    uint8_t ucCheckData = 0;
    switch (pucBuffer[11])
    {
    case 0x32: //����������ݿ�
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Acurrent);
        /*����Ϊ��ֵ,���޸ĵ���*/
        if (Acurrent.isminus || Bcurrent.isminus || Ccurrent.isminus)
            return;
        /*����Ϊ0�����޸ĵ���*/
        if ((Acurrent.value == 0) || (Bcurrent.value == 0) || (Ccurrent.value == 0))
            return;
        /*�����໥֮�䲻������޸ĵ���*/
        if (CURRENT_MAX_DVALUE * MAX(Acurrent.value, MAX(Bcurrent.value, Ccurrent.value)) / 100 <=
            MIN(Acurrent.value, MIN(Bcurrent.value, Ccurrent.value)))
            return;
        else //��Ҫ�޸ģ����ֵ��С����Сֵ����ʣ�µ����м�ֵ
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
            //A�����,ʮ������תBCD��
            vPut_Current(Acurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
            //B�����
            vPut_Current(Acurrent, &pucBuffer[17], &pucBuffer[18], &pucBuffer[19]);
            //C�����
            vPut_Current(Acurrent, &pucBuffer[20], &pucBuffer[21], &pucBuffer[22]);
            for (i = 0; i < 23; i++) //�����򳤶�23
            {
                ucCheckData += pucBuffer[i];
            }
            pucBuffer[23] = ucCheckData;
            pucBuffer[24] = 0x16;
        }
        break;
    }
    case 0x34: //A�����
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Acurrent);
        if (((pucBuffer[16] - 0x33) & 0x80) != 0)
        {
            return;
        }
        if (blCurrentSingle_Modify_Method(&Bcurrent, &Ccurrent, &Acurrent)) //����B���C������޸�A��
        {
            //A�����,ʮ������תBCD��
            vPut_Current(Acurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
            for (i = 0; i < 17; i++) //�����򳤶�17
            {
                ucCheckData += pucBuffer[i];
            }
            pucBuffer[17] = ucCheckData;
            pucBuffer[18] = 0x16;
        }
        break;
    }
    case 0x35: //B�����
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Bcurrent);

        if (((pucBuffer[16] - 0x33) & 0x80) != 0) //�����Ǹ�ֵ
        {
            return;
        }
        if (blCurrentSingle_Modify_Method(&Acurrent, &Ccurrent, &Bcurrent)) //����A���C���޸�B�����
        {
            //B�����,ʮ������תBCD��
            vPut_Current(Bcurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
            for (i = 0; i < 17; i++) //�����򳤶�17
            {
                ucCheckData += pucBuffer[i];
            }
            pucBuffer[17] = ucCheckData;
            pucBuffer[18] = 0x16;
        }
        break;
    }
    case 0x36: //C�����
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Ccurrent);

        if (((pucBuffer[16] - 0x33) & 0x80) != 0)
        {
            return;
        }
        if (blCurrentSingle_Modify_Method(&Acurrent, &Bcurrent, &Ccurrent)) //����A���B���޸�C�����
        {
            //B�����,ʮ������תBCD��
            vPut_Current(Ccurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
            for (i = 0; i < 17; i++) //�����򳤶�16
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
 * ��    �ܣ������ع��ز�����ʼ��
 * ��    �룺����֡����ָ��
 * ��    ����
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2018��8��13��16:13:30
 **********************************************************************/
void vCurrent_Limit_DefaultSet(void)
{
    uint32_t uwLimit;
    uint32_t uwCTratio;
/*��ѹ�������Ͷ���������̻����������޸�*/
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
    /*��FLASH��ȡ��ѹ�����β�CT���*/
    uwCTratio = Flash_Read_Word(FLASH_CURRENT_LIMIT_RATIO_ADDR);
    /*�����Ч ����Ϊ�����ϵ�*/
    if (uwCTratio == 0xFFFFFFFF)
    {
        /*CT����Ϊ80*/
        TransFormer.CTratio = 80;
        /*�洢CTֵ*/
        Flash_Write_Word(FLASH_CURRENT_LIMIT_RATIO_ADDR, TransFormer.CTratio);
    }
    else
    {
        /*������Ч����ʼ��ȫ�ֱ���*/
        TransFormer.CTratio = uwCTratio;
    }
    /*��ȡ�������޲���*/
    uwLimit = Flash_Read_Word(FLASH_CURRENT_LIMIT_VALUE_ADDR);
    /*������Ч�������ϵ�*/
    if (uwLimit == 0xFFFFFFFF)
    {
        /*���ݶ������CT��ȼ����������ֵ*/
        TransFormer.CT2_CurrentLimit = (TransFormer.rated_current / TransFormer.CTratio) * 3 * 8 / 10;
        /*�洢��������ֵ*/
        Flash_Write_Word(FLASH_CURRENT_LIMIT_VALUE_ADDR, TransFormer.CT2_CurrentLimit);
    }
    else
    {
        /*������Ч����ʼ��ȫ�ֱ���*/
        TransFormer.CT2_CurrentLimit = uwLimit;
    }
}

/********************************************************************
 * ��    �ܣ��������ֵ�����޸ĺ���
 * ��    �룺����֡����ָ��
 * ��    ����
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2018.1.30
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
    case 0x32: //����������ݿ�
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
        //A�����,ʮ������תBCD��
        vPut_Current(Acurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
        //B�����
        vPut_Current(Bcurrent, &pucBuffer[17], &pucBuffer[18], &pucBuffer[19]);
        //C�����
        vPut_Current(Ccurrent, &pucBuffer[20], &pucBuffer[21], &pucBuffer[22]);
        for (i = 0; i < 23; i++) //�����򳤶�23
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[23] = ucCheckData;
        pucBuffer[24] = 0x16;
        break;
    }
    case 0x34: //A�����
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Acurrent);
        Acurrent.value = (Acurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Acurrent.value);
        vPut_Current(Acurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
        for (i = 0; i < 17; i++) //�����򳤶�17
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[17] = ucCheckData;
        pucBuffer[18] = 0x16;
        break;
    }
    case 0x35: //B�����
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Bcurrent);
        Bcurrent.value = (Bcurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Bcurrent.value);
        vPut_Current(Bcurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
        for (i = 0; i < 17; i++) //�����򳤶�17
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[17] = ucCheckData;
        pucBuffer[18] = 0x16;

        break;
    }
    case 0x36: //C�����
    {
        vGet_Current(pucBuffer[14], pucBuffer[15], pucBuffer[16], &Ccurrent);
        Ccurrent.value = (Ccurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Ccurrent.value);
        vPut_Current(Bcurrent, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16]);
        for (i = 0; i < 17; i++) //�����򳤶�17
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
 * ��    �ܣ��޸ĵ�������֡
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2017.12.13
 *******************************************************/
void vCurrentDirect_Modify(uint8_t *pucBuffer)
{
    uint8_t i = 0;
    uint8_t ucCheckData;
    switch (pucBuffer[11])
    {
    case 0x32: //����������ݿ�
    {
        pucBuffer[16] = (pucBuffer[16] - 0x33) & 0xEF + 0x33; //���bitλΪ1����ʾΪ��ֵ�������λ����Ϊ1
        pucBuffer[19] = (pucBuffer[19] - 0x33) & 0xEF + 0x33; //���bitλΪ1����ʾΪ��ֵ�������λ����Ϊ1
        pucBuffer[22] = (pucBuffer[22] - 0x33) & 0xEF + 0x33; //���bitλΪ1����ʾΪ��ֵ�������λ����Ϊ1
        for (i = 0; i < 23; i++)                              //�����򳤶�23
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[23] = ucCheckData;
        pucBuffer[24] = 0x16;
        break;
    }
    case 0x34: //A�����
    {
        pucBuffer[16] = (pucBuffer[16] - 0x33) & 0xEF + 0x33; //���bitλΪ1����ʾΪ��ֵ�������λ����Ϊ1
        for (i = 0; i < 17; i++)                              //�����򳤶�17
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[17] = ucCheckData;
        pucBuffer[18] = 0x16;
        break;
    }
    case 0x35: //B�����
    {
        pucBuffer[16] = (pucBuffer[16] - 0x33) & 0xEF + 0x33; //���bitλΪ1����ʾΪ��ֵ�������λ����Ϊ1
        for (i = 0; i < 17; i++)                              //�����򳤶�17
        {
            ucCheckData += pucBuffer[i];
        }
        pucBuffer[17] = ucCheckData;
        pucBuffer[18] = 0x16;
        break;
    }
    case 0x36: //C�����
    {
        pucBuffer[16] = (pucBuffer[16] - 0x33) & 0xEF + 0x33; //���bitλΪ1����ʾΪ��ֵ�������λ����Ϊ1
        for (i = 0; i < 17; i++)                              //�����򳤶�16
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
 * ��    �ܣ������ع��ز�������������ֵ����
 * ��    �룺ά��֡����ָ��
 * ��    ����
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2018��8��13��09:39:18
 * ���ܣ�   68 AA AA AA AA AA AA 68 14 07 33 37 35 35 56 34 12 XX 16(123.456A)
 *�ɹ��ظ���68 AA AA AA AA AA AA 68 94 00 XX 16
 *ʧ�ܻظ���68 AA AA AA AA AA AA 68 D4 00 XX 16
 **********************************************************************/
void vCurrent_Limit_Set(uint8_t *pucBuffer)
{
    uint8_t i;
    CURRENT limit;
    pucBuffer[8] = 0xD4;
    /*���ع��ص����汾ʱ���ظ�����֡*/
#ifdef ENABLE_CURRENT_LIMIT
    /*������ֽڼ�ȥ33����BCD����ô�죿*/
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
    /*���ڻظ�*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڻظ�*/
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
 * ��    �ܣ������ع��ز�����CT�������
 * ��    �룺ά��֡����ָ��
 * ��    ����
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2018��8��13��09:39:18
 * ���ܣ�   68 AA AA AA AA AA AA 68 14 06 33 39 35 35 00 01 XX 16(���100)
 *�ɹ��ظ���68 AA AA AA AA AA AA 68 94 00 XX 16
 *ʧ�ܻظ���68 AA AA AA AA AA AA 68 D4 00 XX 16
 **********************************************************************/
void vCurrent_CTRatio_Set(uint8_t *pucBuffer)
{
    uint8_t i;
    uint32_t uwdata = 0;
    pucBuffer[8] = 0xD4;
/*������ֽڼ�ȥ33����BCD����ô�죿*/
/*��֧���ع����޸Ĺ���ʱ���ظ�����֡*/
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
    /*���ڻظ�*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڻظ�*/
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
 * ��    �ܣ������ع��ز�����ѯ����������ֵ
 * ��    �룺ά��֡����ָ��
 * ��    ����
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2018��8��13��09:39:18
 * ���գ�   68 AA AA AA AA AA AA 68 11 04 33 38 35 35 XX 16
 *�ɹ��ظ���68 AA AA AA AA AA AA 68 91 07 33 38 35 35 56 34 12 XX 16(123.456)
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
    /*���ڻظ�*/
    uc645DataDomainLength = 7;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 19);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڻظ�*/
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
 * ��    �ܣ������ع��ز�����ѯ��CT���
 * ��    �룺ά��֡����ָ��
 * ��    ����
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2018��8��13��09:39:18
 * ���գ�   68 AA AA AA AA AA AA 68 11 04 33 3A 35 35 XX 16
 *�ɹ��ظ���68 AA AA AA AA AA AA 68 91 06 33 3A 35 35 00 01 XX 16(���100)
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
    /*���ڻظ�*/
    uc645DataDomainLength = 6;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڻظ�*/
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
 * ��    �ܣ������ع��ز�����ѯ����ѹ������
 * ��    �룺ά��֡����ָ��
 * ��    ����
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2018��8��13��09:39:18
 * ���գ�   68 AA AA AA AA AA AA 68 11 04 33 3B 35 35 XX 16
 *�ɹ��ظ���68 AA AA AA AA AA AA 68 91 06 33 3B 35 35 00 01 XX 16(���100)
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
    /*���ڻظ�*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڻظ�*/
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
 * ��    �ܣ�����֡��������
 * ��    �룺
 * ��    ������
 * ˵	  �����յ���ѹ�ظ�֡�󣬽�����֡��������
 * �� д �ˣ�������
 * ��д���ڣ�2017.12.13
 *******************************************************/
void vCurrent_Count_Clear(void)
{
    Acurrent.count = 0;
    Bcurrent.count = 0;
    Ccurrent.count = 0;
}
/********************************************************************
 * ��    �ܣ�������֡�ж�ȡ״̬��
 * ��    �룺״̬�ֵ��ֽڡ����ֽ�
 * ��    ���������Ƶ�״̬��
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2017.5.23
 **********************************************************************/
uint16_t Get_Estatus(uint8_t LowByte, uint8_t HighByte)
{
    uint16_t estatus;
    estatus = ((HighByte - 0x33) & 0x00FF) << 8; //00010000 00000000
    estatus |= (LowByte - 0X33) & 0X00FF;
    return estatus;
}
/********************************************************************
 * ��    �ܣ�����ѹ��λת��Ϊ���ֽ�
 * ��    �룺ʮ���Ƶ�ѹֵ
 * ��    ����	BCD��
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
 **********************************************************************/
uint8_t GetLowByte(int Voltage)
{
    uint8_t LowByte = 0x00;
    LowByte = (((((Voltage % 1000) % 100) / 10) << 4) & 0xF0) + ((((Voltage % 1000) % 100) % 10) & 0x0F) + 0x33;
    return LowByte;
}

/********************************************************************
 * ��    �ܣ�����ѹ��λ��ʮλת��Ϊ���ֽ�
 * ��    �룺ʮ���Ƶ�ѹֵ
 * ��    ����	BCD��
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
 **********************************************************************/
uint8_t GetHighByte(int Voltage)
{
    uint8_t HighByte = 0x00;
    HighByte = (((uint8_t)(Voltage / 1000) << 4) & 0xF0) + ((uint8_t)((Voltage % 1000) / 100) & 0x0F) + 0x33;
    return HighByte;
}

/********************************************************************
 * ��    �ܣ��ն��������޸Ĺ��ܳ�ʼ����Ĭ���޸ģ�
 * ��    �룺
 * ��    ����
 *
 * �� д �ˣ�������
 * ��д���ڣ�2017.9.6
 **********************************************************************/
//void vDayfrozen_Change_Init(void)
void vEnergy_Modify_Init(void)
{
    uint8_t tmp;
    /*�ն����޸�ʹ��λ��ʼ��*/
    Dayfrozen_Change_State = EEProm_Read(FLASH_DAYFROZENCHANGESTATE_ADDR);
    tmp = EEProm_Read(FLASH_DAYFROZEN_MODIFY_RATIO_ADDR);
    /*�ж�flash���Ƿ��Ѵ洢��ѹ�޸ı���*/
    if (tmp == 0xFF)
    {
        /*ʹ��Ĭ���޸ı���*/
        ucDayfrozen_Modify_Ratio = UCDAYFROZEN_CHANGE_DEFAULE_RATIO;
    }
    else
    {
        ucDayfrozen_Modify_Ratio = tmp;
    }
}

/********************************************************************
 * ��    �ܣ�������֡�ж�ȡ������ֵ
 * ��    �룺������ֵ�ӵ͵����ĸ��ֽ�
 * ��    ����ʮ���Ƶĵ�����ֵ
 * Ҫ    �����ֵ����100��
 * �� д �ˣ�������
 * ��д���ڣ�2016.6.23
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
 * ��    �ܣ���������ֵת��Ϊ645֡�ֽ�
 * ��    �룺������ֵ�����ֽڰ�����˳������
 * ��    ����none
 * Ҫ    �����ֵ����100��
 * �� д �ˣ�������
 * ��д���ڣ�2018.5.10
 **********************************************************************/
void Fill_Energy(uint32_t energy, uint8_t *pFstByte, uint8_t *pScdByte, uint8_t *pTirdByte, uint8_t *pForthByte)
{
    *pFstByte = DEC2BCD(energy % 100) + 0x33;
    *pScdByte = DEC2BCD(energy / 100 % 100) + 0x33;
    *pTirdByte = DEC2BCD(energy / 10000 % 100) + 0x33;
    *pForthByte = DEC2BCD(energy / 1000000 % 100) + 0x33;
}

/********************************************************************
 * ��    �ܣ���ǰ/�ն����������������
 * ��    �룺
 * ��    ����
 *           
 * �� д �ˣ�������
 * ��д���ڣ�2018.5.8
 **********************************************************************/
uint32_t vNow_Frozen_Modify_Mothod(uint32_t nowenergy, uint32_t RealValueAddr, uint32_t ModifiedValueAddr)
{
    uint32_t RealValue;
    uint32_t Modified_Value;
    uint64_t i;
    /*��ȡ�洢���ϴ���ʵֵ���޸�ֵ*/
    RealValue = Flash_Read_Word(RealValueAddr);
    Modified_Value = Flash_Read_Word(ModifiedValueAddr);
    /*�ն˵�һ��ʹ��*/
    if ((RealValue == 0XFFFFFFFF) || (Modified_Value == 0XFFFFFFFF))
    {
        /*����ֵ�洢Ϊ��ʵֵ���޸�ֵ*/
        Flash_Write_Word(RealValueAddr, nowenergy);
        Flash_Write_Word(ModifiedValueAddr, nowenergy);
        /*���޸�*/
        return nowenergy;
    }
    else
    {
        /*������ʵ�����������ۺ��ۼӵ��ϴε��޸�ֵ*/
        /*���ֵ��������*/
        if (nowenergy < RealValue)
        {
            //i= ((uint64_t)(nowenergy+ 99999999- RealValue)* ucDayfrozen_Modify_Ratio)/100;
            /*2018-6-27 11:04:38�������������룬��������0.05kwh����ʱ�����޸�*/
            i = ((uint64_t)(nowenergy + 99999999 - RealValue) * ucDayfrozen_Modify_Ratio + 50) / 100;
        }
        else
        {
            i = ((uint64_t)(nowenergy - RealValue) * ucDayfrozen_Modify_Ratio + 50) / 100;
        }
        Modified_Value += (uint32_t)i;
        /*�ۼ�ֵ��������*/
        Modified_Value = (Modified_Value > 99999999 ? Modified_Value - 99999999 : Modified_Value);
        /*������ֵ�洢Ϊ��ʵֵ*/
        Flash_Write_Word(RealValueAddr, nowenergy);
        /*�µ��޸�ֵ���Ǿ�ֵ�洢*/
        Flash_Write_Word(ModifiedValueAddr, Modified_Value);
        /*�����޸�ֵ*/
        return Modified_Value;
    }
}

/********************************************************************
 * ��    �ܣ���ǰ���������޸ĺ���
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * ��д���ڣ�2018.5.9
 case ID_00010000:	//��ǰ�����й��ܵ���
 case ID_00010100:	//��ǰ�����й�����1����
 case ID_00010200:	//��ǰ�����й�����2����
 case ID_00010300:	//��ǰ�����й�����3����
 case ID_00010400:	//��ǰ�����й�����4����
 case ID_0001FF00:	//��ǰ�����й��������ݿ�
 case ID_00150000:	//��ǰA�������й�
 case ID_00290000:	//��ǰB�������й�
 case ID_003D0000:	//��ǰC�������й�
 **********************************************************************/
void vNowEnergy_Modifier(uint8_t *pucBuffer)
{
    uint16_t i;
    uint32_t energy;    //������ĵ�һ������ֵ
    uint32_t energy_F1; //����1����ר�ñ���
    uint32_t energy_F2; //����2����ר�ñ���
    uint32_t energy_F3; //����3����ר�ñ���
    uint32_t energy_F4; //����4����ר�ñ���
    if (Dayfrozen_Change_State == 0x00)
        return;
    energy = Get_Energy(pucBuffer[14], pucBuffer[15], pucBuffer[16], pucBuffer[17]);
    switch (pucBuffer[11])
    {
    case 0x33:
    {
        switch (pucBuffer[12])
        {
        /*�����й���*/
        case 0x34:
        {
            energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_ALL_NOWENERGY_ADDR,
                                               FLASH_MODIFIED_ALL_NOWENERGY_ADDR);
            break;
        }
        /*A�������й�*/
        case 0x48:
        {
            energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_A_NOWENERGY_ADDR,
                                               FLASH_MODIFIED_A_NOWENERGY_ADDR);
            break;
        }
        /*B�������й�*/
        case 0x5c:
        {
            energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_B_NOWENERGY_ADDR,
                                               FLASH_MODIFIED_B_NOWENERGY_ADDR);
            break;
        }
        /*C�������й�*/
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
    /*�����й��ܷ���1*/
    case 0x34:
    {
        energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_F1_NOWENERGY_ADDR,
                                           FLASH_MODIFIED_F1_NOWENERGY_ADDR);
        break;
    }
    /*�����й��ܷ���2*/
    case 0x35:
    {
        energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_F2_NOWENERGY_ADDR,
                                           FLASH_MODIFIED_F2_NOWENERGY_ADDR);
        break;
    }
    /*�����й��ܷ���3*/
    case 0x36:
    {
        energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_F3_NOWENERGY_ADDR,
                                           FLASH_MODIFIED_F3_NOWENERGY_ADDR);
        break;
    }
    /*�����й��ܷ���4*/
    case 0x37:
    {
        energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_F4_NOWENERGY_ADDR,
                                           FLASH_MODIFIED_F4_NOWENERGY_ADDR);
        break;
    }
    /*�����й������ݿ�*/
    case 0x32:
    {
        /*			2018-6-27 09:07:02�������ٲ��ն������ݿ鷢���ܺ��ķ���֮���в��죬
	 *			ԭ��Ϊ����0.9��С��������λ�����ԣ��ܵ������ڷַ��ʵ���֮�ͣ�
	 *			���ܵ������㷽����Ϊ���ķ��ʷֵ������
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
        /*��ȡ�ܵ�����ʵֵ*/
        energy = Get_Energy(pucBuffer[14], pucBuffer[15], pucBuffer[16], pucBuffer[17]);
        /*�����ܵ�����ʵֵ�������ܵ���ʱʹ��*/
        Flash_Write_Word(FLASH_REAL_ALL_NOWENERGY_ADDR, energy);
        /*���ݷַ��ʵ��������޸ĺ���ܵ���*/
        energy = energy_F1 + energy_F2 + energy_F3 + energy_F4;
        /*�����޸�ֵ�������ܵ���ʱʹ��*/
        Flash_Write_Word(FLASH_MODIFIED_ALL_NOWENERGY_ADDR, energy);
        break;
    }
    default:
        break;
    }
    switch (pucBuffer[11])
    {
    /*�����й��ܣ���A/B/C�������й�*/
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
    /*�����й������ݿ�*/
    case 0x32:
    {
        /*��*/
        Fill_Energy(energy, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16], &pucBuffer[17]);
        /*��*/
        Fill_Energy(energy_F1, &pucBuffer[18], &pucBuffer[19], &pucBuffer[20], &pucBuffer[21]);
        /*��*/
        Fill_Energy(energy_F2, &pucBuffer[22], &pucBuffer[23], &pucBuffer[24], &pucBuffer[25]);
        /*ƽ*/
        Fill_Energy(energy_F3, &pucBuffer[26], &pucBuffer[27], &pucBuffer[28], &pucBuffer[29]);
        /*��*/
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
 * ��    �ܣ��ն��������޸ĺ���
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * ǰ1�ն���ڶ����ٲ⿪ʼ�ģ�
 * ǰ2�ն���������ٲ⿪ʼ�ģ�
 * ǰ3�ն���������ٲ⿪ʼ��
 * ���ϵ�3���ڣ�ǰ1�ա�ǰ2�ա�ǰ3�ն���ÿ���ٲ�Ĵ�����ȫ��һ�Σ����޷�
 * ʵ�֣��ʲ��޸�ǰ2�պ�ǰ3�ն���
 *
	case ID_05060101:	//��1�ն��������й��������ݣ�����1~4��
	case ID_05060102:	//��2�ն��������й��������ݣ�����1~4��
	case ID_05060103:	//��3�ն��������й��������ݣ�����1~4�� 
 * ��д���ڣ�2018.5.9
 **********************************************************************/
void vDayfrozen_Modifier(uint8_t *pucBuffer) //�޸��ն������ݺ���
{
    uint32_t energy, energy_F1, energy_F2, energy_F3, energy_F4;
    uint16_t i;
    /*��1�Ρ���2�Ρ���3���ն������������*/
    if (Dayfrozen_Change_State == 0x00)
        return;
    /*2018-6-27 13:27:07��
	 *���췴����1�ն��������й������ķ���֮�Ͳ�һ�£�
	 *ԭ��Ϊ����0.9ʱС��������λ�����ԣ�
	 *�����й��ܲ��ٳ���0.9��ֱ���ۼ��ķ����޸ĺ��ֵ
	 *energy = Get_Energy(pucBuffer[14], pucBuffer[15], pucBuffer[16], pucBuffer[17]);	
	 *energy = vNow_Frozen_Modify_Mothod(energy, FLASH_REAL_1DAYFROZEN_ADDR, FLASH_MODIFIED_1DAYFROZEN_ADDR);
	 *Fill_Energy(energy, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16], &pucBuffer[17]);
	*/

    /*�����й�����ʵ���*/
    energy_F1 = Get_Energy(pucBuffer[18], pucBuffer[19], pucBuffer[20], pucBuffer[21]);
    energy_F1 = vNow_Frozen_Modify_Mothod(energy_F1, FLASH_REAL_1DAYFROZEN_ADDR + 4, FLASH_MODIFIED_1DAYFROZEN_ADDR + 4);
    Fill_Energy(energy_F1, &pucBuffer[18], &pucBuffer[19], &pucBuffer[20], &pucBuffer[21]);
    /*�����й�����ʵ���*/
    energy_F2 = Get_Energy(pucBuffer[22], pucBuffer[23], pucBuffer[24], pucBuffer[25]);
    energy_F2 = vNow_Frozen_Modify_Mothod(energy_F2, FLASH_REAL_1DAYFROZEN_ADDR + 8, FLASH_MODIFIED_1DAYFROZEN_ADDR + 8);
    Fill_Energy(energy_F2, &pucBuffer[22], &pucBuffer[23], &pucBuffer[24], &pucBuffer[25]);
    /*�����й�ƽ���ʵ���*/
    energy_F3 = Get_Energy(pucBuffer[26], pucBuffer[27], pucBuffer[28], pucBuffer[29]);
    energy_F3 = vNow_Frozen_Modify_Mothod(energy_F3, FLASH_REAL_1DAYFROZEN_ADDR + 12, FLASH_MODIFIED_1DAYFROZEN_ADDR + 12);
    Fill_Energy(energy_F3, &pucBuffer[26], &pucBuffer[27], &pucBuffer[28], &pucBuffer[29]);
    /*�����й��ȷ��ʵ���*/
    energy_F4 = Get_Energy(pucBuffer[30], pucBuffer[31], pucBuffer[32], pucBuffer[33]);
    energy_F4 = vNow_Frozen_Modify_Mothod(energy_F4, FLASH_REAL_1DAYFROZEN_ADDR + 16, FLASH_MODIFIED_1DAYFROZEN_ADDR + 16);
    Fill_Energy(energy_F4, &pucBuffer[30], &pucBuffer[31], &pucBuffer[32], &pucBuffer[33]);
    /*�����й��ܵ���*/
    /*��ȡ��һ����ʵֵ*/
    energy = Get_Energy(pucBuffer[14], pucBuffer[15], pucBuffer[16], pucBuffer[17]);
    /*�洢��ʵֵ*/
    Flash_Write_Word(FLASH_REAL_1DAYFROZEN_ADDR, energy);
    /*�����޸ĺ��ֵ*/
    energy = energy_F1 + energy_F2 + energy_F3 + energy_F4;
    /*�洢�޸ĺ��ֵ*/
    Flash_Write_Word(FLASH_MODIFIED_1DAYFROZEN_ADDR, energy);
    /*��֡*/
    Fill_Energy(energy, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16], &pucBuffer[17]);

    pucBuffer[34] = 0;
    for (i = 0; i < 34; i++)
    {
        pucBuffer[34] += pucBuffer[i];
    }
    pucBuffer[35] = 0x16;
}

/********************************************************************
 * ��    �ܣ������������й��������ݵ�������
 * ��    �룺
 * ��    ����
 * ˵    ���������յ���Ϊ����ֵ������ֱ�Ӱ�������С
 * �� д �ˣ�������
 * ��д���ڣ�2017.9.6
 **********************************************************************/
uint32_t vAccountEnergy_Modify_Mothod(uint32_t dayfrozen)
{
    uint64_t i;
    i = (uint64_t)dayfrozen * ucDayfrozen_Modify_Ratio;
    return (uint32_t)(i / 100);
}

/********************************************************************
 * ��    �ܣ������������й����������޸ĺ���
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����none
 * �� д �ˣ�������
 *
		case ID_00010001:	//��1�����������й��ܵ���
		case ID_00010101:	//��1�����������й�����1����
		case ID_00010201:	//��1�����������й�����2���� 
		case ID_00010301:	//��1�����������й�����3����
		case ID_00010401:	//��1�����������й�����4����
		case ID_0001FF01:	//��1�����������й��������ݿ�
		case ID_00150001:	//��1������A�������й�����
		case ID_00290001:	//��1������B�������й�����
		case ID_003D0001:	//��1������C�������й�����
 * ��д���ڣ�2017.9.6
 **********************************************************************/
void vAccountEnergy_Modify(uint8_t *pucBuffer)
{
    int energy;
    uint16_t i;
    if (Dayfrozen_Change_State == 0x00)
        return;
    if (pucBuffer[11] == 0x32)
    {
        /*�����й��ܵ���*/
        energy = Get_Energy(pucBuffer[14], pucBuffer[15], pucBuffer[16], pucBuffer[17]);
        energy = vAccountEnergy_Modify_Mothod(energy);
        Fill_Energy(energy, &pucBuffer[14], &pucBuffer[15], &pucBuffer[16], &pucBuffer[17]);
        /*�����й�����ʵ���*/
        energy = Get_Energy(pucBuffer[18], pucBuffer[19], pucBuffer[20], pucBuffer[21]);
        energy = vAccountEnergy_Modify_Mothod(energy);
        Fill_Energy(energy, &pucBuffer[18], &pucBuffer[19], &pucBuffer[20], &pucBuffer[21]);
        /*�����й�����ʵ���*/
        energy = Get_Energy(pucBuffer[22], pucBuffer[23], pucBuffer[24], pucBuffer[25]);
        energy = vAccountEnergy_Modify_Mothod(energy);
        Fill_Energy(energy, &pucBuffer[22], &pucBuffer[23], &pucBuffer[24], &pucBuffer[25]);
        /*�����й�ƽ���ʵ���*/
        energy = Get_Energy(pucBuffer[26], pucBuffer[27], pucBuffer[28], pucBuffer[29]);
        energy = vAccountEnergy_Modify_Mothod(energy);
        Fill_Energy(energy, &pucBuffer[26], &pucBuffer[27], &pucBuffer[28], &pucBuffer[29]);
        /*�����й��ȷ��ʵ���*/
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
 * ��    �ܣ������й��������޸Ľ���/ʹ��
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����none
 * �� д �ˣ�������
 * ��д���ڣ�2018.5.6
 **********************************************************************/
void vEnergy_Modify_AbleSet(uint8_t *pucBuffer)
{
    uint16_t i;
    if (pucBuffer[14] == 0xFF) //ʹ��
    {
        Dayfrozen_Change_State = pucBuffer[14];
        Flash_Write_Word(FLASH_DAYFROZENCHANGESTATE_ADDR, Dayfrozen_Change_State);
        pucBuffer[8] = (EEProm_Read(FLASH_DAYFROZENCHANGESTATE_ADDR) == Dayfrozen_Change_State
                            ? 0x94
                            : 0xD4);
    }
    else if (pucBuffer[14] == 0x00) //������
    {
        /*��մ洢�ĵ���ֵ*/
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
    /*���ڻظ�*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڻظ�*/
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
 * ��    �ܣ������й��������޸ı�������
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * �޸ļ�¼���޸ı�����Ϊ�������ֽڱ�ʾ��BCD�룬���ֽ���ǰ�����ֽ��ں�
 * ��д���ڣ�2018.5.6
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
    /*���ڻظ�*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڻظ�*/
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
 * ��    �ܣ������й��������ָ���������
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * �޸ļ�¼��������֧�ֵ��������ߣ����Ը�λʱ�ն����޸ı��������ٸ�λ:2018��11��5��16:11:34
 * ��д���ڣ�2018.5.6
 **********************************************************************/
void vEnergy_Modify_Reset(uint8_t *pucBuffer)
{
    bool cmdValid;
    uint16_t i;

    cmdValid = (pucBuffer[14] == 0x00);
    if (cmdValid)
    {
        /*�����޸�ʹ��*/
        Flash_Write_Word(FLASH_DAYFROZENCHANGESTATE_ADDR, 0xff);
        Dayfrozen_Change_State = 0xff;
        /*�ն����޸ı�������Ϊ90*/
        /*Flash_Write_Word(FLASH_DAYFROZEN_MODIFY_RATIO_ADDR, 90);*/
        /*ucDayfrozen_Modify_Ratio = 90;*/
        /*ɾ���洢��ԭʼ���޸ĺ�ĵ���ֵ�������洢ҳ��*/
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
    /*���ڻظ�*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 12);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڻظ�*/
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
 * ��    �ܣ������й���������ѯ�޸ı���
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * �޸ļ�¼��
 * ��д���ڣ�2018-11-5 16:12:03
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

    pucbuffer[14] = DEC2BCD(ratio % 100); //��λ��ǰ
    pucbuffer[15] = DEC2BCD(ratio / 100); //��λ�ں�
    pucbuffer[16] = 0;
    for (i = 0; i < 16; i++)
    {
        pucbuffer[16] += pucbuffer[i];
    }
    pucbuffer[17] = 0x16;
    /*���ڻظ�*/
    uc645DataDomainLength = 4;
    ucDrv_Buf_PutBytes(pucbuffer, pUartDB->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
#ifdef ENABLE_INFR
    /*����ڻظ�*/
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
 * ��    �ܣ���ѯ�ն��������й��ܵ�ǰ��ʵֵ���޸�ֵ
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * �޸ļ�¼��
 * ��д���ڣ�2018-11-5 16:12:03
 **********************************************************************/
void vEnergy_Modify_ReadValue(uint8_t *pucbuffer)
{
    uint8_t i;
    int dayFrozen;
    uint32_t realValue;
    uint32_t modifiedValue;

    pucbuffer[8] = 0x91;
    pucbuffer[9] = 0x0C;

    /*��ȡ�洢���ϴ���ʵֵ���޸�ֵ*/
    realValue = Flash_Read_Word(FLASH_REAL_1DAYFROZEN_ADDR + 4);
    modifiedValue = Flash_Read_Word(FLASH_MODIFIED_1DAYFROZEN_ADDR + 4);
    /*�ն˵�һ��ʹ��*/
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
    /*���ڻظ�*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucbuffer, pUartDB->pSndbuf, 24);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
/*����ڻظ�*/
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
 * ��    �ܣ���ѹ�ϸ������޸ĺ������������֡����
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * ��д���ڣ�2018.5.6
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
            pucBuffer[8] = 0x94; //���óɹ���־
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
    pucBuffer[11] = 0x16; //������
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
 * ��    �ܣ���ѹ�ϸ������޸ĺ������������֡������
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * ��д���ڣ�2018.5.6
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
            pucBuffer[8] = 0x94; //���óɹ���־
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
 * ��    �ܣ���ѹ�޸������޸ĺ������������֡������
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * ��д���ڣ�2018.5.6
 **********************************************************************/
void vVoltage_SetMaxMaxValue(uint8_t *pucBuffer)
{
}
/********************************************************************
 * ��    �ܣ���ѹ�޸������޸ĺ������������֡������
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * ��д���ڣ�2018.5.6
 **********************************************************************/
void vVoltage_SetMinMinValue(uint8_t *pucBuffer)
{
}
#if 0
/********************************************************************
 * ��    �ܣ���ȡ�����޸Ĳ������Ƿ��޸�/�޸ı���
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * ��д���ڣ�2018.5.6
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
    /*���ڻظ�*/
    uc645DataDomainLength = 0;
    ucDrv_Buf_PutBytes(pucBuffer, pUartDB->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartDB->pSndbuf))
    {
        Uart_DataPut(USART3);
    }
    /*����ڻظ�*/
    ucDrv_Buf_PutBytes(pucBuffer, pUartIR->pSndbuf, 18);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
}
#endif

/********************************************************************
 * ��    �ܣ����ɼ�¼����������ѹ�����޸ĺ���
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����none
 * �� д �ˣ�������
 * ��д���ڣ�2018��8��7��21:08:05
 * ��һ֡��68 AA AA AA AA AA AA 68 91 0X 32 34 43 39 mm hh DD MM YY 11 11 11 11 11 11 22 22 22 22 22 22 33 33 33  33 33 33 44 44 44 44 44 44 55 55 55 55
 *                                 B1
 * ����֡��68 AA AA AA AA AA AA 68 92 0X 32 34 43 39 11 11 11 11 22 22 22 22 33 33 33 33 44 44 44 44 55 55 55 55 55 55 SEQ
 *                                 B2 04             cs 16
 **********************************************************************/
void vLoadProfile_Voltage_Modify(uint8_t *pucbuffer)
{
    uint16_t n = 0; //���ݿ����
    uint16_t length = pucbuffer[9];
    uint16_t ID_length = 0; //�����������ݱ�ʶ����
    uint32_t voltage = 0;
    uint16_t i = 0;

    if (pucbuffer[8] == 0x91 || pucbuffer[8] == 0xB1) //�޺���֡�����֡�ĵ�һ֡
    {
        ID_length = 9; //���ݱ�ʶ*4 + ʱ��*5
    }
    if (pucbuffer[8] == 0x92 || pucbuffer[8] == 0xB2)
    {
        ID_length = 4; //���ݱ�ʶ*4 �����Ժ����SEQ*1��ԭ��Ϊ����2����һ��
    }
    n = (length - ID_length) / 2; //��ѹ���ݵĸ���������Ϳ����ݶ�����
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
 * ��    �ܣ����ɼ�¼�����������������޸ĺ���
 * ��    �룺Ӧ�ò㻺��ָ��
 * ��    ����	none
 * �� д �ˣ�������
 * ��д���ڣ�2018��8��7��21:08:05
 **********************************************************************/
void vLoadProfile_Current_Modify(uint8_t *pucbuffer)
{
    uint16_t n = 0; //���ݿ����
    uint16_t length = pucbuffer[9];
    uint16_t ID_length = 0;
    uint32_t current = 0;
    uint16_t i = 0;
    uint32_t ABCcurrent = 0;

    if (pucbuffer[8] == 0x91 || pucbuffer[8] == 0xB1) //�޺���֡�����֡�ĵ�һ֡
    {
        ID_length = 9; //���ݱ�ʶ*4 + ʱ��*5
    }
    if (pucbuffer[8] == 0x92 || pucbuffer[8] == 0xB2)
    {
        ID_length = 4; //���ݱ�ʶ*4 �����Ժ����SEQ*1��ԭ��Ϊ����2����һ��
    }

    switch (pucbuffer[10])
    {
    /*A�����*/
    case 0x34:
#ifdef ENABLE_CURRENT_LIMIT
        n = (length - ID_length) / 3;
        while (n--)
        {
            if (pucbuffer[10 + ID_length + 3 * n] == 0x32 || //��3����9
                pucbuffer[11 + ID_length + 3 * n] == 0x32 ||
                pucbuffer[12 + ID_length + 3 * n] == 0x32)
                continue;
            /*��ȡ����*/
            vGet_Current(pucbuffer[10 + ID_length + 3 * n],
                         pucbuffer[11 + ID_length + 3 * n],
                         pucbuffer[12 + ID_length + 3 * n],
                         &Acurrent);
            /*�޸ĵ���*/
            Acurrent.value = (Acurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Acurrent.value);
            /*��д����*/
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
        /*B�����*/
    case 0x35:
#ifdef ENABLE_CURRENT_LIMIT
        n = (length - ID_length) / 3;
        while (n--)
        {
            if (pucbuffer[10 + ID_length + 3 * n] == 0x32 ||
                pucbuffer[11 + ID_length + 3 * n] == 0x32 ||
                pucbuffer[12 + ID_length + 3 * n] == 0x32)
                continue;
            /*��ȡ����*/
            vGet_Current(pucbuffer[10 + ID_length + 3 * n],
                         pucbuffer[11 + ID_length + 3 * n],
                         pucbuffer[12 + ID_length + 3 * n],
                         &Bcurrent);
            /*�޸ĵ���*/
            Bcurrent.value = (Bcurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3)
                                                                                  : Bcurrent.value);
            /*��д����*/
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
    case 0x36: //C�����
#ifdef ENABLE_CURRENT_LIMIT
        n = (length - ID_length) / 3;
        while (n--)
        {
            if (pucbuffer[10 + ID_length + 9 * n] == 0x32 ||
                pucbuffer[11 + ID_length + 9 * n] == 0x32 ||
                pucbuffer[12 + ID_length + 9 * n] == 0x32)
                continue;
            /*��ȡ����*/
            vGet_Current(pucbuffer[10 + ID_length + 3 * n],
                         pucbuffer[11 + ID_length + 3 * n],
                         pucbuffer[12 + ID_length + 3 * n],
                         &Ccurrent);
            /*�޸ĵ���*/
            Ccurrent.value = (Ccurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Ccurrent.value);
            /*��д����*/
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
    case 0x32: //�����
#ifdef ENABLE_CURRENT_DEVIATION
        n = (length - ID_length) / 9; //���ݵ����
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

            //����Ϊ��ֵ,���޸ĵ���
            if (Acurrent.isminus || Bcurrent.isminus || Ccurrent.isminus)
                continue;
            //����Ϊ0�����޸ĵ���
            if ((Acurrent.value == 0) || (Bcurrent.value == 0) || (Ccurrent.value == 0))
                continue;
            //�����໥֮�䲻������޸ĵ���
            if (CURRENT_MAX_DVALUE * MAX(Acurrent.value, MAX(Bcurrent.value, Ccurrent.value)) / 100 <=
                MIN(Acurrent.value, MIN(Bcurrent.value, Ccurrent.value)))
                continue;
            else //��Ҫ�޸ģ����ֵ��С����Сֵ����ʣ�µ����м�ֵ
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
                /*��д����*/
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
        n = (length - ID_length) / 9; //���ݵ����
        while (n--)
        {
            /*FF��Ч����*/
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

            /*��ȡ����*/
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
            /*��֤ÿ�������������*/
            Acurrent.value = (Acurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Acurrent.value);
            Bcurrent.value = (Bcurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Bcurrent.value);
            Ccurrent.value = (Ccurrent.value > (TransFormer.CT2_CurrentLimit / 3) ? (TransFormer.CT2_CurrentLimit / 3) : Ccurrent.value);
            /*��д����*/
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
������01
ɽ����02
���죺03
*/
void vRead_Version(uint8_t *pucbuffer)
{
    uint8_t i;
    pucbuffer[8] = 0x91;
    pucbuffer[9] = 0x08;
    pucbuffer[14] = 0x03; //����
    pucbuffer[15] = 0x05; //������
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
/*����ڻظ�*/
#ifdef ENABLE_INFR
    ucDrv_Buf_PutBytes(pucbuffer, pUartIR->pSndbuf, 20);
    if (!blDrv_Buf_IsEmpty(pUartIR->pSndbuf))
    {
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    }
#endif
}
