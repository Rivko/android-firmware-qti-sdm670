#ifndef TT_LD_H
#define TT_LD_H

/*===========================================================================
         Translation Table Definitions for Long Descriptor Format

Copyright 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/teetest/teetest/src/aarch64/tt_ld.h#1 $
  
  From securemsm/trustzone/armlib/v8/inc/

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/21/13   pre     Initial Revision
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <stdint.h>
#include <stddef.h>

#define PA_SIZE               36
#define TZ_VAS_SIZE           (1ULL << PA_SIZE)
#define VAS_LAST_ADDRESS      (TZ_VAS_SIZE - 1)
                     
#define KB2B(xx)              ((xx) << 10)
#define B2KB(xx)              ((xx) >> 10)

#define TZ_4GB                0x00100000000
#define TZ_1GB                0x00040000000
#define TZ_2MB                0x00000200000
#define TZ_1MB                0x00000100000
#define TZ_4KB                0x00000001000

#define RETCHK(xx)            if ((xx) != E_SUCCESS) {return (xx);}

/* Align val down to be a multiple of size, size needs to be a power of two */
#define ALIGN_DOWN(val, size) ((val) & ~((size) - 1))

/* Align val up to be a multiple of size, size needs to be a power of two */
#define ALIGN_UP(val, size)   (((val) + (size) - 1) & ~((size) - 1))

#define FLOOR_4K(xx)          ALIGN_DOWN((xx), TZ_4KB)

#define ROUNDUP(value, round) (0 == value % round ?     \
                               value :                  \
                               value + (round - (value % round)))

#define TYPE_ROUNDUP(type, val, multiple)   (((val) + (multiple)-1) & \
                                             ~(type)((multiple)-1))
#define TYPE_ROUNDDOWN(type, val, multiple) ((val) & \
                                             ~(type)((multiple)-1))

#define NUM_L1_TTES            (TZ_VAS_SIZE / TZ_1GB)
#define NUM_L2_TTES_PER_L1_TTE (TZ_1GB / TZ_2MB)
#define NUM_L3_TTES_PER_L2_TTE (TZ_2MB / TZ_4KB)

#define MIN_MAP_SIZE_SHIFT 12
#define MIN_MAP_SIZE       (1<<MIN_MAP_SIZE_SHIFT)

#define L1_MAP_SIZE  TZ_1GB
#define L2_MAP_SIZE  TZ_2MB
#define L3_MAP_SIZE  MIN_MAP_SIZE

#define L1_SIZE_MSK ((uint64_t)(L1_MAP_SIZE - 1))
#define L2_SIZE_MSK ((uint64_t)(L2_MAP_SIZE - 1))
#define L3_SIZE_MSK ((uint64_t)(L3_MAP_SIZE - 1))

#define L1_ADDR_MSK ~L1_SIZE_MSK
#define L2_ADDR_MSK ~L2_SIZE_MSK
#define L3_ADDR_MSK ~L3_SIZE_MSK

/* TODO: The dynamic L2 allocation and freeing relied on marking a TTE
   as a large page in order to signify it was not used.  Temporarily
   try setting the descriptor to an invalid descriptor (2) and look
   into fixing later.  */
#define L2_UNUSED_DESC                           2
#define L3_UNUSED_DESC                           2

/* Right shifts are implementation defined, so define a mask that does
   not depend on right shifting the mask arithmetically.  Define size
   masks to be consistant.
 */
#define L1_KB_SIZE_MSK ((uint64_t)(B2KB(L1_MAP_SIZE) - 1))
#define L2_KB_SIZE_MSK ((uint64_t)(B2KB(L2_MAP_SIZE) - 1))
#define L3_KB_SIZE_MSK ((uint64_t)(B2KB(L3_MAP_SIZE) - 1))

#define L1_KB_ADDR_MSK ((uint64_t)~(L1_KB_SIZE_MSK))
#define L2_KB_ADDR_MSK ((uint64_t)~(L2_KB_SIZE_MSK))
#define L3_KB_ADDR_MSK ((uint64_t)~(L3_KB_SIZE_MSK))

#define TTE_SIZE 8
#define L1_TT_SIZE (TZBSP_1TB / TZBSP_1GB)
#define L2_TT_SIZE (TZBSP_1GB / TZBSP_2MB)
#define L3_TT_SIZE (TZBSP_2MB / TZBSP_4KB)

/***************************************************************************/
/*                Block Descriptor Format Definitions                      */
/***************************************************************************/
/* [63:59] Ignored */
/* [58:55] SW Use */
/* [54] Execute Never (XN) */
#define BLK_XN                                  (1ULL << 54)
#define BLK_X                                   (0ULL << 54)

/* [53] Privledged Execute Never (PXN) */
#define BLK_PXN                                 (1ULL << 53)
#define BLK_PX                                  (0ULL << 53)

/* [52] Contiguous Hint */
#define BLK_CONTIGUOUS_HINT                     (1ULL << 52)

/* [51:40] UNK/SBZP */
/* [39:30] Physical Address */
/* [29:12] UNK/SBZP */

/* [11] Non Global (nG) */
#define BLK_NG                                  (1 << 11)
#define BLK_G                                   (0 << 11)

/* [10] Access Flag (AF) */
#define BLK_AF                                  (1 << 10)

/* [9:8] Shareability Field (SH) */
#define BLK_NOT_SHAREABLE                       (0 << 8)
#define BLK_OUT_SHAREABLE                       (2 << 8)
#define BLK_INR_SHAREABLE                       (3 << 8)

/* [7:6] Access Permissions (AP) */
#define BLK_AP_PL1_RW                           (0 << 6)
#define BLK_AP_RW                               (1 << 6)
#define BLK_AP_PL1_RO                           (2 << 6)
#define BLK_AP_RO                               (3 << 6)

/* [5] Non Secure (NS) */
#define BLK_NS                                  (1 << 5)
#define BLK_S                                   (0 << 5)

/* [4:2] Memory Attributes Index (AttrIndx) */
#define BLK_MAI_0                               (0 << 2)
#define BLK_MAI_1                               (1 << 2)
#define BLK_MAI_2                               (2 << 2)
#define BLK_MAI_3                               (3 << 2)
#define BLK_MAI_4                               (4 << 2)
#define BLK_MAI_5                               (5 << 2)
#define BLK_MAI_6                               (6 << 2)
#define BLK_MAI_7                               (7 << 2)

/* [1:0] Descriptor Type */
#define L1_BLK_DESC                              1
#define L2_BLK_DESC                              1
#define L3_BLK_DESC                              3


/***************************************************************************/
/*                Block Descriptor Mask Definitions                        */
/***************************************************************************/
/* [1:0] Descriptor Mask */
#define TTE_DESC_MSK                             0x3

/* [63:52] [11:2] Upper and Lower Page Attributes Mask */
#define TTE_ATTR_MSK                             0xFFF0000000000FFC


/***************************************************************************/
/*                Table Descriptor Format Definitions                      */
/***************************************************************************/
/* [63] NSTable */
#define TBL_NON_SECURE                          (1 << 63)

/* [62:61] APTable */
#define TBL_AP_PL1_RW                           (0 << 61)
#define TBL_AP_RW                               (1 << 61)
#define TBL_AP_PL1_RO                           (2 << 61)
#define TBL_AP_RO                               (3 << 61)

/* [60] XNTable */
#define TBL_XN                                  (1 << 60)

/* [59] PXNTable */
#define TBL_PXN                                 (1 << 59)

/* [58:52] Ignored */
/* [51:40] UNK/SBZP */
/* [39:12] L2 Addr */
/* [11:2] Ignored */

/* [1:0] Descriptor Type */
#define L1_TBL_DESC                              3
#define L2_TBL_DESC                              3

/***************************************************************************/
/*                 Invalid Descriptor Format Definition                    */
/***************************************************************************/
/* [63:2] Ignored */
/* [1:0] Descriptor Type */
#define INV_DESC                                 0
#define L1_INV_DESC                              INV_DESC
#define L2_INV_DESC                              INV_DESC
#define L3_INV_DESC                              INV_DESC

/***************************************************************************/
/*                       MAIR Format Generation                            */
/***************************************************************************/
#define MAIR_STRONGLY_ORDERED                   0x00UL
#define MAIR_INNER_WT_WA_NRA_TR                 0x01UL /* imp def */
#define MAIR_INNER_WT_RA_NWA_TR                 0x02UL /* imp def */
#define MAIR_INNER_WT_RWA_TR                    0x03UL /* imp def */
#define MAIR_INNER_NON_CACHEABLE                0x04UL
#define MAIR_INNER_WB_WA_NRA_TR                 0x05UL /* imp def */
#define MAIR_INNER_WB_RA_NWA_TR                 0x06UL /* imp def */
#define MAIR_INNER_WB_RWA_TR                    0x07UL /* imp def */
#define MAIR_INNER_WT_NRWA_NTR                  0x08UL /* imp def */
#define MAIR_INNER_WT_WA_NRA_NTR                0x09UL /* imp def */
#define MAIR_INNER_WT_RA_NWA_NTR                0x0AUL /* imp def */
#define MAIR_INNER_WT_RWA_NTR                   0x0BUL /* imp def */
#define MAIR_INNER_WB_NRWA_NTR                  0x0CUL /* imp def */
#define MAIR_INNER_WB_WA_NRA_NTR                0x0DUL /* imp def */
#define MAIR_INNER_WB_RA_NWA_NTR                0x0EUL /* imp def */
#define MAIR_INNER_WB_RWA_NTR                   0x0FUL /* imp def */
#define MAIR_OUTER_WT_WA_NRA_TR                 0x10UL /* imp def */
#define MAIR_OUTER_WT_RA_NWA_TR                 0x20UL /* imp def */
#define MAIR_OUTER_WT_RWA_TR                    0x30UL /* imp def */
#define MAIR_OUTER_NON_CACHEABLE                0x40UL
#define MAIR_OUTER_WB_WA_NRA_TR                 0x50UL /* imp def */
#define MAIR_OUTER_WB_RA_NWA_TR                 0x60UL /* imp def */
#define MAIR_OUTER_WB_RWA_TR                    0x70UL /* imp def */
#define MAIR_OUTER_WT_NRWA_NTR                  0x80UL /* imp def */
#define MAIR_OUTER_WT_WA_NRA_NTR                0x90UL /* imp def */
#define MAIR_OUTER_WT_RA_NWA_NTR                0xA0UL /* imp def */
#define MAIR_OUTER_WT_RWA_NTR                   0xB0UL /* imp def */
#define MAIR_OUTER_WB_NRWA_NTR                  0xC0UL /* imp def */
#define MAIR_OUTER_WB_WA_NRA_NTR                0xD0UL /* imp def */
#define MAIR_OUTER_WB_RA_NWA_NTR                0xE0UL /* imp def */
#define MAIR_OUTER_WB_RWA_NTR                   0xF0UL /* imp def */

#define MAIR_DEVICE (MAIR_STRONGLY_ORDERED | MAIR_INNER_NON_CACHEABLE)
#define MAIR_NORMAL (MAIR_INNER_NON_CACHEABLE | MAIR_OUTER_NON_CACHEABLE)

/* TODO: This is here due to some crap in tzbsp_ext_os.c that should
   get fixed. */
#define B2L1TTE(xx)  (((xx) / L1_MAP_SIZE) & 0x1FF)
#define KB2L1TTE(xx) (((xx) / B2KB(L1_MAP_SIZE)) & 0x1FF)
#define B2L2TTE(xx)  (((xx) / L2_MAP_SIZE) & 0x1FF)
#define KB2L2TTE(xx) (((xx) / B2KB(L2_MAP_SIZE)) & 0x1FF)
#define B2L3TTE(xx)  (((xx) / L3_MAP_SIZE) & 0x1FF)
#define KB2L3TTE(xx) (((xx) / B2KB(L3_MAP_SIZE)) & 0x1FF)

#define B2NUML1TTES(xx)  ((xx) / L1_MAP_SIZE)
#define KB2NUML1TTES(xx) ((xx) / B2KB(L1_MAP_SIZE))
#define B2NUML2TTES(xx)  ((xx) / L2_MAP_SIZE)
#define KB2NUML2TTES(xx) ((xx) / B2KB(L2_MAP_SIZE))
#define B2NUML3TTES(xx)  ((xx) / L3_MAP_SIZE)
#define KB2NUML3TTES(xx) ((xx) / B2KB(L3_MAP_SIZE))

#define L1TTE2B(xx)  ((xx) * L1_MAP_SIZE)
#define L1TTE2KB(xx) ((xx) * B2KB(L1_MAP_SIZE))
#define L2TTE2B(xx)  ((xx) * L2_MAP_SIZE)
#define L3TTE2B(xx)  ((xx) * L3_MAP_SIZE)

#define L1BLKADDRMSK(xx) ((xx) & 0xFFC0000000ULL)
#define L1TBLADDRMSK(xx) ((xx) & 0xFFFFFFF000ULL)
#define L2BLKADDRMSK(xx) ((xx) & 0xFFFFE00000ULL)
#define L2TBLADDRMSK(xx) (L1TBLADDRMSK(xx))
#define L3BLKADDRMSK(xx) (L1TBLADDRMSK(xx))
#define L3TBLADDRMSK(xx) (L1TBLADDRMSK(xx))

#define ISL1TBL(xx) (((xx) & 0x3) == L1_TBL_DESC)
#define ISL2TBL(xx) ISL1TBL(xx)

#ifndef _ARM_ASM_

typedef uint64_t ld_tte_t;

typedef struct
{
  ld_tte_t tt[NUM_L1_TTES];
} l1_tt_t;

typedef struct
{
  ld_tte_t tt[NUM_L2_TTES_PER_L1_TTE];
} l2_tt_t;

typedef struct
{
  ld_tte_t tt[NUM_L3_TTES_PER_L2_TTE];
} l3_tt_t;

typedef struct
{
  uint32_t ii;
  uint32_t lvl;
} tt_iter_t;

#endif /* _ARM_ASM_ */

#endif /* TT_LD_H */
