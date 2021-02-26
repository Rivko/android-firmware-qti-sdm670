/** @file common.c
    This module implements some common function used by all LPASS HALs

    Copyright (c) 2006 Qualcomm Technologies, Incorporated.  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

when       who     what, where, why
------     ---    -------------------------------------------------------
10/27/08    RA     Created file

========================================================================== */
#include "common.h"

/** 
 * Update the fields,specified by the mask, in the specified register with the
 * values. This function does not update atomically, as there should be no other
 * HAL writing to the same memory location. 
 * 
 * @param[in] basePhysAddr Base physical address of the hardware block
 * @param[in] virtOffset   Offset of the base virtual address from the base
 *                         physical address
 * @param[in] regOffset    Offset of the register from the base address.
 * @param[in] mask         Bit Fields to be updated (specified by a 1 in the
 *                         corresponding location. Is not useful in case of a
 *                         write-only register.
 * @param[in] fieldValue   value of the fields 
 * @param[in] isWriteOnly  1 indicates the register is write only
 */
void UpdateRegister(uint32_t *basePhysAddr, int32_t virtOffset, uint32_t regOffset, 
                    uint32_t mask, uint32_t fieldValue, int32_t isWriteOnly)
{
   volatile uint32_t *addr;
   uint32_t temp;

   addr = (volatile uint32_t *)((uint32_t)basePhysAddr + regOffset); 

   if (isWriteOnly)
   {
      temp = 0x0; ///< If writeonly then do not care about existing contents
   }
   else
   {
      temp = *(addr+virtOffset); ///< Read the value in the register
   }
   mask = ~mask;
   temp = temp & mask; ///< Zero-out the fields that will be udpated
   temp = temp | fieldValue; ///< Update with new values
   *(addr+virtOffset) = temp; ///< Write back
}

/**
 * Read the contents of the register. It is left to the caller to interpret the
 * contents appropriately. This suffices the requirements.
 *
 * @param[in] basePhysAddr Base physical address of the hardware block
 * @param[in] virtOffset   Offset of the base virtual address from the base
 *                         physical address
 * @param[in] regOffset    Offset of the register from the base address.
 * @param[out] pValue      Value of the register 
 */
void ReadRegister(uint32_t *basePhysAddr, int32_t virtOffset, uint32_t regOffset, 
                  uint32_t *pValue)
{
   volatile uint32_t *addr;

   addr = (volatile uint32 *)((uint32_t)basePhysAddr + regOffset); 
   *pValue = *(addr+virtOffset);
}

/** 
 * Update the fields,specified by the mask, in the specified register with the
 * values. This function does not update atomically, as there should be no other
 * HAL writing to the same memory location. 
 * 
 * @param[in] regPhysAddr  Register physical address
 * @param[in] virtOffset   Offset of the base virtual address from the base
 *                         physical address
 * @param[in] mask         Bit Fields to be updated (specified by a 1 in the
 *                         corresponding location. Is not useful in case of a
 *                         write-only register.
 * @param[in] fieldValue   value of the fields 
 * @param[in] isWriteOnly  1 indicates the register is write only
 */
void UpdateRegister1(uint32_t regPhysAddr, int32_t virtOffset,
                    uint32_t mask, uint32_t fieldValue, int32_t isWriteOnly)
{
   volatile uint32 *addr;
   uint32 temp;

   addr = (volatile uint32 *)regPhysAddr; 

   if (isWriteOnly)
   {
      temp = 0x0; ///< If writeonly then do not care about existing contents
   }
   else
   {
      temp = *(addr+virtOffset); ///< Read the value in the register
   }
   mask = ~mask;
   temp = temp & mask; ///< Zero-out the fields that will be udpated
   temp = temp | fieldValue; ///< Update with new values
   *(addr+virtOffset) = temp; ///< Write back
}

/**
 * Read the contents of the register. It is left to the caller to interpret the
 * contents appropriately. This suffices the requirements.
 *
 * @param[in] regPhysAddr  Register physical address
 * @param[in] virtOffset   Offset of the base virtual address from the base
 *                         physical address
 * @param[in] regOffset    Offset of the register from the base address.
 * @param[out] pValue      Value of the register 
 */
void ReadRegister1(uint32_t regPhysAddr, int32_t virtOffset, uint32_t *pValue)
{
   volatile uint32_t *addr;

   addr = (volatile uint32_t *)regPhysAddr; 
   *pValue = *(addr+virtOffset);
}

/** 
 * Update the fields,specified by the mask, in the specified register with the
 * values. This function does not update atomically, as there should be no other
 * HAL writing to the same memory location. 
 * 
 * @param[in] regPhysAddr  Register physical address
 * @param[in] virtOffset   Offset of the base virtual address from the base
 *                         physical address
 * @param[in] mask         Bit Fields to be updated (specified by a 1 in the
 *                         corresponding location. Is not useful in case of a
 *                         write-only register.
 * @param[in] fieldValue   value of the fields 
 * @param[in] isWriteOnly  1 indicates the register is write only
 */
void UpdateRegisterCSR(uint32_t regPhysAddr, int32_t virtOffset,
                    uint32_t mask, uint32_t fieldValue, int32_t isWriteOnly)
{
   volatile uint32_t *addr;
   uint32_t temp;

   addr = (volatile uint32 *)regPhysAddr; 

   if (isWriteOnly)
   {
      temp = 0x0; ///< If writeonly then do not care about existing contents
   }
   else
   {
      temp = *(addr+virtOffset); ///< Read the value in the register
   }
   mask = ~mask;
   temp = temp & mask; ///< Zero-out the fields that will be udpated
   temp = temp | fieldValue; ///< Update with new values
   *(addr+virtOffset) = temp; ///< Write back
}

/**
 * Read the contents of the register. It is left to the caller to interpret the
 * contents appropriately. This suffices the requirements.
 *
 * @param[in] regPhysAddr  Register physical address
 * @param[in] virtOffset   Offset of the base virtual address from the base
 *                         physical address
 * @param[in] regOffset    Offset of the register from the base address.
 * @param[out] pValue      Value of the register 
 */
void ReadRegisterCSR(uint32_t regPhysAddr, int32_t virtOffset, uint32_t *pValue)
{
   volatile uint32_t *addr;

   addr = (volatile uint32_t *)regPhysAddr; 
   *pValue = *(addr+virtOffset);
}

/** 
 * Update the fields,specified by the mask, in the specified register with the
 * values. This function does not update atomically, as there should be no other
 * HAL writing to the same memory location. 
 * 
 * @param[in] regPhysAddr  Register physical address
 * @param[in] virtOffset   Offset of the base virtual address from the base
 *                         physical address
 * @param[in] mask         Bit Fields to be updated (specified by a 1 in the
 *                         corresponding location. Is not useful in case of a
 *                         write-only register.
 * @param[in] fieldValue   value of the fields 
 * @param[in] isWriteOnly  1 indicates the register is write only
 */
void UpdateRegisterAVS(uint32_t regPhysAddr, int32_t virtOffset,
                    uint32_t mask, uint32_t fieldValue, int32_t isWriteOnly)
{
   volatile uint32_t *addr;
   uint32_t temp;

   addr = (volatile uint32_t *)regPhysAddr; 

   if (isWriteOnly)
   {
      temp = 0x0; ///< If writeonly then do not care about existing contents
   }
   else
   {
      temp = *(addr+virtOffset); ///< Read the value in the register
   }
   mask = ~mask;
   temp = temp & mask; ///< Zero-out the fields that will be udpated
   temp = temp | fieldValue; ///< Update with new values
   *(addr+virtOffset) = temp; ///< Write back
}

/**
 * Read the contents of the register. It is left to the caller to interpret the
 * contents appropriately. This suffices the requirements.
 *
 * @param[in] regPhysAddr  Register physical address
 * @param[in] virtOffset   Offset of the base virtual address from the base
 *                         physical address
 * @param[in] regOffset    Offset of the register from the base address.
 * @param[out] pValue      Value of the register 
 */
void ReadRegisterAVS(uint32_t regPhysAddr, int32_t virtOffset, uint32_t *pValue)
{
   volatile uint32_t *addr;

   addr = (volatile uint32_t *)regPhysAddr; 
   *pValue = *(addr+virtOffset);
}


