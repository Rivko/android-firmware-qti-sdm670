#ifndef BAMI_H_
#define BAMI_H_

/**
  @file bami.h
  @brief
  This file contains definitions of constants, data structures, and
  interfaces internal to BAM driver.

*/
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/28/16   SS      Added Check for Desc fifo and Fifo Size aligment
02/11/14   RL      64-bit support
08/15/13   RL      LPAE support
02/28/13   SS      Added Peer BAM PA to VA Mapping.
02/13/13   SS      Created

===============================================================================
                   Copyright (c) 2013-2016 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "bamcore.h"
#include "bamosal.h"

#define BAM_FLAG_ALLOC_DEV 0x1
#define BAM_FLAG_ALLOC_DESC 0x2
#define BAM_FLAG_ALLOC_DATA 0x4
#define PIPE_MEM_PT_HEADER_MAGIC 0x00434D50
#define BAM_PIPE_INVALID   0xffffffff

/* IOVEC options in bam_iovec_options_type that bam hw would understand */
#define BAM_IOVEC_HW_FLAGS   ( BAM_IOVEC_FLAG_INT | BAM_IOVEC_FLAG_EOT | \
                               BAM_IOVEC_FLAG_EOB | BAM_IOVEC_FLAG_NWD | \
                               BAM_IOVEC_FLAG_CMD | BAM_IOVEC_FLAG_LCK | \
                               BAM_IOVEC_FLAG_UNLCK | BAM_IOVEC_FLAG_IMM_CMD)

/* IOVEC options that would require callback notifications */
#define BAM_IOVEC_CB_FLAGS ( BAM_IOVEC_FLAG_INT | BAM_IOVEC_FLAG_EOT )

#define BAM_GET_DEV_FROM_HANDLE(handle) (*(client_ctx*)(handle)).bam

/* Return TRUE if size is not multiple of 0x8 or greater than 32k-1 else FALSE*/
#define BAM_IS_INVALID_DESC_FIFO_SIZE(size) (((size%8) != 0) || (size > 0x7FFF))
/*< Return TRUE if address is not 8-byte aligned else FALSE */
#define BAM_IS_DESC_FIFO_ALIGNED(desc_pa)   ((desc_pa & 0x7) != 0)

/* Return a 1 if any of the handles are NULL, otherwise return a 0 */
#define BAM_IS_INVALID_CLIENT_HANDLE(handle) ( ((handle)==(bam_handle)NULL)||((*(client_ctx*)(handle)).bam==NULL) )

typedef struct _bamdev bam_dev;

typedef struct _descfifo
{
    void *user_data;
    bam_result_type result;
}desc_fifo_type;

typedef struct _pipemempte
{
    uint64 prod_evt_reg_pa;
    uint64 cons_evt_reg_pa;
    uint64 desc_fifo_pa;
    uint64 data_fifo_pa;
    uint32 fifo_sizes;
}pipe_mem_pte;

typedef struct _pipedev
{
    bam_dev *bam;
    uint32 pipe_num;
    uint32 flags;                           /*<-- refer to BAM_FLAG_ */
    uint32 irq_enabled;
    bam_pipe_config_type pipe_cfg;          /*<-- pipe configuration provided by the client */
    bam_callback_type pipe_cb;
    desc_fifo_type *desc_fifo;              /*<-- a copy of the callback details to be provided */
    uint32 last_offset;                     /*<-- last descriptor submitted */
    uint32 desc_offset;                     /*<-- next new descriptor to be written to hardware */
    uint32 acked_offset;                    /*<-- next descriptor to be retired by software */
    uint32 size_since_eot;                  /*<-- number of bytes processed since last eot */
    bam_osal_sync_type sync;                /*<-- lock to protect pipe_dev */
    bam_osal_sync_type results_sync;        /*<-- lock to protect the result structures in desc_fifo_type */

    bam_osal_meminfo pipe_desc_mem;
    bam_osal_meminfo pipe_data_mem;

    /* peer bam information */
    bam_osal_meminfo bam_mem_peer;          /*<-- handle used for mapping peer bam pa */
    bam_vaddr bam_peer_va;                  /*<-- peer bam va for this pipe */

    /* debug related stats */
    uint32 zlt_trigger_count;               /*<-- number of time zlt trigger pipe event was called */
    uint32 last_desc_full_state;            /*<-- fifo full state for this pipe when zlt trigger pipe event was called */
    uint32  trigger_pipe_sys_replicate_cnt; /*<-- in trigger pipe sys mode api, how many times the descriptor fifo was
                                                  replicated */
    bam_event_type last_error;              /*<-- record the last error encountered by this pipe */
    uint32 toggle_at_reset;                 /*<-- sample the toggle after a pipe reset */
    uint32 zlt_empty_cnt;                   /*<-- zlt empty condition satisfied count */
    uint32 zlt_empty_check_cnt;             /*<-- zlt empty condition check count */
}pipe_dev;

typedef struct _list_t
{
    struct _list_t *next;
    struct _list_t *prev;
}list_t;

typedef struct _client_ctx
{
    list_t list;  /* Must be first  */
    bam_dev *bam;  /* Pointer to bam device in use by client */
    /* ---------------------------------------------------------------*/
    /* Any other info we want to store thats specific to client. */
    bam_callback_type bam_cb;
    uint32 irq_mask; /* bam top interrupt mask  */
}client_ctx;

struct _bamdev
{
    bam_config_type bam_cfg;//Provided by the client
    const bam_target_config_type *bam_tgt_cfg;
    bam_caps_type bam_caps;
    uint32 active_pipes_mask;
    uint32 irq_enabled;
    pipe_dev **pipes;
    bam_osal_sync_type sync;
    bam_osal_irq_type irqhandle;
    bam_osal_meminfo bam_mem;
    bam_result_type    last_event; 
    list_t client_list;
};

/* To intialize the peer BAM info for BAM driver use */
typedef struct
{
    uint32 pipe_num;
    uint32 peerpipe_num;
    uint64 bam_peer_pa;
    bam_vaddr bam_peer_va;
}bam_peer_info;


/**
 * @brief This api is doing mapping of PA to VA
 *
 * This API will map peer bam PA to virtual base address.
 *
 * @param pipe   Pipe device handle
 *
 * @return       None.
 *
 */
void bam_map_peer_pa(pipe_dev *pipe);

/**
 * @brief Free the Mapping of Peer BAM VA.
 *
 * This API will free the Mapping of Peer BAM VA.
 *
 * @param pipe             Bam Pipe Handle
 * @param pipe_initmask	   Mask of intialized BAM pipes.
 *
 * @return                 None.
 */
void bam_unmap_peer_pa(pipe_dev *pipe, uint32 pipe_initmask);

#endif /* BAMI_H_ */
