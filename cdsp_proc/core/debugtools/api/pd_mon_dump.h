#ifndef PD_MON_DUMP_H
#define PD_MON_DUMP_H
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
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/api/pd_mon_dump.h#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

FUNCTIONS
  pd_mon_dump_dalcfg_check

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "stdint.h"
#include "com_dtypes.h"
#include "servreg_common.h"

#define PD_DUMP_DISABLED 0
#define PD_DUMP_ENABLED  1


/**
Initialize PD MON Restart Dal Config and check if PD dump is enabled.
@return
PD_DUMP_DISABLED - PD dump Disabled.
PD_DUMP_ENABLED - PD dump Enabled

*/


uint32_t pd_mon_dump_dalcfg_check_is_enabled(void);

/**
Looks into various dump configuration and enables\disables PDR
@return
None
*/

void pd_mon_pd_dump_config(boolean * dump_config);

#endif /* PD_MON_DUMP */
