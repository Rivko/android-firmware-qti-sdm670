#ifndef SECBOOT_HW_H
#define SECBOOT_HW_H
/*****************************************************************************
*
* @file  secboot_hw.h (Secboot Hardware API)
*
* @brief API to read Security Control Fuses containing authentication
*        information
*       
* Copyright (c) 2010-2014 by Qualcomm, Technologies, Inc.  All Rights Reserved.
*
*****************************************************************************/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/14   hw      Add QPSI code review fix
05/28/14   hw      Add RoT support
05/27/10   sm      Created module

============================================================================*/
/** @ingroup SecureMSM
 *  @{
 */

/** @defgroup SecbootHardware Secboot Hardware Library */

/**
 * @addtogroup SecbootHardware
 *
 * @{
 */
#include "comdef.h"

/**
 Identifies the error type returned by the API.
 */
typedef enum
{
  E_SECBOOT_HW_SUCCESS                = 0x5A9D3AA0, /**< Operation was successful. */
  E_SECBOOT_HW_FAILURE                = 0x1, /**< General failure. */
  E_SECBOOT_HW_INVALID_PARAM          = 0x2, /**< Parameter passed in was invalid */
  E_SECBOOT_HW_OUT_OF_RANGE           = 0x3, /**< Index out of bounds */
  E_SECBOOT_HW_FEC_ERROR              = 0x4, /**< FEC error status detected */
  E_SECBOOT_HW_MAX                    = 0x7FFFFFFF /**< Force to 32 bits */
} secboot_hw_etype;

/**
 Identifies the secure boot fuses which represent the
 authentication information for the code.
 */
#define SECBOOT_HW_APPS_CODE_SEGMENT   1 /**< Code segment in SECURE_BOOTn register */
                                         /**< representing authentication information. */
                                         /**< for the application processors images */     
#define SECBOOT_HW_MBA_CODE_SEGMENT    2 /**< Code segment in SECURE_BOOTn register */
                                         /**< representing authentication information. */
                                         /**< for the Modem Boot Authentication (MBA) image */     
#define SECBOOT_HW_MSS_CODE_SEGMENT    3 /**< Code segment in SECURE_BOOTn register */
                                         /**< representing authentication information. */
                                         /**< for the modem image */     

/* Length of a SHA256 hash */
#define SHA256_HASH_LEN 32

/* define the MACRO for DISABLE_ROT_TRANSFER fuse value, which comes from the hardware efuse. */
#define SECBOOT_HW_ROT_TRANSFER_DISABLE 0x1 /**< Bit value 1 - ROT transfer is turned off */                                     
#define SECBOOT_HW_ROT_TRANSFER_ENABLE  0x0 /**< Bit value 0 - ROT transfer is turned on */

#define SECBOOT_CODE_SEG_MIN 1   /* Corresponds to first SECBOOTn supported */
#define SECBOOT_CODE_SEG_MAX 28  /* Corresponds to last SECBOOTn supported */
/**
 * @brief Secboot Function table
 */
typedef struct secboot_hw_ftbl_type
{
  secboot_hw_etype (*secboot_hw_is_auth_enabled)
                    (uint32    code_segment,
                     uint32*   auth_enabled_ptr);

  secboot_hw_etype (*secboot_hw_get_root_of_trust)
                    (uint32 code_segment,
                     uint8 *root_of_trust);

  secboot_hw_etype (*secboot_hw_get_msm_hw_id)
                    (uint32        code_segment,
                     const uint8   *root_of_trust,
                     uint64*       msm_hw_id_ptr);

  secboot_hw_etype (*secboot_hw_get_use_serial_num)
                    (uint32         code_segment,
                     uint32*        auth_use_serial_num_ptr);

  secboot_hw_etype (*secboot_hw_get_serial_num)
                    (uint32*  serial_num_ptr);

  secboot_hw_etype (*secboot_hw_get_mrc_fuse_info)   /** Pointer to secboot_hw_get_mrc_fuse_info */
                    (uint32   code_segment,
                     uint32* is_root_cert_enabled,
                     uint32* root_cert_total_num,
                     uint32* revocation_list,
                     uint32* activation_list);
}secboot_hw_ftbl_type;

/**
 * @brief This function checks if the image associated with the code segment
 *        needs to be authenticated. If authentication is required, callers
 *        MUST authenticate the image successfully before allowing it to execute.
 *
 * @param[in]     code_segment       Code segment in SECURE_BOOTn register
 *                                   containing authentication information
 *                                   of the image.
 * @param[in,out] auth_enabled_ptr   Pointer to a uint32 indicating whether
 *                                   authentication is required. Will be
 *                                   populated to 0 if authentication
 *                                   is not required, 1 if authentication
 *                                   is required.
 *         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must NOT allow execution to continue if a failure is returned.
 *
 * @dependencies None
 *
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_is_auth_enabled
(
  uint32    code_segment,
  uint32*   auth_enabled_ptr
);

/**
 * @brief This function returns the hash of the trusted root certificate for
 *        the image. The image's certificate chain must chain back to this
 *        trusted root certificate. When RoT transfter is turned off, it returns
 *        the default root of trust 0, but when RoT tranfer is turned on, it
 *        returns the root of trust 1.
 *
 *
 * @param[in]      code_segment       Code segment in SECURE_BOOTn register
 *                                    containing authentication information
 *                                    of the image.
 * @param[in,out]  root_of_trust      32 byte buffer which will be
 *                                    populated with the SHA256 hash of
 *                                    the trusted root certificate.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must not allow execution to continue if a failure is returned. 
 *
 * @dependencies None
 * 
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_root_of_trust
(
  uint32 code_segment,
  uint8 *root_of_trust
);

/**
 * @brief This function returns the msm_hw_id used to authenticate the image's
 *        signature. The 64 bit msm_hw_id is comprised of the 32 bit JTAG ID
 *        (with the tapeout version in the upper 4 bits masked out) + the 32 bit
 *        OEM_ID or SERIAL_NUM value
 *
 * @param[in]      code_segment      Code segment in SECURE_BOOTn register
 *                                   containing authentication information
 *                                   of the image.
 *
 * @param[in]      root_of_trust     32 bytes buffer containing the root of
 *                                   trust hash which was populate by
*                                    calling secboot_hw_get_root_of_trust()
 *
 * @param[in,out] msm_hw_id_ptr      Pointer to a uint64 which will
 *                                   be populated with the msm hardware id.
 *                                   The uint64 is allocated by the caller.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must not allow execution to continue if a failure is returned  
 * 
 * @dependencies secboot_hw_get_root_of_trust() must have been called
 * 
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_msm_hw_id
(
  uint32        code_segment,
  const uint8   *root_of_trust,
  uint64*       msm_hw_id_ptr
);

/**
 * @brief This function returns the family number and device number of soc hw version 
 *
 * @param[in,out] soc_hw_ver_family_device_num     Pointer to a uint32 which will
 *                                                 be populated with the family number and 
 *                                                 device number of soc hw version.
 *                                                 The uint32 is allocated by the caller.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must not allow execution to continue if a failure is returned  
 * 
 * @dependencies None
 * 
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_soc_hw_version
(
  uint32* soc_hw_ver_family_device_num
);

/**
 * @brief This function returns if the serial number is to be used for authentication
 *
 * @param[in]      code_segment             Code segment in SECURE_BOOTn register
 *                                          containing authentication information
 *                                          of the image.
 * @param[in,out]  auth_use_serial_num_ptr  Pointer to a uint32 which will be
 *                                          populated with the USE_SERIAL_NUM
 *                                          fuse value. The uint32 is allocated by
 *                                          the caller.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must not allow execution to continue if a failure is returned 
 * 
 * @dependencies secboot_hw_get_root_of_trust() must have been called
 * 
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_use_serial_num
(
  uint32         code_segment,
  uint32*        auth_use_serial_num_ptr
);

/**
 * @brief This function returns the serial number of the chip
 *                         
 * @param[in,out]  serial_num_ptr       Pointer to a uint32 which will
 *                                      be populated with the SERIAL_NUM
 *                                      fuse value. The uint32 is allocated by
 *                                      the caller.
 *
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must not allow execution to continue if a failure is returned 
 * 
 * @dependencies None
 * 
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_serial_num
(
  uint32*  serial_num_ptr
);

/**
 * @brief This function return pointers to the secboot functions linked into
 *        the image
 *
 * @param[in,out] ftbl_ptr              Pointer to the function table structure
 *                                      to populate. The pointer must be allocated
 *                                      by the caller.
 *
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 *
 */
secboot_hw_etype secboot_hw_get_ftbl(secboot_hw_ftbl_type* ftbl_ptr);

/**
 * @brief This function returns rot fuse info from the chip
 *                         
 * @param[in]      code_segment         Code segment in SECURE_BOOTn register
 *                                      containing authentication information
 *                                      of the image.
 * @param[in,out]  disable_rot_transfer       Pointer to a uint32 which will 
 *                                      be populated by fuse and indicates if
 *                                      ROT is disabled, The uint32 is 
 *                                      allocated by the caller.
 * @param[in,out]  current_sw_rot       Pointer to a uint32 which will
 *                                      be populated by fuse and indicates
 *                                      if current sw is using rot root of trust
 *                                      The uint32 is allocated by the caller.
 * @param[in,out]  sw_rot_use_serial_num       Pointer to a uint32 which will
 *                                      be populated by fuse and indicates
 *                                      if ROT_EN OU field is bound by device's
 *                                      serial num. The uint32 is allocated by
 *                                      the caller.
 *
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. 
 *         Caller's must not allow execution to continue if a failure is returned 
 * 
 * @dependencies None
 * 
 * @sideeffects  None
 *
 * @see Security Control HDD and SWI for SECURE_BOOT fuses
 *
 */
secboot_hw_etype secboot_hw_get_rot_fuse_info
(
  uint32   code_segment,
  uint32*  disable_rot_transfer,
  uint32*  current_sw_rot,
  uint32*  sw_rot_use_serial_num
);

/**
 * @brief This function checks to see if the root of trust hash
 *        is blowin in the OEM_PK_HASH fuses or is in the
 *        root_of_trust_pk_hash_table.
 *
 * @param code_segment       [in] Segment of SECURE_BOOTn register holding
 *                                authentication information for the code
 *
 * @param is_hash_in_fuse_ptr [in,out] Pointer to uint32 that will be
 *                                populated with the fuse value. 0 means
 *                                hash is in the Qualcomm root of trust
 *                                table.
 *                         
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @see Security Control HDD for SECURE_BOOT fuses
 * 
 */
secboot_hw_etype secboot_hw_get_is_hash_in_fuse
(
  uint32 code_segment,
  uint32* is_hash_in_fuse_ptr
);

/// @}
//
/// @}
#endif //SECBOOT_HW_H
