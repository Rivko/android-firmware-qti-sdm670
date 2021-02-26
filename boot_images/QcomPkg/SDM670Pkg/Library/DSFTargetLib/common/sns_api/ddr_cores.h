#ifndef __DDR_CORES_H__
#define __DDR_CORES_H__

/*=============================================================================

                                DDR CORES
                                Header File
GENERAL DESCRIPTION
 This file defines the DDR controller and DDR PHY core data structures


Copyright (c) 2014-2017, Qualcomm Technologies Incorporated. All rights reserved.
=============================================================================*/

/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/05/16   rp      Added version structs for LLCC and SHRM
12/01/13   dp      Initial version.
==============================================================================*/
/*==========================================================================
                               INCLUDE FILES
===========================================================================*/
#include "HALcomdef.h"

/*==============================================================================
                                  TYPES
==============================================================================*/

/**********************/
/*** DDR Controller ***/
/**********************/

struct ddr_mc
{
  struct
  {
   uint32 arch;
	 uint32 major;
	 uint32 minor;
  } version;
};

/**********************/
/*** DDR PHY  ***/
/**********************/

struct ddr_phy
{
	struct
	{
	  uint32 major;
	  uint32 minor;
	} version;
};

/**********************/
/*** SC LLCC  ***/
/**********************/

struct sc_llcc
{
	struct
	{
	  uint32 major;
	  uint32 minor;
	} version;
};

/**********************/
/*** DDR SHRM  ***/
/**********************/

struct ddr_shrm
{
	struct
	{
	  uint32 major;
	  uint32 minor;
	} version;
};


#endif /* __DDR_CORES_H__ */
