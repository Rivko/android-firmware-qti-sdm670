/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Helper macros and defines used by internal and exernal proprietary modules only.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IZATDEFINES_H__
#define __IZAT_MANAGER_IZATDEFINES_H__

namespace izat_manager
{
#define BREAK_IF_ZERO(ERR,X) if(0==(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO(ERR,X) if(0!=(X)) {result = (ERR); break;}

}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IZATDEFINES_H__
