/**
@file sys_m_island.c
@brief This file contains the island part of the System Monitor Framework API 1.0 
service.
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2015-2016 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/sys_m/src/sys_m_island.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $
===============================================================================*/

#include "qurt.h"
#ifdef SYS_M_ISLAND_MODE
#include "uSleep_mode_trans.h"
#endif /* SYS_M_ISLAND_MODE */

extern void sys_m_qurt_fatal_notification_helper(void);
extern void dog_disable_bark_nmi(void);

void sys_m_qurt_fatal_notification(void* no_param)
{
   // prevent warnings
   no_param = NULL;

   // DISABLE DOG BARK AND NMI
   dog_disable_bark_nmi();
   
   #ifdef SYS_M_ISLAND_MODE
   if ( qurt_island_get_status() == TRUE )
   {
     uSleep_exitOnError();
     qurt_exception_shutdown_fatal();
   }
   #endif /* SYS_M_ISLAND_MODE */

   /* Jump to non-island mode function */
   sys_m_qurt_fatal_notification_helper();
}


