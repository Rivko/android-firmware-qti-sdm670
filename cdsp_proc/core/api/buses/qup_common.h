#ifndef QUP_COMMON_H
#define QUP_COMMON_H
/*==================================================================================================

FILE: qup_common.c

DESCRIPTION: This module provides the common features for the QUPv3 protocols ( I2C/I3C/SPI/UART )

Copyright (c) 2017 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   qup_common_pram_tre_malloc
   qup_common_pram_tre_free

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/api/buses/qup_common.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/16/17   UNR     Added qup version API
06/01/17   VV      Initial version
==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "com_dtypes.h"

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/
typedef enum
{
   QUP_V1_0 = 0x10000000,
   QUP_V1_1 = 0x10010000,
   QUP_V1_2 = 0x10020000,
}QUP_VERSION;

typedef enum
{
   SE_PROTOCOL_SPI  = 1,
   SE_PROTOCOL_UART = 2,
   SE_PROTOCOL_I2C  = 3,
   SE_PROTOCOL_I3C  = 4,
   SE_PROTOCOL_MAX  = 5
}SE_PROTOCOL;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/


/*!
 * \brief Allocates memory that is aligned to the size from the buses segment of PRAM
 *
 * \param in SE_PROTOCOL protocol
 * \param in uint32 size
 * \return allocated memory in case of success and NULL in case of failure
 */

void* qup_common_pram_tre_malloc(SE_PROTOCOL protocol, uint32 size);

/*!
 * \brief Allocates memory that is aligned to the size from the buses segment of PRAM
 *
 * \param in void* addr - memory to be freed
 */

void qup_common_pram_tre_free(void* addr);

/*!
 * \brief Determines QUP core version
 *
 * \return QUP_VERSION
 */

uint32 qup_common_get_hw_version (void);

#endif
