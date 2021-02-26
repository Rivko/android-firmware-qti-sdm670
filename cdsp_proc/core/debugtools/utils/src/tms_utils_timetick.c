/**
@file tms_utils_timetick.c
@brief This file contains the API for the TMS Utilities API 0.x
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2016 - 2017 QUALCOMM Technologies Incorporated.
All rights reserved.
QUALCOMM Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/utils/src/tms_utils_timetick.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#include "tms_utils.h"
#include "DDITimetick.h"
#include "tms_utils_msg.h"
#include "err.h"
#include "tms_dll_api.h"

static DalDeviceHandle* TmsTimetickHandle = NULL;

/**
API, Get System Timer value for the TMS Timetick handle
@param[in]     DalTimetickTime64Type*        Pointer to timetick type used to get timer value
@return        void                          Best effort function, no failure action
*/
DLL_API_GLOBAL void tms_get_timetick(uint64* ticktype_p)
{
   if (NULL != TmsTimetickHandle)
  {
    /* Best effort, no failure action */
    DalTimetick_GetTimetick64(TmsTimetickHandle, ticktype_p);
  }
}

/** =====================================================================
 * Function:
 *     tms_timetick_init
 *
 * Description:
 *     Initialization function in user/root pd.
 *     Initialize TMS SystemTimer Handle.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
DLL_API_GLOBAL void tms_timetick_init(void)
{
  if (DAL_SUCCESS != DalTimetick_Attach("SystemTimer", &TmsTimetickHandle))
  {
    ERR_FATAL("tms_timetick SystemTimer initialization failed", 0, 0, 0);
  }
}
