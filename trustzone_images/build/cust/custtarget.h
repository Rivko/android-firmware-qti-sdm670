#ifndef CUSTTARGET_H
#define CUSTTARGET_H
/*===========================================================================

                           C U S T U S U R F

DESCRIPTION
  Customer file for the MSM6280 UMTS (GSM + WCDMA) SURF build.

  Copyright (c) 1996-2008 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tz/2.0.2/cust/custtarget.h#1 $ $DateTime: 2018/02/06 03:02:41 $ $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
# 10/02/08 gdp     Initial version for 76XX.
---------------------------------------------------------------------------
                            Common Features
---------------------------------------------------------------------------*/

#ifdef FEATURE_KARURA
   #ifdef T_FFA
        #if !defined(FEATURE_ANDROID)
             #define CUST_MOB_MODEL 4033
        #else
            #define CUST_MOB_MODEL 4034
        #endif
   #else
        #if !defined(FEATURE_ANDROID)
             #define CUST_MOB_MODEL 4047
        #else
            #define CUST_MOB_MODEL 4032
        #endif
   #endif
#endif

#endif /* CUSTTARGET_H */


/*****************************************************************************/

