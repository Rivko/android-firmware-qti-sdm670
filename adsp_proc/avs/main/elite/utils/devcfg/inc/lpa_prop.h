/*========================================================================

*//** @file lpm_prop.h
This file contains structure definition for LPM hardware properties.

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: 

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/14/12   rkc     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "mmdefs.h"

#ifndef LPA_DEVCFG_H
#define LPA_DEVCFG_H

/** @brief LPM Properties Structure
*/

typedef struct  {

    uint32_t size_lpa_heap_from_ddr; 
    /**< Size allocated for LPA heap from DDR */
} lpa_prop_struct_t;

#endif //LPA_DEVCFG_H


