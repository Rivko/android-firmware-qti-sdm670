/** @file VarStoreManager.h
   
  Contains Variable Storage Manager functions interface

  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/06/12   vk      Initial Revision.

=============================================================================*/

#include "Uefi.h"

#ifndef __VAR_STORE_MANAGER__
#define __VAR_STORE_MANAGER__

EFI_STATUS
LoadTable (OUT VOID         **Handle,
           IN  UINT8        *TableBuffer,
           IN  UINT32       TableSize, 
           OUT UINT32       *Attributes);

EFI_STATUS
InitializeTable (OUT VOID           **Handle,
                 IN  UINT8          *TableBuffer,
                 IN  UINT32         TableSize,
                 IN  UINT32         Attributes);

EFI_STATUS
GetTableStat (IN  VOID              *Handle,
              OUT UINT64            *MaxStorageSize,
              OUT UINT64            *FreeStorageSize,
              OUT UINT64            *MaxVarSize);

EFI_STATUS
GetVariableEx (IN     VOID         *Handle,
               IN     CHAR16       *VariableName, 
               IN     EFI_GUID     *VendorGuid,
               IN OUT UINTN        *DataSize,
               OUT    VOID         *Data,
               OUT    VOID         *ExtendedRecord OPTIONAL,
               IN OUT UINT32       *ExtendedRecordSize OPTIONAL);

EFI_STATUS
SetVariableEx (IN     VOID         *Handle,
               IN     CHAR16       *VariableName,
               IN     EFI_GUID     *VendorGuid,
               IN     UINTN         DataSize,
               IN     VOID         *Data,
               IN     VOID         *ExtendedRecord OPTIONAL,
               IN     UINTN        *ExtendedRecordSize OPTIONAL);

EFI_STATUS
GetNextVariableNameEx (IN     VOID         *Handle,
                       IN OUT UINTN        *VariableNameSize, 
                       IN OUT CHAR16       *VariableName,
                       IN OUT EFI_GUID     *VendorGuid);
EFI_STATUS
DeleteVariableEx (IN     VOID         *Handle,
                  IN     CHAR16       *VariableName,
                  IN     EFI_GUID     *VendorGuid);


#endif /* __VAR_STORE_MANAGER__ */
