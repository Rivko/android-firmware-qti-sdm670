/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * dangergen_qdi_constants.h
 *
 *  Created on: Jan 22, 2015
 *      Author: kingt
 */

#ifndef DANGERGEN_QDI_CONSTANTS_H
#define DANGERGEN_QDI_CONSTANTS_H

#include "qurt_qdi_constants.h"

#define DANGERGEN_QDI_OPEN      (QDI_PRIVATE + 0)
#define DANGERGEN_QDI_CLOSE     (QDI_PRIVATE + 1)
#define DANGERGEN_QDI_CONFIGURE (QDI_PRIVATE + 2)
#define DANGERGEN_QDI_ENABLE    (QDI_PRIVATE + 3)
#define DANGERGEN_QDI_DISABLE   (QDI_PRIVATE + 4)
#define DANGERGEN_QDI_REFRESH   (QDI_PRIVATE + 5)
#define DANGERGEN_QDI_SWITCH_MODE (QDI_PRIVATE + 6) 

#define DANGERGEN_QDI_DRIVER_NAME "/qos/dangergen"


#endif


