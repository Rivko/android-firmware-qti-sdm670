
/*===========================================================================
Copyright (c) 2015-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/

#ifndef __TEETEST_MMU_H
#define __TEETEST_MMU_H

#include <stdint.h>
#include <stddef.h>
#include "teetest.h"

/*
  Bits [14:13] are TEX bits [2:1] for L1 tables and are not used by
  the MMU hardware when TEX remap is enabled, and thus are called "OS
  managed"
 */
#define L1_OS_MANAGED_0                         (0 << 13)
#define L1_OS_MANAGED_1                         (1 << 13)
#define L1_OS_MANAGED_2                         (2 << 13)
#define L1_OS_MANAGED_3                         (3 << 13)

/*
  Bits [8:7] are TEX bits [2:1] for L2 tables and are not used by the
  MMU hardware when TEX remap is enabled, and thus are called "OS
  managed"
 */
#define L2_OS_MANAGED_0                         (0 << 7)
#define L2_OS_MANAGED_1                         (1 << 7)
#define L2_OS_MANAGED_2                         (2 << 7)
#define L2_OS_MANAGED_3                         (3 << 7)


/*
  Bit [16] specifies the S bit for L1 and Bit [10] for L2 - if a
  normal memory region is shared.
*/
#define L1_SHARED                               (1 << 16)
#define L1_NONSHARED                            (0 << 16)
#define L2_SHARED                               (1 << 10)
#define L2_NONSHARED                            (0 << 10)


/*
  Bits [3:2] are used with TEX bit 0 to index into PRRR/NMRR or MAIR0/MAIR1
*/
#define B_SET                                   (1 << 2)
#define B_CLEAR                                 (0 << 2)
#define C_SET                                   (1 << 3)
#define C_CLEAR                                 (0 << 3)

/*
  Bit 12 is the TEX bit for L1.  Together with B/C bits they specify
  an index into PRRR/NMRR or MAIR0/MAIR1
 */
#define L1_TEX_0                                (0 << 12)
#define L1_TEX_1                                (1 << 12)

/*
  Bit 6 is the TEX bit for L2.  Together with B/C bits they specify an
  index into PRRR/NMRR or MAIR0/MAIR1
 */
#define L2_TEX_0                                (0 << 6)
#define L2_TEX_1                                (1 << 6)

/*
  Memory Attribute Indirection Register (MAIR) settings
 */
#define L1_MAIR_0                               (L1_TEX_0 | C_CLEAR | B_CLEAR)
#define L1_MAIR_1                               (L1_TEX_0 | C_CLEAR | B_SET)
#define L1_MAIR_2                               (L1_TEX_0 | C_SET   | B_CLEAR)
#define L1_MAIR_3                               (L1_TEX_0 | C_SET   | B_SET)
#define L1_MAIR_4                               (L1_TEX_1 | C_CLEAR | B_CLEAR)
#define L1_MAIR_5                               (L1_TEX_1 | C_CLEAR | B_SET)
#define L1_MAIR_6                               (L1_TEX_1 | C_SET   | B_CLEAR)
#define L1_MAIR_7                               (L1_TEX_1 | C_SET   | B_SET)

#define L2_MAIR_0                               (L2_TEX_0 | C_CLEAR | B_CLEAR)
#define L2_MAIR_1                               (L2_TEX_0 | C_CLEAR | B_SET)
#define L2_MAIR_2                               (L2_TEX_0 | C_SET   | B_CLEAR)
#define L2_MAIR_3                               (L2_TEX_0 | C_SET   | B_SET)
#define L2_MAIR_4                               (L2_TEX_1 | C_CLEAR | B_CLEAR)
#define L2_MAIR_5                               (L2_TEX_1 | C_CLEAR | B_SET)
#define L2_MAIR_6                               (L2_TEX_1 | C_SET   | B_CLEAR)
#define L2_MAIR_7                               (L2_TEX_1 | C_SET   | B_SET)

#define PRR_TR_STRONGLY_ORDERED                 0
#define PRR_TR_DEVICE                           1
#define PRR_TR_NORMAL                           2
#define PRR_TR_RESERVED                         3

#define SET_PRRR_MEM(I, NOS, TR)                ((NOS & 1UL) << (24 + I) | \
                                                 ((TR & 3)   << (2 * I)))
#define SET_PRRR_NS(I, S)                       ((S & 1) << (18 + I))
#define SET_PRRR_DS(I, S)                       ((S & 1) << (16 + I))

#define NMRR_NON_CACHEABLE                      0
#define NMRR_WRITE_BACK_WA                      1
#define NMRR_WRITE_THROUGH_NWA                  2
#define NMRR_WRITE_BACK_NWA                     3

#define SET_NMRR_OR(I, OR)                      ((OR & 3) << (16 + (I * 2)))
#define SET_NMRR_IR(I, IR)                      ((IR & 3) << (I * 2))


#define L1_MEM_STRONGLY_ORDERED                  L1_MAIR_0
#define L1_MEM_DEVICE                            L1_MAIR_1
#define L1_MEM_NON_CACHEABLE                     L1_MAIR_2
#define L1_MEM_WRITE_THROUGH_WA                  L1_MAIR_3
#define L1_MEM_WRITE_THROUGH_NWA                 L1_MAIR_4
#define L1_MEM_WRITE_BACK_WA                     L1_MAIR_5
#define L1_MEM_WRITE_BACK_NWA                    L1_MAIR_6

#define L2_MEM_STRONGLY_ORDERED                  L2_MAIR_0
#define L2_MEM_DEVICE                            L2_MAIR_1
#define L2_MEM_NON_CACHEABLE                     L2_MAIR_2
#define L2_MEM_WRITE_THROUGH_WA                  L2_MAIR_3
#define L2_MEM_WRITE_THROUGH_NWA                 L2_MAIR_4
#define L2_MEM_WRITE_BACK_WA                     L2_MAIR_5
#define L2_MEM_WRITE_BACK_NWA                    L2_MAIR_6

/*
   Bits [4] is the eXecute Never bit for L1 and bit [0] is the eXecute
   Never bit for L2. The region is executable if set to 0 and not
   executable if set to 1.
*/
#define L1_EXECUTABLE                           (0 << 4)
#define L1_NON_EXECUTABLE                       (1 << 4)
#define L2_EXECUTABLE                           (0)
#define L2_NON_EXECUTABLE                       (1)

/*
   Bits [8:5] specify one of the 16 domains
   everything is mapped to one domain for simplicity:
   The default domain is set to 0
*/
#define L1_DEFAULT_DOMAIN                       (0 << 5)

/*
   Bit [9] Specifies the P bit - whether the memory
   region has ECC enabled. TRM p0r2 says "Should be Zero"
*/
#define L1_ECC_ENABLED                          (1 << 9)
#define L1_ECC_DISABLED                         (0 << 9)

/*
   Bits [11:10] Specifies the Access Permisions (AP bits) along with
   the APX bit [15] for L1. Bits [5:4] specifies the Access Permission
   (AP) along with the APX bit [9] for L2.  AP APX
*/
#define L1_READ_WRITE_PL1                       ((0 << 15) | (1 << 10))
#define L1_READ_WRITE_PL0                       ((0 << 15) | (3 << 10))
#define L1_READ_ONLY_PL1                        ((1 << 15) | (1 << 10))
#define L1_READ_ONLY_PL0                        ((1 << 15) | (3 << 10))
#define L2_READ_WRITE_PL1                       ((0 <<  9) | (1 <<  4))
#define L2_READ_WRITE_PL0                       ((0 <<  9) | (3 <<  4))
#define L2_READ_ONLY_PL1                        ((1 <<  9) | (1 <<  4))
#define L2_READ_ONLY_PL0                        ((1 <<  9) | (3 <<  4))

/* Memory attributes used for translation table walks defined in TTBRs
 */
#define TTBR_IRGN_NC                            ((0 << 0) | (0 << 6))
#define TTBR_IRGN_WB_WA                         ((0 << 0) | (1 << 6))
#define TTBR_IRGN_WT                            ((1 << 0) | (0 << 6))
#define TTBR_IRGN_WB_NWA                        ((1 << 0) | (1 << 6))
#define TTBR_ORGN_NC                            ((0 << 4) | (0 << 3))
#define TTBR_ORGN_WB_WA                         ((0 << 4) | (1 << 3))
#define TTBR_ORGN_WT                            ((1 << 4) | (0 << 3))
#define TTBR_ORGN_WB_NWA                        ((1 << 4) | (1 << 3))
#define TTBR_NON_SHARED                         ((0 << 5) | (0 << 1))
#define TTBR_INNR_SHARED                        ((1 << 5) | (1 << 1))
#define TTBR_OUTR_SHARED                        ((0 << 5) | (1 << 1))

/*****************************************************************************
 *                             !!! CAUTION !!!                               *
 * Indices must match tt_tz.h or else memory type abstraction will not work. *
 *****************************************************************************/

#define PRRR_INDEX_0  SET_PRRR_MEM(0,     0,   PRR_TR_STRONGLY_ORDERED)
#define NMRR_INDEX_0 (SET_NMRR_OR (0,     NMRR_NON_CACHEABLE)      |  \
                      SET_NMRR_IR (0,     NMRR_NON_CACHEABLE))

#define PRRR_INDEX_1  SET_PRRR_MEM(1,     1,   PRR_TR_DEVICE)
#define NMRR_INDEX_1 (SET_NMRR_OR (1,     NMRR_NON_CACHEABLE)      |  \
                      SET_NMRR_IR (1,     NMRR_NON_CACHEABLE))

#define PRRR_INDEX_2  SET_PRRR_MEM(2,     1,   PRR_TR_NORMAL)
#define NMRR_INDEX_2 (SET_NMRR_OR (2,     NMRR_NON_CACHEABLE)      |  \
                      SET_NMRR_IR (2,     NMRR_NON_CACHEABLE))

#define PRRR_INDEX_3  SET_PRRR_MEM(3,     1,   PRR_TR_NORMAL)
#define NMRR_INDEX_3 (SET_NMRR_OR (3,     NMRR_WRITE_THROUGH_NWA)  |  \
                      SET_NMRR_IR (3,     NMRR_WRITE_THROUGH_NWA))

#define PRRR_INDEX_4  SET_PRRR_MEM(4,     1,   PRR_TR_NORMAL)
#define NMRR_INDEX_4 (SET_NMRR_OR (4,     NMRR_WRITE_THROUGH_NWA)  |  \
                      SET_NMRR_IR (4,     NMRR_WRITE_THROUGH_NWA))

#define PRRR_INDEX_5  SET_PRRR_MEM(5,     1,   PRR_TR_NORMAL)
#define NMRR_INDEX_5 (SET_NMRR_OR (5,     NMRR_WRITE_BACK_WA)      |  \
                      SET_NMRR_IR (5,     NMRR_WRITE_BACK_WA))

#define PRRR_INDEX_6  SET_PRRR_MEM(6,     1,   PRR_TR_NORMAL)
#define NMRR_INDEX_6 (SET_NMRR_OR (6,     NMRR_WRITE_BACK_NWA)     |  \
                      SET_NMRR_IR (6,     NMRR_WRITE_BACK_NWA))

#define PRRR_INDEX_7  SET_PRRR_MEM(7,     1,   PRR_TR_NORMAL)
#define NMRR_INDEX_7 (SET_NMRR_OR (7,     NMRR_WRITE_BACK_WA)      |  \
                      SET_NMRR_IR (7,     NMRR_WRITE_BACK_WA))


#define PRRR (PRRR_INDEX_0 | PRRR_INDEX_1 | PRRR_INDEX_2 | PRRR_INDEX_3 | \
              PRRR_INDEX_4 | PRRR_INDEX_5 | PRRR_INDEX_6 | PRRR_INDEX_7 | \
              /*          Index  Shareable */                             \
              SET_PRRR_NS(0,     0)  |                                    \
              SET_PRRR_NS(1,     1)  |                                    \
              SET_PRRR_DS(0,     1)  |                                    \
              SET_PRRR_DS(1,     1))

#define NMRR (NMRR_INDEX_0 | NMRR_INDEX_1 | NMRR_INDEX_2 | NMRR_INDEX_3 |  \
              NMRR_INDEX_4 | NMRR_INDEX_5 | NMRR_INDEX_6 | NMRR_INDEX_7)

#define TTBR_IRGN_WB_WA                         ((0 << 0) | (1 << 6))
#define TTBR_ORGN_WB_WA                         ((0 << 4) | (1 << 3))
#define TTBR_INNR_SHARED                        ((1 << 5) | (1 << 1))

#define TTBR0_MEM_ATTS                          (TTBR_IRGN_WB_WA | TTBR_ORGN_WB_WA | TTBR_INNR_SHARED)

#define PA_SIZE                                 36
#define SIZE_4KB                                0x00000001000
#define SIZE_1MB                                0x00000100000
#define SIZE_4GB                                0x00100000000
#define VAS_SIZE                                SIZE_4GB
#define NUM_L1_TTES                             (VAS_SIZE / SIZE_1MB)

#define ROUND_DOWN_1MB(xx)                      ((uint32_t) (xx) & ~(SIZE_1MB - 1))
#define B2L1TTE(xx)                             ((xx) >> 20)

#define L1_MAP_SIZE                             SIZE_1MB
#define L1_SIZE_MSK                             ((uint32_t)(L1_MAP_SIZE - 1))
#define L1_ADDR_MSK                             ~L1_SIZE_MSK
#define L2_MAP_SIZE                             SIZE_4KB

#define NUM_L2_TTES_PER_L1_TTE                  (SIZE_1MB / SIZE_4KB)

#define B2L2TTE(xx)                             (((xx) >> 12) & 0xFF)
#define B2KB(xx)                                ((xx) >> 10)
#define KB2B(xx)                                ((xx) << 10)
#define B2NUML1TTES(xx)                         ((xx) / L1_MAP_SIZE)
#define KB2NUML1TTES(xx)                        ((xx) / B2KB(L1_MAP_SIZE))
#define B2NUML2TTES(xx)                         ((xx) / L2_MAP_SIZE)
#define KB2NUML2TTES(xx)                        ((xx) / B2KB(L2_MAP_SIZE))
#define L1TTE2B(xx)                             ((xx) << 20)
#define L2TTE2B(xx)                             ((xx) << 12)

#define NUM_L2_TTS                              10

#define L1_SECONDARY_TTE_DESCRIPTOR             0x1
#define L2_LARGE_PAGE_DESCRIPTOR                0x1

#define TTE_INVALID                             (1 << 11)

typedef uintnt    sd_tte_t;
typedef uintnt    l2tt_iter_t;

typedef struct
{
  sd_tte_t tt[NUM_L1_TTES];
} l1_tt_t;

typedef struct
{
  sd_tte_t tt[NUM_L2_TTES_PER_L1_TTE];
} l2_tt_t;

/*****************************************************************************
 * MMU related functions
 *****************************************************************************/
void tee_mmu_map (unsigned int addr, unsigned int size);
void tee_mmu_init(uintptr_t tee_phy_addr, size_t tee_len, uintptr_t ipc_addr, size_t ipc_len);

#endif
