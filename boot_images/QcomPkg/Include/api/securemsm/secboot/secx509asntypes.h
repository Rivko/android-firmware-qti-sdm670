#ifndef SECX509TYPES_H
#define SECX509TYPES_H

/** @file secx509asntypes.h
 * @brief
 * This file contains the definitions of the constants, data structures and
 * and interfaces that provides ASN types for SECX509 standard.
 * 
 */


/*===========================================================================
Copyright (c) 2000-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/10   msh     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
10/31/06   df      CR100945 Increase Max rec len to 64
05/30/01   om      Fixed pointer names
03/02/00   om      Several fixes (first functional version)
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

              DEFINITIONS AND CONSTANTS FROM ASN.1

===========================================================================*/

/**
  SECSSL ASN1 Constants and definitions
*/
#define SECSSLASN_MAX_REC_LEN       64

/* <EJECT> */ 
/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/**
 ASN.1 Integer Type
*/
typedef struct
{
  uint32   length;                 /**< -- Length of the integer data  */
  uint8*   data_ptr;               /**< -- Integer data                */
} SECASN1_INTEGER;

/**
 ASN.1 String Type
*/
typedef struct
{
  uint32   length;                 /**< -- Length of the string data   */
  uint8*   data_ptr;               /**< -- String data                 */
} SECASN1_STRING;

/**
 ASN.1 Universal Time Coordinated (UTC) Type
*/
typedef struct
{
  uint32   length;                 /**< -- Length of the time data     */
  uint8*   data_ptr;               /**< -- Time data                   */
} SECASN1_UTCTIME;


#endif
