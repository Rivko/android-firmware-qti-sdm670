/*
 @file si_oem.c
 @brief OEM approved application list

 */
/*===========================================================================
 Copyright (c) 2014 Qualcomm Technologies, Inc.
 All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
 ===========================================================================*/

/*===========================================================================

 EDIT HISTORY FOR FILE
 $Header:
 $DateTime:
 $Author:

 when         who     what, where, why
 ----------   ---     -------------------------------------------------------
 08/14/2014   rz      Added empty list instructions
 05/20/2014   rz      Initial Version
 ===========================================================================*/

 /*
** This array holds a list of OEM approved applications.
** Applications on this list are approved to retrieve the Secure Indicator.
** OEMs can edit this list to hold only applications approved by the specific OEM.
**
** In order to define an empty list (i.e. no applications are approved to retrieve
** the Secure Indicator), please define:
** char* APPROVED_APPS[] = {""};
*/
 
char* APPROVED_APPS[] = {"secure_ui_sample"};

/* Do not change */
const int approved_list_len = sizeof(APPROVED_APPS)/sizeof(APPROVED_APPS[0]);

