#ifndef _SYNTHREGISTRY_H_
#define _SYNTHREGISTRY_H_
/*==============================================================================
  FILE:         synthRegistry.h
  
  OVERVIEW:     This class provides regitry from where all the LPRs are being
                kept track of. It will enable us to access the (enabled) LPRs
                that are registered with sleep system.

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/synthesizer/synthRegistry.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "sleep_lpr.h"
#include "synthTypes.h"
#include "sleepi.h"

/*==============================================================================
                                TYPES & MACROS
 =============================================================================*/
/* Registry types that can be used to check the status of the particular entity */
typedef enum 
{
  REGISTRY_TYPE_LPR = 0,
  REGISTRY_TYPE_LPI
}registry_type;

/* Master sleep registry type that containes a list of the registered synthmodes */
typedef struct synthRegistry_s
{
  sleep_synth_lpr *LPR;
  sleep_synth_lpi *LPI;
}synthRegistry;

/*==============================================================================
                             FUNCTION DEFINITIONS
 =============================================================================*/
/**
 * synthRegistry_getRegistry
 *
 * @brief Default constructor of synthRegistry class which will create a 
 *        registry without any LPR registered with it.
 */
synthRegistry *synthRegistry_getRegistry(void);

/**
 * synthRegistry_init
 *
 * @brief Default constructor of synthRegistry class which will create a 
 *        registry without any LPR registered with it.
 */
void synthRegistry_init(void);

/**
 * readGlobalConfig
 *
 * @brief Reads the global config from the EFS, and fills in 
 *        enable/disable/config data for the LPR passed in.
 *  
 *        Note that this function is called outside of the sleep
 *        node lock, so it must be re-entrant.
 *
 * @param lpr: Presynthesized lpr we want to read the global 
 *             config for with sleep system.
 */
void synthRegistry_readGlobalConfig(sleep_lpr *sleepLPR);

/**
 * synthRegistry_registerSynthLPR
 *
 * @brief Registers a synth LPR with the registry
 *
 * @param lpr: Presynthesized lpr we want to register with sleep system.
 */
void synthRegistry_registerSynthLPR(sleep_synth_lpr *synthLPR);

/**
 * synthRegistry_registerSynthLPI
 *
 * @brief Registers a synth LPI with the registry
 *
 * @param lpr: Presynthesized lpi we want to register with sleep system.
 */
void synthRegistry_registerSynthLPI(sleep_synth_lpi *synthLPI);

/**
 * synthRegistry_LPRUpdated
 *
 * @brief Updates the list of enabled LPRs based on the status of the input
 *        LPR
 *  
 * @param lpr: sleep_synth_lpr with status change.
 */
void synthRegistry_LPRUpdated(sleep_synth_lpr *synthLPR);

/**
 * synthRegistry_getStatus
 *
 * @brief Gets the master sleep registry status for the given registry type
 *  
 * @param type: Registry type to check
 */
sleep_status synthRegistry_getStatus(registry_type type);

#endif /* _SYNTHREGISTRY_H_ */

