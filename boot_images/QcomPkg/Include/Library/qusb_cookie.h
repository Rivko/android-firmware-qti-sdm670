#ifndef QUSB_BOOT_DLOAD_H
#define QUSB_BOOT_DLOAD_H

#include "comdef.h"

/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/** Maximum length for the serial number.
*/
#define QUSB_SERIAL_NUMBER_LEN 128

/** A constant serial number to be used when IMEI/ESN is not available.
*/
#define QUSB_DEFAULT_SERIAL_NUMBER "1234567890ABCDEF"

// Maximum size for the QPST cookie
#define QUSB_QPST_COOKIE_SIZE     8

/* Magic numbers for notifying SBL3 to boot if Product ID/Serial Number is set into download mode. */
#define PID_MAGIC_NUMBER           0x71432909
#define SERIAL_NUMBER_MAGIC_NUMBER 0x61945374

/* Structure to define if Product ID/Serial Number is set before entering download mode. */ 
typedef struct
{
  uint32 magic_1;
  uint32 magic_2;
} magic_cookie_type;


/* Structure to define QPST cookie to identify phone in download mode. */ 
typedef struct
{
  uint8 qpst_cookie_length;
  byte qpst_cookie_data[QUSB_QPST_COOKIE_SIZE];
} dload_qpst_cookie_type;

/* Structure for passing information required for the SW download between HLOS, 
** DLOAD and ARMPRG  
** Use dload_info_free array within structure for all the futue additions to structure.
** This structure consumes 140 bytes out of 200 Bytes allocated for the USB IRAM parition 
*/
typedef struct
{
 /* The product ID used by HS-USB before going into download mode.
  ** It is used in download mode (DLOAD and ARMPRG), so that the phone
  ** does not switch com ports.
  */
 /* The Product ID is 2 bytes only. The users will have to typecast for the same*/ 
 uint32                          qusb_product_id; 
  /* The Serial Number used by HS-USB before going into download mode.
  ** It is used in download mode (DLOAD and ARMPRG), so that the phone
  ** does not switch com ports.
  */
  char                           serial_number[QUSB_SERIAL_NUMBER_LEN];
  magic_cookie_type              pid_serial_num_cookie;
}qusb_dload_info_type;

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================

FUNCTION          QUSB_BOOT_DLOAD_IS_DLOAD_INFO_PID_VALID

DESCRIPTION
Checks if the DLOAD related PID is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the PID is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean qusb_boot_dload_is_dload_info_pid_valid(const qusb_dload_info_type* dload_info_ptr);

/*==============================================================================

FUNCTION          QUSB_BOOT_DLOAD_IS_DLOAD_INFO_SERIAL_NUM_VALID

DESCRIPTION
Checks if the DLOAD related Serial Number is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the Serial Number is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean qusb_boot_dload_is_dload_info_serial_num_valid(const qusb_dload_info_type* dload_info_ptr);

/*==============================================================================

FUNCTION          QUSB_BOOT_DLOAD_GET_HSU_DLOAD_INFO_PTR

DESCRIPTION
Get a pointer to the structure that holds information needed for enumeration
in download mode. This information is written in HLOS, before going into 
download mode, and read in download mode.

DEPENDENCIES
None.

RETURN VALUE
Return the address of the memory to hold the SW download data

SIDE EFFECTS

==============================================================================*/
qusb_dload_info_type* qusb_boot_dload_get_dload_info_ptr(void);


#endif /*QUSB_BOOT_DLOAD_H */
