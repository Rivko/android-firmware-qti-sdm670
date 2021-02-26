/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/stub_src/AFEDmaManager_stub.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     hal_dma_manager.cpp

DESCRIPTION: Common DMA Manager

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2013 QUALCOMM Technologies, Inc. (QTI)
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/stub_src/AFEDmaManager_stub.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/25/13    RP      created

============================================================================*/

#include "qurt_elite.h"
#include "AFEInternal.h"
#include "AFEInterface.h"
#include "AFEDmaManager.h"


ADSPResult afe_dma_mgr_init(void)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_dma_mgr_deinit(void)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_dma_mgr_open(dma_device_handle *h_dma_ptr, dma_mgr_open_params_t *open_params)
{  
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_dma_mgr_close(dma_device_handle h_dma_ptr)
{ 
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_dma_mgr_start(dma_device_handle h_dma_ptr)
{
	return ADSP_EUNSUPPORTED;
}

ADSPResult afe_dma_mgr_stop(dma_device_handle h_dma_ptr)
{
	return ADSP_EUNSUPPORTED;
}

ADSPResult afe_dma_mgr_read(dma_device_handle h_dma_ptr, uint8_t *buffer_ptr)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_dma_mgr_write(dma_device_handle h_dma_ptr, uint8_t *buffer_ptr)
{
   return ADSP_EUNSUPPORTED;
}


ADSPResult afe_dma_mgr_disable_clear_dma_int(dma_device_handle h_dma_ptr)
{
   return ADSP_EUNSUPPORTED;
}


uint32_t afe_dma_mgr_get_dma_index(dma_device_handle h_dma_ptr)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_dma_mgr_read_timestamp_fifo_count(dma_device_handle h_dma_ptr, uint64_t *dma_int_timestamp_ptr, uint32_t *fifo_count_ptr)
{
   return ADSP_EUNSUPPORTED;
}


