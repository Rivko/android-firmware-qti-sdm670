/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
@file  diagplatform.h
@brief Platform dependent API for Display Calibration Manager

GENERAL DESCRIPTION

  Implementation of platform dependent API for Display Calibration Manager .

  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


#ifndef __DIAGPLATFORM_H__
#define __DIAGPLATFORM_H__

#include "comdef.h"

/*=========================================================================*/
/*Interface API Declaration                                               */
/*=========================================================================*/

/*==========================================================================
FUNCTION dcm_os_intf_control

DESCRIPTION
  This API registers os interface controls.

  flag - TRUE value to register the os interface control.
         FALSE value to unregister the os interface control.

RETURN VALUE
  ZERO - SUCCESS
  NON ZERO - Failed to register different controls.
===========================================================================*/
int dcm_os_intf_control(boolean flag);

#endif //__DIAGPLATFORM_H__
