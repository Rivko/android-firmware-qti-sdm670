#ifndef TIME_TIMETICK_H
#define TIME_TIMETICK_H
/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements functionality to save offsets into efs item file.

EXTERNALIZED FUNCTIONS
  time_efs_rw 
    Local function that provides IO access for generic offsets in EFS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

Copyright (c) 2009 - 2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/services/time_timetick.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/19/10   din     Prevented Multiple efs_writes for same file. 
03/11/10   din     Moved efs write to Time IPC task.
17/10/09   din     File created. 

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "DDITimetick.h"
#include "timetick.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

#define sclk_to_timestamp(x,y) /*Stub this call out for now */
#define sclk_to_timestamp_64(x,y) /*Stub this call out for now */

typedef DalTimetickTime64Type time_timetick_type;


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

void time_timetick_init(void);
time_timetick_type timetick_get_tmp64(void);
time_timetick_type timetick_get_safe_tmp64(void);
time_timetick_type timetick_get_min_rearm_sclk_tmp64(void);
time_timetick_type timetick_cvt_from_sclk_tmp64(time_timetick_type run_time, 
                                                 timer_unit_type tut);
time_timetick_type timetick_cvt_to_sclk_tmp64(time_timetick_type run_time_ms, 
                                               timer_unit_type tut);
time_timetick_type timetick_get_sclk_offset_tmp64(void);
time_timetick_type timetick_set_next_interrupt_tmp64(time_timetick_type match_count, 
                            time_timetick_type ticks_now);
void time_timetick_update_blast_signal_id(uint32 timer_blast_sig_registration_id);
void time_timetick_set_sclk_offset(timetick_type sclk_delta);
void time_timetick_init_sclk64(void);
void time_timetick_enable(void);
//uint32 mpm_get_timetick(void);
#endif /* TIME_TIMETICK_H */

