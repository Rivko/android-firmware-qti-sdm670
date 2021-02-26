#ifndef TRACER_CFGRPM_H
#define TRACER_CFGRPM_H
/** =========================================================================
  @file tracer_cfgrpm.h

  @brief QUALCOMM Debug Subsystem (QDSS) Tracer - Configure RPM processor


               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.qdsp6/2.1/debugtrace/tracer/src/tracer_cfgrpm.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "comdef.h"
#include "tracer_config.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/**
  @brief Wrapper to configure sw events on the RPM processor.
  @return TRACER_CMDRESP_SUCCESS: RPM config driver called.
  @return TRACER_CMDRESP_F_AVAIL: RPM config driver not available (not
          configurable from this processor).
 */
tracer_cmdresp_t tracer_cfgrpm_swe(uint32 setting, uint32 type,
                                   uint64 bitmask);

/*-------------------------------------------------------------------------*/
/**
  @brief Wrapper to configure the ETM on the RPM processor.
  @return TRACER_CMDRESP_SUCCESS: RPM config driver called.
  @return TRACER_CMDRESP_F_AVAIL: RPM config driver not available (not
          configurable from this processor).
 */
tracer_cmdresp_t tracer_cfgrpm_etm(uint8 state);

#endif /* #ifndef TRACER_CFGRPM_H */

