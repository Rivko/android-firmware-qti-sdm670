/*===========================================================================
                           gs_dynamic_lib_proxy.h

DESCRIPTION: Provide a dynamic library proxy for the mm-gestures lib.

INITIALIZATION AND SEQUENCING REQUIREMENTS: None

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifndef __GS_DYNAMIC_LIB_PROXY__
#define __GS_DYNAMIC_LIB_PROXY__

/*----------------------------------------------------------------------------
Include files
----------------------------------------------------------------------------*/
#include "usf_dynamic_lib_proxy.h"
#include "gs_bus_client.h"

/*-----------------------------------------------------------------------------
  Classes
-----------------------------------------------------------------------------*/

class GsDynamicLibProxy: public UsfDynamicLibProxy
{
private:

  /*============================================================================
    Function pointer typedefs
  ============================================================================*/

  typedef GsBusClient* (*createGsBusClient_fp_t)();

  typedef void (*deleteGsBusClient_fp_t)(GsBusClient* gsBusClient);

  /*============================================================================
    Function pointers
  ============================================================================*/

  createGsBusClient_fp_t    m_createGsBusClient;

  deleteGsBusClient_fp_t    m_deleteGsBusClient;

protected:
  /*============================================================================
    FUNCTION:  load_all_methods
  ============================================================================*/
  /**
   * Loads all the methods from the library.
   *
   * @return bool - true success
   *                false failure
   */
  bool load_all_methods();

public:

  /*============================================================================
    Library method wrapper functions
  ============================================================================*/

  /*============================================================================
    FUNCTION:  createGsBusClient
  ============================================================================*/
  /**
   * Creates a new object of GsBusClient.
   *
   * @return new GsBusClient object
   */
  GsBusClient* createGsBusClient();

  /*============================================================================
    FUNCTION:  deleteGsBusClient
  ============================================================================*/
  /**
   * Deletes a GsBusClient object.
   *
   */
  void deleteGsBusClient(GsBusClient* pGsBusClient);

};

#endif //__GS_DYNAMIC_LIB_PROXY__
