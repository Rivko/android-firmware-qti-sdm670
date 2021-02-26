/**
@brief module manager AMDB iterface

 */

/*========================================================================

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

#ifndef _MM_BUILT_IN_MODULES_H_
#define _MM_BUILT_IN_MODULES_H_

#include "adsp_amdb_api.h"
#include "adsp_media_fmt.h"
#include "adsp_vsm_api.h"
#include "adsp_vparams_api.h"

typedef struct mm_module_id_cmp_t
{
   uint32_t mtype;
   uint32_t id1;
   //uint32_t id2; cmp->compressed, to reduce space taken
} mm_module_id_cmp_t;

/**
 * modules in this list must be added to AMDB even if they are not mentioned in the ACDB file.
 * Only if they are marked as not-required in the ACDB file, we can STUB them
 *
 */
mm_module_id_cmp_t mm_backward_compatibility_list[] =
{

};

#define MM_NUM_MOD_IN_BACKWARD_COMPAT_LIST 0

#endif /* _MM_BUILT_IN_MODULES_H_ */
