#ifdef AFE_TEST


enum mytest_e {
  AFE_TEST_BEGIN_ENUM = 1,
  AFE_I2S_PRI_TEST = 1,
  AFE_I2S_SEC_TEST,
  AFE_I2S_PRI_SEC_TEST,
  AFE_PCM_PRI_TEST,
  AFE_PCM_SEC_TEST,
  AFE_PCM_PRI_SEC_TEST,

  // Basic TDM tests, emulate PCM, 16 bits, bit clock @ 2.048 MHz
  AFE_TDM_PRI_TEST,
  AFE_TDM_SEC_TEST,
  AFE_TDM_TER_TEST,
  AFE_TDM_QUA_TEST,

  AFE_TDM_PRI_32BITS_TEST,

  // Self loopback on max available setting, 32bits/8chs/32KHz
  AFE_TDM_MAX_PRI_TEST,

  // 48 KHz doesn't work, as I checked the SCLK have much jitters..
  AFE_TDM_MAX1_PRI_TEST,

  // Basic TDM test, PRI -> SEC
  AFE_TDM_PRI_SEC_TEST,

  // TDM test, PRI -> SEC, 32 bits data, 8 channels, 8 KHz
  AFE_TDM_MAX_8K_PRI_SEC_TEST,

  // Basic TDM test as I2S
  AFE_TDM_I2S_PRI_TEST,
  
  AFE_TEST_END_ENUM = AFE_TDM_I2S_PRI_TEST,
};

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

/* GPIO related */
#include "DDITlmm.h"


// duplicates V2 APR
#define AFE_APR_IBASIC_RSP_RESULT   ( 0x000110E8 )
#define AFE_APR_IBASIC_RSP_ACCEPTED ( 0x000100BE )

static const uint32 my_test_client_port = 0x00002000;
static apr_event_t  my_work_done_event;
static uint32 my_afe_token = 0xF000F000;
static uint32 my_testclient_apr_handle = 0;
static uint16_t my_testclient_apr_addr   = 0;

static uint16 my_afe_addr = 0;

static mytest_e my_afe_test_case;

static bool_t afe_test_stop_bit_comp = FALSE;

static bool_t afe_test_pattern_check = TRUE;

// Type 0 is the ramp
// Type 1 is max on left channel, min on right channel
static uint32 pattern_type = 0;

uint32_t num_sent_rx_client = 0;
uint32_t num_received_tx_client = 0;
uint32_t num_check_count = 0;
uint32_t num_mismatch_count = 0;
   
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
  
} afe_test_client_t;


afe_test_client_t afe_rx_client, afe_tx_client;



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
  };
} afeSetCmdInfo_t;

typedef struct {
   afe_svc_cmd_set_param_t afeSvcSetcmd;
   afe_svc_param_data_t    afeSvcParamHeader;
   union {
       afe_param_id_clock_set_t clkcfg;
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
      case AFE_I2S_PRI_TEST:
      {   
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_I2S_PRI_TEST");
         *pTx_port_id = AFE_PORT_ID_PRIMARY_MI2S_TX;
         *pRx_port_id = AFE_PORT_ID_PRIMARY_MI2S_RX;
      }
      break;
      case AFE_I2S_SEC_TEST:
      {   
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_I2S_SEC_TEST");
         *pTx_port_id = AFE_PORT_ID_SECONDARY_MI2S_TX;
         *pRx_port_id = AFE_PORT_ID_SECONDARY_MI2S_RX;
      }
      break;
      case AFE_I2S_PRI_SEC_TEST:
      {   
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_I2S_PRI_SEC_TEST");
         *pTx_port_id = AFE_PORT_ID_SECONDARY_MI2S_TX;
         *pRx_port_id = AFE_PORT_ID_PRIMARY_MI2S_RX;
      }
      break;
      case AFE_PCM_PRI_TEST:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_PCM_PRI_TEST");
         *pTx_port_id = AFE_PORT_ID_PRIMARY_PCM_TX;
         *pRx_port_id = AFE_PORT_ID_PRIMARY_PCM_RX;
      }
      break;
      case AFE_PCM_SEC_TEST:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_PCM_SEC_TEST");
         *pTx_port_id = AFE_PORT_ID_SECONDARY_PCM_TX;
         *pRx_port_id = AFE_PORT_ID_SECONDARY_PCM_RX;
      }
      break;
      case AFE_PCM_PRI_SEC_TEST:
      {   
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_PCM_PRI_SEC_TEST");
         *pTx_port_id = AFE_PORT_ID_SECONDARY_PCM_TX;
         *pRx_port_id = AFE_PORT_ID_PRIMARY_PCM_RX;
      }
      break;
     case AFE_TDM_PRI_SEC_TEST:
     {   
       MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_I2S_PRI_SEC_TEST");
       *pTx_port_id = AFE_PORT_ID_SECONDARY_TDM_TX;
       *pRx_port_id = AFE_PORT_ID_PRIMARY_TDM_RX;
     }
     break;
     case AFE_TDM_MAX_8K_PRI_SEC_TEST:
     {   
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_MAX_8K_PRI_SEC_TEST");
        *pTx_port_id = AFE_PORT_ID_SECONDARY_TDM_TX;
        *pRx_port_id = AFE_PORT_ID_PRIMARY_TDM_RX;
     }
     break;
     case AFE_TDM_MAX_PRI_TEST:
     {
		 MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_MAX_PRI_TEST");
		 *pTx_port_id = AFE_PORT_ID_PRIMARY_TDM_TX;
		 *pRx_port_id = AFE_PORT_ID_PRIMARY_TDM_RX;	  	
	  }
	  break;
	  case AFE_TDM_MAX1_PRI_TEST:
	  {
		 MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_MAX1_PRI_TEST");
		 *pTx_port_id = AFE_PORT_ID_PRIMARY_TDM_TX;
		 *pRx_port_id = AFE_PORT_ID_PRIMARY_TDM_RX; 	
	  }
	  break;
      case AFE_TDM_PRI_TEST:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_PRI_TEST");
         *pTx_port_id = AFE_PORT_ID_PRIMARY_TDM_TX;
         *pRx_port_id = AFE_PORT_ID_PRIMARY_TDM_RX;
      }
      break;
	  case AFE_TDM_PRI_32BITS_TEST:
	  { 	
		 MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_PRI_32BITS_TEST");
		 *pTx_port_id = AFE_PORT_ID_PRIMARY_TDM_TX;
		 *pRx_port_id = AFE_PORT_ID_PRIMARY_TDM_RX;
	  }
	  break;
      case AFE_TDM_SEC_TEST:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_SEC_TEST");
         *pTx_port_id = AFE_PORT_ID_SECONDARY_TDM_TX;
         *pRx_port_id = AFE_PORT_ID_SECONDARY_TDM_RX;
      }
      break;
      case AFE_TDM_TER_TEST:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_TER_TEST");
         *pTx_port_id = AFE_PORT_ID_TERTIARY_TDM_TX;
         *pRx_port_id = AFE_PORT_ID_TERTIARY_TDM_RX;
      }
      break;
      case AFE_TDM_QUA_TEST:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_QUA_TEST");
         *pTx_port_id = AFE_PORT_ID_QUATERNARY_TDM_TX;
         *pRx_port_id = AFE_PORT_ID_QUATERNARY_TDM_RX;
      }
      break;
	  case AFE_TDM_I2S_PRI_TEST:
	  { 	
		 MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_TDM_QUA_TEST");
		 *pTx_port_id = AFE_PORT_ID_PRIMARY_TDM_TX;
		 *pRx_port_id = AFE_PORT_ID_PRIMARY_TDM_RX;
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

static int32 getTestClkAfe(uint32_t testcase, uint16_t port_id, uint32_t *p_clock_attr, uint32_t *p_clock_id, uint32_t *p_clk_value)
{
   int32 rc = ADSP_EOK;

   // Default to use COUPLE_NO
   *p_clock_attr = AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO;

   switch(testcase)
   {
      case AFE_I2S_PRI_TEST:
      {   
           *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT;
           *p_clk_value = 1536000;
      }
      break;
      case AFE_I2S_SEC_TEST:
      {   
         *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT;
         *p_clk_value = 1536000;
      }
      break;
      case AFE_I2S_PRI_SEC_TEST:
      {   
         if(AFE_PORT_ID_PRIMARY_MI2S_RX == port_id)
         {
            *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT;
            *p_clk_value = 1536000;
         }
         else if(AFE_PORT_ID_SECONDARY_MI2S_TX == port_id)
         {
            *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_EBIT;
            *p_clk_value = 0;
         }            
      }
      break;
      case AFE_PCM_PRI_TEST:
      {   
         *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT;
         *p_clk_value = 2048000;
      }
      break;
      case AFE_PCM_SEC_TEST:
      {   
         *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT;
         *p_clk_value = 2048000;
      }
      break;
      case AFE_PCM_PRI_SEC_TEST:
      {   
         if(AFE_PORT_ID_PRIMARY_PCM_RX == port_id)
         {
            *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT;
            *p_clk_value = 2048000;
         }
         else if(AFE_PORT_ID_SECONDARY_PCM_TX == port_id)
         {
            *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_EBIT;
            *p_clk_value = 0;
         }            
      }
      break;
	  case AFE_TDM_I2S_PRI_TEST:
	  {
	  	// 16 bit samples in 24 bit slots @ 16 KHz
	  	// clock value = 16000 KHz x 24 bits x 2 channels
        *p_clock_attr = AFE_CLOCK_SET_CLOCK_ATTRIBUTE_INVERT_COUPLE_NO;
		*p_clock_id = AFE_CLOCK_SET_CLOCK_ID_PRI_TDM_IBIT;
		*p_clk_value = 768000;
	  }
	  break;
      case AFE_TDM_PRI_TEST:
      {   
         *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_PRI_TDM_IBIT;
         *p_clk_value = 2048000;
      }
      break;
      case AFE_TDM_PRI_32BITS_TEST:
      {   
         *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_PRI_TDM_IBIT;
         *p_clk_value = 2048000;
      }
      break;
      case AFE_TDM_SEC_TEST:
      {   
         *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_SEC_TDM_IBIT;
         *p_clk_value = 2048000;
      }
      break;
      case AFE_TDM_TER_TEST:
      {   
         *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_TER_TDM_IBIT;
         *p_clk_value = 2048000;
      }
      break;
      case AFE_TDM_QUA_TEST:
      {   
         *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_QUAD_TDM_IBIT;
         *p_clk_value = 2048000;
      }
      break;
      case AFE_TDM_PRI_SEC_TEST:
      {
         if(AFE_PORT_ID_PRIMARY_TDM_RX == port_id)
         {
            *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_PRI_TDM_IBIT;
            *p_clk_value = 2048000;
         }
         else if(AFE_PORT_ID_SECONDARY_TDM_TX == port_id)
         {
            *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_SEC_TDM_EBIT;
            *p_clk_value = 0;
         }            
      }
      break;
      case AFE_TDM_MAX_8K_PRI_SEC_TEST:
      {
         if(AFE_PORT_ID_PRIMARY_TDM_RX == port_id)
         {
            *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_PRI_TDM_IBIT;
            *p_clk_value = 2048000;
         }
         else if(AFE_PORT_ID_SECONDARY_TDM_TX == port_id)
         {
            *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_SEC_TDM_EBIT;
            *p_clk_value = 0;
         }            
      }
      break;
      case AFE_TDM_MAX_PRI_TEST:
      {
          *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_PRI_TDM_IBIT;
          *p_clk_value = 8192000;
      }
      break;
      case AFE_TDM_MAX1_PRI_TEST:
      {
          *p_clock_id = AFE_CLOCK_SET_CLOCK_ID_PRI_TDM_IBIT;
          *p_clk_value = 12288000;
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
      case AFE_I2S_PRI_TEST:
      case AFE_I2S_SEC_TEST:
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
         //
         pTxClient->samples_per_int = 48;
         pTxClient->bytes_per_channel = 2;
         pTxClient->num_channels = 2;
         pTxClient->bit_width = 16;
       
      }
      break;
      case AFE_I2S_PRI_SEC_TEST:
      {
         pAfePortCfgCmd->i2scfg.i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
         pAfePortCfgCmd->i2scfg.ws_src = AFE_PORT_CONFIG_I2S_WS_SRC_EXTERNAL;
         pAfePortCfgCmd->i2scfg.bit_width = 16;
         pAfePortCfgCmd->i2scfg.channel_mode = AFE_PORT_I2S_SD0;
         pAfePortCfgCmd->i2scfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->i2scfg.mono_stereo = AFE_PORT_I2S_STEREO;   //AFE_PORT_I2S_MONO;
         pAfePortCfgCmd->i2scfg.sample_rate = 48000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_I2S_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->i2scfg);

         pTxClient->samples_per_int = 48;
         pTxClient->bytes_per_channel = 2;
         pTxClient->num_channels = 2;
         pTxClient->bit_width = 16;
      }
      break;
      case AFE_PCM_PRI_TEST:
      {
         pAfePortCfgCmd->pcmCfg.pcm_cfg_minor_version = AFE_API_VERSION_PCM_CONFIG;
         pAfePortCfgCmd->pcmCfg.aux_mode  = AFE_PORT_PCM_AUX_MODE_PCM;
         pAfePortCfgCmd->pcmCfg.sync_src = AFE_PORT_PCM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->pcmCfg.frame_setting  = AFE_PORT_PCM_BITS_PER_FRAME_256;
         pAfePortCfgCmd->pcmCfg.quant_type = AFE_PORT_PCM_LINEAR_NOPADDING;
         pAfePortCfgCmd->pcmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->pcmCfg.sample_rate = 8000;
         pAfePortCfgCmd->pcmCfg.bit_width = 16;
         pAfePortCfgCmd->pcmCfg.num_channels = 2;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[0]= 1; //NOTE: slot number start from 1 at service layer.
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[1]= 2;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[2]= 0;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[3]= 0;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_PCM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->pcmCfg);

         pTxClient->samples_per_int = 8;
         pTxClient->bytes_per_channel = 2;
         pTxClient->num_channels = 2;
         pTxClient->bit_width = 16;
      }
      break;
      case AFE_PCM_SEC_TEST:
      {
         pAfePortCfgCmd->pcmCfg.pcm_cfg_minor_version = AFE_API_VERSION_PCM_CONFIG;
         pAfePortCfgCmd->pcmCfg.aux_mode  = AFE_PORT_PCM_AUX_MODE_PCM;
         pAfePortCfgCmd->pcmCfg.sync_src = AFE_PORT_PCM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->pcmCfg.frame_setting  = AFE_PORT_PCM_BITS_PER_FRAME_128;
         pAfePortCfgCmd->pcmCfg.quant_type = AFE_PORT_PCM_LINEAR_NOPADDING;
         pAfePortCfgCmd->pcmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->pcmCfg.sample_rate = 16000;
         pAfePortCfgCmd->pcmCfg.bit_width = 16;
         pAfePortCfgCmd->pcmCfg.num_channels = 2;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[0]= 1; //NOTE: slot number start from 1 at service layer.
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[1]= 2;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[2]= 0;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[3]= 0;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_PCM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->pcmCfg);
         
         pTxClient->samples_per_int = 16;
         pTxClient->bytes_per_channel = 2;
         pTxClient->num_channels = 2;
         pTxClient->bit_width = 16;
      }      
      break;
      case AFE_PCM_PRI_SEC_TEST:
      {
		  pAfePortCfgCmd->pcmCfg.pcm_cfg_minor_version = AFE_API_VERSION_PCM_CONFIG;
		  pAfePortCfgCmd->pcmCfg.aux_mode  = AFE_PORT_PCM_AUX_MODE_PCM;
		  pAfePortCfgCmd->pcmCfg.sync_src = AFE_PORT_PCM_SYNC_SRC_EXTERNAL;
		  pAfePortCfgCmd->pcmCfg.frame_setting	= AFE_PORT_PCM_BITS_PER_FRAME_128;
		  pAfePortCfgCmd->pcmCfg.quant_type = AFE_PORT_PCM_LINEAR_NOPADDING;
		  pAfePortCfgCmd->pcmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		  pAfePortCfgCmd->pcmCfg.sample_rate = 16000;
		  pAfePortCfgCmd->pcmCfg.bit_width = 16;
		  pAfePortCfgCmd->pcmCfg.num_channels = 2;
		  pAfePortCfgCmd->pcmCfg.slot_number_mapping[0]= 1; //NOTE: slot number start from 1 at service layer.
		  pAfePortCfgCmd->pcmCfg.slot_number_mapping[1]= 2;
		  pAfePortCfgCmd->pcmCfg.slot_number_mapping[2]= 0;
		  pAfePortCfgCmd->pcmCfg.slot_number_mapping[3]= 0;
		  pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		  pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_PCM_CONFIG;
		  pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->pcmCfg);
		  
		  pTxClient->samples_per_int = 16;
		  pTxClient->bytes_per_channel = 2;
		  pTxClient->num_channels = 2;
		  pTxClient->bit_width = 16;
      }
      break;
	  case AFE_TDM_PRI_SEC_TEST:
	  {
		 pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
		 pAfePortCfgCmd->tdmCfg.num_channels = 2;
		 pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_8K;
		 pAfePortCfgCmd->tdmCfg.bit_width = 16;
		 pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
		 pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
		 pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_EXTERNAL;
		 pAfePortCfgCmd->tdmCfg.nslots_per_frame = 16;
		 pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		 pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
		 pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
		 pAfePortCfgCmd->tdmCfg.slot_width = 16;
		 pAfePortCfgCmd->tdmCfg.slot_mask = 0x0804;
		  
		 pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		 pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
		 pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
	  
		 pTxClient->samples_per_int = 8;
		 pTxClient->bytes_per_channel = 2;
		 pTxClient->num_channels = 2;
		 pTxClient->bit_width = 16;
	  }
	  break;
	  case AFE_TDM_MAX_8K_PRI_SEC_TEST:
	  {
		 pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
		 pAfePortCfgCmd->tdmCfg.num_channels = 8;
		 pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_8K;
		 pAfePortCfgCmd->tdmCfg.bit_width = 32;
		 pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
		 pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
		 pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_EXTERNAL;
		 pAfePortCfgCmd->tdmCfg.nslots_per_frame = 8;
		 pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		 pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
		 pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
		 pAfePortCfgCmd->tdmCfg.slot_width = 32;
		 pAfePortCfgCmd->tdmCfg.slot_mask = 0x00FF;
		  
		 pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		 pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
		 pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
	  
		 pTxClient->samples_per_int = 8;
		 pTxClient->bytes_per_channel = 4;
		 pTxClient->num_channels = 8;
		 pTxClient->bit_width = 32;
	  }
	  break;
	  case AFE_TDM_MAX_PRI_TEST:
	  {
		 pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
		 pAfePortCfgCmd->tdmCfg.num_channels = 8;
		 pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_32K;
		 pAfePortCfgCmd->tdmCfg.bit_width = 32;
		 pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
		 pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
		 pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
		 pAfePortCfgCmd->tdmCfg.nslots_per_frame = 8;
		 pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		 pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
		 pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
		 pAfePortCfgCmd->tdmCfg.slot_width = 32;
		 pAfePortCfgCmd->tdmCfg.slot_mask = 0x00FF;
		  
		 pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		 pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
		 pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
	  
		 pTxClient->samples_per_int = 32;
		 pTxClient->bytes_per_channel = 4;
		 pTxClient->num_channels = 8;
		 pTxClient->bit_width = 32;
	  }
	  break;
	  case AFE_TDM_MAX1_PRI_TEST:
	  {
		 pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
		 pAfePortCfgCmd->tdmCfg.num_channels = 8;
		 pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_48K;
		 pAfePortCfgCmd->tdmCfg.bit_width = 32;
		 pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
		 pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
		 pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
		 pAfePortCfgCmd->tdmCfg.nslots_per_frame = 8;
		 pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		 pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
		 pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
		 pAfePortCfgCmd->tdmCfg.slot_width = 32;
		 pAfePortCfgCmd->tdmCfg.slot_mask = 0x00FF;
		  
		 pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		 pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
		 pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
	  
		 pTxClient->samples_per_int = 48;
		 pTxClient->bytes_per_channel = 4;
		 pTxClient->num_channels = 8;
		 pTxClient->bit_width = 32;
	  }
	  break;
      case AFE_TDM_I2S_PRI_TEST:
      {
         pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
         pAfePortCfgCmd->tdmCfg.num_channels = 2;
         pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_16K;
         pAfePortCfgCmd->tdmCfg.bit_width = 16;
         pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
		  pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_LONG_SYNC_MODE;
		  pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
		  pAfePortCfgCmd->tdmCfg.nslots_per_frame = 2;
		  pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		  pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_INVERT;
		  pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
		  pAfePortCfgCmd->tdmCfg.slot_width = 24;
		  pAfePortCfgCmd->tdmCfg.slot_mask = 0x3;
		  
		  pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		  pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
		  pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
		  
		  pTxClient->samples_per_int = 16;
		  pTxClient->bytes_per_channel = 2;
		  pTxClient->num_channels = 2;
		  pTxClient->bit_width = 16;
      }
	  break;
	  case AFE_TDM_PRI_32BITS_TEST:
	  {
		 pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
		 pAfePortCfgCmd->tdmCfg.num_channels = 2;
		 pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_8K;
		 pAfePortCfgCmd->tdmCfg.bit_width = 32;
		 pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
		 pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
		 pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
		 pAfePortCfgCmd->tdmCfg.nslots_per_frame = 8;
		 pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		 pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
		 pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
		 pAfePortCfgCmd->tdmCfg.slot_width = 32;
		 pAfePortCfgCmd->tdmCfg.slot_mask = 0x3;
		  
		 pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		 pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
		 pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
	  
		 pTxClient->samples_per_int = 8;
		 pTxClient->bytes_per_channel = 4;
		 pTxClient->num_channels = 2;
		 pTxClient->bit_width = 32;
	  }
	  break;	  
      case AFE_TDM_PRI_TEST: // 16 slots x 16 bits x 8 K.. this force the bit rate to 2048 000.. bit width == slot witdh
      case AFE_TDM_TER_TEST:
      {
         pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
         pAfePortCfgCmd->tdmCfg.num_channels = 2;
         pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_8K;
         pAfePortCfgCmd->tdmCfg.bit_width = 16;
         pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
         pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->tdmCfg.nslots_per_frame = 16;
         pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
         pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
         pAfePortCfgCmd->tdmCfg.slot_width = 16;
         pAfePortCfgCmd->tdmCfg.slot_mask = 0x3;

         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);

         pTxClient->samples_per_int = 8;
         pTxClient->bytes_per_channel = 2;
         pTxClient->num_channels = 2;
         pTxClient->bit_width = 16;
      }
      break;
      case AFE_TDM_SEC_TEST: // 8 slots x 16 bits x 16 K this force the bit rate to 2048 000.. bit width == slot witdh
      case AFE_TDM_QUA_TEST:
      {
         pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
         pAfePortCfgCmd->tdmCfg.num_channels = 2;
         pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_16K;
         pAfePortCfgCmd->tdmCfg.bit_width = 16;
         pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
         pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->tdmCfg.nslots_per_frame = 16;
         pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
         pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
         pAfePortCfgCmd->tdmCfg.slot_width = 16;
         pAfePortCfgCmd->tdmCfg.slot_mask = 0x3;
  
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
  
         pTxClient->samples_per_int = 16;
         pTxClient->bytes_per_channel = 2;
         pTxClient->num_channels = 2;
         pTxClient->bit_width = 16;
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

   switch(my_afe_test_case)
   {
      case AFE_I2S_PRI_TEST:
      case AFE_I2S_SEC_TEST:
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
      case AFE_I2S_PRI_SEC_TEST:
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
      case AFE_PCM_PRI_TEST:
      {
         pAfePortCfgCmd->pcmCfg.pcm_cfg_minor_version = AFE_API_VERSION_PCM_CONFIG;
         pAfePortCfgCmd->pcmCfg.aux_mode  = AFE_PORT_PCM_AUX_MODE_PCM;
         pAfePortCfgCmd->pcmCfg.sync_src = AFE_PORT_PCM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->pcmCfg.frame_setting  = AFE_PORT_PCM_BITS_PER_FRAME_256;
         pAfePortCfgCmd->pcmCfg.quant_type = AFE_PORT_PCM_LINEAR_NOPADDING;
         pAfePortCfgCmd->pcmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->pcmCfg.sample_rate = 8000;
         pAfePortCfgCmd->pcmCfg.bit_width = 16;
         pAfePortCfgCmd->pcmCfg.num_channels = 2;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[0]= 1; //NOTE: slot number start from 1 at service layer.
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[1]= 2;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[2]= 0;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[3]= 0;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_PCM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->pcmCfg);

         pRxClient->samples_per_int = 8;
         pRxClient->bytes_per_channel = 2;
         pRxClient->num_channels = 2;
         pRxClient->bit_width = 16;
        
      }
      break;
      case AFE_PCM_SEC_TEST:
      {
         pAfePortCfgCmd->pcmCfg.pcm_cfg_minor_version = AFE_API_VERSION_PCM_CONFIG;
         pAfePortCfgCmd->pcmCfg.aux_mode  = AFE_PORT_PCM_AUX_MODE_PCM;
         pAfePortCfgCmd->pcmCfg.sync_src = AFE_PORT_PCM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->pcmCfg.frame_setting  = AFE_PORT_PCM_BITS_PER_FRAME_128;
         pAfePortCfgCmd->pcmCfg.quant_type = AFE_PORT_PCM_LINEAR_NOPADDING;
         pAfePortCfgCmd->pcmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->pcmCfg.sample_rate = 16000;
         pAfePortCfgCmd->pcmCfg.bit_width = 16;
         pAfePortCfgCmd->pcmCfg.num_channels = 2;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[0]= 1; //NOTE: slot number start from 1 at service layer.
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[1]= 2;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[2]= 0;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[3]= 0;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_PCM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->pcmCfg);

         pRxClient->samples_per_int = 16;
         pRxClient->bytes_per_channel = 2;
         pRxClient->num_channels = 2;
         pRxClient->bit_width = 16;         
      }      
      break;
      case AFE_PCM_PRI_SEC_TEST:
      {
		  pAfePortCfgCmd->pcmCfg.pcm_cfg_minor_version = AFE_API_VERSION_PCM_CONFIG;
		  pAfePortCfgCmd->pcmCfg.aux_mode  = AFE_PORT_PCM_AUX_MODE_PCM;
		  pAfePortCfgCmd->pcmCfg.sync_src = AFE_PORT_PCM_SYNC_SRC_INTERNAL;
		  pAfePortCfgCmd->pcmCfg.frame_setting	= AFE_PORT_PCM_BITS_PER_FRAME_128;
		  pAfePortCfgCmd->pcmCfg.quant_type = AFE_PORT_PCM_LINEAR_NOPADDING;
		  pAfePortCfgCmd->pcmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		  pAfePortCfgCmd->pcmCfg.sample_rate = 16000;
		  pAfePortCfgCmd->pcmCfg.bit_width = 16;
		  pAfePortCfgCmd->pcmCfg.num_channels = 2;
		  pAfePortCfgCmd->pcmCfg.slot_number_mapping[0]= 1; //NOTE: slot number start from 1 at service layer.
		  pAfePortCfgCmd->pcmCfg.slot_number_mapping[1]= 2;
		  pAfePortCfgCmd->pcmCfg.slot_number_mapping[2]= 0;
		  pAfePortCfgCmd->pcmCfg.slot_number_mapping[3]= 0;
		  pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		  pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_PCM_CONFIG;
		  pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->pcmCfg);
		  
		  pRxClient->samples_per_int = 16;
		  pRxClient->bytes_per_channel = 2;
		  pRxClient->num_channels = 2;
		  pRxClient->bit_width = 16;		 
      }
      break;
	  case AFE_TDM_PRI_SEC_TEST:
	  {
		 pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
		 pAfePortCfgCmd->tdmCfg.num_channels = 2;
		 pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_8K;
		 pAfePortCfgCmd->tdmCfg.bit_width = 16;
		 pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
		 pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
		 pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
		 pAfePortCfgCmd->tdmCfg.nslots_per_frame = 16;
		 pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		 pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
		 pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
		 pAfePortCfgCmd->tdmCfg.slot_width = 16;
		 pAfePortCfgCmd->tdmCfg.slot_mask = 0x0804; // 0000 1000 0000 0100
		  
		 pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		 pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
		 pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
	  
		 pRxClient->samples_per_int = 8;
		 pRxClient->bytes_per_channel = 2;
		 pRxClient->num_channels = 2;
		 pRxClient->bit_width = 16;
	  }
	  break;
	  case AFE_TDM_MAX_8K_PRI_SEC_TEST:
	  {
		 pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
		 pAfePortCfgCmd->tdmCfg.num_channels = 8;
		 pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_8K;
		 pAfePortCfgCmd->tdmCfg.bit_width = 32;
		 pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
		 pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
		 pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
		 pAfePortCfgCmd->tdmCfg.nslots_per_frame = 8;
		 pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		 pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
		 pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
		 pAfePortCfgCmd->tdmCfg.slot_width = 32;
		 pAfePortCfgCmd->tdmCfg.slot_mask = 0x00FF;
		  
		 pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		 pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
		 pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
	  
		 pRxClient->samples_per_int = 8;
		 pRxClient->bytes_per_channel = 4;
		 pRxClient->num_channels = 8;
		 pRxClient->bit_width = 32;
	  }
	  break;
	  case AFE_TDM_MAX_PRI_TEST:
	  {
		 pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
		 pAfePortCfgCmd->tdmCfg.num_channels = 8;
		 pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_32K;
		 pAfePortCfgCmd->tdmCfg.bit_width = 32;
		 pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
		 pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
		 pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
		 pAfePortCfgCmd->tdmCfg.nslots_per_frame = 8;
		 pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		 pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
		 pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
		 pAfePortCfgCmd->tdmCfg.slot_width = 32;
		 pAfePortCfgCmd->tdmCfg.slot_mask = 0x00FF;
		  
		 pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		 pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
		 pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
	  
		 pRxClient->samples_per_int = 32;
		 pRxClient->bytes_per_channel = 4;
		 pRxClient->num_channels = 8;
		 pRxClient->bit_width = 32;
	  }
	  break;
	  case AFE_TDM_MAX1_PRI_TEST:
	  {
		 pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
		 pAfePortCfgCmd->tdmCfg.num_channels = 8;
		 pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_48K;
		 pAfePortCfgCmd->tdmCfg.bit_width = 32;
		 pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
		 pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
		 pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
		 pAfePortCfgCmd->tdmCfg.nslots_per_frame = 8;
		 pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		 pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
		 pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
		 pAfePortCfgCmd->tdmCfg.slot_width = 32;
		 pAfePortCfgCmd->tdmCfg.slot_mask = 0x00FF;
		  
		 pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		 pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
		 pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
	  
		 pRxClient->samples_per_int = 48;
		 pRxClient->bytes_per_channel = 4;
		 pRxClient->num_channels = 8;
		 pRxClient->bit_width = 32;
	  }
	  break;
	  case AFE_TDM_I2S_PRI_TEST:
	  {
	    pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
	    pAfePortCfgCmd->tdmCfg.num_channels = 2;
	    pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_16K;
	    pAfePortCfgCmd->tdmCfg.bit_width = 16;
	    pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
	    pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_LONG_SYNC_MODE;
	    pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
	    pAfePortCfgCmd->tdmCfg.nslots_per_frame = 2;
	    pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
	    pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_INVERT;
	    pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
	    pAfePortCfgCmd->tdmCfg.slot_width = 24;
	    pAfePortCfgCmd->tdmCfg.slot_mask = 0x3;
	  
	    pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
	    pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
	    pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
	  
	    pRxClient->samples_per_int = 16;
	    pRxClient->bytes_per_channel = 2;
	    pRxClient->num_channels = 2;
	    pRxClient->bit_width = 16;
	  }
	  break;
	  case AFE_TDM_PRI_32BITS_TEST:
	  {
		 pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
		 pAfePortCfgCmd->tdmCfg.num_channels = 2;
		 pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_8K;
		 pAfePortCfgCmd->tdmCfg.bit_width = 32;
		 pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
		 pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
		 pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
		 pAfePortCfgCmd->tdmCfg.nslots_per_frame = 8;
		 pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
		 pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
		 pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
		 pAfePortCfgCmd->tdmCfg.slot_width = 32;
		 pAfePortCfgCmd->tdmCfg.slot_mask = 0x3;
		  
		 pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
		 pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
		 pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
		 
		 pRxClient->samples_per_int = 8;
		 pRxClient->bytes_per_channel = 4;
		 pRxClient->num_channels = 2;
		 pRxClient->bit_width = 32;
	  }
	  break;	  	
      case AFE_TDM_PRI_TEST: // 16 slots x 16 bits x 8 K.. this force the bit rate to 2048 000.. bit width == slot witdh
      case AFE_TDM_TER_TEST:
      {
         pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
         pAfePortCfgCmd->tdmCfg.num_channels = 2;
         pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_8K;
         pAfePortCfgCmd->tdmCfg.bit_width = 16;
         pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
         pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->tdmCfg.nslots_per_frame = 16;
         pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
         pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
         pAfePortCfgCmd->tdmCfg.slot_width = 16;
         pAfePortCfgCmd->tdmCfg.slot_mask = 0x3;

         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);

         pRxClient->samples_per_int = 8;
         pRxClient->bytes_per_channel = 2;
         pRxClient->num_channels = 2;
         pRxClient->bit_width = 16;
      }
      break;
      case AFE_TDM_SEC_TEST: // 8 slots x 16 bits x 16 K this force the bit rate to 2048 000.. bit width == slot witdh
      case AFE_TDM_QUA_TEST:
      {
         pAfePortCfgCmd->tdmCfg.tdm_cfg_minor_version = AFE_API_VERSION_TDM_CONFIG;
         pAfePortCfgCmd->tdmCfg.num_channels = 2;
         pAfePortCfgCmd->tdmCfg.sample_rate = AFE_PORT_SAMPLE_RATE_16K;
         pAfePortCfgCmd->tdmCfg.bit_width = 16;
         pAfePortCfgCmd->tdmCfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->tdmCfg.sync_mode = AFE_PORT_TDM_SHORT_SYNC_BIT_MODE;
         pAfePortCfgCmd->tdmCfg.sync_src = AFE_PORT_TDM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->tdmCfg.nslots_per_frame = 16;
         pAfePortCfgCmd->tdmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->tdmCfg.ctrl_invert_sync_pulse = AFE_PORT_TDM_SYNC_NORMAL;
         pAfePortCfgCmd->tdmCfg.ctrl_sync_data_delay = AFE_PORT_TDM_DATA_DELAY_1_BCLK_CYCLE;
         pAfePortCfgCmd->tdmCfg.slot_width = 16;
         pAfePortCfgCmd->tdmCfg.slot_mask = 0x3;
          
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_TDM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->tdmCfg);
         
         pRxClient->samples_per_int = 16;
         pRxClient->bytes_per_channel = 2;
         pRxClient->num_channels = 2;
         pRxClient->bit_width = 16;
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

   queue_buf_ptr = (char*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(4),QURT_ELITE_HEAP_DEFAULT);
   if (NULL == queue_buf_ptr)
   {
     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Test data Q Creation Failed");
     return ADSP_ENOMEMORY;
   }
   
	pClient->svc_handle.dataQ =  (qurt_elite_queue_t *)(queue_buf_ptr);

   if (ADSP_FAILED(result = qurt_elite_queue_init(afeTestDataQName, 2, pClient->svc_handle.dataQ)))
   {
		MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Test: Error: Failed to create AFE test dataQ");
		return result;
	}

  	qurt_elite_channel_init(&pClient->afeTxClientBufChannel); 

   if (ADSP_FAILED(result = qurt_elite_channel_addq(&(pClient->afeTxClientBufChannel), pClient->svc_handle.dataQ, 0))) 
   {
		MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Test: Error: Failed to add AFE test dataQ to channel");
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

   queue_buf_ptr = (char*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(4),QURT_ELITE_HEAP_DEFAULT);
   if (NULL == queue_buf_ptr)
   {
     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Test response Creation Failed");
     return ADSP_ENOMEMORY;
   }
   
	pClient->afeClientResponseQ =  (qurt_elite_queue_t *)(queue_buf_ptr);
   if(ADSP_FAILED(result = qurt_elite_queue_init(responseQName, 2, pClient->afeClientResponseQ)))
	{
		MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Test: Error: Failed to create AFE test respQ");
		return result;
	}

  	qurt_elite_channel_init(&pClient->afeClientChannel); 

	if (ADSP_FAILED(result = qurt_elite_channel_addq(&pClient->afeClientChannel, pClient->afeClientResponseQ, 0)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "ADM: Error: Failed to add responseQ to channel, result = %d", result);
		return(result);
	}


   if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(elite_msg_custom_afe_connect_t), &bufMgrNode, &nActualSize)))
   {
      MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "AFE Test: Failed to create mesage");
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
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Test: Failed to get AFE cmdQ ptr");
      goto __bailout;

   }

   afeCmdQ = qurt_elite_globalstate.pAfeStatSvcCmdQ;
   if (ADSP_FAILED(result = qurt_elite_queue_push_back(afeCmdQ, (uint64_t*)&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Test: Failed to push cmd to AFE");
      goto __bailout;
   }

   if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Test: Wait for ack failed");
      goto __bailout;
   }

   result = (ADSPResult)pConnectMsgPayload->response_result;
   if (ADSP_EOK != result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Test: Error [0x%x] in cmd ack", result);
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
       MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "AFE Test: Failed to create mesage");
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
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Test: Failed to get AFE cmdQ ptr");
       goto __bailoutCmdDisconnectAFE;
    }
    afeCmdQ = qurt_elite_globalstate.pAfeStatSvcCmdQ;
    if (ADSP_FAILED(result = qurt_elite_queue_push_back(afeCmdQ, (uint64_t*)&msg)))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Test: Failed to push cmd to AFE");
       goto __bailoutCmdDisconnectAFE;
    }

    if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Test: Wait for ack failed");
       goto __bailoutCmdDisconnectAFE;
    }

    result = (ADSPResult)pConnectMsgPayload->response_result;
    if (ADSP_EOK != result)
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Test: Error [0x%x] in cmd ack", result);
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
   pMultiChMediaFormatBlk->bits_per_sample = pClient->bit_width; //16; //(uint16_t) ((pClientInfo->afe_client.bytes_per_channel)<<3);
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
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FAILED to connect services!!\n");
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

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending EoS: port id : 0x%x", pClient->port_id);

   /* Send the EoS */
   if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_data_eos_header_t), &buf_mgr_node, &actual_size)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get buffer mgr node while sending EoS.");
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
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver EoS dowstream. Dropping");
      elite_msg_return_payload_buffer(&eos_data_msg);
   }

   return result;
}


static ADSPResult afe_test_port_create_buf(afe_test_client_t *pClient)
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

  queue_buf_ptr = (char*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(4),QURT_ELITE_HEAP_DEFAULT);

  if (NULL == queue_buf_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Port bufQ Creation Failed");
    return ADSP_ENOMEMORY;
  }

  pClient->bufQ = (qurt_elite_queue_t*) queue_buf_ptr;

  qurt_elite_channel_init(&pClient->afeClientChannel);
  qurt_elite_channel = &pClient->afeClientChannel;
  
  // Two items can be pushed on the queue
  if (ADSP_FAILED(result = qurt_elite_queue_init(bufQName, 2, pClient->bufQ)) ||
      ADSP_FAILED(result = qurt_elite_channel_addq(qurt_elite_channel, pClient->bufQ, 0)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to init bufQ for client or add bufQ to channel %d!!\n", result);
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
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Out of Memory!!\n");
      return ADSP_ENOMEMORY;
    }

    memset(psPcmBuff,0,reqSize);

    psPcmBuff->nActualSize     = actualSize;
    psPcmBuff->nMaxSize        = psPcmBuff->nActualSize;
    psPcmBuff->pBufferReturnQ  = pClient->bufQ;

    int16_t * pPattern = (int16_t *)&(psPcmBuff->nDataBuf);

    // this patthern are only for 2 channels
    // It will work for other number of channels or sample size..
    // Just the data is not easy to interpret
    for(int16_t i=0; i <  actualSize/2; i++)
    {
      if(i < actualSize/4)
      {
         if(0 == pattern_type)
         {
         *(pPattern+i) = i%(pClient->samples_per_int)+1; //Left
      }
      else
      {
		   *(pPattern+i) = 0xFFFF;
		 }
      }
      else
      {
         if(0 == pattern_type)
		 {
         *(pPattern+i) = i%(pClient->samples_per_int)+0x0100+1; // Right
      }
		 else
		 {
		   *(pPattern+i) = 0xAAAA;
		 }
      }
    }
    
    if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(pClient->bufQ, (elite_msg_any_payload_t*) psPcmBuff)))
    {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to fill ELiteExample buffer queue!! \n");
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
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver buffer dowstream. Dropping!\n");
        (void) elite_msg_push_payload_to_returnq(pClient->bufQ, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
      }
      else
      {
         num_sent_rx_client++;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver buffer dowstream. Dropping!\n");
   }
}

static int afe_test_rx_client_thread(void* arg)
{
   int32 rc = ADSP_EOK;
   afe_test_client_t *p_afe_rx_client =  (afe_test_client_t *)arg;//&afe_rx_client; //

   MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "afe_test_rx_client_thread 0x%x 0x%x",arg, &afe_rx_client);

   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "getTestAfePortId Failed, result=%ld",rc);
      return rc;
   }

   if(ADSP_FAILED(rc = Afe_Test_SendCmdConnectAFE(p_afe_rx_client)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "RX port connection failed %d",rc);
      return rc;
   }


   if(ADSP_FAILED(rc = afe_test_port_send_media_t(p_afe_rx_client)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "RX port meida type update failed %d",rc);
      return rc;
   }

   if(ADSP_FAILED(rc = afe_test_port_create_buf(p_afe_rx_client)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "RX port meida type update failed %d",rc);
      return rc;
   }

   for(int32_t i =0; i < 1000; i++)
   {
      afe_test_send_pcm_samples(p_afe_rx_client);
   }

   if(ADSP_FAILED(rc = afe_ext_generate_eos(p_afe_rx_client)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "RX port meida type update failed %d",rc);
      return rc;
   }

   afe_test_stop_bit_comp = TRUE;

   if(ADSP_FAILED(rc = Afe_Test_SendCmdDisconnectAFE(p_afe_rx_client)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "RX port diconnect failed %d",rc);
      return rc;
   }

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RX client thread exits");

   return rc;
}

/**
 * gets the bufs from queue
 */
 
static ADSPResult afe_test_receive_bufs(afe_test_client_t *pClient)
{
   uint32_t                          unChannelStatus;
   elite_msg_data_buffer_t           *data_buffer_ptr;
   int16_t                           *pPattern; 
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
           MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failure on client_dataQ %p qurt_elite_queue_pop_front: %d", \
                 (client_dataQ), result);
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

                  pPattern = (int16_t*)(&(data_buffer_ptr->nDataBuf));

                  num_received_tx_client++;

                  if(afe_test_pattern_check)
                  {
                     //***************************************************************************
                     // Verify samples from the source.
                     //***************************************************************************
                     //Ex) 48Khz, 2channel, 16bit: 1~240 (0x1E0)...
                     if(0 != (*pPattern) && (FALSE == afe_test_stop_bit_comp))
                     {                  
                        num_check_count++;
                        for(int16_t i=0; i <  data_buffer_ptr->nActualSize/2; i++)
                        {
                           if(i < data_buffer_ptr->nActualSize/4)
                           {
                              if(0 == pattern_type)
                              {
                              if(*(pPattern+i) != ((i+(*pPattern-1))%pClient->samples_per_int+1)) //Left
                              {
                                   num_mismatch_count++;
                                
                                   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Pattern mismatch on left channel (%04X) (%04X)",
								   	 *(pPattern+i), ((i+(*pPattern-1))%pClient->samples_per_int+1));
                                   //result = ADSP_EFAILED;
                                   break;
                                }
                              }
							  else
							  {
								  if(*(pPattern+i) != (int16_t) 0xFFFF) //Left
								  {
  								     num_mismatch_count++;
									 
									 MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Pattern mismatch on left channel (%04X) (%04X)",
									   *(pPattern+i), ((i+(*pPattern-1))%pClient->samples_per_int+1));
									 //result = ADSP_EFAILED;
                                 break;
                              }
                           }
                           }
                           else
                           {
							  if(0 == pattern_type)
							  {
                              if(*(pPattern+i) != ((i+(*pPattern-1))%pClient->samples_per_int+0x0100+1)) // Right
                              {                       
                    				  num_mismatch_count++;
									  
	                                 MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Pattern mismatch on right channel (%04X) (%04X)",
									  *(pPattern+i),   ((i+(*pPattern-1))%pClient->samples_per_int+0x0100+1));
	                                 //result = ADSP_EFAILED;
	                                 break;
	                              }
							  }
							  else
							  {
								  if(*(pPattern+i) != (int16_t) 0xAAAA) //right
								  {
									  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Pattern mismatch on right channel (%04X) (%04X)",
									   *(pPattern+i),	((i+(*pPattern-1))%pClient->samples_per_int+0x0100+1));
									 //result = ADSP_EFAILED;
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
                  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Received MEDIA_TYPE on DataQ");
                  elite_msg_data_media_type_apr_t *pDataMedia = (elite_msg_data_media_type_apr_t*)myDataQMsg.pPayload;
                  if (pDataMedia->unMediaTypeFormat != ELITEMSG_DATA_MEDIA_TYPE_APR)
                  {
                     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort: Error! Unknown unMediaTypeFormat!!!\n");
                  }
                  else
                  {
                    elite_multi_channel_pcm_fmt_blk_t *pMultiChMediaFormatBlk = (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pDataMedia);
                    MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "sample rate %d, numch %d, bitwidth %d, interleaved %d, signed %d",
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
                  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Received EOS on DataQ");
                  eos_received = TRUE;
                  elite_msg_return_payload_buffer(&myDataQMsg);
                  break;
               }
            default:
               // return back any stray payload
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "#### Received unexpected Data command on tx client DataQ");
               elite_msg_return_payload_buffer(&myDataQMsg);
               break;
         }

         if(eos_received)
         {
           if(0 == num_check_count)
           {
             result = ADSP_EFAILED;
           }
           break;
         }
		 if(ADSP_EOK != result)
         {
            break;
         }
      }
   }

   return result;;
}

static int afe_test_tx_client_thread(void* arg)
{
   int32 rc = ADSP_EOK;
   afe_test_client_t *p_afe_tx_client =  (afe_test_client_t *)arg;

   
   MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "afe_test_tx_client_thread 0x%x 0x%x",arg, &afe_tx_client);

   if(ADSP_FAILED(rc = afe_test_create_svc_handle(p_afe_tx_client)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "afe_test_create_svc_handle failed %d",rc);
      return rc;
   }


   if(ADSP_FAILED(rc = Afe_Test_SendCmdConnectAFE(p_afe_tx_client)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "TX port connection failed %d",rc);
      return rc;
   }

   if(ADSP_FAILED(rc = afe_test_receive_bufs(p_afe_tx_client)))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "######## TX port pattern mismatched  failed %d with TestCount %d %d #######",rc, num_check_count, num_mismatch_count);
   }
   else
   {
      MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "######## TX port pattern matached  passed!!!!!!!! %d with TestCount %d #######",rc, num_check_count, num_mismatch_count);
   }

   if(ADSP_FAILED(rc = Afe_Test_SendCmdDisconnectAFE(p_afe_tx_client)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "TX port diconnect failed %d",rc);
      return rc;
   }
   
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "TX client thread exits");

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


static int afe_svc_apr_ext_loopback_test(void* afeCmdQueue)
{

   int32 rc = ADSP_EOK;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"APR AFE external loopback test start");

   // apr registration
   rc = apr_event_create(&my_work_done_event);
   rc = elite_apr_if_register_by_name(
           &my_testclient_apr_handle, &my_testclient_apr_addr, (char*)"local.testclient", sizeof("local.testclient"), testclient_isr_recv_fn, NULL  );
   rc = elite_apr_if_lookup_addr_by_name( (char*)"qcom.adsp.afe", sizeof("qcom.adsp.afe"), &my_afe_addr);

   // loop back ports
   uint16_t tx_port_id = AFE_PORT_ID_INVALID;
   uint16_t rx_port_id = AFE_PORT_ID_INVALID;

   rc = getTestAfePortId(my_afe_test_case, &tx_port_id, &rx_port_id);
   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "getTestAfePortId Failed, result=%ld",rc);
      return rc;
   }

   uint32_t clock_attr, clock_id, clk_value;

   // New clock API config 48000 Hz x 2 channels x 16 bits = 1536000 bps
   // Get the clock cfg info for rx
   getTestClkAfe(my_afe_test_case, rx_port_id, &clock_attr, &clock_id, &clk_value);
   afeTestAfeClkConfig(clock_id, clk_value, clock_attr, 1);
   
   // Get the clock cfg info for tx
   getTestClkAfe(my_afe_test_case, tx_port_id, &clock_attr, &clock_id, &clk_value);
   afeTestAfeClkConfig(clock_id, clk_value, clock_attr, 1);

   /* Send the AFE Tx Port configure command to afe service. */
   afeSetCmdInfo_t afePortCfgCmd;
   memset(&afePortCfgCmd, 0, sizeof(afePortCfgCmd));

   // Get the driver config informtaion
   rc = getTestAfeTxPortConfig(my_afe_test_case, &afePortCfgCmd, &afe_tx_client);
   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "getTestAfeTxPortConfig Failed, result=%ld",rc);
      return rc;
   }

   afePortCfgCmd.afeSetcmd.payload_size = sizeof(afePortCfgCmd.afeParamHeader) + afePortCfgCmd.afeParamHeader.param_size;
   afePortCfgCmd.afeSetcmd.port_id = tx_port_id;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE Tx Port configure Command");

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
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe Port Tx Port configure Command Failed, result=%ld",rc);
   }

   my_afe_token++;

   /* Send the AFE Rx port configure command to afe service.*/
   memset(&afePortCfgCmd, 0, sizeof(afePortCfgCmd));

   rc = getTestAfeRxPortConfig(my_afe_test_case, &afePortCfgCmd, &afe_rx_client);
   if( ADSP_FAILED( rc ))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "getTestAfeRxPortConfig Failed, result=%ld",rc);
      return rc;
   }

   afePortCfgCmd.afeSetcmd.payload_size = sizeof(afePortCfgCmd.afeParamHeader) + afePortCfgCmd.afeParamHeader.param_size;
   afePortCfgCmd.afeSetcmd.port_id = rx_port_id;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE Rx Port configure Command");

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
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe Port Rx Port configure Command Failed, result=%ld",rc);
   }

   my_afe_token++;


   /* Send the start command to afe service. */
   afe_port_cmd_device_start_t   afePortStartCmd;
   afePortStartCmd.port_id = tx_port_id;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE Tx Port Start Command");

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
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe Port Tx Start Command Failed, result=%ld",rc);
   }

   my_afe_token++;

   /* Send the secondary i2s rx start command to afe service.*/
   afePortStartCmd.port_id = rx_port_id;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE Rx Port Start Command");

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
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe Port Rx Start Command Failed, result=%ld",rc);
   }

   my_afe_token++;

   //////////////////////////////////////////
   afe_rx_client.port_id = rx_port_id;
   afe_rx_client.data_path = AFE_RX_AUD_MIXER_IN;

   rc = qurt_elite_thread_launch(&afe_rx_client.tid, (char*)"rx_test_client", \
           NULL, 4096, 120, afe_test_rx_client_thread, &afe_rx_client, QURT_ELITE_HEAP_DEFAULT);


   afe_tx_client.port_id = tx_port_id;
   afe_tx_client.data_path = AFE_PP_OUT;

   rc = qurt_elite_thread_launch(&afe_tx_client.tid, (char*)"tx_test_client", \
           NULL, 4096, 120, afe_test_tx_client_thread, &afe_tx_client, QURT_ELITE_HEAP_DEFAULT);


   qurt_elite_thread_join(afe_rx_client.tid, &afe_rx_client.thread_result);
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe rx client is ending");

 
   /* Send the AFE port stop command to afe service. */
   afe_port_cmd_device_stop_t   afePortStopCmd;
   afePortStopCmd.port_id = tx_port_id;
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE Port Tx Stop Command");

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
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe Port Stop Tx Command Failed, result=%ld",rc);
   }

   my_afe_token++;

   /* Send the secondary i2s rx stop command to afe service. */
   afePortStopCmd.port_id = rx_port_id;
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE Port Rx Stop Command");

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
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe Port Rx Stop Command Failed, result=%ld",rc);
   }

   qurt_elite_thread_join(afe_tx_client.tid, &afe_tx_client.thread_result);
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe tx client is ending"); 

   afe_test_free_resources(&afe_tx_client);
   afe_test_free_resources(&afe_rx_client);

   // disable clock
   getTestClkAfe(my_afe_test_case, rx_port_id, &clock_attr, &clock_id, &clk_value);
   afeTestAfeClkConfig(clock_id, 0, clock_attr, 0);
   
   getTestClkAfe(my_afe_test_case, tx_port_id, &clock_attr, &clock_id, &clk_value);
   afeTestAfeClkConfig(clock_id, 0, clock_attr, 0);
  

   /* Destroy the client and the apr event.*/
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DeInitializing APR resources and thread");
   rc = apr_event_destroy( my_work_done_event );
   rc = elite_apr_if_deregister( my_testclient_apr_handle);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"APR AFE loopback test exiting");
   
   return 0;
}

static DalDeviceHandle *tlmm_hptr = NULL;

static uint32_t n_gpio[4] = {
	 68, 80, 81, 82
	 };

static uint32_t n_in_out[4] = {
	DAL_GPIO_OUTPUT, DAL_GPIO_OUTPUT,
	DAL_GPIO_OUTPUT, DAL_GPIO_OUTPUT
	};

ADSPResult AFE_test_gpio_init()
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

  for(i = 0; i < sizeof(n_gpio)/sizeof(uint32_t); i++)
  	{
	  //create a GPIO configuration for the required pin
	  //function value 0 indicates that this is pin is a dedicated interrupt pin (not a general purpose pin)
	  pin_config = DAL_GPIO_CFG(n_gpio[i], 1, n_in_out[i], DAL_GPIO_NO_PULL, DAL_GPIO_16MA);

	  //call the TLMM DAL API to configure the GPIO
	  if (DalTlmm_ConfigGpio(tlmm_hptr, pin_config, DAL_TLMM_GPIO_ENABLE) != DAL_SUCCESS)
	  {
	    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GPIO TEST INIT: Failed to configure GPIO Pin(%d)", n_gpio[i]);
	    return ADSP_EFAILED;
	  }
  	}


  MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "GPIO TESTINIT: Configured GPIO Interrupt successfully");

  return ADSP_EOK;
}



/**
* This function lauch the whole test thread.
*
* @param[in] afeCmdQueue A pointer to AFE static service cmdQ
*       struct
* @return the test thread result.
*/

int AFE_test_apr_ext_loopback(void* afeCmdQueue)
{
    int           testThreadId, threadResult;
    ADSPResult     result;
    char          threadName[]="AFEAprExtLBThread";

    // Launch the test thread...
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "-----------AFE External Loopback Test start: Launching AFEAprExtLBThread...-----------\n\n");


   {
     num_sent_rx_client = 0;
     num_received_tx_client = 0;
     num_check_count = 0;

     //AFE_test_gpio_init();

     my_afe_test_case = (mytest_e) AFE_TDM_MAX_PRI_TEST; //AFE_TDM_PRI_32BITS_TEST;

    if (ADSP_FAILED (result = qurt_elite_thread_launch(&testThreadId, threadName, NULL, 4096, \
             170, afe_svc_apr_ext_loopback_test, (void *)afeCmdQueue, QURT_ELITE_HEAP_DEFAULT)))
    {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Failed to create Port Threads");
        return result;
    }
    qurt_elite_thread_join(testThreadId, &threadResult);
    }
    return threadResult;
}

#endif //AFE_TEST



