/*============================================================================
  @file sns_sdc_init.c

  @brief
  Load the sdc elf and maps the SDC TCM to SLPI virtual memory.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================*/

/*============================================================================
  INCLUDES
  ============================================================================*/
#include "sns_assert.h"
#include "sns_err.h"
#include "sns_hwio_sdc.h"
#include "sns_memmgr.h"
#include "sns_printf_int.h"
#include "sns_rc.h"
#include "sns_sdc.h"
#include "sns_timer.h"

#include <string.h>
#include "err.h"
#include "fcntl.h"
#include "msg_diag_service.h"
#include "msg_mask.h"
#include "qurt_memory.h"

#ifdef SSC_TARGET_SDC_AVAILABLE
#include "sns_osa_target.h"

/*============================================================================
  Constants
  ============================================================================*/
#define SSC_SDC_PHYSPOOL_NAME    "SSC_SDC"

#define VPAGE_SIZE               0x1000

//The binaries are created by sdc_splitter.py from sdc elf.
//qurt-image-build.py stitch these to bootimage using ramfs
#define SDC_CODE_BIN_NAME        "sdc_code_tcm.bin"
#define SDC_DATA_BIN_NAME        "sdc_data_tcm.bin"

/*============================================================================
  Forward declarations
  ============================================================================*/
void qurt_use_qurtfs( int );
int  qfsys_open(const char *name, int flags);
int  qfsys_read(int fd, void *buf, int len);

/*============================================================================
  static data
  ============================================================================*/
void *ssc_sdc_vaddr;

/*============================================================================
  Helper Functions
  ============================================================================*/
/* ------------------------------------------------------------------------------------ */
static uint32_t sns_conv_sdc_to_vaddr( uint32_t addr )
{
  return ( (ssc_sdc_vaddr)? ((uint32_t)ssc_sdc_vaddr + addr): NULL );
}

static void sns_tcm_clk_config( bool awake )
{
  if (awake)
  {
    HWIO_OUTF(SSC_SCC_SDC_BUS_HCLK_CBCR, CLK_ENABLE, 0x1);
    HWIO_OUTF(SSC_SCC_SDC_BUS_HCLK_CBCR, HW_CTL, 0x0);
  }
  else
  {
    HWIO_OUTF(SSC_SCC_SDC_BUS_HCLK_CBCR, HW_CTL, 0x1);
    HWIO_OUTF(SSC_SCC_SDC_BUS_HCLK_CBCR, CLK_ENABLE, 0x0);
  }
}

/* ------------------------------------------------------------------------------------ */
static void *sns_sdc_map_vaddr( void )
{
  qurt_mem_pool_t        ssc_sdc_phys_pool;
  qurt_mem_pool_attr_t   ssc_sdc_pool_attr;
  unsigned int           ssc_sdc_pool_size;
  qurt_mem_region_t      region;
  qurt_mem_region_attr_t attr;
  qurt_paddr_t           ssc_sdc_base_paddr;
  unsigned int           virt_addr;
  int                    ret;

  ret = qurt_mem_pool_attach(SSC_SDC_PHYSPOOL_NAME, &ssc_sdc_phys_pool);
  SNS_ASSERT(ret == QURT_EOK);
  ret = qurt_mem_pool_attr_get(ssc_sdc_phys_pool, &ssc_sdc_pool_attr);
  SNS_ASSERT(ret == QURT_EOK);
  ret = qurt_mem_pool_attr_get_addr(&ssc_sdc_pool_attr, 0, &ssc_sdc_base_paddr);
  SNS_ASSERT(ret == QURT_EOK);
  ssc_sdc_pool_size = ssc_sdc_pool_attr.ranges[0].size; 
  SNS_ASSERT(ssc_sdc_pool_size != 0);

  /* Reserve the VA space of the required size*/
  qurt_mem_region_attr_init(&attr);
  qurt_mem_region_attr_set_mapping(&attr, QURT_MEM_MAPPING_NONE);
 
  ret = qurt_mem_region_create(&region, ssc_sdc_pool_size, ssc_sdc_phys_pool, &attr);
  if(ret != QURT_EOK) {
    ERR_FATAL("qurt_mem_region_create failed! phys_addr=%p, size=%d, ret=%d",
                ssc_sdc_base_paddr, ssc_sdc_pool_size, ret);
    return NULL;
  }

  /* Get the allocated VA */
  ret = qurt_mem_region_attr_get(region, &attr);
  if(ret != QURT_EOK) {
    ERR_FATAL("qurt_mem_region_get_attr failed! phys_addr=%p, size=%d, ret=%d",
                ssc_sdc_base_paddr, ssc_sdc_pool_size, ret);
    return NULL;
  }

  qurt_mem_region_attr_get_virtaddr(&attr, &virt_addr);

  /* Create the VA to PA mapping */
  ret = qurt_mapping_create(virt_addr, ssc_sdc_base_paddr, ssc_sdc_pool_size,
                            QURT_MEM_CACHE_DEVICE, QURT_PERM_READ | QURT_PERM_WRITE);
  if(ret != QURT_EOK) {
      ERR_FATAL("qurt_mapping_create failed! phys_addr=%p, virt_addr=%d, ret=%d",
                 ssc_sdc_base_paddr, virt_addr, ret);
      return NULL;
   }

  return (void *)(virt_addr);
}

static int ramfs_read(int fd, void *buf, int len)
{
  int bytes_not_read;

  bytes_not_read = qfsys_read(fd, buf, len); 
  if(bytes_not_read < 0)
  {
    return -1;
  }

  return (len - bytes_not_read);
}

static sns_rc load_sdc_tcm_binary(const char *name, void *start_addr, uint32_t tcm_size)
{
  int  fd;
  int  read_bytes;
  void *tcm_buf = start_addr;

  if(!name || !start_addr)
  {
    return SNS_RC_INVALID_VALUE;
  }

  memset(start_addr, 0, tcm_size);

  qurt_use_qurtfs(1);

  fd = qfsys_open(name, POSIX_O_RDONLY);
  if(fd < 0)
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "open =%s", name);
    return SNS_RC_FAILED;
  }

  do {
    read_bytes = ramfs_read(fd, tcm_buf, VPAGE_SIZE);
    if(read_bytes < 0)
    {
      SNS_SPRINTF(ERROR, sns_fw_printf, "read =%s", name);
      return SNS_RC_FAILED;
    }

    tcm_buf = (void *)((uint32_t)tcm_buf + read_bytes);
  } while (read_bytes > 0);

  qurt_use_qurtfs(0);

  return SNS_RC_SUCCESS;
}

/*============================================================================
  Public   Functions
  ============================================================================*/

/* ------------------------------------------------------------------------------------ */
sns_rc sns_load_sdc_bins( void )
{
  void *code_ram_addr;
  void *data_ram_addr;
  sns_rc rc = SNS_RC_SUCCESS;
  const sns_time one_hundred_usec = sns_convert_ns_to_ticks( 1000000ULL );

  code_ram_addr = (void *)((uint32_t)ssc_sdc_vaddr + SDC_CODE_RAM_OFFSET);
  data_ram_addr = (void *)((uint32_t)ssc_sdc_vaddr + SDC_DATA_RAM_OFFSET);

  //reset SDC
  SNS_PRINTF(LOW, sns_fw_printf, "Resetting SDC...");
  HWIO_OUTF(SSC_SCC_SDC_DAP_CBCR, CLK_ENABLE, 0x0);

  HWIO_OUTF(SSC_SCC_SDC_BUS_HCLK_CBCR, HW_CTL, 0x0);
  HWIO_OUTF(SSC_SCC_SDC_BUS_HCLK_CBCR, CLK_ENABLE, 0x1);

  HWIO_OUTF(SSC_SCC_SDC_PROC_FCLK_CBCR, CLK_ENABLE, 0x0);
  HWIO_OUTF(SSC_SCC_SDC_PROC_HCLK_CBCR, HW_CTL, 0x0);
  HWIO_OUTF(SSC_SCC_SDC_PROC_HCLK_CBCR, CLK_ENABLE, 0x0);
  HWIO_OUTF(SSC_SCC_SDC_PROC_FCLK_CBCR, CLK_ARES, 0x1);
  HWIO_OUTF(SSC_SCC_SDC_PROC_HCLK_CBCR, CLK_ARES, 0x1);

  sns_busy_wait(one_hundred_usec);

  HWIO_OUT(SSC_SCC_SDC_BCR, 0x0);
  
  HWIO_OUTF(SSC_SCC_SDC_PROC_FCLK_CBCR, CLK_ARES, 0x0);
  HWIO_OUTF(SSC_SCC_SDC_PROC_HCLK_CBCR, CLK_ARES, 0x0);

  HWIO_OUTF(SSC_SCC_SDC_DAP_CBCR, CLK_ENABLE, 0x1);
  SNS_PRINTF(LOW, sns_fw_printf, "SDC reset complete. Loading SDC binaries...");

  rc |= load_sdc_tcm_binary(SDC_CODE_BIN_NAME, code_ram_addr, SDC_CODE_RAM_SIZE);
  rc |= load_sdc_tcm_binary(SDC_DATA_BIN_NAME, data_ram_addr, SDC_DATA_RAM_SIZE);
  if(rc != SNS_RC_SUCCESS)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Failed to load SDC binaries");
    return rc;
  }
  SNS_PRINTF(HIGH, sns_fw_printf, "SDC load complete. Starting SDC");

  //Start SDC
  HWIO_OUTF(SSC_SCC_SDC_PROC_FCLK_CBCR, CLK_ENABLE, 0x1);
  HWIO_OUTF(SSC_SCC_SDC_PROC_HCLK_CBCR, CLK_ENABLE, 0x1);
      
  sns_tcm_clk_config(false);

  return SNS_RC_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
sns_rc sns_sdc_init( void )
{
  //Map SDC codeRAM and dataRAM region
  ssc_sdc_vaddr = sns_sdc_map_vaddr();
  SNS_ASSERT(NULL != ssc_sdc_vaddr);

  sns_sdc_get_vaddr = &sns_conv_sdc_to_vaddr;
  sns_sdc_tcm_clk_config = &sns_tcm_clk_config;

  MSG_1(MSG_SSID_SNS, MSG_LEGACY_HIGH, "SSC SDC VA=%x", ssc_sdc_vaddr);

  sns_sdc_wdog_enable();

  sns_err_handler_enable();
  
  return SNS_RC_SUCCESS;
}

#else

sns_rc sns_load_sdc_bins( void )
{
  return SNS_RC_FAILED;
}

sns_rc sns_sdc_init( void )
{
  return SNS_RC_FAILED;
}

#endif // SSC_TARGET_SDC_AVAILABLE
