#ifndef DAL_TPDM_H
#define DAL_TPDM_H

/*=============================================================================
   Copyright (c) 2015 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   QUALCOMM Proprietary
=============================================================================*/

//========================================================================
//
//                   TPDM structures for DAL driver
//
// @file dditfunnel.ddi
//
//   This file contains the definitions of the constants, types, and
//   interfaces that comprise the interface to the QDSS TPDM
//   DAL driver.
//
//========================================================================
//
//                             Edit History
//
// $Header: //components/rel/core.qdsp6/2.1/debugtrace/tpdm/dal/src/DalTPDM.h#1 $
//
// when       who     what, where, why
// --------   ---     ----------------------------------------------------
// 08/19/15   ps      Initial revision.
//

#include "DALStdDef.h"


struct TPDMDevicePropCMB_Type
{
   uint32 cmb_width;
};

struct TPDMDevicePropDSB_Type
{
   uint32 dsb_element_size;
   uint32 dsb_width;
};

struct TPDMDevicePropGPR_Type
{
   uint32 num_gprs;
};

typedef struct
{
   struct TPDMDevicePropDSB_Type dsb_prop;
   struct TPDMDevicePropCMB_Type cmb_prop;
   struct TPDMDevicePropGPR_Type gpr_prop;
} TPDMDevicePropSpec;

typedef struct TPDMDeviceProp_Type
{
   uint32 phys_addr;
   const char *tpdm_name;
   const char *tpda_name;
   uint32 tpdm_types;
   TPDMDevicePropSpec const *pattrib_list;
}TPDMDeviceProp;

typedef struct
{
   uint32 length;
}TableLength;

#endif
