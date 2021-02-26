/** @file
   ToolSupportLib Interfaces 

 Copyright (c) 2012-, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
   
**/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 07/10/12    shl     Initial version.

=============================================================================*/
#ifndef _TOOLSUPPORTLIB_H_
#define _TOOLSUPPORTLIB_H_

/**
  Implement get the data.

  @param[in] UINT8 * Buffer        Buffer pointer to load the data to.  
  
  @retval EFI_SUCCESS              Successfully returned data.
  @retval EFI_INVALID_PARAMETER    NULL pointer passed in.

**/

EFIAPI EFI_STATUS GetData( UINT8 * Buffer );

/**
  Implement get the data length.

  @param[in] UINT8 * Ptrlen        Pointer to load the data length to.  
  
  @retval EFI_SUCCESS              Successfully returned data.
  @retval EFI_INVALID_PARAMETER    NULL pointer passed in.

**/
EFIAPI EFI_STATUS GetDataLen( UINT8 * PtrLen );

#endif /* _TOOLSUPPORTLIB_H_ */
