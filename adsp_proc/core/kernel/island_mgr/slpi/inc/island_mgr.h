#ifndef ISLAND_MGR_H
#define ISLAND_MGR_H

/**
  @file island_mgr.h 
  @brief  Prototypes of Island Manager API The APIs allow
    	  entering and exiting island mode where the memory
    	  accesses are limited to local memory.

  EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2014  by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/

#include <qurt.h>

/*=====================================================================
Constants and macros
======================================================================*/
#define ISLAND_MGR_EOK                             0  /**< Operation successfully performed. */ 
#define ISLAND_MGR_EFAILED                        12  /**< Operation failed. */


/**@ingroup func_island_mgr_island_init
  Initialize Island Manager.\n
  This function prepares the data structures and performs the
  operations required to initialize the Island Manager.
 
  @return
  ISLAND_MGR_EOK -- Successfully initialized. \n
  ISLAND_MGR_EFAILURE -- Failed to initilize.

  @dependencies
  None.
 */
int island_mgr_init (void);

/**@ingroup func_island_mgr_island_enter
  Enter island mode.\n
  This function causes the system to enter island mode.
  The system should be in single threaded Mode to be able to
  enter island mode.
 
  @return
  ISLAND_MGR_EOK -- Successfully entered island mode. \n
  ISLAND_MGR_EFAILURE -- Failed to enter island mode.

  @dependencies
  None.
 */
int island_mgr_island_enter (void);

/**@ingroup island_mgr_island_exit
  Exit island mode.\n
  This function brings the system out of island mode. The system should be in
  island mode to be able to bring the system out of island mode. The system
  should also be in Single Threaded Mode to be able to exit island mode.
 
  @return
  ISLAND_MGR_EOK -- Operation was successfully performed. \n
  ISLAND_MGR_EFAILED -- Operation failed.

  @dependencies
  None.
 */
int island_mgr_island_exit (unsigned short);

#endif /* ISLAND_MGR_H */
