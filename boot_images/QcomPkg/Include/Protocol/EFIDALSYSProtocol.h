/** @file EFIDALSYSProtocol.h

  DALSYS Protocol for UEFI

  Copyright (c) 2010-2011,2014 Qualcomm Technologies, Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/15/14   aa      Updated INIT Function declaration to accept new parameter
 08/14/14   aa      Updated DEINIT Function declaration to accept new parameter
 05/01/14   asmitp  Renamed header file DALSYSProtocol.h to EFIDALSYSProtocol.h
 01/27/11   sho     Add history

=============================================================================*/

#ifndef __DAL_SYS_PROTOCOL_H__
#define __DAL_SYS_PROTOCOL_H__

#include "DALStdDef.h"
#include "DALSys.h"
#include "DalDevice.h"

#define DALSYS_REVISION 0x0000000000010002

typedef 
void
(EFIAPI *EFI_DALSYS_PROTOCOL_INIT)(
   IN    DALREG_DriverInfoList      *pModDriverList,
   IN    DALProps                   *pDALStringDevInfoList,
   OUT   DALSYSFncPtrTbl            **pFncTblPtr
   );

typedef 
void
(EFIAPI *EFI_DALSYS_PROTOCOL_DEINIT)(
   IN    DALREG_DriverInfoList      *pModDriverList,
   IN    DALProps                   *pDALStringDevInfoList
   );

typedef struct _EFI_DALSYS_PROTOCOL {
   UINT64                     revision;
   EFI_DALSYS_PROTOCOL_INIT   init;
   EFI_DALSYS_PROTOCOL_DEINIT deinit;
} 
EFI_DALSYS_PROTOCOL; 

extern EFI_GUID gEfiDalSysProtocolGuid;

#endif
