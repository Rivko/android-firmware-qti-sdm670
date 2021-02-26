/*===========================================================================
                              aop_cx_collapse.c

DESCRIPTION:

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "CoreVerify.h"
#include "aop_cx_collapse.h"
#include "pwr_utils_lvl.h"
#include "arc_irq_hwio.h"
#include "unpa_resource.h"
#include "msmhwiobase.h"
#include "hwio_arc_rm.h"
#include "ddr_mgmt.h"
#include "aop_sleep_stats.h"
#include "aoss_misc_hwio.h"
#include "aoss_apm.h"
#ifdef SDM855_TARGET_CONFIG
  #include "../../systemdrivers/clock/hw/sdm855/inc/ClockHWIO.h"
#else
  #include "../../systemdrivers/clock/hw/sdm845/inc/ClockHWIO.h"
#endif  
#include "vdd_cx_apm_hwio.h"
#include "arc_sw_override_hwio.h"
#include "cprf.h"
#include "aop_ulog.h"
#ifdef QCS605_TARGET_CONFIG
	#include "pm_app_pwr.h"
	#include "cprf.h"
	#include "rpmh_arc.h"
#endif

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
// Add this many mV to the retention voltage for correct APM operation
#define APM_CX_RETENTION_OFFSET (50)

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------
boolean g_cx_is_collapsed = FALSE;
#ifdef QCS605_TARGET_CONFIG
  unpa_client *arc_mx_vote_nom = NULL;
#endif
//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
cx_collapse_t cx_collapse;
static unpa_resource_state last_mx_state;
static unsigned int last_sequence_level;
uint32 g_auto_transition = 0;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------
extern unsigned int arc_to_rm[];
extern void arc_unblock_rm_postwork (unsigned int arc_rm);
extern void arc_unblock_rm_prework (unsigned int arc_rm);
extern void arc_xo_rm_vote(unsigned int state, boolean CompletionRequired);
//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================
/**
 * <!-- cx_collapse_voltage_comp_done -->
 *
 * @brief callback function invovked when the comparator is done
 *
 * @return none
 */
void cx_collapse_voltage_comp_done (void)
{
  aop_fsm_process_event (&cx_collapse.fsm, CX_COLLAPSE_EVENT_APM);
}

/**
 * <!-- rail_change_handler -->
 *
 * @brief Main callback function for any rail change indication
 *
 * @param c - the client 
 * @param event - the type of event
 * @param event_data - data associated with the event
 * @param cb_data - callback data that the client has registered for
 *
 * @return 
 */
static uint32 rail_change_handler (arc_runtime_client_t *c,
                                   arc_runtime_event_t event,
                                   uint32 event_data,
                                   uint32 cb_data)
{
  CORE_VERIFY (c != NULL);
  CORE_VERIFY (c->arc == RPMH_ARC_VDD_CX);

  last_sequence_level = event_data;

  if ((last_sequence_level <= 1) || (cx_collapse.fsm.current_state != CX_COLLAPSE_STATE_IDLE))
  {
    if (event == ARC_RUNTIME_EVENT_PREWORK)
    {
      aop_fsm_process_event (&cx_collapse.fsm, CX_COLLAPSE_EVENT_CX_PRE);
    }
    else if (event == ARC_RUNTIME_EVENT_POSTWORK)
    {
      aop_fsm_process_event (&cx_collapse.fsm, CX_COLLAPSE_EVENT_CX_POST);
    }

    // Auto transition to stage 3 for now
    if (cx_collapse.fsm.current_state == CX_COLLAPSE_STATE_STAGE_2)
    {
      if (g_auto_transition)
      {
        aop_fsm_process_event (&cx_collapse.fsm, CX_COLLAPSE_EVENT_APM);
      }
      else
      {
        c->blocked = true;
      }
    }
  }
  
  return 0;
}


/**
 * <!-- idle_enter -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
static void idle_enter (AOP_FSM *fsm, unsigned int event)
{
  // Turn off pmux clocks

  // Vote MX to retention
  unpa_issue_request (cx_collapse.arc_mx_npa_client, RAIL_VOLTAGE_LEVEL_RET);
  // Vote CX retention
  unpa_issue_request (cx_collapse.arc_cx_npa_client, RAIL_VOLTAGE_LEVEL_RET);

  // Vote 0mV to VRM for CX
  unpa_issue_request (cx_collapse.vrm_cx_npa_client, 0);

  HWIO_OUTMI (RMm_SEQ_SW_CONF_EVENT, 0, 0x80, 0x00);

}

/**
 * <!-- stage_1_enter -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
static void stage_1_enter (AOP_FSM *fsm, unsigned int event)
{
  // Vote CX off
  unpa_issue_request (cx_collapse.arc_cx_npa_client, RAIL_VOLTAGE_LEVEL_OFF);

  //Turn off XO Off when going below MOL.
  arc_xo_rm_vote(XO_BUFFER_OFF, FALSE);
}

/**
 * <!-- stage_1_exit -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
static unsigned int stage_1_exit (AOP_FSM *fsm, unsigned int event, unsigned int state)
{
  unsigned int next_state = CX_COLLAPSE_STATE_IDLE;
  
  // TODO: remove hard coded level
  if (last_sequence_level == 0)
  {
    next_state = CX_COLLAPSE_STATE_STAGE_2;
  }
  else
  {
    next_state = CX_COLLAPSE_STATE_IDLE;
    unpa_issue_request (cx_collapse.arc_cx_npa_client, RAIL_VOLTAGE_LEVEL_RET);
  }

  return next_state;
}

/**
 * <!-- stage_2_enter -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
static void stage_2_enter (AOP_FSM *fsm, unsigned int event)
{
  SWEVENT (CX_COLLAPSE_ENTER);
  
#ifdef SDM855_TARGET_CONFIG
  uint32_t mx_max_vlvl = RAIL_VOLTAGE_LEVEL_TUR_L1;
#else
  uint32_t mx_max_vlvl = RAIL_VOLTAGE_LEVEL_TUR;
#endif
  
  // Vote to VRM directly - CX for retention + 50mV, completion required
  // This is mainly to ensure that the PMUX is operational
  //
  uint32 v = cpr_get_ret_voltage (RPMH_ARC_VDD_CX);
  unpa_issue_request (cx_collapse.vrm_cx_npa_client, v + APM_CX_RETENTION_OFFSET);
  
  // Vote MX to max vlvl
  // unpa_set_request_attribute (cx_collapse.arc_mx_npa_client, UNPA_REQUEST_FIRE_AND_FORGET);
  unpa_issue_request (cx_collapse.arc_mx_npa_client, mx_max_vlvl);

  // Enable clock to PMUX
  HWIO_OUT (AOSS_CC_PWR_MUX_CTRL_CBCR, 1);

  // Enable Voltage Comparator
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_FUNC_OUT, EN_COMP, 1);
  HWIO_OUTF(VDD_CX_APM_CONTROLLER_FUNC_OUT, COMP_SPEC, 0x2);
  HWIO_OUTF(VDD_CX_APM_CONTROLLER_FUNC_OUT, COMP_INT_EN, 1);
  // Check?
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_CONFIG_7, COMP_INST_HYST_TIMER, 50);
}

/**
 * <!-- stage_3_enter -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
static void stage_3_enter (AOP_FSM *fsm, unsigned int event)
{
  // Turn off comparator
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_FUNC_OUT, EN_COMP, 0);
  HWIO_OUTF(VDD_CX_APM_CONTROLLER_FUNC_OUT, COMP_INT_EN, 0);
   
  // Set PMUX to use retention pole
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_FUNC_OUT,ENABLE_NORMAL_OP, 0x1);
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_FUNC_OUT,TILE_ACT_BR, 2);
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_FUNC_OUT, APM_AON_OVERRIDE_AUX, 1);

  arc_runtime_client_event_dispatch (cx_collapse.arc_cx_runtime, ARC_RUNTIME_EVENT_PREWORK_UNBLOCK, 0);
}

/**
 * <!-- stage_4_enter -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
static void stage_4_enter (AOP_FSM *fsm, unsigned int event)
{
  g_cx_is_collapsed = TRUE;
  // Turn off clock to pmux
  HWIO_OUT (AOSS_CC_PWR_MUX_CTRL_CBCR, 0);
  
  // Restore MX vote
  unpa_set_request_attribute (cx_collapse.arc_mx_npa_client, UNPA_REQUEST_FIRE_AND_FORGET);
  unpa_issue_request (cx_collapse.arc_mx_npa_client, RAIL_VOLTAGE_LEVEL_RET);

  SWEVENT (CX_COLLAPSE_ENTER_COMPLETE);
  
  // Update stats
  aop_sleep_stats_enter (AOP_SLEEP_STATS_CXSD);
}

/**
 * <!-- stage_5_enter -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
static void stage_5_enter (AOP_FSM *fsm, unsigned int event)
{
  SWEVENT (CX_COLLAPSE_EXIT);

#ifdef SDM855_TARGET_CONFIG
  uint32_t mx_max_vlvl = RAIL_VOLTAGE_LEVEL_TUR_L1;
#else
  uint32_t mx_max_vlvl = RAIL_VOLTAGE_LEVEL_TUR;
#endif

  HWIO_OUT (AOSS_CC_PWR_MUX_CTRL_CBCR, 1);
  
  // Update stats
  aop_sleep_stats_exit (AOP_SLEEP_STATS_CXSD);

  HWIO_OUTMI (RMm_SEQ_SW_CONF_EVENT, 0, 0x80, 0x80);
  HWIO_OUTMI (RMm_SEQ_SW_CONF_EVENT, 0, 0x40, 0x40);

  // Boost MX
  unpa_set_request_attribute (cx_collapse.arc_mx_npa_client, UNPA_REQUEST_FIRE_AND_FORGET);
  unpa_issue_request (cx_collapse.arc_mx_npa_client, mx_max_vlvl);

  // Vote CX
  unpa_set_request_attribute (cx_collapse.arc_cx_npa_client, UNPA_REQUEST_FIRE_AND_FORGET);
  unpa_issue_request (cx_collapse.arc_cx_npa_client, RAIL_VOLTAGE_LEVEL_RET);

  // Need comparator stuff?
  // HWIO_OUTF (VDD_CX_APM_CONTROLLER_FUNC_OUT, EN_COMP, 1);

  // works, but looks wrong COMP_SPEC should be 0x1
  // HWIO_OUTF(VDD_CX_APM_CONTROLLER_FUNC_OUT, COMP_SPEC, 0x1);
  // HWIO_OUTF(VDD_CX_APM_CONTROLLER_FUNC_OUT, COMP_INT_EN, 1);
  
  // Set MUX to active pole
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_FUNC_OUT, TILE_ACT_BR, 0x1);
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_FUNC_OUT, APM_AON_OVERRIDE_AUX, 0);
}

/**
 * <!-- stage_5_exit -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
static unsigned int stage_5_exit (AOP_FSM *fsm, unsigned int event, unsigned int state)
{
  // Vote to ARC MX: retention, FF
  unpa_set_request_attribute (cx_collapse.arc_mx_npa_client, UNPA_REQUEST_FIRE_AND_FORGET);
  unpa_issue_request (cx_collapse.arc_mx_npa_client, RAIL_VOLTAGE_LEVEL_RET);

  // Allow CX sequence to continue
  // arc_unblock_rm_postwork (arc_to_rm[RESOURCE_CX]);

  // CX is now restored
  g_cx_is_collapsed = FALSE;

  SWEVENT (CX_COLLAPSE_EXIT_COMPLETE);

  return CX_COLLAPSE_STATE_IDLE;
}

/**
 * <!-- func_name -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
static void cx_collapse_pmux_init (void)
{
  uint32 rdata;
  HWIO_OUT (AOSS_CC_PWR_MUX_CTRL_CBCR, 1);
  HWIO_OUT (APM_SW_OVERRIDE, 0);
 
  // HWIO_OUTF(VDD_CX_APM_CONTROLLER_FUNC_OUT,APM_RST_OBSER_ARES,0x1);
  // HWIO_OUTF(VDD_CX_APM_CONTROLLER_FUNC_OUT,APM_RST_OBSER_ARES,0x0);
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_FUNC_OUT,TILE_ACT_BR, 0x1);
  //HWIO_OUTF(VDD_CX_APM_CONTROLLER_FUNC_OUT,RST_USE_DOM_POR,0x1);
  // HWIO_OUTF(VDD_CX_APM_CONTROLLER_FUNC_OUT,DIS_SELF_TIMED_OUT_MX_POR,0x1);
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_CONFIG_5,APM2TILE_DOM2AUX_FDBK_CONFIG_SEL, 0x1); //Programming the 1st bit as 1 power muxes in the cx
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_CONFIG_8,APM2TILE_AUX2DOM_FDBK_CONFIG_SEL, 0x1); //Programming the 1st bit as 1 power muxes in the cx

  
  rdata = HWIO_INF (VDD_CX_APM_CONTROLLER_CONFIG_0 , APM_CONFIG);
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_CONFIG_0,APM_CONFIG, (rdata | 0x10));
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_FUNC_OUT,ENABLE_NORMAL_OP, 0x1);

  HWIO_OUT (VDD_CX_APM_CONTROLLER_CONFIG_1, 0x05057005);
  HWIO_OUT (VDD_CX_APM_CONTROLLER_CONFIG_2, 0x05050570);

  //Below settings are in sync with napali v2  
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_CONFIG_5, APM2TILE_DOM2AUX_FDBK_CONFIG_SEL, 0x01400001);
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_CONFIG_6, APM2TILE_DOM2AUX_FDBK_CONFIG_SEL, 0x0);
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_CONFIG_8, APM2TILE_AUX2DOM_FDBK_CONFIG_SEL, 0x01400001);
  HWIO_OUTF (VDD_CX_APM_CONTROLLER_CONFIG_9, APM2TILE_AUX2DOM_FDBK_CONFIG_SEL, 0x0);
  
  //HWIO_OUT(AOSS_CC_PWR_MUX_CTRL_CBCR, 0);
  HWIO_OUT(APM_SW_OVERRIDE, 7);
  HWIO_OUT (AOSS_CC_PWR_MUX_CTRL_CBCR, 0);
}

/**
 * <!-- aop_cx_collapse_init -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_cx_collapse_init (void)
{
  // Initialize the FSM
  aop_fsm_init (&cx_collapse.fsm, CX_COLLAPSE_STATE_NUM_STATES, "cx_collapse_fsm");

  // Initialize the states in the FSM
  aop_fsm_init_state (&cx_collapse.fsm, CX_COLLAPSE_STATE_IDLE, &idle_enter, NULL, "IDLE");

  aop_fsm_init_state (&cx_collapse.fsm, CX_COLLAPSE_STATE_STAGE_1,
                      &stage_1_enter, &stage_1_exit, "STAGE_1");

  aop_fsm_init_state (&cx_collapse.fsm, CX_COLLAPSE_STATE_STAGE_2, &stage_2_enter,
                      NULL, "STAGE_2");

  aop_fsm_init_state (&cx_collapse.fsm, CX_COLLAPSE_STATE_STAGE_3, &stage_3_enter,
                      NULL, "STAGE_3");

  aop_fsm_init_state (&cx_collapse.fsm, CX_COLLAPSE_STATE_STAGE_4, &stage_4_enter,
                      NULL, "STAGE_4");


  aop_fsm_init_state (&cx_collapse.fsm, CX_COLLAPSE_STATE_STAGE_5, &stage_5_enter,
                      &stage_5_exit, "STAGE_5");
  
  // Initialize the transition events
  aop_fsm_add_event (&cx_collapse.fsm, CX_COLLAPSE_STATE_IDLE, CX_COLLAPSE_EVENT_CX_POST,
                     0, CX_COLLAPSE_STATE_STAGE_1);

  // Next state will be determined in the stage 1 exit function
  aop_fsm_add_event (&cx_collapse.fsm, CX_COLLAPSE_STATE_STAGE_1, CX_COLLAPSE_EVENT_CX_PRE,
                     0, 0);

  aop_fsm_add_event (&cx_collapse.fsm, CX_COLLAPSE_STATE_STAGE_2, CX_COLLAPSE_EVENT_APM,
                     0, CX_COLLAPSE_STATE_STAGE_3);
  
  aop_fsm_add_event (&cx_collapse.fsm, CX_COLLAPSE_STATE_STAGE_3, CX_COLLAPSE_EVENT_CX_POST,
                     0, CX_COLLAPSE_STATE_STAGE_4);


  aop_fsm_add_event (&cx_collapse.fsm, CX_COLLAPSE_STATE_STAGE_4, CX_COLLAPSE_EVENT_CX_PRE,
                     0, CX_COLLAPSE_STATE_STAGE_5);

  
  aop_fsm_add_event (&cx_collapse.fsm, CX_COLLAPSE_STATE_STAGE_5, CX_COLLAPSE_EVENT_CX_POST,
                     0, CX_COLLAPSE_STATE_IDLE);

  // NPA
  cx_collapse.arc_cx_npa_client = unpa_create_client ("cx_collapse", UNPA_CLIENT_REQUIRED, "cx.lvl");
  cx_collapse.arc_mx_npa_client = unpa_create_client ("cx_collapse", UNPA_CLIENT_REQUIRED, "mx.lvl");
  cx_collapse.vrm_cx_npa_client = unpa_create_client ("cx_collapse", UNPA_CLIENT_REQUIRED, "vrm.cx");
  
  CORE_VERIFY (cx_collapse.arc_cx_npa_client != NULL);
  CORE_VERIFY (cx_collapse.arc_mx_npa_client != NULL);

  // Put in a default vote of retention
  unpa_issue_request (cx_collapse.arc_cx_npa_client, RAIL_VOLTAGE_LEVEL_RET);
  
#ifdef QCS605_TARGET_CONFIG
  arc_mx_vote_nom = unpa_create_client ("mx_nom_vote", UNPA_CLIENT_REQUIRED, "mx.lvl");       // Client to keep NOM vote on MX always
  CORE_VERIFY_PTR (arc_mx_vote_nom);
  unpa_issue_request (arc_mx_vote_nom, RAIL_VOLTAGE_LEVEL_NOM);
  
  pm_app_rpmh_core_rail_set_voltage(MX, (cpr_get_ret_voltage (RPMH_ARC_VDD_MX)* 1000) , RETENTION );  //Update spare regs to keep MX to ret in PBS
  pm_app_rpmh_core_rail_set_mode(MX , 0x3 , RETENTION);
  #endif
  
  // For the VRM, put in a default votes
  unpa_issue_request (cx_collapse.vrm_cx_npa_client, 0);

  cx_collapse.arc_cx_runtime = arc_runtime_register (RPMH_ARC_VDD_CX, rail_change_handler, 0);
  CORE_VERIFY (cx_collapse.arc_cx_runtime != NULL);

  arc_runtime_event_register (cx_collapse.arc_cx_runtime, ARC_RUNTIME_EVENT_PREWORK);
  arc_runtime_event_register (cx_collapse.arc_cx_runtime, ARC_RUNTIME_EVENT_POSTWORK);

  cx_collapse_pmux_init ();

}
