
/*==========================================================================
ELite Source File

This file implements the state changing functions for the Audio Post 
Processing Dynamic Service.

Copyright (c) 2010-2014, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/16/10   DG      Created file
=========================================================================== */

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "audproc_comdef.h"
#include "adsp_media_fmt.h"
 
/*---------------------------------------------------------------------------
 * Definitions and Constants
 * -------------------------------------------------------------------------*/
 
/*---------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
 
/*---------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
 
/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

/**
 * Moves the PP to sleep state. PP will not process any data in 
 * this state. Data processing will resume when PP is moved to 
 * active state. 
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 * 
 * @param[in, out] me 
 * Pointer to the current instance structure. 
 */
void AudPP_GotoSleepState(ThisAudDynaPPSvc_t *me)
{
    me->audPPStatus.isSleeping = TRUE;
   me->audPPStatus.unCurrentBitfield = me->audPPStatus.unCommandOnlyBitField;
}

/**
 * Moves the PP to active state. This causes the PP to resume 
 * data processing. 
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 * 
 * @param[in, out] me 
 * Pointer to the current instance structure. 
 */
void AudPP_GotoActiveState(ThisAudDynaPPSvc_t *me)
{
    if (AudPP_isSleeping(me))
    {
        // Switch to active state, and listen on data or buffer queue as appropriate
        me->audPPStatus.isSleeping = FALSE;
        uint32_t dataBitField = 0;
        for(uint32_t port_id=0;port_id < me->numActiveInPorts;port_id++)
        {
        	if(NULL == me->in_port[port_id]->pInDataMsg)
        		dataBitField |= me->audPPStatus.unDataBitfield[port_id];
        }

        if (dataBitField)
        {
            // Not working on any input buffer currently
            me->audPPStatus.unCurrentBitfield = dataBitField | me->audPPStatus.unCommandOnlyBitField;
        }
        else
        {
            // Working on an input buffer currently
            me->audPPStatus.unCurrentBitfield = me->audPPStatus.unBufBitfield;
        }
    }
}

/**
 * Indicates whether PP is in sleep state (not processing data).
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 * 
 * @param[in] me 
 * Pointer to the current instance structure. 
 * 
 * @return bool_t 
 * TRUE if PP is in sleep state. 
 * FALSE otherwise. 
 */
bool_t AudPP_isSleeping(const ThisAudDynaPPSvc_t *me)
{
    return me->audPPStatus.isSleeping;
}

/**
 * Indicates whether PP is in paused state (not processing 
 * data). 
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 * 
 * @param[in] me 
 * Pointer to the current instance structure. 
 * 
 * @return bool_t 
 * TRUE if PP is in paused state. 
 * FALSE otherwise. 
 */
bool_t AudPP_isPaused(const ThisAudDynaPPSvc_t *me)
{
    if (DYNA_SVC_PP_STATE_PAUSE == me->audPPStatus.dynaPPSvcState)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 * Indicates whether PP is in disconnected state (no downstream 
 * service present). 
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 * 
 * @param[in] me 
 * Pointer to the current instance structure. 
 * 
 * @return bool_t 
 * TRUE if PP is in disconnected state. 
 * FALSE otherwise. 
 */
bool_t AudPP_isDisconnected(const ThisAudDynaPPSvc_t *me)
{
    if (NULL == me->pDownstreamPeer)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 * Moves the PP to fatal error state. PP will not process any 
 * data or commands except destroy in this state. All other 
 * commands will be acked with ADSP_EFAILED. 
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 * 
 * @param[in, out] me 
 * Pointer to the current instance structure. 
 */
void AudPP_GotoFatalState(ThisAudDynaPPSvc_t *me)
{
    MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX: AudPP: PP going to FATAL error state.", me->objId);
    AudPP_GotoSleepState(me);
    me->audPPStatus.dynaPPSvcState = DYNA_SVC_PP_STATE_FATAL_ERROR;
}

/**
 * Indicates whether PP is in fatal error state.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 * 
 * @param[in] me 
 * Pointer to the current instance structure. 
 * 
 * @return bool_t 
 * TRUE if PP is in fatal error state. 
 * FALSE otherwise. 
 */
bool_t AudPP_isInFatalState(const ThisAudDynaPPSvc_t *me)
{
    if (DYNA_SVC_PP_STATE_FATAL_ERROR == me->audPPStatus.dynaPPSvcState)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

