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

$Header: //components/rel/core.qdsp6/2.1/buses/qup_common/src/qup_stubs.c#2 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/01/17   VV      Initial version
==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "qup_common.h"

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
void qup_common_init(void)
{

}
void qup_common_pram_tre_free(void* mem_addr)
{
   (void)mem_addr;
}

void* qup_common_pram_tre_malloc(SE_PROTOCOL protocol, uint32 size)
{
   (void)protocol;
   (void)size;
   return NULL;
}

uint32 qup_common_get_hw_version ()
{
    return QUP_V1_1;
}
