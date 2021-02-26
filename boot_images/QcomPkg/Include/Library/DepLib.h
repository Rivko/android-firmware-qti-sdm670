/**@file DepLib.h

Copyright (c) 2015 - 2016, Qualcomm Technologies Inc. All rights reserved.<BR>

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 
 --------   ---     -----------------------------------------------------------
 06/13/16   vk      Initial version
=============================================================================*/

#ifndef _DEP_LIB_H_
#define _DEP_LIB_H_

#include <Library/PeCoffLib.h>

EFI_STATUS
SetupDep (IN PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext);

EFI_STATUS
ClearDep (IN OUT PE_COFF_LOADER_IMAGE_CONTEXT  *ImageContext);

#endif
