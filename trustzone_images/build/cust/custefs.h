#ifndef CUSTEFS_H
#define CUSTEFS_H
/*===========================================================================
 * custefs.h
 *
 * DESCRIPTION
 * Configuration for EFS Feature.
 *
 * Copyright (c) 2009  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                    EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tz/2.0.2/cust/custefs.h#1 $
$DateTime: 2018/02/06 03:02:41 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/07/10   rp      Created
===========================================================================*/

#ifndef BUILD_BOOT_CHAIN

  #define FEATURE_FS_BENCHMARK
  #undef FEATURE_FS_UNSAFE_BENCHMARK

  /* Enable Remote Storage on Modem for eMMC Boot build variant */
  #if defined(IMAGE_MODEM_PROC) && defined(FEATURE_SDCC_BOOT)
    #define FEATURE_EFS_EFS2_ON_RMTS
  #endif


  /* Defines DIAG access to EFS on both APPS and modem.
   * Proc ID 0 : Modem => Subsys ID : 19 (ie Primary DIAG_SUBSYS_FS)
   * Proc ID 1 : APPS  => Subsys ID : 62 (ie Alternate DIAG_SUBSYS_FS_ALTERNATE) */
  #ifdef IMAGE_APPS_PROC
    #define EFS_DIAG_USE_THIS_SUBSYS_ID DIAG_SUBSYS_FS_ALTERNATE
    #define EFS_DIAG_SUBSYS_PROCESSOR_ID    1
  #else
    #define EFS_DIAG_USE_THIS_SUBSYS_ID DIAG_SUBSYS_FS
    #define EFS_DIAG_SUBSYS_PROCESSOR_ID  0
  #endif


#define HFAT_WR_DATACACHE_SIZE                128

#ifdef FEATURE_EFS_HOTPLUG
  #define HOTPLUG_HAVE_PARTITION_SUPPORT
#endif


#else  /* BUILD_BOOT_CHAIN */

  /* Define flags for boot */

  /* Tune for RAM usage */
  #define FS_HFAT_MAXVOLUME_COUNT   2
  #define FS_HFAT_MAX_OPEN_FILES    8
  #define FS_VNODE_POOL_SIZE        16

  #define FS_MAX_ITERATORS          2
  #define FS_MAX_EXTFS_MOUNTS       3

  #define FS_PATH_MAX               300
  #define FS_NAME_MAX               100
  #define HFAT_MAX_PATH_UTF16       90
  #define HFAT_MAX_PATH_UTF16_ALLOW 89

#endif /* BUILD_BOOT_CHAIN */

#endif /* CUSTEFS_H */

