#ifndef QURT_FS_H
#define QURT_FS_H

/**
  @file qurt_fs.h 
  @brief  Prototypes for direct manipulation of the QuRT FS extensions.

  EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2015  by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/

#include "qurt_qdi_driver.h"

int qurt_fs_notfound_callback_add(void (*pfnCallback)(void *arg, int handle, const char *name),
                                  void *argCallback);

int qurt_fs_extrafile_add(const char *name,
                          void *pData,
                          unsigned nBytes);

#endif /* QURT_FS_H */
