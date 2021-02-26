#ifndef QSEE_FUSE_H
#define QSEE_FUSE_H

/**
@file qsee_fuse.h
@brief Provide HW & SW Fuse functionality
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_fuse.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
10/03/11    rv      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * Enumeration for the SW fues TZ supports
 */
typedef enum
{
  QSEE_HLOS_IMG_TAMPER_FUSE    = 0, /**< Used during boot to determine
                                        if the HLOS image has successfully
                                        been authenticated. */
  QSEE_WINSECAPP_LOADED_FUSE   = 1, /**< Used by WinSec App to prevent
                                        reloading. */
  QSEE_UEFISECAPP_LOADED_FUSE  = 2, /**< Used by UefiSecApp to prevent
                                        reloading. */
  QSEE_MDTP_RECOVERY_MS_FUSE   = 3, /**< Used by MDTP to prevent 
                                        access to Bootloader APIs. */
  QSEE_MDTP_HLOS_MS_FUSE       = 4, /**< Used by MDTP to prevent 
                                        access to Bootloader & Recovery APIs. */
  QSEE_MSSECAPP_LOADED_FUSE    = 5, /**< Used by MSSecApp App to prevent
                                        reloading. */
  QSEE_OEM_FUSE_1              = 6, /**< 8 reserved fuse bits for OEMs */

  QSEE_OEM_FUSE_2              = 7, 

  QSEE_OEM_FUSE_3              = 8, 

  QSEE_OEM_FUSE_4              = 9, 

  QSEE_OEM_FUSE_5              = 10, 

  QSEE_OEM_FUSE_6              = 11, 

  QSEE_OEM_FUSE_7              = 12, 

  QSEE_QCOM_RESERVED_FUSE_1    = 13,  /**< Reserved fuse bit for QCOM.*/

  QSEE_QCOM_RESERVED_FUSE_2    = 14,  /**< Reserved fuse bit for QCOM.*/

  QSEE_QCOM_RESERVED_FUSE_3    = 15,  /**< Reserved fuse bit for QCOM.*/

  QSEE_QCOM_RESERVED_FUSE_4    = 16,  /**< Reserved fuse bit for QCOM.*/

  QSEE_QCOM_RESERVED_FUSE_5    = 17,  /**< Reserved fuse bit for QCOM.*/

  QSEE_QCOM_RESERVED_FUSE_6    = 18,  /**< Reserved fuse bit for QCOM.*/

  QSEE_QCOM_RESERVED_FUSE_7    = 19,  /**< Reserved fuse bit for QCOM.*/

  QSEE_QCOM_RESERVED_FUSE_8    = 20,  /**< Reserved fuse bit for QCOM.*/

  QSEE_QCOM_RESERVED_FUSE_9    = 21,  /**< Reserved fuse bit for QCOM.*/

  QSEE_HLOS_BL_MILESTONE_FUSE  = 22, /**< Used to notify the bootloader milestone call.*/

  QSEE_HLOS_TAMPER_NOTIFY_FUSE = 23, /**< Used to notify TZ that HLOS has been tampered.*/

  QSEE_NUM_SW_FUSES            = 24  /**< Number of supported > */

} qsee_sw_fuse_t;

/**
 * @brief      Query whether the given SW fuse has been blown
 *
 * @param[in]  fuse_num    the SW fuse to query
 * @param[out] is_blown    whether the given fuse has been blown
 * @param[in]  is_blown_sz size of the return pointer
 *
 * @return     0 on success, negative on failure
 * @warning    function is not thread-safe
 */
int qsee_is_sw_fuse_blown
(
  qsee_sw_fuse_t    fuse_num,
  bool*             is_blown,
  uint32_t          is_blown_sz
);

/**
 * @brief     Change the state of the SW fuse
 * @param[in] fuse_num the SW fuse to query
 * @return    0 on success, negative on failure
 */
int qsee_blow_sw_fuse(qsee_sw_fuse_t fuse_num);



/**
 * @brief     QFPROM write row
 * @param[in] raw_row_address
 * @param[in] row_data
 * @param[in] bus_clk_khz
 * @param[in] qfprom_api_status
 * @return    0 on success, negative on failure
 */
int qsee_fuse_write
(
  uint32_t    raw_row_address,
  uint32_t*   row_data,
  uint32_t    bus_clk_khz,
  uint32_t*   qfprom_api_status
);

/**
 * @brief     QFPROM read row
 * @param[in] row_address
 * @param[in] addr_type
 * @param[in] row_data
 * @param[in] qfprom_api_status
 * @return    0 on success, negative on failure
 */
int qsee_fuse_read
(
  uint32_t   row_address,
  int32_t    addr_type,
  uint32_t*  row_data,
  uint32_t*  qfprom_api_status
);

#endif /* QSEE_FUSE_H */

