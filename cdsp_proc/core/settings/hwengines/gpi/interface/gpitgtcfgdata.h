#ifndef __GPITGTCFGDATA_H__
#define __GPITGTCFGDATA_H__

/**
 * @file gpitgtcfgdata.h     
 * This file contains configuration data for the GPI driver for the Napali SDM845.
 *
 */
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/22/16   AH      Created

===============================================================================
                   Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "comdef.h"

#define MAX_NUM_QUP_BLOCKS           3
#ifdef GPI_SSC
#define NUM_GPII                     6
#elif GPI_SSC_ADSP_COMBINED
#define NUM_GPII                     6
#else
#define NUM_GPII                     8
#endif

/** @brief GPI Configuration
    Provides default GPI configuration.
 */
typedef struct _tgt_gpi_config_type
{
  uint8   active;
  uint8   type;
  uint64  gsi_pa;
  uint32  irq;         /* Muxed IRQ */
  boolean irq_muxed;   /* If IRQ are Muxed or not */
}tgt_gpi_config_type;

/** @brief GPII Configuration
    Provides default GPII configuration.
 */
typedef struct _tgt_gpii_config_type
{
  uint8   qup_type;    /* GSI HW instance this GPII is part of   */
  uint8   gpii_id;     /* GPI Interface ID                       */
  uint8   protocol;    /* Protocol running on this GPII          */
  uint8   options;     /* Various GPII options in bitmask format */
  uint8   ee;          /* Local execution environment index      */
  uint8   se;          /* Serial Engine index                    */
  uint32  irq;         /* IRQ for this ee                        */
}tgt_gpii_config_type;

#endif  /* __GPITGTCFGDATA_H__ */

