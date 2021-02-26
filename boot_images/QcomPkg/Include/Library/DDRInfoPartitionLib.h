/** @file DDRInfoPartitionLib.h
  
  DDR Info interface

  Copyright (c) 2017 by Qualcomm Technologies, Inc. 
  All Rights Reserved.
 
**/

/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    ---------------------------------------------------
04/27/17   ddk      Initital Version
========================================================================*/

#include <Uefi.h>
#include <MemRegionInfo.h>
#include <DDRDetails.h>


/**
  Get DDR details from DDR info stored in shared memory
**/

EFI_STATUS
GetDDRDetails (IN OUT struct ddr_details_entry_info *ddr_detail);

