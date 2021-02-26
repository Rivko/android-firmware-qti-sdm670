/***********************************************************************
 * coldplug_config.h
 *
 * Coldplug module.
 * Copyright (C) 2017 QUALCOMM Technologies, Inc.
 *
 * The Coldplug module initializes the storage device (eMMC / UFS / SD) and
 * parses the partitions (MBR / GPT) and allows users to read and write to
 * the partitions.
 *
 * This file holds all the configurable items for the coldplug module.
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/storage/coldplug_config.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2017-07-12   rp    Increaes max lun count from 64 to 100
  2017-05-18   rp    Create

===========================================================================*/

#ifndef __COLDPLUG_CONFIG_H__
#define __COLDPLUG_CONFIG_H__

#ifdef COLDPLUG_SIMULATOR_BUILD
  #include "customer.h"
#endif

/*************************************************************************//**
 * @defgroup coldplug_module Coldplug Module
 *
 * Coldplug supports both attached media (eMMC / UFS / SPI-NOR) and
 * removable media (MMC/SD) through the notion of a coldplug moudle.
 *
 * The coldplug module can open any of the above mentioned devices and parse
 * the partition-table (MBR/GPT) and detect all the partition and provides
 * APIs for coldplug clients to open/read/write/close and also other
 * operations on these partitions.
 *
 * This module can also mount the FAT-partitions with the EFS.
 *
 * The entire coldplug module is featurized on @b FEATURE_COLDPLUG.
*****************************************************************************/

/*************************************************************************//**
 * @defgroup coldplug_config Coldplug configuration items
 *  @ingroup coldplug_module
 * \n \n
 * These are all the coldplug compile-time configuration items
*****************************************************************************/

/*************************************************************************//**
 * ::COLDPLUG_LBA_SIZE_IN_BYTES
 * The biggest expected LBA size across all the coldplug flash devices.
 * Coldplug module will declare an buffer of this size to use while parsing
 * the partition table on the device.
*****************************************************************************/
#ifndef COLDPLUG_LBA_SIZE_IN_BYTES
  #if defined(COLDPLUG_UFS_ENABLE) ||              \
      defined(COLDPLUG_SPI_NOR_ENABLE)
      #define  COLDPLUG_LBA_SIZE_IN_BYTES  (4096)
  #else
    #define  COLDPLUG_LBA_SIZE_IN_BYTES    (512)
  #endif
#endif

#ifndef COLDPLUG_MIN_LBA_SIZE_IN_BYTES
  #define COLDPLUG_MIN_LBA_SIZE_IN_BYTES   (512)
#endif

#if COLDPLUG_MIN_LBA_SIZE_IN_BYTES > COLDPLUG_LBA_SIZE_IN_BYTES
  #error "Coldplug min-lba-size exceeds lba-size"
#endif

/*************************************************************************//**
 * ::COLDPLUG_PCACHE_SIZE_IN_BYTES
 * The size of the page-cache buffer to use while parsing the partition table
 * on the device. This buffer should be at least as big as one LBA.
 * A bigger buffer would result in less reads on the coldplug flash device
 * and therefore will decrease the GPT partition table parsing time.
*****************************************************************************/
#ifndef COLDPLUG_PCACHE_SIZE_IN_BYTES
  #define COLDPLUG_PCACHE_SIZE_IN_BYTES COLDPLUG_LBA_SIZE_IN_BYTES
#endif

#if COLDPLUG_PCACHE_SIZE_IN_BYTES < COLDPLUG_LBA_SIZE_IN_BYTES
  #error "Coldplug parser cache must be at least 1 LBA"
#endif

/*************************************************************************//**
 * ::COLDPLUG_MAX_DEVICES
 * The maximum number of coldplug devices.
 *****************************************************************************/
#ifndef COLDPLUG_MAX_DEVICES
  #define COLDPLUG_MAX_DEVICES (3)
#endif

/*************************************************************************//**
 * ::COLDPLUG_MAX_LUNS_IN_ONE_SLOT
 * The maximum number of LUNs in each slot.
 *****************************************************************************/
#ifndef COLDPLUG_MAX_LUNS_IN_ONE_SLOT
  #define COLDPLUG_MAX_LUNS_IN_ONE_SLOT (8)
#endif

/*************************************************************************//**
 * ::COLDPLUG_MAX_LUNS
 * The maximum number of LUNs in ALL slots.
 *****************************************************************************/
#ifndef COLDPLUG_MAX_LUNS
  #define COLDPLUG_MAX_LUNS (8)
#endif

/*************************************************************************//**
 * ::COLDPLUG_MAX_SD_OR_EMMC_SLOTS
 * The maximum number of SD/eMMC slots possible in an target.
*****************************************************************************/
#ifndef COLDPLUG_MAX_SD_OR_EMMC_SLOTS
  #define COLDPLUG_MAX_SD_OR_EMMC_SLOTS (4)
#endif

/*************************************************************************//**
 * ::COLDPLUG_MAX_UFS_SLOTS
 * The maximum number of UFS slots possible in an target.
*****************************************************************************/
#ifndef COLDPLUG_MAX_UFS_SLOTS
  #define COLDPLUG_MAX_UFS_SLOTS (1)
#endif

/*************************************************************************//**
 * ::COLDPLUG_MAX_SPI_NOR_SLOTS
 * The maximum number of SPI_NOR slots possible in an target.
*****************************************************************************/
#ifndef COLDPLUG_MAX_SPI_NOR_SLOTS
  #define COLDPLUG_MAX_SPI_NOR_SLOTS (1)
#endif

/*************************************************************************//**
 * ::COLDPLUG_MAX_SLOTS
 * The maximum number of slots possible in an target.
 *****************************************************************************/
#ifndef COLDPLUG_MAX_SLOTS
  #define COLDPLUG_MAX_SLOTS (5)
#endif

/*************************************************************************//**
 * ::COLDPLUG_MAX_SPI_NOR_LUNS
 * The maximum number of SPI_NOR luns possible in an target.
*****************************************************************************/
#ifndef COLDPLUG_MAX_SPI_NOR_LUNS_IN_ONE_SLOT
  #define COLDPLUG_MAX_SPI_NOR_LUNS_IN_ONE_SLOT (1)
#endif



/*************************************************************************//**
 * ::COLDPLUG_MAX_PARTITIONS
 * The maximum number of GPT/MBR/FAT partitions that should be supported
 * across all coldplug flash devices (both internal or external).
*****************************************************************************/
#ifndef COLDPLUG_MAX_PARTITIONS
  #define COLDPLUG_MAX_PARTITIONS (100)
#endif

/*************************************************************************//**
 * ::COLDPLUG_MAX_PARTITION_NAME_LEN
 * The coldplug module assigns an partition-name (ex: ""/p-%d")for all the
 * coldplug partitions and this define specifies maximum how many
 * characters should be in the partition name.
*****************************************************************************/
#ifndef COLDPLUG_MAX_PARTITION_NAME_LEN
  #define COLDPLUG_MAX_PARTITION_NAME_LEN (6)
#endif

/*************************************************************************//**
 * ::COLDPLUG_MAX_HANDLES
 * The maximum number of coldplug-handles that will be use in simultaneously.
 * Each coldplug_open_xxx() API will return an coldplug_handle and this define
 * can be configured to account for the number of coldplug_handles that the
 * coldplug-clients will keep it open simultaneously.
*****************************************************************************/
#ifndef COLDPLUG_MAX_HANDLES
  #define COLDPLUG_MAX_HANDLES (5)
#endif

/*************************************************************************//**
 * ::COLDPLUG_MAX_PARTITIONS_PER_LUN
 * The maximum number of partitions to parse in any one given LUN. This define
 * allows the coldplug-clients to not exhaust all the coldplug-partition
 * structure to one huge lun with lots of partition.
 *
 * While parsing the partition-table in an LUN, only this many partitions
 * in the LUN will be parsed.  If a LUN exceeds this many partitions then
 * the exceeding partitions will not be parsed.
*****************************************************************************/
#ifndef COLDPLUG_MAX_PARTITIONS_PER_LUN
  #define COLDPLUG_MAX_PARTITIONS_PER_LUN (COLDPLUG_MAX_PARTITIONS)
#endif

/*************************************************************************//**
 * ::COLDPLUG_MAX_ITERS
 * The maximum number of iterator handles that will be in use simultaneously.
 *
 * Each coldplug_iter_open() API will return an coldplug_iter_handle and this
 * define can be configured to account for the number of coldplug_iter_handle
 * that the coldplug-clients will keep it open simultaneously.
*****************************************************************************/
#ifndef COLDPLUG_MAX_ITERS
  #define COLDPLUG_MAX_ITERS (2)
#endif

#endif /* __COLDPLUG_CONFIG_H__ */
