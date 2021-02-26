/*=============================================================================
  @file adsp_err_fatal.h

  This file contains api for fatal error crash

*******************************************************************************
* Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved
* Qualcomm Technologies Confidential and Proprietary.
********************************************************************************/

/* $Header: //components/rel/dspcore.adsp/4.0/adsp_err_fatal/inc/adsp_err_fatal.h#1 $ */
/* $DateTime: 2017/03/30 23:23:54 $ */
/* $Author: pwbldsvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-04-28   AJ   fatal error crash for AFE
  
============================================================================*/
#ifndef ADSP_ERR_FATAL_H
#define ADSP_ERR_FATAL_H
#ifdef __cplusplus
extern "C" {
#endif

//void adsp_fatal_error(char *fatal_error);
void AdspfatalerrApi(char* err_str,int strlength);
void Adspfatalerr_Deinit_Client(void);
void Adspfatalerr_Init_Client(void);

#ifdef __cplusplus
}
#endif

#endif
