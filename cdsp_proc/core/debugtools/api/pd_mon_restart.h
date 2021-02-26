#ifndef PD_MON_RESTART_H
#define PD_MON_RESTART_H
/** vi: tw=128 ts=3 sw=3 et
@file dog_hal.c
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
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/api/pd_mon_restart.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

FUNCTIONS
  pd_mon_restart_dalcfg_check

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "stdint.h"
#include "com_dtypes.h"
#include "servreg_common.h"


/**
Initialize PD MON Restart Dal Config and check if PD restart is enabled.
@return
0 - PD restart Disabled.
1 - PD restart Enabled

*/

#define PD_MON_MAX_PD         32


uint32_t pd_mon_restart_dalcfg_check(void);

/**
Looks into various restart configuration and enables\disables PDR
@return
None
*/

void pd_mon_pd_restart_config(boolean * restart_config);

/**
  return asid from pd_mon_internal from the specified index passed in 
   the function 
@return
None
*/
int pd_mon_internal_get_asid(int index);

/**
  return servreg_h from pd_mon_internal from the specified index passed in 
   the function 
@return
None
*/
SERVREG_MON_HANDLE pd_mon_internal_get_servreg_h(int index);


#endif /* PD_MON_RESTART */
