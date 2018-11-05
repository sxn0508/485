#ifndef FLASH_H
#define FLASH_H

#include "stm32f37x.h"

#define FLASH_VMAX1_Addr								0x08031F00
#define FLASH_VMAX2_Addr								0x08031F04   
#define FLASH_VMIN1_Addr								0x08031F08
#define FLASH_VMIN2_Addr								0x08031F0C
#define FLASH_DAYFROZEN_MODIFY_RATIO_ADDR				0x08031F10 //�ն����޸ı����洢��ַ
#define FLASH_VChangeState_Addr							0x08031F14 //��ѹ�޸�״̬FLASH�洢��ַ
#define FLASH_DAYFROZENCHANGESTATE_ADDR					0x08031F18 //�ն����޸�״̬FLASH�洢��ַ
/*�������ء����ز���*/
#define FLASH_CURRENT_LIMIT_VALUE_ADDR					0x08031F1C //��������ֵ����
#define FLASH_CURRENT_LIMIT_RATIO_ADDR					0x08031F20 //��ѹ����ȴ洢��ַ
//#define FLASH_CURRENT_LIMIT_CAPACITY_ADDR				0x08031F24 //��ѹ�������洢��ַ

#define FLASH_ENERGY_PAGE						0x08032000	//������ֵ�洢ҳ��ַ
#define FLASH_REAL_1DAYFROZEN_ADDR				0x08032000	//��1�ն��������й���������ʵֵ�洢��ʼ��ַ���ܡ��⡢�塢ƽ���ȣ�
#define FLASH_REAL_2DAYFROZEN_ADDR				0x08032014	//��2�ն��������й���������ʵֵ�洢��ʼ��ַ���ܡ��⡢�塢ƽ���ȣ�
#define FLASH_REAL_3DAYFROZEN_ADDR				0x08032028	//��3�ն��������й���������ʵֵ�洢��ʼ��ַ���ܡ��⡢�塢ƽ���ȣ�
#define FLASH_MODIFIED_1DAYFROZEN_ADDR			0x0803203C	//��1�ն��������й��������޸�ֵ�洢��ʼ��ַ���ܡ��⡢�塢ƽ���ȣ�
#define FLASH_MODIFIED_2DAYFROZEN_ADDR			0x08032050	//��2�ն��������й��������޸�ֵ�洢��ʼ��ַ���ܡ��⡢�塢ƽ���ȣ�
#define FLASH_MODIFIED_3DAYFROZEN_ADDR			0x08032064	//��3�ն��������й��������޸�ֵ�洢��ʼ��ַ���ܡ��⡢�塢ƽ���ȣ�

#define FLASH_REAL_ALL_NOWENERGY_ADDR			0x08032070	//��ǰ�����й��ܵ�����ʵֵ�洢��ַ
#define FLASH_MODIFIED_ALL_NOWENERGY_ADDR		0x08032074	//��ǰ�����й��ܵ����޸�ֵ�洢��ַ
#define FLASH_REAL_F1_NOWENERGY_ADDR			0x08032078	//��ǰ�����й�����1������ʵֵ�洢��ַ
#define FLASH_MODIFIED_F1_NOWENERGY_ADDR		0x0803207C	//��ǰ�����й�����1�����޸�ֵ�洢��ַ
#define FLASH_REAL_F2_NOWENERGY_ADDR			0x08032080	//��ǰ�����й�����2������ʵֵ�洢��ַ
#define FLASH_MODIFIED_F2_NOWENERGY_ADDR		0x08032084	//��ǰ�����й�����2�����޸�ֵ�洢��ַ
#define FLASH_REAL_F3_NOWENERGY_ADDR			0x08032088	//��ǰ�����й�����3������ʵֵ�洢��ַ
#define FLASH_MODIFIED_F3_NOWENERGY_ADDR		0x0803208C	//��ǰ�����й�����3�����޸�ֵ�洢��ַ
#define FLASH_REAL_F4_NOWENERGY_ADDR			0x08032090	//��ǰ�����й�����4�����޸�ֵ�洢��ַ
#define FLASH_MODIFIED_F4_NOWENERGY_ADDR		0x08032094	//��ǰ�����й�����4�����޸�ֵ�洢��ַ
#define FLASH_REAL_A_NOWENERGY_ADDR				0x08032098	//��ǰA�������й���ʵֵ�洢��ַ
#define FLASH_MODIFIED_A_NOWENERGY_ADDR			0x0803209C	//��ǰA�������й��޸�ֵ�洢��ַ 
#define FLASH_REAL_B_NOWENERGY_ADDR				0x080320A0	//��ǰB�������й���ʵֵ�洢��ַ
#define FLASH_MODIFIED_B_NOWENERGY_ADDR			0x080320A4	//��ǰB�������й��޸�ֵ�洢��ַ
#define FLASH_REAL_C_NOWENERGY_ADDR				0x080320A8	//��ǰC�������й���ʵֵ�洢��ַ
#define FLASH_MODIFIED_C_NOWENERGY_ADDR			0x080320AC	//��ǰC�������й��޸�ֵ�洢��ַ

extern uint32_t Flash_Read_Word(uint32_t addr);										//��4�ֽ�
extern ErrorStatus Flash_Write_Word(uint32_t addr, uint32_t ByteData);		//д4�ֽ�
extern ErrorStatus EEProm_Write(uint32_t addr,uint8_t ByteData);				//д1�ֽ�
extern  uint8_t EEProm_Read(uint32_t addr);											//��1�ֽ�
extern void vInterFlash_Init(void);
extern void Voltage_Limit_Init(void);//���õ�ѹֵ������



#endif
