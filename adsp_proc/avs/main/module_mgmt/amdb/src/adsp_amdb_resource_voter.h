/*========================================================================
ADSP AMDB Resource Voter

This file describes the interface for the resource voter for AMDB. This utility
allows threads to vote for dlopen resources.

Copyright (c) 2016 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Technologies Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/19/15   DG      Created file.

========================================================================== */

#ifndef ADSP_AMDB_RESOURCE_VOTER_H
#define ADSP_AMDB_RESOURCE_VOTER_H


/*----------------------------------------------------------------------------
 * Include files
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

struct amdb_voter_t;

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/

amdb_voter_t *amdb_voter_create(void);
void amdb_voter_vote(amdb_voter_t *obj_ptr);
void amdb_voter_release(amdb_voter_t *obj_ptr);
void amdb_voter_destroy(amdb_voter_t *obj_ptr);

#endif // #ifndef ADSP_AMDB_RESOURCE_VOTER_H
