/*! \file */

/*
===========================================================================

FILE:        mdsshwio.h

DESCRIPTION:  
  Top level MDSS HW Access Definition file.  It includes:
  [1] MDSS Register file
  [2] HW Access Macros
  [3] Additionally Defined Fields for HW Access

===========================================================================

                             Edit History
$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/HALMDSSLib/mdsshwio.h#1 $
$DateTime: 2017/09/18 09:26:13 $


===========================================================================
  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/
#ifndef MDSSHWIO_H
#define MDSSHWIO_H



/* -----------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------- */

// Basic Register Access Macros
#include "mdsshalhwio.h"

// Database Header file of all MDSS Registers: MDP, DSI, HDMI and eDP
// Static MDP Base Defined Here

extern uintPtr MDSS_BASE_PTR;


#define        MDSS_REG_BASE                              (MDSS_BASE_PTR)
#define        MDSS_BASE                                  (MDSS_REG_BASE)


#include "mdssreg.h"

#endif // MDSSHWIO_H
