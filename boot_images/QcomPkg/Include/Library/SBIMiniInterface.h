/** @file SBIInterface.h

  SBI MINI UEFI Interface

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/05/11   unr     Initial revision for UEFI SBI MINI

=============================================================================*/
#ifndef __SBIMINIINTERFACE_H__
#define __SBIMINIINTERFACE_H__

/**
  VOID SBIMiniInit ()

  @brief
  Initializes the SBI device specified by uBusId

  @param[in]  uBusId                SBI Bus ID

 */
VOID SBIMiniInit (UINT32 uBusId);

/**
  BOOLEAN SBIMiniWriteByte ()

  @brief
  This function writes a single byte to the SBI bus.

  @param[in]  uBusId                SBI Bus ID
  @param[in]  usAddr                The address to write  
  @param[in]  ucData                Data to write to the bus

  @retval TRUE on success and FALSE on failure

 */
BOOLEAN SBIMiniWriteByte (UINT32 uBusId, UINT16 usAddr, UINT8 ucData);

/**
  BOOLEAN SBIMiniReadByte ()

  @brief
  This function reads a single byte from the SBI bus.

  @param[in]  uBusId                SBI Bus ID
  @param[in]  usAddr                The address to read
  @param[in]  *pucData              Pointer to location to store the data read
                                    from the bus

  @retval TRUE on success and FALSE on failure

 */
BOOLEAN SBIMiniReadByte (UINT32 uBusId, UINT16 usAddr, UINT8 *pucData);

#endif /* __SBIMINIINTERFACE_H__ */

