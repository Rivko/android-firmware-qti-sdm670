/*========================================================================
Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
========================================================================= */
/**
@file qurt_elite_qdi_driver.h

*/
/*===========================================================================
===========================================================================*/

/** @weakgroup weakf_qurt_elite_memory_map_intro */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_qdi_driver.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/13/2017   rbhatnk      Created file.
========================================================================== */

#ifndef QURT_ELITE_QDI_DRIVER_H
#define QURT_ELITE_QDI_DRIVER_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define QDI_QURT_ELITE_NAME                   "/dev/qurt-elite"
#define QDI_QURT_ELITE_BASE                   256
#define QDI_QURT_ELITE_ADD_REMOVE_PAGE        (QDI_QURT_ELITE_BASE + 0)

/**
 * Use space API to open QDI driver client
 */
static __inline int qurt_elite_qdi_open(void)
{
   return qurt_qdi_open(QDI_QURT_ELITE_NAME);
}

/**
 * Use space API to close QDI driver client
 */
static __inline int qurt_elite_qdi_close(int handle)
{
   return qurt_qdi_close(handle);
}

/**
 * Use space API to Add or Remove pages to/from a pool.
 */
static __inline int qurt_elite_qdi_add_remove_pages(int handle,
      qurt_mem_pool_t pool, bool_t is_add, uint32_t addr, uint32_t num_pages)
{
   return qurt_qdi_handle_invoke(handle, QDI_QURT_ELITE_ADD_REMOVE_PAGE, pool, is_add, addr, num_pages);
}

/**
 * driver init function
 */
int qurt_elite_qdi_init(void);
/**
 * driver deinit function
 */
void qurt_elite_qdi_deinit(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_QDI_DRIVER_H

