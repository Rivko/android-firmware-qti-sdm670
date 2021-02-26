/*===========================================================================
  Copyright (c) 2016-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/sampleapp/src/app_smci_test.c#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

  # when       who     what, where, why
  # --------   ---     ---------------------------------------------------------

  ===========================================================================*/
#include <stdint.h>
#include "object.h"
#include "CAppLegacyTest_open.h"
#include "qsee_log.h"

int32_t app_getAppObject(Object credentials, Object *obj_ptr)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "%s:%u", __FUNCTION__, __LINE__);
  return CAppLegacyTest_open(obj_ptr);
}
