/*========================================================================
  This file contains AFE  unit testing realted apis

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFETestHandlers.h#1 $
 ====================================================================== */
#ifndef _AFE_TESTHANDLERS_H_
#define _AFE_TESTHANDLERS_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** AFE loopback test.

  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_fw(void* afeCmdQueue);

/** AFE playback test.
  
  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_fw_playback(void* afeCmdQueue);

/** AFE Device Access Library (DAL) playback test.
  
  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_fw_playback_dal(void* afeCmdQueue);

/** AFE DAL loopback test.
  
  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_fw_loopback_dal(void* afeCmdQueue);

/** AFE extended DAL loopback test.
  
  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_fw_loopback_ext_dal(void* afeCmdQueue);

/** AFE DAL test.
  
  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_fw_test1Dal(void* afeCmdQueue);

/** APR test; initializes the APR start port, etc.
  
  @param[in] dummy Dummy pointer.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_svc_apr_test(void* dummy);

/** APR loopback test.
  
  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_fw_apr(void* afeCmdQueue);

/** AFE pseudo port test.
  
  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */

int         AFE_test_fw_pseudo_port(void* afeCmdQueue);

/** APR DTMF test.
  
  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_fw_apr_dtmf(void* afeCmdQueue);

/** APR pseudo port test.
  
  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_fw_apr_pseudo_port(void* afeCmdQueue);

/** AFE DAL playback test.
  
  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_fw_playback_dal_ac3(void* afeCmdQueue);

/** DAL HDMI playback test.
  
  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.
  
  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_fw_playback_dal_hdmi(void* afeCmdQueue);

/** APR quad-mic and topology test.
  
  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_fw_apr_topology(void* afeCmdQueue);

/** APR loopback test.

  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_apr_loopback(void* afeCmdQueue);

/** APR external loopback test.

  @param[in] afeCmdQueue Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int         AFE_test_apr_ext_loopback(void* afeCmdQueue);


/** AFE Threadpool test.

  @param[in] dummy, argument not used.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int AFE_test_thread_pool(void *dummy);

#ifdef MSM8960
/** APR riva port test.

  @param[in] afe_cmd_q Static service command queue to send a command 
                         to the AFE.

  @return  
  Test ADSP_EOK return code.

  @dependencies
  None.
 */
int AFE_test_riva(void* afe_cmd_q);
#endif // MSM8960

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _AFE_TESTHANDLERS_H_ */
