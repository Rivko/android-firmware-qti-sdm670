/*============================================================================
@file hwsolver.c

Implements APIs to register modes with the HW Solver, enable/disable modes
at the solver and override solver output

Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

#include "hwsolver_internal.h"
#include "ssc_qtmr.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * Externs and Globals
 * -------------------------------------------------------------------------*/

/* Array of HW solved sleep modes */
extern hwsolver_sleep_mode hwsolver_sleep_modes[];

/* Number of modes in the above array */
extern unsigned int hwsolver_num_sleep_modes;

/* Array of defined timeslot:modes priority tables; "timeslot" fields must be
   in ascending order in the array and timeslot_tables[0].timeslot must be 0 */
extern hwsolver_timeslot_table hwsolver_timeslot_tables[];
   
/* Number of timeslot:modes priority tables */
extern unsigned int hwsolver_num_timeslot_tables;

/* Global state */
extern hwsolver_struct hwsolver;

#ifdef HWSOLVER_USES_ISLAND
/* Log handle and buffer for use in Island mode */
extern micro_ULogHandle hwsolver_micro_log;
extern char hwsolver_micro_logbuf[];

unsigned int hwsolver_bkup_latency_disabled[HWSOLVER_NUM_DRVS];
unsigned int hwsolver_bkup_explicit_disabled[HWSOLVER_NUM_DRVS];

extern unpa_resource_definition hwsolver_mode_disable_rsrc_defn;
extern unpa_resource hwsolver_mode_disable_rsrc; 

extern void _hwsolver_enable_mode( unsigned int drv_id, unsigned int mode_idx );
extern void _hwsolver_disable_mode( unsigned int drv_id, unsigned int mode_idx );

#else

hwsolver_struct hwsolver;

#endif


/*----------------------------------------------------------------------------
 * Routines
 * -------------------------------------------------------------------------*/
 
/** 
 * <!-- hwsolver_get_mode_idx -->
 *
 * @brief Returns the index in "hwsolver_sleep_modes" of the mode
 * with name "mode_name".
 *
 * @param mode_name: Name of the mode to search for
 * @return Index of the mode, if found or -1
 */ 
int hwsolver_get_mode_idx( const char *mode_name )
{
  int i;
  for ( i = 0; i < hwsolver_num_sleep_modes; ++i )
  {
    if ( 0 == strncmp( mode_name, hwsolver_sleep_modes[i].name,
                       HWSOLVER_SLEEP_MODE_NAME_MAXLEN+1 ) )
    {
      return i;
    }
  }
  
  return -1;
}

#ifndef HWSOLVER_USES_ISLAND
/** 
 * <!-- _hwsolver_enable_mode -->
 *
 * @brief Helper routine to program the ENABLED_MODES register
 */
void _hwsolver_enable_mode( unsigned int drv_id, unsigned int mode_idx )
{
  /* We don't shift by ENABLE_MASK_SHFT (0x1 - mode0 cannot be disabled)
     as our mode_idxs already map to the right bits */
  HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_OUTMI( drv_id,
    1 << mode_idx, 1 << mode_idx );
    
  ULOG_RT_PRINTF_1( hwsolver.log, "Enabled mode %d", mode_idx );
}

/** 
 * <!-- _hwsolver_disable_mode -->
 *
 * @brief Helper routine to program the ENABLED_MODES register
 */
void _hwsolver_disable_mode( unsigned int drv_id, unsigned int mode_idx )
{
  /* We don't shift by ENABLE_MASK_SHFT (0x1 - mode0 cannot be disabled)
     as our mode_idxs already map to the right bits */
  HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_OUTMI( drv_id,
    1 << mode_idx, 0 );
    
  ULOG_RT_PRINTF_1( hwsolver.log, "Disabled mode %d", mode_idx );
}
#endif

/** 
 * <!-- hwsolver_disable_modes -->
 *
 * @brief Disable one or more HW-solved sleep modes, each bit in "mode_mask"
 * represents a mode. Mode0 cannot be disabled (bit0 is ignored).
 */
static void hwsolver_disable_modes( unsigned int drv_id, 
                                    unsigned int mode_mask )
{
  int i;
  
  HWSLVR_LOCK();
  
  for ( i = 1; i < hwsolver_num_sleep_modes; ++i )
  {
    if ( (1 << i) & mode_mask )
    {
    _hwsolver_disable_mode( drv_id, i );
    hwsolver.explicit_disabled[drv_id] |= (1 << i);
  }
  }
  
  HWSLVR_UNLOCK();
}

/** 
 * <!-- hwsolver_enable_modes -->
 *
 * @brief Enable one or more HW-solved sleep modes, each bit in "mode_mask"
 * represents a mode. Mode0 is always enabled (bit0 is ignored).
 * If a mode is implicitly disabled due to the current latency restriction,
 * it will not be enabled, until that restriction is lifted.
 */
static void hwsolver_enable_modes( unsigned int drv_id, 
                                   unsigned int mode_mask )
{
  int i;
  
  HWSLVR_LOCK();
  
  for ( i = 1; i < hwsolver_num_sleep_modes; ++i )
  {
    unsigned mode_bit = (1 << i);
    if ( mode_bit & mode_mask )
    {
      if ( !(mode_bit & hwsolver.latency_disabled[drv_id]) )
    {
      _hwsolver_enable_mode( drv_id, i );
    }
      hwsolver.explicit_disabled[drv_id] &= ~mode_bit;
    }
  }
  
  HWSLVR_UNLOCK();
}

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
                                       unsigned int latency_restriction )
{
  int i;
  
  HWSLVR_LOCK();
  
  if ( latency_restriction == 0xFFFFFFFF )
  {
    /* Latency restrictions have been lifted; enable those modes disabled due
       to latency restrictions and not explicitly via hwsolver_disable_mode */
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
      unsigned mode_bit = (1 << i);
      if ( hwsolver_sleep_modes[i].latency > latency_restriction )
      {
        _hwsolver_disable_mode( drv_id, i );
        hwsolver.latency_disabled[drv_id] |= mode_bit;
      }
      else if ( mode_bit & hwsolver.latency_disabled[drv_id] )
      {
        /* Enable a previously disabled mode, only if it hasn't (also)
           been explicitly disabled via hwsolver_disable_mode */
        if ( !( mode_bit & hwsolver.explicit_disabled[drv_id] ) )
        {
          _hwsolver_enable_mode( drv_id, i );
        }
        hwsolver.latency_disabled[drv_id] &= ~mode_bit;
      }
    }
  }
  
  HWSLVR_UNLOCK();
}

#ifdef HWSOLVER_USES_ISLAND
void hwsolver_island_trans_cb( uSleep_state_notification state )
{
  if ( state == USLEEP_STATE_ENTER )
  {
    memcpy( hwsolver_bkup_latency_disabled, hwsolver.latency_disabled,
            sizeof(unsigned int)*HWSOLVER_NUM_DRVS );
  }
  else // state == USLEEP_STATE_EXIT
  {
    memcpy( hwsolver.latency_disabled, hwsolver_bkup_latency_disabled,
            sizeof(unsigned int)*HWSOLVER_NUM_DRVS );
  }
}
#endif

/** 
 * <!-- hwsolver_init -->
 * @brief Initialize the solver; program modes, timeslot:modes priority
 * tables and related registers.
 */
void hwsolver_init( void )
{
  int i, j;
  unsigned int enable_bmsk;
  unsigned int prev_timeslot = 0;
  
  CORE_VERIFY( hwsolver_num_sleep_modes > 0 );
  CORE_VERIFY( hwsolver_num_timeslot_tables > 0 );
  
  /* Setup logging */
  CORE_DAL_VERIFY( ULogFront_RealTimeInit( &hwsolver.log,
                                           "HWSolver Log", 8192,
                                           ULOG_MEMORY_LOCAL, ULOG_LOCK_NONE ) );
  
  /* Additional data sanity checks; timeslot at index 0 must be 0
     and the tables must be in increasing value of timeslot  */               
  CORE_VERIFY( hwsolver_timeslot_tables[0].timeslot == 0 );
  for ( i = 1; i < hwsolver_num_timeslot_tables; ++i )
  {
    CORE_VERIFY( prev_timeslot < hwsolver_timeslot_tables[i].timeslot );
    prev_timeslot = hwsolver_timeslot_tables[i].timeslot;
  }
  
  /* Program modes */
  for ( i = 0; i < hwsolver_num_sleep_modes; ++i )
  {
    HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_OUTMI( i,
      HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_RSC_SEQ_START_ADDR_BMSK,
      (unsigned)hwsolver_sleep_modes[i].rsc_seq_addr <<
        HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER0_DRV0_MODEm_RSC_SEQ_START_ADDR_SHFT );
        
    HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_OUTMI( i,
      HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_SEQ_START_ADDR_BMSK,
      (unsigned)hwsolver_sleep_modes[i].pdc_seq_addr <<
        HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_SEQ_START_ADDR_SHFT );
        
    if ( hwsolver_sleep_modes[i].pdc_seq_valid )
    {
      HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_OUTMI( i,
        HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_SEQ_VALID_BMSK,
        1 << HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_SEQ_VALID_SHFT );
    }
    else
    {
      HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_OUTMI( i,
        HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_SEQ_VALID_BMSK, 0 );
    }
    
    if ( hwsolver_sleep_modes[i].pdc_timer_valid )
    {
      HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_OUTMI( i,
        HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_TIMER_VALID_BMSK,
        1 << HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_TIMER_VALID_SHFT );
    }
    else
    {
      HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_OUTMI( i,
        HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER1_DRV0_MODEm_PDC_TIMER_VALID_BMSK, 0 );
    }
        
    HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_OUTMI( i,
      HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_RSC_BACKOFF_TIME_BMSK,
      hwsolver_sleep_modes[i].rsc_backoff <<
        HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER2_DRV0_MODEm_RSC_BACKOFF_TIME_SHFT );

    HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_OUTMI( i,
      HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_PDC_BACKOFF_TIME_BMSK,
      hwsolver_sleep_modes[i].pdc_backoff <<
        HWIO_SSC_RSCC_SOLVER_MODE_PARAMETER3_DRV0_MODEm_PDC_BACKOFF_TIME_SHFT );
  }

  /* Program timeslot table */
  for ( i = 0; i < hwsolver_num_timeslot_tables; ++i )
  {
    HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_OUTMI( i,
      HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_MIN_TIME_SLOT_BMSK,
      hwsolver_timeslot_tables[i].timeslot << 
        HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_MIN_TIME_SLOT_SHFT );
        
    for ( j = 0; j < hwsolver_timeslot_tables[i].num_modes; ++j )
    {
      HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_OUTMI2( i, j,
        HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_MODE_AT_PRIORITY_BMSK,
        hwsolver_timeslot_tables[i].modes[j] <<
          HWIO_SSC_RSCC_SOLVER_MODE_PRIORITY_TABLE_SLOTs_PRIORITYp_DRV0_MODE_AT_PRIORITY_SHFT );
    }
  }
  
  /* Write 0xFFFFFFFF as timeslot into unused slots */
  for (; i <= HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_MAXs; ++i )
  {
    HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_OUTMI( i,
      HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_MIN_TIME_SLOT_BMSK,
      0xFFFFFFFF << HWIO_SSC_RSCC_SOLVER_TIME_SLOT_TABLE_s_DRV0_MIN_TIME_SLOT_SHFT );
  }
  
  /* Enable all modes. We don't shift "enable_bmsk" by ENABLE_MASK_SHFT (0x1 -
     mode0 cannot be disabled) as our mode_idxs already map to the right bits */
  enable_bmsk = (1 << hwsolver_num_sleep_modes) - 1;
  HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_OUTMI( 0, 
    HWIO_SSC_RSCC_SOLVER_SOLVER_MODES_ENABLED_DRVd_MODE_ENABLE_MASK_BMSK,  
    enable_bmsk );
      
  ULOG_RT_PRINTF_0( hwsolver.log, "Enabled all modes" );  
      
  /* Program TIMERS_CONSIDERED */
  HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_OUTMI( 0,
    HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_TIMERS_CONSIDERED_ENABLE_MASK_BMSK,
    0x3 << HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_TIMERS_CONSIDERED_ENABLE_MASK_SHFT );
    
  /* Initialize soft idle and max duration times to their "unused" values */
  HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_OUTMI( 0,
    HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_SOFT_WAKEUP_TIME_LO_BMSK,
    0xFFFFFFFF << HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_SOFT_WAKEUP_TIME_LO_SHFT );
  
  HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_OUTMI( 0,
    HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_SOFT_WAKEUP_TIME_HI_BMSK,
    0xFFFFFFFF << HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_SOFT_WAKEUP_TIME_HI_SHFT );
  
  HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_OUTMI( 0,
    HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_MAX_IDLE_DURATION_BMSK,
    0xFFFFFFFF << HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_MAX_IDLE_DURATION_SHFT );    

  /* SSC QTimer frames configuration */
  /* Set count frequency for both frames */
  HWIO_SSC_QTMR_AC_CNTFRQ_OUTM( HWIO_SSC_QTMR_AC_CNTFRQ_CNTFRQ_BMSK, 
    0x0124F800 << HWIO_SSC_QTMR_AC_CNTFRQ_CNTFRQ_SHFT );
    
  /* Enable non-secure access to both frames. TODO: Neccessary? */
  HWIO_SSC_QTMR_AC_CNTNSAR_FG0_OUT( 0x3 );
  
  /* Set Frame 0 as the frame the HWSolver can use */
  HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_OUTM( 
    HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_HW_FRAME_SEL2_BMSK,
    0x1 << HWIO_SSC_QTMR_AC_HW_FRAME_SEL_2_HW_FRAME_SEL2_SHFT );

    /* Initialize the HWSolver LPR */
  npa_resource_available_cb( "/sleep/lpr", hwsolver_lpr_init, NULL );
  
  qurt_mutex_init(&hwsolver.lock);
    
#ifdef HWSOLVER_USES_ISLAND
  /* Initialize data structures etc. that need be accessible in island
     and normal modes */
  hwsolver_island_init();
#endif  
  
  /* Define a stub node to advertise presence of the hwsolver */
  npa_stub_resource( HWSOLVER_PRESENT_NODE_NAME );
}

#ifdef HWSOLVER_USES_ISLAND
/**
 * @brief hwsolver_island_init
 * Initialize island data structures, log handles etc. from within DDR/Normal
 * mode. Signature is due to the routine being a NPA resource available CB.
 */
void hwsolver_island_init( void )
{
  uint32 log_bufsz;

  CORE_VERIFY_PTR( 
    uSleep_registerNotificationCallback( 10, 10, hwsolver_island_trans_cb ) );

#ifdef HWSOLVER_USES_UNPA
  hwsolver.ulatency_qh = unpa_getQueryHandle( USLEEP_LATENCY_NODE_NAME );
  CORE_VERIFY_PTR( hwsolver.ulatency_qh );
  
  unpa_initResource( &hwsolver_mode_disable_rsrc_defn, 0, 
                     &hwsolver_mode_disable_rsrc );
#endif

  /* Create a lockless log */
  CORE_VERIFY( micro_ULog_CreateLog( &hwsolver_micro_log,
                                     "HWSolver Island Log",
                                     hwsolver_micro_logbuf,
                                     &log_bufsz, MICRO_ULOG_MEM_NEEDED(1024) )
                                       == MICRO_ULOG_SUCCESS );
}
#endif

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
void hwsolver_set_soft_wakeup_time( unsigned int drv_id, uint64 soft_wakeup )
{
  HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_OUTMI( drv_id,
    HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_SOFT_WAKEUP_TIME_LO_BMSK,
    (unsigned int)soft_wakeup << 
      HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_LO_DRVd_SOFT_WAKEUP_TIME_LO_SHFT );
  HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_OUTMI( drv_id,
    HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_SOFT_WAKEUP_TIME_HI_BMSK,
    (soft_wakeup >> 32) <<
      HWIO_SSC_RSCC_RSC_SOFT_WAKEUP_TIME_HI_DRVd_SOFT_WAKEUP_TIME_HI_SHFT );
}

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
                                unsigned int max_duration )
{
  HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_OUTMI( drv_id,
    HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_MAX_IDLE_DURATION_BMSK,
    max_duration << 
      HWIO_SSC_RSCC_RSC_MAX_IDLE_DURATION_DRVd_MAX_IDLE_DURATION_SHFT );
}

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
                                     unsigned int timers_considered )
{
  HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_OUTMI( drv_id,
    HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_TIMERS_CONSIDERED_ENABLE_MASK_BMSK,
    timers_considered << 
      HWIO_SSC_RSCC_RSC_TIMERS_CONSIDERED_DRVd_TIMERS_CONSIDERED_ENABLE_MASK_SHFT );
}


/** 
 * <!-- _hwsolver_program_override -->
 *
 * @brief Helper routine to program override registers
 */ 
void _hwsolver_program_override( int mode_idx, unsigned int idle_time )
{
  if ( mode_idx < 0 )
  { /* Override by providing idle time or sleep duration */
    HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_OUTM(
      HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_IDLE_TIME_BMSK, 
      idle_time << 
        HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_IDLE_TIME_SHFT );
  }
  else
  { /* Override by providing mode */
    CORE_VERIFY( mode_idx < hwsolver_num_sleep_modes );
    
    /* Clear the override idle time field */
    HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_OUTM(
      HWIO_SSC_RSCC_SOLVER_OVERRIDE_IDLE_TIME_DRV0_IDLE_TIME_BMSK, 0 );
      
    HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_OUTM(
      HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_MODE_BMSK,
      (1 << mode_idx) << HWIO_SSC_RSCC_SOLVER_OVERRIDE_MODE_DRV0_MODE_SHFT );
  }
}

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
void hwsolver_override_once( int mode_idx, unsigned int idle_time )
{
  _hwsolver_program_override( mode_idx, idle_time );
  
  /* Per HPG, SW must write 0 and then write 1 immediately to the ONETIME field
     of SOLVER_OVERRIDE_CTRL_DRV0 to make the one time override operation */
  HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_OUTM(
    HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_ONETIME_BMSK, 0 );

  HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_OUTM(
    HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_ONETIME_BMSK, 
    1 << HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_ONETIME_SHFT );
}

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
void hwsolver_override_continuous( int mode_idx, unsigned int idle_time )
{
  _hwsolver_program_override( mode_idx, idle_time );
  
  HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_OUTM(
    HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_CONTINUOUS_BMSK,
    1 << HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_CONTINUOUS_SHFT );
}

/** 
 * <!-- hwsolver_cancel_override -->
 *
 * @brief Cancel a continuous override of solver output.
 */ 
void hwsolver_cancel_override( void )
{
  HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_OUTM(
    HWIO_SSC_RSCC_SOLVER_OVERRIDE_CTRL_DRV0_CONTINUOUS_BMSK, 0 );
}

#ifndef HWSOLVER_USES_ISLAND
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
#endif

/** 
 * <!-- hwsolver_is_solver_in_override -->
 *
 * @brief Returns 1 if the solver is in continuous override mode.
 */ 
unsigned int hwsolver_is_solver_in_override( unsigned int drv_id )
{
  return HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_INMI( drv_id,
           HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_SOLVER_IN_OVERRIDE_BMSK ) >>
           HWIO_SSC_RSCC_SOLVER_STATUS0_DRVd_SOLVER_IN_OVERRIDE_SHFT;
}

#ifdef __cplusplus
}
#endif
