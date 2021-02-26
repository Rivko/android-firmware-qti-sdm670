#ifndef _TCS_RESOURCE_H_
#define _TCS_RESOURCE_H_
/*============================================================================
  FILE:         tcs_resource.h
  
  OVERVIEW:     This file provides the types for the PDC TCS driver
 
  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/pdc/tcs/src/config/670/aop/tcs_resource.h#1 $
$DateTime: 2018/02/16 04:48:18 $
============================================================================*/

/*===========================================================================
 *                            TYPES AND DEFINITIONS
 *===========================================================================*/
/* Number of commands per TCS; assumed to be the same for all */
#define NUM_COMMANDS_PER_TCS  4

/* Enum for the intended usage of each TCS */
typedef enum 
{
  TCS_NUM_SLEEP0        = 0,            /* TCS 0 - Sleep:  */
  TCS_NUM_SLEEP1,                       /* TCS 1 - Sleep:  */
  TCS_NUM_TOTAL_SLEEPS,                 /* Number of sleep TCS's */
  
  TCS_NUM_WAKE0 = TCS_NUM_TOTAL_SLEEPS, /* TCS 2 - Wake:  */
  TCS_NUM_WAKE1,                        /* TCS 3 - Wake:  */
  
  TCS_NUM_TOTAL                         /* Total TCS's: */
}tcs_usage;

/* General purpose index values which match the order in g_pdcResourceList */
typedef enum 
{
  RES_IDX_SMPA3,
  RES_IDX_SMPA4,
  RES_IDX_SMPA5,
  RES_IDX_BOBB1,
  RES_IDX_PBS,

  TCS_TOTAL_RESOURCE_NUM
}resource_index;

#endif /* _TCS_RESOURCE_H_ */

