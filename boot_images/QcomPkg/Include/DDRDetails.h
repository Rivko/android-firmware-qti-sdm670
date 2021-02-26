/** 
  @file  DDRDetails.h
  @brief DDR device details shared structure interface.
*/
/*=============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who       what, where, why
 --------   ---       -----------------------------------------------------------
 05/26/17   daisond   Initial Revision
=============================================================================*/

#ifndef DDRDETAILS_H
#define DDRDETAILS_H

#include <api/boot/ddr_common.h>

#define DDR_DETAILS_STRUCT_VERSION 0x0000000000010000

struct ddr_part_details
{

  UINT8 revision_id1[2];
  UINT8 revision_id2[2];
  UINT8 width[2];
  UINT8 density[2];
  
};

typedef struct ddr_details_entry_info 
{

  UINT8 manufacturer_id;
  UINT8 device_type;  
  struct ddr_part_details ddr_params[DDR_MAX_NUM_CH];
  UINT8 reserved[66];
  
}ddr_details_entry;


#endif /* DDRDETAILS_H */


