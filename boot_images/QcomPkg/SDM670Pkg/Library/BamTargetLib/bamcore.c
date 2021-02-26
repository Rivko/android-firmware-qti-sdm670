/**
  @file bamcore.c
  @brief
  This file contains definitions of constants, data structures, and
  interfaces that provide access to the BAM Core revision 4 (B-family).

*/
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/07/15   ss      Added istri support.
02/11/14   RL      64-bit support
08/16/13   RL      LPAE support, pipe attrib registers, pipe hresp/error interrupt support
05/06/13   SS      Added Error Logging
04/24/13   RL      Add apis to query if bam or a pipe is enabled
03/15/13   SS      BUS error ctrl reg to bam core dump
03/12/13   SS      Remove IRQ SRCS UNMASKed, Trust and MASTER CRTL reg from bam core dump
02/13/13   SS      Added testbus dump to bam core dump
02/05/13   SS      Added Multi VMID support for pipe access control. 
01/15/13   SS      Added Multi VMID support for BAM ctrl group
12/06/12   RL      Updated support BAM revisions to Gandalf
12/04/12   AN      Added function to read error bus control register
10/30/12   SS      Made changes for B family binary compatibility
09/20/12   SS      Added Desc & Data FIFO info to core dump
06/21/12   SS      Added Core dump support for peer bam pipe
05/16/12   SS      Added Core Register Dump support.
04/13/12   NK      Added bamcore_deinit
02/29/12   MK      Added bamcore_getmaxtransfersize and
                   bamcore_pipe_setlockgroup APIs to support Core Rev 4+
12/02/11   an      branch from core 3

===============================================================================
                   Copyright (c) 2011-2016 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/


#include "bamhwio.h"
#include "DALStdDef.h"
#include "bam.h"
#include "bamtgtcfg.h"
#include "bamcore.h"
#include "bamosal.h" // For debug printfs

//Core specific values
/*
   Revisions:
               BAM    Lite    NDP
            +------+-------+-------+
   Aragorn     0x6    0x13   0x20
   Elan        0x7    0x14   0x21
   Frodo       0x8    0x15   0x22
   Gandalf     0x9    0x16   0x23
   Elessar     0xA    0x17   0x24
   Istari      0xB    0x19   0x27 // Nazgul
*/
#define BAM_TESTBUS_MAX_INDEX             0x30
#define BAM_MIN_VERSION                   0x6 // Aragorn
#define BAM_MAX_VERSION                   0xB // Nazgul

#define BAMLITE_MIN_VERSION               0x13 // Aragorn
#define BAMLITE_MAX_VERSION               0x19 // Nazgul

#define BAMNDP_MIN_VERSION                0x20 // Aragorn
#define BAMNDP_MAX_VERSION                0x27 // Nazgul

#define BAM_PIPE_ATTRB_MIN_VERSION        0x9
#define BAMLITE_PIPE_ATTRB_MIN_VERSION    0x16
#define BAMNDP_PIPE_ATTRB_MIN_VERSION     0x23

#define BAM_REG_SPACE_SIZE 0x4000
#define GET_BAM_PHY_BASE(reg) ((reg) & ~(BAM_REG_SPACE_SIZE - 1))

#define BAM_DESC_MAX_SIZE              (32*1024 - 1)

#define BAM_CORE_DUMP_MINOR_REV 0x02
#define BAM_CORE_DUMP_MAJOR_REV 0x0100
#define BAM_CORE_DUMP_VER       0x010000
#define BAM_CORE_DUMP_REV ((BAM_CORE_DUMP_VER)| \
   (BAM_CORE_DUMP_MAJOR_REV)|(BAM_CORE_DUMP_MINOR_REV))

#define IS_SUPPORTED_BAM(rev) \
 (((rev >= BAM_MIN_VERSION && rev <=BAM_MAX_VERSION) || \
  (rev >= BAMLITE_MIN_VERSION && rev <=BAMLITE_MAX_VERSION)|| \
  (rev >= BAMNDP_MIN_VERSION && rev <=BAMNDP_MAX_VERSION))?1:0)

#define IS_BAM_SG_SUPPORTED(rev) \
 (((rev >= BAM_MIN_VERSION) || (rev >= BAMLITE_MIN_VERSION)|| \
  (rev >= BAMNDP_MIN_VERSION))?1:0)

//pipe attribute registers were introduced after gandalf
#define IS_PIPE_ATTRIBS_SUPPORTED(rev) \
    (( (rev >= BAM_MIN_VERSION && rev <= BAM_MAX_VERSION) && rev >= BAM_PIPE_ATTRB_MIN_VERSION ) || \
    (  (rev >= BAMLITE_MIN_VERSION && rev <= BAMLITE_MAX_VERSION) && rev >= BAMLITE_PIPE_ATTRB_MIN_VERSION ) || \
    (  (rev >= BAMNDP_MIN_VERSION && rev <= BAMNDP_MAX_VERSION) && rev >= BAMNDP_PIPE_ATTRB_MIN_VERSION ))


/**
 * Bam Register structure for core dump
 */
typedef struct
{
    uint32 bam_dumptype;                         /**< -- Dump type, BAM or pipe. */
    uint64 bam_base_pa;                          /**< -- BAM physical base address */
    uint32 bam_ctrl;                             /**< -- BAM control regsiter value */
    uint32 bam_revision;                         /**< -- BAM revision regsiter value */
    uint32 bam_desc_cnt_trshld;                  /**< -- BAM descriptor count threshold regsiter value*/
    uint32 bam_irq_stts;                         /**< -- BAM interrupt status regsiter value */
    uint32 bam_irq_clr;                          /**< -- BAM interrupt clear  regsiter value*/
    uint32 bam_irq_en;                           /**< -- BAM interrupt enable regsiter value*/
    uint32 bam_ahb_master_err_ctrls;             /**< -- BAM AHB data error cntl regsiter value*/
    uint32 bam_ahb_master_err_addr;              /**< -- BAM AHB data error add regsiter value*/
    uint32 bam_ahb_master_err_data;              /**< -- BAM AHB data error regsiter value */
    uint32 bam_num_pipes;                        /**< -- BAM pipe count regsiter value*/
    uint32 bam_timer;                            /**< -- BAM timer regsiter value*/
    uint32 bam_timer_ctrl;                       /**< -- BAM timer control regsiter value*/
    uint32 bam_cnfg_bits;                        /**< -- BAM config regsiter */
    uint32 bam_sw_version;                       /**< -- BAM software version regsiter */
    uint32 bam_test_bus_sel;                     /**< -- BAM test bus selector regsiter */
    uint32 bam_test_bus_reg[BAM_TESTBUS_MAX_INDEX];/**< -- BAM test bus regsiter */
    uint32 bam_irq_srcs_een[BAM_EE_MAX];         /**< -- BAM interrupt source enable regsiter */
    uint32 bam_irq_srcs_msk_een[BAM_EE_MAX];     /**< -- BAM interrupt source enable mask regsiter */
    uint32 bam_irq_srcs_unmasked_een[BAM_EE_MAX];/**< -- BAM interrupt source unmask enb regsiter */
}bamdebug_registers;

/**
 *  Pipe register stucture for core dump
 */
 typedef struct  bamdebug_p_register bamdebug_p_register;

 struct  bamdebug_p_register
{
    uint32 bam_p_dumptype;
    uint32 bam_p_index;
    uint32 bam_p_ctrln;
    uint32 bam_p_rstn;
    uint32 bam_p_haltn;
    uint32 bam_p_irq_sttsn;
    uint32 bam_p_irq_clrn;
    uint32 bam_p_irq_enn;
    uint32 bam_p_timern;
    uint32 bam_p_timer_ctrln;
    uint32 bam_p_prdcr_sdbndn;
    uint32 bam_p_cnsmr_sdbndn;
    uint32 bam_p_evnt_dest_addrn;
    uint32 bam_p_evnt_regn;
    uint32 bam_p_sw_ofstsn;
    uint32 bam_p_data_fifo_addrn;
    uint32 bam_p_desc_fifo_addrn;
    uint32 bam_p_evnt_gen_trshldn;
    uint32 bam_p_fifo_sizesn;
    uint32 bam_p_retr_cntxtn;
    uint32 bam_p_si_cntxtn;
    uint32 bam_p_df_cntxtn;
    uint32 bam_p_au_psm_cntxt_1n;
    uint32 bam_p_psm_cntxt_2n;
    uint32 bam_p_psm_cntxt_3n;
    uint32 bam_p_psm_cntxt_4n;
    uint32 bam_p_psm_cntxt_5n;
    uint32 *bam_p_desc_fifo;
    uint32  bam_p_desc_fifo_size;
    uint32 *bam_p_data_fifo;
    uint32 bam_p_data_fifo_size;
    bamdebug_registers *bam_ptr;
    bamdebug_p_register *bam_p_peer_ptr;  
} ;


/**
 *  BAM dump header stucture for core dump revision, pipe and peep pipe mask.
 *
 *  Header will have revision no., Pipe mask, peerpipe mask & size of header, bam , pipe reg dump.
 *  Revision is determine by following:
 *  LSB 8 bits: Minor rev. Changes as per SW changes
 *  bits 8-15:  Major rev.
 *  bits 16-23: Intial version.
 *  bits 24-31: reserved.
 *
 */
typedef struct
{
    uint32 revision;
    uint32 pipemask;
    uint32 peerpipemask;
    uint32 dump_header_size;
    uint32 bam_buf_size;
    uint32 pipe_buf_size;
}bam_dumpheader;

/* ============================================================================
**  Function : bamcore_pipe_tzprotect
** ============================================================================
*/
/**
  External function provided by TZ to notify the VMID index assignment of
  the pipes.

  This external API is installed by TZ to notify it, when the pipes' VMID index
  gets programmed.

  @return
  0 - Success; Error otherwise

  @dependencies
  BAM being configured must be secure for this API to be called by the BAM
  driver.

*/
uint32 (*bamcore_pipe_tzprotect)(bam_vaddr base, uint32 pipenum, uint32 vmid, uint32 vmid_index) = 0;

/* ============================================================================
**  Function : bamcore_getpiperegdump
** ============================================================================
*/
/**
  Gets the  BAM Pipe Core register value.

  Gets the BAM Pipe Core register value.

  @param[in]  base         BAM virtual base address
  @param[in]  base_pa      Bam base PA.
  @param[in]  bufptr       Buffer to get Bam Pipe register dump.
  @param[in]  pipenum      Pipe Number to be dumped.
  @param[in]  buf_base     Pointer start of bam core register dump.

  @return
  None
 */
static void  bamcore_getpiperegs(bam_vaddr base, uint64 base_pa, uint32 *bufptr, uint32 pipenum, bam_vaddr buf_base);

static void bamdev_write_reg(bam_vaddr base,uint32 offset,uint32 val)
{
    bam_vaddr addr = base + offset;
    (*((volatile uint32 *)(addr))) = val;
    bam_osal_debug(MSG_LOW,"data.set 0x%x %%LONG 0x%x\n",(uint32)offset,(uint32)val);
}

static void bamdev_write_reg_field(bam_vaddr base,uint32 offset,uint32 mask, uint32 shift, uint32 field_val)
{
    bam_vaddr addr = base + offset;
    uint32 regval = (*((volatile uint32 *)(addr)));

    regval &= ~mask;
    regval = regval | (field_val << shift);
    (*((volatile uint32 *)(addr))) = regval;
    bam_osal_debug(MSG_LOW,"data.set 0x%x %%LONG 0x%x\n",(uint32)offset,(uint32)regval);
}

static uint32 bamdev_read_reg(bam_vaddr base,uint32 offset)
{
    bam_vaddr addr = base + offset;

    return (*((volatile uint32 *)(addr)));
}

static uint32 bamdev_read_reg_field(bam_vaddr base,uint32 offset,uint32 mask, uint32 shift)
{
    bam_vaddr addr = base + offset;
    uint32 regval = (*((volatile uint32 *)(addr)));

    regval &= mask;
    regval >>= shift;
    return regval;
}

/* ============================================================================
**  Function : bamcore_lpae_supported
** ============================================================================
*/
/**
  Check whether the given bam supports LPAE

  @return
     1   -  LPAE supported
     0   -  LPAE not supported
*/
uint32 bamcore_lpae_supported(bam_vaddr base )
{
    //BAM_NUM_PIPES_BAM_DATA_ADDR_BUS_WIDTH == 1 means, 36 bit addresses.
    return (bamdev_read_reg_field(base,BAM_NUM_PIPES, BAM_MNS(BAM_NUM_PIPES,BAM_DATA_ADDR_BUS_WIDTH)) == 1);
}


/* ============================================================================
**  Function : bamcore_init
** ============================================================================
*/
/**
  Initialize a BAM device.

  This function initializes a BAM device.

  @param[in]  base            BAM virtual base address
  @param[in]  irq             RAW IRQ Value (BAM_IRQ_INVALID if no IRQ)
  @param[in]  irq_mask        IRQ Mask to use
  @param[in]  sum_thresh      summing threshold (global for all pipes)
  @param[in]  tgt_cfg         BAM target config

  @return
  None.

  @dependencies
  BAM must be clocked.

  @sa
  Other_Relevant_APIS_or_Delete.
*/
void bamcore_init(bam_vaddr base, uint32 irq, uint32 irq_mask, uint32 sum_thresh, const bam_target_config_type* tgt_cfg)
{
    //Reset the BAM
    bamdev_write_reg_field(base, BAM_CTRL, BAM_MNS(BAM_CTRL,BAM_SW_RST), 1);
    /* No delay needed */
    bamdev_write_reg_field(base, BAM_CTRL, BAM_MNS(BAM_CTRL,BAM_SW_RST), 0);

    //Set clock gating config for the bam
    bamdev_write_reg_field(base,BAM_CTRL, BAM_MNS(BAM_CTRL,LOCAL_CLK_GATING),((tgt_cfg->options & BAM_TGT_CFG_DISABLE_CLK_GATING)?0:1));
    //Enable the bam
    bamdev_write_reg_field(base,BAM_CTRL, BAM_MNS(BAM_CTRL,BAM_EN),0x1);

    // Descriptor summing threshold
    bamdev_write_reg(base, BAM_DESC_CNT_TRSHLD, sum_thresh);

    //Config Bits for BAM
    bamdev_write_reg(base, BAM_CNFG_BITS, tgt_cfg->cfg_bits);

    if(irq != BAM_IRQ_INVALID)
    {
        //Enable BAM Interrupt for global interrupts.
        //Pipes interrupts are disabled until the individual pipes are initialized
        uint32 ee = tgt_cfg->ee == BAM_EE_TRUST ? 0 : tgt_cfg->ee; 
        bamcore_setirqmode(base,ee,1,irq_mask);
    }
    return;
}

/* ============================================================================
**  Function : bamcore_deinit
** ============================================================================
*/
/**
  Deinitialize a BAM device.

  This function deinitializes a BAM device.

  @param[in]  base            BAM virtual base address

  @return
  None.

  @dependencies
  BAM must be clocked.

*/
void bamcore_deinit(bam_vaddr base)
{
    //Reset the BAM
    bamdev_write_reg_field(base, BAM_CTRL, BAM_MNS(BAM_CTRL,BAM_SW_RST), 1);

    return;
}
/* ============================================================================
**  Function : bamcore_set_ee
** ============================================================================
*/
/**
  Sets the Execution Environment for the BAM Control registers.

  Allocates the BAM control to a particular execution environment. This API is
  meant to be called from trustzone only.

  @param[in]  base            BAM virtual base address
  @param[in]  ee              BAM execution environment index
  @param[in]  reset_enable    Flag to enable reset functionality

  @return
  None.

  @dependencies
  Dependency_description_or_None.

  @sideeffects
  Sideeffect_description_or_Delete.

  @sa
  Other_Relevant_APIS_or_Delete.
*/
static void bamcore_set_ee(bam_vaddr base, uint32 ee, uint32 reset_enable)
{
    bamdev_write_reg_field(base, BAM_TRUST_REG, BAM_MNS(BAM_TRUST_REG,BAM_EE), ee);
    bamdev_write_reg_field(base, BAM_TRUST_REG, BAM_MNS(BAM_TRUST_REG,BAM_RST_BLOCK), reset_enable);
}

/* ============================================================================
**  Function : bamcore_secure_init
** ============================================================================
*/
/**
  Initializes the BAM device control group access to multi EEs.
  Initializes the BAM device control group access to multi EEs.
 
  @param[in]  base         BAM virtual base address
  @param[in]  tgtcfg       BAM target config handle
 
  @return
  None.
  @dependencies
  Dependency_description_or_None.
*/
bam_status_type bamcore_secure_init(bam_vaddr base, const bam_target_config_type *tgtcfg)
{
    if (( 0 == base ) || (NULL == tgtcfg))
        return BAM_FAILED;

    // Assign top-level interrupt to EE 
    // Disable global reset 
    bamcore_set_ee(base, tgtcfg->sec_config->bam_irq_ee, 0);
     
    // Configure bam to consider EE of the pipe when locking
    if ( (tgtcfg->options & BAM_TGT_CFG_LCK_PGRPONLY) == 0 )
    {
        bamdev_write_reg_field(base, BAM_TRUST_REG, BAM_MNS(BAM_TRUST_REG,LOCK_EE_CTRL), 1);
    }
    return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bamcore_pipe_secure_init
** ============================================================================
*/
/**
  Initializes the security configuration of the BAM Pipe.

  Initializes the security configuration of the BAM Pipe.

  @param[in]  base         BAM virtual base address
  @param[in]  tgt_cfg      BAM target config handle.
  
  @return
  None.

  @dependencies
  Dependency_description_or_None.

*/
bam_status_type bamcore_pipe_secure_init(bam_vaddr base, const bam_target_config_type *tgt_cfg)
{
    bam_caps_type bam_caps;
    uint32 pipe, vmid_index, pipemask, pipemask_ac,ee;

    if(bamcore_check(base, &bam_caps,tgt_cfg->ee) != BAM_SUCCESS)
    {
        bam_osal_debug(MSG_ERR, "bamcore_pipe_secure_init:BAM core check failed \n");
        //BAM hasn't been enabled yet
        return BAM_FAILED;
    }

    for(ee = 0; ee <bam_caps.num_ees; ee++)
    {
        pipemask = tgt_cfg->sec_config->ee[ee].pipe_mask;
        pipemask_ac = tgt_cfg->sec_config->ee[ee].pipe_mask_ac; 
        for (pipe = 0; pipe < bam_caps.num_pipes && pipemask != 0; pipe++, pipemask >>= 1, pipemask_ac >>=1)
        {
            if ((1 & pipemask) != 0)
            {
                // Sets the Execution Environment for the pipe 
                bamdev_write_reg_field(base, BAM_P_TRUST_REGn(pipe), BAM_MNS(BAM_P_TRUST_REGn,BAM_P_EE),ee);
                if(0 != bam_caps.is_secured && NULL != bamcore_pipe_tzprotect)
                {
                    if ((1 & pipemask_ac) != 0)
                    {
                        // Transaction and access contol VMID is diffenet for this pipe.
                        vmid_index = tgt_cfg->sec_config->ee[ee].vmid_ac; 
                    }
                    else
                    {
                        // Transaction and access contol VMID is same for this pipe.
                        // Coverting the value to bit mask
                        vmid_index = (1 << tgt_cfg->sec_config->ee[ee].vmid);
                    }
                    // Configure Pipe transaction and access control vmid 
                    if(0 != bamcore_pipe_tzprotect(base, pipe, tgt_cfg->sec_config->ee[ee].vmid, vmid_index))
                    {
                        bam_osal_debug(MSG_ERR, "bamcore_pipe_secure_init:BAM Pipe VMID config failed pipe:%d, vmid:0X%x\n", pipe,tgt_cfg->sec_config->ee[ee].vmid );
                        return BAM_FAILED;
                    }
                }
            }
        }
    }
    return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bamcore_secure_set_sg
** ============================================================================
*/
/**
  Assigns supergroup ID to a given subset of pipes.

  Assigns supergroup ID to a given subset of pipes.

  @param[in]  base        BAM virtual base address
  @param[in]  tgt_cfg     BAM target config handle

  @return
  None.

*/
bam_status_type bamcore_secure_set_sg( bam_vaddr base, const bam_target_config_type *tgt_cfg)
{
   bam_caps_type bam_caps;
   uint32 pipe, sg, pipemask; 

   bamcore_check(base,&bam_caps,tgt_cfg->ee);
   if ((IS_BAM_SG_SUPPORTED(bam_caps.revision)) != 0)
   {
       for(sg = 0; sg < BAM_MAX_SGS ; sg++)
       {
           pipemask = tgt_cfg->sec_config->sg[sg].pipe_mask;
           for (pipe = 0; pipe < bam_caps.num_pipes && pipemask != 0; pipe++, pipemask >>= 1)
           {
               if ((1 & pipemask) != 0)
               {
                   bamdev_write_reg_field(base, BAM_P_TRUST_REGn(pipe), BAM_MNS(BAM_P_TRUST_REGn, BAM_P_SUP_GROUP),sg);
               }
           }
       }
   }
   return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bamcore_check
** ============================================================================
*/
/**
  Check the state of the BAM Core.

  Check if the BAM is enabled and if enabled query its capabilities.

  @param[in]      base        BAM virtual base address
  @param[in,out]  bam_caps    BAM capabilities struct
  @param[in]      ee          ee of the current enviornment

  @return
  Status of the BAM.

*/
bam_status_type bamcore_check(bam_vaddr base, bam_caps_type *bam_caps, uint32 ee)
{
    uint32 ver;

    if(bam_caps == NULL)
    {
        return BAM_FAILED;
    }

    ver = bamdev_read_reg_field(base, BAM_REVISION,BAM_MNS(BAM_REVISION,REVISION));

    if(!IS_SUPPORTED_BAM(ver))
    {
        bam_osal_debug(MSG_ERR, "bamcore_check:Invalid BAM Version version:0X%x, bam_base:0X%x\n", ver, base );
        return BAM_FAILED;
    }

    bam_caps->revision            = ver;
    bam_caps->num_pipes           = bamdev_read_reg_field(base, BAM_NUM_PIPES, BAM_MNS(BAM_NUM_PIPES,BAM_NUM_PIPES));
    bam_caps->data_addr_width     = bamdev_read_reg_field(base, BAM_NUM_PIPES, BAM_MNS(BAM_NUM_PIPES,BAM_DATA_ADDR_BUS_WIDTH));
    bam_caps->num_ees             = bamdev_read_reg_field(base, BAM_REVISION, BAM_MNS(BAM_REVISION,NUM_EES));
    bam_caps->is_secured          = bamdev_read_reg_field(base, BAM_REVISION, BAM_MNS(BAM_REVISION,SECURED));
    bam_caps->inactive_timer_base = bamdev_read_reg_field(base, BAM_REVISION, BAM_MNS(BAM_REVISION,INACTIV_TMR_BASE));
    bam_caps->has_inactive_timer  = bamdev_read_reg_field(base, BAM_REVISION, BAM_MNS(BAM_REVISION,INACTIV_TMRS_EXST));
    bam_caps->is_hi_freq_bam      = bamdev_read_reg_field(base, BAM_REVISION, BAM_MNS(BAM_REVISION,HIGH_FREQUENCY_BAM));
    bam_caps->no_bypass           = bamdev_read_reg_field(base, BAM_REVISION, BAM_MNS(BAM_REVISION,BAM_HAS_NO_BYPASS));
    bam_caps->cmd_desc_en         = bamdev_read_reg_field(base, BAM_REVISION, BAM_MNS(BAM_REVISION,CMD_DESC_EN      ));
    bam_caps->num_inactive_timers = bamdev_read_reg_field(base, BAM_REVISION, BAM_MNS(BAM_REVISION,NUM_INACTIV_TMRS ));
    bam_caps->desc_cache_depth    = bamdev_read_reg_field(base, BAM_REVISION, BAM_MNS(BAM_REVISION,DESC_CACHE_DEPTH ));
    bam_caps->use_vmidmt_mask     = bamdev_read_reg_field(base, BAM_REVISION, BAM_MNS(BAM_REVISION,USE_VMIDMT       ));
    bam_caps->axi_active_mask     = bamdev_read_reg_field(base, BAM_REVISION, BAM_MNS(BAM_REVISION,AXI_ACTIVE       ));
    bam_caps->ce_buf_size         = bamdev_read_reg_field(base, BAM_REVISION, BAM_MNS(BAM_REVISION,CE_BUFFER_SIZE   ));


    //Check if the bam is enabled or not
    if ( 0 == bamcore_bam_enabled(base,ee) )
    {
        bam_osal_debug(MSG_ERR, "bamcore_check:BAM not enable bam_base:0X%x\n",base);
        return BAM_FAILED;
    }


    return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bamcore_setirqmode
** ============================================================================
*/
/**
  Sets the IRQ mode for the BAM.

  Configures the BAM to operate in IRQ or Polling mode. Does not affect the
  mode in which a pipe in that BAM operates.

  @param[in]  base        BAM virtual base address
  @param[in]  ee          Execution Environment index
  @param[in]  irq_en      Irq Enable Flag (1 - Enable / 0 - Disable)
  @param[in]  irq_mask    Mask to indicate which IRQ bits to enable

  @return
  Status of the operation.

*/
bam_status_type bamcore_setirqmode(bam_vaddr base, uint32 ee, uint32 irq_en,uint32 irq_mask)
{
    // Enable/Disable BAM Interrupt for global interrupts.
    // Pipes interrupts are managed by the pipe APIs
    if(irq_en)
    {
        bamdev_write_reg_field(base, BAM_IRQ_SRCS_MSK_EEn(ee), BAM_MNS(BAM_IRQ_SRCS_MSK_EEn,BAM_IRQ_MSK), 1);
        bamdev_write_reg(base, BAM_IRQ_CLR, irq_mask); // clear interrupts being enabled to avoid phantom interrupts
        bamdev_write_reg(base, BAM_IRQ_EN, irq_mask);
    }
    else
    {
        bamdev_write_reg_field(base, BAM_IRQ_SRCS_MSK_EEn(ee), BAM_MNS(BAM_IRQ_SRCS_MSK_EEn,BAM_IRQ_MSK), 0);
    }
    return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bamcore_getirqmode
** ============================================================================
*/
/**
  Gets the IRQ mode for the BAM.

  Get the current IRQ mode of the BAM.

  @param[in]  base        BAM virtual base address

  @return
  irq mask

*/
uint32 bamcore_getirqmode(bam_vaddr base)
{
    return bamdev_read_reg(base, BAM_IRQ_EN);
}


/* ============================================================================
**  Function : bamcore_getmaxtransfersize
** ============================================================================
*/
/**
  Gets the maximum transfer size the core would support per descriptor.

  Gets the maximum transfer size the core would support per descriptor.

  @param  None

  @return
  returns the maximum transfer size in bytes (Theoretical max is 0xFFFF).

*/
uint32 bamcore_getmaxtransfersize(void)
{
    return BAM_DESC_MAX_SIZE;
}

/* ============================================================================
**  Function : bamcore_get_error_address
** ============================================================================
*/
/**
  Gets the address that caused an error.

  Gets the address that caused an error.

  @param[in]  base    BAM virtual base address

  @return
  returns the address that caused an error.

*/
uint64 bamcore_get_error_address(bam_vaddr base)
{
    uint64 eaddress = 0;
    eaddress =  (uint64)bamdev_read_reg(base, BAM_AHB_MASTER_ERR_ADDR);
    if (bamcore_lpae_supported(base))
    {
        eaddress |= ((uint64)bamdev_read_reg(base,BAM_AHB_MASTER_ERR_ADDR_MSB)<<32) & 0xF;
    }
    return eaddress;
}

/* ============================================================================
**  Function : bamcore_get_error_data
** ============================================================================
*/
/**
  Gets the data that caused an error.

  Gets the data that caused an error.

  @param[in]  base    BAM virtual base address
  @param[in]  ee      Execution Environment index

  @return
  returns the data that caused an error.

*/
uint32 bamcore_get_error_data(bam_vaddr base)
{
    return bamdev_read_reg(base, BAM_AHB_MASTER_ERR_DATA);
}

/* ============================================================================
**  Function : bamcore_get_error_bus_ctrls
** ============================================================================
*/
/**
  Gets the bus control information for an error.

  Gets the bus control information for an error.

  @param[in]  base    BAM virtual base address

  @return
  returns the bus control information for an error.

*/
uint32 bamcore_get_error_bus_ctrls(bam_vaddr base)
{
    return bamdev_read_reg(base, BAM_AHB_MASTER_ERR_CTRLS);
}

/* ============================================================================
**  Function : bamcore_get_irq_sources
** ============================================================================
*/
/**
  Gets the IRQ sources.

  Gets the IRQ sources.

  @param[in]  base    BAM virtual base address
  @param[in]  ee      Execution Environment index
  @param[in]  mask    Mask of the status bits the caller is interested in.

  @return
  Status bits read from the IRQ Sources register based on the input mask

*/
uint32 bamcore_get_irq_sources(bam_vaddr base, uint32  ee, uint32  mask)
{
    uint32  status = bamdev_read_reg(base, BAM_IRQ_SRCS_EEn(ee)) & mask; 
    return status;
}

/* ============================================================================
**  Function : bamcore_get_and_clear_irq_status
** ============================================================================
*/
/**
  Gets the IRQ status and clears it after reading it.

  Gets the IRQ status register value and clears the register after reading it.

  @param[in]  base    BAM virtual base address
  @param[in]  mask    Mask of the status bits the caller is interested in.

  @return
  Status bits read from the IRQ Status register based on the input mask

*/
uint32 bamcore_get_and_clear_irq_status(bam_vaddr base, uint32  mask)
{
    uint32  status = bamdev_read_reg(base, BAM_IRQ_STTS);

    if (status != 0)
    {
        bamdev_write_reg(base, BAM_IRQ_CLR, status);
    }

    return status & mask;
}


/* ============================================================================
**  Function : bamcore_pipe_init
** ============================================================================
*/
/**
  Initializes a bam pipe.

  Optional_detailed_description_goes_here.

  @param[in]      base        BAM virtual base address
  @param[in]      pipe_num    Index of the pipe
  @param[in,out]  pipe_cfg    Configuration to be used
  @param[in]      irq_en      IRQ mode flag
  @param[in]      ee          Execution Environment index

  @return
  Status of the operation.

*/
bam_status_type bamcore_pipe_init(bam_vaddr base, uint32 pipe_num,bam_pipe_config_type *pipe_cfg,uint32 irq_en, uint32 ee)
{

    uint32 lpae_supported = 0;
    // Reset the BAM pipe 
    bamdev_write_reg(base, BAM_P_RSTn(pipe_num), 1);
    // make sure the reset write goes through
    bam_osal_memorybarrier();

    bamdev_write_reg(base, BAM_P_RSTn(pipe_num), 0);

    lpae_supported = bamcore_lpae_supported(base);

    if(irq_en)
    {
        bamcore_pipe_setirqmode(base, pipe_num, ee, irq_en, pipe_cfg->options);
    }

    bamdev_write_reg_field(base, BAM_P_CTRLn(pipe_num), BAM_MNS(BAM_P_CTRLn,P_DIRECTION), pipe_cfg->dir);
    bamdev_write_reg_field(base, BAM_P_CTRLn(pipe_num), BAM_MNS(BAM_P_CTRLn,P_SYS_MODE), pipe_cfg->mode);
    bamdev_write_reg_field(base, BAM_P_CTRLn(pipe_num), BAM_MNS(BAM_P_CTRLn,P_SYS_STRM),(pipe_cfg->options & BAM_O_STREAMING)?1:0 );

    bamdev_write_reg(base, BAM_P_EVNT_GEN_TRSHLDn(pipe_num), pipe_cfg->evt_thresh);

    bamdev_write_reg(base, BAM_P_DESC_FIFO_ADDRn(pipe_num), ADDR_LPAE_LSB(pipe_cfg->desc_base_pa));

    if ( lpae_supported )
    {
        bamdev_write_reg(base, BAM_P_DESC_FIFO_ADDR_MSBn(pipe_num), ADDR_LPAE_MSB(pipe_cfg->desc_base_pa));
    }

    bamdev_write_reg_field(base, BAM_P_FIFO_SIZESn(pipe_num), BAM_MNS(BAM_P_FIFO_SIZESn,P_DESC_FIFO_SIZE), pipe_cfg->desc_size);

    if (pipe_cfg->mode == BAM_MODE_BAM2BAM) 
    {
        uint64 peer_dest_addr = pipe_cfg->peer_base_pa + BAM_P_EVNT_REGn(pipe_cfg->peer_pipe_num);

        bamdev_write_reg(base, BAM_P_DATA_FIFO_ADDRn(pipe_num),ADDR_LPAE_LSB(pipe_cfg->data_base_pa));

        if ( lpae_supported )
        {
            bamdev_write_reg(base, BAM_P_DATA_FIFO_ADDR_MSBn(pipe_num), ADDR_LPAE_MSB(pipe_cfg->data_base_pa));
        } 

        bamdev_write_reg_field(base, BAM_P_FIFO_SIZESn(pipe_num),
                               BAM_MNS(BAM_P_FIFO_SIZESn, P_DATA_FIFO_SIZE),pipe_cfg->data_size);

        bamdev_write_reg(base, BAM_P_EVNT_DEST_ADDRn(pipe_num), ADDR_LPAE_LSB(peer_dest_addr));

        if ( lpae_supported )
        {
            bamdev_write_reg(base, BAM_P_EVNT_DEST_ADDR_MSBn(pipe_num), ADDR_LPAE_MSB(peer_dest_addr));
        }
    }

    bamcore_pipe_setlockgroup(base,pipe_num,pipe_cfg->lock_group);

    // Pipe Enable - at last 
    bamdev_write_reg_field(base, BAM_P_CTRLn(pipe_num), BAM_MNS(BAM_P_CTRLn,P_EN), 1);

    // Verify that the pipe was enabled successfully
    if(0 == bamdev_read_reg_field(base,BAM_P_CTRLn(pipe_num), BAM_MNS(BAM_P_CTRLn,P_EN)))
    {
        bam_osal_debug(MSG_ERR, "bamcore_pipe_init:BAM Pipe successfully not enable bam_base:0X%x\n", base);
        return BAM_FAILED;
    }

    return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bamcore_pipe_deinit
** ============================================================================
*/
/**
  De-initializes a BAM pipe .

  Disables the BAM pipe from further use.

  @param[in]  base        BAM virtual base address
  @param[in]  pipe_num    Index of the pipe
  @param[in]  irq_dis     IRQ disable flag (needed if IRQ was enabled)
  @param[in]  ee          Execution Environment index

  @return
  Status of the operation.

*/
bam_status_type bamcore_pipe_deinit(bam_vaddr base, uint32 pipe_num,uint32 irq_dis, uint32 ee)
{
    if(irq_dis)
    {
        bamdev_write_reg(base, BAM_P_IRQ_ENn(pipe_num), 0);

        // Disable the Pipe Interrupt at the BAM level
        bamdev_write_reg_field(base, BAM_IRQ_SRCS_MSK_EEn(ee), (1 << pipe_num), pipe_num, 0);
    }

    // Pipe Disable - at last
    bamdev_write_reg_field(base, BAM_P_CTRLn(pipe_num), BAM_MNS(BAM_P_CTRLn,P_EN), 0);
    return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bamcore_pipe_setlockgroup
** ============================================================================
*/
/**
  Sets the pipe lock group for a pipe.

  Sets the pipe lock group for a pipe.

  @param[in]  base          BAM virtual base address
  @param[in]  pipe_num      Index of the pipe
  @param[in]  lock_group    Group # to which the pipe belongs to

  @return
  Status of the operation.

  @note
  This API is supported in Cores v0x6 (BAM), 0x13(BAMLite), 0x20(NDPBAM) and
  above.

*/
bam_status_type bamcore_pipe_setlockgroup(bam_vaddr base, uint32 pipe_num, uint32 lock_group)
{
    bamdev_write_reg_field(base,BAM_P_CTRLn(pipe_num),BAM_MNS(BAM_P_CTRLn,P_LOCK_GROUP),lock_group);
    return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bamcore_pipe_setirqmode
** ============================================================================
*/
/**
  Sets the IRQ mode for the BAM pipe.

  Configures the BAM pipe to operate in IRQ or Polling mode. Does not affect the
  mode in which the BAM operates at the top level.

  @param[in]  base             BAM virtual base address
  @param[in]  pipe_num         Index of the pipe
  @param[in]  ee               Execution Environment index
  @param[in]  irq_en           IRQ enable flag
  @param[in]  pipe_cfg_opts    Pipe Config data that indicates the IRQ mask

  @return
  Status of the operation.

*/
bam_status_type bamcore_pipe_setirqmode(bam_vaddr base, uint32 pipe_num, uint32 ee, uint32 irq_en,uint32 pipe_cfg_opts)
{
    uint32 irq_mask = 0;
    if(irq_en)
    {
        // Enable the Pipe Interrupt at the BAM level
        bamdev_write_reg_field(base, BAM_IRQ_SRCS_MSK_EEn(ee), (1 << pipe_num), pipe_num, 1);
        if(pipe_cfg_opts & BAM_O_DESC_DONE)    irq_mask |= BAM_MASK( BAM_P_IRQ_STTSn, P_PRCSD_DESC_IRQ );
        if(pipe_cfg_opts & BAM_O_INACTIVE )    irq_mask |= BAM_MASK( BAM_P_IRQ_STTSn, P_TIMER_IRQ );
        if(pipe_cfg_opts & BAM_O_WAKEUP   )    irq_mask |= BAM_MASK( BAM_P_IRQ_STTSn, P_WAKE_IRQ );
        if(pipe_cfg_opts & BAM_O_OUT_OF_DESC ) irq_mask |= BAM_MASK( BAM_P_IRQ_STTSn, P_OUT_OF_DESC_IRQ );
        if(pipe_cfg_opts & BAM_O_ERROR )       irq_mask |= BAM_MASK( BAM_P_IRQ_STTSn, P_ERR_IRQ );
        if(pipe_cfg_opts & BAM_O_EOT   )       irq_mask |= BAM_MASK( BAM_P_IRQ_STTSn, P_TRNSFR_END_IRQ);
        // set the ERROR and HRESP_ERR bits by default
        irq_mask |= BAM_MASK( BAM_P_IRQ_STTSn, P_PIPE_RST_ERROR_IRQ);
        irq_mask |= BAM_MASK( BAM_P_IRQ_STTSn, P_HRESP_ERR_IRQ);

        bamdev_write_reg(base, BAM_P_IRQ_ENn(pipe_num), irq_mask);
    }
    else
    {
        // Enable the Pipe Interrupt at the BAM level
        bamdev_write_reg_field(base, BAM_IRQ_SRCS_MSK_EEn(ee), (1 << pipe_num), pipe_num, 0);
    }
    return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bamcore_pipe_getirqmode
** ============================================================================
*/
/**
  Gets the IRQ mode for the BAM pipe.

  Checks if a given pipe operates in interrupt mode or not

  @param[in]  base             BAM virtual base address
  @param[in]  pipe_num         Index of the pipe
  @param[in]  ee               Execution Environment index
  @param[in]  *irq_en          IRQ enabled(1)/disabled(0) status

  @return
  Status of the operation.

*/
bam_status_type bamcore_pipe_getirqmode(bam_vaddr base, uint32 pipe_num, uint32 ee, uint32* irq_en)
{
    *irq_en = ((bamdev_read_reg(base, BAM_IRQ_SRCS_MSK_EEn(ee)) & (1<<pipe_num)) != 0);
    return BAM_SUCCESS;
}

/* ============================================================================
**  Function : bamcore_pipe_getirqmask
** ============================================================================
*/
/**
  Returns the IRQs enabled for the BAM Pipe.

  Returns the IRQs enabled for the BAM Pipe.

  @param[in]  base        BAM virtual base address
  @param[in]  pipe_num    Index of the pipe

  @return
  Returns the IRQs enabled for the pipe as a masked value.

*/
uint32 bamcore_pipe_getirqmask(bam_vaddr base, uint32 pipe_num)
{
    return bamdev_read_reg(base, BAM_P_IRQ_ENn(pipe_num));
}

/* ============================================================================
**  Function : bamcore_pipe_getandclearirqstatus
** ============================================================================
*/
/**
  Gets the IRQ status and clears it after reading it.

  Gets the IRQ status register value and clears the register after reading it.

  @param[in]  base        BAM virtual base address
  @param[in]  pipe_num    Index of the pipe

  @return
  Status bits read from the IRQ Status register based on the input mask
*/
uint32 bamcore_pipe_getandclearirqstatus(bam_vaddr base, uint32 pipe_num)
{
        uint32 status = 0;

        status = bamdev_read_reg(base, BAM_P_IRQ_STTSn (pipe_num));
        bamdev_write_reg(base, BAM_P_IRQ_CLRn(pipe_num), status);

        return status;

}

/* ============================================================================
**  Function : bamcore_pipe_setdescwriteoffset
** ============================================================================
*/
/**
  Starts a transfer by writing an offset to the Event Register.

  Starts a transfer by writing an offset to the Event Register. The offset is
  the offset of a descriptor *after* the last descriptor written by software.
  It indicates that BAM should process descriptors prior to that offset from the
  Descriptor Fifo.

  @param[in]  base          BAM virtual base address
  @param[in]  pipe_num      Index of the pipe
  @param[in]  next_write    Offset that will contain the next transfer
  @param[in]  bytes_consumed    Bytes consumed for triggerring event (SBZ for system mode)

  @return
  None.

*/
void bamcore_pipe_setdescwriteoffset(bam_vaddr base, uint32 pipe_num, uint32 next_write, uint32 bytes_consumed)
{
    uint32 value = ((next_write << BAM_SHFT(BAM_P_EVNT_REGn,P_DESC_FIFO_PEER_OFST)) & 
                    BAM_MASK(BAM_P_EVNT_REGn,P_DESC_FIFO_PEER_OFST)) |
                   ((bytes_consumed << BAM_SHFT(BAM_P_EVNT_REGn,P_BYTES_CONSUMED)) & 
                    BAM_MASK(BAM_P_EVNT_REGn,P_BYTES_CONSUMED));

    bamdev_write_reg(base, BAM_P_EVNT_REGn(pipe_num), value);
}

/* ============================================================================
**  Function : bamcore_pipe_getdescwriteoffset
** ============================================================================
*/
/**
  Gets the offset to the next descriptor in the FIFO that will be updated by
  the Software for the next transfer (in future).

  Gets the offset to the next descriptor in the FIFO that will be updated by
  the Software for the next transfer (in future). It indicates that BAM should
  process descriptors prior to that offset from the Descriptor Fifo.

  @param[in]  base        BAM virtual base address
  @param[in]  pipe_num    Index of the pipe

  @return
  Offset of the descriptor to be processed next.

*/
uint32 bamcore_pipe_getdescwriteoffset(bam_vaddr base, uint32 pipe_num)
{
    return bamdev_read_reg_field(base, BAM_P_EVNT_REGn(pipe_num),
            BAM_MNS(BAM_P_EVNT_REGn,P_DESC_FIFO_PEER_OFST));
}

/* ============================================================================
**  Function : bamcore_pipe_getdescreadoffset
** ============================================================================
*/
/**
  Gets the offset to the next descriptor in the FIFO that will be processed by
  the BAM.

  Gets the offset to the next descriptor in the FIFO that will be processed by
  the BAM. It may or may not process this offset, if the Event Register doesnt
  have an offset that is beyond this offset read from the BAM.

  @param[in]  base        BAM virtual base address
  @param[in]  pipe_num    Index of the pipe

  @return
  Offset of the descriptor to be processed next.
*/
uint32 bamcore_pipe_getdescreadoffset(bam_vaddr base, uint32 pipe_num)
{
    return bamdev_read_reg_field(base, BAM_P_SW_OFSTSn(pipe_num), BAM_MNS(BAM_P_SW_OFSTSn,SW_DESC_OFST));
}

/* ============================================================================
**  Function : bamcore_pipe_getdescfifofullflag
** ============================================================================
*/
/**
  Gets the indication of descriptor FIFO being full

  Gets the indication of descriptor FIFO being full. It is applicable to
  BAM2BAM producer pipes only

  @param[in]  base        BAM virtual base address
  @param[in]  pipe_num    Index of the pipe

  @return
  1 if descriptor FIFO is full, 0 otherwise.
*/
uint32 bamcore_pipe_getdescfifofullflag(bam_vaddr base, uint32 pipe_num)
{
    return bamdev_read_reg_field(base, BAM_P_PRDCR_SDBNDn(pipe_num), BAM_MNS(BAM_P_PRDCR_SDBNDn,BAM_P_CTRL)) & 1;
}

/* ============================================================================
**  Function : bamcore_pipe_timerconfig
** ============================================================================
*/
/**
  Configure inactivity timer count for a BAM pipe.

  Optional_detailed_description_goes_here.

  @param[in]  base             BAM virtual base address
  @param[in]  pipe_num         Index of the pipe
  @param[in]  mode             Operating mode of the inactivity timer
  @param[in]  timeout_count    timeout value for the inactivity timer

  @return
  None.

*/
void bamcore_pipe_timerconfig(bam_vaddr base, uint32 pipe_num, bam_timer_mode_type mode,
        uint32 timeout_count)
{
    bamdev_write_reg_field(base, BAM_P_TIMER_CTRLn(pipe_num), BAM_MNS(BAM_P_TIMER_CTRLn,P_TIMER_MODE), mode);
    bamdev_write_reg_field(base, BAM_P_TIMER_CTRLn(pipe_num), BAM_MNS(BAM_P_TIMER_CTRLn,P_TIMER_TRSHLD),
                timeout_count);
}

/* ============================================================================
**  Function : bamcore_pipe_timerreset
** ============================================================================
*/
/**
  Reset inactivity timer for a BAM pipe.

  Reset inactivity timer for a BAM pipe.

  @param[in]  base        BAM virtual base address
  @param[in]  pipe_num    Index of the pipe

  @return
  None.
*/
void bamcore_pipe_timerreset(bam_vaddr base, uint32 pipe_num)
{
    // reset
    bamdev_write_reg_field(base, BAM_P_TIMER_CTRLn(pipe_num), BAM_MNS(BAM_P_TIMER_CTRLn,P_TIMER_RST), 0);
    // active
    bamdev_write_reg_field(base, BAM_P_TIMER_CTRLn(pipe_num), BAM_MNS(BAM_P_TIMER_CTRLn,P_TIMER_RST), 1);
}

/* ============================================================================
**  Function : bamcore_pipe_timergetcount
** ============================================================================
*/
/**
  Gets the inactivity timer count for a BAM pipe.

  Gets the inactivity timer count for a BAM pipe.

  @param[in]  base        BAM virtual base address
  @param[in]  pipe_num    Index of the pipe

  @return
  Value in the inactivity timer's counter.
*/
uint32 bamcore_pipe_timergetcount(bam_vaddr base, uint32 pipe_num)
{
    return bamdev_read_reg(base, BAM_P_TIMERn(pipe_num));
}

/* ============================================================================
**  Function : bamcore_pipe_getevtregoffset
** ============================================================================
*/
/**
  Gets the offset for an event register for a BAM pipe.

  Gets the offset for an event register for a BAM pipe.

  @param[in]  pipe_num    Index of the pipe

  @return
  offset for an event register
*/
uint32 bamcore_pipe_getevtregoffset(uint32 pipe_num)
{
    return BAM_P_EVNT_REGn(pipe_num);
}

/* ============================================================================
**  Function : bamcore_testbusreg_dump
** ============================================================================
*/
/**
  Gets the dump of test bus reg of BAM device.
  Gets the dump of test bus reg of BAM device.
 
  @param[in]  base    BAM virtual base address.
  @param[in]  buf     Buffer to log the bam test bus reg.
 
  @return
  NOne
 
*/
static void bamcore_testbusreg_dump(bam_vaddr base, bamdebug_registers *buf)
{
    uint32 offset_val;
    for(offset_val = 0; offset_val < BAM_TESTBUS_MAX_INDEX; offset_val++)
    {
        bamdev_write_reg_field(base, BAM_TEST_BUS_SEL, BAM_MNS(BAM_TEST_BUS_SEL,BAM_TESTBUS_SEL), offset_val);
        buf->bam_test_bus_reg[offset_val] = bamdev_read_reg(base, BAM_TEST_BUS_REG);
    }
    buf->bam_test_bus_sel = bamdev_read_reg(base, BAM_TEST_BUS_SEL);
    return;
}

/* ============================================================================
**  Function : bamcore_headerdump
** ============================================================================
*/
/**
  Log the  revision, mask of pipe, peerpipe and size of pipe, peerpipe and header in dump buffer header.

  Log the  revision, mask of pipe,Peerpipe and size of pipe, peerpipe and header in dump buffer header.
  
  @param[in] buf          Buffer to log the header.
  @param[in] pipemask     Pipe mask
  @param[in] peerpmask    Peer pipe mask.

  @return
  None
 */
void bamcore_headerdump(uint32 *buf, uint32 pipemask, uint32 peerpmask)
{
    bam_dumpheader *ptr;
    if(NULL != buf)
    {
        ptr = (bam_dumpheader *)buf;
        ptr->revision = BAM_CORE_DUMP_REV;
        ptr->pipemask = pipemask;
        ptr->peerpipemask = peerpmask;
        ptr->dump_header_size = bamcore_getheaderbufferinfo();
        ptr->bam_buf_size = bamcore_getctrlregbufferinfo();
        ptr->pipe_buf_size = bamcore_getpiperegbufferinfo();
    }
    return;
}

/* ============================================================================
**  Function : bamcore_bamregdump
** ============================================================================
*/
/**
  Gets the Dump of BAM Core register.

  Gets the Dump of BAM Core register.

  @param[in] base     BAM virtual base address
  @param[in] buf      Buffer to store the register dump.
  @param[in] base_pa  Bam pa.
  @param[in] ee       Bam EE.

  @return
  None
 */
/* @ Brief about Dump buffer strucure.
 * ========================
 * @Header: Header will have revision no.(0x00010000),Pipe mask, peerpipe mask & size of header, bam , pipe reg dump.
 *                 More details check the header structure documentation.
 *                
 *=========================================================================
 * @BAM Dump: It will dump first Top level BAM.If peerpipe info is required in BAM2BAM mode,we will dump all peer BAM.
 * @Dump buffer structure looks like:
 *               Header-->Top BAM--->PeerBAM-->peerBAM...... goes on
 *  if peerpipe info in header is null, No peerBAM dump will be taken.
 *=========================================================================
 * @Pipe Dump:Pipe dump will be taken after Header, Bam and peerbam(if peerpipe info is not null).
 * @  In B2B mode, both pipe info will be logged on one after another.In system mode it will be                
 *     logged on as sequence of pipe mask.
 *                    
 * DUMP Buffer looks like:
 *   Header-->BAM-->Peer BAM(can be multiple peer BAM)-->Pipe-->peerpipe-->pipe-->pipe so on
 *  (can be multiple entries).
 *
 *              =========
 *              | Revision     |------> rev.
 *              =========
 *              |   Pipe       |------>  Pipe mask.
 *              =========                    
 *              |Peer pipe     |------> maskof peer pipe
 *              =========
 *              |header size   |------> size of header struct
 *              =========
 *              |BAM size      |------> Size of bam core reg. dump
 *              =========
 *              |Pipe size     |------> size of bam pipe reg. dump
 *              =========
 *              | Top BAM      |------> After Header Top BAM Always.Only one top BAM.
 *              =========
 *              |Peer BAM      |------> Only in BAM2BAM Mode & peerpipe mask != NULL. Can be multiple entries.
 *              =========
 *              |  Pipe        |--- ---> Can be Multiple entries.
 *              =========
 *              | Peer Pipe    |------> Only in BAM2 BAMmode & Peerpipe mask != Null in header.
 *              =========
 *
 */
void bamcore_bamregdump(bam_vaddr base, uint32 *buf, uint64 base_pa, uint32 ee)
{
    uint32 *temp = buf + (bamcore_getheaderbufferinfo()/sizeof(uint32 *));
    bamdebug_registers *bambuf = (bamdebug_registers *)temp;
    while(BAM_DUMP_TOP == bambuf->bam_dumptype)
    {
        if(base_pa  != bambuf->bam_base_pa)
        {
            bambuf++;
        }
        else
        {
            return; /* bam dump is already logged so return */
        }
    }
     
    if(0!= base)
    {
        /* Bam registers dump */
        bambuf->bam_dumptype = BAM_DUMP_TOP;
        bambuf->bam_base_pa = base_pa;
        bambuf->bam_ctrl = bamdev_read_reg(base, BAM_CTRL);
        bambuf->bam_revision = bamdev_read_reg(base, BAM_REVISION);
        bambuf->bam_desc_cnt_trshld = bamdev_read_reg(base,BAM_DESC_CNT_TRSHLD);
        bambuf->bam_irq_stts = bamdev_read_reg(base,BAM_IRQ_STTS);
        bambuf->bam_irq_clr = bamdev_read_reg(base,BAM_IRQ_CLR);
        bambuf->bam_irq_en = bamdev_read_reg(base,BAM_IRQ_EN);
        bambuf->bam_ahb_master_err_ctrls = bamdev_read_reg(base,BAM_AHB_MASTER_ERR_CTRLS);
        bambuf->bam_ahb_master_err_addr = bamdev_read_reg(base,BAM_AHB_MASTER_ERR_ADDR);
        bambuf->bam_ahb_master_err_data = bamdev_read_reg(base,BAM_AHB_MASTER_ERR_DATA);
        bambuf->bam_num_pipes = bamdev_read_reg(base, BAM_NUM_PIPES);
        bambuf->bam_timer = bamdev_read_reg(base, BAM_TIMER);
        bambuf->bam_timer_ctrl = bamdev_read_reg(base, BAM_TIMER_CTRL);
        bambuf->bam_cnfg_bits = bamdev_read_reg(base, BAM_CNFG_BITS);
        bambuf->bam_sw_version = bamdev_read_reg(base, BAM_SW_VERSION);
        /* BAM test bus reg dump */
        bamcore_testbusreg_dump(base, bambuf);
        /* Acess only for Top level BAM dump */
        if(ee != BAM_MAX_EES)
        {
            bambuf->bam_irq_srcs_een[ee] = bamdev_read_reg(base,BAM_IRQ_SRCS_EEn (ee));
            bambuf->bam_irq_srcs_msk_een[ee] = bamdev_read_reg(base,BAM_IRQ_SRCS_MSK_EEn(ee));
            bambuf->bam_irq_srcs_unmasked_een[ee] = bamdev_read_reg(base,BAM_IRQ_SRCS_UNMASKED_EEn(ee));
        }
    }
    return;
}

/* ============================================================================
**  Function : bamcore_getpiperegdump
** ============================================================================
*/
/**
  Gets the  BAM Pipe Core register value.

  Gets the BAM Pipe Core register value.

  @param[in] base       BAM virtual base address
  @param[in] base_pa    Bam base PA.
  @param[in] bufptr     Buffer to get Bam Pipe register dump.
  @param[in] pipenum    Pipe Number to be dumped.
  @param[in] buf_base   Pointer start of bam core register dump.

  @return
  None
 */
static void  bamcore_getpiperegs(bam_vaddr base, uint64 base_pa, uint32 *bufptr, uint32 pipenum, bam_vaddr buf_base)
{
    bamdebug_registers *ptr = (bamdebug_registers*)buf_base;
    bamdebug_p_register *buf = (bamdebug_p_register*)bufptr;
    if(0!= base)
    {
        buf->bam_p_dumptype = BAM_DUMP_PIPE;
        /* Get the bam core register dump start pointer */
        while(ptr->bam_dumptype == BAM_DUMP_TOP)
         {
             if(ptr->bam_base_pa == base_pa)
             {
                 buf->bam_ptr = (bamdebug_registers*)ptr;
                 break;
             }
             ptr++;
         }
  
         buf->bam_p_index = pipenum;
         buf->bam_p_ctrln = bamdev_read_reg(base, BAM_P_CTRLn(pipenum));
         buf->bam_p_rstn = bamdev_read_reg(base, BAM_P_RSTn(pipenum));
         buf->bam_p_haltn = bamdev_read_reg(base, BAM_P_HALTn(pipenum));
         buf->bam_p_irq_sttsn = bamdev_read_reg(base, BAM_P_IRQ_STTSn(pipenum));
         buf->bam_p_irq_clrn = bamdev_read_reg(base, BAM_P_IRQ_CLRn(pipenum));
         buf->bam_p_irq_enn = bamdev_read_reg(base, BAM_P_IRQ_ENn(pipenum));
         buf->bam_p_timern = bamdev_read_reg(base, BAM_P_TIMERn(pipenum));
         buf->bam_p_timer_ctrln = bamdev_read_reg(base, BAM_P_TIMER_CTRLn(pipenum));
         buf->bam_p_prdcr_sdbndn = bamdev_read_reg(base, BAM_P_PRDCR_SDBNDn(pipenum));
         buf->bam_p_cnsmr_sdbndn = bamdev_read_reg(base, BAM_P_CNSMR_SDBNDn(pipenum));
         buf->bam_p_evnt_dest_addrn = bamdev_read_reg(base, BAM_P_EVNT_DEST_ADDRn(pipenum));
         buf->bam_p_evnt_regn = bamdev_read_reg(base, BAM_P_EVNT_REGn(pipenum));
         buf->bam_p_sw_ofstsn = bamdev_read_reg(base, BAM_P_SW_OFSTSn(pipenum));
         buf->bam_p_data_fifo_addrn = bamdev_read_reg(base, BAM_P_DATA_FIFO_ADDRn(pipenum));
         buf->bam_p_desc_fifo_addrn = bamdev_read_reg(base, BAM_P_DESC_FIFO_ADDRn(pipenum));
         buf->bam_p_evnt_gen_trshldn = bamdev_read_reg(base, BAM_P_EVNT_GEN_TRSHLDn(pipenum));
         buf->bam_p_fifo_sizesn = bamdev_read_reg(base, BAM_P_FIFO_SIZESn(pipenum));
         buf->bam_p_retr_cntxtn = bamdev_read_reg(base, BAM_P_RETR_CNTXT_n(pipenum));
         buf->bam_p_si_cntxtn = bamdev_read_reg(base, BAM_P_SI_CNTXT_n(pipenum));
         buf->bam_p_df_cntxtn = bamdev_read_reg(base, BAM_P_DF_CNTXT_n(pipenum));
         buf->bam_p_au_psm_cntxt_1n = bamdev_read_reg(base, BAM_P_AU_PSM_CNTXT_1_n(pipenum));
         buf->bam_p_psm_cntxt_2n = bamdev_read_reg(base, BAM_P_PSM_CNTXT_2_n(pipenum));
         buf->bam_p_psm_cntxt_3n = bamdev_read_reg(base, BAM_P_PSM_CNTXT_3_n(pipenum));
         buf->bam_p_psm_cntxt_4n = bamdev_read_reg(base, BAM_P_PSM_CNTXT_4_n(pipenum));
         buf->bam_p_psm_cntxt_5n = bamdev_read_reg(base, BAM_P_PSM_CNTXT_5_n(pipenum));
    }
    else 
    {
         buf->bam_p_dumptype = BAM_DUMP_PIPE;
         buf->bam_p_index = pipenum;
         buf->bam_ptr = 0;
    }
    return;
}

/* ============================================================================
**  Function : bamcore_piperegdump
** ============================================================================
*/
/**
  Gets the Dump of BAM Pipe Core register.

  Gets the Dump of BAM Pipe Core register.

  @param[in] base       BAM virtual base address
  @param[in] base_pa    Bam base PA.
  @param[in] bufptr     Buffer to get Bam Pipe register dump.
  @param[in] pipedump   Pointer to peer pipe ,desc & data fifo core dump structure

  @return
  None
 */
void bamcore_piperegdump(bam_vaddr base, uint64 base_pa, uint32 *bufptr, bamdebug_pipe_config *pipedump)
{
    bam_vaddr buf_base = (bam_vaddr)(bufptr + (bamcore_getheaderbufferinfo()/sizeof(uint32 *)));
    uint32 *temp = bufptr + ((bamcore_getheaderbufferinfo()+ bamcore_getctrlregbufferinfo()) /sizeof(uint32 *));
    bamdebug_p_register *pipebuf = NULL;
    bamdebug_registers *ptr = (bamdebug_registers*)temp;
    /* find out end of bam reg dump */
    while(BAM_DUMP_TOP == ptr->bam_dumptype)
    {
        ptr++;
    }
     
    pipebuf=(bamdebug_p_register *)ptr;
    /* check if it first pipe logging or not */ 
    while(BAM_DUMP_PIPE == pipebuf->bam_p_dumptype)
    {
        pipebuf++;
    }
    temp=(uint32 *)pipebuf;
    /* Get the pipe dump */
    bamcore_getpiperegs(base, base_pa, temp, pipedump->pipenum, buf_base);
    
    if(pipedump->peerpipenum != 0xffffffff)
    { 
        /* update the Descriptor & data fifo buffer pointer */ 
        pipebuf->bam_p_desc_fifo =  pipedump->desc_fifo;
        pipebuf->bam_p_desc_fifo_size = pipedump->desc_fifo_size;
        pipebuf->bam_p_data_fifo =  pipedump->data_fifo;
        pipebuf->bam_p_data_fifo_size = pipedump->data_fifo_size;
       
        /* copy desc & data fifo */
        if((NULL != pipedump->data_fifo) && (0 != pipedump->data_base_va))
        {
            bam_osal_memcpy(pipedump->data_fifo, (uint32 *)pipedump->data_base_va, pipedump->data_fifo_size);
        }
        if((NULL != pipedump->desc_fifo) && (0 != pipedump->desc_base_va))
        {
            bam_osal_memcpy(pipedump->desc_fifo, (uint32 *)pipedump->desc_base_va, pipedump->desc_fifo_size);
        }
        temp += (bamcore_getpiperegbufferinfo() /sizeof(uint32 *));
        /* Update the peer pipe pointer */
        pipebuf->bam_p_peer_ptr = (bamdebug_p_register *)temp;   
        /* Get Peer pipe core dump */
        bamcore_getpiperegs(pipedump->peerbase_va, pipedump->peerbase_pa, temp, pipedump->peerpipenum, buf_base);
        /* update the peer pipe pointer to peer pipe */
        temp =(uint32 *)pipebuf;
        (++pipebuf)->bam_p_peer_ptr = (bamdebug_p_register*)temp;

        /* update the Desc and Data FIFO info in peer pipe side as well */
        pipebuf->bam_p_desc_fifo =  pipedump->desc_fifo;
        pipebuf->bam_p_desc_fifo_size = pipedump->desc_fifo_size;
        pipebuf->bam_p_data_fifo =  pipedump->data_fifo;
        pipebuf->bam_p_data_fifo_size = pipedump->data_fifo_size;
    }
    else
    {
        /* Peer pipe info, Desc and Data FIFO info of client provided buffer will be NULL in system mode */
        pipebuf->bam_p_peer_ptr = NULL;
        pipebuf->bam_p_desc_fifo =  NULL;
        pipebuf->bam_p_desc_fifo_size = 0;
        pipebuf->bam_p_data_fifo =  NULL;
        pipebuf->bam_p_data_fifo_size = 0;
    }
    return;
}

/* ============================================================================
**  Function : bamcore_getctrlregbufferinfo
** ============================================================================
*/
/**
   Gets the BAM control reg dump buffer size.

   Gets the BAM control reg dump buffer size.

   @return
   buffer size required for bam reg.
 */
uint32 bamcore_getctrlregbufferinfo(void)
{
    return(sizeof(bamdebug_registers));
}

/* ============================================================================
**  Function : bamcore_getpiperegbufferinfo
** ============================================================================
*/
/**
   Gets the Pipe register dump buffer size.

   Gets the Pipe register dump buffer size.

   @return
   buffer size requied for single pipe.
  */
uint32 bamcore_getpiperegbufferinfo(void)
{
    return(sizeof(bamdebug_p_register));
}

/* ============================================================================
**  Function : bamcore_getheaderbufferinfo
** ============================================================================
*/
/**
   Gets the Header dump buffer size.

   Gets the Header dump buffer size.

   @return
   buffer size requied for bam core dump header.
  */
uint32 bamcore_getheaderbufferinfo(void)
{
    return(sizeof(bam_dumpheader));
}

/* ============================================================================
**  Function : bamcore_bam_enabled
** ============================================================================
*/
/**
  Check whether a given bam is enabled 

  @param[in] base       BAM virtual base address
  @param[in] ee         ee of the current enviornment

  @return
     1   - bam is enabled
     0   - bam is disabled
*/
uint32 bamcore_bam_enabled(bam_vaddr base,uint32 ee)
{
    uint32 ver = 0;
    uint32 bam_enabled = 0;

    ver = bamdev_read_reg_field(base, BAM_REVISION,BAM_MNS(BAM_REVISION,REVISION));

    //For ndp_bam version >=0x23 , bam_version > 0x9 or bam_lite_version > 16, use the
    //pipe attribute register

    if ( IS_PIPE_ATTRIBS_SUPPORTED(ver) && ee != BAM_EE_TRUST )
    {
        bam_enabled =  bamdev_read_reg_field(base,BAM_PIPE_ATTR_EEn(ee),BAM_MNS(BAM_PIPE_ATTR_EEn,BAM_ENABLED));
    }
    else     
    {
        bam_enabled = bamdev_read_reg_field(base,BAM_CTRL, BAM_MNS(BAM_CTRL,BAM_EN));
    }

    return ( bam_enabled != 0 );

}

/* ============================================================================
**  Function : bamcore_pipe_enabled
** ============================================================================
*/
/**
  Check whether a given pipe is enabled 

  @return
     1   - pipe is enabled
     0   - pipe is disabled
*/
uint32 bamcore_pipe_enabled(bam_vaddr base,uint32 pipe_num)
{
    return bamdev_read_reg_field(base,BAM_P_CTRLn(pipe_num), BAM_MNS(BAM_P_CTRLn,P_EN));
}

/* ============================================================================
**  Function : bamcore_pipe_nwd_done
** ============================================================================
*/
/**
  Check if nwd_done toggle was recived for this pipe
  Check if 2nd bit is set in P_CTRL
  @return
     1   - nwd_done toggle recived
     0   - nwd_done toggle not recived
*/
uint32 bamcore_pipe_nwd_done(bam_vaddr base, uint32 pipe_num)
{
     return (bamdev_read_reg_field(base,BAM_P_CNSMR_SDBNDn(pipe_num),BAM_MNS(BAM_P_CNSMR_SDBNDn,BAM_P_CTRL)) & 0x2) == 0; 
}

/* ============================================================================
**  Function : bamcore_pipe_get_descfifobase
** ============================================================================
*/
/**
  Get the descriptor fifo base of a given pipe

  @return descriptor fifo base (physical address)
*/
uint32 bamcore_pipe_get_descfifobase(bam_vaddr base,uint32 pipe_num)
{
    return bamdev_read_reg(base,BAM_P_DESC_FIFO_ADDRn(pipe_num));
}

/* ============================================================================
**  Function : bamcore_pipe_get_descfifosize
** ============================================================================
*/
/**
  Get the descriptor fifo size of a given pipe

  @return descriptor fifo size
*/
uint32 bamcore_pipe_get_descfifosize(bam_vaddr base,uint32 pipe_num)
{
    return bamdev_read_reg_field(base,BAM_P_FIFO_SIZESn(pipe_num),BAM_MNS(BAM_P_FIFO_SIZESn,P_DESC_FIFO_SIZE));
}

/* ============================================================================
**  Function : bamcore_pipe_get_toggle
** ============================================================================
*/
/**
  Get the toggle for a particular pipe

  @return pipe toggle 
*/
uint32 bamcore_pipe_get_toggle(bam_vaddr base, uint32 pipe_num)
{
   return (bamdev_read_reg_field(base,BAM_P_CNSMR_SDBNDn(pipe_num),BAM_MNS(BAM_P_CNSMR_SDBNDn,BAM_P_TOGGLE)) & 0x1) ; 
}

/* ============================================================================
**  Function : bamcore_pipe_desc_published
** ============================================================================
*/
/**
  Check if a descriptor was published to the consumer

  @return descriptor was published or not
*/
uint32 bamcore_pipe_desc_published(bam_vaddr base,uint32 pipe_num)
{
   return (bamdev_read_reg_field(base,BAM_P_CNSMR_SDBNDn(pipe_num),BAM_MNS(BAM_P_CNSMR_SDBNDn,BAM_P_WAIT_4_ACK)) & 0x1) == 0 ; 
}

