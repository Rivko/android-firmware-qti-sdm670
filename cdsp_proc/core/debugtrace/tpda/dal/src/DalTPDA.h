#ifndef _DAL_TPDA_H_
#define _DAL_TPDA_H_

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
// $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tpda/dal/src/DalTPDA.h#1 $
//
// when       who     what, where, why
// --------   ---     ----------------------------------------------------
// 08/19/15   ps      Initial revision.
//

#include "DALStdDef.h"

typedef struct
{
   uint32 length;
}TableLength;

typedef struct
{
   const char   *tpdm_name;
   const uint32 tpda_port_num;
}TPDAPortSpec;

typedef struct
{
   uint32 phys_addr;
   const char *tpda_name;
   uint32 atid;
   const uint32 port_occupied_mask;
   const char *tpda_funnel_name;
   TPDAPortSpec *port_list;
   const uint32 port_list_size;
}TPDADeviceProp;

#define BIT_MASK(n) 1<<(n)

#define	PORT0	   0
#define	PORT1	   1
#define	PORT2	   2
#define	PORT3	   3
#define	PORT4	   4
#define	PORT5	   5
#define	PORT6	   6
#define	PORT7	   7
#define	PORT8	   8
#define	PORT9	   9
#define	PORT10	10
#define	PORT11	11
#define	PORT12	12
#define	PORT13	13
#define	PORT14	14
#define	PORT15	15
#define	PORT16	16
#define	PORT17	17
#define	PORT18	18
#define	PORT19	19
#define	PORT20	20
#define	PORT21	21
#define	PORT22	22
#define	PORT23	23
#define	PORT24	24
#define	PORT25	25
#define	PORT26	26
#define	PORT27	27
#define	PORT28	28
#define	PORT29	29
#define	PORT30	30
#define	PORT31	31


#endif /*_DAL_TPDA_H_*/
