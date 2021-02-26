#ifndef __DDR_PARAMS_H__
#define __DDR_PARAMS_H__

/*=============================================================================

                                DDR PARAMETERS
                                Header File
GENERAL DESCRIPTION
This file defines the DDR parameters header structure and API. It allows user to pass
their own DDR parameters to DDR driver.


Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/12/14   sr      Initial version.
==============================================================================*/
/*==========================================================================

                               INCLUDE FILES
===========================================================================*/
#include "HAL_SNS_DDR.h"
#include "comdef.h"
#include "dev_cdt_params.h"
#include "dev_extended_cdt_params.h"

/*==============================================================================
                                  TYPES
==============================================================================*/


extern DDR_STRUCT *ddrsns_share_data; // TODO: This should be declared in scl file and used here ..also as of now it is RPM code ram start address , but need to finalize the address correctly .


#ifndef DDR_FREQ_MAX_NUM_LEVELS
#define DDR_FREQ_MAX_NUM_LEVELS  7
#endif

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
ddr_device_params *ddr_get_params
(
  DDR_CHANNEL interface_name
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
  DDR_CHANNEL interface_name
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
boolean ddr_params_detection(DDR_CHANNEL interface_name);

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
union ddr_trained_params *ddr_get_trained_params(DDR_CHANNEL interface_name);

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

/* =============================================================================
**  Function : ddr_get_training_checksum
** =============================================================================
*/
/**
*   @brief
*   Get DDR training checksum.
*
*   @param
*   None
*
*   @return
*   DDR training data checksum, 0 if no valid training partition is found
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
uint32 ddr_get_training_checksum(uint32 serial_number);

/* =============================================================================
**  Function : ddr_set_training_checksum
** =============================================================================
*/
/**
*   @brief
*   Compute the checksum over entire training data
*
*   @param
*   Serial number
*
*   @return
*   CRC checksum
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
boolean ddr_set_training_checksum(uint32 serial_number);
#endif /* __DDR_PARAMS_H__ */
