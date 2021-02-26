/*===========================================================================
                              aop_qdss.c

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
#include "rex.h"
#include "aop_interrupt_table.h"
#include "icb_aop.h"
#include "qdss_lite.h"
#include "target_aop_qdss.h"
#include "msmhwio.h"
#include "aop_hwio.h"
#include "CoreVerify.h"
#include "clock.h"
#include "pdc_global_hwio.h"
#include "qmp_msg_driver.h"
#include "aop_services.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------
static icb_handle_t icb_ddr_handle;
static icb_handle_t icb_mmnoc_handle;
static uint32 lock_mask;
static uint32 boot_lock_mask;
static bool aop_qdss_currently_tracing = false;

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
static const uint32 client_to_mask [] =
  {
    HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_APPS_DBG_RSRC_REL_LOCK_MASK_BMSK,
    HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_MODEM_DBG_RSRC_REL_LOCK_MASK_BMSK,
#ifndef SDX24_STUBS
    HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_SENSORS_DBG_RSRC_REL_LOCK_MASK_BMSK,
    HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_COMPUTE_DBG_RSRC_REL_LOCK_MASK_BMSK,
    HWIO_RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0_AUDIO_DBG_RSRC_REL_LOCK_MASK_BMSK
#endif
  };
    
struct qdss_log_t
{
  uint32 event;
  uint32 data;
};

struct qdss_log_t qdss_log[32];
int qdss_log_index = 0;
int qdss_resource_vote_on = 0;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
void aop_qdss_trace_resume (void);
void aop_qdss_trace_suspend (void);

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------

//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================
#define ARR_SIZE(x) ((sizeof(x) / sizeof(x[0])))

void log_qdss (uint32 event, uint32 data)
{
  qdss_log[qdss_log_index].event = event;
  qdss_log[qdss_log_index].data = data;
  qdss_log_index ++;
  if (qdss_log_index >= ARR_SIZE (qdss_log))
  {
    qdss_log_index = 0;
  }
}

/**
 * <!-- aop_qdss_init -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_init (void)
{
  // Setup the voting mechanism for DDR tracing needs
  icb_ddr_handle = icb_get_handle (ICBID_MASTER_AOSS, ICBID_SLAVE_EBI1);
  icb_mmnoc_handle = icb_get_handle (ICBID_MASTER_MDP0, ICBID_SLAVE_MNOC_HF_MEM_NOC);

  CORE_VERIFY (icb_ddr_handle != NULL);
  CORE_VERIFY (icb_mmnoc_handle != NULL);

  lock_mask = HWIO_IN (RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0);
  boot_lock_mask = lock_mask;
}

/**
 * <!-- aop_qdss_enable_ddr -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_enable_ddr (void)
{
  CORE_VERIFY (icb_ddr_handle != NULL);
  CORE_VERIFY (icb_mmnoc_handle != NULL);

  if (! qdss_resource_vote_on)
  {
    icb_enable_route (icb_ddr_handle, false);
    icb_enable_route (icb_mmnoc_handle, false);
    qdss_resource_vote_on = true;
  }
}

/**
 * <!-- aop_qdss_disable_ddr -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_disable_ddr (void)
{
  CORE_VERIFY (icb_ddr_handle != NULL);
  CORE_VERIFY (icb_mmnoc_handle != NULL);

  if (qdss_resource_vote_on)
  {
    icb_disable_route (icb_ddr_handle, false);
    icb_disable_route (icb_mmnoc_handle, false);
    qdss_resource_vote_on = false;
  }
}

/**
 * <!-- qdss_test -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_test (void)
{
  static bool first = true;

  if (first)
  {
    icb_enable_route (icb_ddr_handle, false);
    icb_enable_route (icb_mmnoc_handle, false);
    Clock_QDSSSetPerfLevel (CLOCK_QDSS_LEVEL_DYNAMIC);
    first = false;
  }
}

/**
 * <!-- aop_qdss_main -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
uint32_t aop_qdss_main (uint32 sigs)
{
  uint32_t ret_val = 0;
  // Debug PDC handling
  if (sigs & DBG_SIG_PDC_LOCK_SIG)
  {
    // Enable the unlock interrupt
    rex_clear_interrupt (DBG_RSRC_REL_PDC_AOP_UNLOCK_REQ);
    rex_enable_interrupt (DBG_RSRC_REL_PDC_AOP_UNLOCK_REQ);

    // Deassert the unlock ack
    HWIO_OUT (AOP_GPO_WDCLR, (1 << 5));
    
    // Assert the lock ack
    HWIO_OUT (AOP_GPO_WDSET, (1 << 4));

    // Suspend CX tracing
    aop_qdss_trace_suspend ();
    
    // Relinquish resource votes
    aop_qdss_disable_ddr ();
  }

  // Debug PDC handling
  if (sigs & DBG_SIG_PDC_UNLOCK_SIG)
  {
    // Enable the lock interrupt
    rex_clear_interrupt (DBG_RSRC_REL_PDC_AOP_LOCK_REQ);
    rex_enable_interrupt (DBG_RSRC_REL_PDC_AOP_LOCK_REQ);

    // Deassert the lock ack
    HWIO_OUT (AOP_GPO_WDCLR, (1 << 4));
    
    // Vote for resources
    aop_qdss_enable_ddr ();
    
    // Assert the unlock ack
    HWIO_OUT (AOP_GPO_WDSET, (1 << 5));

    aop_qdss_trace_resume ();

    ret_val = SERVICE_RESOURCE_DDR; // Keep DDR on.
    rex_clear_interrupt (DBG_RSRC_REL_PDC_AOP_UNLOCK_REQ);
  }

  return ret_val;
}

/**
 * <!-- aop_qdss_add_client -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_add_client (uint32 client)
{
  CORE_VERIFY (client < NUM_OF_SUBSYSTEMS);

  lock_mask &= ~client_to_mask[client];

  aop_qdss_enable_ddr ();
  aop_qdss_currently_tracing = true;

  HWIO_OUT (RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0, lock_mask);
}

/**
 * <!-- aop_qdss_remove_client -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_remove_client (uint32 client)
{
  CORE_VERIFY (client < NUM_OF_SUBSYSTEMS);

  lock_mask |= client_to_mask[client];

  if (lock_mask == boot_lock_mask)
  {
    aop_qdss_disable_ddr ();
    aop_qdss_currently_tracing = false;
  }

  HWIO_OUT (RPMH_PDC_DBG_RSRC_REL_LOCK_MASK_BANK0, lock_mask);
}

/**
 * <!-- aop_qdss_is_trace_enabled -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
bool aop_qdss_is_trace_enabled (void)
{
  return (lock_mask != boot_lock_mask);
}

/**
 * <!-- aop_qdss_trace_resume -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_trace_resume (void)
{
  // If DDR tracing is enabled, turn on the DDR vote and start the ETB
  if (!aop_qdss_currently_tracing && aop_qdss_is_trace_enabled ())
  {
    CORE_VERIFY (QDSSCXTraceResume () == 0);
    aop_qdss_currently_tracing = true;
  }
}

/**
 * <!-- aop_qdss_trace_suspend -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_trace_suspend (void)
{
  // If DDR tracing is enabled, turn off the DDR vote and flush the ETB
  if (aop_qdss_currently_tracing && aop_qdss_is_trace_enabled ())
  {
    CORE_VERIFY (QDSSCXTraceSuspend () == 0);
    aop_qdss_currently_tracing = false;
  }
}
