/**
  @file DbiDumpLib.h
 
  SDI ram dumps 

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/24/16   ai      Initial Version

 =============================================================================*/

#ifndef _DBIDUMPLIB_H_
#define _DBIDUMPLIB_H_

#include <Library/DBIDump.h>

/* Setup DBI Dump Table only */
VOID SetupDBIDumpTable();

/* Init DBI Dump Table and set crashdump cookie */
EFI_STATUS InitDbiDump(); 

/* Save Cpu Context to UefiInfoBlock */
EFI_STATUS SaveUefiContext(EFI_SYSTEM_CONTEXT SystemContext);

#endif /* _DBIDUMPLIB_H_ */

