/**@file WatchDogLib.c

  Copyright (c) 2017, Qualcomm Technologies Inc. All rights reserved.<BR>

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 
 --------   ---     -----------------------------------------------------------
 08/04/17   c_mku   Added Watchdog pet Mechanism
 06/13/16   vk      Initial version
=============================================================================*/
#include <Library/ArmLib.h>
#include <Library/WatchdogLib.h>
#include <WatchDogLibHWIO.h>

#define SLEEP_CLKS_PER_SEC 0x8000
//define max bite for 32 sec in msec
#define MAX_BITE_TIME_IN_MSEC (32*SEC_TO_MSEC(1))
#define FORCED_BITE_TIME_MS 4


/*==============================================================================
**  Functions
**============================================================================*/

/*==============================================================================
  FUNCTION      WatchDogEnable

  DESCRIPTION   Set Bit 0 in WDOG_CTL to Enable WatchDogReset 
  

==============================================================================*/

VOID
WatchDogEnable ( VOID )
{
  HWIO_APSS_WDT_TMR1_WDOG_CTL_OUT(HWIO_APSS_WDT_TMR1_WDOG_CTL_ENABLE_ENABLE_FVAL);
  ArmInstructionSynchronizationBarrier();
}

/*==============================================================================
  FUNCTION      WatchDogDisable

  DESCRIPTION   Reset Bit 0 in WDOG_CTL to Disable WatchDogReset 
  
==============================================================================*/

VOID
WatchDogDisable ( VOID )
{
  HWIO_APSS_WDT_TMR1_WDOG_CTL_OUT(HWIO_APSS_WDT_TMR1_WDOG_CTL_ENABLE_DISABLE_FVAL);
  ArmInstructionSynchronizationBarrier();
}

/*==============================================================================
  FUNCTION      WatchDogSetBiteTimer

  DESCRIPTION   Set the WDOG_BITE_TIME with the input BiteTime
  
  BITETIME      IN PARAM - time in seconds
  
  RETVAL        EFI_SUCCESS upon success
                EFI_INVALID_PARAMTER if BITETIME > max time 
  
  Note: If the watchdog Bite timer value is changed while wdog is enabled,
        the new Bite timer value will take effect immediately
==============================================================================*/

EFI_STATUS
WatchDogSetBiteTimer ( UINT32 BiteTimeMSec )
{
  if(BiteTimeMSec == 0 || BiteTimeMSec > MAX_BITE_TIME_IN_MSEC)
  {
    //Invalid case to pass 0 or > 32
    return EFI_INVALID_PARAMETER;
  }
  //Set the WDOG_BITE_TIME reg with the BiteTimeMSec 
  HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_OUT((BiteTimeMSec*SLEEP_CLKS_PER_SEC/SEC_TO_MSEC(1))-1);
  ArmInstructionSynchronizationBarrier();
  ResetWatchDogCounter();
  return EFI_SUCCESS;  
}

/*==============================================================================
  FUNCTION      ResetWatchDogCounter

  DESCRIPTION   Set the WDOG_RESET register to reset the count in 
                WDOG_STATUS register
==============================================================================*/

VOID
ResetWatchDogCounter ( VOID )
{
  //Set the WDOG_RESET reg 
  // WDOG_STATUS[20:1] will reset to 0 after this
  HWIO_APSS_WDT_TMR1_WDOG_RESET_OUT(HWIO_APSS_WDT_TMR1_WDOG_RESET_RESET_BMSK);
  ArmInstructionSynchronizationBarrier();
}

/*==============================================================================
  FUNCTION      DoWatchDogReset

  DESCRIPTION   Enable WatchDog and force immediate reset
==============================================================================*/

EFI_STATUS
DoWatchDogReset ( VOID )
{
  /*Set the Bitetime register with FORCED_BITE_TIME_MS */
  if(WatchDogSetBiteTimer(SEC_TO_MSEC(FORCED_BITE_TIME_MS)) == EFI_INVALID_PARAMETER)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  /* CTL, Enable */
  WatchDogEnable(); 
  return EFI_SUCCESS;
}