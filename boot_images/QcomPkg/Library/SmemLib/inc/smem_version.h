/*===========================================================================

                  S H A R E D   M E M O R Y   V E R S I O N

DESCRIPTION
    Defines the enums and functions used for file version checking between 
    processors.

DEPENDENCIES
    smem_version_set requires that smem.h be included before this file.

 Copyright (c)  2008-2012,2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/inc/smem_version.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/16   rv      SMEM Version 0x000C0000 changes.
08/11/16   vr      Added SMEM_VERSION_INFO_OFFSET macro for CDSP processor
10/25/12   bt      Add version info support for TZ image.
09/14/12   bt      Move SMEM_VERSION_ID definition to smem_type.h.
01/20/12   bm      Added version info support for RPM
09/21/11   bt      Updated SMEM minor version for reserved smem_mem_types.
05/03/11   bt      Add new bootloader define for 8960.
04/22/11   bt      Use new SMEM PROC defines, remove usage of IMAGE_QDSP6_PROC.
04/07/11   tl      Added UEFI bootloader
02/04/11   tl      Added Q6FW for 8960.
08/03/10   tl      Added wconnect processor for 8960.
04/30/10   rs      Added DSPS processor.
10/27/09   tl      Added support for 9600: Use SMEM_*_PROC macros
09/25/08   hwu     Updated the SMEM version. 
06/30/08   jlk     proc comm version updated to for pcil interface changes
04/29/08   ih      Added WM7 support
04/23/08   bfc     Increased size of the allocation table.
02/01/08   hwu     Added support for new boot architecture.
01/24/08   jlk     proc comm version up for protocol changes for status field
                   monitoring
01/16/08   jlk     initial version
=============================================================================*/
#ifndef _SMEM_VERSION_H_
#define _SMEM_VERSION_H_

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "smem_os.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/
/******************************************************************************
  SMEM_VERSION_INFO_SIZE defines the size of a version array.  
  SMEM_VERSION_INFO_OFFSET is the index into a version array for this processor
   or special code.

  DO NOT CHANGE THE FOLLOWING VALUES AS IT MAY BREAK VERSION CHECKING, ONLY 
  ADD TO THE END OF THE LIST!!

******************************************************************************/
#define SMEM_VERSION_INFO_SIZE        32

/* Define which entry of smem_version_info to use.
 * The order the features are checked is important.
 */
#if defined(MPROC_TZ_PROC) || defined(MPROC_HYP_PROC)
  #define SMEM_VERSION_INFO_OFFSET        0
#elif defined(FEATURE_WINCE_BOOTLOADER)
  #define SMEM_VERSION_INFO_OFFSET        1
#elif defined(FEATURE_WINCE)
  #define SMEM_VERSION_INFO_OFFSET        2
#elif defined(FEATURE_QUARTZ_ENV)
  #define SMEM_VERSION_INFO_OFFSET        3
#elif defined(MPROC_ADSP_PROC) || defined(SMEM_LPASS_PROC)
  #define SMEM_VERSION_INFO_OFFSET        4
#elif defined(FEATURE_COSIM)
  #define SMEM_VERSION_INFO_OFFSET        5
#elif defined(APPSBL) || defined(SBL_APPS_BOOT_LOADER_LIB)
  #define SMEM_VERSION_INFO_OFFSET        6
#elif defined(QCSBL) || defined(OEMSBL) || defined(DBL) || defined(FSBL) || defined(OSBL)
  #define SMEM_VERSION_INFO_OFFSET        7
#elif defined(MPROC_BOOT_PROC)
  #define SMEM_VERSION_INFO_OFFSET        7
#elif defined(MPROC_APPS_PROC)
  #define SMEM_VERSION_INFO_OFFSET        8
#elif defined(MPROC_MODEM_PROC)
  #define SMEM_VERSION_INFO_OFFSET        9
#elif defined(MPROC_SLPI_PROC)
  #define SMEM_VERSION_INFO_OFFSET        10
#elif defined(MPROC_WCN_PROC)
  #define SMEM_VERSION_INFO_OFFSET        11
#elif defined(FEATURE_MODEM_FIRMWARE)
  #define SMEM_VERSION_INFO_OFFSET        12
#elif defined(FEATURE_SMEM_UEFI)
  #define SMEM_VERSION_INFO_OFFSET        13
#elif defined(MPROC_RPM_PROC)
  #define SMEM_VERSION_INFO_OFFSET        14
#elif defined(MPROC_CDSP_PROC)
  #define SMEM_VERSION_INFO_OFFSET        15
#else
  #error "Must define SMEM_VERSION_INFO_OFFSET uniquely for each copy of SMEM"
#endif

#define SMEM_VERSION_BOOT_OFFSET          7
#define SMEM_LEGACY_VERSION_ID            0x000B0000

#endif /*_SMEM_VERSION_H_*/
