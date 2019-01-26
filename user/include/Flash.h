#ifndef FLASH_H
#define FLASH_H

#include "stm32f37x.h"

#define FLASH_VMAX1_Addr 0x0801EF00
#define FLASH_VMAX2_Addr 0x0801EF04
#define FLASH_VMIN1_Addr 0x0801EF08
#define FLASH_VMIN2_Addr 0x0801EF0C
#define FLASH_DAYFROZEN_MODIFY_RATIO_ADDR 0x0801EF10 //日冻结修改比例存储地址
#define FLASH_VChangeState_Addr 0x0801EF14           //电压修改状态FLASH存储地址
#define FLASH_DAYFROZENCHANGESTATE_ADDR 0x0801EF18   //日冻结修改状态FLASH存储地址
/*电流重载、过载参数*/
#define FLASH_CURRENT_LIMIT_VALUE_ADDR 0x0801EF1C //电流上限值设置
#define FLASH_CURRENT_LIMIT_RATIO_ADDR 0x0801EF20 //变压器变比存储地址
//#define FLASH_CURRENT_LIMIT_CAPACITY_ADDR				0x08031F24 //变压器容量存储地址

#define FLASH_ENERGY_PAGE 0x0801F000                  //电能量值存储页地址
#define FLASH_REAL_1DAYFROZEN_ADDR 0x0801F000         //上1日冻结正向有功电能量真实值存储起始地址（总、尖、峰、平、谷）
/*#define FLASH_REAL_2DAYFROZEN_ADDR 0x0801F014*/     //上2日冻结正向有功电能量真实值存储起始地址（总、尖、峰、平、谷）
/*#define FLASH_REAL_3DAYFROZEN_ADDR 0x0801F028*/     //上3日冻结正向有功电能量真实值存储起始地址（总、尖、峰、平、谷）
#define FLASH_MODIFIED_1DAYFROZEN_ADDR 0x0801F03C     //上1日冻结正向有功电能量修改值存储起始地址（总、尖、峰、平、谷）
/*#define FLASH_MODIFIED_2DAYFROZEN_ADDR 0x0801F050*/ //上2日冻结正向有功电能量修改值存储起始地址（总、尖、峰、平、谷）
/*#define FLASH_MODIFIED_3DAYFROZEN_ADDR 0x0801F064*/ //上3日冻结正向有功电能量修改值存储起始地址（总、尖、峰、平、谷）

#define FLASH_REAL_ALL_NOWENERGY_ADDR 0x0801F070     //当前正向有功总电能真实值存储地址
#define FLASH_MODIFIED_ALL_NOWENERGY_ADDR 0x0801F074 //当前正向有功总电能修改值存储地址
#define FLASH_REAL_F1_NOWENERGY_ADDR 0x0801F078      //当前正向有功费率1电能真实值存储地址
#define FLASH_MODIFIED_F1_NOWENERGY_ADDR 0x0801F07C  //当前正向有功费率1电能修改值存储地址
#define FLASH_REAL_F2_NOWENERGY_ADDR 0x0801F080      //当前正向有功费率2电能真实值存储地址
#define FLASH_MODIFIED_F2_NOWENERGY_ADDR 0x0801F084  //当前正向有功费率2电能修改值存储地址
#define FLASH_REAL_F3_NOWENERGY_ADDR 0x0801F088      //当前正向有功费率3电能真实值存储地址
#define FLASH_MODIFIED_F3_NOWENERGY_ADDR 0x0801F08C  //当前正向有功费率3电能修改值存储地址
#define FLASH_REAL_F4_NOWENERGY_ADDR 0x0801F090      //当前正向有功费率4电能修改值存储地址
#define FLASH_MODIFIED_F4_NOWENERGY_ADDR 0x0801F094  //当前正向有功费率4电能修改值存储地址
#define FLASH_REAL_A_NOWENERGY_ADDR 0x0801F098       //当前A相正向有功真实值存储地址
#define FLASH_MODIFIED_A_NOWENERGY_ADDR 0x0801F09C   //当前A相正向有功修改值存储地址
#define FLASH_REAL_B_NOWENERGY_ADDR 0x0801F0A0       //当前B相正向有功真实值存储地址
#define FLASH_MODIFIED_B_NOWENERGY_ADDR 0x0801F0A4   //当前B相正向有功修改值存储地址
#define FLASH_REAL_C_NOWENERGY_ADDR 0x0801F0A8       //当前C相正向有功真实值存储地址
#define FLASH_MODIFIED_C_NOWENERGY_ADDR 0x0801F0AC   //当前C相正向有功修改值存储地址

extern uint32_t Flash_Read_Word(uint32_t addr);                        //读4字节
extern ErrorStatus Flash_Write_Word(uint32_t addr, uint32_t ByteData); //写4字节
extern ErrorStatus EEProm_Write(uint32_t addr, uint8_t ByteData);      //写1字节
extern uint8_t EEProm_Read(uint32_t addr);                             //读1字节
extern void vInterFlash_Init(void);
extern void Voltage_Limit_Init(void); //设置电压值上下限

#endif
