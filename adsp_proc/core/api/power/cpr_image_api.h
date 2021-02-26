/**
 * @file:  cpr_image_api.h
 * @brief:
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2017/12/06 00:59:17 $
 * $Header: //components/rel/core.qdsp6/2.1/api/power/cpr_image_api.h#1 $
 * $Change: 14992535 $
 */
#ifndef CPR_IMAGE_API_H
#define CPR_IMAGE_API_H

#include "cpr_defs.h"
#include "voltage_level.h"
#include "ClockDefs.h"
#include "DALSys.h"

/**
 * @brief Gets the ceiling voltage
 *
 * This function will return the ceiling voltage for
 * the given rail and mode.
 *
 * @param[in] rail  Rail to query
 * @param[in] info  Meta info on this rail
 *
 * @return The voltage in microvolts
 *
 * TODO: delete this API when VCS driver stops maintaining voltage range.
 */
uint32 cpr_get_ceiling_voltage(cpr_domain_id railId, cpr_domain_info* info);

/**
 * @brief Gets the floor voltage
 *
 * This function will return the floor voltage for
 * the given rail and mode.
 *
 * @param[in] rail  Rail to query
 * @param[in] info  Meta info on this rail
 *
 * @return The voltage in microvolts
 *
 * TODO: delete this API when VCS driver stops maintaining voltage range.
 */
uint32 cpr_get_floor_voltage(cpr_domain_id railId, cpr_domain_info* info);

/**
 * @brief Return the CPR eLDO voltage for the corner.
 *        This API is supported on MPSS only
 *
 * @param[in] rail  Rail to query
 * @param[in] info  Meta info on this rail
 *
 * @return The voltage in microvolts
 */
uint32 cpr_get_eldo_voltage(cpr_domain_id railId, cpr_domain_info* info);


/*==============================================================================
**
** OLD APIs (TODO: remove old APIs when VCS/MCPM drivers are ready)
**
**==============================================================================
**/

/**
 * <!-- CPR_GetSupportedCorners -->
 *
 * A wrapper function around cpr_get_supported_modes() to use vlvl enums.
 *
 */
DALResult CPR_GetSupportedCorners( cpr_domain_id eRail, rail_voltage_level **eCorners, uint32 *uCornerCount );

/**
 * <!-- CPR_Enable -->
 *
 */
DALResult CPR_Enable( cpr_domain_id eRail );

/**
 * <!-- CPR_Disable -->
 *
 *
 */
DALResult CPR_Disable( cpr_domain_id eRail );

/**
 * <!-- CPR_SetNextModeCorner -->
 *
 * This function indicates which corner is being entered.
 * Currently, when this is called CPR will be off.
 */
 DALResult CPR_SetNextModeCorner( cpr_domain_id eRail, rail_voltage_level eCorner );

/**
 * <!-- CPR_GetRailVoltageRecommendation -->
 *
 * Besides returning the current recommendation this function currently indicates which corner is being entered.
 * Currently, when this is called CPR will be off or will be turned off.
 */
 DALResult CPR_GetRailVoltageRecommendation( cpr_domain_id eRail, rail_voltage_level eCorner, uint32 *nVoltageuV );

/**
 * <!-- CprGetFloorAndCeiling -->
 *
 * @brief Get the floor and ceiling voltage for the specified mode.
 *
 * @param rail_id : CLOCK_VREG_VDD_MSS (or VCS_RAIL_MSS)
 * @param mode : enum for svs, svs+, nom, turbo etc.
 * @param floorVoltageUV : pointer to a uint32 to be filled in with a voltage in uV
 * @param ceilingVoltageUV : pointer to a uint32 to be filled in with a voltage in uV
 *
 * @return DAL_SUCCESS if there are no issues filling the provided pointers.
 */
DALResult CprGetFloorAndCeiling( cpr_domain_id rail_id, rail_voltage_level mode, uint32* floorVoltageUV, uint32* ceilingVoltageUV );

/**
 * <!-- CPR_GetEldoVoltageRecommendation -->
 *
 * @brief Return the CPR eLDO voltage for the corner.
 *
 * @param eRail : Which rail to provide information for.
 * @param eCorner : SVS, NOM, etc...
 * @param nVoltageUV : The recommended voltage is returned in this uint32*.
 *
 * @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid.
 */
DALResult CPR_GetEldoVoltageRecommendation(cpr_domain_id eRail, rail_voltage_level eCorner, uint32 *nVoltageUV);

/**
 * <!-- CPR_Init_Phase1 -->
 *
 * @brief  Called by rcinit via  currently.  Initializes CPR clocks, ISRs and other setup requirements.
 *
 * @param eRail : Which rail to initialize
 *
 * @return DAL_SUCCESS if the rail is valid.
 */
DALResult CPR_Init_Phase1(cpr_domain_id  eRail);   //Get the data structures and fuses read. (called by RC init currently)

/**
 * <!-- CPR_Init_Phase2 -->
 *
 * @brief  Indication to CPR that the system is in it's lowest power state and can do bootsafe voltage training.
 *
 * @param eRail : Which rail to initialize
 *
 * @return DAL_SUCCESS if the rail is valid.
 */
DALResult CPR_Init_Phase2(cpr_domain_id  eRail);

/**
 * <!-- CPR_GetSafeVoltage -->
 *
 * @brief Get the bootsafe measured voltage for the corner
 *
 * @param eRail : Which rail to provide information for.
 * @param eCorner : RAIL_VOLTAGE_LEVEL_LOW_SVS, RAIL_VOLTAGE_LEVEL_MIN_SVS
 * @param nVoltageUV : The recommended voltage is returned in this uint32*.
 *
 * @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid.
 */
DALResult  CPR_GetSafeVoltage(cpr_domain_id  eRail, rail_voltage_level eCorner, uint32 *nSafeVoltageuV);

/**
 * <!-- CPR_RelinquishedControl_Enter -->
 *
 * @brief CPR will not be operating for an extended period of time.
 *
 * All CPR closed loop recommendations will be reset to higher safe values and
 * enable/disable calls will do nothing until CPR_RelinquishedControl_Exit is called.
 *
 * @param eRail : Which rail to be switched.
 *
 * @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid.
 */
DALResult CPR_RelinquishedControl_Enter(cpr_domain_id  eRail);

/**
 * <!-- CPR_RelinquishedControl_Exit -->
 *
 * @brief CPR will be used again after an extended period of time where it was unused.
 *
 * All CPR closed loop recommendations will be reset to higher safe values and
 * enable/disable calls will do nothing until CPR_RelinquishedControl_Exit is called.
 *
 * @param eRail : Which rail to be switched.
 *
 * @return DAL_SUCCESS if the rail, corner and nVoltage pointer are all valid.
 */
DALResult CPR_RelinquishedControl_Exit(cpr_domain_id  eRail);

#define CPR_MASK_MSS_MODEM_SLEEP     0x1
/**
 * <!-- CPR_MaskSensors -->
 *
 * @brief Some special cases require a change in the CPR configuration. CPR_MASK_... masks
 *        are defined in this cpr.h file and can be applied when needed.
 *
 * @param mask : A CPR_MASK...mask as defined above.
 *
 * @return DAL_SUCCESS if the mask was understood and applied correctly.
 */
DALResult CPR_MaskSensors(uint32 mask);

/**
 * <!-- CPR_UnmaskSensors -->
 *
 * @brief Some special cases require a change in the CPR configuration. CPR_MASK_... masks
 *        are defined in this cpr.h file and can be applied when needed.
 *
 * @param mask : A CPR_MASK...mask as defined above.
 *
 * @return DAL_SUCCESS if the mask was understood and applied correctly.
 */
DALResult CPR_UnmaskSensors(uint32 mask);

#endif
