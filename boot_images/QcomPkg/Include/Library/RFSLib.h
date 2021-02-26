#ifndef __RFS_LIB_H__
#define __RFS_LIB_H__

/*=============================================================================

                     RFS Library Information  Header File

GENERAL DESCRIPTION
  This file contains definitions and functions for RFS Library

Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   vm     Initial creation
            
============================================================================*/

/* This is the function called by OSConfigDxe driver */
EFI_STATUS RFSLib_AMLVariableRegister (VOID);

#endif /* __RFS_LIB_H__ */
