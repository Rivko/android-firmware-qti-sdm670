/*==============================================================================
@file  DALModEnv_userpd.c

Initialization functions on user PD

Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.
==============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALSysCmn.h"
#include "DALQdiDefs.h"

/*=============================================================================
                    DEFINITIONS AND DECLARATIONS
=============================================================================*/
void
_DALSYS_InitMod(DALSYSConfig *pCfg)
{
#ifdef DEVCFG_STRICT_XML
   extern void devcfg_init_new(void);
   devcfg_init_new();
#endif
}
