#ifndef DLT645_H
#define DLT645_H

#include "stm32f37x.h"
#include "BaseDef.h"
#include "Uart.h"
#include "main.h"

/******************************************
����汾���Ʋ��������ݿͻ����������Ӧ���� 
��ѹ������
��ѹ������
��ѹ�ϸ�����Ĭ��ֵ
��ѹ�ϸ�����Ĭ��ֵ
*******************************************/
#ifdef WORK_VOLTAGE_100V

#define WORK_VOLTAGE 1000        //�������ѹ��100V
#define Voltage_MAX_MAX 1100     //��ѹ������
#define Voltage_MIN_MIN 900      //��ѹ������
#define Voltage_MAX_Default 1030 //100V����ĵ��2017-10-25
#define Voltage_MIN_Default 1000 //��ѹ�ϸ�����Ĭ��ֵ��flashδ����ʱ
#else
#define WORK_VOLTAGE 2200        //�������ѹ:220V
#define Voltage_MAX_MAX 2800     //��ѹ������
#define Voltage_MIN_MIN 1200     //��ѹ������
#define Voltage_MAX_Default 2350 //��ѹ�ϸ�����Ĭ��ֵ��flashδ����ʱ #define Voltage_MIN_Default  				1980						//��ѹ�ϸ�����Ĭ��ֵ��flashδ����ʱ
#define Voltage_MIN_Default 1980 //��ѹ�ϸ�����Ĭ��ֵ��flashδ����ʱ
#endif
/*��ѹ���޵�����ز���*/
#define Voltage_Change 0xFF                 //��ѹ��Ҫ�޸�״̬ //STM32�ڲ�flashĬ��ֵΪ0XFF stragen_2017-03-15
#define DAYFROZEN_CHANGE 0xFF               //�ն�����Ҫ�޸�״̬
#define DAYFROZEN_NOCHANGE 0x00             //�����ն����޸�
#define Voltage_NOChange 0x00               //���ε�ѹ�޸�
#define UCDAYFROZEN_CHANGE_DEFAULE_RATIO 90 //Ĭ���ն����޸ı�����90%
#define DLT645_WORDS_DELAY_MAX 20           //645֡���ֽڼ䳬ʱʱ�䣬��λ50����
#define DLT645_FRAMES_DELAY_MAX 25          //645��֡��ʱʱ�䣬��λ50���룬��ʱʱ��Ϊ1.25��

/*���������ֵ������ز���*/
#define CURRENT_MAX_DVALUE 80 //��С����ֵ���ܵ���������ֵ��80%

/*ö�����ͣ���ѹ�����֡���״̬*/
typedef enum
{
    FrameAwaitingReply = 0,
    FrameReplied = !FrameAwaitingReply
} FrameHandleStatus;
/*�ṹ�����ͣ�����ֵ*/
typedef struct current
{
    uint32_t value; //����ֵ,����1000���洢������123.456A������123456����
    uint8_t count;  //�������¼���
    bool isminus;   //������־λ
} CURRENT;
/*�ṹ�����ͣ���ѹ����������ȺͶ��β�ϸ��������ֵ*/
typedef struct
{
    /*����*/
    uint32_t capacity;
    /*�����*/
    uint32_t rated_current;
    /*���β�CT���*/
    uint32_t CTratio;
    /*CT2�β��������������*/
    uint32_t CT2_CurrentLimit;
} TransFormer_TypeDef;

/*�ṹ�����ͣ����ݱ�ʶ���Ӧ�Ĵ�����*/
typedef struct
{
    uint8_t Data_ID[4];
    void (*Handle)(uint8_t *);
} DefDataIDHandle;

/*ȫ�ֱ���*/
/*645֡����ṹ���֡�����򳤶�*/
extern uint8_t ucApp_Buf_DB2ZD[DATA_BUF_SIZE]; //Ӧ�ò㻺��
extern uint8_t ucApp_Buf_ZD2DB[DATA_BUF_SIZE]; //Ӧ�ò㻺��
extern uint8_t ucApp_Buf_INFR[DATA_BUF_SIZE];  //�����Ӧ�û���
extern uint8_t uc645DataDomainLength;          //���յ���֡����������

/*��������ѹ��ֵ*/
extern FrameHandleStatus Voltage_Frame_Status;
extern int Voltage_MAX;              //��ѹ�ϸ�����
extern int Voltage_MIN;              //��ѹ�ϸ�����
extern uint8_t Voltage_Change_State; //��ѹ�Ƿ��޸ı�־λ
/*�������ء����ص�������*/
extern CURRENT Acurrent, Bcurrent, Ccurrent; //����ֵȫ�ֱ���
extern TransFormer_TypeDef TransFormer;
//extern uint32_t			uwCT_Capacity;											//��ѹ����������λkVA
//extern uint32_t			uwCT_Ratio;												//��ѹ��CT��ȣ�һ�β�/���β�
//extern uint32_t			uwCurrent_Limit_Value;	  									//��ѹ�����ص��������β�
//extern uint32_t			uwCT_HeavyLoad_Curents;									//��ѹ�����ص��������β�

/*�������������޸�*/
#ifdef ENABLE_PPLUSENERGY_MODIFY
extern uint8_t ucDayfrozen_Modify_Ratio; //����������λ��BCD��
#endif

/*�ڲ�����*/
static UINT8 GetLowByte(int Voltage);                  //����ѹ��λת��Ϊ���ֽڣ�
static UINT8 GetHighByte(int Voltage);                 //����ѹʮλת��Ϊ���ֽڣ�
static bool BCD_Verity(UINT8 LowByte, UINT8 HighByte); //BCDУ��
static STATUS dwCheckFrame(uint8_t *pucBuffer);        //���֡У���ֽںͽ����ֽ�
static int Get_Voltage(UINT8 LowByte, UINT8 HighByte); //������֡�ж�ȡ��ѹֵ
static void vGet_Current(uint8_t LowByte, uint8_t MidByte, uint8_t HighByte, CURRENT *result);
static void vPut_Current(CURRENT current, uint8_t *pLowByte, uint8_t *pMidByte, uint8_t *pHighByte);
static uint16_t Get_Estatus(UINT8 LowByte, UINT8 HighByte); //������֡�ж�ȡ�������״̬��
static uint32_t Get_Energy(uint8_t FstByte, uint8_t ScdByte, uint8_t TirdByte, uint8_t ForthByte);
static void Fill_Energy(uint32_t energy, uint8_t *pFstByte, uint8_t *pScdByte, uint8_t *pTirdByte, uint8_t *pForthByte);
static bool blCurrentSingle_Modify_Method(CURRENT *ref1, CURRENT *ref2, CURRENT *result); //����ǰ��������޸ĵ��������
static void vCurrentTriple_Modify_Method(CURRENT *current_max, CURRENT *current_mid, CURRENT *current_min);

/*���⺯���ӿ�*/
extern void vDBDataID_Handle(uint8_t *pucBuffer);   //�������ݱ�ʶѡ������
extern void vINFRDataID_Handle(uint8_t *pucBuffer); //�������ݱ�ʶѡ������
extern bool blRecvFrame(UCDRV_BUF *ucDrv_Buf, uint8_t *ucdata_buf);
extern void Voltage_Change_Init(void);             //ʹ�ܵ�ѹ�޸Ĺ��ܣ�����flash�ж�ȡ��ѹ������
extern void vEnergy_Modify_Init(void);             //�ն����޸Ĺ��ܳ�ʼ��
extern void Set_Voltage_Limit(uint8_t *pucBuffer); //����֡����ѹ��ֵ����
extern void vVoltageHandle(uint8_t *pucBuffer);
extern void Voltage_Modifier(uint8_t *pucBuffer);                                                                  //�޸ĵ�ѹ֡�ĵ�ѹ����
extern void vDayfrozen_Modifier(uint8_t *pucBuffer);                                                               //�޸��ն������ݺ���
extern uint32_t vNow_Frozen_Modify_Mothod(uint32_t nowenergy, uint32_t RealValueAddr, uint32_t ModifiedValueAddr); //�ն���͵�ǰ�����޸ķ���
extern void vNowEnergy_Modifier(uint8_t *pucBuffer);                                                               //�޸��ն������ݺ���
extern bool blVoltage_Create(uint8_t *pucBuffer);                                                                  //������ѹ֡
extern void Get_VChange_State(uint8_t *pucBuffer);                                                                 //����֡�����ĵ�ѹ�޸ı�־λ
extern void Query_Voltage_Limit(uint8_t *pucBuffer);                                                               //����֡����ѹ��ֵ��ѯ
extern void Default_Reset(uint8_t *pucBuffer);                                                                     //��ѹ��ֵ�ظ�����
extern int Voltage_Modifier_Method(int Voltage);                                                                   //��ѹ�ϸ��ʵ�������
extern void Estatus_Modify_CurrentEvent(uint8_t *pucBuffer);                                                       //�޸ĵ������״̬��
extern void vCurrentDirect_Modify(uint8_t *pucBuffer);                                                             //�޸ĵ�������
extern void vCurrent_Modify(uint8_t *pucBuffer);                                                                   //���������ֵ�޸�
extern void vCurrent_Count_Clear(void);                                                                            //������������
extern void vEnergy_Modify_AbleSet(uint8_t *pucBuffer);                                                            //�����й�������ʹ��/��������
extern void vEnergy_Modify_RaitoSet(uint8_t *pucBuffer);                                                           //�����й�������ʹ��/��������
extern void vEnergy_Modify_Reset(uint8_t *pucBuffer);                                                              //�������޸Ļظ���������
extern void vAccountEnergy_Modify(uint8_t *pucBuffer);                                                             //�����������й��������޸ĺ���
extern void vVoltage_SetMaxValue(uint8_t *pucbuffer);
extern void vVoltage_SetMinValue(uint8_t *pucbuffer);
extern void vVoltage_SetMaxMaxValue(uint8_t *pucbuffer);
extern void vVoltage_SetMinMinValue(uint8_t *pucbuffer);
extern void vEnergy_Modify_ReadSet(uint8_t *pucbuffer);
extern void vEnergy_Modify_ReadValue(uint8_t *pucbuffer);
extern void vLoadProfile_Voltage_Modify(uint8_t *pucbuffer);
extern void vLoadProfile_Current_Modify(uint8_t *pucbuffer);

/*�����ع��ز�������*/
extern void vCurrent_Limit_DefaultSet(void);                           //�����ع��ز�����ʼ��
extern void vCurrent_Limit_Modify(uint8_t *pucBuffer);                 //��������ֵ�޸�
extern void vCurrent_Limit_Set(uint8_t *pucbuffer);                    //�����ع��ز�������:��������
extern void vCurrent_Limit_ReadSet(uint8_t *pucbuffer);                //�����ع��ز�����ȡ:��������
extern void vCurrent_CTRatio_Set(uint8_t *pucbuffer);                  //�����ع��ز�������:CT���
extern void vCurrent_CTRatio_ReadSet(uint8_t *pucbuffer);              //�����ع��ز�����ȡ:CT���
extern void vCurrent_TransFormer_Capacity_ReadSet(uint8_t *pucbuffer); //�����ع��ز�����ȡ:CT���

/*��ѯ����汾*/
extern void vRead_Version(uint8_t *pucbuffer);

#endif
;
