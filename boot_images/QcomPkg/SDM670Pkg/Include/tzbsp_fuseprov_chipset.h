/**
@file tzbsp_fuseprov_chipset.h
@brief Trustzone Fuse Provisioning chipset specific definitions/routines

This file contains the target specific information for Trustzone Fuse Provisioning

Copyright 2010,2015 by Qualcomm Technologies, Inc.  All Rights Reserved

*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Include/tzbsp_fuseprov_chipset.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2015/29/01     kpa         Update sec dat buffer offset
 2013/12/10     sm          Initial version
=============================================================================*/

#ifndef TZBSP_FUSEPROV_CHIPSET_H
#define TZBSP_FUSEPROV_CHIPSET_H

/* This are has been carved out from TZ DDR shared region */
#define TZBSP_SECDAT_BUFFER_OFFSET                (0x5000)                   // the offset of sec.dat ddr memory
#define TZ_RAM_DUMP_SIZE                          (0xC0000)  

#define TZBSP_SECDAT_BUFFER_BASE (SCL_TZ_STAT_BASE + TZ_RAM_DUMP_SIZE + TZBSP_SECDAT_BUFFER_OFFSET)
#define TZBSP_SECDAT_BUFFER_SIZE                  4096     // the size of the shared region

#endif
