/**
 * HW event driver provides an interface to configure various registers
 * that select HW events from different sub-systems.
 */
/*===========================================================================
  FILE: hwevent_cntrl_stub.c

  OVERVIEW:     HW event not applicable

               Copyright (c) 2013 QUALCOMM Technologies, Inc.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/hwevent/dal/src/hwevent_cntrl_stub.c#1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-05-12 ps Initial revision.
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "qdss_control.h"
#include "DDIHWEvent.h"

int qdss_control_set_hwe_reg(unsigned long addr, unsigned long val)
{
   return QDSS_HWEVENT_UNKNOWN_ERR;
}

