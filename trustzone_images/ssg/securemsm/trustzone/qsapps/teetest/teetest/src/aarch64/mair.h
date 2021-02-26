#ifndef MAIR_H
#define MAIR_H

/*===========================================================================

                    T r u s t z o n e   T a r g e t
                          H e a d e r  F i l e

DESCRIPTION
  Describe MMU-specific register setting that outlines memory types

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/teetest/teetest/src/aarch64/mair.h#1 $
$DateTime: 2018/02/06 03:00:17 $
$Author: pwbldsvc $

From securemsm/trustzone/armlib/v8/inc/

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/13   pre     Initial Revision
============================================================================*/

#include "tt_ld.h"

/*****************************************************************************
 *                             !!! CAUTION !!!                               *
 * Indices must match tt_tz.h or else memory type abstraction will not work. *
 *****************************************************************************/

#define BLK_SO        BLK_MAI_0
#define BLK_DVC       BLK_MAI_1
#define BLK_NC        BLK_MAI_2
#define BLK_WT_WA     BLK_MAI_3
#define BLK_WT_NWA    BLK_MAI_4
#define BLK_WB_WA     BLK_MAI_5
#define BLK_WB_NWA    BLK_MAI_6
#define BLK_NOT_USED  BLK_MAI_7

#endif
