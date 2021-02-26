/*
==============================================================================

FILE:         ClockTargetBSP.c

DESCRIPTION:
  This file contains the custom, target-specific clock driver BSP data.

==============================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/image/cdsp/hw/sdm845/ClockTargetBSP.c#1 $

==============================================================================
            Copyright (c) 2016 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================

*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockImage.h"


/*=========================================================================
      Data
==========================================================================*/

/*
 * Image-specific BSP data.
 */
ClockImageBSPType ClockImageBSP =
{
  .szCPUClockName = "turing_qdsp6ss_core_clk",
  .nInitCPUState  = 0,
};

