/*========================================================================

*//** @file VoiceProcTx_stub.cpp

Copyright (c) 2010, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the stub code for Voice Proc Tx(VPTX) Dynamic service. 
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_tx/stub_src/VoiceProcTx_stub.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/29/10           Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include <VoiceProcTx.h>


ADSPResult vptx_create (void **near_handle, void **far_handle, uint16_t afe_tx_port_id,uint16_t afe_rx_port_id,uint32_t topology_id,  uint32_t sampling_rate, uint32_t session_num, uint32_t shared_mem_client)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult vptx_aud_create (void **near_handle, void **far_handle, uint16_t afe_tx_port_id,uint16_t afe_rx_port_id, uint32_t topology_id,  uint32_t sampling_rate, uint32_t session_num, uint32_t shared_mem_client)
{
   return ADSP_EUNSUPPORTED;
}
