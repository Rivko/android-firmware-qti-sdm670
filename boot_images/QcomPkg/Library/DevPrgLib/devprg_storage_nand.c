/**************************************************************************
 * FILE: devprg_storage_nand.c
 *
 * NAND Device implementation.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_storage_nand.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-07-13   svl   Updated APIs to make devprg work on NAND.
2017-04-20   wek   Include the start and end sectors in set io options.
2017-02-02   wek   Add io options to change how data is read/write.
2017-01-25   wek   Create (partially, change history accordingly)

===========================================================================*/

#ifdef FEATURE_DEVPRG_NAND  /* { */

#include "DALDeviceId.h"
#include "DALSysTypes.h"
#include "flash.h"
#include "devprg_storage_nand.h"
#include "devprg_storage.h"
#include "devprg_log.h"


#include "DALHeap.h"
#include "bam_drv.h"

#include <string.h>
#include <stringl.h>

#define DDP_LOGE  DP_LOGE
#define DDP_LOGI  DP_LOGI
#define DDP_LOGW  DP_LOGW

#define DEVPRG_MULTI_RW_SUPPORT_ENABLED (0)

/******************************************************************************
 * Data structure definitions
 *****************************************************************************/

/* map each block to each bit of a byte */
#define DEVPRG_STORAGE_NAND_BBT_SIZE(block_count)   ( (block_count/8) +   \
                                                      ((block_count%8)?1:0))

/* to check if a block is bad */
#define DEVPRG_STORAGE_NAND_BLOCK_IS_BAD(bbt, block)   \
                                            (bbt[block/8] & (1 << (block%8)))

/* to mark a block as bad */
#define DEVPRG_STORAGE_NAND_MARK_BADBLOCK(bbt, block)  \
                                            (bbt[block/8] |= (1 << (block%8)))


#define DEVPRG_STORAGE_NAND_MAX_NUM_BUFS   (2)
#define DEVPRG_STORAGE_NAND_MAIN_BUF_DESC  (0)
#define DEVPRG_STORAGE_NAND_SPARE_BUF_DESC (1)

/* initialize page data io vectors */
#define INIT_PAGE_DATA_IOVEC(iovec,main_size,spare_size,page,page_count)      \
                                        iovec.main_size_bytes   = main_size;  \
                                        iovec.spare_size_bytes  = spare_size; \
                                        iovec.total_page_count  = page_count; \
                                        iovec.start_page        = page;

#define NAND_UPDATE_PAGE_COUNT(iovec, desc, count)           \
              iovec.total_page_count  = page_count; \
              desc.BufInfo[DEVPRG_STORAGE_NAND_MAIN_BUF_DESC].user=page_count;\
              desc.BufInfo[DEVPRG_STORAGE_NAND_SPARE_BUF_DESC].user=page_count;

#define NAND_MAIN_BUF_ADDR(x)   \
                    (x.BufInfo[DEVPRG_STORAGE_NAND_MAIN_BUF_DESC].VirtualAddr)
#define NAND_SPARE_BUF_ADDR(x)  \
                    (x.BufInfo[DEVPRG_STORAGE_NAND_SPARE_BUF_DESC].VirtualAddr)

/* max pages that can be read from flash at a time */
#define DEVPRG_STORAGE_NAND_MAX_MULTI_PAGE (64)

/* required for device attach */
#define DALSYSCMN_MEM_DESC_OBJECT_ID (0x5)

/*  initialized to 0xFFFFFFFF as there can never be a page or block with
    this value */
#define DP_INVALID_PAGE  (0xFFFFFFFF)
#define DP_INVALID_BLOCK (0xFFFFFFFF)

/* used to configure GPIOs in 9x07 and 9206 QPIC */
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR   (TLMM_BASE + 0x00111000)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR   (TLMM_BASE + 0x0010a000)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OUT(v) \
                                out_dword(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR,v)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUT(v) \
                                out_dword(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,v)

#define DEVPRG_DEV_DEFAULT_INIT(dev)                                          \
                                do{                                           \
                                  dev->skip_bb = BAD_BLOCK_SKIP;              \
                                  dev->get_spare = DATA_ONLY;                 \
                                  dev->ecc_state = ECC_ENABLED;               \
                                  dev->start_pba = DP_INVALID_PAGE;           \
                                  dev->last_avail_pba = DP_INVALID_BLOCK;     \
                                  dev->last_erased_block = DP_INVALID_BLOCK;  \
                                  dev->last_written_page = DP_INVALID_PAGE;   \
                                }while(0)

#define NAND_BB_PATTERN          (0xBD)
#define NAND_ERASED_PAGE_PATTERN (0xEE)
#define NAND_FAILED_PAGE_PATTERN (0xFA)

/* required to initialize DALSys heap */
#pragma arm section zidata = "FLASH_TOOLS_HEAP_ZONE"
uint8 dp_flash_tools_heap[FLASH_TOOLS_HEAP_SIZE];
#pragma arm section zidata

struct dp_storage_nand
{
  flash_handle_t          handle;
  uint8                   in_use;
  enum devprg_skip_bb     skip_bb;
  enum devprg_get_spare   get_spare;
  enum devprg_ecc_state   ecc_state;
  uint32                  total_blocks;
  uint32                  total_part_blocks;
  uint8                   *bbt;
  uint32                  maker_id;
  uint32                  device_id;
  uint32                  start_pba;
  uint32                  last_avail_pba;
  uint32                  last_erased_block;
  uint32                  last_written_page;
}dp_nand_data;

/*
  Defined this structure to remove DAL FW layer dependency
*/
struct flash_mem_desc_buf{
  uint32            dwObjInfo;
  uint32            hOwnerProc;
  DALSYSMemAddr     thisVirtualAddr;
  DALSYSMemAddr     PhysicalAddr;
  DALSYSMemAddr     VirtualAddr;
  uint32            dwCurBufIdx;
  uint32            dwNumDescBufs;
  DALSysMemDescBuf  BufInfo[DEVPRG_STORAGE_NAND_MAX_NUM_BUFS];
};

/*
  Utility variables which will be initialized on open() and then
  referred till close() is called.
  Note: these utility variable once initialized should not be changed,
        so made them const to avoid changing them unknowingly.
*/
static const uint32 nand_total_page_size; /* page size including spare */
static const uint32 nand_page_size; /* page size excluding spare */
static const uint32 nand_pages_per_block;

/******************************************************************************
 * Local Functions
 *****************************************************************************/

/* initialize page data descriptors */
static __inline dp_res_t  devprg_storage_nand_init_buf_desc
                          (
                            struct flash_mem_desc_buf *desc,
                            struct flash_page_iovec  *iovec,
                            void *main_va, void *spare_va
                          )
{
  DALSysMemDescBuf *main = NULL, *spare = NULL;

  if(!desc || !iovec || !main_va)
  {
    DDP_LOGE("[%d] NULL pointer passed desc(%p) iovec(%p) main(%p) spare(%p)\n",
            __LINE__, desc, iovec, main_va, spare_va);
    return DEVPRG_ERROR_STORAGE;
  }
  else
  {
    desc->dwObjInfo        = DALSYSCMN_MEM_DESC_OBJECT_ID;
    desc->hOwnerProc       = 0xFFFFFFFF;
    desc->PhysicalAddr     = DALSYS_MEM_ADDR_NOT_SPECIFIED;
    desc->VirtualAddr      = DALSYS_MEM_ADDR_NOT_SPECIFIED;

    desc->hOwnerProc       &= ~3;
    desc->thisVirtualAddr  = (DALSYSMemAddr)desc;
    desc->dwNumDescBufs    = 2; /* 0-main and 1-spare */
    desc->dwCurBufIdx      = 0; /* start with main buffer */

    main = &desc->BufInfo[DEVPRG_STORAGE_NAND_MAIN_BUF_DESC];
    memset((void*)main, -1, sizeof(DALSysMemDescBuf));
    main->VirtualAddr = (DALSYSMemAddr)main_va;
    main->user = iovec->total_page_count;
    main->size = iovec->main_size_bytes;

    spare = &desc->BufInfo[DEVPRG_STORAGE_NAND_SPARE_BUF_DESC];
    memset((void*)spare,-1, sizeof(DALSysMemDescBuf));
    spare->VirtualAddr = (DALSYSMemAddr)spare_va;
    spare->user = iovec->total_page_count;
    spare->size = iovec->spare_size_bytes;
  }
  return DEVPRG_SUCCESS;
}

static dp_res_t translate_flash_error(int fresult)
{
  dp_res_t error;

  switch(fresult)
  {
    case FLASH_DEVICE_DONE:
    {
      error = DEVPRG_SUCCESS;
    }
    break;
    case FLASH_DEVICE_NOT_SUPPORTED:
    {
      error = DEVPRG_ERROR_NOT_SUPPORTED;
    }
    break;
    case FLASH_DEVICE_INVALID_PARAMETER:
    {
      error = DEVPRG_ERROR_INVAL_PARAM;
    }
    break;
    case FLASH_DEVICE_NO_MEM:
    {
      error = DEVPRG_ERROR_NOMEM;
    }
    break;
    case FLASH_DEVICE_FAIL:
    case FLASH_DEVICE_CP_READ_FAIL:
    case FLASH_DEVICE_CP_WRITE_FAIL:
    case FLASH_DEVICE_FAIL_PAGE_ERASED:
    case FLASH_DEVICE_OP_INPROGRESS:
    case FLASH_DEVICE_PRGM_FAIL_PLANE1:
    case FLASH_DEVICE_PRGM_FAIL_PLANE2:
    case FLASH_DEVICE_PRGM_FAIL_BOTH_PLANES:
    default:
    {
      error = DEVPRG_ERROR_STORAGE;
    }
    break;
  }

  if(error != DEVPRG_SUCCESS)
  {
    DDP_LOGW("[%d]NAND Error %d (%d)", __LINE__, fresult, error);
  }

  return error;
}

static dp_res_t nand_update_bbt(struct dp_storage_nand *dev)
{
  int fresult = 0;
  enum flash_block_state block_state;
  uint32 bbt_size = 0, block = 0, end_block = 0;

  if(!dev)
  {
    DDP_LOGE("[%d] dev ptr in NULL\n", __LINE__);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  DDP_LOGI("[%d] bbt_size:%x blocks:%x\n", __LINE__, DEVPRG_STORAGE_NAND_BBT_SIZE(dev->total_blocks), dev->total_blocks);
  bbt_size = DEVPRG_STORAGE_NAND_BBT_SIZE(dev->total_blocks);
  DDP_LOGI("[%d] bbt_size:%x blocks:%x\n", __LINE__, bbt_size, dev->total_blocks);
  if(!bbt_size)
  {
    DDP_LOGE("[%d] dev ptr in NULL\n", __LINE__);
    return DEVPRG_ERROR_INVAL_PARAM;
  }
  if((DALSYS_Malloc(bbt_size, (void **)&dev->bbt) != DAL_SUCCESS) || !dev->bbt)
  {
    DDP_LOGE("[%d] malloc failed for size : %d\n", __LINE__, bbt_size);
    return DEVPRG_ERROR_NOMEM;
  }

  memset(dev->bbt, 0, bbt_size);
  end_block = block + dev->total_blocks;

  /* block = 0 :-> start block of this partition */
  for (block = 0; block < end_block ; block++ )
  {
    fresult = flash_block_get_state(dev->handle, block, &block_state);
    if(DAL_SUCCESS == fresult)
    {
      if(block_state != FLASH_BLOCK_OK)
      {
        /* mark block as bad in bbt */
        DEVPRG_STORAGE_NAND_MARK_BADBLOCK(dev->bbt, block);
        dev->total_blocks -= 1;
      }
    }
    else
    {
      DDP_LOGE("[%d]dev(%p) failed to get state for block(%d) with error %d\n",
              __LINE__, dev, block, fresult);
      return DEVPRG_ERROR_STORAGE;
    }
  }

  return DEVPRG_SUCCESS;
}

/*
  returns a good phys block for requested logical block.
  This func assumes that bbt table has correct info on bad blocks
  and that there are no new bad blocks apart from the ones mentioned
  in dev->bbt.
  Explanation on finding good block :
  Basically we skip the bad blocks and count the good blocks to give logical
  to physical mapping. Here physical doesn't mean physical address on nand.
  Physical means that if logical block is 10th then actual block in nand
  could be 11 or 20 depending on the bad blocks from 0 to 10.

  e.g.
    in a partition with 10 actual blocks :
    if block numbers 3, 5 and 6 are bad then below would be lba to pba mapping :
    lba-pba : 0-0, 1-1, 2-2, 3-4, 4-7, 5-8, 6-9

    3rd logical block would be 4th phys block,
    similarly 4th logical block would be 7th phys block.
*/
static dp_res_t nand_get_pba_from_lba(struct dp_storage_nand *dev,
                                      uint64 lba, uint32 *pba)
{
  uint32 block_num = 0, i = 0;

  if((NULL == dev) || (NULL == pba) || (lba > dev->last_avail_pba))
  {
    DDP_LOGE("[%d] invalid params passed\n", __LINE__);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  /* get phys block (count good blocks and skip bad blocks) */
  for(i = dev->start_pba, block_num = dev->start_pba; i <= dev->last_avail_pba; i++)
  {
    if(FALSE == DEVPRG_STORAGE_NAND_BLOCK_IS_BAD(dev->bbt, i))
    {
      /* if good block and didn't match lba then count till we get our block */
      if(block_num != lba)
      {
        block_num++;
      }
      else
      {
        *pba = i;
        return DEVPRG_SUCCESS;
      }
    }
  }

  /* not enough good blocks in the given partition */
  DDP_LOGI("[%d] out of good blocks, total blocks:%x Gblocks:%x lba:%x\n",
          __LINE__, dev->last_avail_pba, block_num, lba);
  return DEVPRG_ERROR_OUT_OF_GOOD_BLOCKS;
}

static dp_res_t nand_mark_bad_block(struct dp_storage_nand *dev, uint64 block)
{
  dp_res_t error;
  int fresult = FLASH_DEVICE_DONE;

  if(!dev || block >= dev->total_part_blocks)
  {
    DDP_LOGE("[%d] invalid params passed\n", __LINE__);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  fresult = flash_block_set_state( dev->handle, block, FLASH_BLOCK_BAD);
  if (fresult != FLASH_DEVICE_DONE)
  {
    error = translate_flash_error(fresult);
    DDP_LOGE("[%d] failed to set block %d as bad: fresult- %d, error- %d \n",
             __LINE__, block, fresult, error);
  }
  else
  {
    error = DEVPRG_SUCCESS;
  }

  /* mark block as bad in our BBT anyways */
  DEVPRG_STORAGE_NAND_MARK_BADBLOCK(dev->bbt, block);

  return error;
}

static dp_res_t nand_bad_block_backup(struct dp_storage_nand *dev,
                                      uint32 logical_bad_block,
                                      uint64 phys_failed_page,
                                      uint32 *backed_up_block_ptr)
{
  int fresult;
  dp_res_t error = DEVPRG_SUCCESS;
  char *buffer = NULL;
  struct flash_page_iovec  page_data_iovec;
  struct flash_mem_desc_buf page_buff_desc;
  uint32 done = 0, i = 0, phys_good_block = 0, logical_good_block = 0;
  uint64 num_pages_to_write = 1;
  uint64 bad_block_start_page = 0, phys_good_block_start_page = 0;

  if(!backed_up_block_ptr || !dev)
  {
    DDP_LOGE("[%d] backed_up_block_ptr(%p) dev(%p) invalid \n",
            __LINE__, backed_up_block_ptr, dev);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  if((DALSYS_Malloc(nand_page_size, (void **)&buffer) != DAL_SUCCESS)
      || !buffer)
  {
    DDP_LOGE("[%d] malloc failed for size %d\n", __LINE__, nand_page_size);
    return DEVPRG_ERROR_NOMEM;
  }

  bad_block_start_page = phys_failed_page/nand_pages_per_block;
  num_pages_to_write = (phys_failed_page%nand_pages_per_block);

  INIT_PAGE_DATA_IOVEC( page_data_iovec, nand_page_size, 0/* 0 spare size*/,
                        bad_block_start_page, 1/* 1 page at a time */);

  error = devprg_storage_nand_init_buf_desc(&page_buff_desc, &page_data_iovec,
                                            buffer, NULL);
  if(error != DEVPRG_SUCCESS)
  {
    DALSYS_Free(buffer);
    return DEVPRG_ERROR_STORAGE;
  }

  logical_good_block = logical_bad_block + 1;

  do
  {
    if(dev->last_erased_block < dev->last_avail_pba)
    {
      /*
          suppose write() was called to program 4 blocks, so first it erased 4 blocks
          and then started writing 4 blocks, but while writing 3rd block failed
          so we mark 3rd block as bad and write its data into 4th block and data of 4th
          block in 5th block. but 5th block was not erased so writing to it can result in failure.
          for this purpose we erase 1 extra block here. TODO: better place would be in write() itself
      */
      error = devprg_storage_nand_erase(dev,
                              (dev->last_erased_block+1)*nand_pages_per_block,
                              nand_pages_per_block);
      if(error != DEVPRG_SUCCESS)
      {
        DALSYS_Free(buffer);
        return error;
      }
    }

    /*
      get next good block, bad_block is logical block that has gone bad
       however as bad_block is not marked as bad in bbt
       we need to use bad_block+1 to search next physical good block
    */
    error = nand_get_pba_from_lba(dev, logical_good_block, &phys_good_block);
    if(error != DEVPRG_SUCCESS)
    {
      DDP_LOGE("[%d] error : %d \n", __LINE__, error);
      DALSYS_Free(buffer);
      return error;
    }

    phys_good_block_start_page = phys_good_block * nand_pages_per_block;

    for(i = 0; i < num_pages_to_write; i++)
    {
      /* =========== READ DATA FROM BAD BLOCK 1 PAGE AT A TIME =========== */
      page_data_iovec.start_page = bad_block_start_page + i;
      fresult = flash_read_pages( dev->handle, FLASH_READ_MAIN,
                                  (dalsys_mem_desc_list*)&page_buff_desc,
                                  &page_data_iovec);
      if (fresult != FLASH_DEVICE_DONE)
      {
        if(fresult != FLASH_DEVICE_FAIL_PAGE_ERASED)
        {
          DDP_LOGE("[%d] fresult : %d\n", __LINE__, fresult);
          DALSYS_Free(buffer);
          return  DEVPRG_ERROR_STORAGE;
        }
        else  /* if(FLASH_DEVICE_FAIL_PAGE_ERASED == fresult) */
        {
          DDP_LOGI("[%d] ignoring erased page\n", __LINE__);
          continue;
        }
      }
      /*===================================================================*/

      /* =========== WRITE DATA TO GOOD BLOCK 1 PAGE AT A TIME =========== */
      page_data_iovec.start_page = phys_good_block_start_page + i;
      fresult = flash_write_pages(dev->handle, FLASH_WRITE_MAIN,
                                  (dalsys_mem_desc_list*)&page_buff_desc,
                                  &page_data_iovec);
      if (fresult != FLASH_DEVICE_DONE)
      {
        if(FLASH_DEVICE_INVALID_PARAMETER == fresult)
        {
          DDP_LOGE("[%d] invalid param passed\n", __LINE__);
          DALSYS_Free(buffer);
          return DEVPRG_ERROR_INVAL_PARAM;
        }
        else
        {
          /* the block assumed to be good for write has gone bad
              mark it as bad and continue to the next good block
              for write
          */
          DDP_LOGW("[%d] mark block %d as bad as continue with next block \n",
                  __LINE__, phys_good_block);
          nand_mark_bad_block(dev, phys_good_block);
          break;
        }
      }
      /*===================================================================*/
    }

    if(num_pages_to_write && (i != num_pages_to_write))
    {
      /* find next good block to backup data */
      done = 0;
    }
    else
    {
      done = 1;
    }
  }while(!done);

  *backed_up_block_ptr = phys_good_block;
  DALSYS_Free(buffer);
  return DEVPRG_SUCCESS;
}

/*****************************************************************************
 * APIs
 *****************************************************************************/

void devprg_storage_nand_init(void)
{
  dp_nand_data.handle = NULL;
  dp_nand_data.in_use = 0;

  /* Required for 9x07 and 9206 */
//  HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUT(0x00090404);
//  HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OUT(0x1C210841);


  DALSYS_InitMod(NULL);
  DALSYS_HeapInit(dp_flash_tools_heap, FLASH_TOOLS_HEAP_SIZE, 1);

}

dp_res_t devprg_storage_nand_deinit(void)
{
  struct dp_storage_nand *dev = &dp_nand_data;
  int fresult = 0;

  /*
    devprg will call open and close for every read/write/erase operation.
    so to avoid initializing everthing in open() call,
    moved below part from close to deinit.
    as in_use flag will always be set until deinit is not called,
    unnecessary initialization to flash will be avoided.
  */
  dev->in_use = 0;
  fresult = flash_device_close(dev->handle);
  if(fresult != FLASH_DEVICE_DONE)
  {
    DDP_LOGE("[%d] device close failed, fresult : %d \n", __LINE__, fresult);
    return translate_flash_error(fresult);
  }

  fresult = flash_device_detach(dev->handle);
  if(fresult != FLASH_DEVICE_DONE)
  {
    DDP_LOGE("[%d] device detach failed, fresult : %d \n", __LINE__, fresult);
    return translate_flash_error(fresult);
  }

  dev->handle = NULL;

  if(dev->bbt)
  {
    /* free bbt for each partition */
    DALSYS_Free(dev->bbt);
    dev->bbt = NULL;
  }

  return DEVPRG_SUCCESS;
}

void* devprg_storage_nand_open(uint32 slot, uint32 partition, dp_res_t *error)
{
  flash_handle_t handle;
  struct flash_info info;
  struct dp_storage_nand *dev = &dp_nand_data;
  int fresult = 0;
  uint32 *temp_total_page_size_ptr = (uint32*)&nand_total_page_size;
  uint32 *temp_page_size_ptr = (uint32*)&nand_page_size;
  uint32 *temp_pages_per_block_ptr = (uint32*)&nand_pages_per_block;

  if(slot)
  {
    DDP_LOGE("[%d]multiple devices not supported, slot : %d\n",
            __LINE__, slot);
    *error = DEVPRG_ERROR_INVAL_PARAM;
    return NULL;
  }

  if(partition)
  {
    DDP_LOGE("[%d]physical partition (%d) not supported only supported (0)\n",
            __LINE__, partition);
    *error = DEVPRG_ERROR_INVAL_PARAM;
    return NULL;
  }

  /* Device has already been open, return it. */
  if(1 == dev->in_use)
  {
    DDP_LOGI("NAND was previously open, returning existing handle %d\n", 0);
    error = DEVPRG_SUCCESS;
    return dev;
  }

  /* call attach to get a valid handle */
  fresult = flash_device_attach( DALDEVICEID_FLASH_DEVICE_1, &handle);
  if(fresult != DAL_SUCCESS)
  {
    *error = translate_flash_error(fresult);
    DDP_LOGE("[%d] device attach failed %d (%d)\n", __LINE__, fresult, *error);
    return NULL;
  }

  /* use the handle to open the partition */
  fresult = flash_open_partition( handle, "0:ALL");
  if(fresult != DAL_SUCCESS)
  {
    *error = translate_flash_error(fresult);
    DDP_LOGE("[%d] open partition failed %d (%d)\n", __LINE__, fresult, *error);
    return NULL;
  }

  fresult = flash_get_info(handle, FLASH_DEVICE_INFO, &info);
  if(fresult != FLASH_DEVICE_DONE)
  {
    flash_device_close(handle);
    *error = translate_flash_error(fresult);
    DDP_LOGE("[%d]get info failed %d (%d)\n", __LINE__, fresult, *error);
    return NULL;
  }

  /* check partition for good blocks and update bad block info in BBT */
  dev->handle = handle;
  dev->total_blocks = info.partition.block_count;
  dev->total_part_blocks = info.partition.block_count;
  *error = nand_update_bbt(dev);
  if(*error != DEVPRG_SUCCESS)
  {
    DDP_LOGW("[%d]update bbt failed %d\n", __LINE__, *error);
    return NULL;
  }

  /* initialize local context and utility variables */
  *temp_total_page_size_ptr = info.partition.total_page_size_bytes;
  *temp_page_size_ptr = info.partition.page_size_bytes;
  *temp_pages_per_block_ptr = info.partition.pages_per_block;

  dev->maker_id = info.maker_id;
  dev->device_id = info.device_id;

  DEVPRG_DEV_DEFAULT_INIT(dev);

  dev->in_use = 1;
  return dev;
}

dp_res_t devprg_storage_nand_close(void *device)
{
  struct dp_storage_nand *dev = (struct dp_storage_nand *)device;

  if(!dev || !dev->in_use)
  {
    DDP_LOGE("[%d] invalid params passed device : %p\n", __LINE__, device);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  /*
    initialize internal structures to their default value
    as this won't be done in the next open() call. refer deinit func.
    internal comments for more details.
  */
  DEVPRG_DEV_DEFAULT_INIT(dev);

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_nand_io_options(void *device,
                                   struct devprg_io_options *io_options)
{
  enum devprg_skip_bb skip_bb = io_options->skip_bb;
  enum devprg_get_spare get_spare = io_options->get_spare;
  enum devprg_ecc_state ecc_state = io_options->ecc_state;
  struct dp_storage_nand *dev = (struct dp_storage_nand *)device;

  if(!dev || !dev->in_use)
  {
    DDP_LOGE("[%d] invalid params passed device : %p\n", __LINE__, device);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  if(skip_bb != BAD_BLOCK_SKIP && skip_bb != BAD_BLOCK_FILL)
  {
    DDP_LOGE("[%d] invalid params passed \n", __LINE__);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  if(get_spare != DATA_ONLY && get_spare != DATA_AND_SPARE)
  {
    DDP_LOGE("[%d] invalid params passed \n", __LINE__);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  if(ecc_state != ECC_DISABLED && ecc_state != ECC_ENABLED)
  {
    DDP_LOGE("[%d] invalid params passed \n", __LINE__);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  dev->skip_bb = skip_bb;
  dev->get_spare = get_spare;
  dev->ecc_state = ecc_state;

  /* TODO: bound check these parameters */
  dev->start_pba = io_options->min_sector/nand_pages_per_block;

  dev->last_avail_pba = io_options->max_sector/nand_pages_per_block;
  if (dev->last_avail_pba > dev->total_part_blocks)
    dev->last_avail_pba = dev->total_part_blocks-1;

  return DEVPRG_SUCCESS;
}

/* Read pages from disk. There are different options that may alter
 * how pages are read.
 * - Skip bad blocks
 * - Get spare data
 * - ECC enabled/disabled
 *
 * These options will be provided by a different function and stored in the
 * handle. */
dp_res_t devprg_storage_nand_read(void *device, void *data, uint64 start_page,
                                  uint64 num_of_pages)
{
  struct dp_storage_nand *dev = (struct dp_storage_nand *)device;
  struct flash_page_iovec  page_data_iovec;
  struct flash_mem_desc_buf page_buff_desc;
  enum page_read_opcode read_type = FLASH_READ_MAIN;
  void *spare_buff_addr = (void*)DALSYS_MEM_ADDR_NOT_SPECIFIED;
  uint32 start_block_num = 0, tmp_block_num = 0, pba = 0;
  uint32 num_of_pages_in_bb = 0, actual_page_size = nand_page_size;
  uint64 page_count = 0, total_pages_to_read = num_of_pages, tmp_start_page;
  int fresult = FLASH_DEVICE_DONE, skip_bb = BAD_BLOCK_SKIP;
  dp_res_t error = DEVPRG_SUCCESS;

  if(!dev || !data || !num_of_pages || !dev->in_use)
  {
    DDP_LOGE("[%d] invalid params passed \n", __LINE__);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  if(dev->ecc_state != ECC_ENABLED && dev->ecc_state != ECC_DISABLED)
  {
    DDP_LOGE("[%d] ecc_state - %d not supported\n",
            __LINE__, dev->ecc_state);
    return DEVPRG_ERROR_NOT_SUPPORTED;
  }

  if(dev->get_spare != DATA_ONLY && dev->get_spare != DATA_AND_SPARE)
  {
    DDP_LOGE("[%d] get_spare - %d not supported\n",
            __LINE__, dev->skip_bb);
    return DEVPRG_ERROR_NOT_SUPPORTED;
  }

  if(dev->skip_bb != BAD_BLOCK_SKIP && dev->skip_bb != BAD_BLOCK_FILL)
  {
    DDP_LOGE("[%d] skip_bb - %d not supported\n",
            __LINE__, dev->skip_bb);
    return DEVPRG_ERROR_NOT_SUPPORTED;
  }
  else
  {
    if(BAD_BLOCK_FILL == dev->skip_bb)
    {
      skip_bb = BAD_BLOCK_FILL;
    }
  }

  tmp_start_page = start_page;

  if(DP_INVALID_BLOCK == dev->start_pba)
  {
    DDP_LOGE("[%d] IO options not set start_pba : %d\n",
            __LINE__, dev->start_pba);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  if(DATA_AND_SPARE == dev->get_spare || ECC_DISABLED == dev->ecc_state)
  {
    actual_page_size = nand_total_page_size;
    spare_buff_addr = (void*)((uint32)data+nand_page_size);
    if(dev->ecc_state != ECC_DISABLED)
    {
      read_type = FLASH_READ_MAIN_SPARE;
    }
    else
    {
      read_type = FLASH_READ_RAW;
    }
  }

  start_block_num = (tmp_start_page/nand_pages_per_block);
  error = nand_get_pba_from_lba(dev, start_block_num, &pba);
  if(error != DEVPRG_SUCCESS)
  {
    if(DEVPRG_ERROR_INVAL_PARAM == error)
    { /* params already verified and can't be invalid */
      DDP_LOGE("[%d] unexpected error\n", __LINE__);
      return DEVPRG_ERROR_STORAGE;
    }
    if(skip_bb != BAD_BLOCK_FILL)
    {
      DDP_LOGE("[%d] error : %d \n", __LINE__, error);
      return error;
    }
    else
    { /* fill buffer with pattern NAND_BB_PATTERN (BAD) */
      memset(data, NAND_BB_PATTERN, num_of_pages * actual_page_size);
      return DEVPRG_SUCCESS;
    }
  }

  if((BAD_BLOCK_FILL == skip_bb) && (pba - start_block_num))
  { /* fill buffer with pattern instead of skipping bad block */
    /* if pba doesn't match with start_block_num, it means bad blocks found */
    num_of_pages_in_bb = (pba * nand_pages_per_block) - tmp_start_page ;
    if(total_pages_to_read < num_of_pages_in_bb)
    {
      memset(data, NAND_BB_PATTERN, total_pages_to_read * actual_page_size);
      /* as all requested pages are in bad block we don't have
        anything else to read so return successfully */
      return DEVPRG_SUCCESS;
    }
    else
    {
      memset(data, NAND_BB_PATTERN, num_of_pages_in_bb*actual_page_size);
      tmp_start_page += num_of_pages_in_bb;
      total_pages_to_read -= num_of_pages_in_bb;
      data = (void*)((uint8*)data + num_of_pages_in_bb*actual_page_size);
      if(DATA_AND_SPARE == dev->get_spare || ECC_DISABLED == dev->ecc_state)
      {
        spare_buff_addr = (void*)((uint32)data+nand_page_size);
      }
    }
  }
  else
  {
    /* fix start page acc. to phys block */
    tmp_start_page += (pba-start_block_num)*nand_pages_per_block;
  }

  /*  if start_page is not block aligned then
      first reading till start of next block.
  */
#if DEVPRG_MULTI_RW_SUPPORT_ENABLED
  page_count = (pba+1)*nand_pages_per_block - tmp_start_page;
  if(page_count > total_pages_to_read)
  {
    page_count = total_pages_to_read;
  }
#else
  page_count = 1;
#endif

  /* Initialize the page data IOVEC structure */
  INIT_PAGE_DATA_IOVEC( page_data_iovec, actual_page_size,
                        (actual_page_size-nand_page_size)/*spare size*/,
                        tmp_start_page, page_count);

  /* Initialize buffer descriptors */
  error = devprg_storage_nand_init_buf_desc(&page_buff_desc, &page_data_iovec,
                                            data, spare_buff_addr);
  if(error != DEVPRG_SUCCESS)
  {
    return DEVPRG_ERROR_STORAGE;
  }

  while( total_pages_to_read )
  {
    fresult = flash_read_pages( dev->handle, read_type,
                                (dalsys_mem_desc_list*)&page_buff_desc,
                                &page_data_iovec);
    if (fresult != FLASH_DEVICE_DONE)
    {
      if(fresult != FLASH_DEVICE_FAIL_PAGE_ERASED)
      {
        DDP_LOGE("[%d]ignoring read failure with fresult : %d\n",
                  __LINE__, fresult);
        memset( (void*)NAND_MAIN_BUF_ADDR(page_buff_desc),
                NAND_FAILED_PAGE_PATTERN,
                page_data_iovec.total_page_count * actual_page_size);
      }
      else
      {
        memset( (void*)NAND_MAIN_BUF_ADDR(page_buff_desc),
                NAND_ERASED_PAGE_PATTERN,
                page_data_iovec.total_page_count * actual_page_size);
        DDP_LOGW("[%d] ignoring erased page read\n", __LINE__);
      }
    }

    total_pages_to_read -= page_count;  /* update src for next read */
    if(!total_pages_to_read)
    {
      break;  /* completed reading all pages */
    }

    /* NOTE : the new start_page will always be block aligned */
    page_data_iovec.start_page += page_count;

    /* check if new start page is in next block */
    tmp_block_num = page_data_iovec.start_page/nand_pages_per_block;
    if(tmp_block_num != pba)
    { /* reading from next block */
      start_block_num++;
      error = nand_get_pba_from_lba(dev, start_block_num, &pba);
      if(error != DEVPRG_SUCCESS)
      {
        if(DEVPRG_ERROR_INVAL_PARAM == error)
        { /* params already verified and can't be invalid */
          DDP_LOGE("[%d] unexpected error\n", __LINE__);
          return DEVPRG_ERROR_STORAGE;
        }
        /* out of bad block case */
        if(skip_bb != BAD_BLOCK_FILL)
        {
          DDP_LOGE("[%d] error : %d \n", __LINE__, error);
          return error;
        }
        else
        {
          /*  in case where BAD_BLOCK_FILL option is set
              fill with pattern NAND_BB_PATTERN */
          memset( (void*)NAND_MAIN_BUF_ADDR(page_buff_desc), NAND_BB_PATTERN,
                  total_pages_to_read * actual_page_size);
          return DEVPRG_SUCCESS;
        }
      }

      if((BAD_BLOCK_FILL == skip_bb) && (pba - start_block_num))
      {
        num_of_pages_in_bb = (pba * nand_pages_per_block)
                            - page_data_iovec.start_page ;
        if(total_pages_to_read < num_of_pages_in_bb)
        {
          memset( (void*)NAND_MAIN_BUF_ADDR(page_buff_desc),
                  NAND_BB_PATTERN, total_pages_to_read * actual_page_size);
          /* as all requested pages are in bad block we don't have
            anything else to read so return successfully */
          return DEVPRG_SUCCESS;
        }
        else
        {
          memset( (void*)NAND_MAIN_BUF_ADDR(page_buff_desc),
                  NAND_BB_PATTERN, num_of_pages_in_bb * actual_page_size);
          NAND_MAIN_BUF_ADDR(page_buff_desc)
            +=  num_of_pages_in_bb * actual_page_size;
          if((uint32)spare_buff_addr != DALSYS_MEM_ADDR_NOT_SPECIFIED)
          {
            NAND_SPARE_BUF_ADDR(page_buff_desc)
              += num_of_pages_in_bb * actual_page_size;
          }
          page_data_iovec.start_page += num_of_pages_in_bb;
          total_pages_to_read -= num_of_pages_in_bb;
        }
      }
      else
      {
        page_data_iovec.start_page += (pba-tmp_block_num)*nand_pages_per_block;
      }
    }

    NAND_MAIN_BUF_ADDR(page_buff_desc) += (actual_page_size*page_count);
    if((uint32)spare_buff_addr != DALSYS_MEM_ADDR_NOT_SPECIFIED)
    {
      NAND_SPARE_BUF_ADDR(page_buff_desc)+= (actual_page_size*page_count);
    }

    if(total_pages_to_read < DEVPRG_STORAGE_NAND_MAX_MULTI_PAGE)
    {
      page_count = total_pages_to_read;
    }
    else
    {
      page_count = DEVPRG_STORAGE_NAND_MAX_MULTI_PAGE;
    }

    /* update remaining pages to read */
    NAND_UPDATE_PAGE_COUNT(page_data_iovec, page_buff_desc, page_count);
  }

  return DEVPRG_SUCCESS;
}


/* Write the pages at a given sector. If a bad block happens, copy the
 * data of the current block to the next block, mark block as bad and
 * write the current page to the appropriate location. */
dp_res_t devprg_storage_nand_write( void *device, void *data,
                                    uint64 start_page, uint64 num_of_pages)
{
  struct dp_storage_nand *dev = (struct dp_storage_nand *)device;
  struct flash_page_iovec  page_data_iovec;
  uint32 start_block_num = 0, pba = 0, tmp_block_num = 0;
  uint64 total_pages_to_write = num_of_pages, page_count = 0;
  uint64 tmp_start_page = start_page;
  struct flash_mem_desc_buf page_buff_desc;
  int fresult = FLASH_DEVICE_DONE;
  dp_res_t error = DEVPRG_SUCCESS;
  enum page_write_opcode write_type = FLASH_WRITE_MAIN;

  if(!dev || !data || !num_of_pages || !dev->in_use)
  {
    DDP_LOGE("[%d] invalid params passed \n", __LINE__);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  if(dev->ecc_state != ECC_ENABLED)
  {
    DDP_LOGE("[%d] ecc_state - %d not supported\n",
            __LINE__, dev->ecc_state);
    return DEVPRG_ERROR_NOT_SUPPORTED;
  }

  if(dev->skip_bb != BAD_BLOCK_SKIP)
  {
    DDP_LOGE("[%d] skip_bb - %d not supported\n",
            __LINE__, dev->skip_bb);
    return DEVPRG_ERROR_NOT_SUPPORTED;
  }

  if(dev->get_spare != DATA_ONLY)
  {
    DDP_LOGE("[%d] get_spare - %d not supported\n",
            __LINE__, dev->skip_bb);
    return DEVPRG_ERROR_NOT_SUPPORTED;
  }

  if( (dev->last_written_page != DP_INVALID_PAGE) &&
      (tmp_start_page <= dev->last_written_page)  )
  {
    /* Only sequential writes allowed */
    DDP_LOGE("[%d] write before last_written_page : %d is not allowed \n",
            __LINE__, dev->last_written_page);
    return DEVPRG_ERROR_NOT_SUPPORTED;
  }

  /*=======================================================================*/
  /* from the given page addr get the block which it is in */
  start_block_num = tmp_start_page/nand_pages_per_block;
  if(DP_INVALID_BLOCK == dev->start_pba)
  {
    DDP_LOGE("[%d] IO options not set start_pba : %d\n",
            __LINE__, dev->start_pba);
    return DEVPRG_ERROR_INVAL_PARAM;
  }
  /*=======================================================================*/

  /*=======================================================================*/
  /* erase the block before starting a write */
  if( dev->last_erased_block != DP_INVALID_BLOCK &&
      start_block_num <= dev->last_erased_block)
  {
    /* 
      requested page falls in a block which is already erased,
      so no need to erase current start_block_num. However,
      we need to check if next block is to be written.
      if so then we need to erase the blocks which are to be written.
    */
    uint8 remain_pages = 0;/* remaining pages in blocks that aren't erased */
    remain_pages = nand_pages_per_block - (tmp_start_page % nand_pages_per_block);

    /* current block is erased so erase next block if required */
    if(remain_pages < num_of_pages)
    {
      /* request to write in next block/s so erase next block/s */
      error = devprg_storage_nand_erase
              (device, (tmp_start_page+remain_pages),
              ((num_of_pages-remain_pages)/nand_pages_per_block
                +((num_of_pages-remain_pages)%nand_pages_per_block)?
                nand_pages_per_block:0));
      if(error != DEVPRG_SUCCESS)
      {/* error already printed in erase func so not printing again */
        return error;
      }
    }
  }
  else
  {
    /*
      requested start page falls in a block which is not already erased
      check if start_page is the first page in the block.
    */
    if(tmp_start_page % nand_pages_per_block)
    {
      /* requested page is not first */
      DDP_LOGE("[%d] first write on a block should start with first page \
                and requested page (%d) is not first page of this block\n",
              __LINE__, tmp_start_page);
      return DEVPRG_ERROR_NOT_SUPPORTED;
    }

    error = devprg_storage_nand_erase
            (device, tmp_start_page,
            ((num_of_pages/nand_pages_per_block)
             +(num_of_pages%nand_pages_per_block)?nand_pages_per_block:0));
    if(error != DEVPRG_SUCCESS)
    {
      return error;
    }
  }
  /*=======================================================================*/

  /* get phys block (count good blocks and skip bad blocks) */
  error = nand_get_pba_from_lba(dev, start_block_num, &pba);
  if(error != DEVPRG_SUCCESS)
  {
    DDP_LOGE("[%d] error : %d \n", __LINE__, error);
    return error;
  }

  /* fix start page acc. to phys block */
  tmp_start_page += (pba-start_block_num)*nand_pages_per_block;
#if DEVPRG_MULTI_RW_SUPPORT_ENABLED
  page_count = (pba+1)*nand_pages_per_block - tmp_start_page;
  if(page_count > total_pages_to_write)
  {
    page_count = total_pages_to_write;
  }
#else
  page_count = 1;
#endif

  /* initialize sturts for flash write call */
  INIT_PAGE_DATA_IOVEC( page_data_iovec, nand_page_size, 0/* 0 spare size*/,
                        tmp_start_page, page_count);

  /* Initialize buffer descriptors*/
  error = devprg_storage_nand_init_buf_desc(&page_buff_desc, &page_data_iovec,
                                            data, NULL);
  if(error != DEVPRG_SUCCESS)
  {
    return DEVPRG_ERROR_STORAGE;
  }

  while( total_pages_to_write )
  {
    fresult = flash_write_pages(dev->handle, write_type,
                                (dalsys_mem_desc_list*)&page_buff_desc,
                                &page_data_iovec);
    if(fresult != FLASH_DEVICE_DONE)
    {
      if(FLASH_DEVICE_INVALID_PARAMETER == fresult)
      {
        DDP_LOGE("[%d] invalid param passed\n", __LINE__);
        return DEVPRG_ERROR_INVAL_PARAM;
      }

      DDP_LOGW("[%d] skip bad block %d and continue with next good block\n",
               __LINE__, start_block_num);
      tmp_block_num = page_data_iovec.start_page/nand_pages_per_block;

      /*
        copy data from the block gone bad into the next good block
        also retrive the updated good block number
      */
      error = nand_bad_block_backup(dev, start_block_num,
                                    page_data_iovec.start_page,
                                    &pba);
      if(error != DEVPRG_SUCCESS)
      {
        /* can't get next good block */
        return error;
      }

      /*  data backed up so mark given block as bad in BBT and NAND */
      if(nand_mark_bad_block(dev, tmp_block_num) != DEVPRG_SUCCESS)
      {
        return error;
      }

      /* reset and copy pages already written in bad block to a good block */
      page_data_iovec.start_page += (pba-tmp_block_num)*nand_pages_per_block;
    }
    else
    {
      /* update src for next write */
      total_pages_to_write -= page_count;
      if(0 == total_pages_to_write)
      {
        break;
      }

      /* check if next page falls in next block */
      page_data_iovec.start_page += page_count;
      tmp_block_num = page_data_iovec.start_page/nand_pages_per_block;
      if(pba != tmp_block_num)
      {
        start_block_num++;
        error = nand_get_pba_from_lba(dev, start_block_num, &pba);
        if(error != DEVPRG_SUCCESS)
        {
          DDP_LOGE("[%d] error : %d \n", __LINE__, error);
          return error;
        }
        page_data_iovec.start_page+=(pba-tmp_block_num)*nand_pages_per_block;
      }

      /* update dest for next read */
      NAND_MAIN_BUF_ADDR(page_buff_desc) += (nand_page_size*page_count);

      if(total_pages_to_write < DEVPRG_STORAGE_NAND_MAX_MULTI_PAGE)
      {
        page_count = total_pages_to_write;
      }
      else
      {
        page_count = DEVPRG_STORAGE_NAND_MAX_MULTI_PAGE;
      }

      /* update remaining pages to write */
      NAND_UPDATE_PAGE_COUNT(page_data_iovec, page_buff_desc, page_count);
    }
  }
  dev->last_written_page = start_page + num_of_pages - 1;
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_nand_erase(void *device, uint64 start_page,
                                   uint64 num_of_pages)
{
  uint64 i = 0;
  uint64 result_vector[1] = {0};
  uint32 pba = 0, erase_block = 0, erase_block_count = 0;
  struct flash_block_vector block_vector = {0,};
  struct dp_storage_nand *dev = (struct dp_storage_nand *)device;
  int fresult = 0;
  dp_res_t error = DEVPRG_SUCCESS;
  uint64 max_avail_blocks = (dev->last_avail_pba-dev->start_pba) + 1;

  if(!dev || !dev->in_use
    || !num_of_pages || (num_of_pages%nand_pages_per_block))
  {
    DDP_LOGE("[%d] invalid parameter passed\n", __LINE__);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  if(!max_avail_blocks)
  {
    DDP_LOGE("[%d] IO options not set start_pba : %d\n",
            __LINE__, dev->start_pba);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  if(start_page % nand_pages_per_block)
  {
    DDP_LOGE("[%d] for erase start_page(%d) should be first page of block\n",
              __LINE__, start_page);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  erase_block = start_page / nand_pages_per_block;
  if(DP_INVALID_BLOCK == dev->start_pba)
  {
    DDP_LOGE("[%d] IO options not set start_pba : %d\n",
            __LINE__, dev->start_pba);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  erase_block_count = (num_of_pages%nand_pages_per_block)?1:0
                      + num_of_pages/nand_pages_per_block;

  for(i = 0; i < max_avail_blocks; i++)
  {
    /* get actual block to erase */
    error = nand_get_pba_from_lba(dev, erase_block, &pba);
    if(error != DEVPRG_SUCCESS)
    {
      DDP_LOGE("[%d] error : %d \n", __LINE__, error);
      break;
    }

    block_vector.start_block = pba;
    block_vector.block_count = 1;
    /* TODO: use below struct to optimize in future */
    block_vector.result_vector = (int *)&result_vector[0];

    fresult = flash_erase_blocks(dev->handle, &block_vector, 1);
    if(fresult != FLASH_DEVICE_DONE)
    {
      if(fresult != FLASH_DEVICE_INVALID_PARAMETER)
      {
        /* erase block failed so mark it as bad */
        nand_mark_bad_block(dev, pba);
      }
      else
      {
        /* something went wrong or invalid parameter passed */
        DDP_LOGE("[%d] error: internal\n", __LINE__);
        error = DEVPRG_ERROR_INVAL_PARAM;
        break;
      }
    }
    else
    {
      /* update the number of blocks successfully erased */
      erase_block_count--;
      error = DEVPRG_SUCCESS;
      if(erase_block_count)
      {
        /* use next block */
        erase_block++;
      }
      else
      {
        /* all blocks erased */
        /* save last erased block to be used in write API */
        dev->last_erased_block = pba;
        break;
      }
    }
  }

  if((DEVPRG_SUCCESS == error) && erase_block_count)
  {
    /* out of good blocks */
    DDP_LOGE("[%d] error: out of good block\n", __LINE__);
    return DEVPRG_ERROR_OUT_OF_GOOD_BLOCKS;
  }

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_nand_format(void *device)
{

  struct dp_storage_nand *dev = (struct dp_storage_nand *)device;
  dp_res_t error = DEVPRG_ERROR_STORAGE;
  struct devprg_io_options io_opt;

  io_opt.skip_bb = BAD_BLOCK_SKIP;
  io_opt.get_spare = DATA_ONLY;
  io_opt.ecc_state = ECC_ENABLED;
  io_opt.min_sector = 0;
  io_opt.max_sector = (dev->total_part_blocks * nand_pages_per_block) - 1;


  if(!dev)
  {
    DDP_LOGE("[%d] invalid parameter passed\n", __LINE__);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  if(!dev->in_use)
  {
    return DEVPRG_ERROR_NOT_INIT;
  }

  error = devprg_storage_nand_io_options(device, &io_opt);
  if(error != DEVPRG_SUCCESS)
  {
    DDP_LOGE("[%d] failed to set io options\n", __LINE__);
    return error;
  }

  DDP_LOGI("NAND Erasing whole disk of %d blocks\n", dev->total_blocks);
  error = devprg_storage_nand_erase(device, 0,  /* start from 0th sector */
                                    dev->total_blocks * nand_pages_per_block);

  return error;
}

dp_res_t devprg_storage_nand_set_bootable(void *device, int enable)
{
  (void) device;
  (void) enable;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

dp_res_t devprg_storage_nand_get_info(void *device,
                                      struct devprg_storage_info *dev_info)
{
  struct dp_storage_nand *dev = (struct dp_storage_nand *)device;

  if(!dev || !dev->in_use)
  {
    DDP_LOGE("[%d] invalid parameter passed\n", __LINE__);
    return DEVPRG_ERROR_INVAL_PARAM;
  }

  dev_info->total_blocks    = (uint64)dev->total_blocks;

  /***************************************************************************/
  /* NOTE : do not change sequence of setting page_size and block_size here */
  if(DATA_AND_SPARE == dev->get_spare)
  {
    dev_info->page_size       = nand_total_page_size;
  }
  else
  {
    dev_info->page_size       = nand_page_size;
  }
  dev_info->block_size      = dev_info->page_size * nand_pages_per_block;
  /***************************************************************************/

  dev_info->num_physical    = 1;  /* 0:ALL just 1 partition TODO: 0 or 1 ? */
  dev_info->manufacturer_id = dev->maker_id;
  dev_info->serial_num      = dev->device_id;
  dev_info->fw_version[0]   = 0;
  strlcpy(dev_info->memory_type, "NAND", sizeof(dev_info->memory_type));
  dev_info->product_name[0] = 0;

  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_nand_configure(uint32 slot,
                                       struct devprg_storage_cfg_data *config)
{
  (void) slot;
  (void) config;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

dp_res_t devprg_storage_nand_fw_update(uint32 slot, void *buffer,
                                       uint32 size_in_bytes)
{
  (void) slot;
  (void) buffer;
  (void) size_in_bytes;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

dp_res_t devprg_storage_nand_info_raw_size(void *device, uint32 *size)
{
  (void) device;
  *size = 0;
  return DEVPRG_SUCCESS;
}

dp_res_t devprg_storage_nand_info_raw(void *device, void *buffer,
                                      uint32 *size)
{
  (void) device;
  (void) buffer;
  *size = 0;
  return DEVPRG_ERROR_NOT_SUPPORTED;
}

dp_res_t devprg_storage_nand_info_raw_print(void *device, void *buffer,
                                            uint32 *size)
{
  (void) device;
  (void) buffer;
  *size = 0;
  return DEVPRG_SUCCESS;
}

#else /* }{ If NAND Support is disabled (FEATURE_DEVPRG_NAND not defined)*/

#include "devprg_storage_stubs.h"
void devprg_storage_nand_init(void)
{
  devprg_storage_stub_init();
}

dp_res_t devprg_storage_nand_deinit(void)
{
  return devprg_storage_stub_deinit();
}

void *devprg_storage_nand_open(uint32 slot, uint32 partition,
                               dp_res_t *error)
{
  return devprg_storage_stub_open(slot, partition, error);
}

dp_res_t devprg_storage_nand_close(void *device)
{
  return devprg_storage_stub_close(device);
}

dp_res_t devprg_storage_nand_io_options(void *device,
                                        struct devprg_io_options *io_options)
{
  return devprg_storage_stub_io_options(device, io_options);
}

dp_res_t devprg_storage_nand_read(void *device, void *buffer,
                                  uint64 start_sector, uint64 num_sectors)
{
  return devprg_storage_stub_read(device, buffer, start_sector, num_sectors);
}

dp_res_t devprg_storage_nand_write(void *device, void *buffer,
                                   uint64 start_sector, uint64 num_sectors)
{
  return devprg_storage_stub_write(device, buffer, start_sector, num_sectors);
}

dp_res_t devprg_storage_nand_erase(void *device,
                                   uint64 start_sector, uint64 num_sectors)
{
  return devprg_storage_stub_erase(device, start_sector, num_sectors);
}

dp_res_t devprg_storage_nand_format(void *device)
{
  return devprg_storage_stub_format(device);
}

dp_res_t devprg_storage_nand_set_bootable(void *device, int enable)
{
  return devprg_storage_stub_set_bootable(device, enable);
}

dp_res_t devprg_storage_nand_get_info(void *device,
                                      struct devprg_storage_info *dev_info)
{
  return devprg_storage_stub_get_info(device, dev_info);
}

dp_res_t devprg_storage_nand_configure(uint32 slot,
                                       struct devprg_storage_cfg_data *config)
{
  return devprg_storage_stub_configure(slot, config);
}

dp_res_t devprg_storage_nand_fw_update(uint32 slot, void *buffer,
                                       uint32 size_in_bytes)
{
  return devprg_storage_stub_fw_update(slot, buffer, size_in_bytes);
}

dp_res_t devprg_storage_nand_info_raw_size(void *device, uint32 *size)
{
  return devprg_storage_stub_info_raw_size(device, size);
}

dp_res_t devprg_storage_nand_info_raw(void *device, void *buffer,
                                      uint32 *size)
{
  return devprg_storage_stub_info_raw(device, buffer, size);
}
dp_res_t devprg_storage_nand_info_raw_print(void *device, void *buffer,
                                            uint32 *size)
{
  return devprg_storage_stub_info_raw_print(device, buffer, size);
}


#endif /*  } FEATURE_DEVPRG_NAND */
