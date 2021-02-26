#ifndef SDCCBSP_H
#define SDCCBSP_H

/** @file SdccBsp.h
   
  This file external data structures, definitions for SDCC BSP 
  used exclusively by the SDCC driver. 

  Copyright (c) 2014 - 2017, Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential
    
**/

/*=============================================================================
                              EDIT HISTORY


when         who    what, where, why
----------   ---    --------------------------------------------------------------
2017-08-09   jt     Add 201.5 MHz clock option for Napali
2017-06-12   sb     Get platform specific speed config 
2017-01-17   jt     Add invalid clk entry 
2016-12-12   jt     Remove legacy code 
2015-04-02   bn     Added APIs to query HS200/400 supported frequecies. Clean up
2015-01-26   rm     Integraged HS200/HS400 support for eMMC from UEFI 3.0
2014-12-03   bn     Removed unused sdcc_bsp_config_clock(). Added sdcc_bsp_get_slot_count()
2014-11-07   bn     Added DDR50_DISABLED slot option 
2014-10-18   jb     Add target specific capability override
2014-06-18   bn     Initial version. Branched from 8994 UEFI

=============================================================================*/

/*****************************************************************************/
/*                    INCLUDES                                               */
/*****************************************************************************/

typedef enum  {
   SDCC_BSP_CLK_INVALID    = 0, 
   SDCC_BSP_CLK_144_KHZ    = 144000,
   SDCC_BSP_CLK_400_KHZ    = 400000,
   SDCC_BSP_CLK_20_MHZ     = 20000000,
   SDCC_BSP_CLK_24_MHZ     = 24000000, 
   SDCC_BSP_CLK_25_MHZ     = 25000000,
   SDCC_BSP_CLK_50_MHZ     = 50000000,
   SDCC_BSP_CLK_100_MHZ    = 100000000,
   SDCC_BSP_CLK_177_MHZ    = 177000000,
   SDCC_BSP_CLK_192_MHZ    = 192000000,
   SDCC_BSP_CLK_200_MHZ    = 200000000,
   SDCC_BSP_CLK_201_MHZ    = 201500000,
   SDCC_BSP_CLK_384_MHZ    = 384000000
   /* While adding an enumeration, the sorted order must be maintained.*/
} sdcc_bsp_clk_type;


/*****************************************************************************/
/*              DEFINITIONS AND CONSTANTS                                    */
/*****************************************************************************/

/* Slot type definitions for different slots on existing on the board in use */
#define SDCC_BSP_NO_SLOT             (0)       /* No physical slot exists. Like a DNI slot */
#define SDCC_BSP_EMBEDDED_MEM_SLOT   (1 << 0)  /* Embedded Memory Slot (eMMC, buried SD) */
#define SDCC_BSP_SD_SLOT             (1 << 1)  /* Open Slot where any kind of card can be inserted */
#define SDCC_BSP_8BIT_SLOT           (1 << 2)  /* Slot is 8 bit in width */
#define SDCC_BSP_DDR_SUPPORT_SLOT    (1 << 3)  /* Slot supports DDR */
#define SDCC_BSP_DDR50_DISABLED_SLOT (1 << 4)  /* Slot does not support DDR50 mode */

typedef UINT32 sdcc_bsp_slot_type;

/* bit-wise definitions for slot type external or internal */
#define SDCC_BSP_SLOT_INVALID        (0)       /* Invalid slot configuration */
#define SDCC_BSP_SLOT_INTERNAL       (1 << 0)  /* Internal Memory Slot */
#define SDCC_BSP_SLOT_EXTERNAL       (1 << 1)  /* External Memory Slot */

typedef UINT32 sdcc_bsp_slot_access_type;

/* GPIO control options for sdcc */
typedef enum
{
   SDCC_BSP_GPIO_OFF,
   SDCC_BSP_GPIO_ON, 
}sdcc_bsp_gpio_ctl_type;

/* Clock control operation */
#define SDCC_BSP_CLK_CONFIG_HCLK   (1 << 0)
#define SDCC_BSP_CLK_CONFIG_RESET  (1 << 1)
#define SDCC_BSP_CLK_CONFIG_MCLK   (1 << 2)

/* SDCC BSP Error Types */
typedef enum
{
   SDCC_BSP_NO_ERROR,
   SDCC_BSP_ERR_UNKNOWN,
   SDCC_BSP_ERR_INVALID_PARAM,
   SDCC_BSP_ERR_FEATURE_UNSUPPORTED
} sdcc_bsp_err_type;

/* Cache operations for sdcc */
typedef enum
{
   SDCC_BSP_CACHE_INVAL,
   SDCC_BSP_CACHE_FLUSH,
   SDCC_BSP_MEM_BARRIER
}sdcc_bsp_cache_op_type;

/** Structure used for initializing HWIO base address */
typedef struct sdcc_base_addr
{
  UINT32 sdc1_base_hc;       
  UINT32 sdc2_base_hc;
  UINT32 sdc3_base_hc;    
  UINT32 sdc4_base_hc;
} sdcc_base_addr_type;

#define SDCC_BSP_SLOT_0    0
#define SDCC_BSP_SLOT_1    1
#define SDCC_BSP_SLOT_2    2
#define SDCC_BSP_SLOT_3    3

/*****************************************************************************/
/*                           PROTOTYPES                                      */
/*****************************************************************************/
/******************************************************************************
* FUNCTION      sdcc_bsp_target_cap_override
*
* DESCRIPTION   This function provides target specific cap overrides
*
* PARAMETERS    [IN}: slot number
*
* RETURN VALUE  None
*
******************************************************************************/
VOID sdcc_bsp_target_cap_overrides (UINT32 slot_num);

/*=============================================================================
 * FUNCTION      sdcc_bsp_get_base_addr
 *
 * DESCRIPTION   This function queries the SDCC register base addresses
 *
 * PARAMETERS    [OUT]: sdcc_base_addr_type pointer to retrieve the SDCC base addresses
 *
 * RETURN VALUE  sdcc_bsp_err_type
 *
 *===========================================================================*/
sdcc_bsp_err_type
sdcc_bsp_get_base_addr (sdcc_base_addr_type *sdcc_base_addr);

/*=============================================================================
 * FUNCTION      sdcc_bsp_get_slot_type
 *
 * DESCRIPTION   This function determines and returns the slot type that exists
 *               on the board which is use.
 *
 * PARAMETERS    [IN]: slot number
 *
 * RETURN VALUE  sdcc_bsp_slot_type
 *
 *===========================================================================*/
sdcc_bsp_slot_type
sdcc_bsp_get_slot_type (UINT32 slot_num);

/*=============================================================================
 * DESCRIPTION   This function determines and returns the slot type whether it 
 *               is external or internal memory that exists on the board.
 *
 * PARAMETERS    [IN]: slot number
 *
 * RETURN VALUE  sdcc_bsp_slot_access_type
 *===========================================================================*/
sdcc_bsp_slot_access_type
sdcc_bsp_get_slot_access_type (UINT32 slot_num);

/*=============================================================================
 * FUNCTION      sdcc_bsp_gpio_control
 *
 * DESCRIPTION   This function enables/disables the GPIO interface to the 
 *               card slot.
 *
 * PARAMETERS    [IN]: slot number
 *               [IN]: state of the GPIO interface for the slot
 *
 * RETURN VALUE  sdcc_bsp_err_type.
 *
 *===========================================================================*/
sdcc_bsp_err_type
sdcc_bsp_gpio_control (UINT32 slot_num, sdcc_bsp_gpio_ctl_type state);

/******************************************************************************
* FUNCTION      sdcc_bsp_get_max_supported_mclk_freq
*
* DESCRIPTION   Get the maximum mclock frequency supported
*
* PARAMETERS    [IN]: drive number
*
* RETURN VALUE  Clock frequency supported
*
******************************************************************************/
sdcc_bsp_clk_type sdcc_bsp_get_max_supported_clk (UINT32 driveno);

/******************************************************************************
* FUNCTION      sdcc_bsp_get_hs200_supported_freq
*
* DESCRIPTION   Get the supported frequency for HS200 mode
*
* PARAMETERS    [IN]: drive number
*
* RETURN VALUE  Clock frequency supported
*
******************************************************************************/
sdcc_bsp_clk_type sdcc_bsp_get_hs200_supported_freq (UINT32 driveno);

/******************************************************************************
* FUNCTION      sdcc_bsp_get_hs400_supported_freq
*
* DESCRIPTION   Get the supported frequency for HS400 mode
*
* PARAMETERS    [IN]: drive number
*
* RETURN VALUE  Clock frequency supported
*
******************************************************************************/
sdcc_bsp_clk_type sdcc_bsp_get_hs400_supported_freq (UINT32 driveno);

/******************************************************************************
* Name: sdcc_enable_mclk
*
* Description:
*    This function is to enable the SDCC controller mclock.
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type sdcc_enable_mclk (UINT32 driveno);

/******************************************************************************
* Name: sdcc_disable_mclk
*
* Description:
*    This function is to disable the SDCC controller mclock.
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type sdcc_disable_mclk (UINT32 driveno);

/******************************************************************************
* Name: sdcc_set_mclk_frequency
*
* Description:
*    This function is to select the SDCC mclock frequency.
*
* Arguments:
*    driveno          [IN] : drive number
*    sdcc_mclk        [IN] : clock speed
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type sdcc_set_mclk_frequency (UINT32 driveno, sdcc_bsp_clk_type sdcc_mclk);

/******************************************************************************
* Name: sdcc_reset_mclk
*
* Description:
*    This function resets the mclk
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
     sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type sdcc_reset_mclk (UINT32 driveno);

/******************************************************************************
* FUNCTION      sdcc_bsp_cache_operation
*
* DESCRIPTION   Performs cache operations on the provided memory region.
*               Used for DMA related operations only.
*    
* PARAMETERS    [IN]: cache operation type
*               [IN]: start address of the memory region
*               [IN]: length of the memory region
*
* RETURN VALUE  None
*
******************************************************************************/
void sdcc_bsp_cache_operation
(
   sdcc_bsp_cache_op_type op, void *addr, UINT32 length
);

/******************************************************************************
* FUNCTION      sdcc_bsp_memory_barrier
*
* DESCRIPTION   Performs data memory barrier operation.
*    
* PARAMETERS    None
*
* RETURN VALUE  None
*
******************************************************************************/
void sdcc_bsp_memory_barrier(void);

/******************************************************************************
* Name: sdcc_ahb_clk_init
*
* Description:
*    This function is to initialize the SDCC AHB clock.
*
* Arguments:
*    driveno    [IN]: drive number
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type sdcc_ahb_clk_init (UINT32 driveno);

/******************************************************************************
* Name: sdcc_ahb_clk_disable
*
* Description:
*    This function is to disable the SDCC AHB clock.
*
* Arguments:
*    driveno    [IN]: drive number
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type sdcc_ahb_clk_disable (UINT32 driveno);

/******************************************************************************
* Name: sdcc_bsp_set_sdhc_base_clk
*
* Description:
*   This function overwrites SDHC Capabilities Register SD Base CLK frequency
*
* Arguments:
*    driveno    [IN]: drive number
*    value       [IN]: SD base clock frequency
*
* Returns:
*    NONE
*
******************************************************************************/
void sdcc_bsp_set_sdhc_base_clk (UINT32 driveno, UINT32 value);

/******************************************************************************
* FUNCTION      sdcc_bsp_get_slot_count
*
* DESCRIPTION   Get the maximum SDCC number of slots support
*
* PARAMETERS    [IN]: None
*
* RETURN VALUE  Maximum number of slots on the target
*
******************************************************************************/
UINT32 sdcc_bsp_get_slot_count (VOID);

/******************************************************************************
* FUNCTION      sdcc_bsp_get_platform_speed_config
*
* DESCRIPTION   Get the speed config based on the platform
*
* PARAMETERS    [IN]: speed_mode
*
* RETURN VALUE  Speed config based on the platform
*
******************************************************************************/
UINT8 sdcc_bsp_get_platform_speed_config (UINT8 speed_mode);


#endif /* #ifndef SDCCBSP_H */

