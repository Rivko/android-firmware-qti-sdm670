#ifndef __SDCC_PARTITION_TYPES_H__
#define __SDCC_PARTITION_TYPES_H__
/******************************************************************************
 * File: sdcc_partition_types.h
 *
 * Services: 
 *    Public header for SDCC partition types
 *
 * Description:
 *    This file contains the SDCC partition types.
 *
 * Copyright (c) 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
2014-06-18   bn      Initial version. Branched from 8994
=============================================================================*/

/** \details
 * Partition type USER_AREA denotes the User Data Area on the embedded memory
 * device such as eMMC and eSD.  This user data area is always the default
 * physical partition.
 */
#define PARTITION_USER_AREA          0x00000000

/** \details
 * Partition type BOOT_PARTITION_x denotes the BOOT area partition 1 or 2.
 * Refer to section 7.2.1 in JEDEC Standard No. 84-A43.
 */
#define PARTITION_BOOT_1             0x00000001
#define PARTITION_BOOT_2             0x00000002

/** \details
 * Partition type RPMB_PARTITION denotes the physical partition RPMB.
 * Refer to section 7.2.1 in JEDEC Standard No. 84-A43.
 */
#define PARTITION_RPMB               0x00000003

/** \details
 * Partition type GPP_PARTITION_x denotes the GPP area partition 1,2,3 or 4.
 * Refer to section 7.2.1 in JEDEC Standard No. 84-A43.
 */
#define PARTITION_GPP_1              0x00000004
#define PARTITION_GPP_2              0x00000005
#define PARTITION_GPP_3              0x00000006
#define PARTITION_GPP_4              0x00000007

#endif /* __SDCC_PARTITION_TYPES_H__ */
