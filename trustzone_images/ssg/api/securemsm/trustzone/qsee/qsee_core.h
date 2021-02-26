#ifndef QSEE_CORE_H
#define QSEE_CORE_H

/**
@file qsee_core.h
@brief Provide Core functionality
*/

/*===========================================================================
   Copyright (c) 2014 by QUALCOMM, Technology Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_core.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
01/25/14    hw      add new api qsee_get_secure_state
08/09/11    rv      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  uint32_t value[2];
} qsee_secctrl_secure_status_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/**
 * @brief     Reads the JTAG ID
 * @return    JTAG ID value
 */
int qsee_read_jtag_id(void);

/**
 * @brief     Reads the serial number from PTE chain
 * @return    Serial number
 */
int qsee_read_serial_num(void);

/**
 * Tests whether all of the range [\c start, \c start + \c len] is in
 * non-secure memory. This is a convenience function to access \c
 * tzbsp_is_ns_area. \c NULL is a valid value for \c start, because
 * physical addressing is used.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] len Length of the memory range in bytes.
 *
 * @return \c TRUE if the entire area is in non-secure memory. \c
 * FALSE if the area contains secure memory.
 */
bool qsee_is_ns_range(const void* start, uint32_t len);

/**
 * Tests whether all of the range [\c start, \c end] is tagged 
 * for the particular VMID, specific to CPZ use case 
 *  
 * @param [in] vmid  Virtual machine ID defined in access 
 *        control layer (enum ACVirtualMachineId)
 *  
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] end End of the memory range, physical address,
 * included  in the range.
 *
 * @return \c TRUE if the entire area is tagged for VMID. \c 
 *  FALSE if not
 */
bool qsee_is_s_tag_area
( 
  uint32_t                    vmid,
  uint64_t                    start,
  uint64_t                    end
);

/**
 * Tag all memory in the range [\c start, \c end] with the specified
 * VMID.
 *
 * @param [in] vmid  Virtual machine ID defined in access 
 * control layer (enum ACVirtualMachineId)
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] end End of the memory range, physical address,
 * included  in the range.
 *
 * @return E_SUCCESS on success
 *            error code otherwise:
 */
int qsee_tag_mem(uint32_t vmid, uint64_t start, uint64_t end);

/**
 * Counts the number of 4KB chunks of memory that are in the specified VM.
 *
 * @param [in] vmid Virtual machine ID defined in access 
 *        control layer (enum ACVirtualMachineId)
 *
 * @return \c the number of 4KB chunks in the VM. \c
 */
uint32_t qsee_vm_mem_count(
  uint32_t    vmid
);


/**
* Reads the status of the HDMI link and hardware HDCP
* @param [out] hdmi_enable  HDMI output enabled
* @param [out] hdmi_sense HDMI sense
* @param [out] hdcp_auth HDCP authentication success.
* @return 0 on success
*/
int qsee_hdmi_status_read
(
  uint32_t* hdmi_enable,
  uint32_t* hdmi_sense,
  uint32_t* hdcp_auth
);


/**
 * check security status on the device
 *
 * @param [out] status   the security status (struct qsee_secctrl_secure_status_t)
 *                       - Bit field definition of the status struct:
 *                        Bit 0: secboot enabling check failed
 *                        Bit 1: Sec HW key is not programmed
 *                        Bit 2: debug disable check failed
 *                        Bit 3: Anti-rollback check failed
 *                        Bit 4: fuse config check failed
 *                        Bit 5: rpmb provision check failed
 *
 * @return 0 on call success
 */
int qsee_get_secure_state(qsee_secctrl_secure_status_t* status);


/**
* @brief     return the application ID that is stored in qsee app certificate.
* @return    64 bits long application ID
*/
int qsee_get_tz_app_id(uint8_t * tz_app_id, uint32_t id_buf_len);

/**
* @brief Provides the UUID for the device
* @param [out] uuid_ptr buffer that is filled with a struct based an IETF UUID (GP compatible)
* @param [in/out]  uuid_len size of uuid buffer
*/

int qsee_get_device_uuid(uint8_t *uuid_ptr, size_t *uuid_len);

/**
* @brief The detailed version number of the TEE implementation. 
* @param [out]    version_ptr buffer that is filled with the QSEE version number 
* @param [in/out] version_len size of version buffer
*/
int qsee_get_trusted_os_component_version(uint8_t *version_ptr, size_t *version_len);

/*
* @brief The detailed version number of the firmware which supports the Trusted OS 
*        implementation. This includes all privileged software involved in the
*        secure booting and support of the TEE apart from the secure OS and Trusted
*        Applications.
* @param [out]    version_ptr buffer that is filled with the version number 
* @param [in/out] version_len size of version buffer
*/
int qsee_get_fw_component_version(uint8_t *version_ptr, size_t *version_len);

#endif /* QSEE_CORE_H */

