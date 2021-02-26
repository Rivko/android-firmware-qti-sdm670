/** @file PrePiFvFileIo.h

  Header file for loading files from an FV image in pre PI environment

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 06/29/16   bh       Initial revision
=============================================================================*/

#ifndef _PREIPI_FV_IO_H_
#define _PREIPI_FV_IO_H_

EFI_STATUS
LoadFileFromFV0 (IN   CHAR8*    Name, 
                 OUT  UINT8**   FileBuffer,
                 OUT  UINTN*    FileSize);

#endif
