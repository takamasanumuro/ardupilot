#include "AP_HAL.h"
#include "Storage.h"
#include <AP_Math/AP_Math.h>

/*
  default erase method
 */
bool AP_HAL::Storage::erase(void) //!Erases EEPROM by filling it all with 0
{
    uint8_t blk[16] {};
    uint32_t ofs;
    for (ofs=0; ofs<HAL_STORAGE_SIZE; ofs += sizeof(blk)) {
        uint32_t n = MIN(sizeof(blk), HAL_STORAGE_SIZE - ofs);
        write_block(ofs, blk, n); //!Hardware dependent function to write data to EEPROM
    }
    return true;
}
