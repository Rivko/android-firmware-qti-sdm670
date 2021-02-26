#ifndef __DDIAXICFG_H__
#define __DDIAXICFG_H__
/**
 * @file ddiaxicfg.h
 * @note External header file. API 2.0 for Bus Configuration. Not compatible with API 1.0
 * 
 *                REVISION  HISTORY
 *  
 * This section contains comments describing changes made to this file. Notice
 * that changes are listed in reverse chronological order.
 * 
 * $Header: //components/rel/core.qdsp6/2.1/api/systemdrivers/icb/ddiaxicfg.h#1 $ 
 * $DateTime: 2017/07/21 22:10:47 $ 
 * $Author: pwbldsvc $ 
 * 
 * when         who     what, where, why
 * ----------   ---     ---------------------------------------------------
 * 09/13/2016   sds     Update QoS override API for Hoya (branched)
 * 02/26/2015   sds     Add QoS override API
 * 06/18/2014   sds     Add lockless halt/unhalt APIs to be used during error handling.
 * 02/16/2014   sds     Add port halt/unhalt APIs.
 * 11/28/2011   dj      Branched from rev 1.0 and updated to rev 2.0 
 *  
 *  
 * Copyright (c) 2011-2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
 */ 
#include "icbid.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** AxiCfg return types. */
typedef enum
{
  AXICFG_SUCCESS           = 0,
  AXICFG_ERROR             = 1, /**< Any error not described below */
  AXICFG_ERROR_UNSUPPORTED = 2, /**< Operation unsupported on this target */
  AXICFG_ERROR_INVALID     = 3, /**< Invalid parameter */
  AXICFG_ERROR_TIMEOUT     = 4, /**< Timeout encountered during operation. */
  AXICFG_RETURNCODE_SIZE   = 0x7FFFFFFF,
} AxiCfg_ReturnCodeType;

/** Clock gating type.
 *  Note: Not all fields apply to all ports.
 */
typedef struct
{
  bool bCoreClockGateEn; /**< Gating of the registers on the clock of the core behind the port. */
  bool bArbClockGateEn;  /**< Gating of the arbiter on the port. */ 
  bool bPortClockGateEn; /**< Gating of the registers on the bus core clock. */
} AxiCfg_ClockGatingType;

typedef struct
{
  uint32_t bandwidth;
  uint32_t saturation;
} Axicfg_LimiterType;

typedef struct
{
  uint32_t low_priority;
  uint32_t high_priority;
  uint32_t bandwidth;
  uint32_t saturation;
} Axicfg_RegulatorType;

typedef struct
{
  bool read;  /**< Consider read traffic */
  bool write; /**< Consider write traffic */
} Axicfg_RegulatorModeType;

typedef struct
{
  uint32_t                 defaultPriority;
  Axicfg_LimiterType       limiter;
  bool                     limiterEnabled;
  Axicfg_RegulatorType     regulator;
  Axicfg_RegulatorModeType regulatorMode;
  bool                     urgencyForwardingEnabled;
} AxiCfg_NocQosPolicyType;

/* QOS policy definition */
typedef struct
{
  AxiCfg_NocQosPolicyType noc;
} AxiCfg_QosPolicyType;

/**
  @brief Configure the clock gating for the indicated master.

  @param[in] eMaster    The master port identifier
  @param[in] pGating    A pointer to the clock gating configuration
 */
void AxiCfg_SetMasterClockGating( ICBId_MasterType eMaster, AxiCfg_ClockGatingType * pGating);

/**
  @brief Configure the clock gating for the indicated slave.

  @param[in] eSlave     The slave way identifier
  @param[in] pGating    A pointer to the clock gating configuration
 */
void AxiCfg_SetSlaveClockGating( ICBId_SlaveType eSlave, AxiCfg_ClockGatingType * pGating);

/**
  @brief Attempt to halt the specified master port.

  @param[in] eMaster  The master port identifier
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfg_HaltMasterPort( ICBId_MasterType eMaster );

/**
  @brief Attempt to halt the specified master port.
         MUST ONLY BE USED IN SINGLE THREADED ERROR CONTEXT.

  @param[in] eMaster  The master port identifier
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfg_HaltMasterPort_OnError( ICBId_MasterType eMaster );

/**
  @brief Attempt to unhalt the specified master port.

  @param[in] eMaster  The master port identifier
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfg_UnhaltMasterPort( ICBId_MasterType eMaster );

/**
  @brief Attempt to unhalt the specified master port.
         MUST ONLY BE USED IN SINGLE THREADED ERROR CONTEXT.

  @param[in] eMaster  The master port identifier
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfg_UnhaltMasterPort_OnError( ICBId_MasterType eMaster );

/**
  @brief Set a QoS policy for the given master, overriding default settings.

  @param[in] eMaster     The master port identifier
  @param[in] pQosPolicy  A pointer to the qos policy to set (NULL to unset)
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfg_SetQosPolicy( ICBId_MasterType eMaster, AxiCfg_QosPolicyType *pQosPolicy );

#ifdef __cplusplus
}
#endif

#endif /* __DDIAXICFG_H__ */
