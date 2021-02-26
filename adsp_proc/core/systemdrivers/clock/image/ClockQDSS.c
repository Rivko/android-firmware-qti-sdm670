/*
===========================================================================
*/
/**
  @file ClockQDSS.c

*/
/*
  ====================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/ClockQDSS.c#1 $
  $DateTime: 2017/09/01 02:44:54 $
  $Author: pwbldsvc $
  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockImage.h"
#include "npa_resource.h"
#include "npa_scheduler.h"
#include "qmp_msg_client.h"


/*=========================================================================
      Macros
==========================================================================*/

#define CLOCK_QDSS_BUFFER_SIZE 64


/*=========================================================================
      Prototypes
==========================================================================*/

static npa_resource_state Clock_NPAQDSSNodeDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
);


/*=========================================================================
      Data
==========================================================================*/

/*
 * Clock QDSS NPA resources.
 */
static npa_resource_definition Clock_aNPAQDSSResources[] =
{
  {
    .name       = "/clk/qdss",
    .units      = "State",
    .plugin     = &npa_max_plugin,
    .attributes = NPA_RESOURCE_DEFAULT,
    .max        = NPA_MAX_STATE,
    .data       = "qdss",
  },
  {
    .name       = "/clk/qdss_ao",
    .units      = "State",
    .plugin     = &npa_max_plugin,
    .attributes = NPA_RESOURCE_DEFAULT,
    .max        = NPA_MAX_STATE,
    .data       = "qdss_ao",
  },
};

/*
 * Clock QDSS NPA node.
 */
static npa_node_definition Clock_NPAQDSSNode =
{
  .name           = "/node/clk/qdss",
  .driver_fcn     = Clock_NPAQDSSNodeDriverFunc,
  .attributes     = NPA_NODE_DEFAULT,
  .resource_count = ARR_SIZE(Clock_aNPAQDSSResources),
  .resources      = Clock_aNPAQDSSResources,
};


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_NPAQDSSNodeDriverFunc
** =========================================================================*/
/**
  See ClockImage.h
*/
DALResult Clock_InitQDSS (ClockDrvCtxt *pDrvCtxt)
{
  npa_define_node_cb(&Clock_NPAQDSSNode, NULL, NULL, NULL);
  return DAL_SUCCESS;

} /* END Clock_InitQDSS */


/* =========================================================================
**  Function : Clock_NPAQDSSNodeDriverFunc
** =========================================================================*/
/**
  QDSS NPA node driver function.

  @param *pResource [in] -- Pointer to resource whose state changed.
  @param hClient    [in] -- Client handle that triggered the change.
  @param nState     [in] -- New request state.

  @return
  npa_resource_state -- The new active state of the resource.

  @dependencies
  None.
*/

static npa_resource_state Clock_NPAQDSSNodeDriverFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  const char      *szResource = pResource->definition->data;
  char             szRequest[CLOCK_QDSS_BUFFER_SIZE];
  int              nChar;
  glink_err_type   eGLinkErr;

  if (hClient->type == NPA_CLIENT_INITIALIZE)
  {
    return 0;
  }

  nChar = snprintf(szRequest, CLOCK_QDSS_BUFFER_SIZE,
    "{class: clock, res: %s, val: %d}", szResource, nState);

  if (nChar < 0 || nChar >= CLOCK_QDSS_BUFFER_SIZE)
  {
    DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "QDSS request buffer too small. Size is %d. Tried to write %d.",
      CLOCK_QDSS_BUFFER_SIZE, nChar + 1);
    return pResource->active_state;
  }

  eGLinkErr = send2aop(szRequest, CLOCK_QDSS_BUFFER_SIZE);
  if (eGLinkErr != GLINK_STATUS_SUCCESS)
  {
    DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to send QDSS request to AOP: 0x%x.", eGLinkErr);
    return pResource->active_state;
  }

  return nState;

} /* END Clock_NPAQDSSNodeDriverFunc */

