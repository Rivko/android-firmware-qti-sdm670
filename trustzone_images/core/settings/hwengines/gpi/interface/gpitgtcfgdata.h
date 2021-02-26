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
01/24/18   ah      File modified for Hana.

===============================================================================
                   Copyright (c) 2017-2018 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "comdef.h"

#define NUM_GPII                     5

/** @brief GPI Configuration
    Provides default GPI configuration.
 */
typedef struct _tgt_gpi_config_type
{
  uint8   active;
  uint8   type;
  uint64  gsi_pa;
  uint32  irq;         /* Muxed IRQ */
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

