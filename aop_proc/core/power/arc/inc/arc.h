#ifndef __ARC_H__
#define __ARC_H__
/*===========================================================================
                                  arc.h

DESCRIPTION:
  This file defines public interface for ARC driver.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                       Includes and Type Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------
#include "comdef.h"

//---------------------------------------------------------------------------
//  Type Definitions
//---------------------------------------------------------------------------

/* Enum for various types of ARC operational level */
typedef enum
{
  ARC_OL_CURRENT = 0,
  ARC_OL_SOLVED,
  ARC_OL_AGGREGATED,
  ARC_OL_SEQUENCE,
  ARC_OL_DESTINATION,
  ARC_OL_MAX_TYPE = ARC_OL_DESTINATION
} arc_ol_t;

//===========================================================================
//                       Function Declarations
//===========================================================================

/**
 * <!-- arc_init -->
 *
 * @brief Initializes various modules of ARC HW block like resource managers,
 *        sequences, solver, etc.
 */
void arc_init (void);

/**
 * <!-- arc_rm_id_by_vote_addr -->
 *
 * @brief This function returns the ARC resource/rm id that corresponds
 *        to input resource address.
 *
 * @param vote_addr: PCB vote address from client.
 * @param use_slave_id: Flag indicating if slave id should be used while
 *                      comparing addresses (TRUE - yes, FALSE - no).
 *
 * @return ARC resource index (>=0) if successful, else <0.
 */
int8 arc_rm_id_by_vote_addr (uint32 vote_addr, boolean use_slave_id);

/**
 * <!-- arc_rm_op_level -->
 *
 * @brief Returns the operational level (OL) of requested type for a given
 *        resource.
 *
 * @param rm_id: ID of the resource whose OL is being queried.
 * @param ol: Operational Level type.
 *
 * @return OL (>=0) corresponding type if successful else <0.
 */
int8 arc_rm_op_level (int8 rm_id, arc_ol_t ol);

#endif // __ARC_H__
