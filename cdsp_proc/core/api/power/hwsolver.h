/*============================================================================
@file hwsolver.h

APIs to register modes with the HW Solver, enable/disable modes at the solver
and override solver output

Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
#ifndef HWSOLVER_H
#define HWSOLVER_H

#include "DALStdDef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * Defines/Enums
 * -------------------------------------------------------------------------*/

#define HWSOLVER_SLEEP_MODE_NAME_MAXLEN 32

/* Generally defined for API use; not all modes exist on every target.
 * See hwsolver_sleep_modes and hwsolver_num_sleep_modes in per-target config
 * for actual modes and count.
 */
#define HWSOLVER_SLEEP_MODE1 0x2
#define HWSOLVER_SLEEP_MODE2 0x4
#define HWSOLVER_SLEEP_MODE3 0x8
#define HWSOLVER_SLEEP_MODE4 0x10

#define HWSOLVER_PRESENT_NODE_NAME "/hwsolver/present"
#define HWSOLVER_MODE_DISABLE_NODE_NAME "/hwsolver/mode_disable"

/*----------------------------------------------------------------------------
 * Types
 * -------------------------------------------------------------------------*/

/* Defines a HW solved sleep mode */ 
typedef struct hwsolver_sleep_mode
{
  const char *name;
  
  /* RSC sequence start address offset */
  unsigned int rsc_seq_addr;
  
  /* PDC sequence start address offset */
  unsigned int pdc_seq_addr;
  
  unsigned int pdc_seq_valid;
  unsigned int pdc_timer_valid;
  
  /* RSC latency in bringing up resources/executing the RSC sequence */
  unsigned int rsc_backoff;
  
  /* PDC latency in bringing up resources/executing the PDC sequence */
  unsigned int pdc_backoff;
  
  /* Exit latency of this sleep mode in ticks; used by SW in determining which
   * HW solved modes cannot be entered (and so should be disabled) due to the
   * current imposed interrupt latency restriction.
   */
  unsigned int latency;
} hwsolver_sleep_mode;

/* Defines a timeslot:modes priority table. "modes" should be a priority
 * ordered array of indices of sleep modes in hwsolver_sleep_modes that the
 * solver may pick up if the minimum idle duration is "timeslot"
 */
typedef struct hwsolver_timeslot_table
{
  /* Minimum idle duration for the solver to pick a mode from "modes" */
  unsigned int  timeslot;
  
  /* Array of modes in priority order; if the available sleep duration is
   * > "timeslot", the solver will return the first enabled mode from
   * this array as output.
   */
  unsigned char modes[8];
  
  /* Number of valid modes in "modes" above */
  unsigned int  num_modes;
} hwsolver_timeslot_table;

/** 
 * <!-- hwsolver_get_mode_idx -->
 *
 * @brief Returns the index in "hwsolver_sleep_modes" of the mode
 * with name "mode_name".
 *
 * @param mode_name: Name of the mode to search for
 * @return Index of the mode, if found or -1
 */
int hwsolver_get_mode_idx( const char *mode_name );

/** 
 * <!-- hwsolver_toggle_modes_by_latency -->
 *
 * @brief Disable modes that do not satisfy the given latency restriction
 * and (re-)enable modes previously disabled due to the previous latency
 * restriction.
 *
 * @param drv_id: Id of the DRV making the request
 * @param latency_restriction: The new latency restriction in ticks.
 */
void hwsolver_toggle_modes_by_latency( unsigned int drv_id, 
                                       unsigned int latency_restriction );
                                        
/** 
 * <!-- hwsolver_init -->
 * @brief Initialize the solver; program modes, timeslot:modes priority
 * tables and related registers.
 * Expects extern variables hwsolver_sleep_modes, hwsolver_num_sleep_modes,
 * hwsolver_timeslot_tables and hwsolver_num_timeslot_tables.
 */
void hwsolver_init( void );

/** 
 * <!-- hwsolver_set_soft_wakeup_time -->
 *
 * @brief Additional input to the solver in choosing a low power mode;
 * Programs a "soft" absolute wakeup time. Note that this wakeup time is not
 * used in backoff math or computing an actual wakeup time - QTimers must
 * be used for that. This value is only used in choosing an LPM.
 *
 * @param drv_id: Id of the DRV making the request
 * @param soft_wakeup: Absolute wakeup time as input to the solver
 */
void hwsolver_set_soft_wakeup_time( unsigned int drv_id, 
                                    uint64 soft_wakeup );

/** 
 * <!-- hwsolver_set_max_duration -->
 *
 * @brief Additional input to the solver in choosing a low power mode;
 * Programs a "soft" max sleep duration. Note that this duration is not
 * used in backoff math or computing an actual wakeup time - QTimers must
 * be used for that. This value is only used in choosing an LPM.
 *
 * @param drv_id: Id of the DRV making the request
 * @param max_duration: Max sleep duration as input to the solver
 */                                        
void hwsolver_set_max_duration( unsigned int drv_id,
                                unsigned int max_duration );
                                    
/** 
 * <!-- hwsolver_set_timers_considered -->
 *
 * @brief Program a bitset with each bit representing a QTimer frame
 * into the TIMERS_CONSIDERED register per DRV. If a bit is set, the
 * HW solver will consider that QTimer match value for sleep
 * duration calculation.
 *
 * @param drv_id: Id of the DRV making the request
 * @param timers_considered: Bitset of QTimer frames, with a set bit
 * enabling that particular QTimer match value input.
 */
void hwsolver_set_timers_considered( unsigned int drv_id,
                                     unsigned int timers_considered );                                    
                                    
/** 
 * <!-- hwsolver_override_once -->
 *
 * @brief Set up a one-time override of the solver output. If mode_idx is
 * >= 0, the solver will output mode_idx as the chosen mode in the next
 * solving cycle. If mode_idx is < 0, "idle_time" is used as sleep
 * duration to choose an appropriate timeslot and thus mode, in the next
 * solving cycle.
 *
 * @param mode_idx: Mode that will be output by the solver if >= 0
 * @param idle_time: Value used as sleep duration by the solver if
 * mode_idx is < 0.
 */                                        
void hwsolver_override_once( int mode_idx, unsigned int idle_time );

/** 
 * <!-- hwsolver_override_once -->
 *
 * @brief Continuously override solver output until hwsolver_cancel_override
 * is invoked. If mode_idx is >= 0, the solver will output mode_idx as the
 * chosen mode in all solving cycles until cancelled. If mode_idx is < 0,
 * "idle_time" is used as sleep duration to choose an appropriate timeslot
 * and thus mode, in all solving cycles until cancelled.
 *
 * @param mode_idx: Mode that will be output by the solver if >= 0
 * @param idle_time: Value used as sleep duration by the solver if
 * mode_idx is < 0.
 */ 
void hwsolver_override_continuous( int mode_idx, unsigned int idle_time );

/** 
 * <!-- hwsolver_cancel_override -->
 *
 * @brief Cancel a continuous override of solver output.
 */ 
void hwsolver_cancel_override( void );

/** 
 * <!-- hwsolver_get_timeslot_chosen -->
 *
 * @brief Returns the timeslot chosen by the solver in the last
 * solving cycle.
 */ 
unsigned int hwsolver_get_timeslot_chosen( unsigned int drv_id );

/** 
 * <!-- hwsolver_get_timeslot_chosen -->
 *
 * @brief Returns the mode chosen by the solver in the last
 * solving cycle.
 */ 
unsigned int hwsolver_get_mode_chosen( unsigned int drv_id );

/** 
 * <!-- hwsolver_get_timeslot_chosen -->
 *
 * @brief Returns 1 if the solver is in continuous override mode.
 */ 
unsigned int hwsolver_is_solver_in_override( unsigned int drv_id );

#ifdef __cplusplus
}
#endif

#endif // HWSOLVER_H
