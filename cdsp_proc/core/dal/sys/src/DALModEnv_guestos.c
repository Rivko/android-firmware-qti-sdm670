/*==============================================================================
@file  DALModEnv_guestos.c

Initialization functions on Guest OS

        Copyright © 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "DALStdErr.h"
#include "DALFramework.h"

extern void DevCfg_Init(void);

void _DALSYS_InitMod(DALSYSConfig *pCfg)
{
   DevCfg_Init();
}
