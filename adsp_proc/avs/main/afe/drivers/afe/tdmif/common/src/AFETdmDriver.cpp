/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/src/AFETdmDriver.cpp#3 $
$DateTime: 2018/01/27 23:59:02 $
$Author: pwbldsvc $
$Change: 15333238 $
$Revision: #3 $

FILE:     

DESCRIPTION: Main Interface to the AFE TDM driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013 - 2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/src/AFETdmDriver.cpp#3 $ 
$DateTime: 2018/01/27 23:59:02 $
$Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "qurt_elite.h"
#include "qurt_error.h"
#include "AFEDeviceDriver.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DALSys.h"
#include "AFEInternal.h"
#include "AFEInterface.h"
#include "adsp_afe_service_commands.h"
#include "AFETdmDriver.h"
#include "AFETdmDriver_i.h"
#include "hwd_devcfg.h"
#include "hwd_mmpm.h"
#include "AFEDmaManager.h"
#include "AFEHalTdm.h"
#include "AFEAudioIfCommon.h"
#include "AFEDeviceDriverInterface.h"
#include "hwd_devcfg.h"
#include "AFETdmGroupDevice_i.h"

/*=====================================================================
 Globals
 ======================================================================*/
//Note: if it is possible, decouple driver from service layer for better modulization.

//Primary interface port for non-group case
static afe_dev_port_t port_rx_primary_tdm;
static afe_dev_port_t port_tx_primary_tdm;
static afe_dev_port_t port_rx_secondary_tdm;
static afe_dev_port_t port_tx_secondary_tdm;
static afe_dev_port_t port_rx_tertiary_tdm;
static afe_dev_port_t port_tx_tertiary_tdm;
static afe_dev_port_t port_rx_quaternary_tdm;
static afe_dev_port_t port_tx_quaternary_tdm;
static afe_dev_port_t port_rx_quinary_tdm;
static afe_dev_port_t port_tx_quinary_tdm;

//Primary interface group member ports
static afe_dev_port_t port_rx_primary_tdm_1;
static afe_dev_port_t port_tx_primary_tdm_1;
static afe_dev_port_t port_rx_primary_tdm_2;
static afe_dev_port_t port_tx_primary_tdm_2;
static afe_dev_port_t port_rx_primary_tdm_3;
static afe_dev_port_t port_tx_primary_tdm_3;
static afe_dev_port_t port_rx_primary_tdm_4;
static afe_dev_port_t port_tx_primary_tdm_4;
static afe_dev_port_t port_rx_primary_tdm_5;
static afe_dev_port_t port_tx_primary_tdm_5;
static afe_dev_port_t port_rx_primary_tdm_6;
static afe_dev_port_t port_tx_primary_tdm_6;
static afe_dev_port_t port_rx_primary_tdm_7;
static afe_dev_port_t port_tx_primary_tdm_7;

//Secondary interface group member ports
static afe_dev_port_t port_rx_secondary_tdm_1;
static afe_dev_port_t port_tx_secondary_tdm_1;
static afe_dev_port_t port_rx_secondary_tdm_2;
static afe_dev_port_t port_tx_secondary_tdm_2;
static afe_dev_port_t port_rx_secondary_tdm_3;
static afe_dev_port_t port_tx_secondary_tdm_3;
static afe_dev_port_t port_rx_secondary_tdm_4;
static afe_dev_port_t port_tx_secondary_tdm_4;
static afe_dev_port_t port_rx_secondary_tdm_5;
static afe_dev_port_t port_tx_secondary_tdm_5;
static afe_dev_port_t port_rx_secondary_tdm_6;
static afe_dev_port_t port_tx_secondary_tdm_6;
static afe_dev_port_t port_rx_secondary_tdm_7;
static afe_dev_port_t port_tx_secondary_tdm_7;

//Tertiary interface group member ports
static afe_dev_port_t port_rx_tertiary_tdm_1;
static afe_dev_port_t port_tx_tertiary_tdm_1;
static afe_dev_port_t port_rx_tertiary_tdm_2;
static afe_dev_port_t port_tx_tertiary_tdm_2;
static afe_dev_port_t port_rx_tertiary_tdm_3;
static afe_dev_port_t port_tx_tertiary_tdm_3;
static afe_dev_port_t port_rx_tertiary_tdm_4;
static afe_dev_port_t port_tx_tertiary_tdm_4;
static afe_dev_port_t port_rx_tertiary_tdm_5;
static afe_dev_port_t port_tx_tertiary_tdm_5;
static afe_dev_port_t port_rx_tertiary_tdm_6;
static afe_dev_port_t port_tx_tertiary_tdm_6;
static afe_dev_port_t port_rx_tertiary_tdm_7;
static afe_dev_port_t port_tx_tertiary_tdm_7;


//Quaternary interface group member ports
static afe_dev_port_t port_rx_quaternary_tdm_1;
static afe_dev_port_t port_tx_quaternary_tdm_1;
static afe_dev_port_t port_rx_quaternary_tdm_2;
static afe_dev_port_t port_tx_quaternary_tdm_2;
static afe_dev_port_t port_rx_quaternary_tdm_3;
static afe_dev_port_t port_tx_quaternary_tdm_3;
static afe_dev_port_t port_rx_quaternary_tdm_4;
static afe_dev_port_t port_tx_quaternary_tdm_4;
static afe_dev_port_t port_rx_quaternary_tdm_5;
static afe_dev_port_t port_tx_quaternary_tdm_5;
static afe_dev_port_t port_rx_quaternary_tdm_6;
static afe_dev_port_t port_tx_quaternary_tdm_6;
static afe_dev_port_t port_rx_quaternary_tdm_7;
static afe_dev_port_t port_tx_quaternary_tdm_7;

//quinary interface group member ports
static afe_dev_port_t port_rx_quinary_tdm_1;
static afe_dev_port_t port_tx_quinary_tdm_1;
static afe_dev_port_t port_rx_quinary_tdm_2;
static afe_dev_port_t port_tx_quinary_tdm_2;
static afe_dev_port_t port_rx_quinary_tdm_3;
static afe_dev_port_t port_tx_quinary_tdm_3;
static afe_dev_port_t port_rx_quinary_tdm_4;
static afe_dev_port_t port_tx_quinary_tdm_4;
static afe_dev_port_t port_rx_quinary_tdm_5;
static afe_dev_port_t port_tx_quinary_tdm_5;
static afe_dev_port_t port_rx_quinary_tdm_6;
static afe_dev_port_t port_tx_quinary_tdm_6;
static afe_dev_port_t port_rx_quinary_tdm_7;
static afe_dev_port_t port_tx_quinary_tdm_7;


//static delcarations for tdm dev port state per tdm interfaces.
static tdm_port_state_t  tdm_rx_primary;
static tdm_port_state_t  tdm_tx_primary;
static tdm_port_state_t  tdm_rx_secondary;
static tdm_port_state_t  tdm_tx_secondary;
static tdm_port_state_t  tdm_rx_tertiary;
static tdm_port_state_t  tdm_tx_tertiary;
static tdm_port_state_t  tdm_rx_quaternary;
static tdm_port_state_t  tdm_tx_quaternary;
static tdm_port_state_t  tdm_rx_quinary;
static tdm_port_state_t  tdm_tx_quinary;


//global variable for tdm driver to share any variable common to all ports for example device config, etc.
static tdm_drv_state_t    tdm_driver; 

/*=====================================================================
 Static Functions
 ======================================================================*/
static ADSPResult afe_tdm_set_param(void *afe_drv, uint32_t param_id, int8_t *cfg_ptr, uint32_t payload_size);
static ADSPResult afe_tdm_get_param (void *afe_drv, uint32_t param_id, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr);
static ADSPResult afe_tdm_driver_start(void *afe_drv, dev_start_params_t *tdm_start_params_ptr);
static ADSPResult afe_tdm_driver_stop(void *afe_drv);
static ADSPResult afe_tdm_get_dma_timestamp(void *afe_drv, afe_dev_timestamp_info_t *timestamp_info_ptr);
static ADSPResult afe_tdm_get_properties(afe_drv_h_t afe_drv, uint32_t prop_id, int8_t *prop_ptr, 
  uint32_t payload_size);
static ADSPResult afe_tdm_get_dev_port_cfg (void *afe_drv, afe_dev_port_config_t  *tdm_port_cfg_data_ptr);
static void afe_tdm_write_dma(void *afe_driver_context, uint8_t *src_address, uint32_t *dst_address);
static void afe_tdm_read_dma(void *afe_driver_context, uint32_t *src_address, uint8_t *dst_address);
static ADSPResult afe_tdm_dev_config_port (tdm_port_state_t* p_dev_state, afe_param_id_tdm_cfg_t *tdm_cfg_ptr);


static void afe_tdm_int_callback(void* afe_drv, uint32 event_mask);
static ADSPResult afe_tdm_driver_read(afe_drv_h_t afe_drv,  int16_t *buffer_ptr, uint32_t num_dma_samples);
static ADSPResult afe_tdm_driver_write(afe_drv_h_t afe_drv,  int16_t *buffer_ptr, uint32_t num_dma_samples);
static uint32 afe_tdm_get_watermark(tdm_port_state_t *p_dev_state);
static uint32 afe_tdm_get_num_dma_buffer(tdm_port_state_t *p_dev_state);
static ADSPResult afe_tdm_driver_device_cfg_init(tdm_dev_cfg_struct_t * tdm_hw_info_ptr);

static afe_driver_func_tbl_t sink_ftbl =
{

   afe_tdm_driver_write,
   afe_tdm_set_param,
   afe_tdm_get_param,
   afe_tdm_driver_start,
   afe_tdm_driver_stop,
   NULL,
   afe_tdm_get_properties,
};

static afe_driver_func_tbl_t source_ftbl =
{
   afe_tdm_driver_read,
   afe_tdm_set_param,
   afe_tdm_get_param,
   afe_tdm_driver_start,
   afe_tdm_driver_stop,
   NULL,
   afe_tdm_get_properties,
};

typedef struct {
  uint16_t portid;
  tdm_interface_t inf;
  uint32_t dir;
  afe_driver_func_tbl_t *p_func_tabl;
  tdm_port_state_t *p_dev_state;
} tdm_init_info_type;


// how we handle other group other than primary...??? david
const tdm_init_info_type a_init_tdm_info[MAX_TDM_PORT_NUM] =
{
  { AFE_PORT_ID_PRIMARY_TDM_RX,   TDM_PRIMARY, TDM_SINK,   &sink_ftbl,     &tdm_rx_primary },
  { AFE_PORT_ID_PRIMARY_TDM_TX,   TDM_PRIMARY, TDM_SOURCE, &source_ftbl,   &tdm_tx_primary },

  { AFE_PORT_ID_SECONDARY_TDM_RX, TDM_SECONDARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_secondary},
  { AFE_PORT_ID_SECONDARY_TDM_TX, TDM_SECONDARY, TDM_SOURCE, &source_ftbl, &tdm_tx_secondary},

  { AFE_PORT_ID_TERTIARY_TDM_RX,  TDM_TERTIARY, TDM_SINK,    &sink_ftbl,   &tdm_rx_tertiary },
  { AFE_PORT_ID_TERTIARY_TDM_TX,  TDM_TERTIARY, TDM_SOURCE,  &source_ftbl, &tdm_tx_tertiary},

  { AFE_PORT_ID_QUATERNARY_TDM_RX,  TDM_QUATERNARY, TDM_SINK,   &sink_ftbl, &tdm_rx_quaternary  },
  { AFE_PORT_ID_QUATERNARY_TDM_TX,  TDM_QUATERNARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quaternary},

  { AFE_PORT_ID_QUINARY_TDM_RX,  TDM_QUINARY, TDM_SINK,   &sink_ftbl, &tdm_rx_quinary  },
  { AFE_PORT_ID_QUINARY_TDM_TX,  TDM_QUINARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quinary},

  // Group member port for primary interface
  { AFE_PORT_ID_PRIMARY_TDM_RX_1,   TDM_PRIMARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_primary },
  { AFE_PORT_ID_PRIMARY_TDM_TX_1,   TDM_PRIMARY, TDM_SOURCE, &source_ftbl, &tdm_tx_primary},
  
  { AFE_PORT_ID_PRIMARY_TDM_RX_2,   TDM_PRIMARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_primary},
  { AFE_PORT_ID_PRIMARY_TDM_TX_2,   TDM_PRIMARY, TDM_SOURCE, &source_ftbl, &tdm_tx_primary},
  
  { AFE_PORT_ID_PRIMARY_TDM_RX_3,   TDM_PRIMARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_primary},
  { AFE_PORT_ID_PRIMARY_TDM_TX_3,   TDM_PRIMARY, TDM_SOURCE, &source_ftbl, &tdm_tx_primary},
  
  { AFE_PORT_ID_PRIMARY_TDM_RX_4,   TDM_PRIMARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_primary},
  { AFE_PORT_ID_PRIMARY_TDM_TX_4,   TDM_PRIMARY, TDM_SOURCE, &source_ftbl, &tdm_tx_primary},
  
  { AFE_PORT_ID_PRIMARY_TDM_RX_5,   TDM_PRIMARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_primary},
  { AFE_PORT_ID_PRIMARY_TDM_TX_5,   TDM_PRIMARY, TDM_SOURCE, &source_ftbl, &tdm_tx_primary},
  
  { AFE_PORT_ID_PRIMARY_TDM_RX_6,   TDM_PRIMARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_primary},
  { AFE_PORT_ID_PRIMARY_TDM_TX_6,   TDM_PRIMARY, TDM_SOURCE, &source_ftbl, &tdm_tx_primary},
  
  { AFE_PORT_ID_PRIMARY_TDM_RX_7,   TDM_PRIMARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_primary},
  { AFE_PORT_ID_PRIMARY_TDM_TX_7,   TDM_PRIMARY, TDM_SOURCE, &source_ftbl, &tdm_tx_primary},

  // Group member port for secondary interface
  { AFE_PORT_ID_SECONDARY_TDM_RX_1, TDM_SECONDARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_secondary},
  { AFE_PORT_ID_SECONDARY_TDM_TX_1, TDM_SECONDARY, TDM_SOURCE, &source_ftbl, &tdm_tx_secondary},
  
  { AFE_PORT_ID_SECONDARY_TDM_RX_2, TDM_SECONDARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_secondary},
  { AFE_PORT_ID_SECONDARY_TDM_TX_2, TDM_SECONDARY, TDM_SOURCE, &source_ftbl, &tdm_tx_secondary},
  
  { AFE_PORT_ID_SECONDARY_TDM_RX_3, TDM_SECONDARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_secondary},
  { AFE_PORT_ID_SECONDARY_TDM_TX_3, TDM_SECONDARY, TDM_SOURCE, &source_ftbl, &tdm_tx_secondary},
  
  { AFE_PORT_ID_SECONDARY_TDM_RX_4, TDM_SECONDARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_secondary},
  { AFE_PORT_ID_SECONDARY_TDM_TX_4, TDM_SECONDARY, TDM_SOURCE, &source_ftbl, &tdm_tx_secondary},
  
  { AFE_PORT_ID_SECONDARY_TDM_RX_5, TDM_SECONDARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_secondary},
  { AFE_PORT_ID_SECONDARY_TDM_TX_5, TDM_SECONDARY, TDM_SOURCE, &source_ftbl, &tdm_tx_secondary},
  
  { AFE_PORT_ID_SECONDARY_TDM_RX_6, TDM_SECONDARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_secondary},
  { AFE_PORT_ID_SECONDARY_TDM_TX_6, TDM_SECONDARY, TDM_SOURCE, &source_ftbl, &tdm_tx_secondary},
  
  { AFE_PORT_ID_SECONDARY_TDM_RX_7, TDM_SECONDARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_secondary},
  { AFE_PORT_ID_SECONDARY_TDM_TX_7, TDM_SECONDARY, TDM_SOURCE, &source_ftbl, &tdm_tx_secondary},


  // Group member port for tertiary interface
  { AFE_PORT_ID_TERTIARY_TDM_RX_1,   TDM_TERTIARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_tertiary},
  { AFE_PORT_ID_TERTIARY_TDM_TX_1,   TDM_TERTIARY, TDM_SOURCE, &source_ftbl, &tdm_tx_tertiary},
  
  { AFE_PORT_ID_TERTIARY_TDM_RX_2,   TDM_TERTIARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_tertiary},
  { AFE_PORT_ID_TERTIARY_TDM_TX_2,   TDM_TERTIARY, TDM_SOURCE, &source_ftbl, &tdm_tx_tertiary},
  
  { AFE_PORT_ID_TERTIARY_TDM_RX_3,   TDM_TERTIARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_tertiary},
  { AFE_PORT_ID_TERTIARY_TDM_TX_3,   TDM_TERTIARY, TDM_SOURCE, &source_ftbl, &tdm_tx_tertiary},
  
  { AFE_PORT_ID_TERTIARY_TDM_RX_4,   TDM_TERTIARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_tertiary},
  { AFE_PORT_ID_TERTIARY_TDM_TX_4,   TDM_TERTIARY, TDM_SOURCE, &source_ftbl, &tdm_tx_tertiary},
  
  { AFE_PORT_ID_TERTIARY_TDM_RX_5,   TDM_TERTIARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_tertiary},
  { AFE_PORT_ID_TERTIARY_TDM_TX_5,   TDM_TERTIARY, TDM_SOURCE, &source_ftbl, &tdm_tx_tertiary},
  
  { AFE_PORT_ID_TERTIARY_TDM_RX_6,   TDM_TERTIARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_tertiary},
  { AFE_PORT_ID_TERTIARY_TDM_TX_6,   TDM_TERTIARY, TDM_SOURCE, &source_ftbl, &tdm_tx_tertiary},
  
  { AFE_PORT_ID_TERTIARY_TDM_RX_7,   TDM_TERTIARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_tertiary},
  { AFE_PORT_ID_TERTIARY_TDM_TX_7,   TDM_TERTIARY, TDM_SOURCE, &source_ftbl, &tdm_tx_tertiary},


  // Group member port for quaternary interface
  { AFE_PORT_ID_QUATERNARY_TDM_RX_1, TDM_QUATERNARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quaternary},
  { AFE_PORT_ID_QUATERNARY_TDM_TX_1, TDM_QUATERNARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quaternary},
  
  { AFE_PORT_ID_QUATERNARY_TDM_RX_2, TDM_QUATERNARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quaternary},
  { AFE_PORT_ID_QUATERNARY_TDM_TX_2, TDM_QUATERNARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quaternary},
  
  { AFE_PORT_ID_QUATERNARY_TDM_RX_3, TDM_QUATERNARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quaternary},
  { AFE_PORT_ID_QUATERNARY_TDM_TX_3, TDM_QUATERNARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quaternary},
  
  { AFE_PORT_ID_QUATERNARY_TDM_RX_4, TDM_QUATERNARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quaternary},
  { AFE_PORT_ID_QUATERNARY_TDM_TX_4, TDM_QUATERNARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quaternary},
  
  { AFE_PORT_ID_QUATERNARY_TDM_RX_5, TDM_QUATERNARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quaternary},
  { AFE_PORT_ID_QUATERNARY_TDM_TX_5, TDM_QUATERNARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quaternary},
  
  { AFE_PORT_ID_QUATERNARY_TDM_RX_6, TDM_QUATERNARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quaternary},
  { AFE_PORT_ID_QUATERNARY_TDM_TX_6, TDM_QUATERNARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quaternary},
  
  { AFE_PORT_ID_QUATERNARY_TDM_RX_7, TDM_QUATERNARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quaternary},
  { AFE_PORT_ID_QUATERNARY_TDM_TX_7, TDM_QUATERNARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quaternary},

  // Group member port for quinary interface
  { AFE_PORT_ID_QUINARY_TDM_RX_1, TDM_QUINARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quinary},
  { AFE_PORT_ID_QUINARY_TDM_TX_1, TDM_QUINARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quinary},

  { AFE_PORT_ID_QUINARY_TDM_RX_2, TDM_QUINARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quinary},
  { AFE_PORT_ID_QUINARY_TDM_TX_2, TDM_QUINARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quinary},

  { AFE_PORT_ID_QUINARY_TDM_RX_3, TDM_QUINARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quinary},
  { AFE_PORT_ID_QUINARY_TDM_TX_3, TDM_QUINARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quinary},

  { AFE_PORT_ID_QUINARY_TDM_RX_4, TDM_QUINARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quinary},
  { AFE_PORT_ID_QUINARY_TDM_TX_4, TDM_QUINARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quinary},

  { AFE_PORT_ID_QUINARY_TDM_RX_5, TDM_QUINARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quinary},
  { AFE_PORT_ID_QUINARY_TDM_TX_5, TDM_QUINARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quinary},

  { AFE_PORT_ID_QUINARY_TDM_RX_6, TDM_QUINARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quinary},
  { AFE_PORT_ID_QUINARY_TDM_TX_6, TDM_QUINARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quinary},

  { AFE_PORT_ID_QUINARY_TDM_RX_7, TDM_QUINARY, TDM_SINK,   &sink_ftbl,   &tdm_rx_quinary},
  { AFE_PORT_ID_QUINARY_TDM_TX_7, TDM_QUINARY, TDM_SOURCE, &source_ftbl, &tdm_tx_quinary},

};
  
  
static afe_dev_port_t *a_tdm_dev_port[MAX_TDM_PORT_NUM] = { 
   &port_rx_primary_tdm,
   &port_tx_primary_tdm,
   &port_rx_secondary_tdm,
   &port_tx_secondary_tdm,
   &port_rx_tertiary_tdm,
   &port_tx_tertiary_tdm,
   &port_rx_quaternary_tdm,
   &port_tx_quaternary_tdm,
   &port_rx_quinary_tdm,
   &port_tx_quinary_tdm,
  
   &port_rx_primary_tdm_1,
   &port_tx_primary_tdm_1,
   &port_rx_primary_tdm_2,
   &port_tx_primary_tdm_2,
   &port_rx_primary_tdm_3,
   &port_tx_primary_tdm_3,
   &port_rx_primary_tdm_4,
   &port_tx_primary_tdm_4,
   &port_rx_primary_tdm_5,
   &port_tx_primary_tdm_5,
   &port_rx_primary_tdm_6,
   &port_tx_primary_tdm_6,
   &port_rx_primary_tdm_7,
   &port_tx_primary_tdm_7,

   &port_rx_secondary_tdm_1,
   &port_tx_secondary_tdm_1,
   &port_rx_secondary_tdm_2,
   &port_tx_secondary_tdm_2,
   &port_rx_secondary_tdm_3,
   &port_tx_secondary_tdm_3,
   &port_rx_secondary_tdm_4,
   &port_tx_secondary_tdm_4,
   &port_rx_secondary_tdm_5,
   &port_tx_secondary_tdm_5,
   &port_rx_secondary_tdm_6,
   &port_tx_secondary_tdm_6,
   &port_rx_secondary_tdm_7,
   &port_tx_secondary_tdm_7,

   &port_rx_tertiary_tdm_1,
   &port_tx_tertiary_tdm_1,
   &port_rx_tertiary_tdm_2,
   &port_tx_tertiary_tdm_2,
   &port_rx_tertiary_tdm_3,
   &port_tx_tertiary_tdm_3,
   &port_rx_tertiary_tdm_4,
   &port_tx_tertiary_tdm_4,
   &port_rx_tertiary_tdm_5,
   &port_tx_tertiary_tdm_5,
   &port_rx_tertiary_tdm_6,
   &port_tx_tertiary_tdm_6,
   &port_rx_tertiary_tdm_7,
   &port_tx_tertiary_tdm_7,
  
   &port_rx_quaternary_tdm_1,
   &port_tx_quaternary_tdm_1,
   &port_rx_quaternary_tdm_2,
   &port_tx_quaternary_tdm_2,
   &port_rx_quaternary_tdm_3,
   &port_tx_quaternary_tdm_3,
   &port_rx_quaternary_tdm_4,
   &port_tx_quaternary_tdm_4,
   &port_rx_quaternary_tdm_5,
   &port_tx_quaternary_tdm_5,
   &port_rx_quaternary_tdm_6,
   &port_tx_quaternary_tdm_6,
   &port_rx_quaternary_tdm_7,
   &port_tx_quaternary_tdm_7,

   &port_rx_quinary_tdm_1,
   &port_tx_quinary_tdm_1,
   &port_rx_quinary_tdm_2,
   &port_tx_quinary_tdm_2,
   &port_rx_quinary_tdm_3,
   &port_tx_quinary_tdm_3,
   &port_rx_quinary_tdm_4,
   &port_tx_quinary_tdm_4,
   &port_rx_quinary_tdm_5,
   &port_tx_quinary_tdm_5,
   &port_rx_quinary_tdm_6,
   &port_tx_quinary_tdm_6,
   &port_rx_quinary_tdm_7,
   &port_tx_quinary_tdm_7

};
  
afe_dev_avt_drift_t  tdm_avt_drift[MAX_TDM_PORT_NUM];


/*=====================================================================
 Functions
 ======================================================================*/



/**
  @brief Perform TDM Driver Initialization
            The driver init function does
                  1. get hardware device config for tdm
                  2. get the virtual addres for hardware registers
                  3. get the avtimer tick frequency which will be used for converting the dma timestamp to usec.
                  4. initialize all the device driver structures 

  @param[in]
  None

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_tdm_driver_init(void)
{
	ADSPResult   result = ADSP_EOK;

	memset(&tdm_driver, 0, sizeof(tdm_drv_state_t));
 	tdm_dev_cfg_struct_t *tdm_hw_info_ptr = &tdm_driver.tdm_hw_info;

   // 1. read tdm hwd dev config and cache it to the tdm_driver global state
   if(ADSP_FAILED(result = afe_tdm_driver_device_cfg_init(tdm_hw_info_ptr)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error afe_tdm_driver_device_cfg_init");
      return result;
   }

   // 2. create memory region for lpass tdm interface hw
   //   This information is need to be saved to  tdm_driver global state
   // tdm register interface
   if(ADSP_FAILED(result = HwdDevCfg_GetVirtAddress(&tdm_driver.tdm_qmem_region,
            tdm_hw_info_ptr->tdm_reg_base, tdm_hw_info_ptr->tdm_reg_size, &tdm_driver.tdm_virt_addr)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error obtaining virtual addr for tdm register base");
      return result;
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "tdm virtual addr 0x%x phy addr 0x%x",
      (unsigned int)tdm_driver.tdm_virt_addr, (unsigned int)tdm_hw_info_ptr->tdm_reg_base);
   }

   // 3. do the hal init which save the physical address and virtual address offset.
   hal_tdm_init(tdm_hw_info_ptr->hw_revision, tdm_hw_info_ptr->tdm_reg_base, tdm_driver.tdm_virt_addr, tdm_hw_info_ptr->tdm_reg_offset);

   //4. initialize all the device driver structures 

   tdm_driver.driver_state = TDM_DRIVER_INIT;

   return ADSP_EOK;
}

/**
  @brief Perform TDM Driver De-initialization

  @param[in] none

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_tdm_driver_deinit(void)
{
   if(TDM_DRIVER_INIT != tdm_driver.driver_state)
   {
      return ADSP_EFAILED;
   }

   /* release all resource allocated during init*/
   HwdDevCfg_DestoryVirtAddress(tdm_driver.tdm_qmem_region);

   // change the driver state.
   tdm_driver.driver_state = TDM_DRIVER_DEINIT;

   return ADSP_EOK;
}

/**
  @brief Perform TDM port initialization
               The driver port init function does these per port.
                   init the dev port state, afe service port structures, and service global state

                  1.  save the afe service port structure pointer to service global state aud_stat_afe_svc_t

                  2. service port structure
                       call afe_svc_init_port() per port which fill the basic information and default values.
                       set the avt drvier pointer which is provided from driver side.
                       set the data handling function pointer (afe_dev_process).
                       save the avtimer latch type
                       save the handle of these device state per port to the port structure                    

                  3.  save the interface enum and direction 

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_tdm_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
    ADSPResult      result;
    afe_dev_port    *dev_port_ptr;
    uint16_t        port_id;
    tdm_interface_t tdm_inf;
    uint32_t dir;
    tdm_port_state_t *p_dev_state;
    afe_driver_func_tbl_t *ftable_ptr;
    uint32_t i;

    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_tdm_port_init()");
    if(TDM_DRIVER_INIT != tdm_driver.driver_state)
    {
       return ADSP_EFAILED;
    }

   afe_svc_ptr->ports_audioif_tdm_ptr = a_tdm_dev_port;


    for(i = 0; i < MAX_TDM_PORT_NUM; i++)
    {
      // Collect init info 
      port_id = a_init_tdm_info[i].portid;
      dir = a_init_tdm_info[i].dir;
      tdm_inf = a_init_tdm_info[i].inf;
      ftable_ptr = a_init_tdm_info[i].p_func_tabl;
      p_dev_state = a_init_tdm_info[i].p_dev_state;

      dev_port_ptr = a_tdm_dev_port[i];

      if(ADSP_FAILED(result = afe_svc_init_port(dev_port_ptr, port_id, dir)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to create port = %d, aborting", port_id);
         return result;
      }
  
      dev_port_ptr->avt_drift_params.avt_drift_struct_ptr = &tdm_avt_drift[i];
      
      dev_port_ptr->afe_dev_process = NULL;
      dev_port_ptr->afe_drv_ftbl = ftable_ptr;
      dev_port_ptr->avt_drift_params.avtimer_latch_type = 
        TIMESTAMP_LATCH_TYPE_HARDWARE;

      memset(p_dev_state, 0, sizeof(tdm_port_state_t));
      dev_port_ptr->afe_drv = (void *) p_dev_state;

      p_dev_state->intf = tdm_inf;
      p_dev_state->direction = dir;

    }

    afe_svc_ptr->num_audioif_tdm_port = MAX_TDM_PORT_NUM;

    return ADSP_EOK;
}



/**
  @brief Perform TDM port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_tdm_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
   // free resources if there is anything is allocated during tdm port init.
   afe_dev_port	*dev_port_ptr;
   uint32_t i;

   // check the driver state
   if(TDM_DRIVER_INIT != tdm_driver.driver_state)
   {
      return ADSP_EFAILED;
   }
   
   //  The port id is not straitforward because the port id are mixed up for tdm, 
   for(i = 0; i < MAX_TDM_PORT_NUM; i++)
   {
     dev_port_ptr = a_tdm_dev_port[i];
     if(NULL != dev_port_ptr)
     {
       afe_svc_deinit_port(dev_port_ptr);
     }
   }

   return ADSP_EOK;
}


void afe_tdm_get_dev_port(uint16 unPortId, afe_dev_port_t **pDevPort)
{
    uint32_t i;
    *pDevPort = NULL;

    for(i = 0; i < MAX_TDM_PORT_NUM; i++)
    {
      if(unPortId == a_init_tdm_info[i].portid)
      {
         *pDevPort = a_tdm_dev_port[i];
         return;
      }
    }
}


/**
  @brief Set tdm port config
               shoudn't pass any of afe_dev_port_t structure type from this point to make driver independent from service layer.

  @param[in] afe_drv pointer for AFE dev port structure
  @param[in] cfg_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_tdm_set_param(void *afe_drv, uint32_t param_id, int8_t *cfg_ptr, uint32_t payload_size)
{
   ADSPResult result = ADSP_EOK;
   tdm_port_state_t *p_dev_state = (tdm_port_state_t *) afe_drv;

   // check the driver state
   if(TDM_DRIVER_INIT != tdm_driver.driver_state)
   {
      return ADSP_EFAILED;
   }   

   if((NULL == afe_drv) || (NULL == cfg_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to tdm device handle / params pointer is null");
      return ADSP_EFAILED;
   }   

   switch(param_id)
   {
      case AFE_PARAM_ID_TDM_CONFIG:
      { 
         // choose the appropriate config version
         uint32_t tdm_cfg_version = (uint32_t) *(cfg_ptr);
         if(tdm_cfg_version > AFE_API_VERSION_TDM_CONFIG)
         {
            tdm_cfg_version = AFE_API_VERSION_TDM_CONFIG;
         }

         switch (tdm_cfg_version)
         {
            case AFE_API_VERSION_TDM_CONFIG:
            {
               afe_param_id_tdm_cfg_t *tdm_cfg_ptr =  (afe_param_id_tdm_cfg_t *)cfg_ptr;

               if (payload_size < sizeof(afe_param_id_tdm_cfg_t))
               {
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "TDM Cfg fail: Invalid payload size: %ld, tdm port index:%d, dir:%d", \
                     payload_size, p_dev_state->intf, p_dev_state->direction);
                     result = ADSP_EBADPARAM;
               }
               else
               {
                  result = afe_tdm_dev_config_port(p_dev_state, tdm_cfg_ptr);
               }
               break;
            }
            default:
                return ADSP_EUNSUPPORTED;
         }
         break;
      }
      default:
         return ADSP_EUNSUPPORTED;
   }
   
   return result;
}


/**
  @brief Get TDM port config
               
  @param[in] afe_drv pointer for afe device port structure
  @param[in] param_buf_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload
  @param[out] params_buffer_len_req_ptr   pointer to store the size of the filled buffer

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_tdm_get_param (void *afe_drv, uint32_t param_id, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
   ADSPResult result = ADSP_EOK;
   tdm_port_state_t *p_dev_state = (tdm_port_state_t *) afe_drv;

   if((NULL == afe_drv) || (NULL == param_buf_ptr) ||  (NULL == params_buffer_len_req_ptr) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to tdm device handle / params pointers are null");
      return ADSP_EFAILED;
   }

   switch(param_id)
   {
      case AFE_PARAM_ID_TDM_CONFIG:
      { 
         switch (p_dev_state->dev_cfg_minor_ver)
         {
             case AFE_API_VERSION_TDM_CONFIG:
             {
                 *params_buffer_len_req_ptr = sizeof(afe_param_id_tdm_cfg_t);
                 if(*params_buffer_len_req_ptr > payload_size )
                 {
                     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: TDM cfg required size = %d, given size = %lu",
                                     *params_buffer_len_req_ptr, payload_size);
                     return ADSP_ENOMEMORY;
                 }
                 afe_param_id_tdm_cfg_t  *tdm_cfg_ptr = (afe_param_id_tdm_cfg_t *)param_buf_ptr;
       
                 /* Populate the payload parameters */
                 tdm_cfg_ptr->tdm_cfg_minor_version = p_dev_state->dev_cfg_minor_ver;
                 tdm_cfg_ptr->num_channels  = p_dev_state->num_channels;
                 tdm_cfg_ptr->sample_rate  = p_dev_state->sample_rate;
                 tdm_cfg_ptr->bit_width    = p_dev_state->bit_width;
                 tdm_cfg_ptr->data_format = p_dev_state->data_format;
                 tdm_cfg_ptr->sync_mode = p_dev_state->sync_type;
                 tdm_cfg_ptr->sync_src = p_dev_state->msm_mode;
                 tdm_cfg_ptr->nslots_per_frame = p_dev_state->nslots_per_frame;
                 tdm_cfg_ptr->ctrl_data_out_enable = p_dev_state->ctrl_data_oe;
                 tdm_cfg_ptr->ctrl_invert_sync_pulse = p_dev_state->ctrl_invert_sync_pulse;

                 /* This need conversion... (0, 1, 2) ==> (2, 1, 0) */
                 tdm_cfg_ptr->ctrl_sync_data_delay = p_dev_state->ctrl_sync_data_delay;

                 tdm_cfg_ptr->slot_width = p_dev_state->slot_width;
                 tdm_cfg_ptr->slot_mask = p_dev_state->slot_mask;
              }
              break;
              default:
              {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "TDM cfg Get Cmd Cfg fail: InCompatible cfg version: %ld, tdm intf : %d",
                          p_dev_state->dev_cfg_minor_ver, p_dev_state->intf);
                  return ADSP_EBADPARAM;
              }
         }
         
         break;
     }
     default:
         return ADSP_EUNSUPPORTED;
   }
  
   return result;
}


/**
  @brief Set up the data transfer for TDM port
               1. check if any shared resource is already is used. mux, data lines,...
               2. open dma manger with callback function
               3. config tdm interface
               4. start the dma
               5. enable the tdm interface

  @param[in] afe_drv pointer for afe device port structure
  @param[in]  tdm_open_params_ptr for open parameters

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_tdm_driver_start(void *afe_drv, dev_start_params_t *tdm_start_params_ptr)
{
   ADSPResult result = ADSP_EOK;
   tdm_port_state_t *p_dev_state = (tdm_port_state_t *) afe_drv;
   dma_mgr_open_params_t open_params;
   uint32_t tdm_index = (uint32_t)p_dev_state->intf;
   uint32_t tdm_dir =  (uint32_t)p_dev_state->direction;

   if((NULL == afe_drv) || (NULL == tdm_start_params_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to tdm device handle / Open Params Null");
      return ADSP_EFAILED;
   }

   // config mux only for PRI, SEC, TER, QUA TDM.
   // what if there is no mux required for pri, sec, ter, qua....
   //1. alloc any shared resource
   uint32_t mux_index = (uint32_t)p_dev_state->intf;
   if(ADSP_FAILED(result = afe_audioif_mux_alloc(AUDIOIF_TDM, mux_index)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "The audioif mux already used for tdm %ul", p_dev_state->intf);
      return result;
   }

   /* Set the block transfer time */
   // this will be determined during the beginning of start cmd and after the config cmd 
   p_dev_state->int_samples_per_period = tdm_start_params_ptr->int_samples_per_period;
   
   // callback info for interrupt handling
   p_dev_state->cb_info.afe_dev_int_cb = tdm_start_params_ptr->cb_info.afe_dev_int_cb;
   p_dev_state->cb_info.cb_arg = tdm_start_params_ptr->cb_info.cb_arg;

   // dma manager open
   memset(&open_params, 0, sizeof(dma_mgr_open_params_t));

   // hardware type and dma type
   open_params.hardware_type = LPASS_HW_TDM;
   open_params.dma_type = LPASS_HW_DMA_TYPE0;

   //  open dma manger with callback function for read or write
   if(TDM_SINK == p_dev_state->direction)
   {
      open_params.dma_dir = LPASS_HW_DMA_SINK;
      open_params.dma_write_func_ptr = (dma_mgr_write_func)afe_tdm_write_dma;
   }
   else
   {
      open_params.dma_dir = LPASS_HW_DMA_SOURCE;
    	open_params.dma_read_func_ptr = (dma_mgr_read_func)afe_tdm_read_dma;
   }

   
   open_params.sample_rate = p_dev_state->sample_rate;

   open_params.int_samples_per_period = p_dev_state->int_samples_per_period;

   open_params.input_bytes_per_sample =  p_dev_state->bytes_per_channel;
      
   open_params.output_bytes_per_sample =  p_dev_state->bytes_per_channel;

   open_params.num_channels = p_dev_state->num_channels;
   
   // dma buffer type   
   open_params.buff_mem_type = DMA_MGR_LPM_BUFFER; 

   // 4 kinds of format is possible here. combination of linear/non-linear  packed/non-packed
   open_params.data_type = AFE_UNCOMPRESSED_DATA_PORT;
   if(AFE_GENERIC_COMPRESSED_DATA == p_dev_state->data_format)
   {
   open_params.data_type = p_dev_state->data_format;
   }

 	// register the interrupt callback 
   open_params.dma_int_cb = afe_tdm_int_callback;
   open_params.afe_driver_context = afe_drv;

   // this info will be passing down to dma hal and it is only for audioif cases including tdm, .
   // This value should be matching to audioif dma hal config function
   open_params.ifconfig_dma_control  = hal_tdm_get_intf(p_dev_state->intf, tdm_dir);

   // get the watermark value. if watermark is 1, then dma burst mode is supposed to be off.  
   open_params.watermark = afe_tdm_get_watermark(p_dev_state);

   // dma buffer size need to be checked carefully based on alignment requirement...
   if( 0 == (open_params.num_buffers = afe_tdm_get_num_dma_buffer(p_dev_state)))
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get req DMA num buffers");
     goto __bailout;
   }

   if(ADSP_FAILED(result = afe_dma_mgr_open(&p_dev_state->dma_dev_hdl, &open_params)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register with DMA Manager with status %d",result);
      goto __bailout;
   }

   /* HAL V2 way to reset , release, config depends on direction */
   hal_tdm_reset(tdm_index, tdm_dir);
   hal_tdm_reset_release(tdm_index, tdm_dir);        
  
   // config tdm interface
   tdm_config_t tdm_config_param;
   memset(&tdm_config_param, 0, sizeof(tdm_config_t));

   // Masater or Slave
   tdm_config_param.sync_src = p_dev_state->msm_mode;
   // Short or Long sync type
   tdm_config_param.sync_type = p_dev_state->sync_type;
   // Number of channels
   tdm_config_param.num_channels = p_dev_state->num_channels;
   // Bit width
   tdm_config_param.bit_width = p_dev_state->bit_width;
   // Slot width
   tdm_config_param.slot_width = p_dev_state->slot_width;
   // Number of slots in a frame
   tdm_config_param.nslots_per_frame = p_dev_state->nslots_per_frame;
   // Invert the sync or not
   tdm_config_param.sync_invert = p_dev_state->ctrl_invert_sync_pulse;
   // Delay to data w.r.t sync
   tdm_config_param.sync_data_delay = p_dev_state->ctrl_sync_data_delay;
   // OE setup
   tdm_config_param.ctrl_data_oe = p_dev_state->ctrl_data_oe;
   // Slot mask
   tdm_config_param.slot_mask = p_dev_state->slot_mask;
   
   hal_tdm_config(tdm_index, tdm_dir, &tdm_config_param);

   //start the DMA engine
	if(ADSP_FAILED(result = afe_dma_mgr_start(p_dev_state->dma_dev_hdl)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to start DMA engine with status %d",result);
		afe_dma_mgr_close(p_dev_state->dma_dev_hdl);
		p_dev_state->dma_dev_hdl = NULL;
		goto __bailout;
	}

	// Enabling the interrupts will be done in the dma manager

   //enable the inteface
   hal_tdm_enable(tdm_index, tdm_dir);
    
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "TDM port intf %d successfully opened", p_dev_state->intf);

   __bailout:

   return result;
}


/**
  @brief Stop the data transfer from tdm port

  @param[in] dev_port_ptr pointer for AFE port structure

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_tdm_driver_stop(void *afe_drv)
{
	ADSPResult result = ADSP_EOK;
   tdm_port_state_t *p_dev_state = (tdm_port_state_t *) afe_drv;

   if(NULL == afe_drv)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to tdm device handle Null");
      return ADSP_EFAILED;
   }

   // disable and clear dma interrupt
   afe_dma_mgr_disable_clear_dma_int(p_dev_state->dma_dev_hdl);

   uint32_t tdm_index = (uint32_t)p_dev_state->intf;
   uint32_t tdm_dir =  (uint32_t)p_dev_state->direction;

   hal_tdm_disable(tdm_index, tdm_dir);

   //  free any shared resource
   uint32_t mux_index = (uint32_t)p_dev_state->intf;
   if(ADSP_FAILED(result = afe_audioif_mux_free(AUDIOIF_TDM, mux_index)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "The audioif mux free failed tdm %ul", p_dev_state->intf);
   }

   //stop the DMA engine
   afe_dma_mgr_stop(p_dev_state->dma_dev_hdl);

   //De-register with the DMA Manager including the isr callback
   afe_dma_mgr_close(p_dev_state->dma_dev_hdl);
   p_dev_state->dma_dev_hdl = NULL;

   return result;
}


/**
  @brief Read data from the tdm DMA buffer to the client buffer

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] buffer_ptr pointer to the client buffer
  @param[in] num_dma_samples number of samples to read from DMA buffer

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_tdm_driver_read(afe_drv_h_t afe_drv, int16_t *buffer_ptr, uint32_t num_dma_samples)
{

	ADSPResult result = ADSP_EOK;
   tdm_port_state_t *p_dev_state = (tdm_port_state_t *)afe_drv;
   int16_t           num_channels;
   uint32_t          total_num_samples, samp;
   int32_t           *ptr_pBuf32;
   int64_t           *ptr_dst_64;
   uint16_t          q_format_shift_factor;

   if((NULL == afe_drv) || (NULL == buffer_ptr))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to tdm device handle %p / buffer pointer %p Null", afe_drv, buffer_ptr);
      return ADSP_EFAILED;
   }

   num_channels = p_dev_state->num_channels;
   q_format_shift_factor = p_dev_state->q_format_shift_factor;

   if(ADSP_FAILED(result = afe_dma_mgr_read(p_dev_state->dma_dev_hdl, (uint8_t *)buffer_ptr)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to read data from the DMA buffer");
	}

   // post processing is required for the cases as below.  bit width and number of channels.
   
   /* Calculate the total number of samples*/
   total_num_samples = (num_dma_samples * num_channels);
   
   /* For Non-I2S interface, Only Q31->Q28 scaling, No channel re-ordering */
   if (p_dev_state->bytes_per_channel == 4)
   {
      /* Calculate the total number of samples*/
      total_num_samples = (num_dma_samples * num_channels);

      ptr_dst_64 = (int64_t *)buffer_ptr;

#ifdef __qdsp6__
      for(samp=total_num_samples; samp >= 2; samp -= 2)
      {
         /* Q31 to Q28 conversion, two samples at at time
         */
         (*ptr_dst_64) = Q6_P_vasrw_PR(*ptr_dst_64, q_format_shift_factor);
          ptr_dst_64++;
      }

      /* If number of samples are odd, scale the remaining sample
      */
      if(samp)
      {
         ptr_pBuf32 = (int32_t *)ptr_dst_64;

         (*ptr_pBuf32) >>= q_format_shift_factor;
      }
#else
      /*-------- Non Q6 Version ------*/

      ptr_pBuf32 = (int32_t *)buffer_ptr;

      for(int32_t i=0; i<total_num_samples; i++)
      {
         ptr_pBuf32[i] = (ptr_pBuf32[i] >> q_format_shift_factor);
      }
#endif /* __qdsp6__ */
   }
   
   return result;

}

/**
  @brief Copy data from client buffer to the tdm DMA buffer

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] buffer_ptr pointer to the client buffer
  @param[in] num_dma_samples number of samples to read from DMA buffer

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_tdm_driver_write(afe_drv_h_t afe_drv, int16_t *buffer_ptr, uint32_t num_dma_samples)
{
   ADSPResult result = ADSP_EOK;
   int16_t           num_channels, samp;
   uint32_t          total_num_samples;
   int32_t           *ptr_pBuf32;
   int64_t           *ptr_dst_64;
   uint16_t        	 q_format_shift_factor;

   tdm_port_state_t *p_dev_state = (tdm_port_state_t *) afe_drv;

   if((NULL == afe_drv) || (NULL == buffer_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to tdm device handle / buffer pointer Null");
      return ADSP_EFAILED;
   }

   // pre processing is required for the cases as below.
   // bit width,  number of channels  
   num_channels = p_dev_state->num_channels;
   q_format_shift_factor = p_dev_state->q_format_shift_factor;

   /* Calculate the total number of samples*/
   total_num_samples = (num_dma_samples * num_channels);
   
   if(4 == p_dev_state->bytes_per_channel) /* For Non-I2S interface, Only Q31->Q28 scaling, No swap */
   {
      ptr_dst_64 = (int64_t *)buffer_ptr;

#ifdef __qdsp6__
      for(samp=total_num_samples; samp >= 2; samp -= 2)
      {
         /* Q31 to Q28 conversion, two samples at at time
         */
         (*ptr_dst_64) = Q6_P_vaslw_PR(*ptr_dst_64, q_format_shift_factor);
          ptr_dst_64++;
      }

      /* If number of samples are odd, scale the remaining sample
      */
      if(samp)
      {
         ptr_pBuf32 = (int32_t *)ptr_dst_64;

         (*ptr_pBuf32) <<= q_format_shift_factor;
      }
#else
      /*-------- Non Q6 Version ------*/

      ptr_pBuf32 = (int32_t *)buffer_ptr;

      for(int32_t i=0; i<total_num_samples; i++)
      {
         ptr_pBuf32[i] = (ptr_pBuf32[i] << q_format_shift_factor);
      }
#endif /* __qdsp6__ */
   }

   
   if(ADSP_FAILED(result = afe_dma_mgr_write(p_dev_state->dma_dev_hdl, (uint8_t *)buffer_ptr)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to read data from the DMA buffer");
   }

   return result;
}



/**
  @brief Routine to read the properites for TDM 

  @param[in] afe_port_ptr pointer to the port structure

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_tdm_get_properties(afe_drv_h_t afe_drv, uint32_t prop_id, int8_t *prop_ptr, uint32_t payload_size)
{
   ADSPResult result = ADSP_EOK;
   tdm_port_state_t *p_dev_state = (tdm_port_state_t *) afe_drv;


   if(NULL == afe_drv)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to tdm device handle Null");
      return ADSP_EFAILED;
   }

   switch(prop_id)
   {
      case AFE_DEVICE_PORT_PROP:
      {
         afe_dev_port_config_t  *tdm_port_cfg_data_ptr = (afe_dev_port_config_t  *)prop_ptr;

         if (payload_size < sizeof(afe_dev_port_config_t))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "TDM get property fail: Invalid payload size: %ld, tdm port index:%d, dir:%d", \
              payload_size, p_dev_state->intf, p_dev_state->direction);
              result = ADSP_EBADPARAM;
         }
        
         result = afe_tdm_get_dev_port_cfg(afe_drv, tdm_port_cfg_data_ptr);
         break;
      }
      case AFE_DEVICE_TIMESTAMP_AND_FIFO_PROP:
      {
         afe_dev_timestamp_info_t  *time_stamp_info_ptr = (afe_dev_timestamp_info_t *) prop_ptr;
      
         if (payload_size < sizeof(afe_dev_timestamp_info_t))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "TDM get property fail: Invalid payload size: %ld, tdm port index:%d, dir:%d", \
              payload_size, p_dev_state->intf, p_dev_state->direction);
              result = ADSP_EBADPARAM;
         }
        
         result = afe_tdm_get_dma_timestamp(afe_drv, time_stamp_info_ptr);
         break;
      }
      case AFE_DEVICE_GROUP_PROP:
      {
         afe_dev_grp_config_info_t  *tdm_group_cfg_data_ptr = (afe_dev_grp_config_info_t  *)prop_ptr;

         if (payload_size < sizeof(afe_dev_grp_config_info_t))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "TDM get property fail: Invalid payload size: %ld, i2s port index:%d, dir:%d",\
                     payload_size, p_dev_state->intf, p_dev_state->direction);
            result = ADSP_EBADPARAM;
         }
         result = afe_dev_group_config_tdm_port(p_dev_state, tdm_group_cfg_data_ptr);

         break;
      }      
      default:
         result = ADSP_EUNSUPPORTED;
         break;
   }
   
   return result;


}   

/**
  @brief Get TDM dev port config to fill the info to the port structure.
        NOTE: This is the new function to remove port structure access inside device driver right after config.
                   Upper layer should call this function to fill the port structure.
   
  @param[in] afe_drv pointer for afe device port structure
  @param[out] tdm_port_cfg_data_ptr pointer to the configuration structure

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_tdm_get_dev_port_cfg (void *afe_drv, afe_dev_port_config_t  *tdm_port_cfg_data_ptr)
{
   ADSPResult result = ADSP_EOK;
   tdm_port_state_t *p_dev_state = (tdm_port_state_t *) afe_drv;

   if(TDM_DRIVER_INIT != tdm_driver.driver_state)
   {
      return ADSP_EFAILED;
   }  
   
   if((NULL == afe_drv) || (NULL == tdm_port_cfg_data_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to tdm device handle / params pointer is null");
      return ADSP_EFAILED;
   }   

   // this part is port structure which we'd better access this in the driver.
   // we can have new function such as get function which provide information related to this.

   // port data sturtcure to be filled.
   tdm_port_cfg_data_ptr->port_data_type = AFE_UNCOMPRESSED_DATA_PORT;
   if(AFE_GENERIC_COMPRESSED_DATA == p_dev_state->data_format)
   {
	   tdm_port_cfg_data_ptr->port_data_type = AFE_GENERIC_COMPRESSED_DATA_PORT;
   }

   tdm_port_cfg_data_ptr->q_format_shift_factor  = p_dev_state->q_format_shift_factor;
   tdm_port_cfg_data_ptr->bytes_per_channel =  p_dev_state->bytes_per_channel;
   tdm_port_cfg_data_ptr->bit_width = p_dev_state->bit_width;
   tdm_port_cfg_data_ptr->channels = p_dev_state->num_channels;
   tdm_port_cfg_data_ptr->sample_rate = p_dev_state->sample_rate;
   tdm_port_cfg_data_ptr->is_interleaved = TRUE;

   return result;
}



/**
  @brief Routine to read the hardware timestamp registers for TDM

  @param[in] afe_port_ptr pointer to the port structure

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_tdm_get_dma_timestamp(void *afe_drv, afe_dev_timestamp_info_t *timestamp_info_ptr)
{
   tdm_port_state_t *p_dev_state = (tdm_port_state_t *) afe_drv;
   uint64_t dma_int_timestamp = 0;
   uint32_t fifo_count = 0;
   ADSPResult result = ADSP_EOK;

   if(NULL == afe_drv)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to tdm device handle Null");
      return ADSP_EFAILED;
   }
  
   if(ADSP_FAILED(result = afe_dma_mgr_read_timestamp_fifo_count(p_dev_state->dma_dev_hdl, &dma_int_timestamp, &fifo_count)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_dma_mgr_read_timestamp_fifo_count failed intf[%d] dir[%d]" 
            ,p_dev_state->intf, p_dev_state->direction);
   }
   else
   {
     timestamp_info_ptr->dma_int_timestamp = avtimer_drv_convert_hw_tick_to_time(dma_int_timestamp);
      
      /* FIFO count is in 32 bit words. Convert it to samples as per port byter per sample */
      timestamp_info_ptr->fifo_samples = fifo_count * sizeof(uint32_t)/p_dev_state->bytes_per_channel;
   }

   return result;
}

/**
  @brief Callback routine for the tdm interrupt

  @param[in] afe_drv pointer to the tdm dev port state structure

  @param[in] event_mask indicates if it is dma interrupt or error.

  @return  none

*/
static void afe_tdm_int_callback(void* afe_drv, uint32 event_mask)
{
   tdm_port_state_t         *dev_state_ptr = (tdm_port_state_t *)afe_drv;;

   if(NULL == afe_drv)
   {
      // critical error
      return;
   }

   if(event_mask & DMA_TYPE2_EVENT_FIFOERROR)
   {
      dev_state_ptr->fifo_ovf_cnt++;
   }

   if(event_mask & DMA_TYPE2_EVENT_BUSERROR)
   {
      dev_state_ptr->ahb_err_cnt++;
   }

   if(event_mask & DMA_TYPE2_EVENT_DATA)
   {
      if(NULL != dev_state_ptr->cb_info.afe_dev_int_cb)
      {
         dev_state_ptr->tdm_intr_cnt++;
         dev_state_ptr->cb_info.afe_dev_int_cb(dev_state_ptr->cb_info.cb_arg);
      }
   }
}

/**
  Copy data from the tdm DMA buffer to the client buffer
  This  function which is called by dma manager 

  @param[in]  dma_mgr_state_ptr         pointer to the DMA manager state information
  @param[in]  src_address				pointer to the DMA buffer
  @param[in]  dst_address				pointer to the client buffer

  @return
  None

  @dependencies
  None.
 */
static void afe_tdm_read_dma(void *afe_driver_context, uint32_t *src_address, uint8_t *dst_address)
{
   tdm_port_state_t *p_dev_state;
   uint32_t length;

   if((NULL == afe_driver_context) || (NULL == src_address) || (NULL == dst_address)  )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to dma handle / buffer addresses Null");
      return;
   }

   p_dev_state = (tdm_port_state_t *)afe_driver_context;

   length = 
     p_dev_state->int_samples_per_period * p_dev_state->bytes_per_channel* p_dev_state->num_channels;

   memscpy(dst_address, length, src_address, length);
   
   return;
}

/**
  Copy data from client buffer to the tdm Output DMA buffer
  write function which is called by dma manager

  @param[in]  dma_mgr_state_ptr         pointer to the DMA manager state information
  @param[in]  src_address                    pointer to the client buffer
  @param[in]  dst_address                    pointer to the DMA buffer

  @return
  None

  @dependencies
  None.
 */

static void afe_tdm_write_dma(void *afe_driver_context, uint8_t *src_address, uint32_t *dst_address)
{
   tdm_port_state_t *p_dev_state;
   uint32_t length;

   if((NULL == afe_driver_context) || (NULL == src_address) || (NULL == dst_address)  )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to dma handle / buffer addresses Null");
      return;
   }
   p_dev_state = (tdm_port_state_t *) afe_driver_context;

   length = p_dev_state->int_samples_per_period * p_dev_state->bytes_per_channel * p_dev_state->num_channels;
   
   memscpy(dst_address, length ,src_address, length);
     
   return;
}


/**
  @brief save tdm port config and error check for the params.

  @param[in] p_dev_state pointer for AFE dev port structure
  @param[in] tdm_cfg_ptr   pointer to the configuration structure

  @return  ADSP_EOK on success, an error code on error
*/
static ADSPResult afe_tdm_dev_config_port (tdm_port_state_t* p_dev_state, afe_param_id_tdm_cfg_t *tdm_cfg_ptr)
{
   ADSPResult result = ADSP_EOK;  
   uint32_t nbits_per_frame;

   // Printing the device configuration information
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"Device configuration TDM Port: version %d", tdm_cfg_ptr->tdm_cfg_minor_version);
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"Number Channel: %d Sample rate: %d bit_width: %d", tdm_cfg_ptr->num_channels, tdm_cfg_ptr->sample_rate, tdm_cfg_ptr->bit_width);
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"Data format: %d Sync mode: %d", tdm_cfg_ptr->data_format, tdm_cfg_ptr->sync_mode);
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"Sync Src: %d nslots_per_frame: %d OE enable: %d", tdm_cfg_ptr->sync_src, tdm_cfg_ptr->nslots_per_frame, tdm_cfg_ptr->ctrl_data_out_enable);
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"Invert sync: %d sync_delay: %d slot_width: %d", tdm_cfg_ptr->ctrl_invert_sync_pulse, tdm_cfg_ptr->ctrl_sync_data_delay, tdm_cfg_ptr->slot_width);
   
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"Slot mask: 0x%x", tdm_cfg_ptr->slot_mask);
   
   // parse the bit width information
   if((16 != tdm_cfg_ptr->bit_width)  
     && (24 != tdm_cfg_ptr->bit_width)
     && (32 != tdm_cfg_ptr->bit_width))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM bit width: %x, tdm intf index: %x", 
       tdm_cfg_ptr->bit_width, p_dev_state->intf);
      return ADSP_EUNSUPPORTED;
   }
   // Validate the support sample rate
   if ((AFE_PORT_SAMPLE_RATE_8K != tdm_cfg_ptr->sample_rate)&&
      (AFE_PORT_SAMPLE_RATE_16K != tdm_cfg_ptr->sample_rate)&&
      (AFE_PORT_SAMPLE_RATE_24K != tdm_cfg_ptr->sample_rate)&&
      (AFE_PORT_SAMPLE_RATE_32K != tdm_cfg_ptr->sample_rate)&&
      (AFE_PORT_SAMPLE_RATE_48K != tdm_cfg_ptr->sample_rate)&&
	  (AFE_PORT_SAMPLE_RATE_96K != tdm_cfg_ptr->sample_rate)&&
	  (AFE_PORT_SAMPLE_RATE_192K != tdm_cfg_ptr->sample_rate)&&
	  (AFE_PORT_SAMPLE_RATE_384K != tdm_cfg_ptr->sample_rate)&&
	  (AFE_PORT_SAMPLE_RATE_11_025K != tdm_cfg_ptr->sample_rate)&&
	  (AFE_PORT_SAMPLE_RATE_22_05K != tdm_cfg_ptr->sample_rate)&&
	  (AFE_PORT_SAMPLE_RATE_44_1K != tdm_cfg_ptr->sample_rate)&&
	  (AFE_PORT_SAMPLE_RATE_88_2K != tdm_cfg_ptr->sample_rate)&&
	  (AFE_PORT_SAMPLE_RATE_176_4K != tdm_cfg_ptr->sample_rate)&&
	  (AFE_PORT_SAMPLE_RATE_352_8K != tdm_cfg_ptr->sample_rate))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM sampling rate: %lu, intf: %x", 
      tdm_cfg_ptr->sample_rate, p_dev_state->intf);
      return ADSP_EUNSUPPORTED;
   }


   // Validate the support slot width
   if((16 != tdm_cfg_ptr->slot_width)
      && (24 != tdm_cfg_ptr->slot_width)
      && (32 != tdm_cfg_ptr->slot_width))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM slot width: %x, intf: %x", 
      tdm_cfg_ptr->slot_width, p_dev_state->intf);
      return ADSP_EUNSUPPORTED;
   }
   // Validate the bit width <= slot width
   if(tdm_cfg_ptr->slot_width < tdm_cfg_ptr->bit_width)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM slot width, bit_witdh: %d, %d, intf: %x", 
      tdm_cfg_ptr->slot_width, tdm_cfg_ptr->bit_width, p_dev_state->intf);
      return ADSP_EUNSUPPORTED;
   }
   // Validate number of channels
     // currently for meember port we support only upto 8 channels. Only in group device configuration case
     // we support upto 16 channels.
     if (tdm_cfg_ptr->num_channels > 16) 
   {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM num channels: %lu, intf: %x", tdm_cfg_ptr->num_channels, p_dev_state->intf);
     return ADSP_EUNSUPPORTED;
   }

   /* At 11.025k sample rate, AFE Underun happens for odd number of channels due to lesser value
    * of DMA period count. So, making odd number of channels combination as unsupported    */
   if((AFE_PORT_SAMPLE_RATE_11_025K == tdm_cfg_ptr->sample_rate) && (tdm_cfg_ptr->num_channels & 0x1))
   {
     MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM num channels:%lu for sample rate %lu, intf: %x", \
           tdm_cfg_ptr->num_channels, tdm_cfg_ptr->sample_rate, p_dev_state->intf);
     return ADSP_EUNSUPPORTED;
   }

   //Parse slot mask
   p_dev_state->slot_mask = tdm_cfg_ptr->slot_mask;
   p_dev_state->num_channels = tdm_cfg_ptr->num_channels;

   /* validate active slot mask with num_channels*/
   if (ADSP_FAILED(afe_tdm_validate_slotmask_num_channels(p_dev_state->slot_mask, p_dev_state->num_channels))) 
   {
     return ADSP_EBADPARAM;
   }
   
   // Validate slots per frame <= 32,  this is a hardware limit.
   if(tdm_cfg_ptr->nslots_per_frame > 32)
   {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM nslots per frame: %lu, intf: %x", tdm_cfg_ptr->nslots_per_frame, p_dev_state->intf);
     return ADSP_EFAILED;
   }
   // Validate bits per frame, this is the hardware limit
   nbits_per_frame = tdm_cfg_ptr->nslots_per_frame * tdm_cfg_ptr->slot_width;
   if(nbits_per_frame > 512)
   {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM nbits per frame: %lu, intf: %x", nbits_per_frame, p_dev_state->intf);
     return ADSP_EFAILED;
   }

   p_dev_state->bit_width = tdm_cfg_ptr->bit_width;  
   p_dev_state->data_format = tdm_cfg_ptr->data_format;
   p_dev_state->sample_rate = tdm_cfg_ptr->sample_rate;   


   // Parse the sync mode : short sync or long sync
    switch(tdm_cfg_ptr->sync_mode)
    {
       case AFE_PORT_TDM_LONG_SYNC_MODE:
         p_dev_state->sync_type = TDM_LONG_SYNC_TYPE;
         break;
       case AFE_PORT_TDM_SHORT_SYNC_SLOT_MODE:
         p_dev_state->sync_type = TDM_SLOT_SYNC_TYPE;
         break;
       case AFE_PORT_TDM_SHORT_SYNC_BIT_MODE:
         p_dev_state->sync_type = TDM_SHORT_SYNC_TYPE;
         break;
       default:
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM sync mode: %lu, intf: %x", tdm_cfg_ptr->sync_mode, p_dev_state->intf);
         return ADSP_EFAILED;
    }
    // Parse the sync src: internal or external
    switch (tdm_cfg_ptr->sync_src)
    {
        case AFE_PORT_TDM_SYNC_SRC_EXTERNAL:
          p_dev_state->msm_mode = TDM_MSM_MODE_SLAVE;
          break;
        case AFE_PORT_TDM_SYNC_SRC_INTERNAL:
          p_dev_state->msm_mode = TDM_MSM_MODE_MASTER;
          break;
        default:
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM sync src: %lu, intf: %x", tdm_cfg_ptr->sync_src, p_dev_state->intf);
          return ADSP_EFAILED;
    }

   // Parse the slot per frame
   p_dev_state->nslots_per_frame = tdm_cfg_ptr->nslots_per_frame;

   // Parse the OE enable flag
   switch(tdm_cfg_ptr->ctrl_data_out_enable)
   {
     case AFE_PORT_TDM_CTRL_DATA_OE_ENABLE:
       p_dev_state->ctrl_data_oe = TDM_CTRL_DATA_OE_ENABLE;
       break;
     case AFE_PORT_TDM_CTRL_DATA_OE_DISABLE:
       p_dev_state->ctrl_data_oe = TDM_CTRL_DATA_OE_DISABLE;
       break;
     default:
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM oe: %lu, intf: %x", tdm_cfg_ptr->ctrl_data_out_enable, p_dev_state->intf);
       return ADSP_EFAILED;	
   }

   // Parse the invert sync pulse flag
   switch(tdm_cfg_ptr->ctrl_invert_sync_pulse)
   {
     case AFE_PORT_TDM_SYNC_INVERT:
      p_dev_state->ctrl_invert_sync_pulse = TDM_LONG_SYNC_INVERT;
      break;
    case AFE_PORT_TDM_SYNC_NORMAL:
      p_dev_state->ctrl_invert_sync_pulse = TDM_LONG_SYNC_NORMAL;
      break;
    default:
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM sync pulse: %lu, intf: %x", tdm_cfg_ptr->ctrl_invert_sync_pulse, p_dev_state->intf);
      return ADSP_EFAILED; 
   }

   // Parse the sync data delay
   switch(tdm_cfg_ptr->ctrl_sync_data_delay)
   {
      case AFE_PORT_TDM_DATA_DELAY_0_BCLK_CYCLE:
        p_dev_state->ctrl_sync_data_delay = TDM_DATA_DELAY_0_CYCLE;
        break;
      case AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE:
        p_dev_state->ctrl_sync_data_delay = TDM_DATA_DELAY_1_CYCLE;
        break;
      case AFE_PORT_TDM_DATA_DELAY_2_BCLK_CYCLE:
        p_dev_state->ctrl_sync_data_delay = TDM_DATA_DELAY_2_CYCLE;
        break;
      default:
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported TDM sync delay: %lu, intf: %x", tdm_cfg_ptr->ctrl_sync_data_delay, p_dev_state->intf);
        return ADSP_EFAILED; 
   }

   // Parse the slot width
   p_dev_state->slot_width = tdm_cfg_ptr->slot_width;

   // This is not exactly true, it is true for 16, 24, 32
   // Odd bitwidth like 9, 10, it will expands to 32 bits.. (Currently, no support)
   p_dev_state->bytes_per_channel = (tdm_cfg_ptr->bit_width > 16) ? 4 : 2;

   /* Determining the qformat shift factor based on bit width. We use this shift factor for 24bit */
   if ((AFE_BITS_PER_SAMPLE_16 == p_dev_state->bit_width) || (AFE_BITS_PER_SAMPLE_32 == p_dev_state->bit_width))
   {
     p_dev_state->q_format_shift_factor = 0;
   }
   else
   {
     p_dev_state->q_format_shift_factor = QFORMAT_SHIFT_FACTOR;
   } 


   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "TDM inf %x shift factor %d", p_dev_state->intf, p_dev_state->q_format_shift_factor);

   return result;
}


/**
  @brief calculate the number of dma buffer.
            The default value is 2 as ping-pong buffer.

  @param[in] p_dev_state pointer for AFE dev port state structure

  @return  the number of buffer (0 in error case)

*/
static uint32 afe_tdm_get_num_dma_buffer(tdm_port_state_t *p_dev_state)
{
   uint32 num_dma_buffer = DEFAULT_TDM_DMA_BUFFERS;
   uint32 dma_buf_size_in_bytes_per_intr; 
 
   dma_buf_size_in_bytes_per_intr = p_dev_state->bytes_per_channel*  p_dev_state->int_samples_per_period *  p_dev_state->num_channels;

   /*twice the length of each buffer - ping-pong*/
   uint32 circ_buf_length = num_dma_buffer * dma_buf_size_in_bytes_per_intr;

   while((circ_buf_length & TDM_DMA_BUFFER_16BYTE_ALIGNMENT) && (num_dma_buffer < MAX_TDM_DMA_BUFFERS))
   {
      //Double the number of DMA buffers
      num_dma_buffer <<= 1 ;

      //Double the DMA buf size
      circ_buf_length =dma_buf_size_in_bytes_per_intr * num_dma_buffer;
    }
    /*The lowest supported fractional sampling rate is 11.025 KHz.
    For 16 byte alignment, number of buffers should never be
    more than MAX_TDM_DMA_BUFFERS*/
   if(num_dma_buffer > MAX_TDM_DMA_BUFFERS)
      {
         num_dma_buffer = 0;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DMA total buffer length (%lu) is not 16 byte aligned", circ_buf_length);
   }
   
   return num_dma_buffer;

}


/**
  @brief calculate the watermark value.
            The default value is 8 dwords.

  @param[in] p_dev_state pointer for AFE dev port state structure

  @return  watermark value which is 1 or 8
*/
static uint32 afe_tdm_get_watermark(tdm_port_state_t *p_dev_state)
{
   uint32 water_mark = DEAFULT_TDM_WATERMARK;
   uint32 dma_buf_size_in_bytes_per_intr; 
   uint32 dma_buf_size_in_dword_per_intr;

   /* check if burst mode can be enabled
       * if per length is not bigger than fifo length or per buf size is not aligned to 16 bytes,
       * then disable the burst mode and use watermark as 1 dword
       */
   dma_buf_size_in_bytes_per_intr = p_dev_state->bytes_per_channel*  p_dev_state->int_samples_per_period *  p_dev_state->num_channels;

   dma_buf_size_in_dword_per_intr = dma_buf_size_in_bytes_per_intr >> 2;

   //8k mono case. The intSamplePeriod will be 8 * 1 / 2 = 4 DWs
   //The burst mode needs to be disabled for this case for latency and drift detection issue
   if((DEAFULT_TDM_WATERMARK >= dma_buf_size_in_dword_per_intr) ||
         (dma_buf_size_in_dword_per_intr & 0x3)  )
   {
      water_mark = 1;
   }

   return water_mark;
}



/**
  @brief Obtain the chip specific information for initializing the driver

  @param[in] tdm_hw_info_ptr pointer for TDM Device config structure

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_tdm_driver_device_cfg_init(tdm_dev_cfg_struct_t * tdm_hw_info_ptr)
{
   DALSYSPropertyVar prop_var;
   lpasshwio_prop_lpaif_struct_t  *lpaif_prop_dev_cfg_ptr = NULL;
   const char *dev_id="LPAIF";
   uint32 i;

   DALSYS_PROPERTY_HANDLE_DECLARE(dal_hdl);

   /* Get device handle */
   if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(dev_id, dal_hdl) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting the DAL device handle for TDM");
      return ADSP_EFAILED;
   }
   /* Read property ptr */
   if( DAL_SUCCESS != DALSYS_GetPropertyValue(dal_hdl, "LpaifPropStructPtr", 0, &prop_var) )
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting TDM Property Struct Ptr");
       return ADSP_EFAILED;
   }
   /* Check that it is supported or not */
   lpaif_prop_dev_cfg_ptr = (lpasshwio_prop_lpaif_struct_t *)prop_var.Val.pStruct;
   if (NOT_SUPPORTED == lpaif_prop_dev_cfg_ptr->tdm_prop.is_supported)
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "TDM interface is not supported");
       return ADSP_EUNSUPPORTED;
   }

   /* hw revision */
   tdm_hw_info_ptr->hw_revision = lpaif_prop_dev_cfg_ptr->hw_revision;

   /* TDM register base address */
   tdm_hw_info_ptr->tdm_reg_base = lpaif_prop_dev_cfg_ptr->lpaif_reg_base;
   /* TDM register page size */
   tdm_hw_info_ptr->tdm_reg_size = lpaif_prop_dev_cfg_ptr->lpaif_reg_size;
   /* Number of tdm interfaces */         
   tdm_hw_info_ptr->inf_type_cnt = lpaif_prop_dev_cfg_ptr->tdm_prop.inf_type_cnt;
   /* TDM register offset */
   tdm_hw_info_ptr->tdm_reg_offset = lpaif_prop_dev_cfg_ptr->tdm_prop.tdm_reg_offset_addr;
   for(i=0; i < MAX_TDM_TYPE_NUM; i++)
   {
      tdm_hw_info_ptr->inf_type_list[i] = lpaif_prop_dev_cfg_ptr->tdm_prop.inf_type_list[i];
      tdm_hw_info_ptr->inf_type_data_line_caps[i] = lpaif_prop_dev_cfg_ptr->tdm_prop.inf_type_data_line_caps[i];
      tdm_hw_info_ptr->inf_type_direction[i] = lpaif_prop_dev_cfg_ptr->tdm_prop.inf_type_direction[i];
   }

   return ADSP_EOK;
}


ADSPResult afe_tdm_validate_slotmask_num_channels(uint32_t slot_mask, uint32_t num_channels)
{
   uint8_t idx,num_active_slots_cnt=0;
   ADSPResult result = ADSP_EOK;

   /*Validate if active slot_mask is set same as number of channels*/
   for (idx=0;idx<MAX_TDM_SLOTS;idx++) 
   {
       if (slot_mask&(0x00000001<<idx)) 
       {
          num_active_slots_cnt++;
       }
   }
   if (num_active_slots_cnt != num_channels ) 
   {
     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Number of channels and active slots are mismatch, num_chs:%lu, slotmask 0x%x", num_channels, slot_mask);
     result = ADSP_EFAILED;
   }

   return result;
}

