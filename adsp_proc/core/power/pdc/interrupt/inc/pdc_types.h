#ifndef _PDC_TYPES_H_
#define _PDC_TYPES_H_
/*==============================================================================
  FILE:         pdcTypes.h
  
  OVERVIEW:     This file provides the Abstract Data Types PDC driver code

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/interrupt/inc/pdc_types.h#2 $
$DateTime: 2017/10/16 10:14:08 $
=============================================================================*/ 
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"

/*===========================================================================
 *                                MACROS
 *==========================================================================*/
/* Sets the trigger type */
#define PDC_TRIGGER_TYPE_SET(pol, edge)   (((pol & 0x01) << 2) | (edge & 0x03))

/*===========================================================================
 *                            TYPE DEFINITIONS
 *==========================================================================*/
/* PDC API status values */
typedef enum
{
  PDC_SUCCESS = 0,      /* Function success */
  PDC_FAIL    = 16,     /* General failure */

                            /* Specific failures */
  PDC_QDI_FAILURE,          /* Internal QDI layer failure */
  PDC_INVALID_SS_INT,       /* Invalid or not supported subsystem interrupt/GPIO */
  PDC_INVALID_CONFIG,       /* Invalid interrupt/GPIO configuration */
  PDC_NO_FREE_GPIO_MUX,     /* Maximum number of GPIO's are already enabled */
} pdc_status_type;

/* Predefined enumeration of possible trigger types for a PDC interrupt.
 *
 * Trigger type bit fields:
 *  Polarity  Edge  Output
 *  0         00  Level Sensitive Active Low
 *  0         01  Rising Edge Sensitive
 *  0         10  Falling Edge Sensitive
 *  0         11  Dual Edge Sensitive
 *  1         00  Level Sensitive Active High
 *  1         01  Falling Edge Sensitive
 *  1         10  Rising Edge Sensitive
 *  1         11  Dual Edge Sensitive */
typedef enum
{
  TRIGGER_LEVEL_LOW    = PDC_TRIGGER_TYPE_SET(0, 0x00),
  TRIGGER_RISING_EDGE  = PDC_TRIGGER_TYPE_SET(1, 0x02),
  TRIGGER_FALLING_EDGE = PDC_TRIGGER_TYPE_SET(0, 0x02),
  TRIGGER_DUAL_EDGE    = PDC_TRIGGER_TYPE_SET(1, 0x03),
  TRIGGER_LEVEL_HIGH   = PDC_TRIGGER_TYPE_SET(1, 0x00),

  TRIGGER_LAST
}pdc_trigger_type;

/* Enumeration of MPM mapping table pin types. */
typedef enum
{
  PDC_DRV_INVALID = -1,         /* Initial state of configuration */

  PDC_DRV0        = 0,          /* Valid DRV values to assign interrupt / GPIO's to */
  PDC_DRV1        = 1,

  PDC_DRV_DEFAULT = PDC_DRV0,   /* Default DRV to use when only one is supported, or don't care */
}pdc_drv_type;

/* Interrupt/GPIO PDC pin configuration */
typedef struct
{
  pdc_trigger_type  trig_type;  /* Trigger logic type */
  pdc_drv_type      drv_num;    /* DRV number (always 0 for now) */
}pdc_trigger_config;

#ifdef __cplusplus
}
#endif

#endif  /* _PDC_TYPES_H_ */

