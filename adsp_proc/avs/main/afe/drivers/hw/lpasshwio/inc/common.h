#ifndef __COMMON_H__
#define __COMMON_H__

/** @file common.h
    Publicizes common functions used in all LPASS HALs. These functions are not
    exposed outside LPASS HAL implementations.

    Copyright (c) 2008 Qualcomm Technologies, Incorporated.  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

when       who     what, where, why
------     ---    -------------------------------------------------------
10/30/08    RA     Initial creation

========================================================================== */

#if defined(__cplusplus)
extern "C" {
#endif

#include "mmdefs.h"
#include "typedef_drivers.h"  

void UpdateRegister(uint32_t *basePhysAddr, int32_t virtOffset, uint32_t regOffset, 
                    uint32 mask, uint32 fieldValue, int32_t isWriteOnly);
void ReadRegister(uint32_t *basePhysAddr, int32_t virtOffset, uint32_t regOffset, 
                  uint32 *pValue);
void UpdateRegister1(uint32_t regPhysAddr, int32_t virtOffset, 
                     uint32_t mask, uint32_t fieldValue, int32_t isWriteOnly);
void ReadRegister1(uint32_t regPhysAddr, int32_t virtOffset, uint32_t *pValue);

void UpdateRegisterCSR(uint32_t regPhysAddr, int32_t virtOffset, 
                     uint32_t mask, uint32_t fieldValue, int32_t isWriteOnly);
void ReadRegisterCSR(uint32_t regPhysAddr, int32_t virtOffset, uint32_t *pValue);

void UpdateRegisterAVS(uint32_t regPhysAddr, int32_t virtOffset, 
                     uint32_t mask, uint32_t fieldValue, int32_t isWriteOnly);
void ReadRegisterAVS(uint32_t regPhysAddr, int32_t virtOffset, uint32_t *pValue);

/** 
 * Write the input value in to a register specified by the 
 * register address. This register address is a virt address.
 * This function does not write atomically, as there should be 
 * no other HAL writing to the same memory location. 
 * 
 * @param[in] reg_addr     Register virtual address
 * @param[in] value        value to be written 
 * 
 */
static inline void reg_wr(uint32_t reg_addr,uint32_t value)
{
   volatile uint32_t *addr;
   
   addr = (volatile uint32_t *)reg_addr; 
   //qurt_printf("reg_wr addr is 0x%x \n",addr);
   *(addr) = value; ///< Write back
}

/** 
 * read the contents of a register specified by reg_addr. This 
 * register address is a virt address. This function does not 
 * write atomically, as there should be no other HAL writing to 
 * the same memory location. 
 * 
 * @param[in] reg_addr     Register virtual address
 * @param[in] *value_ptr   register value.      
 * 
 */
static inline void reg_rd(uint32_t reg_addr, uint32_t *value_ptr)
{
	volatile uint32_t *addr;

	addr = (volatile uint32 *)reg_addr; 
    //qurt_printf("reg_rd addr is 0x%x \n",addr);
	*value_ptr = *(addr);

}

/** 
 * calucate the virt offset for the specific module such as i2s inteface, pcm interface, avtimer, atime1, atime2, audsync, dma, etc...
 * 
 * @param[in] reg_virt_addr         register virtual base address
 * @param[in] reg_phy_addr         register physical base address     
 * @param[in] reg_module_addr    module register physical address
 * 
 */
static inline int32_t reg_virt_offset(uint32_t reg_virt_addr, uint32_t reg_phy_addr, uint32_t reg_module_addr)
{
   int32_t virt_offset = 0;

   virt_offset = (reg_virt_addr - reg_phy_addr + (reg_module_addr - reg_phy_addr))>>2;

   return virt_offset;
}



#if defined(__cplusplus)
}
#endif

#endif  // defined(__HAL_MIDI_HW_H__)
