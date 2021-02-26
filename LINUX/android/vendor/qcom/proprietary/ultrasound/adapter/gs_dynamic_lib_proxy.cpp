/*===========================================================================
                           gs_dynamic_lib_proxy.cpp

DESCRIPTION: Implement a dynamic library proxy for the gesture framework lib.

INITIALIZATION AND SEQUENCING REQUIREMENTS: None

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*----------------------------------------------------------------------------
Include files
----------------------------------------------------------------------------*/
#include "usf_log.h"
#include "gs_dynamic_lib_proxy.h"

/*------------------------------------------------------------------------------
  Function implementation
------------------------------------------------------------------------------*/

/*============================================================================
  FUNCTION:  load_all_methods
============================================================================*/
/**
 * See function definition in header file
 */
bool GsDynamicLibProxy::load_all_methods()
{
  GET_METHOD(CreateGsBusClient, createGsBusClient, false);
  GET_METHOD(DeleteGsBusClient, deleteGsBusClient, false);

  return true;
}

/*============================================================================
  Library method wrapper functions
============================================================================*/

GsBusClient* GsDynamicLibProxy::createGsBusClient()
{
  return m_createGsBusClient();
}

void GsDynamicLibProxy::deleteGsBusClient(GsBusClient* pGsBusClient)
{
  m_deleteGsBusClient(pGsBusClient);
}

