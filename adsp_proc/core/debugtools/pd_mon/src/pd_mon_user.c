/** vi: tw=128 ts=3 sw=3 et
@file pd_mon_user.c
@brief This file contains the API details for the Protection Domain Monitor, API 2.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/pd_mon/src/pd_mon_user.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

#include "pd_mon_qurt.h"
#include "pd_mon_internal.h"
#include "tms_dll_api.h"

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

void pd_mon_user_term(void); // forward reference

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Implementation Details
/////////////////////////////////////////////////////////////////////

/**
Initialization of service prior to use
@return
None.
*/
DLL_API_GLOBAL void pd_mon_user_init(void)
{
   secure_memset(&pd_mon_internal_user, 0, sizeof(pd_mon_internal_user)); // Initialize Local Storage

   qurt_pimutex_init(&pd_mon_internal_user.mutex); // serialize access

   qurt_pimutex_lock(&pd_mon_internal_user.mutex); // serialize access

   pd_mon_internal_user.handle = pd_dump_init();

   qurt_pimutex_unlock(&pd_mon_internal_user.mutex); // serialize access
}

/**
Termination of service following use
@return
None.
*/
void pd_mon_user_term(void)
{
   qurt_pimutex_lock(&pd_mon_internal_user.mutex); // serialize access

   pd_mon_internal_user.handle = pd_dump_destroy(pd_mon_internal_user.handle);

   qurt_pimutex_unlock(&pd_mon_internal_user.mutex); // serialize access

   qurt_pimutex_destroy(&pd_mon_internal_user.mutex); // serialize access
}
