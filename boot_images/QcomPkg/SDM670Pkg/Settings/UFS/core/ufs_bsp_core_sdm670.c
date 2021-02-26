/**********************************************************************
* ufs_bsp_uefi_sdm670.c
*
* Board support file for SDM670
*
* Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
* 
*
**********************************************************************/

/*=======================================================================
                        Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Settings/UFS/core/ufs_bsp_core_sdm670.c#1 $
$DateTime: 2017/10/22 23:08:13 $

YYYY-MM-DD   who     what, where, why
---------------------------------------------------------------------- 
2017-07-31   jt      Add log function 
2017-06-02   jt      Add stub function for non-HSG4 PHY init  
2017-04-08   jt      Turn on flags to skip actions in core  
2017-01-25   jt      Add UFS device reset 
2016-11-01   jt      Add reset function 
2016-10-21   jt      Enable HS for RUMI
2016-09-14   jt      AANG RUMI PHY init updates  
2016-08-22   jt      PHY init updates from 8998v2 
2016-07-22   jt      Adapted for Napali 
2015-06-15   rh      Disable the PHY from decoding the LCC sequence
2014-09-17   rh      Adapted for 8996 
2013-10-30   rh      Clock regime function cleanup
2013-10-30   rh      More accurate delay added
2013-06-19   rh      Initial creation
===========================================================================*/

#include <Uefi.h>
#include <HALhwio.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/TimerLib.h>
#include <Protocol/EFIHWIO.h>
#include <Protocol/EFIClock.h>
#include <Protocol/LoadedImage.h> 
#include <Protocol/EFIPlatformInfo.h> 

#include <Library/PcdLib.h>
#include <Library/ArmLib.h>

#include "DDIPlatformInfo.h"
#include "../inc/ufs_osal.h"
#include "../inc/ufs_bsp.h"
#include "../inc/ufs_phy_hwio_v2.h"
#include "../inc/aang_phy_hwio.h"
#include "HALbootHWIO.h"

#define UFS_PHY_PCS_READY_TIMEOUT      0xffffff


EFI_CLOCK_PROTOCOL *hUFS_Clock = NULL;
DalPlatformInfoPlatformType g_platform;


void ufs_bsp_clk_set (uint8_t hostid, uint32_t mode)
{
   (void) hostid;
}   

void ufs_bsp_cache_op (void *addr, uint32_t len, uint32_t op) 
{
   if (op & FLUSH) {
      WriteBackInvalidateDataCacheRange (addr, len);
   }

   if (op & INVAL) {
      InvalidateDataCacheRange (addr, len);
   }

   if (op & CLEAN) {
      WriteBackDataCacheRange (addr, len);
   }
}

void ufs_bsp_busy_wait(uint32_t us)
{
   gBS->Stall(us);
}

void ufs_bsp_memory_barrier ()
{
   ArmDataMemoryBarrier();
}

// Constant table for UFS-PHY initialization
static struct ufs_mphy_init_item ufs_bsp_mphy_init_table[] = {
     { HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_ADDR,                      0x04},       // Adjust for series A
     {0,0}};

static struct ufs_mphy_init_item ufs_bsp_mphy_init_table_rumi[] = {
     { HWIO_UFS_PHY_PHY_START_ADDR                      ,  0x00}, 
     { HWIO_UFS_PHY_POWER_DOWN_CONTROL_ADDR             ,  0x00}, 
     { HWIO_UFS_PHY_RX_SYM_RESYNC_CTRL_ADDR             ,  0x03}, 
     { HWIO_UFS_PHY_TIMER_20US_CORECLK_STEPS_MSB_ADDR   ,  0x0f}, 
     { HWIO_UFS_PHY_TIMER_20US_CORECLK_STEPS_LSB_ADDR   ,  0x00}, 
     { HWIO_QSERDES_COM_BIAS_EN_CLKBUFLR_EN_ADDR        ,  0x3f}, 
     { HWIO_QSERDES_COM_SYSCLK_EN_SEL_ADDR              ,  0x03}, 
     { HWIO_QSERDES_COM_SYS_CLK_CTRL_ADDR               ,  0x16}, 
     { HWIO_QSERDES_COM_RES_CODE_TXBAND_ADDR            ,  0xC0}, 
     { HWIO_QSERDES_COM_PLL_VCOTAIL_EN_ADDR             ,  0x03}, 
     { HWIO_QSERDES_COM_PLL_CNTRL_ADDR                  ,  0x24}, 
     { HWIO_QSERDES_COM_PLL_CLKEPDIV_ADDR               ,  0x03}, 
     { HWIO_QSERDES_COM_RESETSM_CNTRL_ADDR              ,  0x10}, 
     { HWIO_QSERDES_COM_PLL_RXTXEPCLK_EN_ADDR           ,  0x13}, 
     { HWIO_QSERDES_COM_PLL_CRCTRL_ADDR                 ,  0x43}, 
     { HWIO_QSERDES_COM_DEC_START1_ADDR                 ,  0x98}, 
     { HWIO_QSERDES_COM_DEC_START2_ADDR                 ,  0x03}, 
     { HWIO_QSERDES_COM_DIV_FRAC_START1_ADDR            ,  0x80}, 
     { HWIO_QSERDES_COM_DIV_FRAC_START2_ADDR            ,  0x80}, 
     { HWIO_QSERDES_COM_DIV_FRAC_START3_ADDR            ,  0x10}, 
     { HWIO_QSERDES_COM_PLLLOCK_CMP1_ADDR               ,  0x65}, 
     { HWIO_QSERDES_COM_PLLLOCK_CMP2_ADDR               ,  0x1e}, 
     { HWIO_QSERDES_COM_PLLLOCK_CMP3_ADDR               ,  0x00}, 
     { HWIO_QSERDES_COM_PLLLOCK_CMP_EN_ADDR             ,  0x03}, 
     { HWIO_QSERDES_COM_PLL_IP_SETI_ADDR                ,  0x07}, 
     { HWIO_QSERDES_COM_PLL_CP_SETI_ADDR                ,  0x0f}, 
     { HWIO_QSERDES_COM_PLL_IP_SETP_ADDR                ,  0x07}, 
     { HWIO_QSERDES_COM_PLL_CP_SETP_ADDR                ,  0x01}, 
     { HWIO_QSERDES_RX0_PWM_CNTRL1_ADDR                 ,  0x08}, 
     { HWIO_QSERDES_RX0_PWM_CNTRL2_ADDR                 ,  0x40}, 
     { HWIO_QSERDES_RX0_PWM_NDIV_ADDR                   ,  0x30}, 
     { HWIO_QSERDES_RX0_CDR_CONTROL_ADDR                ,  0x40}, 
     { HWIO_QSERDES_RX0_CDR_CONTROL_HALF_ADDR           ,  0x0c}, 
     { HWIO_QSERDES_RX0_CDR_CONTROL_QUARTER_ADDR        ,  0x12}, 
     { HWIO_QSERDES_RX0_SIGDET_CNTRL_ADDR               ,  0xc0}, 
     { HWIO_QSERDES_RX0_SIGDET_CNTRL2_ADDR              ,  0x07}, 
     { HWIO_QSERDES_RX0_RX_BAND_ADDR                    ,  0x06}, 
     { HWIO_QSERDES_RX0_UFS_CNTRL_ADDR                  ,  0x00}, 
     { HWIO_QSERDES_RX0_RX_IQ_RXDET_EN_ADDR             ,  0xf3}, 
     { HWIO_UFS_PHY_POWER_DOWN_CONTROL_ADDR             ,  0x01}, 
     {0,0}};

static struct ufs_mphy_init_item ufs_bsp_mphy_rate_b_init_table[] = {
     { HWIO_UFS_MEM_QSERDES_COM_VCO_TUNE_MAP_ADDR,                      0x44},
     {0,0}};

// 2 lane initialization 
static struct ufs_mphy_init_item ufs_bsp_mphy_2_lane_init_table[] = {
     {0,0}};

static struct ufs_mphy_init_item ufs_bsp_mphy_2_lane_init_table_rumi[] = {
     {0,0}};

uint32_t ufs_bsp_get_mphy_init_item_table (struct ufs_mphy_init_item **itm)
{
   if (g_platform == DALPLATFORMINFO_TYPE_RUMI) {
      //return 0;
      *itm = ufs_bsp_mphy_init_table_rumi;
      return sizeof(ufs_bsp_mphy_init_table_rumi)/sizeof(struct ufs_mphy_init_item);
   }
   else {
      *itm = ufs_bsp_mphy_init_table;
      return sizeof(ufs_bsp_mphy_init_table)/sizeof(struct ufs_mphy_init_item);
   }
}

uint32_t ufs_bsp_get_mphy_init_rate_b_item_table (struct ufs_mphy_init_item **itm)
{
   if (g_platform == DALPLATFORMINFO_TYPE_RUMI) {
      return 0;
   }
   else {
      *itm = ufs_bsp_mphy_rate_b_init_table;
      return sizeof(ufs_bsp_mphy_rate_b_init_table)/sizeof(struct ufs_mphy_init_item);
   }
}

uint32_t ufs_bsp_get_mphy_init_2_lane_item_table (struct ufs_mphy_init_item **itm)
{
   if (g_platform == DALPLATFORMINFO_TYPE_RUMI) {
      //return 0;
      *itm = ufs_bsp_mphy_2_lane_init_table_rumi;
      return sizeof(ufs_bsp_mphy_2_lane_init_table_rumi)/sizeof(struct ufs_mphy_init_item);
   }
   else {
      *itm = ufs_bsp_mphy_2_lane_init_table;
      return sizeof(ufs_bsp_mphy_2_lane_init_table)/sizeof(struct ufs_mphy_init_item);
   }
}

uint32_t ufs_bsp_get_mphy_init_non_hsg4_item_table (struct ufs_mphy_init_item **itm)
{
   (void) itm; 
   return 0; 
}

// Start the PHY and wait for the PHY to be ready 
uint32_t ufs_bsp_mphy_start (uint8_t hostid)
{
   (void) hostid; 
   int32_t tout = UFS_PHY_PCS_READY_TIMEOUT;

   HWIO_OUTM (UFS_MEM_UFS_PHY_PHY_START, 1, 1);
   ufs_bsp_busy_wait (10);
   
   while (!(HWIO_IN(UFS_MEM_UFS_PHY_PCS_READY_STATUS) & 0x01)) {
      if (tout-- == 0) {
         return FAIL;
      }
      ufs_bsp_busy_wait (1);
   }

   if (g_platform == DALPLATFORMINFO_TYPE_RUMI) {
      while (in_dword(HWIO_QSERDES_COM_RESET_SM_ADDR) != 0x3d) {
         if (tout-- == 0) {
            return FAIL;
         }
         ufs_bsp_busy_wait (1);
      }
   }

   return SUCCESS;
}

void ufs_bsp_reset (uint8_t hostid)
{
   (void) hostid;  

   if (g_platform == DALPLATFORMINFO_TYPE_RUMI) {

      // Reset and sleep 50 ms
      out_dword(HWIO_GE_DUT_RESET_REG_ADDR, 0x5f); 
      ufs_bsp_busy_wait (50000);

      // Release reset and sleep 50 ms
      out_dword(HWIO_GE_DUT_RESET_REG_ADDR, 0x0);
      ufs_bsp_busy_wait (50000);

      // Clock frequencies used
      out_dword(HWIO_GE_DUT_CLK_STATUS_SEL_REG_ADDR, 0x1);

      // Reset ICE fuses
      out_dword(HWIO_GE_DUT_UFS_ICE_CTRL_REG_ADDR, 0x0);
   }
}

void ufs_bsp_post_link_init (uint8_t hostid)
{
   (void) hostid; 

   if (g_platform == DALPLATFORMINFO_TYPE_RUMI) {
      return;
   }

   out_dword(HWIO_UFS_MEM_UFS_PHY_LINECFG_DISABLE_ADDR, 0x02);   
}


void ufs_bsp_device_reset (uint8_t hostid)
{
   (void) hostid;   
}

void *ufs_bsp_allocate_norm_buf (size_t size)
{
    VOID *mem;
    EFI_STATUS            status;
    status=gBS->AllocatePool(EfiBootServicesData,size,(VOID**)&mem);

    if (EFI_SUCCESS != status)
    {
        DEBUG((EFI_D_ERROR, "UFS: Allocate memory of size 0x%x     [FAILED]\n",size));
        return NULL;
    }
    return mem;
}

void  ufs_bsp_free_norm_buf (void *pmem)
{
    if ( pmem == NULL )
        return;

    gBS->FreePool(pmem);
}

void *ufs_bsp_allocate_xfer_buf (size_t size)
{
    return UncachedAllocatePool(size);
}

void  ufs_bsp_free_xfer_buf (void *pmem)
{
    if ( pmem == NULL )
        return;

    UncachedFreePool(pmem);
}

void ufs_bsp_get_platform_config (struct ufs_bsp_config_item *itm)
{
   EFI_STATUS                          eResult = EFI_SUCCESS;
   EFI_PLATFORMINFO_PROTOCOL           *hPlatformInfoProtocol;
   EFI_PLATFORMINFO_PLATFORM_INFO_TYPE PlatformInfo;
   uint32_t soc_emulation_type = 0;
   
   eResult = gBS->LocateProtocol(&gEfiPlatformInfoProtocolGuid, NULL,
     (VOID **) &hPlatformInfoProtocol);
   if (eResult != EFI_SUCCESS)
   {
     DEBUG((EFI_D_WARN, "Error: Failed to locate PlatformInfo protocol.\n"));
     goto ConfigEnd;
   }
   
   eResult = hPlatformInfoProtocol->GetPlatformInfo(hPlatformInfoProtocol,
     &PlatformInfo);
   if (eResult != EFI_SUCCESS)
   {
     DEBUG((EFI_D_WARN, "Error: GetPlatformInfo failed.\n"));
     goto ConfigEnd;
   }
   
   if (PlatformInfo.platform >= EFI_PLATFORMINFO_NUM_TYPES)
   {
     DEBUG((EFI_D_WARN, "Error: Unknown platform type (%d)\n", PlatformInfo.platform));
     eResult = EFI_PROTOCOL_ERROR;
     goto ConfigEnd; 
   }

   if (PlatformInfo.platform == EFI_PLATFORMINFO_TYPE_RUMI) {
   	g_platform = DALPLATFORMINFO_TYPE_RUMI;
   }

ConfigEnd:
   if (eResult != EFI_SUCCESS) {
      soc_emulation_type = in_dword(0x01fc8004);

      /* 0 = Silicon, 1 = RUMI, 2 = Veloce, 3 = Virtio */
      if (soc_emulation_type == 1) 
      {
         g_platform = DALPLATFORMINFO_TYPE_RUMI; 
      }
   }

   if (g_platform == DALPLATFORMINFO_TYPE_RUMI) {
      itm->skip_gear_sw = 0;
      itm->skip_link_start = 0;
      itm->slow_xfer = 1;
      itm->core_skip = 0; 
   }
   else {
      /* VIRTIO or Silicon */
      itm->slow_xfer = 0;
      itm->skip_gear_sw = 1;
      itm->skip_link_start = 1;
      itm->core_skip = 1; 
   }
}

uintptr_t ufs_bsp_get_reg_baseaddress (uint8_t hostid)
{
   (void) hostid;
   return (uintptr_t) 0x1d80000;
}

boolean ufs_bsp_get_dal_config(ufs_dal_config_item *itm)
{
   if (g_platform == DALPLATFORMINFO_TYPE_RUMI) {
      itm->enable_hs = 1; 
      itm->gear = 1;
      itm->num_lanes = 1; 
   }

   return TRUE; 
}

void ufs_bsp_log_error(char *str)
{
   DEBUG ((EFI_D_ERROR, "%a\n", str)); 
}
