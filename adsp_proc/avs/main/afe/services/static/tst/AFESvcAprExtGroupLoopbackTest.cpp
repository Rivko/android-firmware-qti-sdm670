#ifdef AFE_TEST

#define AFE_I2S_QUA_LB1_GROUP_TEST_1                    1  // 16bit
#define AFE_I2S_QUA_LB2_GROUP_TEST_1                    2  // 16bit
#define AFE_I2S_QUA_LB1_GROUP_TEST_2                    3  // 24bit
#define AFE_I2S_QUA_LB2_GROUP_TEST_2                    4  // 24bit


#define AFE_TDM_QUA_LB1_GROUP_TEST_1                    5  // 16bit
#define AFE_TDM_QUA_LB2_GROUP_TEST_1                    6  // 16bit
#define AFE_TDM_QUA_LB1_GROUP_TEST_2                    7  // 24bit
#define AFE_TDM_QUA_LB2_GROUP_TEST_2                    8  // 24bit




#define ADD_CUSTOM_HEADER


#include "qurt_elite.h"
#include "AFEDevService.h"
/* APR/API related */
#include "apr_comdef.h"
#include "apr_event.h"
#include "adsp_afe_service_commands.h"
#include "EliteAprIf.h"
#include "AFETestHandlers.h"
#include "AFEInterface.h"
#include "EliteMsg_AfeCustom.h"

#ifndef SIM
/* GPIO related */
#include "DDITlmm.h"
#endif

// duplicates V2 APR
#define AFE_APR_IBASIC_RSP_RESULT   ( 0x000110E8 )
#define AFE_APR_IBASIC_RSP_ACCEPTED ( 0x000100BE )

static const uint32 my_test_client_port = 0x00002000;
static apr_event_t  my_work_done_event;
static uint32 my_afe_token = 0xF000F000;
static uint32 my_testclient_apr_handle = 0;
static uint16_t my_testclient_apr_addr   = 0;

static uint16 my_afe_addr = 0;

static uint32 my_afe_1st_lb_test_case_in_group = AFE_TDM_QUA_LB1_GROUP_TEST_1;

static uint32 my_afe_2nd_lb_test_case_in_group = AFE_TDM_QUA_LB2_GROUP_TEST_1;

//let's see if it can be replaced by eos...
static bool_t afe_test_stop_bit_comp = FALSE;
 
typedef struct {
   qurt_elite_thread_t          tid;
   int                          thread_result;
   uint16_t                     port_id;
   elite_svc_handle_t           *phAfe;
   volatile const afe_drift_info_t *pAfeDriftPtr;
   volatile uint32_t             unAFEDelay; 
 	qurt_elite_queue_t           *afeClientResponseQ;
  	qurt_elite_queue_t           *bufQ;
	qurt_elite_channel_t         afeClientChannel;
   afe_client_data_path   data_path; 
   elite_svc_handle_t     svc_handle; // only for TX
 	qurt_elite_channel_t   afeTxClientBufChannel; // only for TX

   uint32_t samples_per_int;
   uint32_t bytes_per_channel;
   uint32_t num_channels;
   uint32_t bit_width;

   bool_t   afe_test_pattern_check;  // false by default and it can be set to right after or before the thread is created...
   uint32_t num_check_count;
   uint32_t num_sent_rx_client;
   uint32_t num_received_tx_client;
 
} afe_test_client_t;


afe_test_client_t afe_rx_client1, afe_tx_client1;
afe_test_client_t afe_rx_client2, afe_tx_client2;



/* Duplicates the aprv2_ibasic_rsp_result_t */
typedef struct {
   uint32   opcode;
   uint32   status;
} afe_apr_ibasic_rsp_result_t;

//static int16_t afeExit = FALSE;

/* Duplicates the aprv2_ibasic_evt_accepted_t */
typedef struct {
   uint32   opcode;
   uint32   status;
} afe_apr_ibasic_evt_accepted_t;

typedef struct {
   afe_port_cmd_set_param_v2_t afeSetcmd;
   afe_port_param_data_v2_t afeParamHeader;
   union {
       afe_param_id_i2s_cfg_t i2scfg;
       afe_param_id_pcm_cfg_t pcmCfg;
       afe_param_id_tdm_cfg_t tdmCfg;
       afe_param_id_slot_mapping_cfg_t  slotcfg;
  };
} afeSetCmdInfo_t;

typedef struct {
	afe_port_cmd_set_param_v2_t afeSetcmd;
	afe_port_param_data_v2_t afeParamHeader;
	afe_param_id_custom_tdm_header_cfg_t tdmCustomHeadersCfg;
	uint16_t header[8];
} afeSetCustomHeaderCmdInfo_t;

typedef struct {
   afe_svc_cmd_set_param_t afeSvcSetcmd;
   afe_svc_param_data_t    afeSvcParamHeader;
   union {
       afe_param_id_clock_set_t clkcfg;
       afe_param_id_group_device_enable_t group_enable;
       afe_param_id_group_device_i2s_cfg_v1_t i2s_group_cfg;
       afe_param_id_group_device_tdm_cfg_t tdm_group_cfg;
  };
} afeSvcSetCmdInfo_t;


static int32 testclient_isr_recv_fn ( elite_apr_packet_t *packet, void* dispatch_data )
{
   int32_t rc;
   uint32_t token;

   if ( packet == NULL )
   {
      return APR_EBADPARAM;
   }

   switch ( elite_apr_if_get_opcode( packet) )
   {
      case AFE_APR_IBASIC_RSP_ACCEPTED:
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Test client ISR received APR_IBASIC_RSP_ACCEPTED");
         rc = elite_apr_if_free( my_testclient_apr_handle, packet );
         break;

      case AFE_APR_IBASIC_RSP_RESULT:
         {
            afe_apr_ibasic_rsp_result_t   *pRsp = (afe_apr_ibasic_rsp_result_t *) elite_apr_if_get_payload_ptr( packet);

            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Test client ISR received APR_IBASIC_RSP_RESULT, status(%lu),opcode(%lu)",pRsp->status,pRsp->opcode);

            if( pRsp->status != APR_EOK)
            {
                MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"APR_IBASIC_RSP_RESULT failed, status(%lu),clientToken(%lu)",pRsp->status,
                      elite_apr_if_get_client_token(packet));
            }

            token = elite_apr_if_get_client_token(packet);

            rc = apr_event_signal( my_work_done_event );
            
            rc = elite_apr_if_free( my_testclient_apr_handle, packet );
         }
         break;
      case AFE_PORT_CUSTOM_EVENT:
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_PORT_CUSTOM_EVENT received");

            uint32_t payloadSize = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE(packet->header);
            void *payload = APRV2_PKT_GET_PAYLOAD(void, packet);

            afe_port_custom_event_t *p_evt = (afe_port_custom_event_t *) payload;
            MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_PORT_CUSTOM_EVENT received, port id(%lx),module id(%lx), param_id (%lx), event (%x), payloadsize(%lu)", \
               p_evt->port_id, p_evt->module_id, p_evt->param_id, p_evt->event_type, payloadSize);

            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Error unknown event (%x)", p_evt->event_type);

               
            rc = elite_apr_if_free( my_testclient_apr_handle, packet );
         }
      break;
      default:
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Test client ISR received unknown ID: %lu",elite_apr_if_get_opcode(packet));
         return APR_EUNSUPPORTED;
   }

   return APR_EOK;
}

static int32 getTestAfePortId(uint32_t testcase, uint16_t *pTx_port_id, uint16_t *pRx_port_id)
{
   int32 rc = ADSP_EOK;

   switch(testcase)
   {

      // QUAD TDM test cases
      case AFE_TDM_QUA_LB1_GROUP_TEST_1:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_QUA_LB1_GROUP_TEST_1");
         *pTx_port_id = AFE_PORT_ID_QUATERNARY_TDM_TX;
         *pRx_port_id = AFE_PORT_ID_QUATERNARY_TDM_RX;
      }
      break;    
      case AFE_TDM_QUA_LB2_GROUP_TEST_1:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_QUA_LB2_GROUP_TEST_1");
         *pTx_port_id = AFE_PORT_ID_QUATERNARY_TDM_TX_1;
         *pRx_port_id = AFE_PORT_ID_QUATERNARY_TDM_RX_1;
      }
      break;  
      case AFE_TDM_QUA_LB1_GROUP_TEST_2:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_QUA_LB1_GROUP_TEST_2");
         *pTx_port_id = AFE_PORT_ID_QUATERNARY_TDM_TX;
         *pRx_port_id = AFE_PORT_ID_QUATERNARY_TDM_RX;
      }
      break;    
      case AFE_TDM_QUA_LB2_GROUP_TEST_2:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_QUA_LB2_GROUP_TEST_2");
         *pTx_port_id = AFE_PORT_ID_QUATERNARY_TDM_TX_1;
         *pRx_port_id = AFE_PORT_ID_QUATERNARY_TDM_RX_1;
      }
      break;   
      // QUAD I2S test cases
      case AFE_I2S_QUA_LB1_GROUP_TEST_1:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_I2S_QUA_LB1_GROUP_TEST_1");
         *pTx_port_id = AFE_PORT_ID_QUATERNARY_MI2S_TX;
         *pRx_port_id = AFE_PORT_ID_QUATERNARY_MI2S_RX;
      }
      break;    
      case AFE_I2S_QUA_LB2_GROUP_TEST_1:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_I2S_QUA_LB2_GROUP_TEST_1");
         *pTx_port_id = AFE_PORT_ID_QUATERNARY_MI2S_TX_1;
         *pRx_port_id = AFE_PORT_ID_QUATERNARY_MI2S_RX_1;
      }
      break;  
      case AFE_I2S_QUA_LB1_GROUP_TEST_2:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_I2S_QUA_LB1_GROUP_TEST_2");
         *pTx_port_id = AFE_PORT_ID_QUATERNARY_MI2S_TX;
         *pRx_port_id = AFE_PORT_ID_QUATERNARY_MI2S_RX;
      }
      break;    
      case AFE_I2S_QUA_LB2_GROUP_TEST_2:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_I2S_QUA_LB2_GROUP_TEST_2");
         *pTx_port_id = AFE_PORT_ID_QUATERNARY_MI2S_TX_1;
         *pRx_port_id = AFE_PORT_ID_QUATERNARY_MI2S_RX_1;
      }
      break;   
      default:
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "### Error Unknown AFE TEST CASE ###");
         rc = ADSP_EFAILED;
      }
   }
   
   return rc;
}

static int32 getTestClkAfe(uint32_t testcase, uint16_t port_id, uint32_t *p_clock_id, uint32_t *p_clk_value)
{
   int32 rc = ADSP_EOK;

   switch(testcase)
   {
   	
      // QUAD TDM
      case AFE_TDM_QUA_LB1_GROUP_TEST_1:
      case AFE_TDM_QUA_LB2_GROUP_TEST_1:
      {   
         *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_QUAD_TDM_IBIT;
         *p_clk_value = 12288000; //The raw bit clok is 48000 x 8 x 32 = 12288000 Hz and frame size is 32bytes
      }
      break;   
      case AFE_TDM_QUA_LB1_GROUP_TEST_2:
      case AFE_TDM_QUA_LB2_GROUP_TEST_2:
      {   
         *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_QUAD_TDM_IBIT;
         *p_clk_value = 12288000;
      }
      break;         
      // QUAD I2S 
      case AFE_I2S_QUA_LB1_GROUP_TEST_1:
      case AFE_I2S_QUA_LB2_GROUP_TEST_1:
      {   
         *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT;
         *p_clk_value = 1536000;
      }
      break;   
      case AFE_I2S_QUA_LB1_GROUP_TEST_2:
      case AFE_I2S_QUA_LB2_GROUP_TEST_2:
      {   
         *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT;
         *p_clk_value = 3072000;
      }
      break;         
      default:
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "### Error Unknown AFE TEST CASE ###");
         rc = ADSP_EFAILED;
      }
   }
   
   return rc;
}


static int32 getTestAfeTxPortConfig(uint32_t testcase, afeSetCmdInfo_t *pAfePortCfgCmd, afe_test_client_t *pTxClient)
{
   int32 rc = ADSP_EOK;

   switch(testcase)
   {
      // QUAD TDM
      case AFE_TDM_QUA_LB1_GROUP_TEST_1:
      case AFE_TDM_QUA_LB2_GROUP_TEST_1:        
      {
         pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
         pAfePortCfgCmd->tdmCfg.num_channels = 2;
         pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_48K;
         pAfePortCfgCmd->tdmCfg.bit_width = 16;
         pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_SLOT_MODE; 
         pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->tdmCfg.nslots_per_frame = 2; // this is confusing, it may be better to be 2 or 16... NOT 8
         pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_TDM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
         pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
         pAfePortCfgCmd->tdmCfg.slot_width = 16;
         pAfePortCfgCmd->tdmCfg.slot_mask = 3;
         
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
      
         pTxClient->samples_per_int = 48;
         pTxClient->bytes_per_channel = 2;
         pTxClient->num_channels = 2;
         pTxClient->bit_width = 16;
      }      
      break;
      case AFE_TDM_QUA_LB1_GROUP_TEST_2:
      case AFE_TDM_QUA_LB2_GROUP_TEST_2:        
      {
         pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
         pAfePortCfgCmd->tdmCfg.num_channels = 2;
         pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_48K;
         pAfePortCfgCmd->tdmCfg.bit_width = 24;
         pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_SLOT_MODE; 
         pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->tdmCfg.nslots_per_frame = 2; // this is confusing, it may be better to be 2 or 16... NOT 8
         pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_TDM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
         pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
         pAfePortCfgCmd->tdmCfg.slot_width = 24;
         pAfePortCfgCmd->tdmCfg.slot_mask = 3;
         
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
      
         pTxClient->samples_per_int = 48;
         pTxClient->bytes_per_channel = 4;
         pTxClient->num_channels = 2;
         pTxClient->bit_width = 24;
      }
      break;
      // QUAD I2S
      case AFE_I2S_QUA_LB1_GROUP_TEST_1:
      {
         pAfePortCfgCmd->i2scfg.i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
         pAfePortCfgCmd->i2scfg.ws_src = AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL;
         pAfePortCfgCmd->i2scfg.bit_width = 16;
         pAfePortCfgCmd->i2scfg.channel_mode = AFE_PORT_I2S_SD2;
         pAfePortCfgCmd->i2scfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->i2scfg.mono_stereo = AFE_PORT_I2S_STEREO;   //AFE_PORT_I2S_MONO;
         pAfePortCfgCmd->i2scfg.sample_rate = 48000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_I2S_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->i2scfg);
         //
         pTxClient->samples_per_int = 48;
         pTxClient->bytes_per_channel = 2;
         pTxClient->num_channels = 2;
         pTxClient->bit_width = 16;
      }      
      break;         
      case AFE_I2S_QUA_LB2_GROUP_TEST_1:        
      {
         pAfePortCfgCmd->i2scfg.i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
         pAfePortCfgCmd->i2scfg.ws_src = AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL;
         pAfePortCfgCmd->i2scfg.bit_width = 16;
         pAfePortCfgCmd->i2scfg.channel_mode = AFE_PORT_I2S_SD3;
         pAfePortCfgCmd->i2scfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->i2scfg.mono_stereo = AFE_PORT_I2S_STEREO;   //AFE_PORT_I2S_MONO;
         pAfePortCfgCmd->i2scfg.sample_rate = 48000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_I2S_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->i2scfg);
         //
         pTxClient->samples_per_int = 48;
         pTxClient->bytes_per_channel = 2;
         pTxClient->num_channels = 2;
         pTxClient->bit_width = 16;
      }      
      break;
      case AFE_I2S_QUA_LB1_GROUP_TEST_2:
      {
         pAfePortCfgCmd->i2scfg.i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
         pAfePortCfgCmd->i2scfg.ws_src = AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL;
         pAfePortCfgCmd->i2scfg.bit_width = 24;
         pAfePortCfgCmd->i2scfg.channel_mode = AFE_PORT_I2S_SD2;
         pAfePortCfgCmd->i2scfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->i2scfg.mono_stereo = AFE_PORT_I2S_STEREO;   //AFE_PORT_I2S_MONO;
         pAfePortCfgCmd->i2scfg.sample_rate = 48000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_I2S_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->i2scfg);
         //
         pTxClient->samples_per_int = 48;
         pTxClient->bytes_per_channel = 4;
         pTxClient->num_channels = 2;
         pTxClient->bit_width = 24;
       
      }
      break;
      case AFE_I2S_QUA_LB2_GROUP_TEST_2:        
      {
         pAfePortCfgCmd->i2scfg.i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
         pAfePortCfgCmd->i2scfg.ws_src = AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL;
         pAfePortCfgCmd->i2scfg.bit_width = 24;
         pAfePortCfgCmd->i2scfg.channel_mode = AFE_PORT_I2S_SD3;
         pAfePortCfgCmd->i2scfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->i2scfg.mono_stereo = AFE_PORT_I2S_STEREO;   //AFE_PORT_I2S_MONO;
         pAfePortCfgCmd->i2scfg.sample_rate = 48000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_I2S_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->i2scfg);
         //
         pTxClient->samples_per_int = 48;
         pTxClient->bytes_per_channel = 4;
         pTxClient->num_channels = 2;
         pTxClient->bit_width = 24;
       
      }
      break;
      default:
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "### Error Unknown AFE TEST CASE ###");
         rc = ADSP_EFAILED;
      }
   }

   return rc;
}

static int32 getTestAfeRxPortConfig(uint32_t testcase, afeSetCmdInfo_t *pAfePortCfgCmd, afe_test_client_t *pRxClient)
{
   int32 rc = ADSP_EOK;

   switch(testcase)
   {
      // QUAD TDM
      case AFE_TDM_QUA_LB1_GROUP_TEST_1:
      case AFE_TDM_QUA_LB2_GROUP_TEST_1:
      {
         pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
         pAfePortCfgCmd->tdmCfg.num_channels = 2;
         pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_48K;
         pAfePortCfgCmd->tdmCfg.bit_width = 16;
         pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_SLOT_MODE; 
         pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->tdmCfg.nslots_per_frame = 2; // this is confusing, it may be better to be 2 or 16... NOT 8
         pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_TDM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
         pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
         pAfePortCfgCmd->tdmCfg.slot_width = 16;
         pAfePortCfgCmd->tdmCfg.slot_mask = 3;
         
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
      
         pRxClient->samples_per_int = 48;
         pRxClient->bytes_per_channel = 2;
         pRxClient->num_channels = 2;
         pRxClient->bit_width = 16;
      }      
      break;         
      case AFE_TDM_QUA_LB1_GROUP_TEST_2:
      case AFE_TDM_QUA_LB2_GROUP_TEST_2:      
      {
         pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
         pAfePortCfgCmd->tdmCfg.num_channels = 2;
         pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_48K;
         pAfePortCfgCmd->tdmCfg.bit_width = 24;
         pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_SLOT_MODE; 
         pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->tdmCfg.nslots_per_frame = 2; // this is confusing, it may be better to be 2 or 16... NOT 8
         pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_TDM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
         pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
         pAfePortCfgCmd->tdmCfg.slot_width = 24;
         pAfePortCfgCmd->tdmCfg.slot_mask = 3;
         
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
      
         pRxClient->samples_per_int = 48;
         pRxClient->bytes_per_channel = 4;
         pRxClient->num_channels = 2;
         pRxClient->bit_width = 24;
      }      
      break;
      case AFE_I2S_QUA_LB1_GROUP_TEST_1: 
      {           
         pAfePortCfgCmd->i2scfg.i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
         pAfePortCfgCmd->i2scfg.ws_src = AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL;
         pAfePortCfgCmd->i2scfg.bit_width = 16;
         pAfePortCfgCmd->i2scfg.channel_mode = AFE_PORT_I2S_SD0;
         pAfePortCfgCmd->i2scfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->i2scfg.mono_stereo = AFE_PORT_I2S_STEREO;   //AFE_PORT_I2S_MONO;
         pAfePortCfgCmd->i2scfg.sample_rate = 48000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_I2S_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->i2scfg);
      
         pRxClient->samples_per_int = 48;
         pRxClient->bytes_per_channel = 2;
         pRxClient->num_channels = 2;
         pRxClient->bit_width = 16;
      }
      break;
      case AFE_I2S_QUA_LB2_GROUP_TEST_1: 
      {           
         pAfePortCfgCmd->i2scfg.i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
         pAfePortCfgCmd->i2scfg.ws_src = AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL;
         pAfePortCfgCmd->i2scfg.bit_width = 16;
         pAfePortCfgCmd->i2scfg.channel_mode = AFE_PORT_I2S_SD1;
         pAfePortCfgCmd->i2scfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->i2scfg.mono_stereo = AFE_PORT_I2S_STEREO;   //AFE_PORT_I2S_MONO;
         pAfePortCfgCmd->i2scfg.sample_rate = 48000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_I2S_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->i2scfg);
      
         pRxClient->samples_per_int = 48;
         pRxClient->bytes_per_channel = 2;
         pRxClient->num_channels = 2;
         pRxClient->bit_width = 16;
      }
      break;
      case AFE_I2S_QUA_LB1_GROUP_TEST_2: 
      {           
         pAfePortCfgCmd->i2scfg.i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
         pAfePortCfgCmd->i2scfg.ws_src = AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL;
         pAfePortCfgCmd->i2scfg.bit_width = 24;
         pAfePortCfgCmd->i2scfg.channel_mode = AFE_PORT_I2S_SD0;
         pAfePortCfgCmd->i2scfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->i2scfg.mono_stereo = AFE_PORT_I2S_STEREO;   //AFE_PORT_I2S_MONO;
         pAfePortCfgCmd->i2scfg.sample_rate = 48000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_I2S_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->i2scfg);
      
         pRxClient->samples_per_int = 48;
         pRxClient->bytes_per_channel = 4;
         pRxClient->num_channels = 2;
         pRxClient->bit_width = 24;
      }
      break;
      case AFE_I2S_QUA_LB2_GROUP_TEST_2: 
      {           
         pAfePortCfgCmd->i2scfg.i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
         pAfePortCfgCmd->i2scfg.ws_src = AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL;
         pAfePortCfgCmd->i2scfg.bit_width = 24;
         pAfePortCfgCmd->i2scfg.channel_mode = AFE_PORT_I2S_SD1;
         pAfePortCfgCmd->i2scfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->i2scfg.mono_stereo = AFE_PORT_I2S_STEREO;   //AFE_PORT_I2S_MONO;
         pAfePortCfgCmd->i2scfg.sample_rate = 48000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_I2S_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->i2scfg);
      
         pRxClient->samples_per_int = 48;
         pRxClient->bytes_per_channel = 4;
         pRxClient->num_channels = 2;
         pRxClient->bit_width = 24;
      }
      break;
      default:
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "### Error Unknown AFE TEST CASE ###");
         rc = ADSP_EFAILED;
      }
   }
   
   return rc;
}

// Initialize the source port service handler
static ADSPResult afe_test_create_svc_handle(afe_test_client_t *pClient)
{
   ADSPResult                          result = ADSP_EOK;;
   char                             *queue_buf_ptr;
   char                             afeTestDataQName[16];

	//Create AFE Test response queues
   memset(afeTestDataQName, 0, sizeof(afeTestDataQName));
   qurt_elite_strl_cpy(afeTestDataQName,"afeDataQ0000", sizeof(afeTestDataQName));
   afeTestDataQName[8] = pClient->port_id+ '0';

   queue_buf_ptr = (char*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(2),QURT_ELITE_HEAP_DEFAULT);
   if (NULL == queue_buf_ptr)
   {
     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] AFE Test data Q Creation Failed", pClient->port_id);
     return ADSP_ENOMEMORY;
   }
   
	pClient->svc_handle.dataQ =  (qurt_elite_queue_t *)(queue_buf_ptr);

   if (ADSP_FAILED(result = qurt_elite_queue_init(afeTestDataQName, 2, pClient->svc_handle.dataQ)))
   {
		MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] AFE Test: Error: Failed to create AFE test dataQ", pClient->port_id);
		return result;
	}

  	qurt_elite_channel_init(&pClient->afeTxClientBufChannel); 

   if (ADSP_FAILED(result = qurt_elite_channel_addq(&(pClient->afeTxClientBufChannel), pClient->svc_handle.dataQ, 0))) 
   {
		MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] AFE Test: Error: Failed to add AFE test dataQ to channel", pClient->port_id);
		return result;
	}


   pClient->svc_handle.unSvcId  = ELITE_MIXER_SVCID;
   pClient->svc_handle.threadId = 0;
   pClient->svc_handle.cmdQ = NULL; // this is required for avt drift update

   return result;
}

// Initialize the source port service handler
static ADSPResult afe_test_free_resources(afe_test_client_t *pClient)
{

   if(pClient->svc_handle.dataQ)
   {
      qurt_elite_queue_deinit(pClient->svc_handle.dataQ);
      qurt_elite_channel_destroy(&pClient->afeTxClientBufChannel); 
      qurt_elite_memory_free((void*)pClient->svc_handle.dataQ);
   }

   if(pClient->bufQ)
   {
      /* Free up all the buffers and de-init the output buffer queue */
      elite_svc_deinit_buf_queue(pClient->bufQ, 2);
               
      //qurt_elite_queue_deinit(pClient->bufQ);
      qurt_elite_memory_free((void*)pClient->bufQ);
   }

   if(pClient->afeClientResponseQ)
   {
      qurt_elite_queue_deinit(pClient->afeClientResponseQ);
      qurt_elite_memory_free((void*)pClient->afeClientResponseQ);
      qurt_elite_channel_destroy(&pClient->afeClientChannel); 
   }

   return ADSP_EOK;
}


static ADSPResult Afe_Test_SendCmdConnectAFE(afe_test_client_t *pClient)
{

   elite_msg_custom_afe_connect_t      *pConnectMsgPayload;
   qurt_elite_bufmgr_node_t            bufMgrNode;
   ADSPResult                          result;
   qurt_elite_queue_t                  *afeCmdQ;
   elite_msg_any_t                     msg;
   int                                 nActualSize;
   char                             *queue_buf_ptr;
   char                             responseQName[16];

	//Create AFE Test response queues
   memset(responseQName, 0, sizeof(responseQName));
   qurt_elite_strl_cpy(responseQName,"afeRepQ0000", sizeof(responseQName));
   responseQName[7] = pClient->port_id+ '0';

   queue_buf_ptr = (char*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(2),QURT_ELITE_HEAP_DEFAULT);
   if (NULL == queue_buf_ptr)
   {
     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] AFE Test response Creation Failed", pClient->port_id);
     return ADSP_ENOMEMORY;
   }
   
	pClient->afeClientResponseQ =  (qurt_elite_queue_t *)(queue_buf_ptr);
   if(ADSP_FAILED(result = qurt_elite_queue_init(responseQName, 2, pClient->afeClientResponseQ)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] AFE Test: Error: Failed to create AFE test respQ", pClient->port_id);
		return result;
	}

  	qurt_elite_channel_init(&pClient->afeClientChannel); 

	if (ADSP_FAILED(result = qurt_elite_channel_addq(&pClient->afeClientChannel, pClient->afeClientResponseQ, 0)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] ADM: Error: Failed to add responseQ to channel, result = %d", pClient->port_id, result);
		return(result);
	}


   if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(elite_msg_custom_afe_connect_t), &bufMgrNode, &nActualSize)))
   {
      MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "[port id:0x%x] AFE Test: Failed to create mesage", pClient->port_id);
      return result;
   }

   pConnectMsgPayload = (elite_msg_custom_afe_connect_t*) bufMgrNode.pBuffer;

   memset(&pConnectMsgPayload->afe_client, 0, sizeof(afe_client_t));
   pConnectMsgPayload->afe_client.sample_rate = pClient->samples_per_int*1000; //sample_rate;
   pConnectMsgPayload->afe_client.data_path   = pClient->data_path; //AFE_RX_AUD_MIXER_IN; //data_path;
   pConnectMsgPayload->afe_client.cust_proc.subscribe_to_avt_drift = FALSE; //bShouldSubscribeToAVDrift;
   pConnectMsgPayload->afe_client.channel = pClient->num_channels;//unNumChannels;
   pConnectMsgPayload->afe_client.buf_size =  pClient->samples_per_int*5; // 240 ADM_SMPLS_PER_MS_48K * unFrameSizeInMs;
   pConnectMsgPayload->afe_client.num_buffers =  2; // num_buffers;
   pConnectMsgPayload->afe_client.bytes_per_channel = pClient->bytes_per_channel; //bytes_per_sample;
   pConnectMsgPayload->afe_client.bit_width = pClient->bit_width;
   pConnectMsgPayload->afe_client.interleaved = 0;

   // it is required only for afe_port_send_avt_drift_update if it is rx client.
   // for TX client, it is required to send buffer, media type, EOS, avt update, etc....
   if(pClient->port_id & 1)
   {
      pConnectMsgPayload->afe_client.afe_client_handle =  &pClient->svc_handle; //NULL; // can be null for direct connect use case ;//pCoppHandle;  david
   }
   else
   {
      pConnectMsgPayload->afe_client.afe_client_handle =  NULL; 
   }
   pConnectMsgPayload->afe_client.afe_dp_delay_ptr =   &pClient->unAFEDelay; //david

   // two channel
   pConnectMsgPayload->afe_client.channel_mapping[0] = 1;
   pConnectMsgPayload->afe_client.channel_mapping[1] = 2;

   pConnectMsgPayload->afe_id = pClient->port_id;
   pConnectMsgPayload->responseq_ptr = pClient->afeClientResponseQ;  //david
   pConnectMsgPayload->client_token = 0;
   pConnectMsgPayload->buffer_returnq_ptr = bufMgrNode.pReturnQ;
   pConnectMsgPayload->sec_op_code = ELITEMSG_CUSTOM_AFECONNECT_REQ; 

   msg.unOpCode = ELITE_CUSTOM_MSG;
   msg.pPayload = (void*) pConnectMsgPayload;

   if (qurt_elite_globalstate.pAfeStatSvcCmdQ == NULL)
   {
      result = ADSP_EFAILED;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] AFE Test: Failed to get AFE cmdQ ptr", pClient->port_id);
      goto __bailout;

   }

   afeCmdQ = qurt_elite_globalstate.pAfeStatSvcCmdQ;
   if (ADSP_FAILED(result = qurt_elite_queue_push_back(afeCmdQ, (uint64_t*)&msg)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] AFE Test: Failed to push cmd to AFE", pClient->port_id);
      goto __bailout;
   }

   if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] AFE Test: Wait for ack failed", pClient->port_id);
      goto __bailout;
   }

   result = (ADSPResult)pConnectMsgPayload->response_result;
   if (ADSP_EOK != result)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] AFE Test: Error [0x%x] in cmd ack", pClient->port_id, result);
      goto __bailout;
   }

   pConnectMsgPayload = (elite_msg_custom_afe_connect_t *) msg.pPayload;
   pClient->phAfe = pConnectMsgPayload->svc_handle_ptr;
   pClient->pAfeDriftPtr = pConnectMsgPayload->afe_drift_ptr;

__bailout:
   elite_msg_release_msg(&msg);
   return result;
}

static ADSPResult Afe_Test_SendCmdDisconnectAFE(afe_test_client_t *pClient)
{
    qurt_elite_bufmgr_node_t         bufMgrNode;
    elite_msg_any_t               msg;
    elite_msg_custom_afe_connect_t  *pConnectMsgPayload;
    ADSPResult                    result;
    qurt_elite_queue_t              *afeCmdQ;
    int                          nActualSize;
    if (ADSP_FAILED(result = elite_mem_get_buffer(sizeof(elite_msg_custom_afe_connect_t), &bufMgrNode, &nActualSize)))
    {
       MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "[port id:0x%x] AFE Test: Failed to create mesage", pClient->port_id);
       return result;
    }
    pConnectMsgPayload = (elite_msg_custom_afe_connect_t*) bufMgrNode.pBuffer;
    pConnectMsgPayload->afe_id = pClient->port_id;
    pConnectMsgPayload->responseq_ptr =pClient->afeClientResponseQ;
    pConnectMsgPayload->client_token = 0;
    pConnectMsgPayload->buffer_returnq_ptr = bufMgrNode.pReturnQ;
    pConnectMsgPayload->sec_op_code =ELITEMSG_CUSTOM_AFEDISCONNECT_REQ;
    pConnectMsgPayload->svc_handle_ptr =  pClient->phAfe;

    msg.unOpCode = ELITE_CUSTOM_MSG;
    msg.pPayload = (void*) pConnectMsgPayload;
    if (qurt_elite_globalstate.pAfeStatSvcCmdQ == NULL)
    {
       result = ADSP_EFAILED;
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] AFE Test: Failed to get AFE cmdQ ptr", pClient->port_id);
       goto __bailoutCmdDisconnectAFE;
    }
    afeCmdQ = qurt_elite_globalstate.pAfeStatSvcCmdQ;
    if (ADSP_FAILED(result = qurt_elite_queue_push_back(afeCmdQ, (uint64_t*)&msg)))
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] AFE Test: Failed to push cmd to AFE", pClient->port_id);
       goto __bailoutCmdDisconnectAFE;
    }

    if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] AFE Test: Wait for ack failed", pClient->port_id);
       goto __bailoutCmdDisconnectAFE;
    }

    result = (ADSPResult)pConnectMsgPayload->response_result;
    if (ADSP_EOK != result)
    {
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] AFE Test: Error [0x%x] in cmd ack", pClient->port_id, result);
    }
__bailoutCmdDisconnectAFE:
    elite_msg_release_msg(&msg);
    return result;
}

static ADSPResult afe_test_port_send_media_t(afe_test_client_t *pClient)
{
   qurt_elite_bufmgr_node_t              bufMgrNode;
   int                                 	nActualSize;
   elite_msg_data_media_type_apr_t       *pMediaFormatData;
   elite_msg_any_t                      	msg;
   ADSPResult                           	result = ADSP_EOK;
   uint32_t        						      unMediaFormatID;

   // send regulat PCM format for all clients except audio
   unMediaFormatID = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;

   if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_data_media_type_apr_t) + \
                                     sizeof(elite_multi_channel_pcm_fmt_blk_t), &bufMgrNode, &nActualSize)))
   {
   return ADSP_ENEEDMORE;
   }

   pMediaFormatData = (elite_msg_data_media_type_apr_t*) bufMgrNode.pBuffer;
   elite_multi_channel_pcm_fmt_blk_t *pMultiChMediaFormatBlk = (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pMediaFormatData);

   //Update the media format in the pClientInfo.
   pMultiChMediaFormatBlk->sample_rate = pClient->samples_per_int*1000; // 48000; //pClientInfo->afe_client.sample_rate;
   pMultiChMediaFormatBlk->num_channels = pClient->num_channels; //2; //(uint16_t) (pClientInfo->afe_client.channel);
   pMultiChMediaFormatBlk->bits_per_sample = pClient->bytes_per_channel << 3; //16 or 32  (uint16_t) ((pClientInfo->afe_client.bytes_per_channel)<<3);
   pMultiChMediaFormatBlk->is_interleaved = 0 ;//(uint16_t) (pClientInfo->afe_client.interleaved);
   pMultiChMediaFormatBlk->is_signed = 1;

   pMultiChMediaFormatBlk->channel_mapping[0] = 1;
   pMultiChMediaFormatBlk->channel_mapping[1] = 2;

   pMediaFormatData->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;
   pMediaFormatData->unMediaFormatID = unMediaFormatID;

   pMediaFormatData->pResponseQ = NULL;
   pMediaFormatData->unClientToken = 0;
   pMediaFormatData->pBufferReturnQ = bufMgrNode.pReturnQ;

   msg.unOpCode = ELITE_DATA_MEDIA_TYPE;
   msg.pPayload = (void*) pMediaFormatData;

   if (ADSP_EOK != (result = qurt_elite_queue_push_back(pClient->phAfe->dataQ, (uint64_t*)&msg)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] FAILED to connect services!!!",  pClient->port_id);
      elite_msg_return_payload_buffer(&msg);
      return result;
   }

   return result;
}

static ADSPResult afe_ext_generate_eos(afe_test_client_t *pClient)
{
   ADSPResult                             result = ADSP_EOK;
   qurt_elite_bufmgr_node_t               buf_mgr_node;
   elite_msg_data_eos_header_t            *eos_msg;
   elite_msg_any_t                        eos_data_msg;
   int                                    actual_size;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] Sending EoS", pClient->port_id);

   /* Send the EoS */
   if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_data_eos_header_t), &buf_mgr_node, &actual_size)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] Failed to get buffer mgr node while sending EoS.",  pClient->port_id);
      return result;
   }

   eos_msg                    = (elite_msg_data_eos_header_t*) buf_mgr_node.pBuffer;
   eos_msg->pBufferReturnQ    = buf_mgr_node.pReturnQ;
   eos_msg->pResponseQ        = NULL;
   eos_msg->unClientToken     = 0;
   eos_msg->unEosFormat       = ELITEMSG_DATA_EOS_APR;
   eos_msg->unResponseResult  = 0;

   eos_data_msg.unOpCode      = ELITE_DATA_EOS;
   eos_data_msg.pPayload      = (void*) eos_msg;

   result = qurt_elite_queue_push_back(pClient->phAfe->dataQ, (uint64_t*) &eos_data_msg);

   if(ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] Failed to deliver EoS dowstream. Dropping",  pClient->port_id);
      elite_msg_return_payload_buffer(&eos_data_msg);
   }

   return result;
}


static ADSPResult afe_test_port_lb1_create_buf(afe_test_client_t *pClient)
{
  elite_msg_data_buffer_t    *psPcmBuff;
  ADSPResult                 result = ADSP_EOK;
  int                        reqSize, actualSize;
  uint32_t                   num_buffers; //, client_bytes_per_channel;
  char                       bufQName[16];
  char                       *queue_buf_ptr;
  qurt_elite_channel_t       *qurt_elite_channel;

  // create new buffers, buffQ and queue the buffers in bufQ.
  qurt_elite_strl_cpy(bufQName,"afeTestBufI0D0N0", sizeof(bufQName));
  bufQName[9] = pClient->port_id + '0';

  queue_buf_ptr = (char*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(2),QURT_ELITE_HEAP_DEFAULT);

  if (NULL == queue_buf_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] Port bufQ Creation Failed", pClient->port_id);
    return ADSP_ENOMEMORY;
  }

  pClient->bufQ = (qurt_elite_queue_t*) queue_buf_ptr;

  //qurt_elite_channel_init(&pClient->afeClientChannel);
  qurt_elite_channel = &pClient->afeClientChannel;
  

  if (ADSP_FAILED(result = qurt_elite_queue_init(bufQName, 2, pClient->bufQ)) ||
      ADSP_FAILED(result = qurt_elite_channel_addq(qurt_elite_channel, pClient->bufQ, 0)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] ADSP_FAILED to init bufQ for client or add bufQ to channel %d!!!",pClient->port_id, result);
    return result;
  }

  //Create nNumBuffers of pcm Buffers and queue them up for usage
  num_buffers   = 2;
  //example of 48Khz, 16bit, 2 channel, 5ms    = 48 * 2 * 2 * 5 /*5ms*/;
  actualSize = pClient->samples_per_int * pClient->bytes_per_channel * pClient->num_channels * 5/*5ms*/;
 

  //psNewClient->actual_buf_size = actualSize;

  reqSize = GET_ELITEMSG_DATABUF_REQ_SIZE(actualSize);

  // Allocate and queue up the output buffers.
  for (uint32_t i = 0; i < num_buffers; i++)
  {
    //allocate the databuffer payload (metadata + pcm buffer size)
    psPcmBuff = (elite_msg_data_buffer_t*) qurt_elite_memory_malloc(reqSize, QURT_ELITE_HEAP_DEFAULT);
    if (NULL == psPcmBuff)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[port id:0x%x] Out of Memory!!!", pClient->port_id);
      return ADSP_ENOMEMORY;
    }

    memset(psPcmBuff,0,reqSize);

    psPcmBuff->nActualSize     = actualSize;
    psPcmBuff->nMaxSize        = psPcmBuff->nActualSize;
    psPcmBuff->pBufferReturnQ  = pClient->bufQ;

    if(2 == pClient->bytes_per_channel)
    {
       uint16_t * pPattern16 = (uint16_t *)&(psPcmBuff->nDataBuf);
       
       // this patthern are only for 2 channels
       for(int16_t i=0; i <  actualSize/2; i++)
       {
         if(i < actualSize/4)
         {
            *(pPattern16+i) = 0x1b11; //Left
         }
         else
         {
            *(pPattern16+i) = 0x1b22; // Right
         }
       }
    }
    else
    {
       uint32_t * pPattern32 = (uint32_t *)&(psPcmBuff->nDataBuf);
       
       // this patthern are only for 2 channels
       for(int16_t i=0; i <  actualSize/4; i++)
       {
         if(i < actualSize/8)
         {
            *(pPattern32+i) = 0x01b01110; //Left
         }
         else
         {
            *(pPattern32+i) = 0x01b02220; // Right 123456789ABCDEF
         }
       }
    }


    if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(pClient->bufQ, (elite_msg_any_payload_t*) psPcmBuff)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] Failed to fill ELiteExample buffer queue!!!", pClient->port_id);
      qurt_elite_memory_free(psPcmBuff);
      return result;
    }
  }
  return result;
}

static ADSPResult afe_test_port_lb2_create_buf(afe_test_client_t *pClient)
{
  elite_msg_data_buffer_t    *psPcmBuff;
  ADSPResult                 result = ADSP_EOK;
  int                        reqSize, actualSize;
  uint32_t                   num_buffers; //, client_bytes_per_channel;
  char                       bufQName[16];
  char                       *queue_buf_ptr;
  qurt_elite_channel_t       *qurt_elite_channel;

  // create new buffers, buffQ and queue the buffers in bufQ.
  qurt_elite_strl_cpy(bufQName,"afeTestBufI0D0N0", sizeof(bufQName));
  bufQName[9] = pClient->port_id + '0';

  queue_buf_ptr = (char*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(2),QURT_ELITE_HEAP_DEFAULT);

  if (NULL == queue_buf_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] Port bufQ Creation Failed", pClient->port_id);
    return ADSP_ENOMEMORY;
  }

  pClient->bufQ = (qurt_elite_queue_t*) queue_buf_ptr;

  //qurt_elite_channel_init(&pClient->afeClientChannel);
  qurt_elite_channel = &pClient->afeClientChannel;
  

  if (ADSP_FAILED(result = qurt_elite_queue_init(bufQName, 2, pClient->bufQ)) ||
      ADSP_FAILED(result = qurt_elite_channel_addq(qurt_elite_channel, pClient->bufQ, 0)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] ADSP_FAILED to init bufQ for client or add bufQ to channel %d!!!",pClient->port_id, result);
    return result;
  }

  //Create nNumBuffers of pcm Buffers and queue them up for usage
  num_buffers   = 2;
  //example of 48Khz, 16bit, 2 channel, 5ms    = 48 * 2 * 2 * 5 /*5ms*/;
  actualSize = pClient->samples_per_int * pClient->bytes_per_channel * pClient->num_channels * 5/*5ms*/;
 

  //psNewClient->actual_buf_size = actualSize;

  reqSize = GET_ELITEMSG_DATABUF_REQ_SIZE(actualSize);

  // Allocate and queue up the output buffers.
  for (uint32_t i = 0; i < num_buffers; i++)
  {
    //allocate the databuffer payload (metadata + pcm buffer size)
    psPcmBuff = (elite_msg_data_buffer_t*) qurt_elite_memory_malloc(reqSize, QURT_ELITE_HEAP_DEFAULT);
    if (NULL == psPcmBuff)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[port id:0x%x] Out of Memory!!!", pClient->port_id);
      return ADSP_ENOMEMORY;
    }

    memset(psPcmBuff,0,reqSize);

    psPcmBuff->nActualSize     = actualSize;
    psPcmBuff->nMaxSize        = psPcmBuff->nActualSize;
    psPcmBuff->pBufferReturnQ  = pClient->bufQ;

    if(2 == pClient->bytes_per_channel)
    {
       uint16_t * pPattern16 = (uint16_t *)&(psPcmBuff->nDataBuf);
       
       // this patthern are only for 2 channels
       for(int16_t i=0; i <  actualSize/2; i++)
       {
         if(i < actualSize/4)
         {
            *(pPattern16+i) = 0x2b11; //Left
         }
         else
         {
            *(pPattern16+i) = 0x2b22; // Right
         }
       }
    }
    else
    {
       uint32_t * pPattern32 = (uint32_t *)&(psPcmBuff->nDataBuf);
       
       // this patthern are only for 2 channels
       for(int16_t i=0; i <  actualSize/4; i++)
       {
         if(i < actualSize/8)
         {
            *(pPattern32+i) = 0x02b01110; //Left
         }
         else
         {
            *(pPattern32+i) = 0x02b02220; // Right
         }
       }
    }


    if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(pClient->bufQ, (elite_msg_any_payload_t*) psPcmBuff)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] Failed to fill ELiteExample buffer queue!!!", pClient->port_id);
      qurt_elite_memory_free(psPcmBuff);
      return result;
    }
  }
  return result;
}


static void afe_test_send_pcm_samples(afe_test_client_t *pClient)
{
   ADSPResult                 result = ADSP_EOK;
   //uint32_t                      nSamplesOut;
   qurt_elite_bufmgr_node_t   outBufMgrNode;
   elite_msg_any_t*        pPeerDataQMsg;
   uint32_t                   unChannelStatus;

   unChannelStatus = qurt_elite_channel_wait(qurt_elite_queue_get_channel(pClient->bufQ), qurt_elite_queue_get_channel_bit(pClient->bufQ));
   if (unChannelStatus)
   {
      result = qurt_elite_queue_pop_front(pClient->bufQ, (uint64_t*)&outBufMgrNode);


      /* Send output buffers to downstream service. */
      //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE RX client sending buf: %X", (uint32_t) outBufMgrNode.pBuffer);
      pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(
                                                      &outBufMgrNode,
                                                      ELITE_DATA_BUFFER,
                                                      NULL,                   /* do not need response */
                                                      0,                      /* token */
                                                      0);                     /* do not care response result*/
      result = qurt_elite_queue_push_back(pClient->phAfe->dataQ, (uint64_t*)pPeerDataQMsg);

      if(ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] Failed to deliver buffer dowstream. Dropping!", pClient->port_id);
        (void) elite_msg_push_payload_to_returnq(pClient->bufQ, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
      }
      else
      {
         pClient->num_sent_rx_client++;
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x]  Failed to deliver buffer dowstream. Dropping!!!", pClient->port_id);
   }
}

static int afe_test_lb1_rx_client_thread(void* arg)
{
   int32 rc = ADSP_EOK;
   afe_test_client_t *p_afe_rx_client =  (afe_test_client_t *)arg;//&afe_rx_client; //

   MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] afe_test_lb1_rx_client_thread", p_afe_rx_client->port_id);


   if(ADSP_FAILED(rc = Afe_Test_SendCmdConnectAFE(p_afe_rx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] RX port connection failed %d",p_afe_rx_client->port_id, rc);
      return rc;
   }


   if(ADSP_FAILED(rc = afe_test_port_send_media_t(p_afe_rx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] RX port meida type update failed %d",p_afe_rx_client->port_id, rc);
      return rc;
   }

   if(ADSP_FAILED(rc = afe_test_port_lb1_create_buf(p_afe_rx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] RX port meida type update failed %d",p_afe_rx_client->port_id, rc);
      return rc;
   }

   for(int32_t i =0; i < 1000; i++)
   {
      afe_test_send_pcm_samples(p_afe_rx_client);
   }

   // set this when the first external loopback sent all the data....
   // This is enough to check both external loopback
   afe_test_stop_bit_comp = TRUE;

   if(ADSP_FAILED(rc = afe_ext_generate_eos(p_afe_rx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] RX port meida type update failed %d",p_afe_rx_client->port_id, rc);
      return rc;
   }

   if(ADSP_FAILED(rc = Afe_Test_SendCmdDisconnectAFE(p_afe_rx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] RX port diconnect failed %d",p_afe_rx_client->port_id, rc);
      return rc;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] RX client thread exits", p_afe_rx_client->port_id);

   return rc;
}

static int afe_test_lb2_rx_client_thread(void* arg)
{
   int32 rc = ADSP_EOK;
   afe_test_client_t *p_afe_rx_client =  (afe_test_client_t *)arg;//&afe_rx_client; //

   MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] afe_test_lb2_rx_client_thread", p_afe_rx_client->port_id);


   if(ADSP_FAILED(rc = Afe_Test_SendCmdConnectAFE(p_afe_rx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] RX port connection failed %d",p_afe_rx_client->port_id, rc);
      return rc;
   }


   if(ADSP_FAILED(rc = afe_test_port_send_media_t(p_afe_rx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] RX port meida type update failed %d",p_afe_rx_client->port_id, rc);
      return rc;
   }

   if(ADSP_FAILED(rc = afe_test_port_lb2_create_buf(p_afe_rx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] RX port meida type update failed %d",p_afe_rx_client->port_id, rc);
      return rc;
   }

   for(int32_t i =0; i < 1000; i++)
   {
      afe_test_send_pcm_samples(p_afe_rx_client);
   }

   // set this when the first external loopback sent all the data....
   // This is enough to check both external loopback
   afe_test_stop_bit_comp = TRUE;

   if(ADSP_FAILED(rc = afe_ext_generate_eos(p_afe_rx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] RX port meida type update failed %d",p_afe_rx_client->port_id, rc);
      return rc;
   }

   if(ADSP_FAILED(rc = Afe_Test_SendCmdDisconnectAFE(p_afe_rx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] RX port diconnect failed %d",p_afe_rx_client->port_id, rc);
      return rc;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] RX client thread exits", p_afe_rx_client->port_id);

   return rc;
}


/**
 * gets the bufs from queue
 */
 
static ADSPResult afe_test_lb1_receive_bufs(afe_test_client_t *pClient)
{
   uint32_t                          unChannelStatus;
   elite_msg_data_buffer_t           *data_buffer_ptr;
   ADSPResult                        result = ADSP_EOK;
   elite_msg_any_t                   myDataQMsg;
   bool_t                            eos_received = FALSE;

   qurt_elite_queue_t *     client_dataQ      = pClient->svc_handle.dataQ; //pClientInfo->this_client_manager.dataQ;
   uint32_t                 client_dataQ_bit     = qurt_elite_queue_get_channel_bit(client_dataQ);

   for(;;)
   {
      unChannelStatus = qurt_elite_channel_wait(qurt_elite_queue_get_channel(client_dataQ), client_dataQ_bit);

      if(unChannelStatus &  client_dataQ_bit)
      {
         //pop dataQ until it's empty or until we have data buffer.
         result = qurt_elite_queue_pop_front(client_dataQ, (uint64_t*)&myDataQMsg);
         if (ADSP_FAILED(result))
         {
           MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] Failure on client_dataQ %p qurt_elite_queue_pop_front: %d", \
                 pClient->port_id, (client_dataQ), result);
           break;
         }
         //else
         //{
         //   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Msg opcode 0x%x and payload ptr 0x%x",  (myDataQMsg.unOpCode), myDataQMsg.pPayload);
         //}

         switch (myDataQMsg.unOpCode)
         {
            // If it is a Data Buffer
            case ELITE_DATA_BUFFER:
            {
                  // type case to a data buffer pointer
                  data_buffer_ptr = (elite_msg_data_buffer_t*)myDataQMsg.pPayload;

                  pClient->num_received_tx_client++;

                  if(pClient->afe_test_pattern_check)
                  {
                     pClient->num_check_count++;
                     //***************************************************************************
                     // Verify samples from the source.
                     //***************************************************************************
                     //Ex) 48Khz, 2channel, 16bit: 1~240 (0x1E0)...
                     if(2 == pClient->bytes_per_channel)
                     {
                        uint16_t *pPattern16  = (uint16_t*)(&(data_buffer_ptr->nDataBuf));
                  
                        if(0 != (*pPattern16) && (FALSE == afe_test_stop_bit_comp))
                        {                  
                           for(int16_t i=0; i <  data_buffer_ptr->nActualSize/2; i++)
                           {
                              if(i < data_buffer_ptr->nActualSize/4)
                              {
                                 if(*(pPattern16+i) != 0x1b11) //Left
                                 {
                                    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[port id:0x%x] Pattern mismatch on left channel [0x%x]", pClient->port_id, *pPattern16);
                                    result = ADSP_EFAILED;
                                    break;
                                 }
                              }
                              else
                              {
                                 if(*(pPattern16+i) != 0x1b22) // Right
                                 {                       
                                    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[port id:0x%x] Pattern mismatch on right channel [0x%x]",  pClient->port_id, *pPattern16);
                                    result = ADSP_EFAILED;
                                    break;
                                 }
                              }
                           }
                        }
                     }
                     else // 24bit case
                     {
                         uint32_t *pPattern32  = (uint32_t*)(&(data_buffer_ptr->nDataBuf));
                                             
                        if(0 != (*pPattern32) && (FALSE == afe_test_stop_bit_comp))
                        {                  
                           for(int16_t i=0; i <  data_buffer_ptr->nActualSize/4; i++)
                           {
                              if(i < data_buffer_ptr->nActualSize/8)
                              {
                                 //Q format mask
                                 if((*(pPattern32+i) & 0x0FFFFFF0) != (0x01b01110 & 0x0FFFFFF0)) //Left
                                 
                                 {
                                    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[port id:0x%x] Pattern mismatch on left channel [0x%x]", pClient->port_id, *pPattern32);
                                    result = ADSP_EFAILED;
                                    break;
                                 }
                              }
                              else
                              {
                                 if((*(pPattern32+i)  & 0x0FFFFFF0) != (0x01b02220 & 0x0FFFFFF0)) // Right
                                 {                       
                                    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[port id:0x%x] Pattern mismatch on right channel [0x%x]",  pClient->port_id, *pPattern32);
                                    result = ADSP_EFAILED;
                                    break;
                                 }
                              }
                           }
                        }

                     }
                  }
                  //MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"TX client [size:%d] starting buf 0x%8x, 0x%8x, 0x%8x, 0x%8x", 
                  //       data_buffer_ptr->nActualSize, *(pPattern), *(pPattern+1), *(pPattern+2), *(pPattern+3));

                  // Return payload buffer to buffer manger queue
                  elite_msg_return_payload_buffer(&myDataQMsg);
                  break;
               }
            case ELITE_DATA_MEDIA_TYPE:
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] Received MEDIA_TYPE on DataQ", pClient->port_id);
                  elite_msg_data_media_type_apr_t *pDataMedia = (elite_msg_data_media_type_apr_t*)myDataQMsg.pPayload;
                  if (pDataMedia->unMediaTypeFormat != ELITEMSG_DATA_MEDIA_TYPE_APR)
                  {
                     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort: Error! Unknown unMediaTypeFormat!!!\n");
                  }
                  else
                  {
                    elite_multi_channel_pcm_fmt_blk_t *pMultiChMediaFormatBlk = (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pDataMedia);
                    MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] sample rate %d, numch %d, bitwidth %d, interleaved %d, signed %d",
                                         pClient->port_id,
                                         pMultiChMediaFormatBlk->sample_rate, 
                                         pMultiChMediaFormatBlk->num_channels, 
                                         pMultiChMediaFormatBlk->bits_per_sample,
                                         pMultiChMediaFormatBlk->is_interleaved,
                                         pMultiChMediaFormatBlk->is_signed);

                  }
                  elite_msg_return_payload_buffer(&myDataQMsg);
                  break;
               }
            case ELITE_DATA_EOS:
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] Received EOS on DataQ", pClient->port_id);
                  eos_received = TRUE;
                  elite_msg_return_payload_buffer(&myDataQMsg);
                  break;
               }
            default:
               // return back any stray payload
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "####[port id:0x%x]  Received unexpected Data command on tx client DataQ", pClient->port_id);
               elite_msg_return_payload_buffer(&myDataQMsg);
               break;
         }

         if(eos_received || (ADSP_EOK != result))
         {
            break;
         }
      }
   }

   return result;;
}

static ADSPResult afe_test_lb2_receive_bufs(afe_test_client_t *pClient)
{
   uint32_t                          unChannelStatus;
   elite_msg_data_buffer_t           *data_buffer_ptr;
   ADSPResult                        result = ADSP_EOK;
   elite_msg_any_t                   myDataQMsg;
   bool_t                            eos_received = FALSE;

   qurt_elite_queue_t *     client_dataQ      = pClient->svc_handle.dataQ; //pClientInfo->this_client_manager.dataQ;
   uint32_t                 client_dataQ_bit     = qurt_elite_queue_get_channel_bit(client_dataQ);

   for(;;)
   {
      unChannelStatus = qurt_elite_channel_wait(qurt_elite_queue_get_channel(client_dataQ), client_dataQ_bit);

      if(unChannelStatus &  client_dataQ_bit)
      {
         //pop dataQ until it's empty or until we have data buffer.
         result = qurt_elite_queue_pop_front(client_dataQ, (uint64_t*)&myDataQMsg);
         if (ADSP_FAILED(result))
         {
           MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "[port id:0x%x] Failure on client_dataQ %p qurt_elite_queue_pop_front: %d", \
                 pClient->port_id, (client_dataQ), result);
           break;
         }
         //else
         //{
         //   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Msg opcode 0x%x and payload ptr 0x%x",  (myDataQMsg.unOpCode), myDataQMsg.pPayload);
         //}

         switch (myDataQMsg.unOpCode)
         {
            // If it is a Data Buffer
            case ELITE_DATA_BUFFER:
            {
                  // type case to a data buffer pointer
                  data_buffer_ptr = (elite_msg_data_buffer_t*)myDataQMsg.pPayload;

                  pClient->num_received_tx_client++;

                  if(pClient->afe_test_pattern_check)
                  {
                     pClient->num_check_count++;
                     //***************************************************************************
                     // Verify samples from the source.
                     //***************************************************************************
                     //Ex) 48Khz, 2channel, 16bit: 1~240 (0x1E0)...
                     if(2 == pClient->bytes_per_channel)
                     {
                        uint16_t *pPattern16  = (uint16_t*)(&(data_buffer_ptr->nDataBuf));
                  
                        if(0 != (*pPattern16) && (FALSE == afe_test_stop_bit_comp))
                        {                  
                           for(int16_t i=0; i <  data_buffer_ptr->nActualSize/2; i++)
                           {
                              if(i < data_buffer_ptr->nActualSize/4)
                              {
                                 if(*(pPattern16+i) != 0x2b11) //Left
                                 {
                                    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[port id:0x%x] Pattern mismatch on left channel [0x%x]", pClient->port_id, *pPattern16);
                                    result = ADSP_EFAILED;
                                    break;
                                 }
                              }
                              else
                              {
                                 if(*(pPattern16+i) != 0x2b22) // Right
                                 {                       
                                    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[port id:0x%x] Pattern mismatch on right channel [0x%x]",  pClient->port_id, *pPattern16);
                                    result = ADSP_EFAILED;
                                    break;
                                 }
                              }
                           }
                        }
                     }
                     else // 24bit case
                     {
                         uint32_t *pPattern32  = (uint32_t*)(&(data_buffer_ptr->nDataBuf));
                                             
                        if(0 != (*pPattern32) && (FALSE == afe_test_stop_bit_comp))
                        {                  
                           for(int16_t i=0; i <  data_buffer_ptr->nActualSize/4; i++)
                           {
                              if(i < data_buffer_ptr->nActualSize/8)
                              {
                                 //Q format mask
                                 if((*(pPattern32+i) & 0x0FFFFFF0) != (0x02b01110 & 0x0FFFFFF0)) //Left
                                 
                                 {
                                    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[port id:0x%x] Pattern mismatch on left channel [0x%x]", pClient->port_id, *pPattern32);
                                    result = ADSP_EFAILED;
                                    break;
                                 }
                              }
                              else
                              {
                                 if((*(pPattern32+i)  & 0x0FFFFFF0) != (0x02b02220 & 0x0FFFFFF0)) // Right
                                 {                       
                                    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[port id:0x%x] Pattern mismatch on right channel [0x%x]",  pClient->port_id, *pPattern32);
                                    result = ADSP_EFAILED;
                                    break;
                                 }
                              }
                           }
                        }

                     }
                  }
                  //MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"TX client [size:%d] starting buf 0x%8x, 0x%8x, 0x%8x, 0x%8x", 
                  //       data_buffer_ptr->nActualSize, *(pPattern), *(pPattern+1), *(pPattern+2), *(pPattern+3));

                  // Return payload buffer to buffer manger queue
                  elite_msg_return_payload_buffer(&myDataQMsg);
                  break;
               }
            case ELITE_DATA_MEDIA_TYPE:
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] Received MEDIA_TYPE on DataQ", pClient->port_id);
                  elite_msg_data_media_type_apr_t *pDataMedia = (elite_msg_data_media_type_apr_t*)myDataQMsg.pPayload;
                  if (pDataMedia->unMediaTypeFormat != ELITEMSG_DATA_MEDIA_TYPE_APR)
                  {
                     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort: Error! Unknown unMediaTypeFormat!!!\n");
                  }
                  else
                  {
                    elite_multi_channel_pcm_fmt_blk_t *pMultiChMediaFormatBlk = (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pDataMedia);
                    MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] sample rate %d, numch %d, bitwidth %d, interleaved %d, signed %d",
                                         pClient->port_id,
                                         pMultiChMediaFormatBlk->sample_rate, 
                                         pMultiChMediaFormatBlk->num_channels, 
                                         pMultiChMediaFormatBlk->bits_per_sample,
                                         pMultiChMediaFormatBlk->is_interleaved,
                                         pMultiChMediaFormatBlk->is_signed);

                  }
                  elite_msg_return_payload_buffer(&myDataQMsg);
                  break;
               }
            case ELITE_DATA_EOS:
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] Received EOS on DataQ", pClient->port_id);
                  eos_received = TRUE;
                  elite_msg_return_payload_buffer(&myDataQMsg);
                  break;
               }
            default:
               // return back any stray payload
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "####[port id:0x%x]  Received unexpected Data command on tx client DataQ", pClient->port_id);
               elite_msg_return_payload_buffer(&myDataQMsg);
               break;
         }

         if(eos_received || (ADSP_EOK != result))
         {
            break;
         }
      }
   }

   return result;;
}


static int afe_test_lb1_tx_client_thread(void* arg)
{
   int32 rc = ADSP_EOK;
   afe_test_client_t *p_afe_tx_client =  (afe_test_client_t *)arg;

   
   MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] afe_test_lb1_tx_client_thread",p_afe_tx_client->port_id);

   if(ADSP_FAILED(rc = afe_test_create_svc_handle(p_afe_tx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] afe_test_create_svc_handle failed %d",p_afe_tx_client->port_id, rc);
      return rc;
   }


   if(ADSP_FAILED(rc = Afe_Test_SendCmdConnectAFE(p_afe_tx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] TX port connection failed %d",p_afe_tx_client->port_id, rc);
      return rc;
   }

   // it should be true later
   p_afe_tx_client->afe_test_pattern_check = FALSE;

   if(ADSP_FAILED(rc = afe_test_lb1_receive_bufs(p_afe_tx_client)))
   {
      MSG_4(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "######## [port id:0x%x] TX port pattern mismatched  failed %d check_count %d num_received_tx_client %d #######",
	  	 p_afe_tx_client->port_id, rc, p_afe_tx_client->num_check_count, p_afe_tx_client->num_received_tx_client );
   }
   else
   {
      MSG_4(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "######## [port id:0x%x] TX port pattern matached  passed!!!!!!!! %d check_count %d num_received_tx_client %d #######",
	  	p_afe_tx_client->port_id, rc, p_afe_tx_client->num_check_count, p_afe_tx_client->num_received_tx_client );
   }

   if(ADSP_FAILED(rc = Afe_Test_SendCmdDisconnectAFE(p_afe_tx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] TX port diconnect failed %d",p_afe_tx_client->port_id ,rc);
      return rc;
   }
   
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] TX client thread exits", p_afe_tx_client->port_id);

   return rc;
}

static int afe_test_lb2_tx_client_thread(void* arg)
{
   int32 rc = ADSP_EOK;
   afe_test_client_t *p_afe_tx_client =  (afe_test_client_t *)arg;

   
   MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] afe_test_lb2_tx_client_thread",p_afe_tx_client->port_id);

   if(ADSP_FAILED(rc = afe_test_create_svc_handle(p_afe_tx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] afe_test_create_svc_handle failed %d",p_afe_tx_client->port_id, rc);
      return rc;
   }


   if(ADSP_FAILED(rc = Afe_Test_SendCmdConnectAFE(p_afe_tx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] TX port connection failed %d",p_afe_tx_client->port_id, rc);
      return rc;
   }

   // it should be true later
   p_afe_tx_client->afe_test_pattern_check = FALSE;

   if(ADSP_FAILED(rc = afe_test_lb2_receive_bufs(p_afe_tx_client)))
   {
      MSG_4(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "######## [port id:0x%x] TX port pattern mismatched  failed %d check_count %d num_received_tx_client %d #######",
	  	 p_afe_tx_client->port_id, rc, p_afe_tx_client->num_check_count, p_afe_tx_client->num_received_tx_client );
   }
   else
   {
      MSG_4(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "######## [port id:0x%x] TX port pattern matached  passed!!!!!!!! %d check_count %d num_received_tx_client %d #######",
	  	p_afe_tx_client->port_id, rc, p_afe_tx_client->num_check_count, p_afe_tx_client->num_received_tx_client );
   }


   if(ADSP_FAILED(rc = Afe_Test_SendCmdDisconnectAFE(p_afe_tx_client)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "[port id:0x%x] TX port diconnect failed %d",p_afe_tx_client->port_id ,rc);
      return rc;
   }
   
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[port id:0x%x] TX client thread exits", p_afe_tx_client->port_id);

   return rc;
}

static int32 afeTestAfeClkConfig(uint32_t clk_id, uint32_t clk_freq_in_hz, uint32_t clk_attri, uint32_t enable)
{
   int32 rc = ADSP_EOK;
   afeSvcSetCmdInfo_t  afeClkCfg;
   memset(&afeClkCfg, 0, sizeof(afeClkCfg));
   afeClkCfg.clkcfg.clk_set_minor_version = AFE_API_VERSION_CLOCK_SET;
   afeClkCfg.clkcfg.clk_id = clk_id;
   afeClkCfg.clkcfg.clk_freq_in_hz = clk_freq_in_hz;
   afeClkCfg.clkcfg.clk_attri = clk_attri;
   afeClkCfg.clkcfg.clk_root = AFE_CLOCK_SET_CLOCK_ROOT_DEFAULT;
   afeClkCfg.clkcfg.enable = enable;
   afeClkCfg.afeSvcParamHeader.module_id = AFE_MODULE_CLOCK_SET;
   afeClkCfg.afeSvcParamHeader.param_id = AFE_PARAM_ID_CLOCK_SET;
   afeClkCfg.afeSvcParamHeader.param_size = sizeof(afeClkCfg.clkcfg);
   afeClkCfg.afeSvcSetcmd.payload_size = sizeof(afeClkCfg.afeSvcParamHeader) + afeClkCfg.afeSvcParamHeader.param_size;

  //print dbg msg
  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"CLK TEST clk_id: 0x%x clk_freq_in_hz: %d enable: %d",clk_id, clk_freq_in_hz, enable);

  rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_SVC_CMD_SET_PARAM,
                                &afeClkCfg,
                                sizeof(afeClkCfg));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "CLK TEST config Failed, result=%ld",rc);
   }

   my_afe_token++;

   return 0;
}

static int32 afeTestRxGroupConfig(uint32_t testcase)
{
   int32 rc = ADSP_EOK;
   afeSvcSetCmdInfo_t  afeGroupCfg;
   memset(&afeGroupCfg, 0, sizeof(afeGroupCfg));

   switch(testcase)
   {
      // QUAD TDM
      case AFE_TDM_QUA_LB1_GROUP_TEST_1:
      case AFE_TDM_QUA_LB2_GROUP_TEST_1:
      case AFE_TDM_QUA_LB1_GROUP_TEST_2:
      case AFE_TDM_QUA_LB2_GROUP_TEST_2:
      {
         afeGroupCfg.tdm_group_cfg.group_device_cfg_minor_version  = AFE_API_VERSION_GROUP_DEVICE_TDM_CONFIG;
         afeGroupCfg.tdm_group_cfg.group_id  = AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX;
         afeGroupCfg.tdm_group_cfg.port_id[0] = AFE_PORT_ID_QUATERNARY_TDM_RX;
         afeGroupCfg.tdm_group_cfg.port_id[1] = AFE_PORT_ID_QUATERNARY_TDM_RX_1;
         afeGroupCfg.tdm_group_cfg.num_channels = 8;
         afeGroupCfg.tdm_group_cfg.sample_rate = AFE_PORT_SAMPLE_RATE_48K;
         afeGroupCfg.tdm_group_cfg.bit_width = 32;
         afeGroupCfg.tdm_group_cfg.nslots_per_frame = 8;
         afeGroupCfg.tdm_group_cfg.slot_width = 32;
         afeGroupCfg.tdm_group_cfg.slot_mask = 0xFF;
        
         afeGroupCfg.afeSvcParamHeader.module_id = AFE_MODULE_GROUP_DEVICE;
         afeGroupCfg.afeSvcParamHeader.param_id = AFE_PARAM_ID_GROUP_DEVICE_TDM_CONFIG;
         afeGroupCfg.afeSvcParamHeader.param_size = sizeof(afeGroupCfg.tdm_group_cfg);
         afeGroupCfg.afeSvcSetcmd.payload_size = sizeof(afeGroupCfg.afeSvcParamHeader) + afeGroupCfg.afeSvcParamHeader.param_size;

         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[Groupid:0x%x] [num_channels:%d] [sample rate:%d]  [bit width:%d]", afeGroupCfg.tdm_group_cfg.group_id, \
           afeGroupCfg.tdm_group_cfg.num_channels, afeGroupCfg.tdm_group_cfg.sample_rate, afeGroupCfg.tdm_group_cfg.bit_width);    
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[nslots_per_frame:0x%x] [slot_width:%d] [slot_mask:%d]", afeGroupCfg.tdm_group_cfg.nslots_per_frame, \
           afeGroupCfg.tdm_group_cfg.slot_width, afeGroupCfg.tdm_group_cfg.slot_mask);  
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[portid:0x%x][portid:0x%x][portid:0x%x][portid:0x%x]", afeGroupCfg.tdm_group_cfg.port_id[0], \
           afeGroupCfg.tdm_group_cfg.port_id[1], afeGroupCfg.tdm_group_cfg.port_id[2], afeGroupCfg.tdm_group_cfg.port_id[3]);  
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[portid:0x%x][portid:0x%x][portid:0x%x][portid:0x%x]", afeGroupCfg.tdm_group_cfg.port_id[4], \
           afeGroupCfg.tdm_group_cfg.port_id[5], afeGroupCfg.tdm_group_cfg.port_id[6], afeGroupCfg.tdm_group_cfg.port_id[7]);  
      }
      break;
      // QUAD I2S 
      case AFE_I2S_QUA_LB1_GROUP_TEST_1:
      case AFE_I2S_QUA_LB2_GROUP_TEST_1:
      {
         afeGroupCfg.i2s_group_cfg.minor_version  = AFE_API_VERSION_GROUP_DEVICE_I2S_CONFIG;
         afeGroupCfg.i2s_group_cfg.group_id  = AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX;
         afeGroupCfg.i2s_group_cfg.channel_mode = AFE_PORT_I2S_QUAD01;
         afeGroupCfg.i2s_group_cfg.sample_rate = 48000;
         afeGroupCfg.i2s_group_cfg.port_id[0] = AFE_PORT_ID_QUATERNARY_MI2S_RX;
         afeGroupCfg.i2s_group_cfg.port_id[1] = AFE_PORT_ID_QUATERNARY_MI2S_RX_1;
         afeGroupCfg.i2s_group_cfg.bit_width = 16;
         afeGroupCfg.afeSvcParamHeader.module_id = AFE_MODULE_GROUP_DEVICE;
         afeGroupCfg.afeSvcParamHeader.param_id = AFE_PARAM_ID_GROUP_DEVICE_I2S_CONFIG;
         afeGroupCfg.afeSvcParamHeader.param_size = sizeof(afeGroupCfg.i2s_group_cfg);
         afeGroupCfg.afeSvcSetcmd.payload_size = sizeof(afeGroupCfg.afeSvcParamHeader) + afeGroupCfg.afeSvcParamHeader.param_size;

         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[Groupid:0x%x] [channel mode:%d] [sample rate:%d]  [bit width:%d]", afeGroupCfg.i2s_group_cfg.group_id, \
           afeGroupCfg.i2s_group_cfg.channel_mode, afeGroupCfg.i2s_group_cfg.sample_rate, afeGroupCfg.i2s_group_cfg.bit_width);
      }      
      break;         
      case AFE_I2S_QUA_LB1_GROUP_TEST_2:
      case AFE_I2S_QUA_LB2_GROUP_TEST_2:     
      {
         afeGroupCfg.i2s_group_cfg.minor_version  = AFE_API_VERSION_GROUP_DEVICE_I2S_CONFIG;
         afeGroupCfg.i2s_group_cfg.group_id  = AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX;
         afeGroupCfg.i2s_group_cfg.channel_mode = AFE_PORT_I2S_QUAD01;
         afeGroupCfg.i2s_group_cfg.sample_rate = 48000;
         afeGroupCfg.i2s_group_cfg.port_id[0] = AFE_PORT_ID_QUATERNARY_MI2S_RX;
         afeGroupCfg.i2s_group_cfg.port_id[1] = AFE_PORT_ID_QUATERNARY_MI2S_RX_1;
         afeGroupCfg.i2s_group_cfg.bit_width = 32;
         afeGroupCfg.afeSvcParamHeader.module_id = AFE_MODULE_GROUP_DEVICE;
         afeGroupCfg.afeSvcParamHeader.param_id = AFE_PARAM_ID_GROUP_DEVICE_I2S_CONFIG;
         afeGroupCfg.afeSvcParamHeader.param_size = sizeof(afeGroupCfg.i2s_group_cfg);
         afeGroupCfg.afeSvcSetcmd.payload_size = sizeof(afeGroupCfg.afeSvcParamHeader) + afeGroupCfg.afeSvcParamHeader.param_size;

         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[Groupid:0x%x] [channel mode:%d] [sample rate:%d]  [bit width:%d]", afeGroupCfg.i2s_group_cfg.group_id, \
           afeGroupCfg.i2s_group_cfg.channel_mode, afeGroupCfg.i2s_group_cfg.sample_rate, afeGroupCfg.i2s_group_cfg.bit_width);
         
      }
      break;
      default:
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "### Error Unknown AFE TEST CASE ###");
         rc = ADSP_EFAILED;
      }
   }

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_SVC_CMD_SET_PARAM,
                                &afeGroupCfg,
                                sizeof(afeGroupCfg));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Rx Group config Failed, result=%ld",rc);
   }

   my_afe_token++;

   return 0;
}

static int32 afeTestTxGroupConfig(uint32_t testcase)
{
   int32 rc = ADSP_EOK;
   afeSvcSetCmdInfo_t  afeGroupCfg;
   memset(&afeGroupCfg, 0, sizeof(afeGroupCfg));

   switch(testcase)
   {
      // QUAD TDM
      case AFE_TDM_QUA_LB1_GROUP_TEST_1:
      case AFE_TDM_QUA_LB2_GROUP_TEST_1:        
      case AFE_TDM_QUA_LB1_GROUP_TEST_2:
      case AFE_TDM_QUA_LB2_GROUP_TEST_2:        
      {
         afeGroupCfg.tdm_group_cfg.group_device_cfg_minor_version  = AFE_API_VERSION_GROUP_DEVICE_TDM_CONFIG;
         afeGroupCfg.tdm_group_cfg.group_id  = AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX;
         afeGroupCfg.tdm_group_cfg.port_id[0] = AFE_PORT_ID_QUATERNARY_TDM_TX;
         afeGroupCfg.tdm_group_cfg.port_id[1] = AFE_PORT_ID_QUATERNARY_TDM_TX_1;
         afeGroupCfg.tdm_group_cfg.num_channels = 8;
         afeGroupCfg.tdm_group_cfg.sample_rate = AFE_PORT_SAMPLE_RATE_48K;
         afeGroupCfg.tdm_group_cfg.bit_width = 32;
         afeGroupCfg.tdm_group_cfg.nslots_per_frame = 8;
         afeGroupCfg.tdm_group_cfg.slot_width = 32;
         afeGroupCfg.tdm_group_cfg.slot_mask = 0xFF;
        
         afeGroupCfg.afeSvcParamHeader.module_id = AFE_MODULE_GROUP_DEVICE;
         afeGroupCfg.afeSvcParamHeader.param_id = AFE_PARAM_ID_GROUP_DEVICE_TDM_CONFIG;
         afeGroupCfg.afeSvcParamHeader.param_size = sizeof(afeGroupCfg.tdm_group_cfg);
         afeGroupCfg.afeSvcSetcmd.payload_size = sizeof(afeGroupCfg.afeSvcParamHeader) + afeGroupCfg.afeSvcParamHeader.param_size;

         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[Groupid:0x%x] [num_channels:%d] [sample rate:%d]  [bit width:%d]", afeGroupCfg.tdm_group_cfg.group_id, \
           afeGroupCfg.tdm_group_cfg.num_channels, afeGroupCfg.tdm_group_cfg.sample_rate, afeGroupCfg.tdm_group_cfg.bit_width);    
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[nslots_per_frame:0x%x] [slot_width:%d] [slot_mask:%d]", afeGroupCfg.tdm_group_cfg.nslots_per_frame, \
           afeGroupCfg.tdm_group_cfg.slot_width, afeGroupCfg.tdm_group_cfg.slot_mask);  
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[portid:0x%x][portid:0x%x][portid:0x%x][portid:0x%x]", afeGroupCfg.tdm_group_cfg.port_id[0], \
           afeGroupCfg.tdm_group_cfg.port_id[1], afeGroupCfg.tdm_group_cfg.port_id[2], afeGroupCfg.tdm_group_cfg.port_id[3]);  
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[portid:0x%x][portid:0x%x][portid:0x%x][portid:0x%x]", afeGroupCfg.tdm_group_cfg.port_id[4], \
           afeGroupCfg.tdm_group_cfg.port_id[5], afeGroupCfg.tdm_group_cfg.port_id[6], afeGroupCfg.tdm_group_cfg.port_id[7]);  
      }
      break;
      // QUAD I2S
      case AFE_I2S_QUA_LB1_GROUP_TEST_1:
      case AFE_I2S_QUA_LB2_GROUP_TEST_1:        
      {
         afeGroupCfg.i2s_group_cfg.minor_version  = AFE_API_VERSION_GROUP_DEVICE_I2S_CONFIG;
         afeGroupCfg.i2s_group_cfg.group_id  = AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX;
         afeGroupCfg.i2s_group_cfg.channel_mode = AFE_PORT_I2S_QUAD23;
         afeGroupCfg.i2s_group_cfg.sample_rate = 48000;
         afeGroupCfg.i2s_group_cfg.port_id[0] = AFE_PORT_ID_QUATERNARY_MI2S_TX ;
         afeGroupCfg.i2s_group_cfg.port_id[1] = AFE_PORT_ID_QUATERNARY_MI2S_TX_1;
         afeGroupCfg.i2s_group_cfg.bit_width = 16;
         afeGroupCfg.afeSvcParamHeader.module_id = AFE_MODULE_GROUP_DEVICE;
         afeGroupCfg.afeSvcParamHeader.param_id = AFE_PARAM_ID_GROUP_DEVICE_I2S_CONFIG;
         afeGroupCfg.afeSvcParamHeader.param_size = sizeof(afeGroupCfg.i2s_group_cfg);
         afeGroupCfg.afeSvcSetcmd.payload_size = sizeof(afeGroupCfg.afeSvcParamHeader) + afeGroupCfg.afeSvcParamHeader.param_size;

         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[Groupid:0x%x] [channel mode:%d] [sample rate:%d]  [bit width:%d]", afeGroupCfg.i2s_group_cfg.group_id, \
           afeGroupCfg.i2s_group_cfg.channel_mode, afeGroupCfg.i2s_group_cfg.sample_rate, afeGroupCfg.i2s_group_cfg.bit_width);
      }      
      break;         
      case AFE_I2S_QUA_LB1_GROUP_TEST_2:
      case AFE_I2S_QUA_LB2_GROUP_TEST_2:     
      {
         afeGroupCfg.i2s_group_cfg.minor_version  = AFE_API_VERSION_GROUP_DEVICE_I2S_CONFIG;
         afeGroupCfg.i2s_group_cfg.group_id  = AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX;
         afeGroupCfg.i2s_group_cfg.channel_mode = AFE_PORT_I2S_QUAD23;
         afeGroupCfg.i2s_group_cfg.sample_rate = 48000;
         afeGroupCfg.i2s_group_cfg.port_id[0] = AFE_PORT_ID_QUATERNARY_MI2S_TX ;
         afeGroupCfg.i2s_group_cfg.port_id[1] = AFE_PORT_ID_QUATERNARY_MI2S_TX_1;
         afeGroupCfg.i2s_group_cfg.bit_width = 32;
         afeGroupCfg.afeSvcParamHeader.module_id = AFE_MODULE_GROUP_DEVICE;
         afeGroupCfg.afeSvcParamHeader.param_id = AFE_PARAM_ID_GROUP_DEVICE_I2S_CONFIG;
         afeGroupCfg.afeSvcParamHeader.param_size = sizeof(afeGroupCfg.i2s_group_cfg);
         afeGroupCfg.afeSvcSetcmd.payload_size = sizeof(afeGroupCfg.afeSvcParamHeader) + afeGroupCfg.afeSvcParamHeader.param_size;

         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"[Groupid:0x%x] [channel mode:%d] [sample rate:%d]  [bit width:%d]", afeGroupCfg.i2s_group_cfg.group_id, \
           afeGroupCfg.i2s_group_cfg.channel_mode, afeGroupCfg.i2s_group_cfg.sample_rate, afeGroupCfg.i2s_group_cfg.bit_width);
         
      }
      break;
      default:
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "### Error Unknown AFE TEST CASE ###");
         rc = ADSP_EFAILED;
      }
   }

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_SVC_CMD_SET_PARAM,
                                &afeGroupCfg,
                                sizeof(afeGroupCfg));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Tx Group config Failed, result=%ld",rc);
   }

   my_afe_token++;

   return 0;
}

static int32 afeTestRxGroupEnable(uint32_t testcase, uint16_t enable)
{
   int32 rc = ADSP_EOK;
   afeSvcSetCmdInfo_t  afeGroupEnable;
   memset(&afeGroupEnable, 0, sizeof(afeGroupEnable));

   switch(testcase)
   {
	  
      // QUAD TDM
      case AFE_TDM_QUA_LB1_GROUP_TEST_1:
      case AFE_TDM_QUA_LB2_GROUP_TEST_1:        
      case AFE_TDM_QUA_LB1_GROUP_TEST_2:
      case AFE_TDM_QUA_LB2_GROUP_TEST_2:        
      {
         afeGroupEnable.group_enable.group_id  = AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX;
      }
      break;
      // QUAD I2S
      case AFE_I2S_QUA_LB1_GROUP_TEST_1:
      case AFE_I2S_QUA_LB2_GROUP_TEST_1:        
      case AFE_I2S_QUA_LB1_GROUP_TEST_2:
      case AFE_I2S_QUA_LB2_GROUP_TEST_2:     
      {
         afeGroupEnable.group_enable.group_id  = AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX;
      }
      break;
      default:
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "### Error Unknown AFE TEST CASE ###");
         rc = ADSP_EFAILED;
      }
   }

   afeGroupEnable.group_enable.enable = enable;
   afeGroupEnable.afeSvcParamHeader.module_id = AFE_MODULE_GROUP_DEVICE;
   afeGroupEnable.afeSvcParamHeader.param_id = AFE_PARAM_ID_GROUP_DEVICE_ENABLE;
   afeGroupEnable.afeSvcParamHeader.param_size = sizeof(afeGroupEnable.group_enable);
   afeGroupEnable.afeSvcSetcmd.payload_size = sizeof(afeGroupEnable.afeSvcParamHeader) + afeGroupEnable.afeSvcParamHeader.param_size;


   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"afeTestRxGroupEnable [Groupid:0x%x] [enable:%d]", afeGroupEnable.group_enable.group_id, afeGroupEnable.group_enable.enable);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_SVC_CMD_SET_PARAM,
                                &afeGroupEnable,
                                sizeof(afeGroupEnable));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Tx Group eanble Failed, result=%ld",rc);
   }

   my_afe_token++;

   return 0;
}


static int32 afeTestTxGroupEnable(uint32_t testcase, uint16_t enable)
{
   int32 rc = ADSP_EOK;
   afeSvcSetCmdInfo_t  afeGroupEnable;
   memset(&afeGroupEnable, 0, sizeof(afeGroupEnable));

   switch(testcase)
   {
      // QUAD TDM
      case AFE_TDM_QUA_LB1_GROUP_TEST_1:
      case AFE_TDM_QUA_LB2_GROUP_TEST_1:  
      case AFE_TDM_QUA_LB1_GROUP_TEST_2:
      case AFE_TDM_QUA_LB2_GROUP_TEST_2:
      {
         afeGroupEnable.group_enable.group_id  = AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX;
      }      
      break;         
      // QUAD I2S
      case AFE_I2S_QUA_LB1_GROUP_TEST_1:
      case AFE_I2S_QUA_LB2_GROUP_TEST_1:     
      case AFE_I2S_QUA_LB1_GROUP_TEST_2:
      case AFE_I2S_QUA_LB2_GROUP_TEST_2:     
      {
         afeGroupEnable.group_enable.group_id  = AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX;
      }
      break;
      default:
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "### Error Unknown AFE TEST CASE ###");
         rc = ADSP_EFAILED;
      }
   }

   afeGroupEnable.group_enable.enable = enable;
   afeGroupEnable.afeSvcParamHeader.module_id = AFE_MODULE_GROUP_DEVICE;
   afeGroupEnable.afeSvcParamHeader.param_id = AFE_PARAM_ID_GROUP_DEVICE_ENABLE;
   afeGroupEnable.afeSvcParamHeader.param_size = sizeof(afeGroupEnable.group_enable);
   afeGroupEnable.afeSvcSetcmd.payload_size = sizeof(afeGroupEnable.afeSvcParamHeader) + afeGroupEnable.afeSvcParamHeader.param_size;


   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"afeTestTxGroupEnable [Groupid:0x%x] [enable:%d]", afeGroupEnable.group_enable.group_id, afeGroupEnable.group_enable.enable);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_SVC_CMD_SET_PARAM,
                                &afeGroupEnable,
                                sizeof(afeGroupEnable));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Tx Group eanble Failed, result=%ld",rc);
   }

   my_afe_token++;

   return 0;
}


static int32 afeTestConfigSlotMapping(afe_test_client_t *pClient)
{
   int32 rc = ADSP_EOK;
   afeSetCmdInfo_t afePortSlotMapping;
   memset(&afePortSlotMapping, 0, sizeof(afePortSlotMapping));


   switch(pClient->port_id)
   {
      case AFE_PORT_ID_TERTIARY_TDM_RX_1:

      case AFE_PORT_ID_QUATERNARY_TDM_TX_1:
      case AFE_PORT_ID_QUATERNARY_TDM_RX_1:
      {
         afePortSlotMapping.slotcfg.minor_version = AFE_API_VERSION_SLOT_MAPPING_CONFIG;
         afePortSlotMapping.slotcfg.num_channel = pClient->num_channels;
         afePortSlotMapping.slotcfg.bitwidth = (pClient->bit_width > 16) ? 32 : 16;
         afePortSlotMapping.slotcfg.data_align_type = AFE_SLOT_MAPPING_DATA_ALIGN_MSB;
         if(16 == pClient->bit_width)
         {
            afePortSlotMapping.slotcfg.offset[0] = 4;
            afePortSlotMapping.slotcfg.offset[1] = 6;
         }
         else
         {
            afePortSlotMapping.slotcfg.offset[0] = 8;
            afePortSlotMapping.slotcfg.offset[1] = 12;
         }
         afePortSlotMapping.slotcfg.offset[2] = 0xFFFF;
         afePortSlotMapping.slotcfg.offset[3] = 0xFFFF;
         afePortSlotMapping.slotcfg.offset[4] = 0xFFFF;
         afePortSlotMapping.slotcfg.offset[5] = 0xFFFF;
         afePortSlotMapping.slotcfg.offset[6] = 0xFFFF;
         afePortSlotMapping.slotcfg.offset[7] = 0xFFFF;
      }      
      break;         

      case AFE_PORT_ID_TERTIARY_TDM_RX:

      case AFE_PORT_ID_QUATERNARY_TDM_RX:
      case AFE_PORT_ID_QUATERNARY_MI2S_RX:
      case AFE_PORT_ID_QUATERNARY_MI2S_RX_1:
      case AFE_PORT_ID_QUATERNARY_TDM_TX:
      case AFE_PORT_ID_QUATERNARY_MI2S_TX:
      case AFE_PORT_ID_QUATERNARY_MI2S_TX_1:
      {
         afePortSlotMapping.slotcfg.minor_version = AFE_API_VERSION_SLOT_MAPPING_CONFIG;
         afePortSlotMapping.slotcfg.num_channel = pClient->num_channels;
         afePortSlotMapping.slotcfg.bitwidth = (pClient->bit_width > 16) ? 32 : 16;
         afePortSlotMapping.slotcfg.data_align_type = AFE_SLOT_MAPPING_DATA_ALIGN_MSB;
         if(16 == pClient->bit_width)
         {
            afePortSlotMapping.slotcfg.offset[0] = 0;
            afePortSlotMapping.slotcfg.offset[1] = 2;
         }
         else
         {
            afePortSlotMapping.slotcfg.offset[0] = 0;
            afePortSlotMapping.slotcfg.offset[1] = 4;
         }
         afePortSlotMapping.slotcfg.offset[2] = 0xFFFF;
         afePortSlotMapping.slotcfg.offset[3] = 0xFFFF;
         afePortSlotMapping.slotcfg.offset[4] = 0xFFFF;
         afePortSlotMapping.slotcfg.offset[5] = 0xFFFF;
         afePortSlotMapping.slotcfg.offset[6] = 0xFFFF;
         afePortSlotMapping.slotcfg.offset[7] = 0xFFFF;
      }
      break;
      default:
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "### Error Unknown AFE TEST CASE ###");
         rc = ADSP_EFAILED;
      }
   }

   afePortSlotMapping.afeParamHeader.module_id = AFE_MODULE_TDM;
   afePortSlotMapping.afeParamHeader.param_id = AFE_PARAM_ID_PORT_SLOT_MAPPING_CONFIG;
   afePortSlotMapping.afeParamHeader.param_size = sizeof(afePortSlotMapping.slotcfg);
   afePortSlotMapping.afeSetcmd.payload_size = sizeof(afePortSlotMapping.afeParamHeader) + afePortSlotMapping.afeParamHeader.param_size;
   afePortSlotMapping.afeSetcmd.port_id = pClient->port_id;


   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE Slot Mapping Config Port[0x%x] configure Command", pClient->port_id);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_SET_PARAM_V2,
                                &afePortSlotMapping,
                                sizeof(afePortSlotMapping));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Slot Mapping Config Command Failed, result=%ld",rc);
   }

   my_afe_token++;


   return 0;

}

#ifdef ADD_CUSTOM_HEADER
static int32 afeTestConfigCustomHeader( afe_test_client_t *pClient) 
{
  int32 rc = ADSP_EOK;
  afeSetCustomHeaderCmdInfo_t afePortCustomHeader;

  memset(&afePortCustomHeader, 0, sizeof(afePortCustomHeader));

  afePortCustomHeader.tdmCustomHeadersCfg.minor_version = AFE_API_VERSION_CUSTOM_TDM_HEADER_CONFIG;
  afePortCustomHeader.tdmCustomHeadersCfg.start_offset = 2 + 16;
  afePortCustomHeader.tdmCustomHeadersCfg.header_width = 2;
  afePortCustomHeader.tdmCustomHeadersCfg.header_type = AFE_CUSTOM_TDM_HEADER_TYPE_DEFAULT;
  afePortCustomHeader.tdmCustomHeadersCfg.num_frame_repeat = 8;

  afePortCustomHeader.afeParamHeader.module_id = AFE_MODULE_TDM;
  afePortCustomHeader.afeParamHeader.param_id = AFE_PARAM_ID_CUSTOM_TDM_HEADER_CONFIG;
  afePortCustomHeader.afeParamHeader.param_size = sizeof(afePortCustomHeader.tdmCustomHeadersCfg) + sizeof(afePortCustomHeader.header);
  afePortCustomHeader.afeSetcmd.payload_size = sizeof(afePortCustomHeader.afeParamHeader) + afePortCustomHeader.afeParamHeader.param_size;
  afePortCustomHeader.afeSetcmd.port_id = pClient->port_id;

  afePortCustomHeader.header[0] = 0x3C00;
  afePortCustomHeader.header[1] = 0xB218;
  afePortCustomHeader.header[2] = 0x0000;
  afePortCustomHeader.header[3] = 0x00C0;
  afePortCustomHeader.header[4] = 0x0007;
  afePortCustomHeader.header[5] = 0x0000;
  afePortCustomHeader.header[6] = 0x0000;
  afePortCustomHeader.header[7] = 0x0000;


  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE custom header Config Port[0x%x] configure Command", pClient->port_id);

  rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
							 my_testclient_apr_addr,
							 my_test_client_port,
							 my_afe_addr,
							 0,
							 my_afe_token,
							 AFE_PORT_CMD_SET_PARAM_V2,
							 &afePortCustomHeader,
							 sizeof(afePortCustomHeader));

  rc = apr_event_wait( my_work_done_event );
  if( ADSP_FAILED( rc ))
  {
     MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE custom header Config Command Failed, result=%ld", rc);
  }

  my_afe_token++;

  return 0;

}
#endif

static int afe_svc_apr_ext_group_loopback_test(void* afeCmdQueue)
{

   int32 rc = ADSP_EOK;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"APR AFE external group loopback test start");

   // apr registration
   rc = apr_event_create(&my_work_done_event);
   rc = elite_apr_if_register_by_name(
           &my_testclient_apr_handle, &my_testclient_apr_addr, (char*)"local.testclient", sizeof("local.testclient"), testclient_isr_recv_fn, NULL  );

   rc = elite_apr_if_lookup_addr_by_name( (char*)"qcom.adsp.afe", sizeof("qcom.adsp.afe"), &my_afe_addr);

   // loop back ports for the first loopback
   rc = getTestAfePortId(my_afe_1st_lb_test_case_in_group, &afe_tx_client1.port_id, &afe_rx_client1.port_id);
   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "getTestAfePortId Failed, result=%ld",rc);
      return rc;
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "my_afe_1st_lb_test_case_in_group AFE Port [RX0x%x ==> TX0x%x]", afe_rx_client1.port_id, afe_tx_client1.port_id);
   }

   // loop back ports for the 2nd loopback

   rc = getTestAfePortId(my_afe_2nd_lb_test_case_in_group, &afe_tx_client2.port_id, &afe_rx_client2.port_id);
   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "getTestAfePortId Failed, result=%ld",rc);
      return rc;
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "my_afe_1st_lb_test_case_in_group AFE Port [RX0x%x ==> TX0x%x]", afe_rx_client2.port_id, afe_tx_client2.port_id);
   }

  
   uint32_t clock_id, clk_value;

   // clock config for the first loopback
   getTestClkAfe(my_afe_1st_lb_test_case_in_group, afe_rx_client1.port_id, &clock_id, &clk_value);
   afeTestAfeClkConfig(clock_id, clk_value, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, 1);
   
   getTestClkAfe(my_afe_1st_lb_test_case_in_group, afe_tx_client1.port_id, &clock_id, &clk_value);
   afeTestAfeClkConfig(clock_id, clk_value, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, 1);

   // clock config for the 2nd loopback
   getTestClkAfe(my_afe_2nd_lb_test_case_in_group, afe_rx_client2.port_id, &clock_id, &clk_value);
   afeTestAfeClkConfig(clock_id, clk_value, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, 1);
   
   getTestClkAfe(my_afe_2nd_lb_test_case_in_group, afe_tx_client2.port_id, &clock_id, &clk_value);
   afeTestAfeClkConfig(clock_id, clk_value, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, 1);

   //################ Group Quad MI2S RX Config      #####################
   afeTestRxGroupConfig(my_afe_1st_lb_test_case_in_group); 
   
  
   //################ Group Quad MI2S TX Config      #####################
   afeTestTxGroupConfig(my_afe_1st_lb_test_case_in_group); 

   
   //################ Group Quad MI2S RX Enable      #####################
   afeTestRxGroupEnable(my_afe_1st_lb_test_case_in_group, 1); 
   
   //################ Group Quad MI2S TX Enable      #####################
   afeTestTxGroupEnable(my_afe_1st_lb_test_case_in_group, 1); 


   /* Send the first loopback AFE Tx Port configure command to afe service. */
   afeSetCmdInfo_t afePortCfgCmd;
   memset(&afePortCfgCmd, 0, sizeof(afePortCfgCmd));

   rc = getTestAfeTxPortConfig(my_afe_1st_lb_test_case_in_group, &afePortCfgCmd, &afe_tx_client1);
   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "getTestAfeTxPortConfig Failed, result=%ld",rc);
      return rc;
   }

   afePortCfgCmd.afeSetcmd.payload_size = sizeof(afePortCfgCmd.afeParamHeader) + afePortCfgCmd.afeParamHeader.param_size;
   afePortCfgCmd.afeSetcmd.port_id = afe_tx_client1.port_id;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE First Tx Port[0x%x] configure Command", afe_tx_client1.port_id);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_SET_PARAM_V2,
                                &afePortCfgCmd,
                                sizeof(afeSetCmdInfo_t));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe First Tx Port configure Command Failed, result=%ld",rc);
   }

   my_afe_token++;

   afeTestConfigSlotMapping(&afe_tx_client1);

   /* Send the 2nd loopback AFE Tx Port configure command to afe service. */
   memset(&afePortCfgCmd, 0, sizeof(afePortCfgCmd));

   rc = getTestAfeTxPortConfig(my_afe_2nd_lb_test_case_in_group, &afePortCfgCmd, &afe_tx_client2);
   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "getTestAfeTxPortConfig Failed, result=%ld",rc);
      return rc;
   }

   afePortCfgCmd.afeSetcmd.payload_size = sizeof(afePortCfgCmd.afeParamHeader) + afePortCfgCmd.afeParamHeader.param_size;
   afePortCfgCmd.afeSetcmd.port_id = afe_tx_client2.port_id;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending 2nd AFE Tx Port[0x%x] configure Command", afe_tx_client2.port_id);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_SET_PARAM_V2,
                                &afePortCfgCmd,
                                sizeof(afeSetCmdInfo_t));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe 2nd Tx Port configure Command Failed, result=%ld",rc);
   }

   my_afe_token++;

   afeTestConfigSlotMapping(&afe_tx_client2);


   /* Send the frist AFE Rx port configure command to afe service.*/
   memset(&afePortCfgCmd, 0, sizeof(afePortCfgCmd));

   rc = getTestAfeRxPortConfig(my_afe_1st_lb_test_case_in_group, &afePortCfgCmd, &afe_rx_client1);
   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "getTestAfeRxPortConfig Failed, result=%ld",rc);
      return rc;
   }

   afePortCfgCmd.afeSetcmd.payload_size = sizeof(afePortCfgCmd.afeParamHeader) + afePortCfgCmd.afeParamHeader.param_size;
   afePortCfgCmd.afeSetcmd.port_id = afe_rx_client1.port_id;;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE Rx Port[0x%x] configure Command", afe_rx_client1.port_id);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_SET_PARAM_V2,
                                &afePortCfgCmd,
                                sizeof(afeSetCmdInfo_t));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe 1st Rx Port configure Command Failed, result=%ld",rc);
   }

   my_afe_token++;

   #ifdef ADD_CUSTOM_HEADER
   afeTestConfigCustomHeader(&afe_rx_client1);
   #endif

   afeTestConfigSlotMapping(&afe_rx_client1);

   /* Send the 2nd AFE Rx port configure command to afe service.*/
   memset(&afePortCfgCmd, 0, sizeof(afePortCfgCmd));

   rc = getTestAfeRxPortConfig(my_afe_2nd_lb_test_case_in_group, &afePortCfgCmd, &afe_rx_client2);
   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "getTestAfeRxPortConfig Failed, result=%ld",rc);
      return rc;
   }

   afePortCfgCmd.afeSetcmd.payload_size = sizeof(afePortCfgCmd.afeParamHeader) + afePortCfgCmd.afeParamHeader.param_size;
   afePortCfgCmd.afeSetcmd.port_id = afe_rx_client2.port_id;;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending 2nd AFE Rx Port[0x%x] configure Command", afe_rx_client2.port_id);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_SET_PARAM_V2,
                                &afePortCfgCmd,
                                sizeof(afeSetCmdInfo_t));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe 2nd Rx Port configure Command Failed, result=%ld",rc);
   }

   my_afe_token++;

   afeTestConfigSlotMapping(&afe_rx_client2);

   /* Send the start command to afe service. */
   afe_port_cmd_device_start_t   afePortStartCmd;
   afePortStartCmd.port_id = afe_tx_client1.port_id;;


   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending 1st AFE Tx Port[0x%x] Start Command",  afe_tx_client1.port_id);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                               my_testclient_apr_addr,
                               my_test_client_port,
                               my_afe_addr,
                               0,
                               my_afe_token,
                               AFE_PORT_CMD_DEVICE_START,
                               &afePortStartCmd,
                               sizeof(afePortStartCmd));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe 1st Tx Start Command Failed, result=%ld",rc);
   }

   my_afe_token++;

   afePortStartCmd.port_id = afe_tx_client2.port_id;;

   
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending 2nd AFE Tx Port[0x%x] Start Command",  afe_tx_client2.port_id);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                               my_testclient_apr_addr,
                               my_test_client_port,
                               my_afe_addr,
                               0,
                               my_afe_token,
                               AFE_PORT_CMD_DEVICE_START,
                               &afePortStartCmd,
                               sizeof(afePortStartCmd));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe 2nd Tx Start Command Failed, result=%ld",rc);
   }

   my_afe_token++;


   afePortStartCmd.port_id = afe_rx_client1.port_id;;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending 1st AFE Rx Port[0x%x] Start Command", afe_rx_client1.port_id);
   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_DEVICE_START,
                                &afePortStartCmd,
                                sizeof(afePortStartCmd));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe 1st Rx Start Command Failed, result=%ld",rc);
   }

   my_afe_token++;


   afePortStartCmd.port_id = afe_rx_client2.port_id;;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending 2nd AFE Rx Port[0x%x] Start Command", afe_rx_client2.port_id);
   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_DEVICE_START,
                                &afePortStartCmd,
                                sizeof(afePortStartCmd));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe 2nd Rx Start Command Failed, result=%ld",rc);
   }

   my_afe_token++;


   //////////////////////////////////////////////////////////////////////////////////////////////
   afe_rx_client1.data_path = AFE_RX_AUD_MIXER_IN;

   rc = qurt_elite_thread_launch(&afe_rx_client1.tid, (char*)"rx_test_client1", \
           NULL, 4096, 120, afe_test_lb1_rx_client_thread, &afe_rx_client1, QURT_ELITE_HEAP_DEFAULT);


   afe_tx_client1.data_path = AFE_PP_OUT;

   rc = qurt_elite_thread_launch(&afe_tx_client1.tid, (char*)"tx_test_client1", \
           NULL, 4096, 120, afe_test_lb1_tx_client_thread, &afe_tx_client1, QURT_ELITE_HEAP_DEFAULT);


   afe_rx_client2.data_path = AFE_RX_AUD_MIXER_IN;

   rc = qurt_elite_thread_launch(&afe_rx_client2.tid, (char*)"rx_test_client2", \
           NULL, 4096, 120, afe_test_lb2_rx_client_thread, &afe_rx_client2, QURT_ELITE_HEAP_DEFAULT);


   afe_tx_client2.data_path = AFE_PP_OUT;

   rc = qurt_elite_thread_launch(&afe_tx_client2.tid, (char*)"tx_test_client2", \
           NULL, 4096, 120, afe_test_lb2_tx_client_thread, &afe_tx_client2, QURT_ELITE_HEAP_DEFAULT);



   qurt_elite_thread_join(afe_rx_client2.tid, &afe_rx_client2.thread_result);
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe rx client2 is ending");

   qurt_elite_thread_join(afe_rx_client1.tid, &afe_rx_client1.thread_result);
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe rx client1 is ending");

 
   /* Send the AFE port stop command to afe service. */
   afe_port_cmd_device_stop_t   afePortStopCmd;
   afePortStopCmd.port_id = afe_tx_client1.port_id;;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending 1st AFE Port Tx [0x%x] Stop Command", afe_tx_client1.port_id);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_DEVICE_STOP,
                                &afePortStopCmd,
                                sizeof(afePortStopCmd));


   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe 1st Stop Tx Command Failed, result=%ld",rc);
   }

   my_afe_token++;

   afePortStopCmd.port_id = afe_tx_client2.port_id;;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending 2nd AFE Port Tx [0x%x] Stop Command", afe_tx_client2.port_id);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_DEVICE_STOP,
                                &afePortStopCmd,
                                sizeof(afePortStopCmd));


   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe 2nd Stop Tx Command Failed, result=%ld",rc);
   }

   my_afe_token++;

   afePortStopCmd.port_id = afe_rx_client1.port_id;;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending 1st AFE Port Rx [0x%x] Stop Command", afe_rx_client1.port_id);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_DEVICE_STOP,
                                &afePortStopCmd,
                                sizeof(afePortStopCmd));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe 1st Rx Stop Command Failed, result=%ld",rc);
   }

   afePortStopCmd.port_id = afe_rx_client2.port_id;;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending 2nd AFE Port Rx [0x%x] Stop Command", afe_rx_client2.port_id);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_DEVICE_STOP,
                                &afePortStopCmd,
                                sizeof(afePortStopCmd));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe 2nd Rx Stop Command Failed, result=%ld",rc);
   }

   //################ Group Quad MI2S RX Disable      #####################
   afeTestRxGroupEnable(my_afe_1st_lb_test_case_in_group, 0); 
   
   //################ Group Quad MI2S TX Disable      #####################
   afeTestTxGroupEnable(my_afe_1st_lb_test_case_in_group, 0);   

   qurt_elite_thread_join(afe_tx_client2.tid, &afe_tx_client2.thread_result);
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe tx client2 is ending"); 
   

   qurt_elite_thread_join(afe_tx_client1.tid, &afe_tx_client1.thread_result);
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe tx client1 is ending"); 

   afe_test_free_resources(&afe_tx_client2);
   afe_test_free_resources(&afe_rx_client2);


   afe_test_free_resources(&afe_tx_client1);
   afe_test_free_resources(&afe_rx_client1);

   // disable clock
   getTestClkAfe(my_afe_1st_lb_test_case_in_group, afe_rx_client2.port_id, &clock_id, &clk_value);
   afeTestAfeClkConfig(clock_id, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, 0);
   
   getTestClkAfe(my_afe_1st_lb_test_case_in_group, afe_tx_client2.port_id, &clock_id, &clk_value);
   afeTestAfeClkConfig(clock_id, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, 0);

   getTestClkAfe(my_afe_1st_lb_test_case_in_group, afe_rx_client1.port_id, &clock_id, &clk_value);
   afeTestAfeClkConfig(clock_id, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, 0);
   
   getTestClkAfe(my_afe_1st_lb_test_case_in_group, afe_tx_client1.port_id, &clock_id, &clk_value);
   afeTestAfeClkConfig(clock_id, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, 0);

  

   /* Destroy the client and the apr event.*/
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DeInitializing APR resources and thread");
   rc = apr_event_destroy( my_work_done_event );
   rc = elite_apr_if_deregister( my_testclient_apr_handle);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"APR AFE External Group loopback test exiting");
   
   return 0;
}

#ifndef SIM
static DalDeviceHandle *tlmm_hptr = NULL;
#if 0

// The following is for QUAD self loopback
static uint32_t qua_n_mi2s_group_gpio[6] = {
 58, 59, 
 60, 61, 
 62, 63
}; 

static uint32_t qua_n_mi2s_group_in_out[6] = {
  DAL_GPIO_OUTPUT, DAL_GPIO_OUTPUT,
  DAL_GPIO_INPUT,  DAL_GPIO_INPUT,
  DAL_GPIO_OUTPUT, DAL_GPIO_OUTPUT
};

#endif

// The following is for TER to QUAD loopback
static uint32_t ter2qua_group_gpio[4] = {
 75, 76, 
 78, 60, 
};

static uint32_t ter2qua_group_in_out[4] = {
  DAL_GPIO_OUTPUT,  DAL_GPIO_OUTPUT,
  DAL_GPIO_OUTPUT, DAL_GPIO_INPUT
};


static ADSPResult AFE_test_gpio_init()
{
  DALGpioSignalType pin_config;
  uint32_t i;

  // Attach to TLMM
  if (DAL_DeviceAttach(DALDEVICEID_TLMM, &tlmm_hptr) != DAL_SUCCESS)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO TEST INIT: Failed to attach to TLMM(1)");
    return ADSP_EFAILED;
  }

  if(NULL == tlmm_hptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO TEST INIT: Failed to attach to TLMM(2)");
   return ADSP_EFAILED;
  }

  for(i = 0; i < sizeof(ter2qua_group_gpio)/sizeof(uint32_t); i++)
  {
    //create a GPIO configuration for the required pin
    //function value 0 indicates that this is pin is a dedicated interrupt pin (not a general purpose pin)
    pin_config = DAL_GPIO_CFG(ter2qua_group_gpio[i], 1, ter2qua_group_in_out[i], DAL_GPIO_NO_PULL, DAL_GPIO_16MA);

    //call the TLMM DAL API to configure the GPIO
    if (DalTlmm_ConfigGpio(tlmm_hptr, pin_config, DAL_TLMM_GPIO_ENABLE) != DAL_SUCCESS)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO TEST INIT: Failed to configure GPIO Pin(%d)", ter2qua_group_gpio[i]);
      return ADSP_EFAILED;
    }
   }


  MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "GPIO TESTINIT: Configured GPIO Interrupt successfully");

  return ADSP_EOK;
}
#endif


/**
* This function lauch the whole test thread.
*
* @param[in] afeCmdQueue A pointer to AFE static service cmdQ
*       struct
* @return the test thread result.
*/
int AFE_test_apr_ext_group_loopback(void* afeCmdQueue)
{
    int           testThreadId, threadResult;
    ADSPResult     result;
    char          threadName[]="AFEAprExtGrpLBThread";

    // Launch the test thread...
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "-----------AFE External Group Loopback Test start: Launching AFEAprExtGrpLBThread...-----------\n");

#ifndef SIM
    AFE_test_gpio_init();
#endif
    if (ADSP_FAILED (result = qurt_elite_thread_launch(&testThreadId, threadName, NULL, 4096, \
             170, afe_svc_apr_ext_group_loopback_test, (void *)afeCmdQueue, QURT_ELITE_HEAP_DEFAULT)))
    {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Failed to create Port Threads");
        return result;
    }
    qurt_elite_thread_join(testThreadId, &threadResult);
    return threadResult;
}

#endif //AFE_TEST



