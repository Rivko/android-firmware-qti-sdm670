#ifndef _SYNTHTYPES_H_
#define _SYNTHTYPES_H_
/*==============================================================================
  FILE:         synthTypes.h
  
  OVERVIEW:     This file defines the structures for synth LPRs, & LPRMs

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/synthesizer/synthTypes.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "sleepi.h"
#include "sleep_lpr.h"
#include "SleepLPR_synth_modes.h"
#include "sleep_stats.h"

/*==============================================================================
                               COMMON TYPES
 =============================================================================*/
/* Enum that will used to map various properties of synth mode to its 
 * attribute bitmask. */
typedef enum
{
  /* A mask denoting that no attributes are set  */
  SYNTH_MODE_ATTR_LPR_MASK_NONE = 0,

  /* Indicates that this synth mode is actually a low power island resource */
  SYNTH_MODE_ATTR_LPI = 1 << 0,

  /* A mask denoting that all attributes are set  */
  SYNTH_MODE_ATTR_LPR_MASK_ANY = 0x7FFFFFFF
}synth_mode_attr;

/*==============================================================================
                                LPR TYPES
 =============================================================================*/
/* Helper structure for the presynthesized LPR data */
typedef struct synth_seed_latency_s
{
  /* Store enter latenecy as seperate value */
  uint32 enter[SLEEP_NUM_STATIC_FREQ];

  /* Combined enter & exit latency values at each scaled frequency */
  uint32 enter_exit[SLEEP_NUM_STATIC_FREQ];

  /* If the backoff adjustment feature is enabled, this will be a single backoff
   * value at slowest given frequency.
   * If feature is disabled, it will be the scaled values of the given static
   * frequency list
   */ 
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  uint32  backoff[1]; 
#else
  uint32  backoff[SLEEP_NUM_STATIC_FREQ];
#endif
}synth_seed_latency;

/* Synthesized mode subset (cacheable/uncacheable) */
typedef struct sleep_lprm_group_s
{
  /* The number of modes that are expected to be registered individually in 
   * order to complete this mode. */
  uint32 num_component_modes; 

  /* Array of pointers to the component modes in this synthesized mode. */
  sleep_lprm **component_modes;
} sleep_lprm_group;

/* Typedef's here due to circular dependency between the two strcutures */
typedef struct sleep_synth_lpr_s   sleep_synth_lpr;
typedef struct sleep_synth_lprm_s  sleep_synth_lprm;

/* Structure that stores information for a presynthesized mode */
struct sleep_synth_lprm_s
{
  /* The name of this synthesized low power mode.  This should use the standard
   * naming convention for "lpr.lprm", with a '+' in between
   * modes. For example: "cxo.shutdown + pxo.shutdown". */
  const char                  *name;

  /* Dynamic index number used to access synth LPRM array data in the dynamic statistics array */
  uint32                      dynamic_num;

  /* Cacheable component modes in this synthesized mode */
  sleep_lprm_group            cacheable;

  /* Uncacheable component modes in this synthesized mode */
  sleep_lprm_group            uncacheable;

  /* Initial latency values as generated via SleepSynth tool. These are the combined* 
   * LPRM latency values */
  synth_seed_latency          *seed_latency;

  /* Pointer to the parent synth LPR*/
  sleep_synth_lpr             *synth_LPR_parent;

  /* Bitmask containing various attributes for the synth mode like cacheability. */
  uint32                      attr;

  /* Current status of the mode */
  sleep_status                mode_status;

  /* Number of component modes that are enabled in this synthesized mode */
  uint32                      num_enabled_component_modes;

  /* Synth mode statistics */
  sleepStats_synthmode_stats  synth_stats;

  /* The wake-up tick that's currently being used to the enter functions. 
   * This duration could be updated by an enter function. */ 
  uint64                      wakeup_tick;
};

/* Structure that represents presynthesized LPRs */
struct sleep_synth_lpr_s
{
  /* The name associated with this resource, that is used to reference the
   * resource by other LPRs. This name can be used to indicate the LPR name
   * and the LPRM group name. */
  const char        *resource_name;

  /* The number of low power modes this resource has. */
  uint32            mode_count;

  /* The list of low power modes of this resource where preferred modes are
   * listed first. 
   */
  sleep_synth_lprm  *modes;

  /* Number of enabled modes in the array */
  uint32            enabled_mode_count;
};

/*==============================================================================
                                LPI TYPES
 =============================================================================*/
/* For now, define an LPI the same as an LPR */
typedef struct sleep_synth_lprm_s   sleep_synth_lpim;
typedef struct sleep_synth_lpr_s    sleep_synth_lpi;

#endif /* _SYNTHTYPES_H_ */

