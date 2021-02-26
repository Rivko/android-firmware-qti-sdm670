#ifndef __HWIOQDSS_LPA_REPLICATOR_REGS_H__
#define __HWIOQDSS_LPA_REPLICATOR_REGS_H__
/*
===========================================================================
*/
/**
  @file qdss_lpa_replicator_regs.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:

  */
/*
  ===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1/debugtrace/lpa_replicator/hal/src/hwioqdss_lpa_replicator_regs.h#1 $
  $DateTime: 2017/09/14 01:25:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "HALhwio.h"

/*----------------------------------------------------------------------------
 * MODULE: QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP
 *--------------------------------------------------------------------------*/

extern volatile uint32  QDSS_SWAO_REPLIC_CXATBREPLICATOR_64WP_REG_BASE;

#define HWIO_QDSS_SWAO_REPLIC_IDFILTER1_ADDR          (QDSS_SWAO_REPLIC_CXATBREPLICATOR_64WP_REG_BASE + 0x00000004)

#define HWIO_QDSS_SWAO_REPLIC_IDFILTER1_OUT(v)        out_dword(HWIO_QDSS_SWAO_REPLIC_IDFILTER1_ADDR, v)


#endif /* __QDSS_LPA_REPLICATOR_REGS_H__ */
