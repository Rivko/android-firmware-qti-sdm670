#ifndef BAMCORE_H_
#define BAMCORE_H_

/**
  @file bamcore.h
  @brief
  This file contains definitions of constants, data structures, and
  interfaces to control the BAM hardware.

*/
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/11/14   RL      64-bit support
08/15/13   RL      Added LPAE support
04/23/13   RL      Added apis to query if bam or a pipe is enabled
02/05/13   SS      Added Multi VMID support for pipe access control.
01/15/13   SS      Added Multi VMID support for BAM ctrl group
12/04/12   AN      Added function to read error bus control register
09/20/12   SS      Added Desc & Data FIFO info to core dump
05/16/12   SS      Added Core Register Dump support.
04/23/12   NK      Added bam_deinit
02/29/12   MK      Added bamcore_getmaxtransfersize and
                   bamcore_pipe_setlockgroup APIs to support Core Rev 4+
04/04/11   MK      Created

===============================================================================
                   Copyright (c) 2011-2014 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/


#include "DALStdDef.h"
#include "bam.h"
#include "bamtgtcfg.h"



#define BAM_DESC_SIZE 	8 // Size of a descriptor in bytes
#define BAM_EE_MAX		8

//extract the MSB 4 bits of a 36 bit LPAE address
#define ADDR_LPAE_MSB(x)                 ((((uint64)(x))>>32) & 0xF )

//extract the LSB 32 bits of a 36 bit address
#define ADDR_LPAE_LSB(x)                 ((uint32)((x)&0xFFFFFFFF))


/** @brief BAM device capabilities structure
  */

typedef struct _bamcaps
{
    uint32 num_pipes;           /**< -- Number of pipes */
    uint32 revision;            /**< -- Revision of BAM */
    uint32 num_ees;             /**< -- Number of EEs */
    uint32 is_secured;          /**< -- Is a secure BAM */
    uint32 inactive_timer_base; /**< -- Width of the inactivity timer */
    uint32 has_inactive_timer;  /**< -- Has a inactivity timer */
    uint32 is_hi_freq_bam;      /**< -- Is a high frequency BAM */
    uint32 no_bypass;           /**< -- Has no bypass feature */
    uint32 cmd_desc_en;         /**< -- Support Command Descriptors */
    uint32 num_inactive_timers; /**< -- Number of inactive timers */
    uint32 desc_cache_depth;    /**< -- Descriptor Cache Depth per pipe */
    uint32 use_vmidmt_mask;     /**< -- BAM has VMIDMT supported in HW */
    uint32 axi_active_mask;     /**< -- BAM_NDP uses internal AXI bridge*/
    uint32 ce_buf_size;         /**< -- Size of CE buf (1-4words;2-8words;3-16words) */
    uint32 data_addr_width;     /**< -- Data bus address width ( 0 - 32bit, 1-36 bit ) */
}bam_caps_type;

/**@ Peer pipe and descriptor and data fifo info structure.
    */
    
typedef struct 
{  
    uint32 pipenum;             /**<-- Pipe index */
    uint32 peerpipenum;         /**<-- Peer pipe index  */
    bam_vaddr peerbase_va;      /**<-- Peer BAM virtual base address */ 
    uint64 peerbase_pa;         /**<-- Peer BAM physical address */     
    bam_vaddr desc_base_va;     /**<-- Descriptor virtual base address  */     
    void *desc_fifo;            /**<-- Descriptor fifo buffer to hold desciptor info */    
    uint32 desc_fifo_size;      /**<-- Descriptor fifo buf size */	
    bam_vaddr data_base_va;     /**<-- Data fifo virtula base address */    
    void *data_fifo;            /**<-- Data fifo buffer to fold data fifo data*/    
    uint32 data_fifo_size;	    /**<-- Size of data fifo buffer. */	 
}bamdebug_pipe_config;


/** Mask 'n' Shift Macro
 *  @note Can be used only as params to a function!!!
   */
#define BAM_MNS(reg,field) reg##_##field##_MASK, reg##_##field##_SHFT

/** Mask macro */
#define BAM_MASK(reg,field) ( reg##_##field##_MASK )

/** Shift Macro */
#define BAM_SHFT(reg,field) ( reg##_##field##_SHFT )

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
void bamcore_init(bam_vaddr base, uint32 irq, uint32 irq_mask, uint32 sum_thresh, const bam_target_config_type* tgt_cfg);

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
void bamcore_deinit(bam_vaddr base);

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
bam_status_type bamcore_secure_init(bam_vaddr base, const bam_target_config_type *tgtcfg);

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
bam_status_type bamcore_pipe_secure_init(bam_vaddr base, const bam_target_config_type *tgt_cfg);

/* ============================================================================
**  Function : bamcore_secure_set_sg
** ============================================================================
*/
/**
  Assigns supergroup ID to a given subset of pipes.

  Assigns supergroup ID to a given subset of pipes.

  @param[in]  base        BAM virtual base address
  @param[in]  tgt_cfg     BAM target config handle.

  @return
  None.

*/
bam_status_type bamcore_secure_set_sg( bam_vaddr base, const bam_target_config_type *tgt_cfg);

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
bam_status_type bamcore_check(bam_vaddr base, bam_caps_type *bam_caps, uint32 ee);

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
bam_status_type bamcore_setirqmode(bam_vaddr base, uint32 ee, uint32 irq_en,uint32 irq_mask);

/* ============================================================================
**  Function : bamcore_getirqmode
** ============================================================================
*/
/**
  Get the irq mask for BAM


  @param[in]  base        BAM virtual base address

  @return
  bam irq mask

*/
uint32 bamcore_getirqmode(bam_vaddr base);



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
bam_status_type bamcore_pipe_getirqmode(bam_vaddr base, uint32 pipe_num, uint32 ee, uint32* irq_en);

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
uint32 bamcore_getmaxtransfersize(void);

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
uint64 bamcore_get_error_address(bam_vaddr base);

/* ============================================================================
**  Function : bamcore_get_error_data
** ============================================================================
*/
/**
  Gets the data that caused an error.

  Gets the data that caused an error.

  @param[in]  base    BAM virtual base address

  @return
  returns the data that caused an error.

*/
uint32 bamcore_get_error_data(bam_vaddr base);

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
uint32 bamcore_get_error_bus_ctrls(bam_vaddr base);

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
uint32 bamcore_get_irq_sources(bam_vaddr base, uint32  ee, uint32  mask);

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
uint32 bamcore_get_and_clear_irq_status(bam_vaddr base, uint32  mask);

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
bam_status_type bamcore_pipe_init(bam_vaddr base, uint32 pipe_num,bam_pipe_config_type *pipe_cfg,uint32 irq_en, uint32 ee);

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
bam_status_type bamcore_pipe_deinit(bam_vaddr base, uint32 pipe_num,uint32 irq_dis, uint32 ee);

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
bam_status_type bamcore_pipe_setlockgroup(bam_vaddr base, uint32 pipe_num, uint32 lock_group);

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
bam_status_type bamcore_pipe_setirqmode(bam_vaddr base, uint32 pipe_num, uint32 ee, uint32 irq_en,uint32 pipe_cfg_opts);

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
uint32 bamcore_pipe_getirqmask(bam_vaddr base, uint32 pipe_num);

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
uint32 bamcore_pipe_getandclearirqstatus(bam_vaddr base, uint32 pipe_num);

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
void bamcore_pipe_setdescwriteoffset(bam_vaddr base, uint32 pipe_num, uint32 next_write, uint32 bytes_consumed);

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
uint32 bamcore_pipe_getdescwriteoffset(bam_vaddr base, uint32 pipe_num);

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
uint32 bamcore_pipe_getdescfifofullflag(bam_vaddr base, uint32 pipe_num);

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
uint32 bamcore_pipe_getdescreadoffset(bam_vaddr base, uint32 pipe_num);

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
		uint32 timeout_count);


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
void bamcore_pipe_timerreset(bam_vaddr base, uint32 pipe_num);

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
uint32 bamcore_pipe_timergetcount(bam_vaddr base, uint32 pipe_num);

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
uint32 bamcore_pipe_getevtregoffset(uint32 pipe_num);

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
uint32 bamcore_getheaderbufferinfo(void);

/* ============================================================================
**  Function : bamcore_headerdump
** ============================================================================
*/
/**
  Log the pipe,Pipepipe & revision in dump buffer header.

  Log the pipe,Pipepipe & revision in dump buffer header.

  @param[in]  buf		  Buffer to log the header.
  @param[in]  pipemask	  Pipe mask
  @param[in]  peerpmask	  Peer pipe mask.

  @return
  None
 */
void bamcore_headerdump(uint32 *buf, uint32 pipemask, uint32 peerpmask);

/* ============================================================================
**  Function : bamcore_bamregdump
** ============================================================================
*/
/**
  Gets the Dump of BAM Core register.

  Gets the Dump of BAM Core register.

  @param[in]  base      BAM virtual base address
  @param[in]  buf	    Buffer to store the register dump.
  @param[in]  base_pa   Bam pa.
  @param[in]  ee        Bam EE.

  @return
  None
 */
void bamcore_bamregdump(bam_vaddr base, uint32 *buf, uint64 base_pa, uint32 ee);

/* ============================================================================
**  Function : bamcore_piperegdump
** ============================================================================
*/
/**
  Gets the Dump of BAM Pipe Core register.

  Gets the Dump of BAM Pipe Core register.

  @param[in]  base       BAM virtual base address
  @param[in]  base_pa    Bam base PA.
  @param[in]  bufptr	 Buffer to get Bam Pipe register dump.
  @param[in]  pipedump   Pointer to peer pipe ,desc & data fifo core dump structure

  @return
  None
 */
void bamcore_piperegdump(bam_vaddr base, uint64 base_pa, uint32 *bufptr, bamdebug_pipe_config *pipedump);

/* ============================================================================
**  Function : bamcore_getctrlregbufferinfo
** ============================================================================
*/
/**
  Gets the BAM control reg dump buffer size.

  Gets the BAM control reg dump buffer size.

  @return
  buffer size requied for bam reg.
  */
uint32 bamcore_getctrlregbufferinfo(void);

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
uint32 bamcore_getpiperegbufferinfo(void);

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

uint32 bamcore_bam_enabled(bam_vaddr base, uint32 ee);

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
uint32 bamcore_pipe_enabled(bam_vaddr base,uint32 pipe_num);

/* ============================================================================
**  Function : bamcore_pipe_nwd_done
** ============================================================================
*/
/**
  Check if nwd_done toggle was recived for this pipe
  @return
     1   - nwd_done toggle recived
     0   - nwd_done toggle not recived
*/
uint32 bamcore_pipe_nwd_done(bam_vaddr base, uint32 pipe_num);

/* ============================================================================
**  Function : bamcore_pipe_get_descfifosize
** ============================================================================
*/
/**
  Get the descriptor fifo size of a given pipe

  @return descriptor fifo size
*/
uint32 bamcore_pipe_get_descfifosize(bam_vaddr base,uint32 pipe);

/* ============================================================================
**  Function : bamcore_pipe_get_descfifobase
** ============================================================================
*/
/**
  Get the descriptor fifo base of a given pipe

  @return descriptor fifo base (physical address)
*/

uint32 bamcore_pipe_get_descfifobase(bam_vaddr base,uint32 pipe);

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
uint32 bamcore_lpae_supported(bam_vaddr base );

/* ============================================================================
**  Function : bamcore_pipe_desc_published
** ============================================================================
*/
/**
  Check if a descriptor was published to the consumer

  @return descriptor was published or not
*/
uint32 bamcore_pipe_desc_published(bam_vaddr base,uint32 pipe_num);

/* ============================================================================
**  Function : bamcore_pipe_get_toggle
** ============================================================================
*/
/**
  Get the toggle for a particular pipe

  @return pipe toggle 
*/
uint32 bamcore_pipe_get_toggle(bam_vaddr base, uint32 pipe_num);


#endif /* BAMCORE_H_ */

