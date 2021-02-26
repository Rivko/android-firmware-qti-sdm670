#ifndef DDR_COMMON_H
#define DDR_COMMON_H

#define DDR_COMMON_H_MAJOR  05
#define DDR_COMMON_H_MINOR  00

/**
  @file ddr_common.h
  @brief
  Common definitions used by DDR drivers on all targets.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ddr_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ddr_mainpage.dox file or contact Tech Pubs.
=============================================================================*/
/*=============================================================================
  Copyright (c) 2014-2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  All rights reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/boot/ddr_common.h#2 $ 
$DateTime: 2019/03/08 09:58:25 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/07/16   rp      Cleaned up the unused structs and renamed all sdram to ddr for naming consistency
07/18/14   tw      added support for highest bank bit information
12/01/13   dp      Initial revision.
=============================================================================*/

 #include "comdef.h"
 
// Add a definition of 'inline' to work around the QNPL comdef.h, which mysteriously omits a definition // of inline.
#if (! defined inline)
#define inline __inline
#endif /* (! defined inline) */

#define DDR_MAX_NUM_CH  4

/** DDR interfaces. */
typedef enum
{
  DDR_CH_NONE   = 0x0,
  DDR_CH0       = 0x1,
  DDR_CH1       = 0x2,
  DDR_CH2       = 0x4,
  DDR_CH3       = 0x8,
  DDR_CH_ALL    = 0xF,
  DDR_CH_MAX    = 0x7FFFFFFF  /**< Forces the enumerator to 32 bits. */
} DDR_CHANNEL;

/** DDR chip selects. */
typedef enum
{
  DDR_CS_NONE        = 0,                 /**< 0b00 -- no chip select. */
  DDR_CS0            = 1,                 /**< 0b01 -- chip select 0 only. */
  DDR_CS1            = 2,                 /**< 0x10 -- chip select 1 only. */
  DDR_CS_BOTH        = 3,                 /**< 0x11 -- Both chip selects. */
  DDR_CS_MAX         = 0x7FFFFFFF         /**< Forces the enumerator to 32 bits. */
} DDR_CHIPSELECT;

/** DDR SNS status. */
typedef enum
{
  DDR_UNAVAILABLE,               /**< DDR is unavailable on this chip select. */
  DDR_ACTIVE,                    /**< Active mode. */
  DDR_SELF_REFRESH,              /**< Self Refresh mode. */
  DDR_STALL,                     /**< Stalled mode. */
  DDR_DPD,                       /**< Deep Power Down mode. */
  DDR_STATUS_MAX = 0x7FFFFFFF    /**< Forces the enumerator to 32 bits. */
} DDR_SNS_STATUS;

/** DDR types. */
typedef enum
{
  DDR_TYPE_LPDDR1 = 0,           /**< Low power DDR1. */
  DDR_TYPE_LPDDR2 = 2,       /**< Low power DDR2  set to 2 for compatibility*/
  DDR_TYPE_PCDDR2 = 3,           /**< Personal computer DDR2. */
  DDR_TYPE_PCDDR3 = 4,           /**< Personal computer DDR3. */
  
  DDR_TYPE_LPDDR3 = 5,           /**< Low power DDR3. */  
  DDR_TYPE_LPDDR4 = 6,           /**< Low power DDR4. */
  DDR_TYPE_LPDDR4X = 7,           

  DDR_TYPE_RESERVED = 8,         /**< Reserved for future use. */
  DDR_TYPE_UNUSED = 0x7FFFFFFF  /**< For compatibility with deviceprogrammer(features not using DDR). */
} DDR_TYPE;

/** DDR manufacturers. */
typedef enum
{
  RESERVED_0,                        /**< Reserved for future use. */
  SAMSUNG,                           /**< Samsung. */
  QIMONDA,                           /**< Qimonda. */
  ELPIDA,                            /**< Elpida Memory, Inc. */
  ETRON,                             /**< Etron Technology, Inc. */
  NANYA,                             /**< Nanya Technology Corporation. */
  HYNIX,                             /**< Hynix Semiconductor Inc. */
  MOSEL,                             /**< Mosel Vitelic Corporation. */
  WINBOND,                           /**< Winbond Electronics Corp. */
  ESMT,                              /**< Elite Semiconductor Memory Technology Inc. */
  RESERVED_1,                        /**< Reserved for future use. */
  SPANSION,                          /**< Spansion Inc. */
  SST,                               /**< Silicon Storage Technology, Inc. */
  ZMOS,                              /**< ZMOS Technology, Inc. */
  INTEL,                             /**< Intel Corporation. */
  NUMONYX = 254,                     /**< Numonyx, acquired by Micron Technology, Inc. */
  MICRON = 255,                      /**< Micron Technology, Inc. */
  DDR_MANUFACTURES_MAX = 0x7FFFFFFF  /**< Forces the enumerator to 32 bits. */
} DDR_MANUFACTURES;

/** DDR interleave type */
typedef enum
{
  DDR_CS0_INTERLEAVE = 1,
  DDR_CS1_INTERLEAVE = 2,
  DDR_CS0_CS1_INTERLEAVE = 3,
} ddr_interleave_type;     


/** @brief DDR states of all interfaces and chip selects. */
typedef struct
{
  DDR_SNS_STATUS ddr_cs0[DDR_MAX_NUM_CH];    /**< Array of DDR status of chip select 0 for each channel. */
  DDR_SNS_STATUS ddr_cs1[DDR_MAX_NUM_CH];    /**< Array of DDR status of chip select 1 for each channel. */

  uint32 clk_speed;           /**< Clock speed. */
  uint32 vddmx_voltage;       /**< VDDMX level in microvolts. */
  uint32 vddcx_voltage;       /**< VDDCX level in microvolts. */
} ddr_interface_state;

/** @brief DDR sizes and start addresses of all interfaces and chip selects. */
typedef struct
{
  uint32 ddr_cs0[DDR_MAX_NUM_CH];       /**< DDR size of Interface0 and chip select 0. */
  uint32 ddr_cs1[DDR_MAX_NUM_CH];       /**< DDR size of Interface0 and chip select 1. */
  
  uint64 ddr_cs0_addr[DDR_MAX_NUM_CH];  /**< DDR start address of Interface0 and chip select 0. */
  uint64 ddr_cs1_addr[DDR_MAX_NUM_CH];  /**< DDR start address of Interface0 and chip select 1. */
  
  uint32 highest_bank_bit; /**< DDR Highest bank bit based on detected topology */
} ddr_size_info;


/** @brief ddr information that are relevent to clients outside of ddr driver */
typedef struct
{
  ddr_size_info ddr_size; /**< DDR size and address configuration */  
  uint32 interleaved_memory; /**< Return whether the ddr is interleaved or not. */
  DDR_TYPE ddr_type;  /**< Return ddr type enum. */ 
} ddr_info;

typedef struct
{
  uint32 num_partitions;   /**< DDR size of Interface0/CS0 */
  uint64 ddr_addr;       /**< DDR start address of Interface0/CS0 */
} ddr_partitions;

typedef struct
{
  ddr_partitions ddr_cs0[DDR_MAX_NUM_CH]; /**< Partition information for CS0 for each interface */
  ddr_partitions ddr_cs1[DDR_MAX_NUM_CH]; /**< Partition information for CS1 for each interface */
} ddr_size_partition;

/** @brief DDR sizes and start addresses of all interfaces and chip selects. */
typedef struct
{
  uint32 ddr_cs0_mb[DDR_MAX_NUM_CH];    /**< DDR size in MBytes of Interface and chip select 0. */
  uint32 ddr_cs1_mb[DDR_MAX_NUM_CH];    /**< DDR size in MBytes of Interface and chip select 1. */
  
  uint8 ddr_cs0_io_width[DDR_MAX_NUM_CH];    /**< DDR Device IO Width of Interface and chip select 0. */
  uint8 ddr_cs1_io_width[DDR_MAX_NUM_CH];    /**< DDR Device IO Width of Interface and chip select 1. */
  
  uint64 ddr_cs0_addr[DDR_MAX_NUM_CH];  /**< DDR start address of Interface and chip select 0. */
  uint64 ddr_cs1_addr[DDR_MAX_NUM_CH];  /**< DDR start address of Interface and chip select 1. */
  
  uint64 ddr_cs0_remapped_addr[DDR_MAX_NUM_CH];  /**< DDR remapped start address of Interface and chip select 0. */
  uint64 ddr_cs1_remapped_addr[DDR_MAX_NUM_CH];  /**< DDR remapped start address of Interface and chip select 1. */
  
  uint32 highest_bank_bit; /**< DDR Highest bank bit based on detected topology */
} ddr_sns_size_info;


/** @} */ /* end_addtogroup ddr_data_structures */

#endif  /* DDR_COMMON_H */