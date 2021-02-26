#ifndef SDCCCOMMON_H
#define SDCCCOMMON_H

/** @file SdccCommon.h
   
  This file external data structures, definitions for SDCC common code 
  shared between SdccDxe and SdccCommonLib. 

  Copyright (c) 2014 - 2015, Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
    
**/

/*=============================================================================
                              EDIT HISTORY


when         who    what, where, why
----------   ---    --------------------------------------------------------------
2015-10-13   rm     Added BlockIO2 defines
2015-01-26   rm     Integraged HS200/HS400 support for eMMC from UEFI 3.0
2014-09-24   bn     Code clean up. Use SdccHal.h from SdccHalLib
2014-06-18   bn     Initial version. 

=============================================================================*/

#include "api/storage/sdcc_api.h"
#include <Library/SdccCommonLib/SdccHalLib/SdccHal.h>
#include <api/systemdrivers/busywait.h>

#define sdcc_udelay(us) busywait(us)
#define sdcc_mdelay(ms) busywait(1000*ms)

/* maximum number of partition access that can be set */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_MAX_PARTITION_ACCESS   8

/* External SD card User partition */
#define SDCC_SD_PHYSICAL_PARTITION_USER  0
/* The following definitions are the physical partitions that can be */
/* accessed. */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_PHY_PARTITION_USER_AREA    0
#define SDCC_EMMC_PHY_PARTITION_BOOT_PART_1  1
#define SDCC_EMMC_PHY_PARTITION_BOOT_PART_2  2
#define SDCC_EMMC_PHY_PARTITION_RPMB         3
#define SDCC_EMMC_PHY_PARTITION_GPP_1        4
#define SDCC_EMMC_PHY_PARTITION_GPP_2        5
#define SDCC_EMMC_PHY_PARTITION_GPP_3        6
#define SDCC_EMMC_PHY_PARTITION_GPP_4        7

#define  RPMB_PROVISION_PKT         0
#define  RPMB_DATA_PKT              1


/* SDCC frequency types */
typedef enum
{
   SDCC_SD_DEFAULT_FREQUENCY = 0, /* Frequency <= 25MHz for SD */
   SDCC_SD_HIGH_FREQUENCY,        /* Frequency > 25MHz and <= 50MHz for SD */
   SDCC_MMC_DEFAULT_FREQUENCY,    /* Frequency <= 20MHz for MMC */
   SDCC_MMC_FREQUENCY_26MHZ,      /* MMC 4.X cards that support upto 26MHz */
   SDCC_MMC_FREQUENCY_52MHZ,      /* MMC 4.x cards that support upto 52MHz */
   SDCC_MMC_FREQUENCY_200MHZ,     /* MMC 4.5 cards that support up to 200MHz */
   SDCC_MMC_FREQUENCY_400MHZ      /* MMC 5.0 cards that support up to 400MHz */
} sdcc_card_freq_type;


/* On-card states */
typedef enum sdcc_card_state_t
{
   SDCC_CARD_IDLE = 0,
   SDCC_CARD_READY,
   SDCC_CARD_IDENT,
   SDCC_CARD_STBY,
   SDCC_CARD_TRAN,
   SDCC_CARD_DATA,
   SDCC_CARD_RCV,
   SDCC_CARD_PRG,
   SDCC_CARD_DIS,
   SDCC_CARD_IGNORE         /*dummy entry*/
} SDCC_CARD_STATUS;

/* SDCC mode types */
typedef enum sdcc_card_mode_t
{
   SDCC_IDENTIFICATION_MODE = 0,
   SDCC_DATA_TRANSFER_MODE,
   SDCC_READ_TRANSFER_MODE,
   SDCC_WRITE_TRANSFER_MODE,
   SDCC_HS_TRANSFER_MODE,
   SDCC_INIT_MODE,
   SDCC_DATA_TRANSFER_DDR_MODE,
   SDCC_DATA_TRANSFER_HS200_MODE,
   SDCC_DATA_TRANSFER_HS400_MODE
} SDCC_CARD_MODE;

/* SDHCi UHS mode types */
typedef enum sdhci_uhs_mode_t
{
   SDHCI_SDR12_MODE = 0,
   SDHCI_SDR25_MODE,
   SDHCI_SDR50_MODE,
   SDHCI_SDR104_MODE,
   SDHCI_DDR50_MODE
} SDHCI_UHS_MODE;

/*SDCC_EXTRACT_COMMON_END*/

/* On-card states */
typedef enum sdcc_host_state_t
{
   SDCC_HOST_IDLE = 0,
   SDCC_HOST_IDENT,
   SDCC_HOST_TRAN
} SDCC_HOST_STATE;

/* SDCC response types */
typedef enum sdcc_card_resp_t
{
   SDCC_RESP_NONE   = 0x00,
   SDCC_RESP_SHORT  = 0x01,
   SDCC_RESP_LONG   = 0x04
} SDCC_CARD_RESP;

typedef struct {
   /* memory device information such as card type, card size (in sectors) */
   /* block length, speed class and card identification information */
   sdcc_mem_info_type  mem_info;
   UINT32              block_mode;
   /* For eMMC only: indicates the device type -- removable card or */
   /* embedded BGA */
   UINT8               mmc_device_type;
   UINT8               spec_vers;   /* For MMC only: MMC System Spec version */
   BOOLEAN             mmcv45_support;     /* MMC v4.5 support */
   BOOLEAN             sanitize_support;   /* Indicates device support */
                                           /* for sanitize operation */
   BOOLEAN             write_prot_support; /* indicates if write protection is */
                                           /* supported on the embedded device */
   sdcc_card_freq_type clk_freq;    /* freq card supported */
   UINT8               speed_class; /* card's speed class */
                                    /* e.g. SD has speed class 0, 2, 4 and 6 */
   BOOLEAN             partition_support; /* indicate if partition is supported */
   UINT32              partition_boot_size;      /* boot partition size in kilobytes */
   UINT32              partition_gp_size[4];     /* GPP  partition size in sectors */
   UINT8               boot_partition_enable_val; /* indicate which partition */
                                                  /* is bootable */
   UINT32              partition_inuse_flag;     /* Bit OR mask to indicate partition use */
   UINT8               partition_conf_done;      /* Partition setting done value in extCSD */
   UINT32              read_timeout_in_ms;     /* read timeout based on CSD */
   UINT32              write_timeout_in_ms;    /* write timeout based on CSD */
   BOOLEAN             perm_write_prot_enable; /* indicates if permanent write */
                                               /* protection is enabled on the embedded device */  
   UINT32              hc_erase_timeout_in_ms; /* High capacity erase timeout based on ext_csd */ 
   UINT32              trim_timeout_in_ms;     /* Trim timeout based on ext_csd */    
} sdcc_mem_type;

/* Name the sdcc_dev structure sdcc_card_info due to similarly named 
 * structure sdcc_device */
typedef struct sdcc_card_info {
   UINT32                 driveno;            /* controller ID    */
   BOOLEAN                sdhci_mode;           /* SDHCi Mode enabled */
   BOOLEAN                sdhci_adma_enable;    /* SDHCi ADMA Mode enabled */
   BOOLEAN                sdhci_ddr_support;    /* SDHCi eMMC's DDR Support */
   UINT32                 active_partition_num; /* current partition region in use */
   SDCC_CARD_TYPE         card_type;          /* card type        */
   SDCC_HOST_STATE        host_state;         /* controller state */
   UINT16                 rca;
   BOOLEAN                enable_dma;
   BOOLEAN                prog_scan;
   SDCC_STATUS            errno;              /* only for debugging */
   UINT32                 status;             /* cntr status register */
   sdcc_mem_type          mem;                /* info pertain to memory device */
   UINT32                 clk_freq_in_khz;    /* current SDCC clock freq in KHz */
   UINT32                 cache_align_sz;     /* cache alignment size */
   struct sdcc_slot_data  *pslot;
   INT8                   selected_phase;        /* Selected phase for HS200 operation */   
   BOOLEAN                tuning_needed;
   BOOLEAN                tuning_in_progress;
   BOOLEAN                tuning_done;
   SDCC_CARD_MODE         speed_mode; 
} sdcc_dev_type;

typedef struct sdcc_slot_data
{
   UINT32                 driveno;          /* SDCC controller ID    */
   BOOLEAN                drive_in_use;     /* current drive in use  */
   sdcc_dev_type          dev;              /* devices attached to the drive */
   BOOLEAN                slot_state_changed; /* Set if card is inserted or removed */
   struct HAL_sdhci_Capabilities capabilities;
} sdcc_slot_type;

/* internal client handle structure to track the device handle, partition, */
/* task control block and SPS data transfer end point */
/* This sdcc_device is a common structure to SD/MMC and SDIO. */
typedef struct sdcc_device
{
   struct sdcc_card_info *hdev;             /* pointer to the device info structure */
   UINT32                phy_partition_num; /* physical partition number */
   void                 *data_xfer_end_pt;  /* SPS end pt for data xfer */
}sdcc_device;

typedef struct{
  BOOLEAN                    SendStopCmd; 
  UINT32                     TransferSize;
  UINT8                     *Buffer; 
  sdcc_dev_type             *Pdevice; 
  OPERATION_TYPE             Type;         
} BLKIO2_XFER_INFO;

/*****************************************************************************/
/*                    INCLUDES                                               */
/*****************************************************************************/


#endif /* #ifndef SDCCCOMMON_H */

