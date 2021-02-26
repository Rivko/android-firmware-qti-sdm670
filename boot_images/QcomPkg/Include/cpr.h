/**
 * @file:  cpr.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2018/07/19 03:25:26 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/cpr.h#3 $
 * $Change: 16679875 $
 */
#ifndef CPR_H
#define CPR_H

#include "cpr_image_api.h" /* for image specific APIs */
#include "cpr_defs.h"

/**
 * @brief Initializes the CPR driver.
 */
void cpr_init(void);

/**
 * @brief Gets supported modes
 *
 * This function will return an immutable array of supported modes
 * in ascending order.
 *
 * @param[in]  rail        Rail to get modes for
 * @param[out] modes       Sorted array of supported modes
 * @param[out] modesCount  Number of modes supported
 */
void cpr_get_supported_modes(cpr_domain_id railId, const cpr_voltage_mode** modes, uint32* modesCount);

/**
 * @brief Gets the current optimal voltage recommendation
 *
 * This function will return the current optimal voltage recommendation for
 * the given rail and mode.
 *
 * @param[in] rail  Rail to query
 * @param[in] info  Meta info on this rail
 *
 * @return The voltage in microvolts
 */
uint32 cpr_get_voltage(cpr_domain_id railId, cpr_domain_info* info);

/**
 * @brief Gets the safe voltage recommendation
 *
 * This function will return a safe voltage recommendation for the given rail and mode.
 *
 * @param[in] rail  Rail to query
 * @param[in] info  Meta info on this rail
 *
 * @return The voltage in microvolts
 */
uint32 cpr_get_safe_voltage(cpr_domain_id railId, cpr_domain_info* info);

/**
 * @brief Notifies CPR of an imminent mode switch.
 *
 * This function will notify CPR a voltage change is about to occur.
 *
 * @param[in] railId  Rail to be switched
 */
void cpr_pre_state_switch(cpr_domain_id railId);

/**
 * @brief Notifies CPR a mode switch occurred.
 *
 * This function will notify CPR a voltage change occurred.
 *
 * @param[in] railId   Rail switched
 * @param[in] info     Meta info on this rail
 * @param[in] voltage  Current voltage (microvolts)
 */
void cpr_post_state_switch(cpr_domain_id railId, cpr_domain_info* info, uint32 voltage);

/**
 * @brief Relinquishes CPR voltage management control.
 *
 * @param[in] railId  CPR will be disabled on this rail
 *
 * @see cpr_resume_control
 */
void cpr_relinquish_control(cpr_domain_id railId);

/**
 * @brief Resumes CPR voltage management functionality
 *
 * Resumes CPR voltage management functionality on a rail that was
 * previously relinquished.
 *
 * @param[in] railId   CPR will be enabled on this rail
 * @param[in] info     Meta info on this rail
 * @param[in] voltage  Current voltage (microvolts)
 *
 * @see cpr_relinquish_control
 */
void cpr_resume_control(cpr_domain_id railId, cpr_domain_info* info, uint32 voltage);

 /**
 * @brief Set the CPR for sleep specific to the image.
 *
 */
void cpr_enter_sleep(void);

 /**
 * @brief Set the CPR for active specific to the image.
 *
 */
void cpr_exit_sleep(void);

extern void cpr_set_apc0_olv(uint32 corner);

typedef enum
{
	APC_NOM = 0,
	APC_TUR
}apc_corners;

 #endif

