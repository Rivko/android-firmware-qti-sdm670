#ifndef DIAG_DEFS_H
#define DIAG_DEFS_H
/*!
@ingroup diag_misc
  @file diag_defs.h 
  @brief

  This file contains definitions used by all of Diag (root, user, island, non-island).
  
*/  

/*
Copyright (c) 2018 by QUALCOMM Technologies, Incorporated. All Rights Reserved.
Qualcomm Confidential and Proprietary
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/api/services/diag_defs.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/19/18   is      Add support for pd threshold buffering mode in island mode
===========================================================================*/


/* Tx Modes */
#define DIAG_TX_MODE_STREAMING         0    /* Streaming mode (default) */
#define DIAG_TX_MODE_BUFFERED_THRESH   1    /* Threshold buffering mode */
#define DIAG_TX_MODE_BUFFERED_CIR      2    /* Circular buffering mode */
#define DIAG_TX_MODE_NOT_SUPPORTED     0xFE /* Feature not supported */
#define DIAG_TX_MODE_INVALID           0xFF /* Invalid mode */
#define DIAG_TX_MODE_LAST              2


#endif /* DIAG_UIMAGE_H */
