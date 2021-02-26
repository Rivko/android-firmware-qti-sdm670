#ifndef NPA_DDR_H
#define NPA_DDR_H

#define NPA_DDR_H_MAJOR  01
#define NPA_DDR_H_MINOR  02

/**
 @file npa_ddr.h
 @brief
 This file contains declarations for use by the NPA memory node for
 apps-dedicated memory.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF. 

      The ddr_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      ddr_mainpage.dox file or contact Tech Pubs.

	  The above description for this file is part of the xxx group
      description in the ddr_mainpage.dox file.
=============================================================================*/
/*=============================================================================
  Copyright (c) 2009 - 2011 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/02/11   SD      (Tech Pubs) Edited/added Doxygen markup and comments.
11/17/10   dxiang  Doxygen grouping changes for API auto-generation
10/26/10   dxiang  Doxygen markup standards changes 
04/16/09   SL      Initial version.

==============================================================================*/

/** @addtogroup ddr_apis
@{ */

/**
  Node power architecture memory node states.
 
  @note1hang When two or more requests are active at the same time, the request with the 
  higher priority is honored. Status priority from highest to lowest is:\n
  - Idle Retention \n
  - Active \n
  - Retention \n
  - Off
 */
typedef enum
{
  MEM_STATE_OFF = 0,         /**< Turns off the application DMM memory. */
  MEM_STATE_RETENTION,       /**< Self-refreshes the application DMM memory. */
  MEM_STATE_ACTIVE,          /**< Turns on the application DMM memory. */
  MEM_STATE_IDLE_RETENTION,  /**< Forces the application DMM memory into 
                                  retention, even when a request for the Active 
                                  state has also been received. */
  MEM_MAX_STATE = 0x7FFFFFFF /**< Forces the enumerator to 32 bits. */
} npa_ddr_node_state_type;


/*=============================================================================
    Function : npa_ddr_node_init
=============================================================================*/

/**
 Defines the node. This is the initialization function for this resource. 
 
 @return
 None.

 @dependencies
 None.
 */
void npa_ddr_node_init( void );

/** @} */ /* end_addtogroup ddr_apis */

#endif /* NPA_DDR_H */
