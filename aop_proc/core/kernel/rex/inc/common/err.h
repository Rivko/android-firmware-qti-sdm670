/*============================================================================

FILE:      err.h

DESCRIPTION: Function declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

      Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
         QUALCOMM Proprietary and Confidential

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/inc/common/err.h#1 $
$Date: 2018/02/16 $

============================================================================*/

#ifndef ERR_H
#define ERR_H

#include <stdint.h>
#include <stdlib.h>
#include "comdef.h" 

// This is the macro that most erroring people should call.
#define ERR_FATAL(string, a, b, c) { abort(); }

// Initializes the error handling system.
void err_init(void);

#endif /* ERR_H */

