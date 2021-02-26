#ifndef QSEE_CLK_H
#define QSEE_CLK_H

/**
@file qsee_clk.h
@brief Provide clock interface
*/

/*===========================================================================
   Copyright (c) 2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
10/02/14   yk      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*
 * Level of clock frequence
*/

typedef enum
{
  INACTIVE      = 0x0,
  LOW           = 0x1,
  MEDIUM        = 0x2,
  HIGH          = 0x3,
  COUNT_MAX     = 0x4,
  MAX_ENTRY     = 0x7FFFFFFF
}QSEE_CLKLEV;

/*
 * Voteable clock resource, will 
 * ensure to vote for all dependencies 
*/

typedef enum
{
  CE1           = 0x1,
  CE2           = 0x2,
  CE3           = 0x3,
  COUNT         = 0x4,       
  MAX           = 0x7FFFFFFF
}QSEE_RESOURCE;


/**
 * @brief     set crypto/bimc/snoc bandwidth
 * @return    E_SUCCESS if success,-E_FAILURE otherwise
 */
uint32_t qsee_set_bandwidth(void *reqClient, uint32_t reqClientlen,uint32_t res_req, uint32_t level, uint32_t flags);

#endif /*QSEE_CLK_H*/

