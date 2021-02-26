#ifndef __DDR_PARAMS_H__
#define __DDR_PARAMS_H__

/*=============================================================================

                                DDR PARAMETERS
                                Header File
GENERAL DESCRIPTION
This file defines the DDR parameters header structure and API. It allows user to pass
their own DDR parameters to DDR driver.


Copyright 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/04/13   sr      New timing parameters added.
08/22/13   sr      Initial version.
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
  uint32 manufacture_name;
  uint32 device_type;

  uint32 num_rows_cs0;
  uint32 num_cols_cs0;
  uint32 num_banks_cs0;
  uint32 interface_width_cs0;
  uint32 num_rows_cs1;
  uint32 num_cols_cs1;
  uint32 num_banks_cs1;
  uint32 interface_width_cs1;

  uint32 reserved_0;
  uint32 reserved_1;
  uint32 reserved_2;
  uint32 reserved_3;
};

struct ddr_device_params_lpddr
{
  /* Common parameters: must be same with ddr_device_params_common */
  uint32 manufacture_name;
  uint32 device_type;

  uint32 num_rows_cs0;
  uint32 num_cols_cs0;
  uint32 num_banks_cs0;
  uint32 interface_width_cs0;
  uint32 num_rows_cs1;
  uint32 num_cols_cs1;
  uint32 num_banks_cs1;
  uint32 interface_width_cs1;

  /* reserved common fields */
  uint32 reserved_0;
  uint32 reserved_1;
  uint32 reserved_2;
  uint32 reserved_3;

  /* lpddr specific parameters */
  uint32 tRFC;
  uint32 tRAS_Min;
  uint32 tRAS_Max;
  uint32 tREFI;
  uint32 tXSR;
  uint32 tXP;
  uint32 tWTR;
  uint32 tRP_AB;
  uint32 tRP_PB;
  uint32 tRRD;
  uint32 tWR;
  uint32 tCKE;
  uint32 tRCD;
  uint32 tFAW;
  uint32 tRTP;
  uint32 tMRR;
  uint32 tZQCL;
  uint32 tZQCS;
  uint32 tMRW;
  uint32 tCKESR;
  uint32 tODTonmax;
  uint32 tODTonmin;
  uint32 tODToffmax;
  uint32 tODToffmin;
  uint32 tDQSCK_min;
  uint32 tDQSCK_max;
  uint32 tDQSQ_max;
  uint32 burst_length;
  uint32 drive_strength;

  /* reserved lpddr specific fields */
  uint32 reserved_4;
  uint32 reserved_5;
  uint32 reserved_6;
  uint32 reserved_7;
  uint32 reserved_8;
  uint32 reserved_9;
  uint32 reserved_10;
  uint32 reserved_11;

};

struct ddr_device_params_pcddr3
{
  /* Common parameters: must be same with ddr_device_params_common */
  uint32 manufacture_name;
  uint32 device_type;

  uint32 num_rows_cs0;
  uint32 num_cols_cs0;
  uint32 num_banks_cs0;
  uint32 interface_width_cs0;
  uint32 num_rows_cs1;
  uint32 num_cols_cs1;
  uint32 num_banks_cs1;
  uint32 interface_width_cs1;

  /* reserved common fields */
  uint32 reserved_0;
  uint32 reserved_1;
  uint32 reserved_2;
  uint32 reserved_3;

  /* PCDDR3 specific parameters */
  uint32 tRFC;
  uint32 tRAS_Min;
  uint32 tRAS_Max;
  uint32 tREFI;
  uint32 tXP;
  uint32 tWTR;
  uint32 tRP_AB;
  uint32 tRRD;
  uint32 tWR;
  uint32 tCKE;
  uint32 tRCD;

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
  uint32 tAONmax;
  uint32 tAONmin;
  uint32 tMRD;
  uint32 tXS;
  uint32 tDQSCK_min;
  uint32 tDQSCK_max;
  uint32 tDQSQ_max;
  uint32 drive_strength;

  /* reserved pcddr3 specific fields */
  uint32 reserved_4;
  uint32 reserved_5;
  uint32 reserved_6;
  uint32 reserved_7;
  uint32 reserved_8;
  uint32 reserved_9;
  uint32 reserved_10;

};

union ddr_device_params
{
  struct ddr_device_params_common common;
  struct ddr_device_params_lpddr  lpddr;
  struct ddr_device_params_pcddr3 pcddr3;
};



#ifndef DDR_FREQ_MAX_NUM_LEVELS
#define DDR_FREQ_MAX_NUM_LEVELS  7
#endif



struct ddr_trained_params_lpddr3
{
    uint8 nlevels;
    uint32 freq[DDR_FREQ_MAX_NUM_LEVELS];

    struct
    {
      uint16 ca_training_full_cdc[2][2][4];                          /* coarse\fine [channel][site][delay]*/
      uint32 ca_training_perbit[2][2][2];             /* perbit deskew [channel][site][deskew]*/

      uint16 dq_read_training[2][4][2][4];               /* dq read coarse\fine [channel][dq][byte][delay]*/
      uint32 dq_read_perbit_training[2][4][2][2];        /* perbit deskew [channel][dq][byte][deskew odd\even]*/
      uint32 dq_read_perbit_dm_training[2][4];           /* perbit DM register [channel][dq] */

      uint16 dq_write_training[2][4][2][4];              /* dq write coarse\fine [channel][dq][byte][delay] */
      uint32 dq_write_perbit_training[2][4][2];          /* perbit deskew [channel][dq][byte] */
      uint32 dq_write_perbit_dm_training[2][4];          /* perbit DM register [channel][dq] */
    } phy_cdc_params;


   uint32 ca_dcc_training[2][2][2][2];                      /* DCC training     [channel][jcpll_sel][site][wr90/wrlvl]    */
   uint32 dq_dcc_training[2][4][2][2][2];                   /* DCC training     [channel][dq][jcpll_sel][byte][wr90/wrlvl] */
};




struct ddr_trained_params_pcddr3
{
    uint8 nlevels;
    uint32 freq[DDR_FREQ_MAX_NUM_LEVELS];

    struct
    {
      uint16 dq_read_training[2][4][2][4];               /* dq read coarse\fine [channel][dq][byte][delay]*/
      uint32 dq_read_perbit_training[2][4][2][2];        /* perbit deskew [channel][dq][byte][deskew odd\even]*/
      uint32 dq_read_perbit_dm_training[2][4];           /* perbit DM register [channel][dq] */

      uint16 dq_write_training[2][4][2][4];              /* dq write coarse\fine [channel][dq][byte][delay] */
      uint32 dq_write_perbit_training[2][4][2];          /* perbit deskew [channel][dq][byte] */
      uint32 dq_write_perbit_dm_training[2][4];          /* perbit DM register [channel][dq] */
    } phy_cdc_params;

    struct  
    {
        uint32 start_dly_byte0_3[2][DDR_FREQ_MAX_NUM_LEVELS];
        uint32 start_dly_byte4_7[2][DDR_FREQ_MAX_NUM_LEVELS];
    } bimc_rcw_params;

    uint16 dq_ren_training[2][DDR_FREQ_MAX_NUM_LEVELS][8];        /* REN_training [channel][dq][cfg_num][byte_delay]*/

    uint16 dq_wrlvl_retimer_training[2][4][ DDR_FREQ_MAX_NUM_LEVELS][2];  /* WRLVL_retimer_training[channel][dq][cfg_num][byte]*/
    uint16 dq_wrlvl_delay_training[2][4][ DDR_FREQ_MAX_NUM_LEVELS][2];     /* WRLVL_delay_training[channel][dq][cfg_num][byte]*/

    uint32 ca_dcc_training[2][2][2][2];                      /* DCC training     [channel][jcpll_sel][site][wr90/wrlvl]    */
    uint32 dq_dcc_training[2][4][2][2][2];                   /* DCC training     [channel][dq][jcpll_sel][byte][wr90/wrlvl] */

};


/** Meta DDR trained parameters */
union ddr_trained_params
{
  struct ddr_trained_params_lpddr3 lpddr3_training;
  struct ddr_trained_params_pcddr3 pcddr3_training;
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
struct ddr_device_params_header_v2
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

  /*OBT On\Off threshold*/
  uint32 OBT_Threshold;

  /*ODT On\Off threshold*/
  uint32 ODT_Threshold;

  /*Extra Features*/
  uint32 Optional_Features;

  /*Reserved Field*/
  uint32 Reserved_0;

  /*Reserved Field*/
  uint32 Reserved_1;

  /*Reserved Field*/
  uint32 Reserved_2;

  /**More fields can be added here, each field must be multiple of 4 bytes**/

};


/**
* Size of the DDR parameters header.
*/
#define DDR_PARAMS_HEADER_V2_SIZE sizeof(struct ddr_device_params_header_v2)

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
union ddr_device_params *ddr_get_params_size
(
  SDRAM_INTERFACE interface_name
);



/* ============================================================================
**  Function : ddr_get_header
** ============================================================================
*/
/**
*   @brief
*   Get DDR device parameters header. This contains useful information such as
*   interleave mode, optional features, etc that applies to all ddr devices
*
*   @param[in]  None
*
*   @return
*   DDR device header
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
struct ddr_device_params_header_v2 *ddr_get_header(void);

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
