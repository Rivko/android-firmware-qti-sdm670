#ifndef COREINIT_H
#define COREINIT_H
/**
@file coreinit.h

@brief Core default resource voting driver.

===========================================================================
                             Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/coreinit.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2017/01/06   sds     Initial revision

===========================================================================
           Copyright 2017 QUALCOMM Technologies, Incorporated.
                       All Rights Reserved.
                     QUALCOMM Proprietary/GTDR
===========================================================================
*/

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/* ============================================================================
**  Function : coreinit_init
** ============================================================================
*/
/*!
    @brief
    Initializes and issues any core system default resource votes.
*/
void coreinit_init (void);

#endif /* COREINIT_H */

