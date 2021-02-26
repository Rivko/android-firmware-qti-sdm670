#ifndef _SLEEP_NPAI_H_
#define _SLEEP_NPAI_H_
/*==============================================================================
  FILE:         sleep_npa.h
  
  OVERVIEW:     This file provides the private externs and definitions fpr
                accessing the various sleep nodes and functions.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/npa_nodes/sleep_npai.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                             TYPES & DEFINITIONS
 =============================================================================*/
/* Used to create the node names for LPR clients to main sleep/lpr node
 * This macro will create a unique, standard name based on the base node_name */
#define SLEEP_LPR_CLIENT_NODE(baseName) "/node" baseName

/*==============================================================================
                             EXTERNAL FUNCTIONS
 =============================================================================*/
/**
 * sleepLatencyNode_define
 *
 * @brief Constructs npa resources for latency node
 */
void sleepLatencyNode_define(void);

/**
 * sleepMaxDurNode_define
 *
 * @brief Constructs npa resources for the max duration node
 */
void sleepMaxDurNode_define(void);

#endif /* _SLEEP_NPAI_H_ */

