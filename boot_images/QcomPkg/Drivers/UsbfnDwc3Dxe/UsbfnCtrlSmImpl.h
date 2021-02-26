/**
 * @file UsbfnCtrlSmImpl.h
 *
 * @ingroup CtrlSm
 *
 * @brief State function declarations
 *
 * This file declares the internal implementation of the control state machine
 * state entry and transition functions.
 *
 * @copyright Copyright (c) 2013-2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/12/14   ck      Address pending setup pkt and End Transfer issues
 06/10/13   mliss   Initial implementation
 =============================================================================*/

#ifndef _USBFN_CTRL_SM_IMPL_H_
#define _USBFN_CTRL_SM_IMPL_H_


/* State entry/exit functions */
VOID Usm_CtrlSetup_Entry   (USM_STATE_MACHINE *Usm, USM_STATE PrevState);
VOID Usm_WaitHost2_Entry   (USM_STATE_MACHINE *Usm, USM_STATE PrevState);
VOID Usm_CtrlStatus2_Entry (USM_STATE_MACHINE *Usm, USM_STATE PrevState);
VOID Usm_CtrlData_Entry    (USM_STATE_MACHINE *Usm, USM_STATE PrevState);
VOID Usm_WaitHost3_Entry   (USM_STATE_MACHINE *Usm, USM_STATE PrevState);
VOID Usm_CtrlStatus3_Entry (USM_STATE_MACHINE *Usm, USM_STATE PrevState);
VOID Usm_SetStall_Entry    (USM_STATE_MACHINE *Usm, USM_STATE PrevState);
VOID Usm_BypassSts_Entry   (USM_STATE_MACHINE *Usm, USM_STATE PrevState);


/* Transition functions */
USM_STATE Usm_EventNotHandled              (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_Stall_Reset                  (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_NoAction_Disconnect          (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_ExternalStall                (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_CtrlSetup_XferComplete       (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_CtrlSetup_XferNotReady       (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_WaitHost2_XferComplete       (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_WaitHost2_XferNotReady       (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_CtrlStatus2_XferComplete     (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_CtrlStatus2_XferNotReady     (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_CtrlStatus2_Reset_Disconnect (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_CtrlData_XferComplete        (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_BypassSts_Handle_PendingSetup(USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_CtrlData_XferNotReady        (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_CtrlData_Reset_Disconnect    (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_WaitHost3_XferComplete       (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_WaitHost3_XferNotReady       (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_CtrlStatus3_XferComplete     (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_CtrlStatus3_XferNotReady     (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_CtrlStatus3_Reset_Disconnect (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_SetStall_Reset               (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);
USM_STATE Usm_SetStall_CmdComplete         (USM_STATE_MACHINE *Usm, DWC_EVT *Evt);


#endif /* _USBFN_CTRL_SM_IMPL_H_ */
