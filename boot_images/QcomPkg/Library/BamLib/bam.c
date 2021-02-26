/**
  @file bam.c
  @brief
  This file contains the implementation of the interfaces that provide operational 
  control to the BAM device.

*/
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/28/16   SS      Added Check for Desc fifo and Fifo Size aligment
02/11/14   RL      64-bit support
08/19/13   RL      LPAE support
06/19/13   RL      Consider NWD done toggle when polling
05/06/13   SS      Added Error Logging
02/28/13   SS      Added Peer BAM PA to VA mapping for core dump
02/13/13   SS      Moved bam core dump to different file.
02/05/13   SS      Added Multi VMID support for pipe access control.
01/14/13   SS      Added Multi VMID support for BAM ctrl group
12/04/12   AN      Added error bus control register to result data structure
11/15/12   SS      Added KW warning fixes and Lowred the level of BAM informative message logging.
11/12/12   AN      Added trigger event and current descriptor info funcs
09/20/12   SS      Added Desc & Data FIFO info to core dump
08/06/12   PR      check for bam_osal_mallocmemregion call successful or not
08/02/12   NK      Remove memory malloc from bamdrv init 
06/21/12   SS      Added core dump support for peer bam pipe
04/16/12   SS      Added Support for Core Dump feature
04/13/12   NK      Bam de-init support plus blsp fixes
03/20/12   NK      Add memory mapping support for descriptor fifos
03/09/12   NK      Lazy INIT support, minor fixes
02/08/12   NK      Added support for bamcore4, tracing and debug logging
09/17/11   MK      Added TZ support
04/04/11   MK      Created

===============================================================================
                   Copyright (c) 2012-2016 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/



#include "bam.h"
#include "bami.h"
#include "bamtgtcfg.h"
#include "bamcore.h"
#include "bamosal.h"
#include "string.h"

#define BAM_OPT_SET(x,opt)  (((x)&(opt))!=0)
#define BAM_OPT_NOT_SET(x,opt)  (((x)&(opt))==0)

// Globals common to the driver
bam_dev **bams;
bam_osal_sync_type bam_drv_sync;
uint32 num_allocated_devices = 0;
pipe_mem_pte *pipe_mem_pt = NULL;
bam_osal_clock_ctl_type pipe_mem_clock_ctl;
uint32 pipe_mem_pte_num = 0;
static uint32 bam_drv_init_done=0;

static int32 insertHead(list_t *list, list_t *i)
{
   i->next = list->next;
   i->prev = list; 

   if(list->next == NULL)
   {
      return -1; 
   }
   list->next->prev = i; 
   list->next = i;

   return 0;
}

static int32 removeHead(list_t *i)
{
  if(i->next == NULL || i->prev == NULL)
  {
     return -1; 
  }
  i->prev->next = i->next; 
  i->next->prev = i->prev; 

  return 0;
}

// Pipe memory parition table (version 0)
// --------------------------------------
// table header:
// word  0: 0x00434D50
// word  1: [31:24] 0x0
//          [23: 0] number of partiton elements
// 
// partition elements list follows
// table header with no gaps
// 
// partition element:
// word 0: producer pipe event register address
// word 1: consumer pipe event register address
// word 0: descriptors FIFO address
// word 0: data FIFO address address
// word 0: [31:16] data FIFO size
//         [15: 0] descriptors FIFO size
// --------------------------------------

static bam_status_type read_pipe_mem_pt(void)
{
    uint32* pt_addr_pa = (uint32*)bam_tgt_getpipememptaddr();
    uint32* pt_addr;
    bam_status_type status = BAM_FAILED;
    bam_osal_meminfo pipe_mem;
    
    if (pt_addr_pa == NULL)
    {
        return BAM_SUCCESS;
    }

    // map pipe memory to read partition table
    pipe_mem.handle = NULL;
    pipe_mem.pa = (uint64)((bam_vaddr)pt_addr_pa);
    pipe_mem.va = 0;
    pipe_mem.size = (uint32)(bam_tgt_getpipememendaddr()- bam_tgt_getpipememstartaddr() + 1);

    bam_osal_mallocmemregion(&pipe_mem, pipe_mem.size);

    if (pipe_mem.handle == NULL)
    {
        bam_osal_debug(MSG_ERR,"read_pipe_mem_pt:BAM pipe memory mapping failed\n");
        return BAM_FAILED;
    }

    pt_addr = (uint32*)pipe_mem.va;
    

    if (bam_osal_clock_ctl_enable(&pipe_mem_clock_ctl) != BAM_SUCCESS)
    {
        bam_osal_debug(MSG_ERR, "Pipe Memory clk enable failed \n" );
        return BAM_FAILED;
    }

    do
    {
        uint32 pte_num;

        if (pt_addr[0] != PIPE_MEM_PT_HEADER_MAGIC)
        {
            bam_osal_debug(MSG_ERR, "pipe mem pt magic not found 0x%X\n",pt_addr[0]);
            break;
        }

        if ((pt_addr[1] & 0xFF000000) > 0) // for now support version 0 only
        {
            bam_osal_debug(MSG_ERR, "pipe mem pt version not supported 0x%X\n",(pt_addr[1] >> 24));
            break;
        }

        pte_num = pt_addr[1] & 0x00FFFFFF;

        if (pte_num != 0)
        {
            uint32 pt_size = sizeof(pipe_mem_pte)*pte_num;

            pipe_mem_pt = (pipe_mem_pte*)bam_osal_malloc(pt_size);

            if (pipe_mem_pt == NULL)
            {
                bam_osal_debug(MSG_ERR, "could not allocate memory for pipe mem pt 0x%X\n",pte_num);
                break;
            }

            bam_osal_memcpy(pipe_mem_pt, &pt_addr[2], pt_size);

            // set the global variable after the partition
            // table has been completely initialized
            pipe_mem_pte_num = pte_num;
        }

        status = BAM_SUCCESS;
    } while (status != BAM_SUCCESS);
    
    bam_osal_clock_ctl_disable(&pipe_mem_clock_ctl);
    bam_osal_freememregion(&pipe_mem);
    
    return status;
}

static bam_status_type bam_validate_pipe_cfg(bam_dev *bam, uint32 pipe_num, bam_pipe_config_type *pipe_cfg)
{
    // if the client has not provided desc FIFO address then 
    // look up pipe memory partition table
    if (pipe_cfg->mode == BAM_MODE_BAM2BAM)
    {
        uint32 pe;
        uint64 prod_evt_reg, cons_evt_reg;

        // read pipe mem pt if haven't done yet 
        if (pipe_mem_pte_num == 0) 
        {
            bam_status_type status = BAM_SUCCESS;

            bam_osal_syncenter(&bam_drv_sync);
            // check one more time to be sure
            // the partition table has not been
            // initialized while we waited to
            // enter driver sync section
            if (pipe_mem_pte_num == 0) 
            {
                status = read_pipe_mem_pt();
            }
            bam_osal_syncleave(&bam_drv_sync);

            if (status != BAM_SUCCESS)
                return status;
        }

        if (pipe_cfg->dir == BAM_DIR_PRODUCER)
        {
            prod_evt_reg = bam->bam_cfg.bam_pa + bamcore_pipe_getevtregoffset(pipe_num);
            cons_evt_reg = pipe_cfg->peer_base_pa + bamcore_pipe_getevtregoffset(pipe_cfg->peer_pipe_num);
        }
        else
        {
            prod_evt_reg = pipe_cfg->peer_base_pa + bamcore_pipe_getevtregoffset(pipe_cfg->peer_pipe_num);
            cons_evt_reg = bam->bam_cfg.bam_pa + bamcore_pipe_getevtregoffset(pipe_num);
        }

        for (pe = 0; pe < pipe_mem_pte_num; pe++)
        {
            if (prod_evt_reg == pipe_mem_pt[pe].prod_evt_reg_pa && 
                cons_evt_reg == pipe_mem_pt[pe].cons_evt_reg_pa)
            {
                if (!pipe_cfg->desc_base_pa)
                {
                    pipe_cfg->desc_base_pa = pipe_mem_pt[pe].desc_fifo_pa;
                    pipe_cfg->desc_size = pipe_mem_pt[pe].fifo_sizes & 0xFFFF;
                }
                else if (pipe_cfg->desc_base_pa != pipe_mem_pt[pe].desc_fifo_pa && pipe_mem_pt[pe].desc_fifo_pa)
                {
                    bam_osal_debug(MSG_HIGH, "Client overrode descriptor FIFO address = 0x%016llX\n",pipe_cfg->desc_base_pa);
                }

                if (!pipe_cfg->data_base_pa)
                {
                    pipe_cfg->data_base_pa = pipe_mem_pt[pe].data_fifo_pa;
                    pipe_cfg->data_size = (pipe_mem_pt[pe].fifo_sizes >> 16) & 0xFFFF;
                }
                else if (pipe_cfg->data_base_pa != pipe_mem_pt[pe].data_fifo_pa && pipe_mem_pt[pe].data_fifo_pa)
                {
                    bam_osal_debug(MSG_HIGH, "Client overrode data FIFO address = 0x%016llX\n",pipe_cfg->data_base_pa);
                }

                break;
            }
        }

        if ( pipe_cfg->desc_base_pa == 0 ||
             pipe_cfg->data_base_pa == 0 )
        {
            return BAM_FAILED;
        }
    }

    return BAM_SUCCESS;
}

static boolean bam_pipe_cfg_uses_pipemem(bam_pipe_config_type *pipe_cfg)
{
    return (pipe_cfg->desc_base_pa >= bam_tgt_getpipememstartaddr() &&
            pipe_cfg->desc_base_pa < bam_tgt_getpipememendaddr()) ||
           (pipe_cfg->data_base_pa >= bam_tgt_getpipememstartaddr() &&
            pipe_cfg->data_base_pa < bam_tgt_getpipememendaddr());
}

BAM_API_NON_PAGED void bam_pipe_eot_handler(pipe_dev *pipe, bam_result_type *pollresult)
{
    uint32 endoffset,ackoffset; 
    bam_iovec_type *iovec;
    uint32 num_results = 0;
    bam_callback_func_type pipe_cb_func = NULL;
    bam_result_type *presult;

    // here it is assumed that only one IST is running for a given pipe
    // bam_pipe_poll will always provide pollresult location
    // BAM_INVOKE_CB_IN_POLL signifies user given callback is invoked in this
    // thread context ( in the polling thread )
    if(pollresult != NULL && pollresult->cb_mode != BAM_INVOKE_CB_IN_POLL)
    {
        pollresult->event = BAM_EVENT_INVALID;
        presult = pollresult;
    }
    else
    {
        presult = &pipe->desc_fifo[0].result;
        if ( !pollresult )
            presult->cb_mode = BAM_INVOKE_CB_DEFAULT;
        else
        {
            presult->cb_mode = BAM_INVOKE_CB_IN_POLL;
            pollresult->event = BAM_EVENT_INVALID;
    }
    }

    // lock the results structures in desc_fifo
    bam_osal_syncenter(&pipe->results_sync);

    // lock the pipe_dev structure
    bam_osal_syncenter(&pipe->sync);

    // Unlikely. However, if the pipe is already disabled, simply return
    if ( (pipe->bam->active_pipes_mask & ( 1 << pipe->pipe_num )) == 0 )
    {
        bam_osal_syncleave(&pipe->sync);
        bam_osal_syncleave(&pipe->results_sync);
        return;
    }

    // Get the last descriptor completed by the pipe
    endoffset = bamcore_pipe_getdescreadoffset(pipe->bam->bam_cfg.bam_va, pipe->pipe_num);
    // if we are polling, then check if we got back the nwd_done toggle
    while((pipe->acked_offset != endoffset) && 
          (pipe->irq_enabled || bamcore_pipe_nwd_done(pipe->bam->bam_cfg.bam_va,pipe->pipe_num)))
    {
        if( pipe->pipe_cfg.options & BAM_O_ACK_LAST_ONLY )
        {
            // client does not care about EOT,INT bits set on each individual descriptors
            // Update the acked_offset, but do not go through proccessed descriptors
            pipe->acked_offset = endoffset;
            ackoffset = ( endoffset ? endoffset : pipe->pipe_cfg.desc_size ) - BAM_DESC_SIZE; 
            // get the last processed descriptor. It's one before the SW_OFSET
            iovec = ((bam_iovec_type *) (pipe->pipe_cfg.desc_base_va + ackoffset));
            //size since last eot is set to zero, since we do not go though all the
            //processed descriptors 
            pipe->size_since_eot = 0; 
        }
        else
    {
        iovec = ((bam_iovec_type *) (pipe->pipe_cfg.desc_base_va + pipe->acked_offset));
        pipe->size_since_eot += iovec->buf_size;
        ackoffset = pipe->acked_offset;
        pipe->acked_offset += BAM_DESC_SIZE;
        if(pipe->acked_offset == pipe->pipe_cfg.desc_size)
        {
            pipe->acked_offset = 0;
        }
        } 
        // For descriptors that generated a CB event and client has it enabled via the pipe_options
        // we invoke the callback otherwise we continue acking descriptors
        if( pipe->pipe_cfg.options & BAM_O_ACK_LAST_ONLY )
        {
            presult->event = BAM_EVENT_DESC_PROCESSED;
        }
        else if ( (iovec->flags & (BAM_IOVEC_FLAG_EOT)) && (pipe->pipe_cfg.options & BAM_O_EOT))
       {
          presult->event = BAM_EVENT_EOT;
       }
       else if( (iovec->flags & BAM_IOVEC_FLAG_INT) && (pipe->pipe_cfg.options & BAM_O_DESC_DONE))
       {
          presult->event = BAM_EVENT_DESC_DONE;
       }
       else
       {
          continue; 
       }

       presult->data.xfer.iovec = *iovec;
       presult->cb_data = pipe->pipe_cb.data;
       presult->data.xfer.xfer_cb_data = pipe->desc_fifo[ackoffset / BAM_DESC_SIZE].user_data;
       pipe->desc_fifo[ackoffset / BAM_DESC_SIZE].user_data = NULL;
       presult->data.xfer.accum_size_bytes = pipe->size_since_eot;
       pipe->size_since_eot = 0;

       if(pollresult != NULL)
       {
            // Just return the first result. The caller is expected
            // to call the pollresult until it is empty.
            if( pollresult->cb_mode == BAM_INVOKE_CB_IN_POLL )
                ++num_results;
           break;
       }

       presult = &pipe->desc_fifo[++num_results].result;
        presult->cb_mode = BAM_INVOKE_CB_DEFAULT;
    }

    if(num_results != 0)
    {
        pipe_cb_func = pipe->pipe_cb.func;
    }

    bam_osal_syncleave(&pipe->sync);

    // invoke callbacks outside of critical section
    if (pipe_cb_func != NULL)
    {
        uint32 i;

        for (i = 0; i < num_results; i++)
        {
            BAM_OSAL_TRACE(BAM_PIPE_CB_START, pipe->pipe_num, i);
            pipe_cb_func(pipe->desc_fifo[i].result);
            BAM_OSAL_TRACE(BAM_PIPE_CB_END, pipe->pipe_num, i);
        }
    }

    bam_osal_syncleave(&pipe->results_sync);
    return;
}

void bam_pipe_isr(bam_dev *bam, uint32 pipe_num)
{
    uint32 status,mask;
    pipe_dev *pipe;
    bam_result_type result = {0};

    if(pipe_num >= bam->bam_caps.num_pipes)
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_isr:BAM:0x%x Invalid BAM pipe,max_pipe:%d,pipe_num:%d\n", 
                       bam->bam_cfg.bam_pa,bam->bam_caps.num_pipes,pipe_num);
        return;
    }
    pipe = bam->pipes[pipe_num];

    if(pipe == NULL)
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_isr:BAM:0x%x Pipe %d not intialized\n", 
                       bam->bam_cfg.bam_pa, pipe_num);
        return;
    }

    bam_osal_syncenter(&pipe->sync);
    // Record the last AHB Master access information for error reporting
    // before clearning the interrupts. If there is an error, clearing the
    // interrupt will cause the last AHB master access information to change
    result.data.error.address = bamcore_get_error_address(bam->bam_cfg.bam_va);
    result.data.error.data = bamcore_get_error_data(bam->bam_cfg.bam_va);
    result.data.error.bus_ctrls = bamcore_get_error_bus_ctrls(bam->bam_cfg.bam_va);
    // Check the pipe IRQ Status register 
    status = bamcore_pipe_getandclearirqstatus(bam->bam_cfg.bam_va, pipe_num);

    bam_osal_syncleave(&pipe->sync);

    // Get the mask of IRQs enabled
    mask = bamcore_pipe_getirqmask(bam->bam_cfg.bam_va,pipe_num);
    // Get the sources that we had enabled 
    status &= mask;

    if(status == 0)
    {
        return;
    }

    // Process the interrupt sources in order of frequency of occurrance.
    // Check for early exit opportunities.
   if ((status & (BAM_O_EOT | BAM_O_DESC_DONE)) != 0 )
   {
      bam_pipe_eot_handler(pipe,NULL);
      status &= ~(BAM_O_EOT | BAM_O_DESC_DONE);
      if (status == 0)
      {
         return;
      }
   }

    result.cb_data = pipe->pipe_cb.data;

   if ((status & BAM_O_WAKEUP) != 0)
   {
       if(pipe->pipe_cb.func)
       {
           result.event = BAM_EVENT_WAKEUP;
           pipe->pipe_cb.func(result);
       }
      status &= ~BAM_O_WAKEUP;
      if (status == 0)
      {
         return;
      }
   }

   if ((status & BAM_O_INACTIVE) != 0)
   {
       if(pipe->pipe_cb.func)
       {
           result.event = BAM_EVENT_INACTIVE;
           pipe->pipe_cb.func(result);
       }
       status &= ~BAM_O_INACTIVE;
      if (status == 0)
      {
         return;
      }
   }

   if ((status & BAM_O_OUT_OF_DESC) != 0)
   {
       if(pipe->pipe_cb.func)
       {
           result.event = BAM_EVENT_OUT_OF_DESC;
           pipe->pipe_cb.func(result);
       }
      status &= ~BAM_O_OUT_OF_DESC;
      if (status == 0)
      {
         return;
      }
   }

    if ( (status & BAM_O_HRESP_ERROR) != 0 )
    {
        pipe->last_error = BAM_EVENT_HRESP_ERROR;
        if ( pipe->pipe_cb.func )
        {
            result.event = BAM_EVENT_HRESP_ERROR;
            pipe->pipe_cb.func(result);
        }
        status &= ~BAM_O_HRESP_ERROR;
        if (status == 0)
        {
            return;
        }

    }

    if ( (status & BAM_O_RST_ERROR) != 0 )
    {

        pipe->last_error = BAM_EVENT_RST_ERROR;
        if ( pipe->pipe_cb.func)
        {
            result.event = BAM_EVENT_RST_ERROR;
            pipe->pipe_cb.func(result);
        }
        status &= ~BAM_O_RST_ERROR;
        if (status == 0)
        {
            return;
        }
    }

   // unlikely as pipe error irq is unsupported
   if ((status & BAM_O_ERROR) != 0)
   {
        pipe->last_error = BAM_EVENT_ERROR;
       if(pipe->pipe_cb.func)
       {
           result.event = BAM_EVENT_ERROR;
           pipe->pipe_cb.func(result);
       }
   }
   return;
}

// BAM ISR processor 
void bam_isr(void *ctxt)
{

   bam_dev* bam = ((bam_dev*)ctxt);
   uint32 irq_src;
   uint32 pipe_num;
   uint32 active_pipes_mask;

   BAM_OSAL_TRACE(BAM_ISR_ENTRY, 0, 0);

   active_pipes_mask = bam->active_pipes_mask;

   irq_src = bamcore_get_irq_sources(bam->bam_cfg.bam_va,
         bam->bam_tgt_cfg->ee,
         active_pipes_mask);

   while(irq_src != 0)
   {
      for (pipe_num = 0; irq_src != 0 && pipe_num < bam->bam_caps.num_pipes; pipe_num++)
      {
            // Is an IRQ pending for this channel? 
         if ((irq_src & 1) != 0 && ((1 << pipe_num) & active_pipes_mask))
         {
                // Yes, so call the channel handler for this channel
            BAM_OSAL_TRACE(BAM_PIPE_ISR_ENTRY, pipe_num, 1);
            bam_pipe_isr(bam, pipe_num);
            BAM_OSAL_TRACE(BAM_PIPE_ISR_EXIT, pipe_num, 1);
         }
            // Shift the next channel IRQ status bit down into bit 0 
         irq_src >>= 1;
      }

      active_pipes_mask = bam->active_pipes_mask;
        // Read the status again for any new interrupts
      irq_src = bamcore_get_irq_sources(bam->bam_cfg.bam_va,
            bam->bam_tgt_cfg->ee,
            active_pipes_mask);

        // If error was generated, no way to recover so just clear it
      if(irq_src & 0x80000000) 
      {
         uint32 mask;
         client_ctx *clnt_ctx;
         bam_result_type result = {0};

         bam_osal_syncenter(&bam->sync);

         // save error information registers before clearing the interrupt
         result.data.error.address = bamcore_get_error_address(bam->bam_cfg.bam_va);
         result.data.error.data = bamcore_get_error_data(bam->bam_cfg.bam_va);
         result.data.error.bus_ctrls = bamcore_get_error_bus_ctrls(bam->bam_cfg.bam_va);

         mask = bamcore_get_and_clear_irq_status(bam->bam_cfg.bam_va, bam->bam_cfg.bam_irq_mask);

            bam_osal_debug(MSG_HIGH, "BAM_ISR:BAM 0x%x ,mask = 0x%x\n",bam->bam_cfg.bam_pa,mask);

         if (mask & (BAM_IRQ_HRESP_ERR_EN | BAM_IRQ_ERR_EN))
         {
            result.event = BAM_EVENT_ERROR;
         }
         else if (mask & BAM_IRQ_TIMER_EN)
         {
            result.event = BAM_EVENT_INACTIVE;
         }
         else if (mask & BAM_IRQ_EMPTY_EN)
         {
            result.event = BAM_EVENT_EMPTY;
         }
         else
         {
            result.event = BAM_EVENT_INVALID;
         }
            // record the last error
            bam->last_event = result;
         // traverse through clients list to notify according to mask setting
         for (clnt_ctx = (client_ctx *)bam->client_list.next; 
              clnt_ctx != NULL && clnt_ctx != (client_ctx *)&bam->client_list; 
              clnt_ctx = (client_ctx *)clnt_ctx->list.next)
         {
             if ((mask & clnt_ctx->irq_mask) && clnt_ctx->bam_cb.func)
             {
                 result.cb_data = clnt_ctx->bam_cb.data;
                 result.data.error.status = mask & clnt_ctx->irq_mask;

                 clnt_ctx->bam_cb.func(result);
             }
         }

         irq_src&=~0x80000000; 
         bam_osal_syncleave(&bam->sync);
      }   
   } 
   bam_osal_israck(&bam->irqhandle);

   BAM_OSAL_TRACE(BAM_ISR_EXIT, 0, 0);
}

BAM_API_NON_PAGED static boolean bam_needs_init(const bam_target_config_type *tgt_cfg)
{
    if (BAM_OPT_SET(tgt_cfg->options,BAM_TGT_CFG_NO_INIT) || 
        BAM_OPT_SET(tgt_cfg->options,BAM_TGT_CFG_SW_DEBUG_INIT))
        return FALSE;
    // Always EE0 will initialize BAM except for multi-EE BAMs where the owner may not belong to EE0
    return (tgt_cfg->ee == 0 || tgt_cfg->sec_config != NULL) || 
            BAM_OPT_SET(tgt_cfg->options,BAM_TGT_CFG_FORCEINIT);
}

BAM_API_NON_PAGED static boolean bam_needs_secure_init(bam_dev *bam)
{
    // Security config required for the BAM that is also to be used
    // by the secure processor
    return BAM_OPT_SET(bam->bam_tgt_cfg->options,BAM_TGT_CFG_SECURE) &&
            (bam->bam_caps.is_secured != 0x0 || bam->bam_caps.num_ees > 1);
}

/**
 * @brief Inits the security configuration of the BAM
 *  
 * Inits the security configuration of the BAM. This can be directly called if 
 * and only if the execution environment is trustzone and does NOT intend to use 
 * the BAM. This API ensures that no resources are allocated to the BAM and only 
 * configures the security detail of that BAM 
 *
 * @param bam_cfg - Pointer to the BAM config
 *
 * @return Status of the operation
 *
*/
BAM_API_NON_PAGED bam_status_type bam_secure_init(bam_config_type *bam_cfg)
{
    const bam_target_config_type* sec_tgt_cfg;
    
    sec_tgt_cfg = bam_tgt_getcfg(bam_cfg->bam_pa);

    if(sec_tgt_cfg == NULL || sec_tgt_cfg->sec_config == NULL)
    {
        // Either target non-existent or the security config is not present
        bam_osal_debug(MSG_ERR, "bam_secure_init:BAM 0x%x Invalid BAM secure config params, sec_tgt_cfg:0x%X\n",
                       bam_cfg->bam_pa,sec_tgt_cfg);
        return BAM_FAILED;
    }
    
    // Configure BAM Pipe secure config init
    if (BAM_SUCCESS != bamcore_pipe_secure_init(bam_cfg->bam_va, sec_tgt_cfg))
    {
        bam_osal_debug(MSG_ERR, "bam_secure_init:BAM 0x%x Pipe secure config Failed  sec_tgt_cfg:0x%x\n", 
                       bam_cfg->bam_pa,sec_tgt_cfg);
        return BAM_FAILED;
    }
    
    // Configure BAM device secure init & control group VMID
    if(BAM_SUCCESS != bamcore_secure_init(bam_cfg->bam_va, sec_tgt_cfg))
    {
        bam_osal_debug(MSG_ERR, "bam_secure_init:BAM 0x%x Device secure init Failed, sec_tgt_cfg:0x%X\n", 
                       bam_cfg->bam_pa, sec_tgt_cfg);
        return BAM_FAILED;
    }
    // Configure BAM Pipe Super group 
    if(BAM_SUCCESS != bamcore_secure_set_sg( bam_cfg->bam_va, sec_tgt_cfg))
    {
        bam_osal_debug(MSG_ERR, "bam_secure_init:BAM 0x%x Pipe super group config Failed, sec_tgt_cfg:0x%x\n", 
                       bam_cfg->bam_pa,sec_tgt_cfg);
        return BAM_FAILED;
    }
    
    return BAM_SUCCESS;
}

/**
 * @brief Initialize BAM driver .
 * Initializes the BAM driver data structures and pipe memory,
 *
 *  @return Status of the operation
 *
 * @note This API is not thread-safe
*/
bam_status_type bam_drv_init(void)
{
    // if bam_drv_init is already done in this EE, return 
    if ( bam_drv_init_done ) 
        return BAM_SUCCESS;

    // in certain envs bam_osal_init loads target_config structure, so
    // call bam_osal_init first
    bam_osal_init();

    // Allocate the driver level sync object
    bam_osal_syncinit(&bam_drv_sync);
        
    if (bam_tgt_getpipememstartaddr() != 0x0)
    {
        bam_osal_clock_ctl_init(&pipe_mem_clock_ctl, BAM_PIPE_MEM_CLOCK_ID);
        bam_osal_clock_ctl_enable(&pipe_mem_clock_ctl);
        bam_tgt_initpipemem();
        bam_osal_clock_ctl_disable(&pipe_mem_clock_ctl);
    }
    bam_drv_init_done = 1;
    return BAM_SUCCESS;
}

/**
 * @brief Deinitialize BAM driver .
 * Deinitializes the BAM driver data structures,
 *
 *  @return Status of the operation
 *
 * @note This API is not thread-safe
*/

bam_status_type bam_drv_deinit(void)
{
    // if bam_drv is already de-initialized return
    if ( bam_drv_init_done == 0 ) 
        return BAM_SUCCESS;

    if (bam_tgt_getpipememstartaddr() != 0x0)
    {
        bam_osal_clock_ctl_deinit(&pipe_mem_clock_ctl);
    }
    bam_osal_syncdestroy(&bam_drv_sync); 
    bam_osal_deinit();
    bam_drv_init_done = 0;

    return BAM_SUCCESS;
}

/**
 * @brief Initialize BAM and configures it.
 * Initializes the BAM. Allocates a handle to the BAM,
 * which the client is expected to use for future calls to the driver.
 *
 * @param *handle - Pointer to be allocated for the BAM handle
 * @param bam_cfg  - Configuration data for BAM. This is peripheral specific
 * @param bam_cb - Callback to get any top level BAM events
 *
 * @return A handle to the BAM Device if successful
 *
*/

BAM_API_NON_PAGED bam_handle bam_init(bam_config_type *bam_cfg, bam_callback_type *bam_cb)
{
    bam_dev *bam = NULL, **free_bam = NULL;
    bam_osal_meminfo bam_mem = {0};
    client_ctx *clnt_ctx = NULL; 
    uint32 i=0, install_isr = 0; 

    /** Internals
    -- Will set up
       1. BAM_CTRL
       2. BAM_DESC_CNT_TRSHLD
      3. BAM_IRQ_SRCS_MSK --> if dwBamIRQ is provided we will set the mask for 
         BAM_IRQ. Pipe IRQ will be configured as and when needed
    -- Return error if Config was provided and the EE is untrusted
    */
    const bam_target_config_type* tgt_cfg;
    tgt_cfg = bam_tgt_getcfg(bam_cfg->bam_pa);

    if(tgt_cfg == NULL)
    {
        bam_osal_debug(MSG_ERR, "bam_init:BAM 0x%x Does not exist in target config, tgt_cfg:0x%x\n", bam_cfg->bam_pa, tgt_cfg);
        return 0;
    }
   
    if (BAM_SUCCESS != bam_drv_init())
        return 0;

    bam_mem.handle = NULL;
    if ( bam_cfg->bam_va == 0x0)
    {
        bam_mem.pa = bam_cfg->bam_pa;
        bam_mem.va = 0;
        bam_mem.size = tgt_cfg->size;
        bam_osal_mallocmemregion(&bam_mem, bam_mem.size);
        if (bam_mem.handle == NULL)
        {
            bam_osal_debug(MSG_ERR, "bam_init:BAM 0x%x PA to VA mapping failed\n", bam_cfg->bam_pa);
            return 0;
        }
        bam_cfg->bam_va = bam_mem.va;
    }
    
    bam_osal_syncenter(&bam_drv_sync);
    // If bam handle has never been allocated, allocate it 
    if(bams == NULL)
    {
	bams = (bam_dev **)bam_osal_malloc(sizeof(bam_dev *) * bam_tgt_getnumdevs());
	if(bams == NULL)
	{
	    bam_osal_freememregion(&bam_mem);
	    bam_osal_syncleave(&bam_drv_sync);
            bam_osal_debug(MSG_ERR, "bam_init:BAM 0x%x Buffer allocation failed for BAM device pool size:%d bytes\n",
                           bam_cfg->bam_pa,(sizeof(bam_dev *) * bam_tgt_getnumdevs()));
	    return (bam_handle)NULL;
        }
        memset(bams,0x0,sizeof(bam_dev *) * bam_tgt_getnumdevs());
    }
    // Look for already configured devices. We look through array of possible device objects for existing devices,
    // and if none are found, the first free spot in the array is used to store the new device object. This is 
    // inneficient if we have many supported devices. In future we can sort the array to have the used device objects,
    // always @ the beginning of the array 
    for(i = 0;i < bam_tgt_getnumdevs() ; i++)
    {
	if(bams[i]==NULL)
        {
	   if(free_bam == NULL)
           {
	       free_bam = &bams[i]; 
           }
           continue; 
        }

        // If bam device already exists, verify new config matches that of current one
        if( bams[i]->bam_cfg.bam_pa == bam_cfg->bam_pa)
        {
	    if(bams[i]->bam_cfg.bam_irq == bam_cfg->bam_irq &&
               bams[i]->bam_cfg.options == bam_cfg->options &&
               bams[i]->bam_cfg.sum_thresh == bam_cfg->sum_thresh &&
                BAM_OPT_SET(tgt_cfg->options,BAM_TGT_CFG_SHARABLE) )
            {
		        bam=bams[i];  
                break;
            }
            else
            {
		bam_osal_debug(MSG_HIGH, "bam_init:BAM 0x%x: Client config doesn't match for shared device\n", bam_cfg->bam_pa); 
                goto err; 
            }
        }
    }
   
    // If bam is not allocated make sure there is a free spot for a new device in the BAMs array.
    if(free_bam==NULL && bam==NULL)
    {
	bam_osal_debug(MSG_HIGH, "bam_init:BAM 0x%x: Couldn't find a free slot or BAMs array possibly corrupted\n", 
                       bam_cfg->bam_pa);
        goto err;
    }

    // Create a new client config
    if((clnt_ctx = bam_osal_malloc(sizeof(client_ctx))) == NULL )
    {
	bam_osal_debug(MSG_HIGH, "bam_init:BAM 0x%x: Unable to allocate client context\n", bam_cfg->bam_pa); 
        bam=NULL; /* Clear so that we do not free the shared bam */
        goto err;
    }
   
    // save bam top interrupt mask in client's context
    clnt_ctx->irq_mask = bam_cfg->bam_irq_mask;
    if (bam_cb != NULL)
	clnt_ctx->bam_cb = *bam_cb; 

    // Allocate a bam device, if we havent found an existing one and initialize the bam. Also install the ISR
    // IF the device exists, we do not initialize it because we could affect the other clients 
    if(bam==NULL)   
    {
	bam =bam_osal_malloc(sizeof(bam_dev));
        if ( NULL == bam )
        {
	    bam_osal_debug(MSG_ERR, "bam_init:BAM 0x%x: Allocating memory for bam_dev failed size:%d bytes\n", 
                           bam_cfg->bam_pa,sizeof(bam_dev));
	    goto err;
        }
        memset(bam,0x0,sizeof(bam_dev));
        bam_osal_syncinit(&bam->sync);
        bam->client_list.next = &bam->client_list; 
        bam->client_list.prev = &bam->client_list; 

        *free_bam=bam;
        // Store the bam_cfg in the device object
        bam->bam_cfg = *bam_cfg; 
        bam->bam_mem = bam_mem;
        bam->bam_tgt_cfg = tgt_cfg; 

        // If LAZY init is enabled for this HW, bam_init will only initialize 
        // sw handles and not the actual HW. THe HW will be init when client calls bam_reset. Note this should only be 
        // set for non TRUST EE bams
        if(BAM_OPT_NOT_SET(tgt_cfg->options,BAM_TGT_CFG_LAZYINIT))
	{
	    //Enable the hardware
	    if(bam_needs_init(tgt_cfg)&& ! bamcore_bam_enabled(bam_cfg->bam_va, tgt_cfg->ee))
	    {
		bamcore_init(bam_cfg->bam_va, bam_cfg->bam_irq, bam_cfg->bam_irq_mask, bam_cfg->sum_thresh, tgt_cfg);
	    }
            if(bamcore_check(bam->bam_cfg.bam_va,&bam->bam_caps,bam->bam_tgt_cfg->ee) != BAM_SUCCESS)
            {
                // Note: if the clocks were off, the num_pipes returned by bamcore_check could be zero
                // In that case, we may run into issues if the pipes were used later on by a driver 
                // which has BAM_TGT_CFG_SW_DEBUG_INIT set.
                if (BAM_OPT_NOT_SET(tgt_cfg->options,BAM_TGT_CFG_SW_DEBUG_INIT))
                {
		    //Either enable failed or BAM isnt enabled by EE0
		    bam_osal_debug(MSG_ERR, "bam_init:BAM 0x%x: Core Check Failed\n", bam_cfg->bam_pa);
		    goto err;
                }
	    }
	    if(bam_needs_secure_init(bam))
            {
		bam_secure_init(bam_cfg);
            }
	    bam->pipes = (pipe_dev **)bam_osal_malloc(sizeof(pipe_dev*)*bam->bam_caps.num_pipes);
	    if(bam->pipes == NULL)
            {
		bam_osal_debug(MSG_ERR, "bam_init:BAM 0x%x: pipe dev pool allocation failed, size:%d bytes\n",
                               bam_cfg->bam_pa,(sizeof(pipe_dev*)*bam->bam_caps.num_pipes));
		goto err;
            }
	    memset(bam->pipes,0x0,sizeof(pipe_dev*)*bam->bam_caps.num_pipes);
	}
	if(bam_cfg->bam_irq != BAM_IRQ_INVALID)
	{
	    install_isr = 1;
	    bam->irq_enabled = 1;
	    bam->active_pipes_mask = bam_cfg->bam_irq_mask ? 0x80000000 : 0;
	}
        //  Attach the client to the bam device
	if(insertHead(&bam->client_list, (list_t*)clnt_ctx))
        {
	    bam_osal_debug(MSG_ERR, "bam_init:BAM 0x%x:Failed to attach client to BAM device, Possible list corruption\n", bam_cfg->bam_pa);
	    goto err; 
        }
    } //end of bam==Null
    else
    {
	// if bam is already initialized, cleanup the bam_mem that might have been allocated, 
        // since we do not need it
        bam_osal_freememregion(&bam_mem);
	bam_osal_syncenter(&bam->sync);
   
        // Attach the client to the bam device 
	if(insertHead(&bam->client_list, (list_t*)clnt_ctx))
	{
	    bam_osal_debug(MSG_ERR, "bam_init:BAM 0x%x: Failed to attach client to bam device, Possible list corruption\n", bam_cfg->bam_pa);
	    bam_osal_syncleave(&bam->sync);
	    goto err; 
        }
	if (bam->bam_cfg.bam_irq_mask != (bam->bam_cfg.bam_irq_mask | clnt_ctx->irq_mask))
        {
	    bam->bam_cfg.bam_irq_mask |= clnt_ctx->irq_mask;
	    bam->active_pipes_mask |= 0x80000000;
	    bamcore_setirqmode(bam->bam_cfg.bam_va, bam->bam_tgt_cfg->ee, 1, bam->bam_cfg.bam_irq_mask);
        }
	if (bam->bam_cfg.bam_irq == BAM_IRQ_INVALID && bam_cfg->bam_irq != BAM_IRQ_INVALID)
	{
	    bam->bam_cfg.bam_irq = bam_cfg->bam_irq;
	    bam->irq_enabled = 1;
	    install_isr = 1;
	}
	bam_osal_syncleave(&bam->sync);
    }
   
    /* Link the bam context with the client context */  
    clnt_ctx->bam = bam; 

    /* Increment succesfully allocated device count */
    num_allocated_devices++;
    goto exit;

err:
    if(clnt_ctx)
	bam_osal_free(clnt_ctx); 

    // don't clear shared bam
    if (bam && bam->client_list.next == &bam->client_list)
    {
	if (bam->pipes)
	    bam_osal_free(bam->pipes);

	bam_osal_free(bam);
    }
    if(free_bam)
	*free_bam=NULL;
   
    if(bam_mem.handle != NULL)
	bam_osal_freememregion(&bam_mem);

    clnt_ctx=NULL;
    bam = NULL;
    install_isr = 0;

exit:

    bam_osal_syncleave(&bam_drv_sync);
    // intall isr outside of critical section to avoid race conditions
    if (install_isr)
	bam_osal_isrinstall(&bam->irqhandle,bam->bam_cfg.bam_irq,bam_isr,(void *)bam);

    return (bam_handle)clnt_ctx;
}

/**
 * @brief Uninits the BAM if required & release the IRQ if allocated. Finally
 *        releases any allocations if made by the driver
 * Uninitializes the BAM HW if mentioned and frees up resources that are
 * allocated by the driver (IRQ, memory etc). This should be the last API to
 * be called, and any pipe disable must happen before this API is called.
 *
 * @param *handle - Pointer to the BAM handle
 * @param disable_bam  - Flag to indicate if the BAM HW must be disabled
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_deinit(bam_handle handle, uint8 disable_bam)
{
    uint32 i; 
    bam_dev *bam = NULL;
    bam_status_type status = BAM_FAILED;

    bam_osal_syncenter(&bam_drv_sync);
    if(BAM_IS_INVALID_CLIENT_HANDLE(handle))
    {
        bam_osal_debug(MSG_ERR,"bam_deinit:Bad handle,  handle:0x%X\n", handle);
	goto exit; 
    }
    // Remove the client context from the list
    if(removeHead((list_t*)handle))
    {
	bam_osal_debug(MSG_ERR,"bam_deinit:Client context is corrupted handle:0x%X\n", (list_t*)handle);
	goto exit; 
    }
    bam = BAM_GET_DEV_FROM_HANDLE(handle);
    bam_osal_free((void*)handle);
    // If device has no other clients, clean up, otherwise we do not touch the bam, 
    // as other clients are still using it
    if(bam->client_list.next != &bam->client_list)
    {
       status = BAM_SUCCESS; 
       goto exit; 
    }
    // Uninstall the interrupt
    bam_osal_isruninstall(&bam->irqhandle);
    if(bam_needs_init(bam->bam_tgt_cfg) && disable_bam)
    {
	bamcore_deinit(bam->bam_cfg.bam_va); 
    }

    // Find the device pointer to our bam and clear it
    for(i = 0; i < bam_tgt_getnumdevs(); i++)
    {
	if(bams[i] == bam)
	{
	    bams[i] = NULL;
            break;
	}
    }
    bam_osal_syncdestroy(&bam->sync);

    // sanity test : all pipes de-initialized already?
    if ( bam->active_pipes_mask )
    {
        bam_osal_debug(MSG_ERR, "bam_deinit:BAM 0x%x:Not all pipes were de-initialized: active pipes=%d\n", 
                       bam->bam_cfg.bam_pa,bam->active_pipes_mask);
    }
    bam_osal_free(bam->pipes);
    if(NULL != bam->bam_mem.handle)
    {
	bam_osal_freememregion(&(bam->bam_mem));
    }
    bam_osal_free(bam); 
    num_allocated_devices--;
    if ( num_allocated_devices == 0 )
    {
        bam_osal_free(bams);
        bams = NULL;
    }
    status = BAM_SUCCESS; 
exit:
    bam_osal_syncleave(&bam_drv_sync);
    return status;
}

/**
 * @brief Simply resets the BAM hw and restores the config.
 *
 * Resets the BAM hw and restores the config provided via bam_init API. This
 * can be used to re-initialize a bam after the core was reset
 *
 * @param *handle - Pointer to the BAM handle
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_reset(bam_handle handle)
{
    bam_dev *bam = NULL;
    bam_status_type status = BAM_FAILED;

    //Sanity check
    if(BAM_IS_INVALID_CLIENT_HANDLE(handle))
    {
        bam_osal_debug(MSG_ERR, "bam_reset:Invalid BAM handle, handle:0X%x\n", handle);
        return BAM_FAILED;
    }

    bam = BAM_GET_DEV_FROM_HANDLE(handle);

    bam_osal_syncenter(&bam->sync);
        //Enable the hardware
    if(bam_needs_init(bam->bam_tgt_cfg))
    {
        bamcore_init(bam->bam_cfg.bam_va, bam->bam_cfg.bam_irq, bam->bam_cfg.bam_irq_mask, bam->bam_cfg.sum_thresh, bam->bam_tgt_cfg);
    }

    if(bamcore_check(bam->bam_cfg.bam_va,&bam->bam_caps,bam->bam_tgt_cfg->ee) != BAM_SUCCESS)
    {
        bam_osal_debug(MSG_ERR, "bam_reset:BAM 0x%x: BamCore check failed\n", bam->bam_cfg.bam_pa);
        goto exit;
    }
    if(bam_needs_secure_init(bam))
    {
        bam_secure_init(&bam->bam_cfg);
    }
    bam->active_pipes_mask = bam->bam_cfg.bam_irq_mask ? 0x80000000 : 0x0;
    if(bam->pipes == NULL)
    {
        bam->pipes = (pipe_dev **)bam_osal_malloc(sizeof(pipe_dev*)*bam->bam_caps.num_pipes);
        if(bam->pipes == NULL)
        {
            bam_osal_debug(MSG_ERR,"bam_reset:BAM 0x%x: pipe device poll allocation failed, size:%d bytes\n",bam->bam_cfg.bam_pa,(sizeof(pipe_dev*)*bam->bam_caps.num_pipes));
            goto exit;
        } 
        memset(bam->pipes,0x0,sizeof(pipe_dev*)*bam->bam_caps.num_pipes);
    }
    status = BAM_SUCCESS;
exit:
    bam_osal_syncleave(&bam->sync);

    return status;
}


/**
 * @brief Enables/Disables the IRQ mode of the BAM Top Level
 *
 * Enables/Disables the IRQ mode of the BAM Top Level.
 *
 * @param *handle - Pointer to the BAM handle
 * @param irq_en  - Enable (1)/ Disable (0) the irq mode
 * @param irq_mask - IRQs to be enabled for the BAM
 *
 * @return Status of the operation
 *
 * @note This API only sets the IRQ mode for the BAM and NOT the individual pipes
*/
bam_status_type bam_setirqmode(bam_handle bamhandle, uint32 irq_en,uint32 irq_mask)
{
    bam_dev *bam = NULL;
    client_ctx *clnt_ctx = (client_ctx *)bamhandle;
    uint32 mask = 0;

    //Sanity check
    if(BAM_IS_INVALID_CLIENT_HANDLE(bamhandle))
    {
        bam_osal_debug(MSG_ERR, "bam_setirqmode:Invalid BAM handle, handle:0X%Xs\n", bamhandle);
        return BAM_FAILED;
    }

    bam = BAM_GET_DEV_FROM_HANDLE(bamhandle);
    bam_osal_syncenter(&bam->sync);

    // update client's interrupt mask
    clnt_ctx->irq_mask = irq_en ? irq_mask : 0;

    // traverse through clients list to aggregate interrupt masks
    for (clnt_ctx = (client_ctx *)bam->client_list.next; 
         clnt_ctx != NULL && clnt_ctx != (client_ctx *)&bam->client_list; 
         clnt_ctx = (client_ctx *)clnt_ctx->list.next)
    {
        mask |= clnt_ctx->irq_mask;
    }

    if(bam->bam_cfg.bam_irq_mask != mask)
    {
        bam->irq_enabled = mask ? 1 : 0;
        bam->bam_cfg.bam_irq_mask = mask;
        bam->active_pipes_mask |= mask ? 0x80000000 : 0;
        bamcore_setirqmode(bam->bam_cfg.bam_va, bam->bam_tgt_cfg->ee, bam->irq_enabled, mask);
    }
    bam_osal_syncleave(&bam->sync);
    return BAM_SUCCESS;
}
/**
 * @brief Free the Mapping of Peer BAM VA.
 *
 * This API will free the Mapping of Peer BAM VA
 *
 * @param pipe             Bam Pipe Handle
 * @param pipe_initmask       Mask of intialized BAM pipes.
 *
 * @return                 None.
 */
void bam_unmap_peer_pa(pipe_dev *pipe, uint32 pipe_initmask)
{
    bam_dev *bam;
    pipe_dev *pipe_offset;
    uint32 index, id = 0;

    bam = pipe->bam;
    // Check whether pipe has mapped va or not
    if(0 != pipe->bam_peer_va)
    {
        for (index = 0; (pipe_initmask != 0 && index < BAM_MAX_PIPES); pipe_initmask >>= 1,index++)
        {
            if (1 & pipe_initmask)
            {
                pipe_offset = bam->pipes[index];
                if((NULL !=pipe_offset) && (pipe_offset->pipe_cfg.peer_base_pa == pipe->pipe_cfg.peer_base_pa))
                {
                    // Can not free the BAM PA mapping. Initialized pipe is present with same BAM PA
                    if(pipe_offset->bam_peer_va == pipe->bam_peer_va)
                    {
                        id++;
                        break; 
                    }
                }
            }
        }
        // Unmapped only if no pipe is intialized with same BAM PA
        if (!id)
            bam_osal_freememregion(&pipe->bam_mem_peer);
    }
    return;
}

/**
 * @brief his api is doing mapping of PA to VA
 *
 * This API will map peer bam physical base address.
 *
 * @param pipe   Pipe device handle
 *
 * @return       Peer BAM va base address.
 *
 */
void bam_map_peer_pa(pipe_dev *pipe)
{
    bam_osal_meminfo bam_mem;
    bam_dev *bam = NULL;
    pipe_dev *pipe_offset = NULL;
    uint32 index, pipe_initmask = 0x7FFFFFFF;

    bam = pipe->bam;
    // Mask BAM pipe bit 31
    pipe_initmask &= bam->active_pipes_mask;
    // If bam device already exists, get the peer bam vitual address
    for(index = 0; index<bam_tgt_getnumdevs(); index++)
    {
        if(NULL != bams[index])
        {
            if(bams[index]->bam_cfg.bam_pa == pipe->pipe_cfg.peer_base_pa)
            {
                pipe->bam_peer_va = bams[index]->bam_cfg.bam_va;
                break;
            }
        }
    }

    // Look for already map VA for same BAM device
    if(0 == pipe->bam_peer_va)
    {
        for (index = 0;(pipe_initmask != 0 && index < BAM_MAX_PIPES); index++, pipe_initmask >>= 1)
        {
            if (((1 & pipe_initmask) != 0)&(index != pipe->pipe_num))
            {
                pipe_offset = bam->pipes[index];
                if((NULL != pipe_offset) && (pipe_offset->pipe_cfg.mode == BAM_MODE_BAM2BAM))
                {
                    if((pipe_offset->pipe_cfg.peer_base_pa == pipe->pipe_cfg.peer_base_pa) && (0 != pipe_offset->bam_peer_va))
                    {
                        pipe->bam_peer_va = pipe_offset->bam_peer_va;
                        pipe->bam_mem_peer = pipe_offset->bam_mem_peer;
                        break;
                    }
                }
            }
        }
    }
    // Not mapped yet so map virtual address for BAM
    if(0 == pipe->bam_peer_va)
    {
        bam_mem.handle = NULL;
        bam_mem.pa = pipe->pipe_cfg.peer_base_pa;
        bam_mem.va = 0;
        bam_mem.size = BAM_MAX_MMAP;
        bam_osal_mallocmemregion(&bam_mem, bam_mem.size);
        if(bam_mem.handle != NULL)
        {
            pipe->bam_peer_va = bam_mem.va;
            pipe->bam_mem_peer = bam_mem;
        }
    }
    return;
}


/**
 * @brief Initializes pipe pipe_num in the BAM based on the config provided.
 *
 * Enables pipe pipe_num in the BAM based on the pipe_cfg provided. Returns
 * a handle to the pipe allocated to the client to be used for future pipe
 * operations
 *
 * @param *bamhandle - Pointer to the handle to the BAM from which the pipe
 *                     is to be allocated
 * @param pipe_num - Pipe to be allocated in the bam.
 * @param bam_cfg  - Configuration data for the pipe. This is connection specific
 * @param pipe_cb  - Callback to be notified of Pipe level events
 *
 * @return A handle to the pipe allocated successfully
 *
*/

bam_handle bam_pipe_init(bam_handle bamhandle, uint32 pipe_num, bam_pipe_config_type *pipe_cfg, bam_callback_type *pipe_cb)
{
    bam_dev *bam = NULL;
    pipe_dev *pipe = NULL;
    bam_status_type status = BAM_FAILED;

    //Sanity check
    if( BAM_IS_INVALID_CLIENT_HANDLE(bamhandle) || pipe_cfg == NULL)
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_init: Invalid BAM Pipe init params, pipe:%d, pipe_cfg:0X%X, handle:0X%X\n",
                       pipe_num, pipe_cfg, bamhandle);
        return (bam_handle)NULL;
    }


    bam = BAM_GET_DEV_FROM_HANDLE(bamhandle);

    if (bam_validate_pipe_cfg(bam, pipe_num, pipe_cfg) != BAM_SUCCESS)
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_init:BAM 0x%x:Invalid BAM Pipe init config, pipe_cfg:0x%x,pipe:%d\n", 
                       bam->bam_cfg.bam_pa,pipe_cfg,pipe_num);
        return (bam_handle)NULL;
    }

    bam_osal_syncenter(&bam->sync);
    if( (bam->active_pipes_mask & 1<< pipe_num) || (bam->bam_caps.num_pipes <= pipe_num))
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_init:BAM 0x%x: Pipe already active (0x%x) or invalid pipe number (%d)\n",
                       bam->bam_cfg.bam_pa,bam->active_pipes_mask,pipe_num);
        goto exit;
    }
    if ((BAM_IS_INVALID_DESC_FIFO_SIZE(pipe_cfg->desc_size)) || (BAM_IS_DESC_FIFO_ALIGNED(pipe_cfg->desc_base_pa)))
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_init:BAM 0x%x: Desc fifo or Desc fifo size not aligned to 8 byte boundary Desc pa:0x%x, Desc fio size:0x%x\n",
                       bam->bam_cfg.bam_pa, pipe_cfg->desc_base_pa, pipe_cfg->desc_size);
        goto exit;
    }

    pipe = (pipe_dev*)bam_osal_malloc(sizeof(pipe_dev));
    if(pipe == NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_init:BAM 0x%x: Pipe dev allocation failed, size:%d bytes\n",
                       bam->bam_cfg.bam_pa,sizeof(pipe_dev));
        goto exit;
    }
    memset(pipe,0x0,sizeof(pipe_dev));
    if (bam_pipe_cfg_uses_pipemem(pipe_cfg) && bam_osal_clock_ctl_enable(&pipe_mem_clock_ctl) != BAM_SUCCESS)
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_init:BAM 0x%x pipe mem clock could not be enabled\n", bam->bam_cfg.bam_pa);
        goto err;
    }

    status = BAM_SUCCESS;
    if ( BAM_OPT_NOT_SET(pipe_cfg->options,BAM_O_PIPE_NO_INIT) )
    {
	status = bamcore_pipe_init(bam->bam_cfg.bam_va,pipe_num,pipe_cfg,(pipe_cb==0)?0:1,bam->bam_tgt_cfg->ee);
	if(status == BAM_SUCCESS)
            pipe->toggle_at_reset = bamcore_pipe_get_toggle(bam->bam_cfg.bam_va,pipe_num);
    }
    if(status == BAM_SUCCESS)
    {
        pipe->bam = bam;
        pipe->pipe_num = pipe_num;
        pipe->pipe_cfg = *pipe_cfg;
        bam_osal_syncinit(&pipe->sync);
        bam_osal_syncinit(&pipe->results_sync);
        if(pipe_cb != NULL)
        {
            pipe->pipe_cb = *pipe_cb;
            pipe->irq_enabled = 1;
        }
        bam->active_pipes_mask |= (1<<pipe_num);
        
        if(pipe_cfg->mode == BAM_MODE_SYSTEM)
        {
            pipe->desc_fifo = (desc_fifo_type *)bam_osal_malloc((sizeof(desc_fifo_type) * pipe->pipe_cfg.desc_size)/BAM_DESC_SIZE);
            if(pipe->desc_fifo == NULL)
            {
                bam_osal_debug(MSG_ERR, "bam_pipe_init:BAM 0x%x: Desc FIFO allocation failed, size:%d bytes\n",
                               bam->bam_cfg.bam_pa,(sizeof(desc_fifo_type) * pipe->pipe_cfg.desc_size)/BAM_DESC_SIZE);
                 goto err;
            }
            memset((void*)pipe->desc_fifo,0,sizeof(desc_fifo_type)*(pipe->pipe_cfg.desc_size)/BAM_DESC_SIZE);
        }
        pipe->last_offset = 0xFFFFFFFF; 
        bam->pipes[pipe_num]=pipe;
    }
    else
    {
        goto err;
    }
   
    // Map the descriptor VA if it hasn't been mapped and if the options does not 
    // contain no-init
    if ( BAM_OPT_NOT_SET(pipe->pipe_cfg.options,BAM_O_PIPE_NO_INIT) )
    {
    if(pipe->pipe_cfg.desc_base_va == 0)
    {
        pipe->pipe_desc_mem.pa=pipe_cfg->desc_base_pa; 
        pipe->pipe_desc_mem.va=0;
        pipe->pipe_desc_mem.size=pipe_cfg->desc_size;

        bam_osal_mallocmemregion(&pipe->pipe_desc_mem, pipe->pipe_desc_mem.size);

        if(pipe->pipe_desc_mem.handle == NULL)
        {
           bam_osal_free(pipe->desc_fifo);
                bam_osal_debug(MSG_ERR,"bam_pipe_init:BAM 0x%x: Descriptor fifo PA to VA mapping failed\n", 
                               pipe_cfg->desc_base_pa);
           goto err;
         }
        pipe->pipe_cfg.desc_base_va = pipe->pipe_desc_mem.va;
     }

        // clear the desc fifo
        memset((void*)pipe->pipe_cfg.desc_base_va,0,pipe->pipe_cfg.desc_size);

        // Map the Data VA if it hasn't mapped
     if((pipe_cfg->mode == BAM_MODE_BAM2BAM) && (pipe->pipe_cfg.data_base_va == 0))
     {
        pipe->pipe_data_mem.pa=pipe_cfg->data_base_pa;
        pipe->pipe_data_mem.va=0;
        pipe->pipe_data_mem.size=pipe_cfg->data_size;
        
        bam_osal_mallocmemregion(&pipe->pipe_data_mem, pipe->pipe_data_mem.size);
        if(pipe->pipe_data_mem.handle == NULL)
        {
            pipe->pipe_cfg.data_base_va = 0;
                bam_osal_debug(MSG_ERR,"bam_pipe_init:BAM 0x%xl: Data fifo PA to VA mapping failed\n", 
                               pipe_cfg->data_base_pa);
            goto exit;
        }
            pipe->pipe_cfg.data_base_va = (bam_vaddr)pipe->pipe_data_mem.va;
     }

     }

    // map the peer bam PA to VA
    if(pipe_cfg->mode == BAM_MODE_BAM2BAM)
        bam_map_peer_pa(pipe);
    else
        pipe->bam_peer_va = 0;

    goto exit;
err:
    bam_osal_free(pipe);
    pipe=NULL;

exit:
    bam_osal_syncleave(&bam->sync);
    return (bam_handle)pipe;
}

/**
 * @brief Uninitializes the pipe.
 *
 * Uninitializes a pipe and disables it. No further operations
 * can be performed on the pipe, and the handle is invalidated.
 *
 * @param *pipehandle - handle to the pipe to be uninitialized
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_pipe_deinit(bam_handle pipehandle)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    bam_dev *bam;

    if(pipe==NULL)
        return BAM_FAILED;

    bam = pipe->bam;
    if ( bam == NULL )
        return BAM_FAILED;

    bam_osal_syncenter(&bam->sync);

    if ( BAM_OPT_NOT_SET(pipe->pipe_cfg.options,BAM_O_PIPE_NO_INIT) )
    {
    bamcore_pipe_deinit(bam->bam_cfg.bam_va,pipe->pipe_num,(pipe->pipe_cb.func==NULL)?1:0,bam->bam_tgt_cfg->ee);
    }
    if (bam_pipe_cfg_uses_pipemem(&pipe->pipe_cfg) &&
        bam_osal_clock_ctl_disable(&pipe_mem_clock_ctl) != BAM_SUCCESS)
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_deinit:BAM 0x%x:pipe mem clock could not be disabled,pipe:%d\n", 
                       bam->bam_cfg.bam_pa,pipe->pipe_num);
    }

    bam->pipes[pipe->pipe_num] = NULL;
    bam->active_pipes_mask &= ~(1<<pipe->pipe_num);
    
    // Unmap the memregion if it was mapped
    if(pipe->pipe_desc_mem.va != 0)
        bam_osal_freememregion(&pipe->pipe_desc_mem);

    if(pipe->pipe_data_mem.va != 0)
        bam_osal_freememregion(&pipe->pipe_data_mem);

    if(pipe->pipe_cfg.mode == BAM_MODE_BAM2BAM)
        bam_unmap_peer_pa(pipe, bam->active_pipes_mask);

    bam_osal_syncdestroy(&pipe->sync);
    bam_osal_syncdestroy(&pipe->results_sync);

    if(pipe->pipe_cfg.mode == BAM_MODE_SYSTEM)
        bam_osal_free(pipe->desc_fifo);

    bam_osal_free(pipe);
    pipe = NULL;
    bam_osal_syncleave(&bam->sync);

    return BAM_SUCCESS;
}

/**
 * @brief Enables a pipe.
 *
 * Enables a pipe for transfers
 *
 * @param *pipehandle - handle to the pipe to be enabled
 *
 * @return Status of the operation
 *
 * @dependencies bam_pipe_init must have succeeded
 *
*/
bam_status_type bam_pipe_enable(bam_handle pipehandle)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    bam_dev *bam;
    bam_status_type status = BAM_SUCCESS;

    if(pipe==NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_enable:Invalid BAM pipe handle, handle:0X%x\n", pipe);
        return BAM_FAILED;
    }
    bam = pipe->bam;
    bam_osal_syncenter(&bam->sync);
    bam_osal_syncenter(&pipe->sync);

    if ( BAM_OPT_NOT_SET(pipe->pipe_cfg.options,BAM_O_PIPE_NO_INIT) )
    {
    status = bamcore_pipe_init(bam->bam_cfg.bam_va,pipe->pipe_num,&pipe->pipe_cfg,pipe->irq_enabled,bam->bam_tgt_cfg->ee);
        if ( status == BAM_SUCCESS )
            pipe->toggle_at_reset = bamcore_pipe_get_toggle(bam->bam_cfg.bam_va,pipe->pipe_num);
        else
    {
            bam_osal_debug(MSG_ERR,"bam_pipe_enable:BAM 0x%x: BamCore pipe init failed, pipe:%d\n",
                           bam->bam_cfg.bam_pa,pipe->pipe_num);
        goto exit;
    }
    }
    //Reset the state variables
    pipe->acked_offset = 0x0;
    pipe->desc_offset = 0x0;
    pipe->size_since_eot = 0x0; 
    pipe->last_error = BAM_EVENT_INVALID;
    bam->active_pipes_mask |= (1<<pipe->pipe_num);
    status = BAM_SUCCESS;
exit:

    bam_osal_syncleave(&pipe->sync);
    bam_osal_syncleave(&bam->sync);

    return status;
}

/**
 * @brief Disables a pipe.
 *
 * Disables a pipe preventing further transfers
 *
 * @param *pipehandle - handle to the pipe to be disabled
 *
 * @return Status of the operation
 *
 * @dependencies bam_pipe_init must have succeeded
 *
*/

bam_status_type bam_pipe_disable(bam_handle pipehandle)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    bam_dev *bam;
    if(pipe==NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_disable:Invalid BAM pipe handle  handle:0X%x\n", pipe);
        return BAM_FAILED;
    }
    bam = pipe->bam;
    bam_osal_syncenter(&bam->sync);
    bam_osal_syncenter(&pipe->sync);
    if ( BAM_OPT_NOT_SET(pipe->pipe_cfg.options,BAM_O_PIPE_NO_INIT) )
    {
    bamcore_pipe_deinit(bam->bam_cfg.bam_va,pipe->pipe_num,pipe->irq_enabled,bam->bam_tgt_cfg->ee);
    }
    bam->active_pipes_mask &= ~(1<<pipe->pipe_num);
    bam_osal_syncleave(&pipe->sync);
    bam_osal_syncleave(&bam->sync);
    return BAM_SUCCESS;
}


/**
 * @brief Enables/Disables the IRQ mode of a pipe.
 *
 * Enables/Disables the IRQ mode of a pipe.
 *
 * @param *handle - handle to the pipe
 * @param irq_en  - Enable (1)/ Disable (0) the irq mode
 * @param pipe_cfg_opts - IRQs to be enabled for the BAM. See bam_options_type
 *
 * @return Status of the operation
 *
 * @note This API only sets the IRQ mode for the individual pipes and NOT the BAM.
*/
BAM_API_NON_PAGED bam_status_type bam_pipe_setirqmode(bam_handle pipehandle, uint32 irq_en, uint32 pipe_cfg_opts)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    bam_dev *bam;
    if(pipe==NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_setirqmode:Invalid BAM pipe handle handle:0x%x\n", pipe);
        return BAM_FAILED;
    }

    bam = pipe->bam;
    //Changing IRQ status requires modification of the global BAM registers

    bam_osal_syncenter(&bam->sync);
    bam_osal_syncenter(&pipe->sync);

    if (pipe->irq_enabled == 0 && irq_en != 0)
    {
        // clear all pending interrupts from this pipe, since we are going
        // to enable pipe interrupts
        bamcore_pipe_getandclearirqstatus(bam->bam_cfg.bam_va, pipe->pipe_num);
    }

    pipe->irq_enabled = irq_en;
    pipe->pipe_cfg.options = pipe_cfg_opts;
    bamcore_pipe_setirqmode(bam->bam_cfg.bam_va, pipe->pipe_num, bam->bam_tgt_cfg->ee,irq_en, pipe_cfg_opts);
    bam_osal_syncleave(&pipe->sync);
    bam_osal_syncleave(&bam->sync);

    return BAM_SUCCESS;
}

BAM_API_NON_PAGED bam_status_type bami_pipe_transfer_no_lock(pipe_dev* pipe, uint64 buf_pa, uint16 buf_size, uint16 xfer_opts, void* user_data)
{
    uint32 next_write_offset;
    bam_iovec_type iovec;
    bam_status_type result = BAM_FAILED;

    next_write_offset = pipe->desc_offset + BAM_DESC_SIZE;

    if(next_write_offset>=pipe->pipe_cfg.desc_size)
        next_write_offset = 0;

    if(next_write_offset == pipe->acked_offset)
    {
        bam_osal_debug(MSG_ERR, "bami_pipe_transfer_no_lock:BAM 0x%x:BAM out of descriptors next_Write_offset: %d, acked_offset %d \n",
                       pipe->bam->bam_cfg.bam_pa,next_write_offset, pipe->acked_offset);
        //TODO: Do we have to poll here?
        // Out of Descriptors
        goto exit;
    }

    // Validate descriptor flags

    // IF a command desriptor is being submitted make sure preceding DD has a NWD set
    if(xfer_opts & BAM_IOVEC_FLAG_CMD && pipe->last_offset != 0xFFFFFFFF &&
       !(((bam_iovec_type*)(pipe->last_offset + pipe->pipe_cfg.desc_base_va))->flags & (BAM_IOVEC_FLAG_CMD|BAM_IOVEC_FLAG_NWD)))
    {
        bam_osal_debug(MSG_ERR, "bami_pipe_transfer_no_lock:BAM 0x%x:DD descriptor preceding CMD must have NWD bit set\n",
                       pipe->bam->bam_cfg.bam_pa); 
        goto exit;
    }
    if(xfer_opts & BAM_IOVEC_FLAG_CMD && xfer_opts & BAM_IOVEC_FLAG_EOT )
    {
        bam_osal_debug(MSG_ERR, "bami_pipe_transfer_no_lock:BAM 0x%x: CMD descriptor cannot have EOT, pipeno:%d\n",
                       pipe->bam->bam_cfg.bam_pa, pipe->pipe_num); 
        goto exit;
    }
    if(xfer_opts & BAM_IOVEC_FLAG_NWD && !(xfer_opts & (BAM_IOVEC_FLAG_CMD|BAM_IOVEC_FLAG_EOT)))
    {
        bam_osal_debug(MSG_ERR, "bami_pipe_transfer_no_lock:BAM 0x%x: NWD must be paired with EOT or CMD descriptor,pipeno:%d\n",
                       pipe->bam->bam_cfg.bam_pa, pipe->pipe_num); 
        goto exit;
    }
    if(xfer_opts & BAM_IOVEC_FLAG_CMD && xfer_opts & BAM_IOVEC_FLAG_IMM_CMD )
    {
        bam_osal_debug(MSG_ERR, "bami_pipe_transfer_no_lock:CMD descriptor cannot have immediate flag set,pipeno:%d\n",
                       pipe->bam->bam_cfg.bam_pa, pipe->pipe_num); 
        goto exit;
    }
    if ( !bamcore_lpae_supported(pipe->bam->bam_cfg.bam_va) && ADDR_LPAE_MSB(buf_pa))
    {
        bam_osal_debug(MSG_ERR, "bami_pipe_transfer_no_lock:BAM 0x%x: buffer address > 32bits in a non-LPAE BAM, pipeno %d MSB 0x%x\n",
                       pipe->bam->bam_cfg.bam_pa, pipe->pipe_num, ADDR_LPAE_MSB(buf_pa)); 
        goto exit;
    }

    // Create a descriptor
    iovec.buf_pa     = (uint32)ADDR_LPAE_LSB(buf_pa);
    iovec.buf_pa_msb = ADDR_LPAE_MSB(buf_pa);
    iovec.buf_size = buf_size;
    iovec.flags = xfer_opts & BAM_IOVEC_HW_FLAGS; 

    // Update hardware descriptor FIFO
    *((bam_iovec_type *) (pipe->pipe_cfg.desc_base_va + pipe->desc_offset)) = iovec;

    pipe->last_offset = pipe->desc_offset; 

    // Now store the user data for later use
    pipe->desc_fifo[pipe->desc_offset / BAM_DESC_SIZE].user_data = user_data;

    // Move the desc offset
    pipe->desc_offset = next_write_offset;

    // Finally submit the command to the pipe if no_submit is NOT provided
    if(BAM_OPT_NOT_SET(xfer_opts,BAM_IOVEC_FLAG_NO_SUBMIT))
    {
        // Ideally we should operate with uncached memory. But some systems may not
        // provision uncached memory
        bam_osal_cachecmd(BAM_CACHE_FLUSH,pipe->pipe_cfg.desc_base_va,pipe->pipe_cfg.desc_size);
        bam_osal_memorybarrier();
        bamcore_pipe_setdescwriteoffset(pipe->bam->bam_cfg.bam_va, pipe->pipe_num,next_write_offset, 0);
        bam_osal_memorybarrier();
    }
    result = BAM_SUCCESS;
exit:

    return result;
}

/**
 * @brief Check if a pipe is operating in interrupt or polling mode
 *
 *
 * @param *handle - handle to the pipe
 * @param *irq_en  - irq mode enabled ( 1 ) or disabled ( 0 )
 *
 * @return Status of the operation
 *
*/
bam_status_type bam_pipe_isirqmode(bam_handle pipehandle, uint32* irq_en)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    bam_dev *bam;
    if(pipe==NULL || pipe->bam == NULL || irq_en == NULL)
        return BAM_FAILED;
    bam = pipe->bam;

    bam_osal_syncenter(&bam->sync);
    bam_osal_syncenter(&pipe->sync);

    bamcore_pipe_getirqmode(bam->bam_cfg.bam_va, pipe->pipe_num,bam->bam_tgt_cfg->ee,irq_en);

    bam_osal_syncleave(&pipe->sync);
    bam_osal_syncleave(&bam->sync);

    return BAM_SUCCESS;
}

/**
 * @brief Perform a single DMA transfer via the BAM pipe
 *
 * @details
 *
 * This function submits a DMA transfer request consisting of a single buffer
 * for a BAM pipe associated with a peripheral-to/from-memory
 * connection.
 *
 * The data buffer must be DMA ready.  The client is responsible for insuring
 * physically contiguous memory, cache maintenance, and memory barrier.
 *
 * The client must not modify the data buffer until the completion indication is
 * received.
 *
 * This function cannot be used if transfer queuing is disabled (see option
 * BAM_O_NO_Q).  The client must set the BAM_O_EOT option to receive a callback
 * event trigger when the transfer is complete.  The BAM driver will insure the
 * appropriate flags in the I/O vectors are set to generate the completion
 * indication.
 *
 * The return value from this function may indicate that an error occurred.
 * Possible causes include invalid arguments.
 *
 * @param pipe_handle - Handle to the pipe
 *
 * @param buf_pa - Physical address of buffer to transfer
 *
 * @param buf_size - Size in bytes of buffer to transfer
 *
 * @param xfer_opts - OR'd I/O vector option flags (see BAM_IOVEC_FLAG defines)
 *
 * @param user_data - User pointer that will be returned to user as part of
 *    event payload
 *
 *
 * @return Status of the operation
 *
 */
BAM_API_NON_PAGED bam_status_type bam_pipe_transfer(bam_handle pipehandle, uint64 buf_pa, uint16 buf_size, uint16 xfer_opts, void *user_data)
{

    pipe_dev *pipe = (pipe_dev *)pipehandle;
    bam_status_type result = BAM_FAILED;

    if(pipe==NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_transfer: Invalid BAM pipe handle, handle:0x%x buf_pa: 0x%xl \n", pipe,buf_pa);
        return BAM_FAILED;
    }

    if( pipe->pipe_cfg.mode ==  BAM_MODE_BAM2BAM)
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_transfer:BAM 0x%x: BAM2BAM mode, cannot call pipe transfer,\n",
                       pipe->bam->bam_cfg.bam_pa);
        return BAM_FAILED;
    }
    if (!bamcore_pipe_enabled(pipe->bam->bam_cfg.bam_va, pipe->pipe_num))
    {
	bam_osal_debug(MSG_ERR, "bam_pipe_transfer:BAM 0x%x: BAM Pipe not enabled Pipe:%d,\n",
                       pipe->bam->bam_cfg.bam_pa, pipe->pipe_num);
        return BAM_FAILED;
    }

    BAM_OSAL_TRACE(BAM_PIPE_TRANSFER_ENTRY, pipe->pipe_num, buf_size);

    bam_osal_syncenter(&pipe->sync);

    result = bami_pipe_transfer_no_lock(pipe,buf_pa,buf_size,xfer_opts,user_data);

    bam_osal_syncleave(&pipe->sync);

    BAM_OSAL_TRACE(BAM_PIPE_TRANSFER_EXIT, pipe->pipe_num, xfer_opts);
    return result;
}

/**
 * @brief Perform a DMA transfer via the BAM pipe
 *
 * @details
 *
 * This function submits multiple DMA transfer request consisting of a contiguous buffer
 * for a BAM pipe associated with a peripheral-to/from-memory
 * connection.
 *
 * The data buffer must be DMA ready.  The client is responsible for insuring
 * physically contiguous memory, cache maintenance, and memory barrier.
 *
 * The client must not modify the data buffer until the completion indication is
 * received.
 *
 * This function cannot be used if transfer will result in EOT trigger. That is, if
 * the buffers can be partially filled for a complete transfer, then this function
 * CANNOT be used. In that case, bam_pipe_transfer must be used and the transfer must
 * be split in 64k chunks.
 *
 * The return value from this function may indicate that an error occurred.
 * Possible causes include invalid arguments.
 *
 * @param pipe_handle - Handle to the pipe
 *
 * @param buf_pa - Physical address of buffer to transfer
 *
 * @param buf_size - Size in bytes of buffer to transfer
 *
 * @param xfer_opts - OR'd I/O vector option flags (see BAM_IOVEC_FLAG defines)
 *
 * @param user_data - User pointer that will be returned to user as part of
 *    event payload
 *
 *
 * @return Status of the operation
 *
 */
bam_status_type bam_pipe_bulktransfer(bam_handle pipehandle, uint64 buf_pa, uint32 buf_size, uint16 xfer_opts, void *user_data)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    uint32 free_count = 0;
    uint32 num_descs, buf_len;
    bam_status_type status = BAM_FAILED;
    uint32 max_transfer_size = 0xFFFF;

    if(pipe==NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_bulktransfer: Invalid BAM pipe handle, handle:0x%x buf_pa: 0x%xl \n", pipe,buf_pa);
            return BAM_FAILED;
    }
    if( pipe->pipe_cfg.mode ==  BAM_MODE_BAM2BAM)
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_bulktransfer:BAM 0x%x: BAM2BAM mode, cannot call pipe transfer,\n",
                       pipe->bam->bam_cfg.bam_pa);
        return BAM_FAILED;
    }
    if (!bamcore_pipe_enabled(pipe->bam->bam_cfg.bam_va, pipe->pipe_num))
    {
	bam_osal_debug(MSG_ERR, "bam_pipe_bulktransfer:BAM 0x%x: BAM Pipe not enabled Pipe:%d,\n",
                       pipe->bam->bam_cfg.bam_pa, pipe->pipe_num);
        return BAM_FAILED;
    }
    if(xfer_opts & BAM_IOVEC_FLAG_CMD)
    {
       bam_osal_debug(MSG_ERR, "bam_pipe_bulktransfer:BAM 0x%x: CMD descriptor is not valid for bulk transfer, pipeno:%d\n",
                      pipe->bam->bam_cfg.bam_pa,pipe->pipe_num);
       return BAM_FAILED; 
    }

    bam_osal_syncenter(&pipe->sync);

    max_transfer_size = bamcore_getmaxtransfersize();
    free_count = bam_pipe_getfreecount(pipehandle);
    num_descs=(buf_size+max_transfer_size-1)/max_transfer_size;

    if(free_count < num_descs)
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_bulktransfer:BAM 0x%x: Not enough descriptor, free_count:%d, num_desc:%d, buf_pa:0x%X, pipeno:%d\n",
                       pipe->bam->bam_cfg.bam_pa,free_count, num_descs, buf_pa, pipe->pipe_num);
        status = BAM_NOT_ENOUGH_SPACE;
        goto exit;
    }
    while(buf_size!=0)
    {
        buf_len = (buf_size <= max_transfer_size)?buf_size:max_transfer_size;
        buf_size = buf_size - buf_len;
        if(buf_size!=0)
            bami_pipe_transfer_no_lock(pipe, buf_pa,(uint16)buf_len, BAM_IOVEC_FLAG_NO_SUBMIT, NULL);
        else
            bami_pipe_transfer_no_lock(pipe,buf_pa,(uint16)buf_len,xfer_opts,user_data);
        buf_pa += buf_len;
    }

    status = BAM_SUCCESS;
exit:
    bam_osal_syncleave(&pipe->sync);
    return status;
}

/**
 * @brief Gets the number of free transfer entries for a BAM Pipe
 *
 * This function returns the number of free transfer entries
 * for a BAM Pipe
 *
 * @param pipe_handle - Pointer to client's handle to the BAM Pipe
 *
 * @return Number of free entries. < 0 indicates an error
 *
 */
uint32 bam_pipe_getfreecount(bam_handle pipehandle)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    uint32 next_write_offset;
    uint32 free_count;

    if(pipe == NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_getfreecount:Invalid BAM pipe handle  handle:0X%x\n", pipe);
        return 0;
    }

   // Determine descriptor FIFO state
   next_write_offset = pipe->desc_offset + BAM_DESC_SIZE;

   if (next_write_offset >= pipe->pipe_cfg.desc_size)
   {
      next_write_offset = 0;
   }
   if (pipe->acked_offset >= next_write_offset)
   {
      free_count = pipe->acked_offset - next_write_offset;
   }
   else
   {
      free_count = pipe->pipe_cfg.desc_size - next_write_offset + pipe->acked_offset;
   }
   free_count  /= BAM_DESC_SIZE;

   return free_count;

}

/**
 * @brief Determine whether a BAM Pipe's FIFO is empty
 *
 * This function returns the empty state of a BAM Pipe
 *
 * @param pipe_handle - Pointer to client's handle to the BAM Pipe
 *
 * @return 1 - Empty 0 - Not Empty
 *
 */
BAM_API_NON_PAGED uint32 bam_pipe_isempty(bam_handle pipehandle)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    bam_vaddr producer_bam, consumer_bam;
	uint32 producer_pipe, consumer_pipe;
    uint32 end_offset,empty=0;

    if(pipe == NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_isempty:Invalid BAM pipe handle  handle:0X%x\n", pipe);
        return 0;
    }

    // Get offset of last descriptor completed by the pipe
   end_offset = bamcore_pipe_getdescreadoffset(pipe->bam->bam_cfg.bam_va,pipe->pipe_num);

    // Determine descriptor FIFO state 
   if (pipe->pipe_cfg.mode != BAM_MODE_BAM2BAM)
   {
        empty =  (end_offset == pipe->acked_offset); 
   }
   else
   {
        uint32 consumer_evnt_offset;
        uint32 producer_sw_offset;
        uint32 producer_evnt_offset;
        // determine the producer and consumer bams
        if ( pipe->pipe_cfg.dir == BAM_DIR_PRODUCER )
        {
            producer_bam = pipe->bam->bam_cfg.bam_va;
            producer_pipe = pipe->pipe_num;

            consumer_bam = pipe->bam_peer_va;
            consumer_pipe = pipe->pipe_cfg.peer_pipe_num;
        }
        else
        {
            consumer_bam  = pipe->bam->bam_cfg.bam_va;
            consumer_pipe = pipe->pipe_num;

            producer_bam  = pipe->bam_peer_va;
            producer_pipe = pipe->pipe_cfg.peer_pipe_num;
        }

        if ( consumer_bam == 0 || producer_bam == 0 )
        {
            bam_osal_debug(MSG_ERR,"bam_pipe_isempty: peer bam va is not mapped. consumer:0x%x producer:0x%x\n", consumer_bam,producer_bam);
            return 0;
        }
        consumer_evnt_offset = bamcore_pipe_getdescwriteoffset(consumer_bam,consumer_pipe);
        producer_sw_offset   = bamcore_pipe_getdescreadoffset(producer_bam,producer_pipe);
        producer_evnt_offset = bamcore_pipe_getdescwriteoffset(producer_bam,producer_pipe);
        // preliminary check that all events from producer to consumer are done
        // compare the SW_OFST of producer to EVENT_REGn of consumer
        if ( producer_sw_offset != consumer_evnt_offset )
        {
            bam_osal_debug(MSG_HIGH,"bam_pipe_isempty: missing events,consumer bam:0x%x, pipe:%d\n",consumer_bam,consumer_pipe);
            empty = 0; 
        }
        else if ( producer_evnt_offset == consumer_evnt_offset )
        {
            if ( bamcore_pipe_getdescfifofullflag(producer_bam,producer_pipe))
            {
                bam_osal_debug(MSG_HIGH,"bam_pipe_isempty: descriptor fifo full bam:0x%x, pipe:%d\n",producer_bam,producer_pipe);
                empty = 0;
   }
   else
   {
                bam_osal_debug(MSG_HIGH,"bam_pipe_isempty: prod_evt_ofst == cons_evnt_ofst prod_bam:0x%x, pipe:%d\n",producer_bam,producer_pipe);
                empty = 1;
            }
        }
        else
        {
            uint32 prod_next_offset = 0;
            uint32 zlt_desc_loc     = 0;

            empty = 0;

            prod_next_offset = producer_evnt_offset + BAM_DESC_SIZE;

            // desc size is common to both producer and consumer, so take the size from
            // any pipe
            if ( prod_next_offset >= pipe->pipe_cfg.desc_size )
                prod_next_offset = 0;

            if ( prod_next_offset == producer_sw_offset )
            {
                // possible ZLT condition
                bam_iovec_type* desc = (bam_iovec_type*)(pipe->pipe_cfg.desc_base_va + producer_evnt_offset );
                bam_osal_debug(MSG_HIGH,"bam_pipe_isempty: prod_evt_ofst+8 == prod_sw_ofst prod_bam:0x%x, pipe:%d\n",producer_bam,producer_pipe);
                bam_osal_debug(MSG_HIGH,"bam_pipe_isempty: zlt check, size %d flags 0x%x prod_bam:0x%x, pipe:%d\n",desc->buf_size, desc->flags,producer_bam,producer_pipe);

                pipe->zlt_empty_check_cnt++;

                if ( (desc->buf_size == 0) && (desc->flags & BAM_IOVEC_FLAG_EOT) )
                {
                    zlt_desc_loc = producer_evnt_offset/BAM_DESC_SIZE;
                    if ( bamcore_pipe_desc_published(consumer_bam, consumer_pipe) )
                    {
                        uint32 offset_even  = 0;
                        uint32 toggle = bamcore_pipe_get_toggle(consumer_bam,consumer_pipe);
                        offset_even  =  (zlt_desc_loc % 2) == 0; 
                        if ( (pipe->toggle_at_reset == 0 && ( toggle == offset_even )) || ( pipe->toggle_at_reset && ( toggle != offset_even )))
                        {
                            bam_osal_debug(MSG_HIGH,"bam_pipe_isempty: zlt condition met. cons_bam:0x%x, pipe:%d\n",consumer_bam,consumer_pipe);
                            pipe->zlt_empty_cnt++;
                            empty = 1;
                        }
                        else
                        {
                            bam_osal_debug(MSG_HIGH,"bam_pipe_isempty: zlt condition not met,toggle:%d,toggle_rst:%d,zlt_loc:%d,cons_bam:0x%x,pipe:%d\n",
                                    toggle,pipe->toggle_at_reset,zlt_desc_loc/BAM_DESC_SIZE,consumer_bam,consumer_pipe);
   }
}
                    else
                    {
                        bam_osal_debug(MSG_HIGH,"bam_pipe_isempty: zlt condition not met.descriptor not acked.cons_bam:0x%x,pipe:%d\n",consumer_bam,consumer_pipe);
                    }
                }
            }
        }
    }
    return empty;
}

/**
 * @brief Triggers a descriptor event on behalf of peer BAM
 *
 * This function triggers a descriptor event on behalf of peer BAM
 * in BAM2BAM configuration to clear producer pipe state
 *
 * @param pipe_handle - Pointer to client's handle to the BAM Pipe
 *
 * @return Status of the operation
 *
 */
bam_status_type bam_pipe_trigger_event(bam_handle pipehandle)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    uint32 produced_offset;
    uint32 acked_offset;

    if(pipe == NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_trigger_event:Invalid BAM pipe params, pipe:0X%x\n", pipe);
        return BAM_FAILED;
    }
    if ((pipe->pipe_cfg.mode != BAM_MODE_BAM2BAM) || (pipe->pipe_cfg.dir != BAM_DIR_PRODUCER))
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_trigger_event:BAM 0x%x:Invalid BAM pipe params, pipe:0X%x, Dir:0X%x, mode:0X%x\n", 
                       pipe->bam->bam_cfg.bam_pa,pipe,pipe->pipe_cfg.mode,pipe->pipe_cfg.dir);
        return BAM_FAILED;
   }

   // Get offset of last descriptor completed by the pipe 
   acked_offset = bamcore_pipe_getdescwriteoffset(pipe->bam->bam_cfg.bam_va,pipe->pipe_num);
   // Get offset of last descriptor completed by the pipe
   produced_offset = bamcore_pipe_getdescreadoffset(pipe->bam->bam_cfg.bam_va,pipe->pipe_num);

   if (produced_offset == acked_offset && 
       bamcore_pipe_getdescfifofullflag(pipe->bam->bam_cfg.bam_va,pipe->pipe_num) != 0)
   {
      // if descriptor FIFO is full then trigger one descriptor less
      produced_offset = (produced_offset ? produced_offset : pipe->pipe_cfg.desc_size) - BAM_DESC_SIZE;
   }

   // Trigger event if pipe is not empty
   if (produced_offset != acked_offset)
   {
      bamcore_pipe_setdescwriteoffset(pipe->bam->bam_cfg.bam_va, pipe->pipe_num, produced_offset, pipe->pipe_cfg.data_size);
   }

   return BAM_SUCCESS;
}

/**
 * @brief Simulates  a ZLT EOT event from peer pipe in bam2bam mode
 *
 * This function proxies a ZLT EOT event from the peer pipe, so as to flush
 * bam pipe fifos.
 *
 * This api is only valid for pipes configured as:
 *    a. bam2bam
 *    b. consumer type
 *
 * The following pre-conditions apply for this api to have the required effect
 *    a. pipe is quiet. The caller of this api should gurantee that
 *       peer pipe will not generate new write events.
 *
 * @param pipehandle - Pointer to client's handle to the BAM Pipe
 *
 * @return Status of the operation
 *
 */
bam_status_type bam_pipe_trigger_zlt_event(bam_handle pipehandle)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    bam_iovec_type *iovec;
    uint32 evt_offset,sw_offset,next_write_offset;
    uint32 last_acked;

    if(pipe == NULL ) 
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_trigger_zlt_event:Null pipe handle , handle:0x%X\n", pipe);
        return BAM_FAILED;
    }

    if( pipe->pipe_cfg.mode != BAM_MODE_BAM2BAM || pipe->pipe_cfg.dir != BAM_DIR_CONSUMER)
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_trigger_zlt_event:Cannot call ZLT trigger event for {bam 0x%x pipe %d}. Invalid config\n",
                       pipe->bam->bam_cfg.bam_pa,pipe->pipe_num);
        return BAM_FAILED;
    }

    if ( pipe->bam_peer_va == 0 )
    {
        bam_osal_debug(MSG_ERR, "Error calling ZLT trigger pipe event. Peer bam va is not mapped: bam 0x%x pipe %d\n",
                       pipe->bam->bam_cfg.bam_pa,pipe->pipe_num);
        return BAM_FAILED;
    }

    if ( bamcore_bam_enabled(pipe->bam_peer_va,pipe->bam->bam_tgt_cfg->ee) == 0)
    {
        bam_osal_debug(MSG_ERR, "Error calling ZLT trigger pipe event. Peer bam 0x%x not enabled",pipe->pipe_cfg.peer_base_pa);
        return BAM_FAILED;
    }

    if ( bamcore_pipe_enabled(pipe->bam_peer_va,pipe->pipe_cfg.peer_pipe_num) == 0)
    {
        bam_osal_debug(MSG_ERR, "Error calling ZLT trigger pipe event. Peer bam's 0x%x pipe 0x%x not enabled",
                       pipe->pipe_cfg.peer_base_pa,pipe->pipe_cfg.peer_pipe_num);
        return BAM_FAILED;
    }

    bam_osal_debug(MSG_HIGH,"Trigger ZLT : bam 0x%x pipe %d\n",pipe->bam->bam_cfg.bam_pa,pipe->pipe_num);

    evt_offset = bamcore_pipe_getdescwriteoffset(pipe->bam->bam_cfg.bam_va,pipe->pipe_num);
    sw_offset  = bamcore_pipe_getdescreadoffset(pipe->bam->bam_cfg.bam_va,pipe->pipe_num);

    // get the desc full state of the peer pipe,i.e consumer pipe
    pipe->last_desc_full_state = bamcore_pipe_getdescfifofullflag(pipe->bam_peer_va,pipe->pipe_cfg.peer_pipe_num);
    if (pipe->last_desc_full_state || (sw_offset != evt_offset)) 
    {
        bam_osal_debug(MSG_ERR, "Trigger ZLT pipe event needs the pipe to be empty: bam 0x%x pipe %d\n",pipe->bam->bam_cfg.bam_pa,pipe->pipe_num);
        return BAM_FAILED;
    }

    last_acked = (sw_offset ? sw_offset : pipe->pipe_cfg.desc_size) - BAM_DESC_SIZE;

    iovec = (bam_iovec_type *) (pipe->pipe_cfg.desc_base_va + last_acked);

    if ( (iovec->flags & BAM_IOVEC_FLAG_EOT) == 0 && (iovec->buf_size != 0) && (iovec->buf_pa != 0) )
    {
        iovec = (bam_iovec_type *) (pipe->pipe_cfg.desc_base_va + sw_offset );
        iovec->flags = BAM_IOVEC_FLAG_EOT;
        iovec->buf_size = 0;
        next_write_offset = (sw_offset + BAM_DESC_SIZE) >= pipe->pipe_cfg.desc_size ?0: sw_offset + BAM_DESC_SIZE;
        bam_osal_cachecmd(BAM_CACHE_FLUSH,pipe->pipe_cfg.desc_base_va,pipe->pipe_cfg.desc_size);
        bam_osal_memorybarrier();
        bamcore_pipe_setdescwriteoffset(pipe->bam->bam_cfg.bam_va, pipe->pipe_num,next_write_offset, 0);
        bam_osal_memorybarrier();
        pipe->zlt_trigger_count++;
    }
    else
    {
        bam_osal_debug(MSG_HIGH, "EOT has already arrived.Trigger ZLT not required. Last desc info { pa=0x%x size=0x%x flags=0x%x}\n",
                       iovec->buf_pa,iovec->buf_size,iovec->flags);
    }
    return BAM_SUCCESS;
}
/**
 * @brief helper function to load the HW configs of a given pipe
 *
 * This function fills the pipe structures with the current HW
 * configurations.
 *
 * @param pipe_handle - Pointer to client's handle to the BAM Pipe
 *
 * @return Status of the operation
 *
 */
int bami_pipe_load_configs(pipe_dev* pipe)
{
    if ( pipe->pipe_desc_mem.handle )
        return 0;

    pipe->pipe_cfg.desc_base_pa = bamcore_pipe_get_descfifobase(pipe->bam->bam_cfg.bam_va,pipe->pipe_num);
    pipe->pipe_cfg.desc_size    = bamcore_pipe_get_descfifosize(pipe->bam->bam_cfg.bam_va,pipe->pipe_num);
    pipe->pipe_desc_mem.pa      = pipe->pipe_cfg.desc_base_pa;
    pipe->pipe_desc_mem.size    = pipe->pipe_cfg.desc_size;
    pipe->pipe_desc_mem.va      = 0;

    bam_osal_mallocmemregion(&pipe->pipe_desc_mem,pipe->pipe_desc_mem.size);

    if ( pipe->pipe_desc_mem.handle == NULL )
    {
        bam_osal_debug(MSG_ERR, "bami_pipe_load_configs: mallocmemregion failed.\n");
        return -1;
    }

    pipe->pipe_cfg.desc_base_va = pipe->pipe_desc_mem.va;

    return 0;
}

bam_status_type bam_pipe_trigger_event_sys(bam_handle pipehandle)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    bam_dev* bam = NULL;
    bam_iovec_type *prev_iovec = NULL;
    uint32 prev_offset = 0;
    bam_status_type ret = BAM_FAILED;

    if(pipe == NULL || pipe->pipe_cfg.mode != BAM_MODE_SYSTEM || pipe->pipe_cfg.dir != BAM_DIR_PRODUCER)
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_trigger_event_sys: Incorrect configuration. pipe handle 0x%x\n",pipe);
        return BAM_FAILED;
    }

    bam_osal_syncenter(&pipe->sync);

    bam = pipe->bam;

    if ( (pipe->pipe_cfg.options & BAM_O_PIPE_NO_INIT)  == 0 )
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_trigger_event_sys: Pipe initialized by this EE.\n");
        goto exit;
    }
    if ( bamcore_pipe_enabled(bam->bam_cfg.bam_va,pipe->pipe_num) == 0 )
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_trigger_event_sys: Pipe not enabled\n");
        ret = BAM_PIPE_DISABLED;
        goto exit;

    }
    //load the hw configurations for this pipe.
    // hw configurations includes:
    //  - descriptor fifo base
    //  - descriptor fifo size
    if ( 0 != bami_pipe_load_configs(pipe))
    {
        bam_osal_debug(MSG_ERR, "bam_pipe_trigger_event_sys: Error loading pipe configurations.\n");
        goto exit;
    }

    pipe->acked_offset = bamcore_pipe_getdescreadoffset(pipe->bam->bam_cfg.bam_va,pipe->pipe_num);
    pipe->desc_offset  = bamcore_pipe_getdescwriteoffset(pipe->bam->bam_cfg.bam_va,pipe->pipe_num);

    // system mode pipes. unsued slot in the descriptor fifo.
    // sw_offset == p_event_offset means, pipe is empty.
    if ( pipe->acked_offset != pipe->desc_offset )
    {
        //trying to trigger a non-empty pipe
        bam_osal_debug(MSG_ERR,"bam_pipe_trigger_event_sys: non-empty pipe.\n");
        ret = BAM_PIPE_NOT_EMPTY;
        goto exit;
    }

    // put the pipe to polling mode.
    bamcore_pipe_setirqmode(bam->bam_cfg.bam_va, pipe->pipe_num, bam->bam_tgt_cfg->ee,0, 0);

    // generate descriptors
    //   - take the descriptor @ sw_offset
    prev_offset = ( pipe->acked_offset ? pipe->acked_offset : pipe->pipe_cfg.desc_size ) - BAM_DESC_SIZE ;
    prev_iovec  = (bam_iovec_type*)(pipe->pipe_cfg.desc_base_va + prev_offset );

    //replicate the descriptor over the full descriptor fifo
    //this should be done once per pipe
    if ( pipe->trigger_pipe_sys_replicate_cnt == 0 )
    {
        uint32 i = 0;
        bam_iovec_type iovec = *prev_iovec;
        bam_iovec_type* next_iovec = NULL;
        next_iovec = (bam_iovec_type*)pipe->pipe_cfg.desc_base_va; 
        bam_osal_debug(MSG_HIGH,"bam_pipe_trigger_event_sys:BAM 0x%x:replicating descriptor {pa=0x%x,size=0x%x,flags=0x%x} for pipe %d\n",
                       pipe->bam->bam_cfg.bam_pa,prev_iovec->buf_pa,prev_iovec->buf_size,prev_iovec->flags,pipe->pipe_num);
        for(;i < pipe->pipe_cfg.desc_size/BAM_DESC_SIZE ; i++ )
        {
            next_iovec[i] = iovec;
        }
        bam_osal_cachecmd(BAM_CACHE_FLUSH,pipe->pipe_cfg.desc_base_va,pipe->pipe_cfg.desc_size);
        bam_osal_memorybarrier();
        pipe->trigger_pipe_sys_replicate_cnt++;
    }
    bam_osal_debug(MSG_HIGH,"bam_pipe_trigger_event_sys:BAM 0x%x: submitting descriptors for pipe %d\n", 
                   pipe->bam->bam_cfg.bam_pa,pipe->pipe_num);
    bamcore_pipe_setdescwriteoffset(pipe->bam->bam_cfg.bam_va, pipe->pipe_num,prev_offset, 0);
    bam_osal_memorybarrier();

    ret = BAM_SUCCESS;

exit:
    bam_osal_syncleave(&pipe->sync);
    return ret;
}

/**
 * @brief Polls for a result
 *
 * This function polls the pipe for a result. As a side-effect,
 * the callback registered via bam_pipe_init might be called
 * if a result is pending. Upon an error detected, the callback
 * registered via bam_init will get called. This function by
 * itself will not return any result if a callback was provided.
 *
 * @param pipe_handle - Pointer to client's handle to the BAM Pipe
 * @param[out] result - Pointer to the structure that can hold the result.
 *                      If a callback was provided at pipe init, then
 *                      the callback will be invoked with the result and
 *                      this param will be set to null/invalid.
 *
 * @return Status of the operation. 
 *  
 * @note The Pipe should be operating in polling mode 
 *
 */
BAM_API_NON_PAGED bam_status_type bam_pipe_poll(bam_handle pipehandle, bam_result_type *result)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;

    if(pipe == NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_poll:Invalid BAM pipe handle, pipe:0X%x\n", pipe);
        return BAM_FAILED;
    }

    // if result location is not provided then a temprary result placeholder 
    // is used to avoid overwriting pipe->desc_fifo in bam_pipe_eot_handler 
    // in case an IST is running in parallel
    if (result == NULL)
    {
        bam_result_type temp_result;
        bam_callback_func_type pipe_cb_func = pipe->pipe_cb.func;

        do
        {
            bam_pipe_eot_handler(pipe,&temp_result);

            if (temp_result.event != BAM_EVENT_INVALID && pipe_cb_func != NULL)
            {
                pipe_cb_func(temp_result);
            }
        } while (temp_result.event != BAM_EVENT_INVALID);
    }
    else if ( result->cb_mode == BAM_INVOKE_CB_IN_POLL )
    {
        bam_pipe_eot_handler(pipe,result);
        if ( result->event == BAM_EVENT_INVALID )
            return BAM_POLL_NO_RESULT;
    }
    else 
        bam_pipe_eot_handler(pipe,result);

    return BAM_SUCCESS;
}

/**
 * @brief Gets the current descriptor in FIFO
 *
 * This function return information about the descriptor currently being 
 * processed in descriptor FIFO. if there is no pending descriptor 
 * then result->event is set to BAM_EVENT_INVALID
 *
 * @param[in]  pipe_handle  Pointer to client's handle to the BAM Pipe
 * @param[out] *result      Pointer to the structure that can hold the result.
 *
 * @return Status of the operation.
 *
 * @note the actual descriptor being processed may change anytime 
 *       (even before this function returns)
 *
 */
bam_status_type bam_pipe_get_current_descriptor_info(bam_handle pipehandle, bam_result_type *result)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    uint32 endoffset;

    if(pipe == NULL || result == NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_get_current_descriptor_info:Invalid BAM pipe handle, pipe:0X%x\n", pipe);
        return BAM_FAILED;
    }

    bam_osal_syncenter(&pipe->sync);

    endoffset = bamcore_pipe_getdescreadoffset(pipe->bam->bam_cfg.bam_va, pipe->pipe_num);

    result->event = BAM_EVENT_IN_PROGRESS;
    result->cb_data = pipe->pipe_cb.data;
    result->data.xfer.iovec = *((bam_iovec_type *) (pipe->pipe_cfg.desc_base_va + endoffset));
    result->data.xfer.xfer_cb_data = pipe->desc_fifo ? pipe->desc_fifo[endoffset / BAM_DESC_SIZE].user_data : 0;

    // last chance to catch the change in hw state
    if (bam_pipe_isempty(pipehandle) != 0)
    {
        result->event = BAM_EVENT_INVALID;
    }

    bam_osal_syncleave(&pipe->sync);

    return BAM_SUCCESS;
}

/**
 * @brief Perform timer control
 *
 * This function performs timer control operations.
 *
 * @param pipe_handle - Pointer to client's handle to the BAM Pipe
 * @param timer_ctrl - Pointer to timer control operation parameters
 * @param [out] timer_rslt - Pointer to buffer for timer operation result.
 *    This argument can be NULL if no result is expected for the operation.
 *    If non-NULL, the current timer value will always provided.
 *
 * @return Status of the operation.
 *
 */
bam_status_type bam_pipe_timerctrl(bam_handle pipehandle, bam_timer_ctrl_type *timer_ctrl, bam_timer_rslt_type *timer_rslt)
{
    pipe_dev *pipe = (pipe_dev *)pipehandle;
    bam_dev *bam;

    if(pipe == NULL || timer_ctrl == NULL || timer_rslt == NULL)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_timerctrl:Invalid BAM pipe params, pipe:0X%x, timer_rslt:0X%x\n", pipe, timer_rslt);
        return BAM_FAILED;
    }

    bam = pipe->bam;

    // Is this pipe locally controlled? 
    if((bam->active_pipes_mask & (1UL << pipe->pipe_num)) == 0)
    {
        bam_osal_debug(MSG_ERR,"bam_pipe_timerctrl:BAM pipe not intialized, pipe_active_mask:0X%x, pipe_num:%d\n", 
                       bam->active_pipes_mask, timer_ctrl, pipe->pipe_num);
        return BAM_FAILED;
    }

    // Perform the timer operation
   switch (timer_ctrl->op)
   {
      case BAM_TIMER_OP_CONFIG:
         if(timer_ctrl->mode == BAM_TIMER_MODE_ONESHOT) 
         {    
             bamcore_pipe_timerconfig(bam->bam_cfg.bam_va, pipe->pipe_num, timer_ctrl->mode,
                timer_ctrl->timeout_ms * 10);
         }
         break;
      case BAM_TIMER_OP_RESET:
         bamcore_pipe_timerreset(bam->bam_cfg.bam_va, pipe->pipe_num);
         break;
/* Not supported by hardware yet - use RESET
      case BAM_TIMER_OP_START:
         break;
      case BAM_TIMER_OP_STOP:
         break;
*/
      case BAM_TIMER_OP_READ:
         break;
      default:
            bam_osal_debug(MSG_ERR,"bam_pipe_timerctrl:Invalid timer op requested, timer_op:0x%x\n", timer_ctrl->op);
         return BAM_FAILED;
   }

    // Provide the current timer value
   timer_rslt->curr_timer = 
       bamcore_pipe_timergetcount(bam->bam_cfg.bam_va, pipe->pipe_num);

   return BAM_SUCCESS;

}
/**
 * @brief fills a command element
 *
 * Fills a command element (should be of bam_ce_type size)
 *
 * @param[in]      ce_base   pointer to the command element base
 * @param[in]      index     command element index
 * @param[in]      reg       register address to program
 * @param[in]      cmd       command (see bam_ce_oper_type)
 * @param[in]      data      data to program to the register
 * @param[in]      mask      mask to use                      
 */
void bam_fill_ce(bam_vaddr ce_base, uint32 index ,uint64 reg, uint32 cmd, uint64 data, uint32 mask)
{
    bam_ce_type* ce = ((bam_ce_type*)ce_base) + index;
    ce->reg_addr  = reg&0xFFFFFF;
    ce->command   = cmd;
    ce->data      = data&0xFFFFFFFF;
    if ( cmd == BAM_CE_OP_WRITE )
        ce->mask      = mask;
    else
        ce->mask      = ADDR_LPAE_MSB(data);
    ce->reserved  = 0x00000000;
}
  
  
