#ifndef FLASH_H
#define FLASH_H

#include "stm32f37x.h"

#define FLASH_VMAX1_Addr 0x0801EF00
#define FLASH_VMAX2_Addr 0x0801EF04
#define FLASH_VMIN1_Addr 0x0801EF08
#define FLASH_VMIN2_Addr 0x0801EF0C
#define FLASH_DAYFROZEN_MODIFY_RATIO_ADDR 0x0801EF10 //�ն����޸ı����洢��ַ
#define FLASH_VChangeState_Addr 0x0801EF14           //��ѹ�޸�״̬FLASH�洢��ַ
#define FLASH_DAYFROZENCHANGESTATE_ADDR 0x0801EF18   //�ն����޸�״̬FLASH�洢��ַ
/*�������ء����ز���*/
#define FLASH_CURRENT_LIMIT_VALUE_ADDR 0x0801EF1C //��������ֵ����
#define FLASH_CURRENT_LIMIT_RATIO_ADDR 0x0801EF20 //��ѹ����ȴ洢��ַ
//#define FLASH_CURRENT_LIMIT_CAPACITY_ADDR				0x08031F24 //��ѹ�������洢��ַ

#define FLASH_ENERGY_PAGE 0x0801F000                  //������ֵ�洢ҳ��ַ
#define FLASH_REAL_1DAYFROZEN_ADDR 0x0801F000         //��1�ն��������й���������ʵֵ�洢��ʼ��ַ���ܡ��⡢�塢ƽ���ȣ�
/*#define FLASH_REAL_2DAYFROZEN_ADDR 0x0801F014*/     //��2�ն��������й���������ʵֵ�洢��ʼ��ַ���ܡ��⡢�塢ƽ���ȣ�
/*#define FLASH_REAL_3DAYFROZEN_ADDR 0x0801F028*/     //��3�ն��������й���������ʵֵ�洢��ʼ��ַ���ܡ��⡢�塢ƽ���ȣ�
#define FLASH_MODIFIED_1DAYFROZEN_ADDR 0x0801F03C     //��1�ն��������й��������޸�ֵ�洢��ʼ��ַ���ܡ��⡢�塢ƽ���ȣ�
/*#define FLASH_MODIFIED_2DAYFROZEN_ADDR 0x0801F050*/ //��2�ն��������й��������޸�ֵ�洢��ʼ��ַ���ܡ��⡢�塢ƽ���ȣ�
/*#define FLASH_MODIFIED_3DAYFROZEN_ADDR 0x0801F064*/ //��3�ն��������й��������޸�ֵ�洢��ʼ��ַ���ܡ��⡢�塢ƽ���ȣ�

#define FLASH_REAL_ALL_NOWENERGY_ADDR 0x0801F070     //��ǰ�����й��ܵ�����ʵֵ�洢��ַ
#define FLASH_MODIFIED_ALL_NOWENERGY_ADDR 0x0801F074 //��ǰ�����й��ܵ����޸�ֵ�洢��ַ
#define FLASH_REAL_F1_NOWENERGY_ADDR 0x0801F078      //��ǰ�����й�����1������ʵֵ�洢��ַ
#define FLASH_MODIFIED_F1_NOWENERGY_ADDR 0x0801F07C  //��ǰ�����й�����1�����޸�ֵ�洢��ַ
#define FLASH_REAL_F2_NOWENERGY_ADDR 0x0801F080      //��ǰ�����й�����2������ʵֵ�洢��ַ
#define FLASH_MODIFIED_F2_NOWENERGY_ADDR 0x0801F084  //��ǰ�����й�����2�����޸�ֵ�洢��ַ
#define FLASH_REAL_F3_NOWENERGY_ADDR 0x0801F088      //��ǰ�����й�����3������ʵֵ�洢��ַ
#define FLASH_MODIFIED_F3_NOWENERGY_ADDR 0x0801F08C  //��ǰ�����й�����3�����޸�ֵ�洢��ַ
#define FLASH_REAL_F4_NOWENERGY_ADDR 0x0801F090      //��ǰ�����й�����4�����޸�ֵ�洢��ַ
#define FLASH_MODIFIED_F4_NOWENERGY_ADDR 0x0801F094  //��ǰ�����й�����4�����޸�ֵ�洢��ַ
#define FLASH_REAL_A_NOWENERGY_ADDR 0x0801F098       //��ǰA�������й���ʵֵ�洢��ַ
#define FLASH_MODIFIED_A_NOWENERGY_ADDR 0x0801F09C   //��ǰA�������й��޸�ֵ�洢��ַ
#define FLASH_REAL_B_NOWENERGY_ADDR 0x0801F0A0       //��ǰB�������й���ʵֵ�洢��ַ
#define FLASH_MODIFIED_B_NOWENERGY_ADDR 0x0801F0A4   //��ǰB�������й��޸�ֵ�洢��ַ
#define FLASH_REAL_C_NOWENERGY_ADDR 0x0801F0A8       //��ǰC�������й���ʵֵ�洢��ַ
#define FLASH_MODIFIED_C_NOWENERGY_ADDR 0x0801F0AC   //��ǰC�������й��޸�ֵ�洢��ַ

extern uint32_t Flash_Read_Word(uint32_t addr);                        //��4�ֽ�
extern ErrorStatus Flash_Write_Word(uint32_t addr, uint32_t ByteData); //д4�ֽ�
extern ErrorStatus EEProm_Write(uint32_t addr, uint8_t ByteData);      //д1�ֽ�
extern uint8_t EEProm_Read(uint32_t addr);                             //��1�ֽ�
extern void vInterFlash_Init(void);
extern void Voltage_Limit_Init(void); //���õ�ѹֵ������

#endif
