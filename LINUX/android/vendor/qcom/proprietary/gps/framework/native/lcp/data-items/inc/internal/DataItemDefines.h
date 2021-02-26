/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemDefines

    Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMDEFINES_H__
#define __IZAT_MANAGER_DATAITEMDEFINES_H__

namespace izat_manager {

#define BREAK_IF_ZERO(ERR,X) if(0==(X)) {result = (ERR); break;}
#define BREAK_IF_NON_ZERO(ERR,X) if(0!=(X)) {result = (ERR); break;}

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMDEFINES_H__
