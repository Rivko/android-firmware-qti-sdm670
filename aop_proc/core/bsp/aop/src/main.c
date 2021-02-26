/*===========================================================================
                              main.c

DESCRIPTION:

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <assert.h>
#include "cortex-m3.h"
#include "comdef.h"
#include "time_service.h"
//#include "image_layout.h"
#include "aop_interrupt_table.h"
#include "rex.h"
#include <string.h>
#include "rpmh_client.h"
#include "pdc_global.h"
#include "pdc_seq.h"
#include "cmd_db_int.h"
#include "cmd_db_aop.h"
#include "icb_aop.h"
#include "clock.h"
#include "dog.h"
#include "aop_mpu.h"
#include "qmp_msg_driver.h"
#include "aop_ulog.h"
//#include "aop_qdss.h"
#include "pm_resources_and_types.h"
#include "vrm.h"
#include "CoreVerify.h"
#include "DALStdDef.h"

#if defined(SDM670_STUBS) || defined(SDM855_STUBS) || defined(SDM1000_STUBS)
#include "cprf.h"
#endif

#if defined(SDM855_STUBS) || defined(SDM1000_STUBS)
#include "HALhwio.h"
#include "msmhwiobase.h"
#define RPMH_PDC_AOP_AOP_PDC_REG_BASE                                                           (AOSS_BASE      + 0x00260000)
#define HWIO_RPMH_PDC_AOP_ENABLE_PDC_ADDR                                                       (RPMH_PDC_AOP_AOP_PDC_REG_BASE      + 0x00004500)
#define HWIO_RPMH_PDC_AOP_ENABLE_PDC_RMSK                                                       0x80000001
#define HWIO_RPMH_PDC_AOP_ENABLE_PDC_IN          \
        in_dword_masked(HWIO_RPMH_PDC_AOP_ENABLE_PDC_ADDR, HWIO_RPMH_PDC_AOP_ENABLE_PDC_RMSK)
#define HWIO_RPMH_PDC_AOP_ENABLE_PDC_ENABLE_PDC_BMSK                                                   0x1
#endif

#define AUTO_MODE_VAL     0x6
//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------
rex_tcb_type main_idle_tcb;
REX_DECLARE_STACK(main_idle_stack, 768);

extern void aop_sleep_init (void);
extern void service_fsm_init(void);
extern void pm_init (void);
extern void vrm_task_init(void);
extern void aop_sleep_main (void);
extern void ddr_mgr_init(void);
extern void pdcTcs_initialize(void);
extern void arc_init (void);
extern void arc_test (void);
extern void cpr_init (void);
extern void xport_qmp_init(void *arg);
#ifdef XPORT_QMP_TEST
extern void xport_qmp_test_init(void);
#endif
extern void unpa_init (void);
extern void aop_server_init (void);
extern void aop_cx_collapse_init (void);
extern void msg_ram_init (void);
extern void arc_runtime_init (void);
extern void msg_ram_set_boot_cookie (void);
extern void aop_sleep_stats_init (void);
extern void qmp_msg_init(void);
extern void QDSSInit(void);
extern void aop_qdss_init (void);
extern void lpi_mx_scaling(void);
extern DALResult ChipInfo_Init(void);
extern void pil_driver_init(void);
#if DAL_CONFIG_TARGET_ID == 0x845 
extern void gpu_clampIO_init( void );
#endif
extern void aop_reclaim_code_ram_pool(void);
void debug_init (void);

//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================
/**
 * <!-- set_pmic_modes -->
 *
 * @brief Sets the pmic mode for system rails - CX, MX and EBI to auto mode
 *
 * @return none
 */
void set_pmic_modes (void)
{
  CORE_VERIFY (vrm_arc_drv_mode_update ("vrm.cx", AUTO_MODE_VAL) == VRM_SUCCESS);
  CORE_VERIFY (vrm_arc_drv_mode_update ("vrm.mx", AUTO_MODE_VAL) == VRM_SUCCESS);
#if !defined(SDM670_STUBS)  
  CORE_VERIFY (vrm_arc_drv_mode_update ("vrm.ebi", AUTO_MODE_VAL) == VRM_SUCCESS);
#endif
}

#if defined(SDM1000_STUBS)
//CPR STUBS
void cpr_init () {}
uint32 cpr_get_ret_voltage (uint8 id) { return 0; }
cpr_return_status cpr_enable_controller (uint8 id, boolean enable) { return 0;}
cpr_return_status cpr_enable_controller_force (uint8 id, boolean enable) { return 0;}
cpr_return_status cpr_toggle_cprf_interface_en(uint8 id, boolean enable) { return 0; };
#endif

/**
 * <!-- main_idle -->
 *
 * @brief This is the idle task. All decisions of sleep are handled inside this task.
 *
 * @param 
 * @param 
 *
 * @return This function does not return from execution.
 */
void main_idle (dword param1)
{
  qmp_msg_init();

  /* Need to call after qmp_msg_init */
  Clock_PostInit();

  pil_driver_init();

#ifdef XPORT_QMP_TEST
  xport_qmp_test_init();
#endif

  vrm_task_init();

  aop_server_init ();

  ddr_mgr_init();

  icb_post_init();

#if !defined(SDX24_STUBS) && !defined(SDM1000_STUBS)
  aop_cx_collapse_init ();
#endif


  aop_qdss_init ();

  /* Initialize watchdog timer */
  dog_init();

  msg_ram_set_boot_cookie();

  //pdcTcs_initialize();

#if !defined(SDX24_STUBS) && !defined(SDM1000_STUBS)
  set_pmic_modes ();
#endif

#if !defined(SDX24_STUBS) && !defined(SDM855_STUBS) && !defined(SDM1000_STUBS)
  lpi_mx_scaling();  
#endif


  service_fsm_init();


  while (1)
  {
    aop_sleep_main ();
  }
}

/**
 * <!-- main -->
 *
 * @brief The main entry point for the AOP software
 *
 * @return This function does not return. Execution is handed off to the main_idle task.
 */
int main(void)
{
  // Lock all interrupts until initialization is complete. Interrupts will be unlocked once the main idle
  // task stars running
  lock_ints ();
  
  /* Timer service init */
  time_service_init ();

  /* Configure AOP interrupts and register exception ISRs */
  aop_interrupt_init ();

  /* Enable MPU to detect null pointer dereference and stack overflow */
  aop_mpu_init ();

  /* Populate chip info*/
  ChipInfo_Init();

 

  /* PDC global wrapper init */
  pdc_global_init ();

  msg_ram_init();

  /* PDC SEQ common (instance agnostic) init */
  pdc_seq_driver_init();
  cmd_db_aop_init ();
  cmd_db_init ();
  rpmh_client_init ();

  unpa_init ();

  pm_init ();
   
  aop_reclaim_code_ram_pool();

#if defined(SDM1000_STUBS) /* until aop_sleep_init is enabled */
  out_dword_masked_ns(HWIO_RPMH_PDC_AOP_ENABLE_PDC_ADDR, HWIO_RPMH_PDC_AOP_ENABLE_PDC_ENABLE_PDC_BMSK, 1, HWIO_RPMH_PDC_AOP_ENABLE_PDC_IN);
  extern void aop_timer_init();
  aop_timer_init();
#endif 

#if !defined(SDM1000_STUBS)
  /* Initializing CPR HW blocks */
  cpr_init ();
#endif

  /* Initializing ARC HW accelerator */
  arc_init ();

  icb_init();

  Clock_Init();

#if DAL_CONFIG_TARGET_ID == 0x845 
  gpu_clampIO_init();
#endif

#if !defined(SDM1000_STUBS)
  aop_sleep_init ();
#endif

  aop_sleep_stats_init ();
  
  xport_qmp_init(NULL);

  arc_runtime_init ();  

  /* All entry additions to Cmd DB must be completed prior to this call */
  cmd_db_aop_update_msg_ram_info();

  debug_init();

  QDSSInit();

  // Start the first thread, and kick off the rex scheduler
  memset (&main_idle_tcb, 0, sizeof (main_idle_tcb));

  // Allow interrupts
  unlock_ints ();
  
  rex_def_task (&main_idle_tcb, main_idle_stack, REX_STACK_SIZE(main_idle_stack), 100, main_idle, 0);
  return 0;
}
