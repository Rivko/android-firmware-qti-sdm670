/**
  @file bam_debug.c
  @brief
  This file contains the implementation of the debug interfaces 
  that provide BAM core dump. 

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/12/15   SS      Optimize for istari
05/08/13   SS      Fix for 16 bytes desc buffer corruption in back to back core dump
02/28/13   SS      Added Peer BAM PA to VA mapping for Core dump
02/13/13   SS      Created 

===============================================================================
                   Copyright (c) 2013-2015 QQUALCOMM Technologies Inc.
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

/* Globals common to the driver */
extern bam_dev **bams;
uint32 *bam_buf = NULL;

/**
 * @brief Peer bam instance & VA info
 *
 * This API will provide peer bam instance for requested pipe numbers.
 *
 * @param bam           Pointer to Bam device.
 * @param pmask         Bam Pipemask. 
 * @param peerpmask     pipe mask for which peer need to dump.
 * @param pipe_nums     Supported pipe numbers.
 * @param pipes         Holds Bam Peer Va & Pa.
 *
 * @return              Peer bam instance count.
 *
 */
  static uint32  bam_get_peerbamscount(bam_dev *bam, uint32 pmask, uint32 peerpmask, uint32 pipe_nums, bam_peer_info *pipes);

/**
 * @brief BAM pipe info
 *
 * This API will provide bam pipe numbers.
 *
 * @param nums_pipes    Bam Pipe supported
 * @param pipemask	   Pipemask for pipe info.
 *
 * @return              BAM Pipe count.
 */
static uint32 bam_get_pipecount(uint32 num_pipes, uint32 pipemask);

/**
 * @brief Bam get info
 *
 * This API will provide BAM, pipe & Descriptor buffer size info.
 *
 * @param bamhandle      BAM handle
 * @param info           Get bam dump buffer size info.
 *
 *
 */
bam_status_type bam_get_info(bam_handle bamhandle, bam_info_type *info)
{
    bam_dev *bam = NULL;
    bam_status_type status = BAM_SUCCESS;

    if(((bam_handle)NULL==bamhandle) || (NULL == info))
    {
        status = BAM_FAILED;
    }
    else
    {
        bam = BAM_GET_DEV_FROM_HANDLE(bamhandle);
        info->num_pipes = bam->bam_caps.num_pipes;
        info->bam_header_buffer_size = bamcore_getheaderbufferinfo();
        info->bam_buffer_size = bamcore_getctrlregbufferinfo();
        info->bam_pipe_buffer_size = bamcore_getpiperegbufferinfo();
        info->bam_max_desc_size = bamcore_getmaxtransfersize();
    }
    return status;
}

/**
 * @brief Peer bam instance & VA info
 *
 * This API will provide peer bam instance for requested pipe numbers.
 *
 * @param bam          Bam handle
 * @param pmask        Pipemask. 
 * @param peerpmask    pipe mask for which peer need to dump. 
 * @param pipe_nums    Supported pipe numbers.
 * @param pipes        Holds Bam Peer Va & Pa.
 *
 * @return             Nums of peer bam instance.
 *
 */
static uint32 bam_get_peerbamscount(bam_dev *bam, uint32 pmask, uint32 peerpmask, uint32 pipe_nums, bam_peer_info *pipe_base)
{
    pipe_dev *pipe = NULL;
    uint32 count, id, index, peerbam_nums = 0, mask = 1;

    for(count = 0, index = 0; count< pipe_nums && pmask != 0; pmask >>= 1, count++)
    {
        if((1 & pmask) != 0)
        {
            /* Check if pipe is intialize or not */
            if ((bam->active_pipes_mask & (mask << count)) != 0)
            {
                pipe = bam->pipes[count];
                if((NULL != pipe) && ((0 != (peerpmask & mask)) && (BAM_MODE_BAM2BAM == pipe->pipe_cfg.mode)))
                {
                    pipe_base[index].pipe_num = pipe->pipe_num;
                    pipe_base[index].peerpipe_num= pipe->pipe_cfg.peer_pipe_num;
                    pipe_base[index].bam_peer_pa = pipe->pipe_cfg.peer_base_pa;
                    pipe_base[index].bam_peer_va = pipe->bam_peer_va;
                    if(bam->bam_cfg.bam_pa != pipe->pipe_cfg.peer_base_pa)
                    {
                        for(id = 0; id < index; id++)
                        {
                            if(pipe_base[index].bam_peer_pa == pipe_base[id].bam_peer_pa)
                                break;
                        }
                        if(id == index)
                            peerbam_nums++;
                    }
                }
                else
                {
                    pipe_base[index].pipe_num = count;
                    pipe_base[index].peerpipe_num= BAM_PIPE_INVALID;
                    pipe_base[index].bam_peer_pa = 0x0;
                    pipe_base[index].bam_peer_va = 0x0;
                }
            }
            else
            {
                pipe_base[index].pipe_num = count;
                pipe_base[index].peerpipe_num= BAM_PIPE_INVALID;
            }
            index++;
        }
    }
    return peerbam_nums;
}

/**
 * @brief BAM pipe info
 *
 * This API will provide bam pipe numbers.
 *
 * @param num_pipes   Number of supported BAM Pipe
 * @param pipemask	   pipemask for pipe info.
 *
 * @return             BAM Pipe count.
 */
static uint32 bam_get_pipecount(uint32 num_pipes, uint32 pipemask)
{
    uint32 pipecount = 0;
    /* clear out unused bit */
    pipemask = pipemask&~(~0 << num_pipes);
    for (;pipemask; pipemask &= pipemask-1, pipecount++);
    return pipecount;
}

/**
 * @brief bam_core_dump
 *
 * This function get the Bam & pipe register dump after error condition.
 *
 * @param bamhandle      Pointer to client's handle to BAM.
 * @param coredump_cfg   Pointer to bam core dump Config structure.
 *
 * @return 				- None.
 *
 * Note: Before calling this API, use API bam_get_info() to find out required core dump buffer size.
 *
 */
bam_status_type bam_core_dump(bam_handle bamhandle, bam_coredump_config_type *coredump_cfg)
{
     bam_dev *bam = NULL;
     pipe_dev *pipe = NULL;
     bam_caps_type bam_caps;
     bam_info_type info;
     static bam_peer_info pipes_cfg[31] = {{0}};
     const bam_target_config_type* tgt_cfg;
     bamdebug_pipe_config  pipedump = {0};
     uint32  count, bam_peernums = 1, pipe_count = 0, peerpcount = 0;
     uint32 datafifooffset = 0, descfifooffset = 0, bufsize=0;

     if(((bam_handle)NULL == bamhandle) || (NULL == coredump_cfg))
     {
         bam_osal_debug(MSG_ERR,"Corrupted user input  bamhandle:0x%X, coredump_cfg:0x%X\n", bamhandle, coredump_cfg);
         return BAM_FAILED;
     }
     if(NULL == coredump_cfg->user_buffer)
     {
         bam_osal_debug(MSG_ERR,"Corrupted user Buffer  user_buffer:0x%X\n", coredump_cfg->user_buffer);
         return BAM_FAILED;
     }
     bam_osal_debug(MSG_ERR,"Vailid BAM core dump config ptr   coredump_cfg:0x%X\n", coredump_cfg);
     bam_osal_debug(MSG_HIGH,"Core dump user Buffer,  bufptr:0x%X, buf_size:%d\n", coredump_cfg->user_buffer, coredump_cfg->buf_size);  
       
     if(BAM_SUCCESS != bam_get_info(bamhandle, &info))
     {
         bam_osal_debug(MSG_ERR, "bam_get_info: BAM return error \n");
         return BAM_FAILED;
     }
     bam = BAM_GET_DEV_FROM_HANDLE(bamhandle);
     
     /* check if BAM is enabled */
     if(BAM_SUCCESS != bamcore_check(bam->bam_cfg.bam_va, &bam_caps,bam->bam_tgt_cfg->ee)) 
     {
         bam_osal_debug(MSG_ERR, "bamcore_check: BAM is not enable\n");
         return BAM_FAILED;
     }
     
     if (0 != coredump_cfg->pipe_mask)
     {
         /* Get the top level pipe count */
         pipe_count = bam_get_pipecount(bam_caps.num_pipes, coredump_cfg->pipe_mask);
         memset(pipes_cfg, 0x0, sizeof(bam_peer_info *) * (pipe_count));
         /* Get the Peer bam count , update the pipe info and map Bam pa*/
         bam_peernums += bam_get_peerbamscount(bam, coredump_cfg->pipe_mask, coredump_cfg->peer_pipe_mask, bam_caps.num_pipes, &pipes_cfg[0]);
     }
     else
     {
         bam_osal_debug(MSG_ERR, "Invalid Pipe Mask pipemask:0x%X\n", coredump_cfg->pipe_mask);
         return BAM_FAILED;
     }

     if(coredump_cfg->peer_pipe_mask)
     {
         /* Get the Peer pipe count */
         peerpcount = bam_get_pipecount(bam_caps.num_pipes, coredump_cfg->peer_pipe_mask);
     }

     bufsize = (bam_peernums *(info.bam_buffer_size))+(info.bam_pipe_buffer_size * (peerpcount + pipe_count))+info.bam_header_buffer_size;
     if(coredump_cfg->buf_size < bufsize)
     {
         bam_osal_debug(MSG_ERR,"Invailid BAM core dump buffer size  bufsize:%d, buf_size:%d\n", bufsize, coredump_cfg->buf_size);
         return BAM_INVALID_BUFSIZE;
     }
     
     bam_osal_debug(MSG_HIGH,"BAM core dump Desc buffer info  descfifo:0x%X, fifo_size:%d\n",coredump_cfg->descfifo_buf,coredump_cfg->descfifo_size);
     bam_osal_debug(MSG_HIGH,"BAM core dump Desc buffer info  datafifo:0x%X, fifo_size:%d\n",coredump_cfg->datafifo_buf,coredump_cfg->datafifo_size);
     bam_buf = (uint32 *)coredump_cfg->user_buffer;
     memset(bam_buf, 0, coredump_cfg->buf_size);
     if(coredump_cfg->peer_pipe_mask)
     {
         memset(coredump_cfg->descfifo_buf, 0, coredump_cfg->descfifo_size);
         memset(coredump_cfg->datafifo_buf, 0, coredump_cfg->datafifo_size);
     }
     
     /* log the BAM header */
     bamcore_headerdump(bam_buf, coredump_cfg->pipe_mask, coredump_cfg->peer_pipe_mask);
     /* get EE info */
     tgt_cfg = bam_tgt_getcfg(bam->bam_cfg.bam_pa);
     if(tgt_cfg == NULL)
     {
         bam_osal_debug(MSG_ERR,"bam_tgt_getcfg has returned NULL  tgt_cfg:0x%X, user_buffer:0x%X\n", tgt_cfg, bam->bam_cfg.bam_pa);
         return BAM_FAILED;
     }
     /* get Top level BAM dump	 */ 
     bamcore_bamregdump(bam->bam_cfg.bam_va, bam_buf, bam->bam_cfg.bam_pa, tgt_cfg->ee);
     /* get peer BAM dump */
     if(coredump_cfg->peer_pipe_mask)
     {
         for(count =0; count < pipe_count;  count++)
         {
             if(pipes_cfg[count].peerpipe_num != BAM_PIPE_INVALID)
             {  
                 pipe = bam->pipes[ pipes_cfg[count].pipe_num];
                 /* check if this is same peer bam requested by peer mask */
                 if((BAM_MODE_BAM2BAM == pipe->pipe_cfg.mode) && (0 !=(coredump_cfg->peer_pipe_mask & (1 << pipes_cfg[count].pipe_num))))
                 {
                     /* To avoid duplication  */
                     if(pipes_cfg[count].bam_peer_pa != bam->bam_cfg.bam_pa)
                     {
                         bamcore_bamregdump(pipes_cfg[count].bam_peer_va, bam_buf, pipes_cfg[count].bam_peer_pa, BAM_MAX_EES);
                     }
                 }
             }
         }
     }
     /* get pipe dump */
     if(coredump_cfg->pipe_mask)
     {
         for(count = 0; count< pipe_count; count++)
         {
             pipe = bam->pipes[pipes_cfg[count].pipe_num];
             /* check if this peer pipe dump is needed */
             if((NULL != pipe) && ((0x0 != coredump_cfg->peer_pipe_mask) && (BAM_MODE_BAM2BAM == pipe->pipe_cfg.mode)))
             {
                 if((coredump_cfg->peer_pipe_mask & (1<< pipes_cfg[count].pipe_num))!= 0x0)
                 {
                     pipedump.pipenum = pipes_cfg[count].pipe_num;
                     pipedump.peerpipenum = pipes_cfg[count].peerpipe_num;
                     pipedump.peerbase_va = pipes_cfg[count].bam_peer_va;
                     pipedump.peerbase_pa = pipes_cfg[count].bam_peer_pa;
                     pipedump.desc_base_va = pipe->pipe_cfg.desc_base_va;
                     pipedump.data_base_va = pipe->pipe_cfg.data_base_va;               
                     if(NULL !=coredump_cfg->descfifo_buf)
                     {
                         if(descfifooffset >= coredump_cfg->descfifo_size)
                         {
                             bam_osal_debug(MSG_ERR,"Invailid BAM Desc buffer size, buf_size:%d\n", coredump_cfg->descfifo_size);
                             pipedump.desc_base_va = 0;
                         }
                         else
                         {
                             pipedump.desc_fifo = (uint8 *)coredump_cfg->descfifo_buf + descfifooffset;
                             pipedump.desc_fifo_size = pipe->pipe_cfg.desc_size;
                             descfifooffset += pipe->pipe_cfg.desc_size;
                             bam_osal_debug(MSG_HIGH,"BAM Desc buffer info, pipe:%d, descptr:0x%X, buf_size:0x%X\n", pipedump.pipenum, pipedump.desc_fifo, descfifooffset);
                         }
                     }
                     if(NULL != coredump_cfg->datafifo_buf)
                     {
                         if (datafifooffset >= coredump_cfg->datafifo_size)
                         {
                             bam_osal_debug(MSG_ERR,"Invailid BAM Data fifo buffer size, buf_size:%d\n", coredump_cfg->datafifo_size);
                             pipedump.data_base_va = 0;
                         }
                         else
                         {
                             pipedump.data_fifo = (uint8 *)coredump_cfg->datafifo_buf + datafifooffset;
                             pipedump.data_fifo_size = pipe->pipe_cfg.data_size;
                             datafifooffset += pipe->pipe_cfg.data_size;
                             bam_osal_debug(MSG_HIGH,"BAM Data buffer info, pipe:%d,dataptr:0x%X,buf_size:0x%X\n", pipedump.pipenum, pipedump.data_fifo, datafifooffset);
                         }
                     }
                 }
                 else
                 {
                     pipedump.pipenum = pipes_cfg[count].pipe_num;
                     pipedump.peerpipenum = BAM_PIPE_INVALID;
                     bam_osal_debug(MSG_HIGH,"BAM pipe of peer info in not requested, pipe:%d\n", pipedump.pipenum);
                 }
             }
             else
             {
                 pipedump.pipenum = pipes_cfg[count].pipe_num;
                 pipedump.peerpipenum = pipes_cfg[count].peerpipe_num;
                 bam_osal_debug(MSG_HIGH,"Pipehandle may be NULL or Pipe in system mode, handle:0x%X, pipe:%d\n",pipe, pipedump.pipenum);
             }
             
             /* get pipe and peerpipe(if B2B mode) register dump */
             if(BAM_PIPE_INVALID != pipedump.pipenum)        
                 bamcore_piperegdump(bam->bam_cfg.bam_va, bam->bam_cfg.bam_pa, bam_buf, &pipedump);
         }
     }
     return BAM_SUCCESS;
}
 

