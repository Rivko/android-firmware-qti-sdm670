#ifndef TT_TZ_H
#define TT_TZ_H


/*===========================================================================
         Translation Table Definitions for Abstraction Format

Copyright 2016 by Qualcomm Technologies Incorporated. All Rights Reserved.

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/teetest/teetest/src/aarch64/tt_tz.h#1 $

  From securemsm/trustzone/armlib/v8/inc/
  
when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/11/13   pre     Initial Revision
===========================================================================*/


/* Define TZ proprietary bit format for memory attributes that TZ can
   translate to translation table attributes.
 */

/* Memory type, bits [3:0] */
#define TZ_MEMT_MSK                             0x0000000F
#define TZ_MEM_STRONGLY_ORDERED                 (0 << 0)
#define TZ_MEM_DEVICE                           (1 << 0)
#define TZ_MEM_NON_CACHEABLE                    (2 << 0)
#define TZ_MEM_WRITE_THROUGH_WA                 (3 << 0)
#define TZ_MEM_WRITE_THROUGH_NWA                (4 << 0)
#define TZ_MEM_WRITE_BACK_WA                    (5 << 0)
#define TZ_MEM_WRITE_BACK_NWA                   (6 << 0)
#define TZ_MEM_NOT_USED                         (7 << 0)

/* Memory access, bits [5:4] */
#define TZ_AP_MSK                               0x00000030
#define TZ_READ_WRITE_PL1                       (0 << 4)
#define TZ_READ_WRITE_PL0                       (1 << 4)
#define TZ_READ_ONLY_PL1                        (2 << 4)
#define TZ_READ_ONLY_PL0                        (3 << 4)

/* Executable, bit [6] */
#define TZ_XN_MSK                               0x00000040
#define TZ_EXECUTABLE                           (0 << 6)
#define TZ_NON_EXECUTABLE                       (1 << 6)

/* Domain, bits [10:7] */
#define TZ_DEFAULT_DOMAIN                       (0 << 7)

/* Invalid bit [11] */
#define TZ_INVALID_MSK                          0x00000800
#define TZ_TTE_VALID                            (0 << 11)
#define TZ_TTE_INVALID                          (1 << 11)

/* PXN bit [12] */
#define TZ_PXN_MSK                              0x00001000
#define TZ_PL1_EXECUTABLE                       (0 << 12)
#define TZ_PL1_NON_EXECUTABLE                   (1 << 12)

/* Non secure bit [13] */
#define TZ_NS_MSK                               0x00002000
#define TZ_SECURE                               (0 << 13)
#define TZ_NON_SECURE                           (1 << 13)

/* Shareability bits [15:14] */
#define TZ_SH_MSK                               0x0000C000
#define TZ_NON_SHAREABLE                        (0 << 14)
#define TZ_OUT_SHAREABLE                        (1 << 14)
#define TZ_INR_SHAREABLE                        (2 << 14)

/* Compatability stuff: */
#define TZ_MEM_NON_CACHEABLE_S       (TZ_INR_SHAREABLE | TZ_MEM_NON_CACHEABLE)
#define TZ_MEM_WRITE_BACK_CACHE_S    (TZ_INR_SHAREABLE | TZ_MEM_WRITE_BACK_WA)
#define TZ_MEM_WRITE_THROUGH_CACHE_S (TZ_INR_SHAREABLE | TZ_MEM_WRITE_THROUGH_WA)


/* Macros for User/Privilege permissions */
#define TZ_USER_R                     (TZ_READ_ONLY_PL0 | TZ_NON_EXECUTABLE | TZ_PL1_NON_EXECUTABLE)
#define TZ_USER_RX                    (TZ_READ_ONLY_PL0 | TZ_EXECUTABLE | TZ_PL1_NON_EXECUTABLE)  
#define TZ_USER_RW                    (TZ_READ_WRITE_PL0 | TZ_NON_EXECUTABLE | TZ_PL1_NON_EXECUTABLE)

#define TZ_PRIVILEGE_R                (TZ_READ_ONLY_PL1 | TZ_PL1_NON_EXECUTABLE | TZ_NON_EXECUTABLE)
#define TZ_PRIVILEGE_RX               (TZ_READ_ONLY_PL1 | TZ_PL1_EXECUTABLE | TZ_NON_EXECUTABLE)
#define TZ_PRIVILEGE_RW               (TZ_READ_WRITE_PL1 | TZ_PL1_NON_EXECUTABLE | TZ_NON_EXECUTABLE)

#endif /* TT_TZ_H */
