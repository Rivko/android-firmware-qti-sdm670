/**************************************************************************************

      N M _ L I N K _ L O C A L _ R O U T E _ P R O V I D E R _ F A C T O R Y . H

***************************************************************************************/

/**************************************************************************************

  @file    nm_link_local_route_provider_factory.h
  @brief   Link-local routing plugin

  DESCRIPTION
  Manage the routing rules required for link-local data calls

**************************************************************************************/
/*====================================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

======================================================================================*/

#ifndef __ROUTE_FACTORY_H__
#define __ROUTE_FACTORY_H__
#endif /* __ROUTE_FACTORY_H__ */

typedef struct route_provider_s
{
  int (*init)         (boolean);
  int (*add_route)    (int, char*);
  int (*remove_route) (int, char*);
} nm_ll_route_provider_t;

/*====================================================================================
  FUNCTION  nm_link_local_get_route_provider
====================================================================================*/
/*!
@brief
  Returns the route provider instance based on the platform

@return
  struct route_provider instance

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*==================================================================================*/
nm_ll_route_provider_t *nm_link_local_get_route_provider(void);
