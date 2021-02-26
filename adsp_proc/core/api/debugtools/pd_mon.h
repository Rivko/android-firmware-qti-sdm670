#ifndef PD_MON_H
#define PD_MON_H
/**
@file pd_mon.h
@brief This file contains the API details for the Program Domain Monitor
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2012 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/api/debugtools/pd_mon.h#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

// USE THE APPROPRIATE NHLOS HEADER FILE TO ACCESS THE API AS DEPENDENCY.
// DIRECT CLIENT DEPENDENCY THROUGH THIS HEADER FILE ALONE IS NOT A SUPPORTED
// OPERATION.

// #include "pd_mon_qurt.h"   // NHLOS QURT

#if !defined(PD_MON_QURT_H)
#error INCLUDE NHLOS API SPECIFIC HEADER
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/** @addtogroup pd_mon_api */ /** @{ */

/**
API, Typedefs and Enumerations
*/
typedef char const* PD_MON_NAME;          /**< Name */

typedef int PD_MON_RESTART;               /**< Restart Counter */

#define PD_MON_RESTART_ONCE   1           /**< Restart Once */
#define PD_MON_RESTART_ALWAYS 0x7fffffff  /**< Restart 2 Billion Times */

typedef void* PD_MON_HANDLE;              /**< Opaque Handle */

#define PD_MON_ERROR    ((PD_MON_HANDLE)0) /**< Error Indication (for Comparision) */

/**
API, Spawn a Program Domain with Restart Monitoring
PD_MON_RESTART -- Default Restart Monitoring Counter
PD_MON_NAME -- Pointer to NULL terminated string, name of the Program Domain
... -- Optional Args
@return
PD_MON_HANDLE -- Error Indication or Program Domain Handle
*/
PD_MON_HANDLE pd_mon_spawn(PD_MON_RESTART*, PD_MON_NAME const, ...) __attribute__((warn_unused_result));

/**
API, Runtime Query of Restart Monitoring Counter
PD_MON_HANDLE -- Program Domain Handle
@return
PD_MON_RESTART -- Restart Monitoring Counter
*/
PD_MON_HANDLE pd_mon_restart_get(PD_MON_HANDLE, PD_MON_RESTART*);

/**
API, Runtime Change of Restart Monitoring Counter
PD_MON_RESTART -- Restart Monitoring Counter
PD_MON_HANDLE -- Program Domain Handle
@return
PD_MON_HANDLE -- Error Indication or Program Domain Handle
*/
PD_MON_HANDLE pd_mon_restart_set(PD_MON_HANDLE, PD_MON_RESTART*);

/**
API, RCINIT, Initialization of service prior to use
@return
None.
*/
void pd_mon_guest_init(void);

/**
API, RCINIT, Termination of service following use
@return
None.
*/
void pd_mon_guest_term(void);

/**
API, RCINIT, Initialization of service prior to use
@return
None.
*/
void pd_mon_guestos_init(void);

/**
API, RCINIT, Termination of service following use
@return
None.
*/
void pd_mon_guestos_term(void);

/** @} */ /* end_addtogroup pd_mon_api */

#if defined(__cplusplus)
}
#endif

#endif
