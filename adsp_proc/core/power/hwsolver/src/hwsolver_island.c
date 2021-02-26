/*============================================================================
@file hwsolver_island.c
 
Minimal code for the HWSolver in island mode.
 
Copyright (c) 2017 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

#include "hwsolver_internal.h"
#include "unpa_resource.h"

/*----------------------------------------------------------------------------
 * Externs
 * -------------------------------------------------------------------------*/

extern hwsolver_sleep_mode hwsolver_sleep_modes[];
extern unsigned int hwsolver_num_sleep_modes;

extern unsigned int uATS_GetSolverOutputModeLatency(void);

unpa_resource_state
hwsolver_mode_disable_driver_fcn( unpa_resource *resource, unpa_client *client,
                                  unpa_resource_state state );

/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/

hwsolver_struct hwsolver;

char hwsolver_micro_logbuf[MICRO_ULOG_MEM_NEEDED(1024)];
micro_ULogHandle hwsolver_micro_log;

unpa_resource_definition hwsolver_mode_disable_rsrc_defn = {
  HWSOLVER_MODE_DISABLE_NODE_NAME,
  unpa_orUpdateFcn,
  hwsolver_mode_disable_driver_fcn,
  0xFFFFFFFF,
  UNPA_UPDATE_CLIENT_TYPES,
  UNPA_RESOURCE_DEFAULT
};

unpa_resource hwsolver_mode_disable_rsrc; 

/*----------------------------------------------------------------------------
 * Routines
 * -------------------------------------------------------------------------*/

/**
 * <!-- _hwsolver_enable_mode -->
 */
void _hwsolver_enable_mode( unsigned int drv_id, unsigned int mode_idx )
{
  /* We don't shift by ENABLE_MASK_SHFT (0x1 - mode0 cannot be disabled)
     as our mode_idxs already map to the right bits */
  HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_OUTMI( drv_id,
    1 << mode_idx, 1 << mode_idx );
    
  MICRO_ULOG_RT_PRINTF_1( hwsolver_micro_log, "Enabled mode %d", mode_idx );
}

/** 
 * <!-- _hwsolver_disable_mode -->
 */
void _hwsolver_disable_mode( unsigned int drv_id, unsigned int mode_idx )
{
  /* We don't shift by ENABLE_MASK_SHFT (0x1 - mode0 cannot be disabled)
     as our mode_idxs already map to the right bits */
  HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_OUTMI( drv_id,
    1 << mode_idx, 0 );
    
  MICRO_ULOG_RT_PRINTF_1( hwsolver_micro_log, "Disabled mode %d", mode_idx );
}

/**
 * @brief hwsolver_mode_disable_driver_fcn
 */ 
unpa_resource_state
hwsolver_mode_disable_driver_fcn( unpa_resource *resource, unpa_client *client,
                                  unpa_resource_state state )
{
  int i;
  
  HWSLVR_LOCK();
  for ( i = 1; i < hwsolver_num_sleep_modes; ++i )
  {
    unsigned mode_bit = (1 << i);
    if ( mode_bit & state )
    {
      _hwsolver_disable_mode( 0, i );
      hwsolver.explicit_disabled[0] |= mode_bit;
    }
    else if ( mode_bit & hwsolver.explicit_disabled[0] )
    {
      if ( !(mode_bit & hwsolver.latency_disabled[0]) )
      {
        _hwsolver_enable_mode( 0, i );
      }
      hwsolver.explicit_disabled[0] &= ~mode_bit;
    }
  }
  HWSLVR_UNLOCK();
  
  return state;
}

/**
 * @brief hwsolver_island_lpr_enter
 */
void hwsolver_island_lpr_enter()
{
#ifdef HWSOLVER_USES_ISLAND
  unpa_query_result result;
  int latency_restriction = 0xFFFFFFFF;
  int drv_id;
  
#ifdef HWSOLVER_USES_UNPA
  /* Get current latency restriction from usleep */
  CORE_VERIFY( 
    UNPA_QUERY_SUCCESS == unpa_query( hwsolver.ulatency_qh, 
                                      UNPA_QUERY_ACTIVE_STATE, &result ) );
#else
  result.val = 0xFFFFFFFF;
#endif
                                    
  if ( result.val != 0xFFFFFFFF )
  {
    /* Obtain the MIN of latency restrictions across island and normal modes */
    unsigned int latency_val = MIN(result.val, hwsolver.latency_val);
    
    /* Latency restriction for HW Solver = (current latency restriction - 
       latency of the chosen software solved mode) */
    latency_restriction = 
      MAX(latency_val - uATS_GetSolverOutputModeLatency(), 1);
  }
  
  MICRO_ULOG_RT_PRINTF_3( hwsolver_micro_log, "hwsolver_island_lpr_enter "
                          "(normal_lr: %d) (island_lr: %d) (agg_lr: %d)",
                          hwsolver.latency_val, result.val, 
                          latency_restriction );
  
  for ( drv_id = 0; drv_id <= HWIO_SSC_RSCC_RSC_ID_DRVd_MAXd; ++drv_id )
  {
    int i;
    
    if ( latency_restriction == 0xFFFFFFFF )
    {
      /* Latency restrictions have been lifted; enable those modes disabled due
         to latency restrictions and not explicitly via disable_mode */
      unsigned int to_enable = 
        hwsolver.latency_disabled[drv_id] & ~hwsolver.explicit_disabled[drv_id];
        
      if ( to_enable )
      {
        for ( i = 1; i < hwsolver_num_sleep_modes; ++i )
        {
          if ( (1 << i) & to_enable )
          {
            _hwsolver_enable_mode( drv_id, i );
          }
        }
      }

      hwsolver.latency_disabled[drv_id] = 0;
    }
    else
    {
      for ( i = 1; i < hwsolver_num_sleep_modes; ++i )
      {
        if ( hwsolver_sleep_modes[i].latency > latency_restriction )
        {
          _hwsolver_disable_mode( drv_id, i );
          hwsolver.latency_disabled[drv_id] |= (1 << i);

        }
        else if ( (1 << i) & hwsolver.latency_disabled[drv_id] )
        {
          /* Enable a previously disabled mode, only if it hasn't (also)
             been disabled explicitly via disable_mode */
          if ( !( (1 << i) & hwsolver.explicit_disabled[drv_id] ) )
          {
            _hwsolver_enable_mode( drv_id, i );
          }
          hwsolver.latency_disabled[drv_id] &= ~(1 << i);
        }
      }
    }    
  }
  
  /* TODO: Do we need the below? */
  
  /* Configure SSC HW Frame 0 - the one used by the solver */
  /* Set a high value in TVAL; the solver will overwrite */
  HWIO_SSC_QTMR_V1_CNTP_TVAL_0_OUT( 0x7fffffff );
  HWIO_SSC_QTMR_V1_CNTP_CTL_0_OUTM( HWIO_SSC_QTMR_V1_CNTP_CTL_0_IMSK_BMSK, 0 );
  HWIO_SSC_QTMR_V1_CNTP_CTL_0_OUTM( HWIO_SSC_QTMR_V1_CNTP_CTL_0_EN_BMSK, 
    1 << HWIO_SSC_QTMR_V1_CNTP_CTL_0_EN_SHFT );
#endif
}

/**
 * @brief hwsolver_island_lpr_exit
 */
void hwsolver_island_lpr_exit()
{
#ifdef HWSOLVER_USES_ISLAND
  MICRO_ULOG_RT_PRINTF_2( hwsolver_micro_log, "hwsolver_island_lpr_exit "
                          "(last timeslot: %d) (last mode: %d)",
                          hwsolver_get_timeslot_chosen( 0 ),
                          hwsolver_get_mode_chosen( 0 ) );
#endif
}

/** 
 * <!-- hwsolver_get_timeslot_chosen -->
 *
 * @brief Returns the timeslot chosen by the solver in the last
 * solving cycle.
 */ 
unsigned int hwsolver_get_timeslot_chosen( unsigned int drv_id )
{
  return HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_INMI( drv_id,
           HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_TIMESLOT_CHOSEN_BMSK ) >>
           HWIO_SSC_RSCC_SOLVER_STATUS1_DRVd_TIMESLOT_CHOSEN_SHFT;
}

/** 
 * <!-- hwsolver_get_mode_chosen -->
 *
 * @brief Returns the mode chosen by the solver in the last
 * solving cycle.
 */ 
unsigned int hwsolver_get_mode_chosen( unsigned int drv_id )
{
  return HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_INMI( drv_id,
           HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_MODE_CHOSEN_BMSK ) >>
           HWIO_SSC_RSCC_SOLVER_STATUS2_DRVd_MODE_CHOSEN_SHFT;
}
