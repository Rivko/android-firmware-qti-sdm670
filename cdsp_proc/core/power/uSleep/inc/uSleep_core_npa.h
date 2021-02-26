#ifndef USLEEP_CORE_NPA_H
#define USLEEP_CORE_NPA_H
/*==============================================================================
  FILE:         uSleep_core_npa.h
  
  OVERVIEW:     This file provides the CORE public definitions that
                are exported by the uSleep module for accessing the various 
                nodes and functions.

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/inc/uSleep_core_npa.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
                           USLEEP CORE DRIVER NODE
 =============================================================================*/
/* Name of internal core node used to control island entry.  Any core drivers that
 * have no specific use case should use this node to control island entry. */
#define USLEEP_CORE_DRIVER_NODE_NAME            "/island/core/drivers"

/* Valid required requests for the core driver node */
#define USLEEP_CORE_DRIVER_RESTRICT_ISLAND      0
#define USLEEP_CORE_DRIVER_ALLOW_ISLAND         1

#ifdef __cplusplus
}
#endif

#endif /* SLEEP_NPA_H */

