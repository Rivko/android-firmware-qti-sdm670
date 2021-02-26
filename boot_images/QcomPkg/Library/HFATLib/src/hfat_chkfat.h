/****************************************************************************
 * hfat_chkfat.h
 *
 * Short description.
 * Copyright (C) 2006, 2008 QUALCOMM Technologies, Inc.
 *
 * Verbose Description
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-09-16   ebb   Removed test function declarations.
2008-09-16   yog   Remove trailing commas from the enum declaration.
2008-08-05   ebb   Added HFAT_CHKFAT_MESSAGES flag for diag debug.
2008-05-29   ebb   Removed LOG define and updated stack calculation
2008-04-21   ebb   Modified flags, added bit patterns
2008-04-10   ebb   Exposed internal functions for testing
2006-07-06   sch   Create

===========================================================================*/
#ifndef __CHKFAT_H
#define __CHKFAT_H

/* If defined this will print informational messages out through the DIAG
 * messaging system as chkfat encounters errors on the volume. */
//#define HFAT_CHKFAT_MESSAGES

/* max. stack= ~(CHKFAT_MAX_DIR_DEPTH*200) + 800
 * NOTE: When CHKFAT_MAX_DIR_DEPTH = 16 checkfat will occupy 
 * 4000 bytes on the stack worst case. This value MUST be 
 * altered based on the size of the stack of the caller */
#ifndef CHKFAT_MAX_DIR_DEPTH
#define CHKFAT_MAX_DIR_DEPTH 16
#endif

/* Checkfat return responses */
enum
  {
    FC_NO_ERROR,
    FC_WRITE_ERROR = 50,
    FC_READ_ERROR,
    FC_CLUSTER_ERROR
  };

#endif
