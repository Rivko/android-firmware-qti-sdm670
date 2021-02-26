#ifndef ERR_INJECT_CRASH_SETTINGS_H
#define ERR_INJECT_CRASH_SETTINGS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      ERR Inject Crash Settings constants
                
GENERAL DESCRIPTION
  Defines constants that may be used in configuring ERR Inject Crash Settings

Copyright (c) 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.qdsp6/2.1/settings/debugtools/eic/interface/err_inject_crash_settings.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/25/17   jlg     File created. Establish settings interface

===========================================================================*/
/*===========================================================================

                     EXTERNAL DATA DECLARATIONS

===========================================================================*/

typedef enum
{
  ERR_INJECT_ERR_FATAL,
  ERR_INJECT_WDOG_TIMEOUT,
  ERR_INJECT_NULLPTR,
  ERR_INJECT_DIV0,
  ERR_INJECT_DYNAMIC_ERR_FATAL,
  ERR_INJECT_WDOG_NMI,
  ERR_INJECT_WDOG_MULTITHREADED,
  SIZEOF_ERR_INJECT_CRASH_TYPE
} err_inject_crash_type;

#endif /* ERR_INJECT_CRASH_SETTINGS_H */
