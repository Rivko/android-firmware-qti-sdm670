/*===========================================================================

FILE:        icb_aop_target.c

DESCRIPTION: This file describes the target code for ICB on AOP.

   Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
             All Rights Reserved
             QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/systemdrivers/icb/src/670/icb_aop_target.c#2 $ 

                      EDIT HISTORY FOR FILE

 when         who   what, where, why
 ----------   ---   -----------------------------------------------------------
 2017/07/20   sds   Add MMNOC TBU workaround
 2017/04/12   sds   Add QUPv3 vote proxy
 2017/03/15   sds   Use clock APIs to fetch MC/SHUB CPs
 2017/01/11   sds   Initial version

=============================================================================*/
#include "icb_aopi.h"
#include "CoreVerify.h"
#include "bcm_hwio.h"
#include "bcm_wa_hwio.h"
#include "cmd_db.h"
#include "aop_interrupt_table.h"
#include "clock.h"
#include "arc.h"
#include "rex.h"
#include "aop_services.h"

/*============================================================================
                               DEFINITIONS
============================================================================*/
#define QUP_VCD 8
#define MMNOC_VCD 4
#define HLOS_DRV_ID 2
#define HLOS_ACV (1 << 3)
#define BCM_OFFSET_MASK 0xFFFF
#define MMNOC_WA_ENABLE 0
/*============================================================================
                            TYPE DECLARATIONS
============================================================================*/
typedef enum
{
  BCM_VCD_MC,
  BCM_VCD_SHUB,
  BCM_VCD_MMNOC,
  BCM_VCD_SNOC,
  BCM_VCD_CE,
  BCM_VCD_IPA,
  BCM_VCD_CNOC,
  BCM_VCD_QUP,
  BCM_VCD_ACV,
  BCM_VCD_ALC,
} icb_bcm_cd_type_t;

struct icb_bcm_cd_t;
typedef void (*icb_bcm_cd_fcn)(struct icb_bcm_cd_t *);
typedef struct icb_bcm_cd_t
{
  icb_bcm_cd_type_t type;      /**< VCD Identifier. */
  uint32_t          id;        /**< VCD index in hardware. */
  icb_bcm_cd_fcn    proxy_fcn; /**< Proxy vote init function. */
  const char *      bcm_name; /**< Associated bcm for proxy vote. */
  uint32_t          bcm_idx;  /**< Calculated bcm index. */
} icb_bcm_cd_t;

typedef struct
{
  uint32_t      num_cds;
  icb_bcm_cd_t *cds;
} icb_bcm_cds_t;

typedef enum
{
  BCM_IDLE = 0,
  BCM_WAIT_POST_INIT,
  BCM_WAIT_MMNOC_POWER_UP,
  BCM_WAIT_DVM_HALT,
  BCM_WAIT_TBU_HALT,
  BCM_WAIT_NOC_HALT,
  BCM_WAIT_DVM_UNHALT,
  BCM_WAIT_TBU_UNHALT,
  BCM_WAIT_NOC_UNHALT,
} icb_bcm_wait_state_t;

/*============================================================================
                           FUNCTION DECLARATIONS
============================================================================*/
static void bcm_shub_mc_init_proxy(icb_bcm_cd_t *);
static void bcm_cx_init_proxy(icb_bcm_cd_t *);
static void bcm_ab_init_proxy(icb_bcm_cd_t *);
static void bcm_acv_init_proxy(icb_bcm_cd_t *);

/*============================================================================
                            DATA DECLARATIONS
============================================================================*/
static icb_info_t *icb_info = NULL;
static int8_t cx_op_level = 0;
//static icb_handle_t cnoc_handle = NULL;
extern rex_crit_sect_type match_lock;
static volatile icb_bcm_wait_state_t wait_state = BCM_IDLE;

/*============================================================================
                         TARGET DATA DECLARATIONS
============================================================================*/
static icb_bcm_cd_t target_cds[] =
{
  [0] = 
  {
    .type      = BCM_VCD_MC,
    .id        = 0,
    .proxy_fcn = bcm_shub_mc_init_proxy,
    .bcm_name  = "MC0",
  },
  [1] = 
  {
    .type      = BCM_VCD_SHUB,
    .id        = 1,
    .proxy_fcn = bcm_shub_mc_init_proxy,
    .bcm_name  = "SH0",
  },
  [2] = 
  {
    .type      = BCM_VCD_MMNOC,
    .id        = 4,
    .proxy_fcn = NULL,
  },
  [3] = 
  {
    .type      = BCM_VCD_SNOC,
    .id        = 3,
    .proxy_fcn = bcm_cx_init_proxy,
    .bcm_name  = "SN0",
  },
  [4] = 
  {
    .type = BCM_VCD_CE,
    .id   = 6,
  },
  [5] = 
  {
    .type = BCM_VCD_IPA,
    .id   = 7,
  },
  [6] = 
  {
    .type      = BCM_VCD_CNOC,
    .id        = 5,
    .proxy_fcn = bcm_cx_init_proxy,
    .bcm_name  = "CN0",
  },
  [7] = 
  {
    .type = BCM_VCD_QUP,
    .id   = 8,
    .proxy_fcn = bcm_ab_init_proxy,
    .bcm_name  = "QUP0",
  },
  [8] = 
  {
    .type      = BCM_VCD_ACV,
    .id        = 11,
    .proxy_fcn = bcm_acv_init_proxy,
    .bcm_name  = "ACV",
  },
  [9] = 
  {
    .type = BCM_VCD_ALC,
    .id   = 10,
  },
};

static icb_bcm_cds_t target_data =
{
  .num_cds = 10,
  .cds =     target_cds,
};

/*============================================================================
                      INTERNAL FUNCTION DECLARATIONS
============================================================================*/
/**
*/
static void bcm_ab_init_proxy
(
  icb_bcm_cd_t *cd
)
{
  uint32_t vote;

  vote = BCM_VOTETABLE_COMMIT_BMSK | BCM_VOTETABLE_VOTE_VALID_BMSK |
         (1 << BCM_VOTETABLE_VOTE_X_SHFT);
  HWIO_OUTI2(VOTETABLE_DRVr_NDd, HLOS_DRV_ID, cd->bcm_idx, vote);
}

/**
*/
static void bcm_shub_mc_init_proxy
(
  icb_bcm_cd_t *cd
)
{
  ClockCfgCPType *clock_cfg_cp = Clock_GetBootConfigCP();
  uint8_t cp;

  /* Fetch from boot config data */
  if( NULL != clock_cfg_cp )
  {
    switch( cd->type )
    {
      case BCM_VCD_SHUB:
        cp = clock_cfg_cp->nSHUB;
        break;
      case BCM_VCD_MC:
        cp = clock_cfg_cp->nDDR;
        break;
      /* Shouldn't get here, but assume the safe value just in case. */
      default:
        cp = 2;
        break;
    }
  }
  /* Unable to fetch from boot data, assume 2. */
  else
  {
    cp = 2;
  }

  /* Get the associated threshold for the CP and vote for that. */
  uint32_t vote = HWIO_INI2(CD_TH_CDm_CPn, cd->id, cp);
  vote |= BCM_VOTETABLE_COMMIT_BMSK | BCM_VOTETABLE_VOTE_VALID_BMSK;
  HWIO_OUTI2(VOTETABLE_DRVr_NDd, HLOS_DRV_ID, cd->bcm_idx, vote);
}

/**
*/
static void bcm_cx_init_proxy
(
  icb_bcm_cd_t *cd
)
{
  int8_t cp = 0;
  uint32_t vote;

  /* Search BCM TCS for the highest matching CP. */
  for(uint32_t i = 1; i < 16; i++)
  {
    int8_t ol = HWIO_INFI2(TCS_CFG_VOTE_DATA_CDm_CPn, cd->id, i, CMD0_DATA);
    if( ol <= cx_op_level && ol!=0x0 )
    {
      cp = i;
    }
    else
    {
      break;
    }
  }

  vote = HWIO_INI2(CD_TH_CDm_CPn, cd->id, cp);
  vote |= BCM_VOTETABLE_COMMIT_BMSK | BCM_VOTETABLE_VOTE_VALID_BMSK;
  HWIO_OUTI2(VOTETABLE_DRVr_NDd, HLOS_DRV_ID, cd->bcm_idx, vote);
}

/**
*/
static void bcm_acv_init_proxy
(
  icb_bcm_cd_t *cd
)
{
  uint32_t vote = BCM_VOTETABLE_COMMIT_BMSK | BCM_VOTETABLE_VOTE_VALID_BMSK | HLOS_ACV;
  HWIO_OUTI2(VOTETABLE_DRVr_NDd, HLOS_DRV_ID, cd->bcm_idx, vote);
}

/**
*/
static void set_bcm_proxy_votes( void )
{
  /* Force one-shot SW override for IPA/MMNOC clocks and
   * restore the SW override start address afterwards. */
  HWIO_OUTI(SW_CP_CDm, 4, 0x0);
  HWIO_OUTI(SW_CP_CDm, 7, 0x0);

  /* Wait for the sequencers to finish, then fix their start addresses. */
  while( 0 == HWIO_INFI(STATUS_BE_SEQ_CDm, 4, SEQ_STATE) );
  while( 0 == HWIO_INFI(STATUS_BE_SEQ_CDm, 7, SEQ_STATE) );
  HWIO_OUTI(SEQ_CFG_SW_START_ADDR_CDm, 4, HWIO_INI(SEQ_CFG_SW_START_ADDR_CDm, 3));
  HWIO_OUTI(SEQ_CFG_SW_START_ADDR_CDm, 7, HWIO_INI(SEQ_CFG_HW_START_ADDR_CDm, 7));

  /* Get the current Cx ARC operating level. */
  cx_op_level = arc_rm_op_level( 0, ARC_OL_CURRENT );
  CORE_VERIFY(cx_op_level >= 0);

  /* Initialize proxy votes. */
  for(uint32_t idx = 0; idx < target_data.num_cds; idx++)
  {
    icb_bcm_cd_t *cd = &target_data.cds[idx];
    if(cd->proxy_fcn)
    {
      /* Fetch the BCM index, since we'll need it in the proxy vote. */
      uint32_t bcm_addr = cmd_db_query_addr( cd->bcm_name );
      CORE_VERIFY(bcm_addr != 0);
      cd->bcm_idx = (bcm_addr & BCM_OFFSET_MASK)/4;

      cd->proxy_fcn(cd);
    }
  }
}


/*============================================================================
                          MMNOC TBU WORKAROUND
============================================================================*/
/**
*/
#if MMNOC_WA_ENABLE

static void init_mmnoc_workaround( void )
{
  /* Enable SEQ_GEN1 INT for MMNOC. */
  HWIO_OUT(INTERRUPT_SEQ_GENERIC_INT_1_CLEAR, 1 << MMNOC_VCD);
  HWIO_OUTM(INTERRUPT_SEQ_GENERIC_INT_1_EN,   1 << MMNOC_VCD, 1 << MMNOC_VCD);

  /* Disable MMNOC clock branches temporarily. */
  HWIO_OUTF(GCC_MMNOC_TBU_SF_CBCR,  CLK_ENABLE, 0);
  HWIO_OUTF(GCC_MMNOC_TBU_HF0_CBCR, CLK_ENABLE, 0);
  HWIO_OUTF(GCC_MMNOC_TBU_HF1_CBCR, CLK_ENABLE, 0);
  HWIO_OUTF(GCC_MMNOC_HF_QX_CBCR,   CLK_ENABLE, 0);
  HWIO_OUTF(GCC_MMNOC_SF_QX_CBCR,   CLK_ENABLE, 0);

  /* Enable SW override for GDS HW CTRL */
  HWIO_OUTF(GCC_MMNOC_GDS_HW_CTRL, SW_OVERRIDE, 1);

  /* Make sure ONLY we're doing this. */
  HWIO_OUTF(GCC_GDS_HW_CTRL_SW_OVRD, DISABLE, 1);

  /* Re-enable MMNOC clock branches. */
  HWIO_OUTF(GCC_MMNOC_TBU_SF_CBCR,  CLK_ENABLE, 1);
  HWIO_OUTF(GCC_MMNOC_TBU_HF0_CBCR, CLK_ENABLE, 1);
  HWIO_OUTF(GCC_MMNOC_TBU_HF1_CBCR, CLK_ENABLE, 1);
  HWIO_OUTF(GCC_MMNOC_HF_QX_CBCR,   CLK_ENABLE, 1);
  HWIO_OUTF(GCC_MMNOC_SF_QX_CBCR,   CLK_ENABLE, 1);
}

/**
 * MMNOC power down sequence for HW CR QCTDD04214452
*/
static bool mmnoc_workaround_power_down( void )
{
  /* Halt DVM network and wait for halt ACK. */
  HWIO_OUTF(GCC_MMNOC_HALT_REQ_GDS_HW_CTRL, DVM_HALT_REQ, 7);
  wait_state = BCM_WAIT_DVM_HALT;
  while( 0 != HWIO_INF(GCC_MMNOC_GDS_HW_CTRL_STATUS, DVM_HALT1_PWR_DOWN_ACK_STATUS))
  {
    if (abort_service_now())
    {
      return false;
    }
  }

  /* Halt TBUs and wait for halt ACK. */
  HWIO_OUTF(GCC_MMNOC_HALT_REQ_GDS_HW_CTRL, MMU_TBU_HALT_REQ, 7);
  wait_state = BCM_WAIT_TBU_HALT;
  while( 0 != HWIO_INF(GCC_MMNOC_GDS_HW_CTRL_STATUS, HALT1_PWR_DOWN_ACK_STATUS))
  {
    if (abort_service_now())
    {
      return false;
    }
  }

  /* Halt NoC and wait for halt ACK. */
  HWIO_OUTF(GCC_MMNOC_HALT_REQ_GDS_HW_CTRL, NOC_HALT_REQ, 1);
  wait_state = BCM_WAIT_NOC_HALT;
  while( 0 != HWIO_INF(GCC_MMNOC_GDS_HW_CTRL_STATUS, HALT2_PWR_DOWN_ACK_STATUS))
  {
    if (abort_service_now())
    {
      return false;
    }
  }
  wait_state = BCM_IDLE;

  /* Power collapse GDS with retention. */
  HWIO_OUTF(GCC_MMNOC_GDSCR, RETAIN_FF_ENABLE, 1);
  HWIO_OUTF(GCC_MMNOC_GDSCR, SW_COLLAPSE, 1);

  return true;
}

/**
 * MMNOC power up sequence for HW CR QCTDD04214452
*/
static bool mmnoc_workaround_power_up( void )
{
  /* Power up GDS, and wait for it to turn back on. */
  HWIO_OUTF(GCC_MMNOC_GDSCR, SW_COLLAPSE, 0);
  wait_state = BCM_WAIT_MMNOC_POWER_UP;
  while( 0 == HWIO_INF(GCC_MMNOC_GDS_HW_CTRL, PWR_ON_STATUS) )
  {
    if (abort_service_now())
    {
      return false;
    }
  }

  /* Unhalt DVM/TBU/NoC connections wait for ACK. */
  HWIO_OUTF(GCC_MMNOC_HALT_REQ_GDS_HW_CTRL, DVM_HALT_REQ, 0);
  HWIO_OUTF(GCC_MMNOC_HALT_REQ_GDS_HW_CTRL, MMU_TBU_HALT_REQ, 0);
  HWIO_OUTF(GCC_MMNOC_HALT_REQ_GDS_HW_CTRL, NOC_HALT_REQ, 0);
  wait_state = BCM_WAIT_DVM_UNHALT;
  while( 7 != HWIO_INF(GCC_MMNOC_GDS_HW_CTRL_STATUS, DVM_HALT1_PWR_UP_ACK_STATUS))
  {
    if (abort_service_now())
    {
      return false;
    }
  }
  wait_state = BCM_WAIT_TBU_UNHALT;
  while( 7 != HWIO_INF(GCC_MMNOC_GDS_HW_CTRL_STATUS, HALT1_PWR_UP_ACK_STATUS))
  {
    if (abort_service_now())
    {
      return false;
    }
  }
  wait_state = BCM_WAIT_NOC_UNHALT;
  while( 1 != HWIO_INF(GCC_MMNOC_GDS_HW_CTRL_STATUS, HALT2_PWR_UP_ACK_STATUS))
  {
    if (abort_service_now())
    {
      return false;
    }
  }
  wait_state = BCM_IDLE;

  return true;
}

static bool mmnoc_workaround( void )
{
  bool ret = true;

  /* Check the direction we're going. */
  if(0 != HWIO_INFI(STATUS_BE_CDm, MMNOC_VCD, WRITTEN_CP))
  {
    ret = mmnoc_workaround_power_up();
  }
  /* Going to collapse. */
  else
  {
    ret = mmnoc_workaround_power_down();
  }

  /* Clear interrupt. */
  HWIO_OUT(INTERRUPT_SEQ_GENERIC_INT_1_CLEAR, 1 << MMNOC_VCD);

  /* Unblock the sequencer. */
  HWIO_OUTI(SW_OVERRIDE_SEQ_UNBLOCK_ONCE_B_CDm, MMNOC_VCD, 0x1);

  return ret;
}
#endif
/**
*/
static void init_bcm_workarounds( void )
{
  /* Create our handle to enable CNOC. */
  #if MMNOC_WA_ENABLE

  CORE_VERIFY(cnoc_handle = icb_get_handle(ICBID_MASTER_AOSS,ICBID_SLAVE_DCC_CFG));
  init_mmnoc_workaround();
  #endif
}

/**
*/
static void post_init_bcm_workarounds( void )
{
  /* Since we're forcing MMNOC off, we won't get an interrupt as BCM thinks
   * it's off already. Just do power down. */
  #if MMNOC_WA_ENABLE
  mmnoc_workaround_power_down();
  #endif
}

/*============================================================================
                      SW Sequence Workaround
============================================================================*/
void handle_vcd_done( void )
{
  rex_enter_crit_sect(&match_lock);

  /* Find out which VCDs are done. */
  uint32_t int_mask = HWIO_IN(INTERRUPT_CP_CHANGE_DONE_INT_STATUS);

  /* Go through our match VCDs and see if any are up. */
  for( uint32_t cd = 0;
       cd < icb_info->num_match_vcds;
       cd++ )
  {
    icb_match_vcd_t *match_vcd = &icb_info->match_vcds[cd];

    /* If this VCD is in the interrupt mask, service it. */
    if( int_mask & (1 << match_vcd->vcd) )
    {
      uint32_t be_status = HWIO_INI(STATUS_BE_CDm, match_vcd->vcd);
      uint32_t sw_cp_snap = (be_status & HWIO_FMSK(STATUS_BE_CDm, SW_CP_SNAP))
                              >> HWIO_SHFT(STATUS_BE_CDm, SW_CP_SNAP);
      uint32_t curr_cp = (be_status & HWIO_FMSK(STATUS_BE_CDm, CURR_CP))
                          >> HWIO_SHFT(STATUS_BE_CDm, CURR_CP);

      /* We're potentially done if we hit the following conditions:
       * Our SW_CP_SNAP matches our last written SW_CP AND
       *  CURR_CP is at least SW_CP or the domain is off. */
      if( (sw_cp_snap == match_vcd->pending_cp) &&
          ((curr_cp >= match_vcd->pending_cp) || (curr_cp == 0)) )
      {
        /* If pending matches desired, we're done.
         * Clear busy and interrupt enable for this domain. */
        if( match_vcd->pending_cp == match_vcd->desired_cp )
        {
          match_vcd->busy = false;
          HWIO_OUTM(INTERRUPT_CP_CHANGE_DONE_INT_EN, 1 << match_vcd->vcd, 0);
        }
        /* We must have updated desired while waiting for pending.
         * Update pending, but do not clear busy. */
        else
        {
          match_vcd->pending_cp = match_vcd->desired_cp;
        }
      }

      /* Clear the interrupt before we potentially issue a new request. */
      HWIO_OUT(INTERRUPT_CP_CHANGE_DONE_INT_CLEAR, 1 << match_vcd->vcd);

      /* If we're still busy at this point, it means we need to reissue. */
      if( match_vcd->busy )
      {
        HWIO_OUTFI(SW_CP_CDm, match_vcd->vcd, SW_CP, match_vcd->pending_cp);
      }
    }
  }

  rex_leave_crit_sect(&match_lock);
}

/*============================================================================
                      EXTERNAL FUNCTION DECLARATIONS
============================================================================*/
/**
@brief Service any bcm workarounds.

@param[in] irq - the interrupt to service
*/
bool icb_target_service_ist
(
  int irq
)
{
  bool ret = true;

  switch(irq)
  {
    case BCM_IRQ_1:
      /* Check for SW_CP workaround. */
      handle_vcd_done();
      break;

    case BCM_IRQ_4:
      /* Check for MMNOC workaround. */
    #if MMNOC_WA_ENABLE
      if(0 != (HWIO_IN(INTERRUPT_SEQ_GENERIC_INT_1_STATUS) & (1 << MMNOC_VCD)))
      {
        ret = mmnoc_workaround();
      }
    #endif
      break;

    /* We only service IRQ 1/4 here.
     * If we get anything else, crash. */
    default:
      CORE_VERIFY(false);
      break;
  }

  return ret;
}

/**
@copydoc icb_target_init
*/
void icb_target_init
(
  icb_info_t *info
)
{
  icb_info = info;
  set_bcm_proxy_votes();
  init_bcm_workarounds();
}

/**
@copydoc icb_target_post_init
*/
void icb_target_post_init( void )
{
  post_init_bcm_workarounds();
}
