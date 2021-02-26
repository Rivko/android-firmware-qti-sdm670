#ifndef __DDR_PARAMS_H__
#define __DDR_PARAMS_H__

/*=============================================================================

                                DDR PARAMETERS
                                Header File
GENERAL DESCRIPTION
This file defines the DDR parameters header structure and API. It allows user to pass
their own DDR parameters to DDR driver.


Copyright 2011-2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/26/13   sr      Fixed reset issue by making phy_params[] to phy_params
01/10/13   tw      Synced up training param with RPM
12/06/12   tw      Added support for mx scaling, cleaned up SITE training data
09/21/12   sl      Combined LPDDR device parameters.
02/02/12   sl      Common LPDDR1/LPDDR2/PCDDR3 device parameters.
06/02/11   dh      Initial version.
==============================================================================*/
/*==========================================================================

                               INCLUDE FILES
===========================================================================*/
#include "HAL_DDR.h"
#include "comdef.h"


/*==============================================================================
                                  TYPES
==============================================================================*/
struct ddr_device_params_common
{
  uint32 device_name;
  uint32 manufacture_name;
  uint32 device_type;

  uint32 tRFC;
  uint32 tRAS_Min;
  uint32 tRAS_Max;
  uint32 tRC;
  uint32 tREF;
  uint32 tREFI;
  uint32 tXSR;
  uint32 tXP;
  uint32 tWTR;
  uint32 tRP_AB;
  uint32 tRRD;
  uint32 tWR;
  uint32 tCKE;
  uint32 tRCD;
  uint32 tMRD;

  uint32 num_rows_cs0;
  uint32 num_cols_cs0;
  uint32 num_banks_cs0;
  uint32 num_rows_cs1;
  uint32 num_cols_cs1;
  uint32 num_banks_cs1;
  uint32 interface_width;
  uint32 burst_length;
  uint32 cas_latency;
};

struct ddr_device_params_lpddr
{
  /* Common parameters: must be same with ddr_device_params_common */
  uint32 device_name;
  uint32 manufacture_name;
  uint32 device_type;

  uint32 tRFC;
  uint32 tRAS_Min;
  uint32 tRAS_Max;
  uint32 tRC;
  uint32 tREF;
  uint32 tREFI;
  uint32 tXSR;
  uint32 tXP;
  uint32 tWTR;
  uint32 tRP_AB;
  uint32 tRRD;
  uint32 tWR;
  uint32 tCKE;
  uint32 tRCD;
  uint32 tMRD;

  uint32 num_rows_cs0;
  uint32 num_cols_cs0;
  uint32 num_banks_cs0;
  uint32 num_rows_cs1;
  uint32 num_cols_cs1;
  uint32 num_banks_cs1;
  uint32 interface_width;
  uint32 burst_length;
  uint32 cas_latency;

  /* LPDDR specific parameters */
  uint32 tRP_PB;
  uint32 tFAW;
  uint32 tRTP;
  uint32 tRL;
  uint32 tWL;
  uint32 tINIT2;
  uint32 tINIT3;
  uint32 tINIT4;
  uint32 tMRR;
  uint32 tZQCL;
  uint32 tZQCS;
  uint32 tZQINIT;
  uint32 tMRW;
  uint32 tCKESR;
  uint32 tZQRESET;
  uint32 tREFBW;
  uint32 tRmin;
  uint32 tDQSCK_min;
  uint32 tDQSCK_max;
};

struct ddr_device_params_pcddr3
{
  /* Common parameters: must be same with ddr_device_params_common */
  uint32 device_name;
  uint32 manufacture_name;
  uint32 device_type;

  uint32 tRFC;
  uint32 tRAS_Min;
  uint32 tRAS_Max;
  uint32 tRC;
  uint32 tREF;
  uint32 tREFI;
  uint32 tXSR;
  uint32 tXP;
  uint32 tWTR;
  uint32 tRP_AB;
  uint32 tRRD;
  uint32 tWR;
  uint32 tCKE;
  uint32 tRCD;
  uint32 tMRD;

  uint32 num_rows_cs0;
  uint32 num_cols_cs0;
  uint32 num_banks_cs0;
  uint32 num_rows_cs1;
  uint32 num_cols_cs1;
  uint32 num_banks_cs1;
  uint32 interface_width;
  uint32 burst_length;
  uint32 cas_latency;

  /* PCDDR3 specific parameters */
  uint32 tFAW;
  uint32 tRTP;
  uint32 tZQoper;
  uint32 tZQCS;
  uint32 tXSDLL;
  uint32 tCKSRE;
  uint32 tCKSRX;
  uint32 tXPDLL;
  uint32 tAOFPD_Min;
  uint32 tAOFPD_Max;
  uint32 tMOD;
  uint32 reserved_0;
  uint32 reserved_1;
  uint32 reserved_2;
  uint32 reserved_3;
};

union ddr_device_params
{
  struct ddr_device_params_common common;
  struct ddr_device_params_lpddr  lpddr;
  struct ddr_device_params_pcddr3 pcddr3;
};

#ifndef DDR_FREQ_MAX_NUM_LEVELS
#define DDR_FREQ_MAX_NUM_LEVELS  4
#endif
#ifndef DDR_CX_VOLTAGE_MAX_NUM
#define DDR_CX_VOLTAGE_MAX_NUM   4
#endif
#ifndef DDR_MX_VOLTAGE_MAX_NUM
#define DDR_MX_VOLTAGE_MAX_NUM   2
#endif


/** DDR RCW trained parameters */
struct ddr_trained_params_rcw
{

  uint32 nlevels;
  struct
  {
    uint32 freq;
    uint32 window;
    uint32 coarse_dly;
    uint32 fine_dly;
  } params[DDR_FREQ_MAX_NUM_LEVELS];
};

__packed struct ddr_trained_params_lpddr3
{
    #define DDR_JCPLL_MAX_NUM 2
    uint8 nlevels;
    boolean cpr_enabled;
    uint32 freq[DDR_FREQ_MAX_NUM_LEVELS];
    uint8 cx_voltage[DDR_CX_VOLTAGE_MAX_NUM];
    uint8 mx_voltage[DDR_MX_VOLTAGE_MAX_NUM];
    
    __packed struct
    {
        uint8 ca_coarse_dly:4;
        uint8 ca_fine_dly:3;

        uint8 read_coarse_delay[4];
        uint8 read_fine_delay[4];

        uint8 write_coarse_delay[4];
        uint8 write_fine_delay[4];

    } phy_params;

    __packed struct ddr_site_data
    {
      uint8 dll_delay: 6;
      uint8 push: 1;

    } site_params[DDR_FREQ_MAX_NUM_LEVELS][DDR_JCPLL_MAX_NUM][DDR_CX_VOLTAGE_MAX_NUM][DDR_MX_VOLTAGE_MAX_NUM][5];

    uint8  wrlvl_delay[4];
    
    __packed struct
    {
      uint8 dcc0:5;
      uint8 dcc1:5;
    } phy_dcc[5]; /* CA + DQ0-3 */
};

/** Meta DDR trained parameters */
union ddr_trained_params
{
  struct ddr_trained_params_rcw rcw;
  struct ddr_trained_params_lpddr3 lpddr3_training;
};

/** DDR parameter partition */
struct ddr_params_partition
{
  uint32 magic_number;
  uint32 version;
  uint32 checksum;
  union ddr_trained_params trained_params[2];
};

/**
* This is the DDR parameters header which must be appended before the external DDR
* device table if the user wants to use their own DDR device table.
* This structure must be 4 bytes aligned and the user defined DDR device table must 
* follow this structure immediately.
* User should append this header in front of their own DDR device table and pass
* a pointer points to the beginning of this header via the ddr_set_params() API to 
* force DDR driver to utilize the user defined parameters.
* The user defined ddr device table must contain a minimal number of entries to 
* match the system's ddr devices.
* For example if a system has two ddr devices attached then the external table must 
* define at least two entries to match those two devices.
* However, more entries can be added to the end of table to support other ddr devices
* with different parameters.
* 
*/
struct ddr_device_params_header_v1
{
  /*Version number of the CDT DDR parameters, always the first four byte of this header*/
  uint32 version_number;
  
  /*Magic number of the DDR block of CDT, must have the value of DDR_PARAMS_MAGIC_NUM*/
  uint32 magic_number;
  
  /*Check sum of all the DDR parameters, currently not used*/
  uint32 check_sum;
  
  /*Number of DDR interfaces*/
  uint32 num_of_device;
  
  /*Total size of the ddr parameters for a single interface*/
  uint32 size_of_param;

  /*DDR interfaces are interleaved or not*/
  uint32 interleaved;
  
  /**More fields can be added here, each field must be multiple of 4 bytes**/
  
};


/**
* Size of the DDR parameters header.
*/
#define DDR_PARAMS_HEADER_V1_SIZE sizeof(struct ddr_device_params_header_v1)


/**
* Magic number of the DDR parameters header. It's the ASCII of "DDR" as DDR Parameters
*/
#define DDR_PARAMS_MAGIC_NUM   0x44445200


/* ============================================================================
**  Function : ddr_set_params
** ============================================================================
*/
/**
*   @brief
*   This function sets the DDR driver's device table to an external device table.
*   It will also update the interleaving mode information according to the parameter
*   header that's passed in.
*   User should append ddr_device_params_header_v1 header in front of their own DDR device 
*   table and pass  a pointer points to the beginning of this header via the this API to 
*   force DDR driver to utilize the user defined parameters.
*   The user defined ddr device table must contain a minimal number of entries to 
*   match the system's ddr devices.
*   For example if a system has two ddr devices attached then the external table must 
*   define at least two entries to match those two devices.
*   However, more entries can be added to the end of table to support other ddr devices
*   with different parameters.
*
*   @param[in]  ddr_params_ptr   Pointer to the external ddr parameters. It should
*                                point to the user defined ddr device table with ddr
*                                parameters header appended in front. This pointer must
*                                points to a 4 bytes aligned address.
*                         
*
*   @return
*   TRUE if DDR device table is updated, FALSE if an error happens and this operation
*   is aborted
*
*   @dependencies
*   Must be called prior to ddr_initialize_device. 
*   This API will take no effect after ddr_initialize_device
*
*   @sa
*   None
*
*   @sa
*   None
*/
boolean ddr_set_params(void *ddr_params_ptr, uint32 ddr_params_size);

/* ============================================================================
**  Function : ddr_get_params
** ============================================================================
*/
/**
*   @brief
*   Returns DDR parameters.
*
*   @param[in]  interface_name    Interface to get DDR parameters for
*
*   @return
*   DDR parameters
*
*   @dependencies
*   None
*
*   @sa
*   None
*
*   @sa
*   None
*/
union ddr_device_params *ddr_get_params
(
  SDRAM_INTERFACE interface_name
);

/* =============================================================================
**  Function : ddr_params_detection
** =============================================================================
*/
/**
*   @brief
*   Detect correct DDR device parameters based on manufacture and dimension
*   information.
*
*   @param[in]  interface_name  Interface to detect for
*
*   @retval  TRUE   Parameters detected
*   @retval  FALSE  Parameters not dectected
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_params_detection(SDRAM_INTERFACE interface_name);

/* ============================================================================
**  Function : ddr_param_interleaved
** ============================================================================
*/
/**
*   @brief
*   Return the interleave setting based on ddr parameters
*
*   @return
*   return a bitmask of interleave status
*
*   @dependencies
*   Must be called after ddr_set_params. Calling it before ddr_set_params will
*   return the default setting which is TRUE.
*
*   @sa
*   None
*
*   @sa
*   None
*/
uint32 ddr_param_interleaved(void);

/* ============================================================================
**  Function : ddr_params_set_shared_memory
** ============================================================================
*/
/**
*   @brief
*   Copies the detected ddr parameter to shared memory region
*
*   @return
*   None
*
*   @dependencies
*   None
*
*   @sa
*   None
*
*   @sa
*   None
*/
void ddr_params_set_shared_memory( void );

/* =============================================================================
**  Function : ddr_params_get_partition
** =============================================================================
*/
/**
*   @brief
*   Get DDR parameter partition: partition base is returned, and partition size
*   is updated through pointer.
*
*   @param[out]  size  DDR parameter partition size
*
*   @return
*   DDR parameter partition base
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void *ddr_params_get_training_data(uint32 *size);

/* =============================================================================
**  Function : ddr_params_set_partition
** =============================================================================
*/
/**
*   @brief
*   Set DDR parameter partition.
*
*   @param[in]  base  DDR parameter partition base
*   @param[in]  size  DDR parameter partition size
*
*   @retval  TRUE   Partition set successfully
*   @retval  FALSE  Partition set unsuccessfully
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_params_set_training_data(void *base, uint32 size);

/* =============================================================================
**  Function : ddr_params_is_training_required
** =============================================================================
*/
/**
*   @brief
*   Indicate whether DDR parameter training is required or not. Training is
*   required if and only if DDR itself (e.g. PCDDR3) requires parameter training
*   and DDR parameter partition is invalid.
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_params_is_training_required( void );

/* =============================================================================
**  Function : ddr_get_trained_params
** =============================================================================
*/
/**
*   @brief
*   Get DDR trained parameters.
*
*   @param[in]  interface_name  Interface to get for
*
*   @return
*   DDR trained parameters
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
union ddr_trained_params *ddr_get_trained_params(SDRAM_INTERFACE interface_name);

/* =============================================================================
**  Function : ddr_get_training_cookie
** =============================================================================
*/
/**
*   @brief
*   Get DDR training magic cookie.
*
*   @param
*   None
*
*   @return
*   DDR training cookie, 0 if no valid training partition is found
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
uint32 ddr_get_training_cookie(void);

/* =============================================================================
**  Function : ddr_set_training_cookie
** =============================================================================
*/
/**
*   @brief
*   Set DDR training magic cookie.
*
*   @param
*   None
*
*   @return
*   boolean - operation successful or fail
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_set_training_cookie(uint32 cookie);
#endif /* __DDR_PARAMS_H__ */
