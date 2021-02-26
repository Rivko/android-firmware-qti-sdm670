/*=============================================================================

FILE:         qdss_tfunnel.c

DESCRIPTION:

================================================================================
Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.
==============================================================================*/
#include "qdss_tfunnel.h"
#include "qdss_control_priv.h"
#include "qdss_control.h"


/*-------------------------------------------------------------------------*/

/**
  @brief Disable all trace funnel ports

  @return 0 if successful, error code otherwise
 */
int qdss_tfunnel_disable_all_ports(void)
{
   int nErr;

   if (0==qdss.hSync) {
      return QDSS_CONTROL_BAD_STATE;
   }

   DALSYS_SyncEnter(qdss.hSync);
   TRY(nErr,qdss_ensure_hw_ready());

   THROW_IF(nErr,DAL_SUCCESS !=
            DalTFunnel_DisableAll(qdss.hFunnel),
            QDSS_CONTROL_ERROR);

   CATCH(nErr){}
   DALSYS_SyncLeave(qdss.hSync);
   return nErr;
}


/*-------------------------------------------------------------------------*/

/**
  @brief  Enable or disable a a tfunnel port

  @param            port_name [in ] :  port name
  @param              bEnable [in ] :  enable or disable

  @return 0 if successful, error code otherwise
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_ERROR = Failed.
 */
static int qdss_tfunnel_toggle_port(const char *port_name,DALBOOL bEnable)
{
   int nErr;
   TFunnelControlParam control_param;

   if (0==qdss.hSync) {
      //TODO:
      //We were called before we were initialized
      //Should return failure here - but that messes up buspm
      //So silently returning success here for now
      return QDSS_CONTROL_SUCCESS;
   }

   DALSYS_SyncEnter(qdss.hSync);
   TRY(nErr,qdss_ensure_hw_ready());

   control_param.pPortStr = port_name;
   control_param.Val.bEnable = bEnable;
   THROW_IF(nErr,DAL_SUCCESS !=
            DalTFunnel_Control(qdss.hFunnel,TFUNNEL_PORT_ENABLE,&control_param),
            QDSS_CONTROL_ERROR);

   CATCH(nErr){}
   DALSYS_SyncLeave(qdss.hSync);
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Enable a trace funnel port

  @param            port_name [in ] :  port name

  @return 0 if successful, error code otherwise
 */
int qdss_tfunnel_enable_port(const char *port_name)
{
   return qdss_tfunnel_toggle_port(port_name,TRUE);
}

/*-------------------------------------------------------------------------*/

/**
  @brief     Disable a trace funnel port

  @param            port_name [in ] :  port name

  @return 0 if successful, error code otherwise
 */
int qdss_tfunnel_disable_port(const char *port_name)
{
   return qdss_tfunnel_toggle_port(port_name,FALSE);
}

/*-------------------------------------------------------------------------*/

/* Below are qdss control interface functions. Definitions of these
 * are in qdss_contro.h
 */

/*-------------------------------------------------------------------------*/

/* Return
      QDSS_CONTROL_SUCCESS = Successful.
      QDSS_CONTROL_BAD_STATE = A handle is not valid.
      QDSS_CONTROL_ERROR = Failed.
*/
int qdss_control_get_funnel(const char *name, int *state)
{
   int nErr = 0;
   TFunnelControlParam control_param;

   if (0==qdss.hSync) {
      return QDSS_CONTROL_BAD_STATE;
   }

   DALSYS_SyncEnter(qdss.hSync);
   TRY(nErr,qdss_ensure_hw_ready());

   control_param.pPortStr = name;
   THROW_IF(nErr,DAL_SUCCESS !=
            DalTFunnel_Control(qdss.hFunnel,TFUNNEL_PORT_STATUS,&control_param),
            QDSS_CONTROL_ERROR);

   *state =  (control_param.Val.bEnable) ? 1:0;

   CATCH(nErr){}
   DALSYS_SyncLeave(qdss.hSync);
   return nErr;
}

/*-------------------------------------------------------------------------*/

int qdss_control_set_funnel(const char *name, int state)
{
   return qdss_tfunnel_toggle_port(name,state);
}


