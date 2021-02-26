#ifdef AFE_TEST

#define LPAIF_CLK_TEST
#undef LPAIF_CLK_TEST

#define AFE_SLIMBUS_TEST                  1
#define AFE_RIVA_BT_SCO_TEST              2
#define AFE_RIVA_FM_RX_SLIMBUS_TEST       3
#define AFE_RIVA_SLIMBUS_FM_TX_TEST       4
#define AFE_RIVA_SLIMBUS_BT_A2DP_TEST     5
#define AFE_I2S_TEST                      6
#define AFE_PCM_TEST                      7
#define AFE_SPDIF_TEST                    8
#define AFE_I2S_RESTART_TEST              11
#define AFE_HDMI_OUTPUT_TEST              12


#include "qurt_elite.h"
#include "AFEDevService.h"
/* APR/API related */
#include "apr_comdef.h"
#include "apr_event.h"
#include "adsp_afe_service_commands.h"
#include "EliteAprIf.h"
#include "AFETestHandlers.h"

// duplicates V2 APR
#define AFE_APR_IBASIC_RSP_RESULT   ( 0x000110E8 )
#define AFE_APR_IBASIC_RSP_ACCEPTED ( 0x000100BE )

#define AFE_VFR_TEST
#undef AFE_VFR_TEST

static const uint32 my_test_client_port = 0x00002000;
static apr_event_t  my_work_done_event;
static uint32 my_afe_token = 0xF000F000;
static uint32 my_testclient_apr_handle = 0;
static uint16_t my_testclient_apr_addr   = 0;

static uint16 my_afe_addr = 0;

static uint32 my_afe_test_case = AFE_I2S_TEST;

static uint32_t spdif_sampling_rate = 48000;
static uint16_t spdif_bitwidth = 16;


/* Duplicates the aprv2_ibasic_rsp_result_t */
typedef struct {
   uint32   opcode;
   uint32   status;
} afe_apr_ibasic_rsp_result_t;

static int16_t afeExit = FALSE;

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
       afe_param_id_slimbus_cfg_t sbCfg;
       afe_param_id_internal_bt_fm_cfg_t btCfg;
       afe_loopback_cfg_v1_t  lbCfg;
       afe_param_id_lpaif_clk_cfg_t busclkCfg;
       afe_param_id_internal_digital_cdc_clk_cfg_t cdcclkCfg;
       afe_param_id_spdif_audio_cfg_t spdifcfg;
       afe_param_id_spdif_clk_cfg_t spdifclkcfg;
       afe_param_id_chstatus_cfg_t chstatCfg;
       afe_param_id_ahb_clock_cfg_t ahbClkCfg;
       afe_param_id_hdmi_multi_chan_audio_cfg_t hdmitOutCfg;
       
   };
} afeSetCmdInfo_t;
typedef struct {
   afe_svc_cmd_set_param_t afesvcsetparam;
   afe_svc_param_data_t afesvcparam;
   afe_param_id_clock_set_t busclkCfg_v2;
} afeSvcCmdInfo_t;
int afe_vfr_test_thread(void *afeCmdQueue);

static afe_cmd_rsp_remote_lpass_core_hw_vote_request pRspPayload;

static int32 testclient_isr_recv_fn ( elite_apr_packet_t *packet, void* dispatch_data )
{
   if ( packet == NULL )
   {
      return APR_EBADPARAM;
   }

   switch ( elite_apr_if_get_opcode( packet) )
   {
      case AFE_APR_IBASIC_RSP_ACCEPTED:
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Test client ISR received APR_IBASIC_RSP_ACCEPTED");
         elite_apr_if_free( my_testclient_apr_handle, packet );
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

            elite_apr_if_get_client_token(packet);

            apr_event_signal( my_work_done_event );
            
            elite_apr_if_free( my_testclient_apr_handle, packet );
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

               
            elite_apr_if_free( my_testclient_apr_handle, packet );
         }
      break;
      case AFE_CMD_RSP_REMOTE_LPASS_CORE_HW_VOTE_REQUEST:
      {
		  void *pPayload;

		  /* Get the response payload */
		  elite_apr_if_get_payload((void**)(&pPayload), packet);
		  pRspPayload.client_handle = ((afe_cmd_rsp_remote_lpass_core_hw_vote_request *)pPayload)->client_handle;

          apr_event_signal( my_work_done_event );
          elite_apr_if_free( my_testclient_apr_handle, packet );
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
      case AFE_SLIMBUS_TEST:
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_SLIMBUS_TEST");
         *pTx_port_id = AFE_PORT_ID_SLIMBUS_MULTI_CHAN_0_TX;
         *pRx_port_id = AFE_PORT_ID_SLIMBUS_MULTI_CHAN_0_RX;
      }
      break;
      case AFE_RIVA_BT_SCO_TEST:
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_RIVA_BT_SCO_TEST");
         *pTx_port_id = AFE_PORT_ID_INTERNAL_BT_SCO_TX;
         *pRx_port_id = AFE_PORT_ID_INTERNAL_BT_SCO_RX;
      }
      break;
      case AFE_RIVA_FM_RX_SLIMBUS_TEST:
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_RIVA_FM_RX_SLIMBUS_TEST");
         *pTx_port_id = AFE_PORT_ID_INTERNAL_FM_TX;
         *pRx_port_id = AFE_PORT_ID_SLIMBUS_MULTI_CHAN_0_RX;
      }
      break;
      case AFE_RIVA_SLIMBUS_FM_TX_TEST:
      {   
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_RIVA_SLIMBUS_FM_TX_TEST");
         *pTx_port_id = AFE_PORT_ID_SLIMBUS_MULTI_CHAN_0_TX;
         *pRx_port_id = AFE_PORT_ID_INTERNAL_FM_RX;
      }
      break;
      case AFE_RIVA_SLIMBUS_BT_A2DP_TEST:
      {   
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_RIVA_SLIMBUS_BT_A2DP_TEST");
         *pTx_port_id = AFE_PORT_ID_SLIMBUS_MULTI_CHAN_0_TX;
         *pRx_port_id = AFE_PORT_ID_INTERNAL_BT_A2DP_RX;
      }
      break;
      case AFE_I2S_TEST:
      {   
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_I2S_TEST");
         *pTx_port_id = AFE_PORT_ID_PRIMARY_MI2S_TX;
         *pRx_port_id = AFE_PORT_ID_PRIMARY_MI2S_RX;
      }
      break;
      case AFE_I2S_RESTART_TEST:
      {   
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_I2S_RESTART_TEST");
         *pTx_port_id = AFE_PORT_ID_PRIMARY_MI2S_TX;
         *pRx_port_id = AFE_PORT_ID_PRIMARY_MI2S_RX;
      }
      break;
      case AFE_PCM_TEST:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_PCM_TEST");
         *pTx_port_id = AFE_PORT_ID_PRIMARY_PCM_TX;
         *pRx_port_id = AFE_PORT_ID_PRIMARY_PCM_RX;
      }
      break;
      case AFE_SPDIF_TEST:
      {     
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_SPDIF_TEST");
         *pTx_port_id = AFE_PORT_ID_PRIMARY_MI2S_TX;
         *pRx_port_id = AFE_PORT_ID_SPDIF_RX;
      }
      break;
      case AFE_HDMI_OUTPUT_TEST:
      {
    	  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE_HDMI_OUTPUT_TEST");
    	  *pTx_port_id = AFE_PORT_ID_PRIMARY_MI2S_TX;
    	  *pRx_port_id = AFE_PORT_ID_HDMI_MULTISTREAM_0_RX;
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

static int32 getTestAfeTxPortConfig(uint32_t testcase, afeSetCmdInfo_t *pAfePortCfgCmd)
{
   int32 rc = ADSP_EOK;

   switch(testcase)
   {
      case AFE_SLIMBUS_TEST:
      {
         pAfePortCfgCmd->sbCfg.sb_cfg_minor_version = AFE_API_VERSION_SLIMBUS_CONFIG;
         pAfePortCfgCmd->sbCfg.bit_width = 16;
         pAfePortCfgCmd->sbCfg.data_format = 0;
         pAfePortCfgCmd->sbCfg.num_channels = 1;
         pAfePortCfgCmd->sbCfg.sample_rate = 48000;
         pAfePortCfgCmd->sbCfg.shared_channel_mapping[0] = 128;
         pAfePortCfgCmd->sbCfg.slimbus_dev_id = AFE_SLIMBUS_DEVICE_1;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_SLIMBUS_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->sbCfg);
      }
      break;
      case AFE_RIVA_BT_SCO_TEST:
      {
         pAfePortCfgCmd->btCfg.bt_fm_cfg_minor_version = AFE_API_VERSION_INTERNAL_BT_FM_CONFIG;
         pAfePortCfgCmd->btCfg.num_channels = 1;
         pAfePortCfgCmd->btCfg.bit_width = 16;
         pAfePortCfgCmd->btCfg.sample_rate = 8000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->btCfg);
      }
      break;
      case AFE_RIVA_FM_RX_SLIMBUS_TEST:
      {         
         pAfePortCfgCmd->btCfg.bt_fm_cfg_minor_version = AFE_API_VERSION_INTERNAL_BT_FM_CONFIG;
         pAfePortCfgCmd->btCfg.num_channels = 2;
         pAfePortCfgCmd->btCfg.bit_width = 16;
         pAfePortCfgCmd->btCfg.sample_rate = 48000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->btCfg);
      }
      break;
      case AFE_RIVA_SLIMBUS_FM_TX_TEST:
      {
         pAfePortCfgCmd->sbCfg.sb_cfg_minor_version = AFE_API_VERSION_SLIMBUS_CONFIG;
         pAfePortCfgCmd->sbCfg.bit_width = 16;
         pAfePortCfgCmd->sbCfg.data_format = 0;
         pAfePortCfgCmd->sbCfg.num_channels = 2;
         pAfePortCfgCmd->sbCfg.sample_rate = 48000;
         pAfePortCfgCmd->sbCfg.shared_channel_mapping[0] = 128;
         pAfePortCfgCmd->sbCfg.slimbus_dev_id = AFE_SLIMBUS_DEVICE_1;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_SLIMBUS_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->sbCfg);
      }
      break;
      case AFE_RIVA_SLIMBUS_BT_A2DP_TEST:
      {
         pAfePortCfgCmd->sbCfg.sb_cfg_minor_version = AFE_API_VERSION_SLIMBUS_CONFIG;
         pAfePortCfgCmd->sbCfg.bit_width = 16;
         pAfePortCfgCmd->sbCfg.data_format = 0;
         pAfePortCfgCmd->sbCfg.num_channels = 2;
         pAfePortCfgCmd->sbCfg.sample_rate = 48000;
         pAfePortCfgCmd->sbCfg.shared_channel_mapping[0] = 128;
         pAfePortCfgCmd->sbCfg.slimbus_dev_id = AFE_SLIMBUS_DEVICE_1;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_SLIMBUS_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->sbCfg);
      }
      break;
      case AFE_I2S_TEST:
      case AFE_HDMI_OUTPUT_TEST:
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
      }
      break;
      case AFE_I2S_RESTART_TEST:
      {
         pAfePortCfgCmd->i2scfg.i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
         pAfePortCfgCmd->i2scfg.ws_src = AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL;
         pAfePortCfgCmd->i2scfg.bit_width = 16;
         pAfePortCfgCmd->i2scfg.channel_mode = AFE_PORT_I2S_SD0;
         pAfePortCfgCmd->i2scfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->i2scfg.mono_stereo = AFE_PORT_I2S_MONO;
         pAfePortCfgCmd->i2scfg.sample_rate = 8000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_I2S_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->i2scfg);
      }
      break;
      case AFE_PCM_TEST:
      {
         pAfePortCfgCmd->pcmCfg.pcm_cfg_minor_version = AFE_API_VERSION_PCM_CONFIG;
         pAfePortCfgCmd->pcmCfg.aux_mode  = AFE_PORT_PCM_AUX_MODE_PCM;
         pAfePortCfgCmd->pcmCfg.sync_src = AFE_PORT_PCM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->pcmCfg.frame_setting  = AFE_PORT_PCM_BITS_PER_FRAME_256;
         pAfePortCfgCmd->pcmCfg.quant_type = AFE_PORT_PCM_LINEAR_NOPADDING;
         pAfePortCfgCmd->pcmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->pcmCfg.sample_rate = 8000;
         pAfePortCfgCmd->pcmCfg.bit_width = 16;
         pAfePortCfgCmd->pcmCfg.num_channels = 4;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[0]= 1; //NOTE: slot number start from 1 at service layer.
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[1]= 2;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[2]= 3;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[3]= 4;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_PCM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->pcmCfg);
      }
      break;
      case AFE_SPDIF_TEST:
      {
         pAfePortCfgCmd->i2scfg.i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
         pAfePortCfgCmd->i2scfg.ws_src = AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL;
         pAfePortCfgCmd->i2scfg.bit_width = spdif_bitwidth;
         pAfePortCfgCmd->i2scfg.channel_mode = AFE_PORT_I2S_SD0;
         pAfePortCfgCmd->i2scfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->i2scfg.mono_stereo = AFE_PORT_I2S_MONO;
         pAfePortCfgCmd->i2scfg.sample_rate = spdif_sampling_rate;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_I2S_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->i2scfg);
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

static int32 getTestAfeRxPortConfig(uint32_t testcase, afeSetCmdInfo_t *pAfePortCfgCmd)
{
   int32 rc = ADSP_EOK;

   switch(my_afe_test_case)
   {
      case AFE_SLIMBUS_TEST:
      {
         pAfePortCfgCmd->sbCfg.sb_cfg_minor_version = AFE_API_VERSION_SLIMBUS_CONFIG;
         pAfePortCfgCmd->sbCfg.bit_width = 16;
         pAfePortCfgCmd->sbCfg.data_format = 0;
         pAfePortCfgCmd->sbCfg.num_channels = 1;
         pAfePortCfgCmd->sbCfg.sample_rate = 48000;
         pAfePortCfgCmd->sbCfg.shared_channel_mapping[0] = 129;
         pAfePortCfgCmd->sbCfg.slimbus_dev_id = AFE_SLIMBUS_DEVICE_1;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_SLIMBUS_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->sbCfg);
      }
      break;
      case AFE_RIVA_BT_SCO_TEST:
      {
         pAfePortCfgCmd->btCfg.bt_fm_cfg_minor_version = AFE_API_VERSION_INTERNAL_BT_FM_CONFIG;
         pAfePortCfgCmd->btCfg.num_channels = 1;
         pAfePortCfgCmd->btCfg.bit_width = 16;
         pAfePortCfgCmd->btCfg.sample_rate = 8000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->btCfg);
      }
      break;
      case AFE_RIVA_FM_RX_SLIMBUS_TEST:
      {
         pAfePortCfgCmd->sbCfg.sb_cfg_minor_version = AFE_API_VERSION_SLIMBUS_CONFIG;
         pAfePortCfgCmd->sbCfg.bit_width = 16;
         pAfePortCfgCmd->sbCfg.data_format = 0;
         pAfePortCfgCmd->sbCfg.num_channels = 1;
         pAfePortCfgCmd->sbCfg.sample_rate = 48000;
         pAfePortCfgCmd->sbCfg.shared_channel_mapping[0] = 128;
         pAfePortCfgCmd->sbCfg.slimbus_dev_id = AFE_SLIMBUS_DEVICE_1;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_SLIMBUS_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->sbCfg);
      }
      break;
      case AFE_RIVA_SLIMBUS_FM_TX_TEST:
      {
         pAfePortCfgCmd->btCfg.bt_fm_cfg_minor_version = AFE_API_VERSION_INTERNAL_BT_FM_CONFIG;
         pAfePortCfgCmd->btCfg.num_channels = 2;
         pAfePortCfgCmd->btCfg.bit_width = 16;
         pAfePortCfgCmd->btCfg.sample_rate = 48000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->btCfg);
      }
      break;
      case AFE_RIVA_SLIMBUS_BT_A2DP_TEST:
      {
         pAfePortCfgCmd->btCfg.bt_fm_cfg_minor_version = AFE_API_VERSION_INTERNAL_BT_FM_CONFIG;
         pAfePortCfgCmd->btCfg.num_channels = 2;
         pAfePortCfgCmd->btCfg.bit_width = 16;
         pAfePortCfgCmd->btCfg.sample_rate = 48000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->btCfg);
      }
      break;
      case AFE_I2S_TEST:
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
      }
      break;
      case AFE_I2S_RESTART_TEST:
      {           
         pAfePortCfgCmd->i2scfg.i2s_cfg_minor_version = AFE_API_VERSION_I2S_CONFIG;
         pAfePortCfgCmd->i2scfg.ws_src = AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL;
         pAfePortCfgCmd->i2scfg.bit_width = 16;
         pAfePortCfgCmd->i2scfg.channel_mode = AFE_PORT_I2S_SD1;
         pAfePortCfgCmd->i2scfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->i2scfg.mono_stereo = AFE_PORT_I2S_MONO;
         pAfePortCfgCmd->i2scfg.sample_rate = 8000;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_I2S_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->i2scfg);
      }
      break;
      case AFE_PCM_TEST:
      {
         pAfePortCfgCmd->pcmCfg.pcm_cfg_minor_version = AFE_API_VERSION_PCM_CONFIG;
         pAfePortCfgCmd->pcmCfg.aux_mode  = AFE_PORT_PCM_AUX_MODE_PCM;
         pAfePortCfgCmd->pcmCfg.sync_src = AFE_PORT_PCM_SYNC_SRC_INTERNAL;
         pAfePortCfgCmd->pcmCfg.frame_setting  = AFE_PORT_PCM_BITS_PER_FRAME_256;
         pAfePortCfgCmd->pcmCfg.quant_type = AFE_PORT_PCM_LINEAR_NOPADDING;
         pAfePortCfgCmd->pcmCfg.ctrl_data_out_enable = AFE_PORT_PCM_CTRL_DATA_OE_DISABLE;
         pAfePortCfgCmd->pcmCfg.sample_rate = 8000;
         pAfePortCfgCmd->pcmCfg.bit_width = 16;
         pAfePortCfgCmd->pcmCfg.num_channels = 4;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[0]= 1;  //NOTE: slot number start from 1 at service layer.
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[1]= 2;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[2]= 3;
         pAfePortCfgCmd->pcmCfg.slot_number_mapping[3]= 4;
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_PCM_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->pcmCfg);
      }
      break;
      case AFE_SPDIF_TEST:
      {
         pAfePortCfgCmd->spdifcfg.spdif_cfg_minor_version = AFE_API_VERSION_SPDIF_CONFIG;
         pAfePortCfgCmd->spdifcfg.data_format = AFE_LINEAR_PCM_DATA;
         pAfePortCfgCmd->spdifcfg.bit_width = spdif_bitwidth;
         pAfePortCfgCmd->spdifcfg.sample_rate = spdif_sampling_rate;
         pAfePortCfgCmd->spdifcfg.num_channels = 2;
         
         pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
         pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_SPDIF_CONFIG;
         pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->spdifcfg);
      }
      break;
      case AFE_HDMI_OUTPUT_TEST:
      {
         //FENG
    	  pAfePortCfgCmd->hdmitOutCfg.hdmi_cfg_minor_version = AFE_API_VERSION_HDMI_CONFIG;
    	  pAfePortCfgCmd->hdmitOutCfg.data_type = AFE_LINEAR_PCM_DATA;
    	  pAfePortCfgCmd->hdmitOutCfg.channel_allocation = 0;
    	  pAfePortCfgCmd->hdmitOutCfg.sample_rate = 48000;
    	  pAfePortCfgCmd->hdmitOutCfg.bit_width = 16;
    	  pAfePortCfgCmd->afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
    	  pAfePortCfgCmd->afeParamHeader.param_id = AFE_PARAM_ID_HDMI_CONFIG;
    	  pAfePortCfgCmd->afeParamHeader.param_size = sizeof(pAfePortCfgCmd->hdmitOutCfg);
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


/*
clk_value    - spdif core clock which is double of bit clock
clk_root      - AFE_PORT_CLK_ROOT_LPAPLL is only supported now.
*/
static int32 afeTestSpdifClkConfig(uint32_t clk_value, uint32_t clk_root)
{
   int32 rc = ADSP_EOK;
   afeSetCmdInfo_t afeSpdifclkCfg;
   memset(&afeSpdifclkCfg, 0, sizeof(afeSpdifclkCfg));
   afeSpdifclkCfg.spdifclkcfg.spdif_clk_cfg_minor_version = AFE_API_VERSION_SPDIF_CLK_CONFIG;
   afeSpdifclkCfg.spdifclkcfg.clk_value = clk_value;
   afeSpdifclkCfg.spdifclkcfg.clk_root = clk_root;
   afeSpdifclkCfg.afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
   afeSpdifclkCfg.afeParamHeader.param_id = AFE_PARAM_ID_SPDIF_CLK_CONFIG;
   afeSpdifclkCfg.afeParamHeader.param_size = sizeof(afeSpdifclkCfg.spdifclkcfg);
   afeSpdifclkCfg.afeSetcmd.payload_size = sizeof(afeSpdifclkCfg.afeParamHeader) + afeSpdifclkCfg.afeParamHeader.param_size;
   afeSpdifclkCfg.afeSetcmd.port_id = AFE_PORT_ID_SPDIF_RX;


   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_SET_PARAM_V2,
                                &afeSpdifclkCfg,
                                sizeof(afeSpdifclkCfg));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Port Spdif Clock Config Failed result=%ld",rc);
   }

   my_afe_token++;

   return 0;
}


/*
samping rate  32K 48K
bitwidth          16, 24.
*/
static int32 afeTestChStatusConfig(uint16_t port_id, uint32_t sampling_rate, uint16_t bitwidth)
{
   int32 rc = ADSP_EOK;
   afeSetCmdInfo_t afeChStatusCfg;
   memset(&afeChStatusCfg, 0, sizeof(afeChStatusCfg));
   afeChStatusCfg.chstatCfg.ch_status_cfg_minor_version = AFE_API_VERSION_CH_STATUS_CONFIG;
   afeChStatusCfg.chstatCfg.ch_status_type = AFE_CH_STATUS_A;

   //data format : linear
   afeChStatusCfg.chstatCfg.ch_stat_mask[0] = 0x2;


   //samping rate
   afeChStatusCfg.chstatCfg.ch_stat_mask[3] = 0xf;
   if(48000 == sampling_rate)
   { 
      afeChStatusCfg.chstatCfg.ch_stat_buf[3] = 0x2;
   }
   else if(32000 == bitwidth)
   {
      afeChStatusCfg.chstatCfg.ch_stat_buf[3] = 0x3;
   }   

   //bit width
   afeChStatusCfg.chstatCfg.ch_stat_mask[4] = 0xf;
   if(24 == bitwidth)
   { 
      afeChStatusCfg.chstatCfg.ch_stat_buf[4] = 0xb;
   }
   else if(16 == bitwidth)
   {
      afeChStatusCfg.chstatCfg.ch_stat_buf[4] = 0x2;
   }

   afeChStatusCfg.afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
   afeChStatusCfg.afeParamHeader.param_id = AFE_PARAM_ID_CH_STATUS_CONFIG;
   afeChStatusCfg.afeParamHeader.param_size = sizeof(afeChStatusCfg.chstatCfg);
   afeChStatusCfg.afeSetcmd.payload_size = sizeof(afeChStatusCfg.afeParamHeader) + afeChStatusCfg.afeParamHeader.param_size;
   afeChStatusCfg.afeSetcmd.port_id = port_id;

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_SET_PARAM_V2,
                                &afeChStatusCfg,
                                sizeof(afeChStatusCfg));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Port Channel Status Config Failed, result=%ld",rc);
   }

   my_afe_token++;

   return 0;
}

/*
clk_value1  - It is I2S bit clk for MI2S port, or PCM bit clk for PCM port.
clk_value2  - It is I2S OSR for MI2S port.
clk_src       0 – External
              1 - Internal
clk_root      0 – use internal default setting
clk_set_mode  0 – both invalid
              1 – value1 is valid
              2 – value2 is valid
              3 – both are valid
*/
static int32 afeTestLpaifClkConfig(uint32_t port_id, uint32_t clk_value1, uint32_t clk_value2, uint32_t clk_src, uint32_t clk_root, uint32_t clk_set_mode)
{
   int32 rc = ADSP_EOK;
   afeSetCmdInfo_t afeBusclkCfg;
   memset(&afeBusclkCfg, 0, sizeof(afeBusclkCfg));
   afeBusclkCfg.busclkCfg.lpaif_cfg_minor_version = AFE_API_VERSION_LPAIF_CLK_CONFIG;
   afeBusclkCfg.busclkCfg.clk_value1 = clk_value1;
   afeBusclkCfg.busclkCfg.clk_value2 = clk_value2;
   afeBusclkCfg.busclkCfg.clk_src = clk_src;
   afeBusclkCfg.busclkCfg.clk_root = clk_root;
   afeBusclkCfg.busclkCfg.clk_set_mode = clk_set_mode;
   afeBusclkCfg.afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
   afeBusclkCfg.afeParamHeader.param_id = AFE_PARAM_ID_LPAIF_CLK_CONFIG;
   afeBusclkCfg.afeParamHeader.param_size = sizeof(afeBusclkCfg.busclkCfg);
   afeBusclkCfg.afeSetcmd.payload_size = sizeof(afeBusclkCfg.afeParamHeader) + afeBusclkCfg.afeParamHeader.param_size;
   afeBusclkCfg.afeSetcmd.port_id = port_id;

  //print dbg msg
  //MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"LPAIF_CLK TEST port_id: 0x%x clk_value1: %d clk_value2: %d",port_id, clk_value1, clk_value2);
  //MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"LPAIF_CLK TEST clk_src: %d clk_root: %d clk_set_mode: %d",clk_src, clk_root, clk_set_mode);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_SET_PARAM_V2,
                                &afeBusclkCfg,
                                sizeof(afeBusclkCfg));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Port Loopback Enable Failed, result=%ld",rc);
   }

   my_afe_token++;

   return 0;
}

/*
clk_id             - It is used to identify which clock to set
clk_freq_in_hz     – Clock frequency
clk_attri          - *_COUPLE_NO:Set the clock as specified.
                     *_COUPLE_DIVIDEND: Set the dividend of the clock.
                     *_COUPLE_DIVSOR: Set the divisor of the clock.
enable             - 0:Disable the clock (It will disable the clock specified in clk_id.)
                     1: Enable the clock (As specified in the command.)              
*/
static int32 afeTestLpaifClkConfig_v2(uint32_t port_id, uint32_t clk_id, uint32_t clk_freq_in_hz, uint16_t clk_attri, uint32_t clk_root, uint32_t enable)
{
   int32 rc = ADSP_EOK;
   afeSvcCmdInfo_t afeBusclkCfg;
   memset(&afeBusclkCfg, 0, sizeof(afeBusclkCfg));
   afeBusclkCfg.busclkCfg_v2.clk_set_minor_version = AFE_API_VERSION_CLOCK_SET;
   afeBusclkCfg.busclkCfg_v2.clk_id = clk_id;
   afeBusclkCfg.busclkCfg_v2.clk_freq_in_hz = clk_freq_in_hz;
   afeBusclkCfg.busclkCfg_v2.clk_attri = clk_attri;
   afeBusclkCfg.busclkCfg_v2.clk_root = clk_root;
   afeBusclkCfg.busclkCfg_v2.enable = enable;
   afeBusclkCfg.afesvcparam.module_id = AFE_MODULE_CLOCK_SET;
   afeBusclkCfg.afesvcparam.param_id = AFE_PARAM_ID_CLOCK_SET;
   afeBusclkCfg.afesvcparam.param_size = sizeof(afeBusclkCfg.busclkCfg_v2);
   afeBusclkCfg.afesvcsetparam.payload_size = sizeof(afeBusclkCfg.afesvcparam) + afeBusclkCfg.afesvcparam.param_size;
   afeBusclkCfg.afesvcsetparam.mem_map_handle = 0;

  //print dbg msg
  //MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"LPAIF_CLK TEST port_id: 0x%x clk_value1: %d clk_value2: %d",port_id, clk_value1, clk_value2);
  //MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"LPAIF_CLK TEST clk_src: %d clk_root: %d clk_set_mode: %d",clk_src, clk_root, clk_set_mode);

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_SVC_CMD_SET_PARAM,
                                &afeBusclkCfg,
                                afeBusclkCfg.afesvcsetparam.payload_size + sizeof(afe_svc_cmd_set_param_t));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Port Loopback Enable Failed, result=%ld",rc);
   }

   my_afe_token++;

   return 0;
}

#ifdef LPAIF_CLK_TEST

/*
clk_value  -  Specifies the digital codec core clock freq in Hz to set.
clk_root          #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_INVALID                  0x0
                  #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_PRII2SOSR                0x1
                  #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_SECI2SOSR                0x2
                  #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_CODEC                    0x5
*/
static int32 afeTestCdcClkConfig(uint32_t port_id, uint32_t clk_value, uint32_t clk_root)
{
   int32 rc = ADSP_EOK;
   afeSetCmdInfo_t afeCdcclkCfg;
   memset(&afeCdcclkCfg, 0, sizeof(afeCdcclkCfg));
   afeCdcclkCfg.cdcclkCfg.cdc_clk_cfg_minor_version = AFE_API_VERSION_INTERNAL_DIGIATL_CDC_CLK_CONFIG;
   afeCdcclkCfg.cdcclkCfg.clk_value = clk_value;
   afeCdcclkCfg.cdcclkCfg.clk_root = clk_root;
   afeCdcclkCfg.afeParamHeader.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
   afeCdcclkCfg.afeParamHeader.param_id = AFE_PARAM_ID_INTERNAL_DIGIATL_CDC_CLK_CONFIG;
   afeCdcclkCfg.afeParamHeader.param_size = sizeof(afeCdcclkCfg.cdcclkCfg);
   afeCdcclkCfg.afeSetcmd.payload_size = sizeof(afeCdcclkCfg.afeParamHeader) + afeCdcclkCfg.afeParamHeader.param_size;
   afeCdcclkCfg.afeSetcmd.port_id = port_id;

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_SET_PARAM_V2,
                                &afeCdcclkCfg,
                                sizeof(afeCdcclkCfg));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Port Loopback Enable Failed, result=%ld",rc);
   }

   my_afe_token++;

   return 0;
}
#endif

static int afe_svc_apr_loopback_test(void* afeCmdQueue)
{

   int32 rc = ADSP_EOK;
   //uint16 my_afe_addr;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"APR AFE loopback test start");

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

   if(AFE_SPDIF_TEST == my_afe_test_case)
   {
      uint32_t i2s_bit_clock = spdif_sampling_rate * 2 * 16;
      uint32_t spdif_intf_clock = spdif_sampling_rate * 2 * 32 * 2;
      
      afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_RX, i2s_bit_clock, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
      afeTestSpdifClkConfig(spdif_intf_clock, AFE_PORT_CLK_ROOT_LPAPLL);
   }

   
   if(AFE_I2S_RESTART_TEST == my_afe_test_case)
   {
      afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_RX, 256000, 2048000, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);
      afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_TX, 256000, 2048000, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);
   }

   /**<PRI MI2S RX- IBIT.*/
   afeTestLpaifClkConfig_v2(AFE_PORT_ID_PRIMARY_MI2S_RX, AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT, 1536000, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 1);
   /**<SEC MI2S RX- IBIT.*/
   afeTestLpaifClkConfig_v2(AFE_PORT_ID_SECONDARY_MI2S_RX, AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT, 1536000, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 1);
   /**<TER MI2S TX- IBIT.*/
   afeTestLpaifClkConfig_v2(AFE_PORT_ID_TERTIARY_MI2S_TX, AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_IBIT, 1536000, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 1);
   /**<QUA MI2S RX- IBIT.*/
   afeTestLpaifClkConfig_v2(AFE_PORT_ID_QUATERNARY_MI2S_RX, AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT, 1536000, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 1);
   /**<QUA MI2S TX- IBIT.*/
   afeTestLpaifClkConfig_v2(AFE_PORT_ID_QUATERNARY_MI2S_TX, AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT, 1536000, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 1);
   /**<QUI MI2S RX- IBIT.*/
   afeTestLpaifClkConfig_v2(AFE_PORT_ID_QUINARY_MI2S_RX, AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_IBIT, 1536000, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 1);
   /**<QUI MI2S TX- IBIT.*/
   afeTestLpaifClkConfig_v2(AFE_PORT_ID_QUINARY_MI2S_TX, AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_IBIT, 1536000, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 1);
   /**<SEN MI2S TX- IBIT.*/
   afeTestLpaifClkConfig_v2(AFE_PORT_ID_SENARY_MI2S_TX, AFE_CLOCK_SET_CLOCK_ID_SEN_MI2S_IBIT, 1536000, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 1);

   /**<PRI PCM - IBIT.*/
   afeTestLpaifClkConfig_v2(AFE_PORT_ID_PRIMARY_PCM_RX, AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT, 2048000, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 1);
   /**<PRI PCM - IBIT.*/
   afeTestLpaifClkConfig_v2(AFE_PORT_ID_PRIMARY_PCM_TX, AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT, 2048000, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 1);
   /**<SEC PCM - IBIT.*/
   afeTestLpaifClkConfig_v2(AFE_PORT_ID_SECONDARY_PCM_RX, AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT, 2048000, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 1);
   /**<SEC PCM - IBIT.*/
   afeTestLpaifClkConfig_v2(AFE_PORT_ID_SECONDARY_PCM_TX, AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT, 2048000, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 1);

#if defined(LPAIF_CLK_TEST)

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Port LPAIF_CLK Enable Start ###############################################");
   //Test 1: TER I2S - Both OSR and BIT Internal
   afeTestLpaifClkConfig(AFE_PORT_ID_TERTIARY_MI2S_TX, 1536000, 12288000, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);
   afeTestLpaifClkConfig(AFE_PORT_ID_TERTIARY_MI2S_RX, 1536000, 12288000, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);

   //        TER MI2S - External BIT clk
   afeTestLpaifClkConfig(AFE_PORT_ID_TERTIARY_MI2S_TX, 1536000, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);

   //Test 2: Primary I2S - OSR only
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_RX, 0, 12288000, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE2_VALID_ONLY);
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_TX, 0, 12288000, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE2_VALID_ONLY);

   //Enable CDC clk for testing
   afeTestCdcClkConfig(AFE_PORT_ID_PRIMARY_MI2S_RX, 12288000, AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_PRII2SOSR);

   //        PRI MI2S - IBIT only
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_RX, 1536000, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   //        PRI MI2S - EBIT only
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_RX, 1536000, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);

   //Test 3: Sec I2S -- BIT only - External
   afeTestLpaifClkConfig(AFE_PORT_ID_SECONDARY_MI2S_TX, 1536000, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   afeTestLpaifClkConfig(AFE_PORT_ID_SECONDARY_MI2S_RX, 1536000, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);

   //       SEC MI2S - IBIT and OSR
   afeTestLpaifClkConfig(AFE_PORT_ID_SECONDARY_MI2S_RX, 1536000, 12288000, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);


   //Test 4: Quad I2S -- IBIT and OSR - Internal
   afeTestLpaifClkConfig(AFE_PORT_ID_QUATERNARY_MI2S_RX, 1536000, 12288000, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);
   afeTestLpaifClkConfig(AFE_PORT_ID_QUATERNARY_MI2S_TX, 1536000, 12288000, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);
   //        quad mi2s - EIBIT
   afeTestLpaifClkConfig(AFE_PORT_ID_QUATERNARY_MI2S_TX, 1536000, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);


   //Test 5: PCM1 -- Internal
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_PCM_RX, 2048000, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_PCM_TX, 2048000, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   //        PCM1 -- External
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_PCM_TX, 2048000, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);

   //Test 6: PCM2 -- External
   afeTestLpaifClkConfig(AFE_PORT_ID_SECONDARY_PCM_RX, 2048000, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   afeTestLpaifClkConfig(AFE_PORT_ID_SECONDARY_PCM_TX, 2048000, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   //        PCM2 -- Internal
   afeTestLpaifClkConfig(AFE_PORT_ID_SECONDARY_PCM_TX, 2048000, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);

   //Test 7: SPK I2S - OSR only
   afeTestLpaifClkConfig(AFE_PORT_ID_I2S_RX, 0, 12288000, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE2_VALID_ONLY);
   //        SPK I2S -- IBIT
   afeTestLpaifClkConfig(AFE_PORT_ID_I2S_RX, 1536000, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   //        SPK I2S -- EBIT
   afeTestLpaifClkConfig(AFE_PORT_ID_I2S_RX, 1536000, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Port LPAIF_CLK Enable End ###############################################");   
#endif //#if defined(LPAIF_CLK_TEST)

   // enable loopback
   afeSetCmdInfo_t afeLbCfg;
   memset(&afeLbCfg, 0, sizeof(afeLbCfg));
   afeLbCfg.lbCfg.dst_port_id = rx_port_id;
   afeLbCfg.lbCfg.enable = 1;
   afeLbCfg.lbCfg.loopback_cfg_minor_version = AFE_API_VERSION_LOOPBACK_CONFIG;
   afeLbCfg.lbCfg.routing_mode = LB_MODE_DEFAULT;
   afeLbCfg.afeParamHeader.module_id = AFE_MODULE_LOOPBACK;
   afeLbCfg.afeParamHeader.param_id = AFE_PARAM_ID_LOOPBACK_CONFIG;
   afeLbCfg.afeParamHeader.param_size = sizeof(afeLbCfg.lbCfg);
   afeLbCfg.afeSetcmd.payload_size = sizeof(afeLbCfg.afeParamHeader) + afeLbCfg.afeParamHeader.param_size;
   afeLbCfg.afeSetcmd.port_id = tx_port_id;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE Port Loopback Enable Command");

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_SET_PARAM_V2,
                                &afeLbCfg,
                                sizeof(afeLbCfg));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Port Loopback Enable Failed, result=%ld",rc);
   }

   my_afe_token++;

   /* Send the AFE Tx Port configure command to afe service. */
   afeSetCmdInfo_t afePortCfgCmd;
   memset(&afePortCfgCmd, 0, sizeof(afePortCfgCmd));

   rc = getTestAfeTxPortConfig(my_afe_test_case, &afePortCfgCmd);
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

   rc = getTestAfeRxPortConfig(my_afe_test_case, &afePortCfgCmd);
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

   if(AFE_SPDIF_TEST == my_afe_test_case)
   {
      rc = afeTestChStatusConfig(AFE_PORT_ID_SPDIF_RX, spdif_sampling_rate, spdif_bitwidth);
   }

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

#ifdef AFE_HLOS_VOTING_TEST
   rc = afeTestHwVoteConfig();
   rc = afeTestHwDevoteConfig();
#endif // AFE_HLOS_VOTING_TEST

#ifdef AFE_VFR_TEST
   int vfrThreadId;
   rc = qurt_elite_thread_launch (&vfrThreadId, (char*)"vfrTest", \
        NULL, 4096, 170, afe_vfr_test_thread, afeCmdQueue, QURT_ELITE_HEAP_DEFAULT);
#endif
#if 1
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "!!!!!!!!!!!!!!!afe_svc_apr_loopback_test Will wait for 1s before ending simulation\n" );
   while(TRUE != afeExit)
   {
      qurt_elite_timer_sleep(1000000);
      afeExit = TRUE;
   }
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "!!!!!!!!!!!!!!!1s wait done, afe_svc_apr_loopback_test ending simulation\n" );
#endif
 
   if(AFE_I2S_RESTART_TEST == my_afe_test_case)
   {
      /* Send the restart command to afe service. */
      afe_port_cmd_devices_restart_t   afePortReStartCmd;
      afePortReStartCmd.rx_port_id =   rx_port_id;
      afePortReStartCmd.tx_port_id =   tx_port_id;
      afePortReStartCmd.sample_rate = 16000;
   
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE ReStart Command");

      rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                               my_testclient_apr_addr,
                               my_test_client_port,
                               my_afe_addr,
                               0,
                               my_afe_token,
                               AFE_PORT_CMD_DEVICES_RESTART,
                               &afePortReStartCmd,
                               sizeof(afePortReStartCmd));

      rc = apr_event_wait( my_work_done_event );
      if( ADSP_FAILED( rc ))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe Port Tx Start Command Failed, result=%ld",rc);
      }


      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "2!!!!!!!!!!!!!!!!afe_svc_apr_loopback_test Will wait for 1s before ending simulation\n" );
      afeExit = FALSE;
      while(TRUE != afeExit)
      {
         qurt_elite_timer_sleep(1000000);
         afeExit = TRUE;
      }
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "2!!!!!!!!!!!!!!!!1s wait done, afe_svc_apr_loopback_test ending simulation\n" );


      /* Send the restart command to afe service. */
      //afe_port_cmd_devices_restart_t   afePortReStartCmd;
      afePortReStartCmd.rx_port_id =   rx_port_id;
      afePortReStartCmd.tx_port_id =   tx_port_id;
      afePortReStartCmd.sample_rate = 8000;
      

      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE Tx Port Start Command");

      rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                  my_testclient_apr_addr,
                                  my_test_client_port,
                                  my_afe_addr,
                                  0,
                                  my_afe_token,
                                  AFE_PORT_CMD_DEVICES_RESTART,
                                  &afePortReStartCmd,
                                  sizeof(afePortReStartCmd));

      rc = apr_event_wait( my_work_done_event );
      if( ADSP_FAILED( rc ))
      {
          MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Afe Port Restart Command Failed, result=%ld",rc);
      }

      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "3!!!!!!!!!!!!!afe_svc_apr_loopback_test Will wait for 1s before ending simulation\n" );
      afeExit = FALSE;
      while(TRUE != afeExit)
      {
         qurt_elite_timer_sleep(1000000);
         afeExit = TRUE;
      }
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "3!!!!!!!!!!!!!! wait done, afe_svc_apr_loopback_test ending simulation\n" );
   }
   
   memset(&afeLbCfg, 0, sizeof(afeLbCfg));
   afeLbCfg.lbCfg.dst_port_id = rx_port_id;
   afeLbCfg.lbCfg.enable = 0;
   afeLbCfg.lbCfg.loopback_cfg_minor_version = AFE_API_VERSION_LOOPBACK_CONFIG;
   afeLbCfg.lbCfg.routing_mode = LB_MODE_DEFAULT;
   afeLbCfg.afeParamHeader.module_id = AFE_MODULE_LOOPBACK;
   afeLbCfg.afeParamHeader.param_id = AFE_PARAM_ID_LOOPBACK_CONFIG;
   afeLbCfg.afeParamHeader.param_size = sizeof(afeLbCfg.lbCfg);
   afeLbCfg.afeSetcmd.payload_size = sizeof(afeLbCfg.afeParamHeader) + afeLbCfg.afeParamHeader.param_size;
   afeLbCfg.afeSetcmd.port_id = tx_port_id;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending AFE Port Loopback Disable Command");

   rc = elite_apr_if_alloc_send_cmd( my_testclient_apr_handle,
                                my_testclient_apr_addr,
                                my_test_client_port,
                                my_afe_addr,
                                0,
                                my_afe_token,
                                AFE_PORT_CMD_SET_PARAM_V2,
                                &afeLbCfg,
                                sizeof(afeLbCfg));

   rc = apr_event_wait( my_work_done_event );
   if( ADSP_FAILED( rc ))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AFE Port Loopback Disable Command Failed, result=%ld",rc);
   }

   my_afe_token++;

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


if(AFE_I2S_RESTART_TEST == my_afe_test_case)
{
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_TX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);
}
/**<PRI MI2S RX- IBIT.*/
  afeTestLpaifClkConfig_v2(AFE_PORT_ID_PRIMARY_MI2S_RX, AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 0);
  /**<SEC MI2S RX- IBIT.*/
  afeTestLpaifClkConfig_v2(AFE_PORT_ID_SECONDARY_MI2S_RX, AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 0);
  /**<TER MI2S TX- IBIT.*/
  afeTestLpaifClkConfig_v2(AFE_PORT_ID_TERTIARY_MI2S_TX, AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_IBIT, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 0);
  /**<QUA MI2S RX- IBIT.*/
  afeTestLpaifClkConfig_v2(AFE_PORT_ID_QUATERNARY_MI2S_RX, AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 0);
  /**<QUA MI2S TX- IBIT.*/
  afeTestLpaifClkConfig_v2(AFE_PORT_ID_QUATERNARY_MI2S_TX, AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 0);
  /**<QUI MI2S RX- IBIT.*/
  afeTestLpaifClkConfig_v2(AFE_PORT_ID_QUINARY_MI2S_RX, AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_IBIT, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 0);
  /**<QUI MI2S TX- IBIT.*/
  afeTestLpaifClkConfig_v2(AFE_PORT_ID_QUINARY_MI2S_TX, AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_IBIT, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 0);
  /**<SEN MI2S TX- IBIT.*/
  afeTestLpaifClkConfig_v2(AFE_PORT_ID_SENARY_MI2S_TX, AFE_CLOCK_SET_CLOCK_ID_SEN_MI2S_IBIT, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 0);

  /**<PRI PCM - IBIT.*/
  afeTestLpaifClkConfig_v2(AFE_PORT_ID_PRIMARY_PCM_RX, AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 0);
  /**<PRI PCM - IBIT.*/
  afeTestLpaifClkConfig_v2(AFE_PORT_ID_PRIMARY_PCM_TX, AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 0);
  /**<SEC PCM - IBIT.*/
  afeTestLpaifClkConfig_v2(AFE_PORT_ID_SECONDARY_PCM_RX, AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 0);
  /**<SEC PCM - IBIT.*/
  afeTestLpaifClkConfig_v2(AFE_PORT_ID_SECONDARY_PCM_TX, AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT, 0, AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, 0);

#if defined(LPAIF_CLK_TEST)
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Port LPAIF_CLK Disable Start ###############################################");
   //Test 1: TER I2S - Both OSR and BIT Internal
   afeTestLpaifClkConfig(AFE_PORT_ID_TERTIARY_MI2S_TX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);
   afeTestLpaifClkConfig(AFE_PORT_ID_TERTIARY_MI2S_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);

   //        TER MI2S - External BIT clk
   afeTestLpaifClkConfig(AFE_PORT_ID_TERTIARY_MI2S_TX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);

   //Disable CDC clk for testing
   afeTestCdcClkConfig(AFE_PORT_ID_PRIMARY_MI2S_RX, 0, AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_PRII2SOSR);

   //Test 2: Primary I2S - OSR only
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE2_VALID_ONLY);
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_TX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE2_VALID_ONLY);

   //        PRI MI2S - IBIT only
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   //        PRI MI2S - EBIT only
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_MI2S_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);

   //Test 3: Sec I2S -- BIT only - External
   afeTestLpaifClkConfig(AFE_PORT_ID_SECONDARY_MI2S_TX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   afeTestLpaifClkConfig(AFE_PORT_ID_SECONDARY_MI2S_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);

   //       SEC MI2S - IBIT and OSR
   afeTestLpaifClkConfig(AFE_PORT_ID_SECONDARY_MI2S_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);


   //Test 4: Quad I2S -- IBIT and OSR - Internal
   afeTestLpaifClkConfig(AFE_PORT_ID_QUATERNARY_MI2S_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);
   afeTestLpaifClkConfig(AFE_PORT_ID_QUATERNARY_MI2S_TX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID);
   //        quad mi2s - EIBIT
   afeTestLpaifClkConfig(AFE_PORT_ID_QUATERNARY_MI2S_TX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);


   //Test 5: PCM1 -- Internal
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_PCM_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_PCM_TX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   //        PCM1 -- External
   afeTestLpaifClkConfig(AFE_PORT_ID_PRIMARY_PCM_TX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);

   //Test 6: PCM2 -- External
   afeTestLpaifClkConfig(AFE_PORT_ID_SECONDARY_PCM_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   afeTestLpaifClkConfig(AFE_PORT_ID_SECONDARY_PCM_TX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   //        PCM2 -- Internal
   afeTestLpaifClkConfig(AFE_PORT_ID_SECONDARY_PCM_TX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);

   //Test 7: SPK I2S - OSR only
   afeTestLpaifClkConfig(AFE_PORT_ID_I2S_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE2_VALID_ONLY);
   //        SPK I2S -- IBIT
   afeTestLpaifClkConfig(AFE_PORT_ID_I2S_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_INTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);
   //        SPK I2S -- EBIT
   afeTestLpaifClkConfig(AFE_PORT_ID_I2S_RX, 0, 0, AFE_PORT_LPACLK_CLK_SRC_EXTERNAL, AFE_PORT_LPACLK_CLK_ROOT_DEFAULT, AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Port LPAIF_CLK Disable End ###############################################");
#endif //#if defined(LPAIF_CLK_TEST)

   /* Destroy the client and the apr event.*/
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DeInitializing APR resources and thread");
   rc = apr_event_destroy( my_work_done_event );
   rc = elite_apr_if_deregister( my_testclient_apr_handle);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"APR AFE loopback test exiting");
   
   return 0;
}
/**
* This function lauch the whole test thread.
*
* @param[in] afeCmdQueue A pointer to AFE static service cmdQ
*       struct
* @return the test thread result.
*/

int AFE_test_apr_loopback(void* afeCmdQueue)
{
    int           testThreadId, threadResult;
    ADSPResult     result;
    char          threadName[]="AFEtestAprLBThread";

    // Launch the test thread...
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "-----------AFE Test start: Launching AprLoopbackTest test...-----------\n\n");

    if (ADSP_FAILED (result = qurt_elite_thread_launch(&testThreadId, threadName, NULL, 4096, \
             170, afe_svc_apr_loopback_test, (void *)afeCmdQueue, QURT_ELITE_HEAP_DEFAULT)))
    {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Failed to create Port Threads");
        return result;
    }
    qurt_elite_thread_join(testThreadId, &threadResult);
    return threadResult;
}
#endif //AFE_TEST


