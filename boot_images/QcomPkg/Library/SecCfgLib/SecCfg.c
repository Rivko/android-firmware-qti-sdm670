/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          SECURITY CONFIGURATION INFORMATION 

GENERAL DESCRIPTION
  This module defines the security configuration information required for boot 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "secboot.h"
#include "boot_hash.h"
#include "boot_util.h"
#include <string.h>
#include "icbcfg.h"
#include "SecDbg.h"

#define  QSEE_SHARED_IMEM_TZ_AREA_BASE   (SCL_IMEM_BASE + 0x734)
#define  QSEE_SHARED_SBL_ZERO_PERIPH_0   (QSEE_SHARED_IMEM_TZ_AREA_BASE + 0x50)
#define  QSEE_SUBSYS_INFO_ADDRESS        (SCL_QSEE_CODE_BASE + 0x0014C000)
#define  QSEE_PAGE_4K                    4096
#define  QSEE_DDR_MEM_BASE               DDR_MEM_BASE
#define  QSEE_DDR_MEM_END                (DDR_MEM_BASE + DDR_MEM_SIZE)
#define  QSEE_SBL1_REGION_BASE           SCL_SBL1_DDR_DATA_BASE
#define  QSEE_SBL1_REGION_END            (SCL_SBL1_DDR_DATA_BASE + SCL_SBL1_DDR_DATA_SIZE)
#define  QHEE_CODE_BASE                  SCL_QHEE_CODE_BASE
#define  QHEE_CODE_SIZE                  SCL_QHEE_TOTAL_SIZE
#define  QSEE_CODE_BASE                  SCL_QSEE_CODE_BASE
#define  QSEE_CODE_SIZE                  SCL_QSEE_TOTAL_SIZE
#define  QSEE_SIZE_BASE_TO_QHEE          (SCL_QHEE_CODE_BASE - DDR_MEM_BASE)
#define  QSEE_SIZE_SBL_TO_END            (QSEE_DDR_MEM_END - QSEE_SBL1_REGION_END)

#define  QSEE_ROUNDDOWN(type, val, multiple) ((val) & \
               ~(type)((multiple)-1))

typedef enum
{
  QSEE_PIL_AUTH_MODEM_PROC,       /**< @deprecated This enum member
                                        is deprecated. Use
                                        QSEE_PIL_AUTH_MODEM_FW_PROC. */
  QSEE_PIL_AUTH_QDSP6_PROC,       /**< LPASS Hexagon@tm processor. */
  QSEE_PIL_AUTH_SPS_PROC,         /**< @deprecated SPS processor. */
  QSEE_PIL_AUTH_EXEC,             /**< QSEE generic executive image. */
  QSEE_PIL_AUTH_MODEM_SW_PROC,    /**< Modem software processor. */
  QSEE_PIL_AUTH_MODEM_FW_PROC,    /**< Modem firmware processor. */
  QSEE_PIL_AUTH_WLAN_PROC,        /**< Riva processor. */
  QSEE_PIL_AUTH_SEC_APP,          /**< QSEE software secure applications. */
  QSEE_PIL_AUTH_GNSS_PROC,        /**< GNSS processor. */
  QSEE_PIL_AUTH_VIDEO_PROC,       /**< Video processor */
  QSEE_PIL_AUTH_VPU_PROC,         /**< VPU processor */
  QSEE_PIL_AUTH_BCSS_PROC,        /**< BCSS processor */
  QSEE_PIL_AUTH_UNSUPPORTED_PROC  /**< Processor not supported. */
} qsee_pil_proc_e_type;


typedef struct subsys_region_s
{
  uint32 start;
  uint32 size;
}subsys_region_t;

typedef struct qsee_subsys_info_s
{
  subsys_region_t subsys[QSEE_PIL_AUTH_UNSUPPORTED_PROC];
  uint8 hash[PBL_SHA256_HASH_LEN];
}qsee_subsys_info_t;


/* DDR controller supports maximum of 2 ranks. */
#define QSEE_NUM_RANKS             2

#define TZBSP_FLOOR_256MB(addrxx)   ((addrxx) & ~(TZBSP_256MB-1))

/* Shortcut macros to access DDR map fields using rank indexing. The mapping:
 *   Rank   DDR      Rank
 *   index  channel  region
 *     0       0       0
 *     1       0       1
 */
#define DDR_REGION_BASE(mapxx, idxx) \
    (mapxx).channels[(idxx) >> 1].regions[(idxx) & 1].base_addr

#define DDR_REGION_SIZE(mapxx, idxx) \
    (mapxx).channels[(idxx) >> 1].regions[(idxx) & 1].size

#define DDR_REGION_INTERLEAVED(mapxx, idxx) \
    (mapxx).channels[(idxx) >> 1].regions[(idxx) & 1].interleaved

#define DDR_CHANNEL(idxx) ((idxx) >> 1)

/**
 * DDR layout is stored here. This needs to be separate from the MMU DDR
 * regions as those can be coalesced and won't contain interleaving
 * information.
 *
 * Note that the queried DDR regions appear in the order they are defined
 * in the BIMC segments/slaves, which doesn't guarantee ascending order.
 * The DDR regions per each channel are packed to the beginning of the array.
 */
static icb_mem_map_type g_ddr_map;

extern  void boot_is_auth_enabled(boot_boolean *is_auth_enabled);
extern  boolean boot_dload_is_dload_mode_set( void );

/*===========================================================================
**  Function :  qsee_is_memory_dump_allowed
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will determine ram dump allowed or not
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   None
* 
* @note   This function is owned by SecureMSM TrustZone team; please contact for
*         questions and modifications.
*/
boot_boolean qsee_is_memory_dump_allowed( secboot_verified_info_type *secboot_info) 
{
  boot_boolean allowed = FALSE;
  boot_boolean is_auth_enabled;

  boot_is_auth_enabled(&is_auth_enabled);

  if((FALSE == is_auth_enabled) || (SECBOOT_DEBUG_ENABLE == secboot_info->enable_debug) || (sec_dbg_is_flag_set(SEC_DBG_ENABLE_OFFLINE_CRASH_DUMPS)))
  {
    /*if secboot is not enabled we always allow tz dump */	
    allowed = TRUE;
  }
  
  return allowed;
}

/*===========================================================================
**  Function :  qsee_is_retail_unlocked
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will determine retail crash dump allowed or not
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   None
* 
* @note   This function is owned by SecureMSM TrustZone team; please contact for
*         questions and modifications.
*/
boot_boolean qsee_is_retail_unlocked (secboot_verified_info_type *secboot_info) 
{

  return (((secboot_info->enable_crash_dump & SECBOOT_CRASH_DUMP_ENABLE) == SECBOOT_CRASH_DUMP_ENABLE) && 
                  !(SECBOOT_DEBUG_ENABLE == secboot_info->enable_debug));

}

/*===========================================================================
**  Function :  query_ddr_map
** ==========================================================================
 *
 * Queries the DDR memory map from BIMC bus segment/slave configuration (BRIC
 * registers). 
 *
 * @return \c ICBCFG_SUCCESS if query was successfully completed, negative error
 *         code otherwise.
 */
static int query_ddr_map(void)
{
  icbcfg_error_type icb_err;

  /* Query the DDR configuration from ICB driver. */
  icb_err = ICB_Get_Memmap("/dev/icbcfg/boot", &g_ddr_map);

  if(ICBCFG_SUCCESS != icb_err)
  {
    //TZBSP_LOG_ERR("ICB_Get_Memmap failed: %u", icb_err);
    return ICBCFG_ERROR;
  }

  return ICBCFG_SUCCESS;
}

/*===========================================================================
**  Function :  qsee_zero_peripheral_memory
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will clean PIL image area 
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   None
* 
* @note   This function is owned by SecureMSM TrustZone team; please contact for
*         questions and modifications.
*/
void qsee_zero_peripheral_memory(void) 
{
  uint8 *imem_sha_digest = (uint8 *) QSEE_SHARED_SBL_ZERO_PERIPH_0;
  uint8 sha_digest[PBL_SHA256_HASH_LEN];
  qsee_subsys_info_t * g_subsys_info = (qsee_subsys_info_t *) QSEE_SUBSYS_INFO_ADDRESS;
  int ret, i;
  uintnt base;
  uint32 size;

#if 0
  //TODO: needs add amt clean
  if(E_SUCCESS != tz_mpu_amt_clear())
  {
    return FALSE;
  }
#endif

  /* Query the full DDR memory map. */
  if(query_ddr_map())
  {
    return;
  }

  ret = boot_hash(SBL_HASH_SHA256,
                   (uint8 *)g_subsys_info->subsys,
                   (sizeof(subsys_region_t) * QSEE_PIL_AUTH_UNSUPPORTED_PROC),
                   sha_digest,
                   sizeof(sha_digest));

  if ( (ret == 0) &&
       (!bByteCompare(sha_digest, g_subsys_info->hash, sizeof(sha_digest))) &&
       (!bByteCompare(sha_digest, imem_sha_digest, sizeof(sha_digest))))
  {
    for (i=0; i<QSEE_PIL_AUTH_UNSUPPORTED_PROC; i++) 
    {
      if (g_subsys_info->subsys[i].size) 
      {
        /* TODO: make sure we are not clearing SBL1 regions */
       // fast_memzi((uint32 *)QSEE_ROUNDDOWN(uint32, g_subsys_info->subsys[i].start, QSEE_PAGE_4K), g_subsys_info->subsys[i].size);
       memset((void *)QSEE_ROUNDDOWN(uint32, g_subsys_info->subsys[i].start, QSEE_PAGE_4K), 0x0, g_subsys_info->subsys[i].size);
      }
    }
  } else {
      /* Overall assumption is that maximum of 4 regions are present. DDR
       * controller supports four ranks, hence this should be safe assumption.
       * Truncation to 32 bit is safe as sizes and bases are all < 4GB. */
    for(i=0; i < QSEE_NUM_RANKS; i++)
    {
      base = (uintnt) DDR_REGION_BASE(g_ddr_map, i);
      size  = (uint32) DDR_REGION_SIZE(g_ddr_map, i);
      if(size)
      {
        if(base < QSEE_SBL1_REGION_BASE && (base + size) > QSEE_SBL1_REGION_END)
        {
          /* SBL1 region falls in this rank, preserve it. */ 
          memset((void *)base, 0x0, (QSEE_SBL1_REGION_BASE - base));
          memset((void *)QSEE_SBL1_REGION_END, 0x0, ((base + size) - (uint32)QSEE_SBL1_REGION_END));
        }
        else if(base < QSEE_SBL1_REGION_BASE && (base + size) < QSEE_SBL1_REGION_END)
        {
         /* SBL1 region start falls in this rank but continues onto next rank, clear DDR in this rank until SBL1 start. */ 
         memset((void *)base, 0x0, (QSEE_SBL1_REGION_BASE - base));
        }
        else if(base < QSEE_SBL1_REGION_END)
        {
          /* SBL1 region start falls in this rank but continues onto next rank, clear all DDR in this rank after SBL1. */ 
          memset((void *)QSEE_SBL1_REGION_END, 0x0, ((base + size) - (uint32)QSEE_SBL1_REGION_END));
        }
        else
        {
          /* Clear all region in this rank */
          memset((void *)base, 0x0, size);
        }
      }
    }
  }


    /* SBL area will be preserved */
    //fast_memzi((uint32 *)QSEE_DDR_MEM_BASE, QSEE_SIZE_BASE_TO_QHEE);
    //memset((void *)QSEE_DDR_MEM_BASE, 0x0,  QSEE_SIZE_BASE_TO_QHEE);
    //fast_memzi((uint32 *)QSEE_SBL1_REGION_END, QSEE_SIZE_SBL_TO_END);
    //memset((void *)QSEE_SBL1_REGION_END, 0x0, QSEE_SIZE_SBL_TO_END);

    /* Clear QHEE memory region 
    memset((void *)QHEE_CODE_BASE,
           0x0,
           QHEE_CODE_SIZE); */

    /* Clear QSEE memory region 
    memset((void *)QSEE_CODE_BASE,
           0x0,
           QSEE_CODE_SIZE); */
}
