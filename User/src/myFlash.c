#include "myFlash.h"

uint8_t IsFlashPageEmpty(uint32_t pageAddress)
{
    uint32_t *address = (uint32_t *)pageAddress;
    for (int i = 0; i < (UPAGE_SIZE / sizeof(uint32_t)); ++i)
    {
        if (address[i] != 0xFFFFFFFF)
        {
            return 0; // 页面不是空的
        }
    }
    return 1; // 页面是空的
}

// 函数：查找下一个空闲区域
uint32_t FindFreeFlashRegion()
{
    for (uint32_t address = UFLASH_BASE_ADDRESS; address < (UFLASH_BASE_ADDRESS + UFLASH_SIZE); address += UPAGE_SIZE)
    {
        if (IsFlashPageEmpty(address))
        {
            return address; // 找到空闲区域的起始地址
        }
    }
    return 0; // 没有找到空闲区域
}

HAL_StatusTypeDef Data_Save_To_Flash(uint32_t adress, uint64_t data[], uint8_t size)
{

    /*擦除内存*/
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError;

    EraseInitStruct.TypeErase = adress;
    EraseInitStruct.Banks = FLASH_BANK_1;
    EraseInitStruct.Page = 15;   // 起始页
    EraseInitStruct.NbPages = 1; // 擦除的页数

    /*存入数据*/
    HAL_FLASH_Unlock();
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
    {
        return HAL_ERROR;
    }
    for (uint32_t i = 0; i < size; i++)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, adress, data[i]) != HAL_OK)
        {
            return HAL_ERROR;
        }
        adress += 8;
    }

    HAL_FLASH_Lock();
}

void Data_Read_From_Flash(uint32_t address, uint64_t *data, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        // 读取 64 位数据
        data[i] = *(volatile uint64_t *)(address + i * 8);
    }
}
