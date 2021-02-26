/*
#============================================================================
#  Name:
#    dog_hb_legacy.c 
#
#  Description:
#    Legacy Watchdog Heartbeat monitoring 
#
# Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
#============================================================================
*/

#include "stdarg.h"
#include "stdlib.h"
#include "DALSys.h"
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "dog_hb.h"
#include "rex_types.h"

/* Legacy : take it out as soon as clients stop using these old types */
dog_report_type dog_mm_rpt_2;
dog_report_type dog_ftm_rpt_2;
dog_report_type dog_hdrmc_rpt_2;
dog_report_type dog_plt_rpt_2;

const rex_timer_cnt_type   dog_plt_rpt_time_2;
const rex_timer_cnt_type   dog_ftm_rpt_time_2;
