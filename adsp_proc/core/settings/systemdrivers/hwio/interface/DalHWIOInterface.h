#ifndef DALHWIOINTERFACE_H
#define DALHWIOINTERFACE_H
/*==============================================================================

FILE:      DalHWIOSettings.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2008-2017 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALFramework.h"

/*
 * HWIOModuleType
 *
 * Type definition for the module elements in the HWIO memory map structure.
 * Each entry defines a 'MODULE' as taken from the ARM_ADDRESS_FILE.  Each
 * physical entry contains an array of such modules.
 */
typedef struct
{
  char           *szName;
  uint32          nOffset;
  uint32          dwSizeInBytes;
} HWIOModuleType;

/*
 * HWIOPhysRegionType
 *
 * Type definition for the elements in the HWIO memory map structure
 * declared in msmhwiomap.h.
 */
typedef struct
{
  char           *szName;
  DALSYSMemAddr   pPhysAddr;
  uint32          dwSizeInBytes;
  DALSYSMemAddr   pVirtAddr;
  HWIOModuleType *pModules;
} HWIOPhysRegionType;

#endif /* DALHWIOINTERFACE_H */


