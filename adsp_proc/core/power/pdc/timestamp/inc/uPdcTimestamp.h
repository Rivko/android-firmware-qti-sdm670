#ifndef _UPDCTIMESTAMP_H_
#define _UPDCTIMESTAMP_H_
/*============================================================================
  FILE:         uPdcTimestamp.h
  
  OVERVIEW:     This file provides the types for the PDC timestamp driver while in
                island mode
 
  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/timestamp/inc/uPdcTimestamp.h#1 $
$DateTime: 2017/11/05 22:36:52 $
============================================================================*/
#include "comdef.h"
#include "pdcTimestamp.h"

/*===========================================================================
 *                         GLOBAL FUNCTION DEFINITIONS
 *===========================================================================*/
/**
 * uPdcTimestamp_getUnitData
 *
 * @brief Gets the desired timestamp unit data which will be populated after the interrupt
 *        is asserted while in island mode
 * 
 * @note Once a unit is read, it's status bits will be reset, however the actual timestamp
 *       value should be readable multiple times.
 * 
 * @param unitData:   Data structure pointer where the requested unit data will be populated
 * @param intVector:  Interrupt vector for desired timestamp information
 */
pdcTimestamp_status uPdcTimestamp_getUnitData(pdcTimestamp_unit *unitData, uint32 intVector);

#endif /* _UPDCTIMESTAMP_H_ */
