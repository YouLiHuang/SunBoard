#ifndef __MYFLASH_H__
#define __MYFLASH_H__

#include "main.h"

#define UFLASH_BASE_ADDRESS 0x08000000
#define UFLASH_SIZE 0x00007fff                                        // 32KB
#define UPAGE_SIZE 0x800                                              // 2KB
#define FLASH_USER_START_ADDR (UFLASH_BASE_ADDRESS + 15 * UPAGE_SIZE) // 用户数据起始地址(使用最后2kb作为用户存储区)

uint8_t IsFlashPageEmpty(uint32_t pageAddress);
uint32_t FindFreeFlashRegion();
HAL_StatusTypeDef Data_Save_To_Flash(uint32_t adress, uint64_t data[], uint8_t size);
void Data_Read_From_Flash(uint32_t address, uint64_t *data, size_t length);

#endif