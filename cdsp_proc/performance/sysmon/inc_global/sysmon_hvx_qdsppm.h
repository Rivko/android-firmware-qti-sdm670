/*-----------------------------------------------------------------------------
   Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------------*/

#ifndef SYSMON_HVX_QDSPPM_H_
#define SYSMON_HVX_QDSPPM_H_

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @enum sh_qdsppm_state_t
 * @brief HVX state feedback from QDSPPM 
 */
 typedef enum {
     SYSMON_HVX_STATE_ACTIVE = 1,
     /**< HVX is ACTIVE */
     
     SYSMON_HVX_STATE_DISABLE = 2,
     /**< HVX is going to be powered down */
 } sh_qdsppm_state_t;
 
/**
 *  @file sysmon_hvx_qdsppm.h
 *  @brief SYSMON HVX Throttling register store/restore APIs for qdsppm
 */

/**************************************************************************//**
 * @fn sysmon_qdsppm_hvx_state_change
 * @brief To be called by QDSPPM whenever there is a transition in HVX power 
 *        request in context of LPR. SysMon will restore HVX throttling 
 *        registers on HVX being turned ON i.e transitioned from OFF. 
 *        Should only be called in STM (LPR context).
 * @param state HVX power state from sh_qdsppm_state_t
 * @parblock
 *        SYSMON_HVX_STATE_ACTIVE when HVX power is turned ON (previously OFF)
 *        SYSMON_HVX_STATE_DISABLE when HVX power is going to be turned off 
 *           (previously ON)
 * @endparblock
 * @return none
 *****************************************************************************/
void sysmon_qdsppm_hvx_state_change(sh_qdsppm_state_t state);

/**************************************************************************//**
 * @fn sysmon_qdsppm_hvxthrottle_config
 * @brief QDSPPM to call this API when HVX is powered ON (transitioned
 *        from OFF state) under client's (Requestor for HVX power ON) context. 
 *        COPROC EGY registers are written with necessary values. 
 *        Not to be called in LPR context.
 *****************************************************************************/
void sysmon_qdsppm_hvxthrottle_config(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSMON_HVX_QDSPPM_H_ */
