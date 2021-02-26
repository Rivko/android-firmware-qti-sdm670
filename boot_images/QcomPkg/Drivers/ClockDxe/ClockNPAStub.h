#ifndef __CLOCKNPASTUB_H__
#define __CLOCKNPASTUB_H__

/*=========================================================================

                    C L O C K   D E V I C E   D R I V E R
               
                   N P A   S T U B   D E F I N I T I O N S

GENERAL DESCRIPTION
  This file contains stubs for NPA functions and variable types.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

      Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
01/14/11   vs      Initial release.

==========================================================================*/ 

/*=========================================================================
      Includes
==========================================================================*/


/*=========================================================================
      Constants & Macros & Typedefs
==========================================================================*/

typedef uint32 * npa_client_handle;

#define NPA_CLIENT_REQUIRED 0

#define npa_issue_scalar_request( param1, param2 ) {}
#define npa_create_sync_client( param1, param2, param3 ) NULL
#define npa_resource_available_cb( param1, param2, param3 ) {}

#endif  /* __CLOCKNPASTUB_H__ */ 

