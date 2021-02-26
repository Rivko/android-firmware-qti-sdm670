/*==============================================================================

    D A L I N T E R R U P T   C O N T R O L L E R    



  DESCRIPTION
   This file is autogenerated for interrupt controller config for this platform.

          Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                 All Rights Reserved.
              QUALCOMM Proprietary/GTDR

  ===========================================================================*/


#include "DALReg.h"
#include "DDIInterruptController.h"
#include "InterruptControllerInterface.h"

static InterruptConfigType InterruptConfigs[] = 
{
  {INVALID_INTERRUPT,DALINTRCTRL_ENABLE_DEFAULT_SETTINGS,""}
};


InterruptPlatformDataType  pInterruptControllerConfigData[] =
{
  {
    InterruptConfigs,
    128,
  }
};
