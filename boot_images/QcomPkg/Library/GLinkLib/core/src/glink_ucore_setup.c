/*===========================================================================

GLINK uCore internal setup Source File


Copyright (c) 2014 by QUALCOMM Technologies, Incorporated.  All Rights
Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/core/src/glink_ucore_setup.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/12/14   an      Core refactoring for limited memory environment support
===========================================================================*/

/*===========================================================================
INCLUDE FILES
===========================================================================*/

#include "glink.h"
#include "glink_internal.h"

glink_core_if_type* glink_core_get_intentless_interface(void);
glink_core_if_type* glink_core_get_intent_interface(void);

/*===========================================================================
EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      glink_core_get_default_interface
===========================================================================*/
/**

  Provides default core interface.

  @return     Pointer to the default core interface.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_core_if_type* glink_core_get_default_interface(void)
{
  return glink_core_get_intentless_interface();
}

/*===========================================================================
FUNCTION      glink_core_setup
===========================================================================*/
/**

  Initializes internal core functions based on the transport capabilities.

  @param[in]  if_ptr   The Pointer to the interface instance.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_core_setup(glink_transport_if_type *if_ptr)
{
  if (if_ptr->glink_core_priv->xport_capabilities & GLINK_CAPABILITY_INTENTLESS)
  {
    glink_core_setup_intentless_xport(if_ptr);
  }
  else
  {
    glink_core_setup_intent_xport(if_ptr);
  }
}
