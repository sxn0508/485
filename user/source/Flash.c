#include "Flash.h"

/********************************************************************
 * ��    �ܣ�flash��ʼ��
 * ��    �룺none
 * ��    ����	none
 * �� д �ˣ�stragen
 * ��д���ڣ�2016.6.23
 **********************************************************************/
void vInterFlash_Init(void)
{
    FLASH_SetLatency(FLASH_Latency_2);                                      //����д��ʱ
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); //�����־λ
}

/********************************************************************
 * ��    �ܣ� ���ڲ�flashд���ֽ�
 * ��    �룺 EEPROM��һ���洢��ַ��˫�ֶ���
 * ��    ����ERROR/SUCESS
 * �� д �ˣ�stragen
 * ��д���ڣ�2016.6.23
 **********************************************************************/
ErrorStatus EEProm_Write(uint32_t addr, uint8_t ByteData)
{
    uint8_t i = 2;         //flash ��д����
    FLASH_Unlock();        //����д����
    FLASH_ErasePage(addr); //����flash
    while ((FLASH_ProgramHalfWord(addr, ByteData) != FLASH_COMPLETE) && (i--))
        ;
    FLASH_Lock();
    if ((uint16_t) * (__IO uint32_t *)addr != ByteData)
        return ERROR;
    else
        return SUCCESS;
}

/********************************************************************
 * ��    �ܣ� ���ڲ�flashд���ֽ�
 * ��    �룺 EEPROM��һ���洢��ַ�����ֶ���
 * ��    ����ERROR/SUCESS
 * �� д �ˣ�stragen
 * ��д���ڣ�2018.5.8
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
    /*��ȡ����ҳ��ԭʼ����*/
    for (i = 0; i < 256; i++)
    {
        ReadedWord[i] = *(__IO uint32_t *)(PageAddr + i * 4);
    }
    ReadedWord[WordOffset] |= 0XFFFFFFFF;
    ReadedWord[WordOffset] &= WordData;
    FLASH_Unlock();        //����д����
    FLASH_ErasePage(addr); //����flash
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
 * ��    �ܣ� ��FLASH��һ����ַ�϶��ĸ��ֽ�
 * ��    �룺 FLASH��һ���洢��ַ�����ֶ���
 * ��    ����	none
 * �� д �ˣ�stragen
 * ��д���ڣ�2018.5.8
 **********************************************************************/
uint32_t Flash_Read_Word(uint32_t addr)
{
    uint32_t ReadByte;
    ReadByte = *(__IO uint32_t *)addr;
    return ReadByte;
}

/********************************************************************
 * ��    �ܣ� ��EEPROM��һ����ַ�϶������ֽ�
 * ��    �룺 EEPROM��һ���洢��ַ��˫�ֶ���
 * ��    ����	none
 * �� д �ˣ�stragen
 * ��д���ڣ�2016.6.23
 **********************************************************************/
uint8_t EEProm_Read(uint32_t addr)
{
    uint8_t ReadByte;
    ReadByte = (uint8_t) * (__IO uint32_t *)addr;
    return ReadByte;
}
