#include "Flash.h"

/********************************************************************
 * 功    能：flash初始化
 * 输    入：none
 * 输    出：	none
 * 编 写 人：stragen
 * 编写日期：2016.6.23
 **********************************************************************/
void vInterFlash_Init(void)
{
    FLASH_SetLatency(FLASH_Latency_2);                                      //设置写延时
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); //清除标志位
}

/********************************************************************
 * 功    能： 向内部flash写两字节
 * 输    入： EEPROM的一个存储地址，双字对齐
 * 输    出：ERROR/SUCESS
 * 编 写 人：stragen
 * 编写日期：2016.6.23
 **********************************************************************/
ErrorStatus EEProm_Write(uint32_t addr, uint8_t ByteData)
{
    uint8_t i = 2;         //flash 重写次数
    FLASH_Unlock();        //解锁写保护
    FLASH_ErasePage(addr); //擦除flash
    while ((FLASH_ProgramHalfWord(addr, ByteData) != FLASH_COMPLETE) && (i--))
        ;
    FLASH_Lock();
    if ((uint16_t) * (__IO uint32_t *)addr != ByteData)
        return ERROR;
    else
        return SUCCESS;
}

/********************************************************************
 * 功    能： 向内部flash写四字节
 * 输    入： EEPROM的一个存储地址，四字对齐
 * 输    出：ERROR/SUCESS
 * 编 写 人：stragen
 * 编写日期：2018.5.8
 **********************************************************************/
ErrorStatus Flash_Write_Word(uint32_t addr, uint32_t WordData)
{
    //uint32_t test			= addr/ 0x400;
    uint32_t PageAddr = addr - addr % 0x400;
    uint32_t WordOffset = (addr % 0x400) / 4;
    uint32_t ReadedWord[256];
    uint8_t RepeatTime = 2;
    uint16_t i;
    uint16_t j;

    if (addr % 4 != 0)
        return ERROR;
    /*读取所在页的原始数据*/
    for (i = 0; i < 256; i++)
    {
        ReadedWord[i] = *(__IO uint32_t *)(PageAddr + i * 4);
    }
    ReadedWord[WordOffset] |= 0XFFFFFFFF;
    ReadedWord[WordOffset] &= WordData;
    FLASH_Unlock();        //解锁写保护
    FLASH_ErasePage(addr); //擦除flash
    for (j = 0; j < 256; j++)
    {
        while ((FLASH_ProgramWord(PageAddr + j * 4, ReadedWord[j]) != FLASH_COMPLETE) && (RepeatTime--))
            ;
    }
    FLASH_Lock();
    if (*(__IO uint32_t *)addr == WordData)
        return SUCCESS;
    else
        return ERROR;
}

/********************************************************************
 * 功    能： 从FLASH的一个地址上读四个字节
 * 输    入： FLASH的一个存储地址，四字对齐
 * 输    出：	none
 * 编 写 人：stragen
 * 编写日期：2018.5.8
 **********************************************************************/
uint32_t Flash_Read_Word(uint32_t addr)
{
    uint32_t ReadByte;
    ReadByte = *(__IO uint32_t *)addr;
    return ReadByte;
}

/********************************************************************
 * 功    能： 从EEPROM的一个地址上读两个字节
 * 输    入： EEPROM的一个存储地址，双字对齐
 * 输    出：	none
 * 编 写 人：stragen
 * 编写日期：2016.6.23
 **********************************************************************/
uint8_t EEProm_Read(uint32_t addr)
{
    uint8_t ReadByte;
    ReadByte = (uint8_t) * (__IO uint32_t *)addr;
    return ReadByte;
}
