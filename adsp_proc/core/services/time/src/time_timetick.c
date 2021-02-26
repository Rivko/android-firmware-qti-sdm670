/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements functionality to save offsets into efs item file.

EXTERNALIZED FUNCTIONS
  time_efs_rw 
    Local function that provides IO access for generic offsets in EFS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

Copyright (c) 2009 - 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/services/time/src/time_timetick.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/23/10   din     Changed to handle allocation failure in malloc.
04/27/10   din     Fixed compiler warnings.
04/19/10   din     Prevented Multiple efs_writes for same file. Reinsertion of 
                   same element was prevented.
03/11/10   din     Moved efs write to Time IPC task.
03/10/10   din     Removed efs_unlink() as efs_put() overwites the item file.
17/10/09   din     File created. 

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "DDITimetick.h"
#include "DALSys.h"
#include "time_timetick.h"
#include "timetick.h"
#include "assert.h"
#include "qurt_cycles.h"
#include "timer_v.h"


/*=============================================================================

                         DATA DEFINITIONS
=============================================================================*/

static DalDeviceHandle * hTimetick = NULL;  

/*=============================================================================

                         FUNCTION DEFINITIONS
=============================================================================*/

void time_timetick_init(void)
{
  DALSYS_InitMod(NULL);
  DalTimetick_Attach("SystemTimer", &hTimetick);
  ASSERT(hTimetick != NULL);
}

time_timetick_type timetick_get_tmp64(void)
{
  DalTimetickTime64Type ttRet = -1;
  ASSERT(hTimetick != NULL);

  DalTimetick_GetTimetick64(hTimetick, &ttRet);

  ASSERT(ttRet != -1 && ttRet != 0); /*Temp To check it's working*/
  return (time_timetick_type)ttRet;  
}

time_timetick_type timetick_get_safe_tmp64(void)
{
  DalTimetickTime64Type ttRet = -1;
  ASSERT(hTimetick != NULL);
  DalTimetick_GetTimetick64(hTimetick, &ttRet);
  ASSERT(ttRet != -1);
  return (time_timetick_type)ttRet;
}

time_timetick_type timetick_set_next_interrupt_tmp64(time_timetick_type match_count, 
                            time_timetick_type ticks_now)
{
  DalTimetickTime64Type ttRet = 0;
  //ASSERT(hTimetick != NULL);
  DalTimetick_SetNextInterrupt64(hTimetick, (DalTimetickTime64Type)match_count, (DalTimetickTime64Type)ticks_now, &ttRet);
  ASSERT(ttRet != 0);
  return (time_timetick_type)ttRet;
}

time_timetick_type timetick_get_min_rearm_sclk_tmp64(void)
{
  DalTimetickTime32Type ttRet = -1;
  //ASSERT(hTimetick != NULL);
  DalTimetick_GetMinRearmSclk(hTimetick,&ttRet);
  return (time_timetick_type)ttRet;
}

time_timetick_type timetick_cvt_from_sclk_tmp64(time_timetick_type run_time, 
                                                 timer_unit_type tut)
{
  DalTimetickTime64Type ttRet = -1;
  //ASSERT(hTimetick != NULL);
  DalTimetick_CvtFromTimetick64(hTimetick, (DalTimetickTime64Type)run_time, (DalTimetickUnitType)tut, &ttRet);
  ASSERT(ttRet != -1);
  return (time_timetick_type)ttRet;
}

time_timetick_type timetick_cvt_to_sclk_tmp64(time_timetick_type run_time_ms, 
                                               timer_unit_type tut)
{
  DalTimetickTime64Type ttRet = -1;
  //ASSERT(hTimetick != NULL);
  DalTimetick_CvtToTimetick64(hTimetick, (DalTimetickTime64Type)run_time_ms, (DalTimetickUnitType)tut, &ttRet);
  ASSERT(ttRet != -1);
  return (time_timetick_type)ttRet;
}

time_timetick_type timetick_get_sclk_offset_tmp64(void)
{
  DalTimetickTime32Type ttRet = -1;
  //ASSERT(hTimetick != NULL);
  DalTimetick_GetOffset(hTimetick,&ttRet);
  //ASSERT(ttRet != -1);
  return (time_timetick_type)ttRet;  
}

void time_timetick_update_blast_signal_id(uint32 timer_blast_sig_registration_id)
{
  ASSERT(hTimetick != NULL);
  DalTimetick_UpdateBlastSigId(hTimetick,timer_blast_sig_registration_id);
}

void time_timetick_enable(void)
{
  ASSERT(hTimetick != NULL);
  DalTimetick_Enable(hTimetick, 1);
}

void time_timetick_set_sclk_offset(timetick_type sclk_delta)
{
  //ASSERT(hTimetick != NULL);
  DalTimetick_SetOffset(hTimetick,(DalTimetickTime32Type)sclk_delta);
}

void time_timetick_init_sclk64(void)
{
  ASSERT(hTimetick != NULL);
  DalTimetick_InitTimetick64(hTimetick);
}

/*uint32 mpm_get_timetick(void)
{
  return timetick_get();
}*/
