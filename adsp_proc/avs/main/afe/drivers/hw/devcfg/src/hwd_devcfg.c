/*========================================================================
   This file contains HWD device configuration functions

  Copyright (c) 2009-2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Technologies, Inc. (QTI) Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/devcfg/src/hwd_devcfg.c#1 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
//#include "lpm_prop.h"   // enable it when fwk lpm prop ready
#include "DALSys.h"
#include "DALSysTypes.h"
//#include "DDIChipInfo.h"

#include "common.h"
#include "lpasshwio_devcfg.h"
#include "hwd_devcfg.h"
#include "hwd_devcfg_internal.h"
#include "qurt_elite.h"
#include "LPASS_ADDRESS_FILE.h"
#include <stringl.h>


typedef struct
{
   HwdAvtimerPropertyType     avtProp;
   HwdGenericClkPropertyType  genericclk_Prop;
   HwdResamplerPropertyType   rsProp;
   lpasshwio_prop_spdif_struct_t   spdif_prop;
   lpasshwio_prop_hdmi_output_struct_t   hdmi_output_prop;
   lpasshwio_prop_lpaif_struct_t     lpaif_prop;
} HwdDevCfgPropertyType;

static boolean               devCfgInitFlag = FALSE;
static HwdDevCfgPropertyType hwdDevCfgProp;


ADSPResult HwdDevCfg_Init(void)
{
   ADSPResult rc = ADSP_EFAILED;

   if (ADSP_EOK != (rc = HwdDevCfg_ReadAudioIfConfigData(&(hwdDevCfgProp.lpaif_prop))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: HwdDevCfg_ReadAudioIfConfigData failed");
   }
   else if (ADSP_EOK != (rc = HwdDevCfg_ReadGenericclkConfigData(&(hwdDevCfgProp.genericclk_Prop))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: HwdDevCfg_ReadGenericclkConfigData failed");
   }
   else if (ADSP_EOK != (rc = HwdDevCfg_ReadAvtimerConfigData(&(hwdDevCfgProp.avtProp))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: HwdDevCfg_ReadAvtimerConfigData failed");
   }
   else if (ADSP_EOK != (rc = HwdDevCfg_ReadResamplerConfigData(&(hwdDevCfgProp.rsProp))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: HwdDevCfg_ReadResamplerConfigData failed");
   }
   else if (ADSP_EOK != (rc = HwdDevCfg_ReadSpdifConfigData(&(hwdDevCfgProp.spdif_prop))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: HwdDevCfg_ReadSpdifConfigData failed");
   }
   else if (ADSP_EOK != (rc = HwdDevCfg_ReadHdmiOutputConfigData(&(hwdDevCfgProp.hdmi_output_prop))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: HwdDevCfg_ReadHdmiOutputConfigData failed");
   }
   else
   {
      devCfgInitFlag = TRUE;
      rc = ADSP_EOK;
   }

   return rc;
}

#ifdef DEBUG_HWD_DEV_CONFIG_DATA
void HwdDevCfg_Debug(void)
{

   uint32 i, maxDataSizeInDword;
   uint32 *pConfigData;

   if (TRUE == devCfgInitFlag)
   {

      maxDataSizeInDword = sizeof(HwdDevCfgPropertyType) / 4;
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DevCfg: HwdDevCfg Data Dump Start");
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Seq: audIfProp avtProp dmlProp rsProp lpass_prop");

      pConfigData = (uint32 *)&hwdDevCfgProp;

      for (i = 0; i < maxDataSizeInDword; i++)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DevCfgData: 0x%x", (unsigned int)*pConfigData);
         pConfigData++;
      }
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DevCfg: HwdDevCfg Data Dump End");

   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DevCfg: HwdDevCfg Not initialized yet");
   }
}
#endif

ADSPResult HwdDevCfg_ReadModuleConfigData(HwdModuleType moduleType, void *pDev)
{
   ADSPResult rc = ADSP_EOK;

   if (NULL != pDev)
   {
      if (devCfgInitFlag)
      {
         // Read Driver Configuration Data
         switch (moduleType)
         {
            // case AudioIf
            case HWD_MODULE_TYPE_AUDIOIF:
            {
               memscpy(pDev, sizeof(lpasshwio_prop_lpaif_struct_t),
                       (void *)&(hwdDevCfgProp.lpaif_prop), sizeof(lpasshwio_prop_lpaif_struct_t));
               break;
            }
               // case Generic clk
            case HWD_MODULE_TYPE_GENERICCLK:
            {
               memscpy(pDev, sizeof(HwdGenericClkPropertyType),
                       (void *)&(hwdDevCfgProp.genericclk_Prop), sizeof(HwdGenericClkPropertyType));
               break;
            }

               // case AVTimer
            case HWD_MODULE_TYPE_AVTIMER:
            {
               memscpy(pDev, sizeof(HwdAvtimerPropertyType),
                       (void *)&(hwdDevCfgProp.avtProp), sizeof(HwdAvtimerPropertyType));
               break;
            }
               // case Resampler
            case HWD_MODULE_TYPE_RESAMPLER:
            {
               memscpy(pDev, sizeof(HwdResamplerPropertyType),
                       (void *)&(hwdDevCfgProp.rsProp), sizeof(HwdResamplerPropertyType));
               break;
            }
            case HWD_MODULE_TYPE_HDMI_OUTPUT:
            {
               memscpy(pDev, sizeof(lpasshwio_prop_hdmi_output_struct_t),
                       (void *)&(hwdDevCfgProp.hdmi_output_prop), sizeof(lpasshwio_prop_hdmi_output_struct_t));
               break;
            }
            default:
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: Invalid HWD Module Type %d", moduleType);
               rc = ADSP_EFAILED;
            }
         }
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: it is not initialized sccessfully");
         rc = ADSP_EFAILED;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: NULL dev pointer\n");
      rc = ADSP_EFAILED;
   }

   return rc;
}

boolean HwdDevCfg_HWSupport(HwdModuleType moduleType)
{
   boolean result = FALSE;

   if (devCfgInitFlag)
   {
      switch (moduleType)
      {
         // case AudioIf
         case HWD_MODULE_TYPE_AUDIOIF:
         {
            if (0 != hwdDevCfgProp.lpaif_prop.hw_revision)
            {
               result = TRUE;
            }
            break;
         }
            // case Generic clk
         case HWD_MODULE_TYPE_GENERICCLK:
         {
            if (0 != hwdDevCfgProp.genericclk_Prop.hw_revision)
            {
               result = TRUE;
            }
            break;
         }
            // case AVTimer
         case HWD_MODULE_TYPE_AVTIMER:
         {
            if (0 != hwdDevCfgProp.avtProp.hw_revision)
            {
               result = TRUE;
            }
            break;
         }
            // case Resampler
         case HWD_MODULE_TYPE_RESAMPLER:
         {
            if (0 != hwdDevCfgProp.rsProp.hw_revision)
            {
               result = TRUE;
            }
            break;
         }
         case HWD_MODULE_TYPE_SPDIF:
         {
            if (0 != hwdDevCfgProp.spdif_prop.hw_revision)
            {
               result = TRUE;
            }
            break;
         }
         case HWD_MODULE_TYPE_HDMI_OUTPUT:
         {
            if (0 != hwdDevCfgProp.hdmi_output_prop.hw_revision)
            {
               result = TRUE;
            }
            break;
         }
         default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: Invalid HWD Module Type %d", moduleType);
         }
      } //switch: moduleType
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: it is not initialized sccessfully");
   }

   return result;
}

static ADSPResult HwdDevCfg_ReadAudioIfConfigData(lpasshwio_prop_lpaif_struct_t *pDev)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   ADSPResult rc = ADSP_EOK;
   lpasshwio_prop_lpaif_struct_t *pAudIfProp;
   char *devId = "LPAIF";

   // get device handle
   if (ADSP_FAILED(rc = (ADSPResult)DALSYS_GetDALPropertyHandleStr(devId, hProp)))
   {
      // error handle
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "DevCfg: Error establishing AudioIf dev handles returned %d", rc);
   }
   else
   {
      // read property ptr
      if (ADSP_FAILED(rc = (ADSPResult)DALSYS_GetPropertyValue(hProp, "LpaifPropStructPtr", 0, &PropVar)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "DevCfg: Error getting AudioIf Property Struct Ptr returned %d", rc);
      }
      else
      {
         pAudIfProp = (lpasshwio_prop_lpaif_struct_t *)PropVar.Val.pStruct;
         *pDev = *pAudIfProp;
      }
   }

   return rc;
}

static ADSPResult HwdDevCfg_ReadGenericclkConfigData(HwdGenericClkPropertyType *pDev)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   ADSPResult rc = ADSP_EOK;
   HwdGenericClkPropertyType *pgGnericclkProp;


   char *devId = "GENERICCLK";

   // get device handle
   if (ADSP_FAILED(rc = (ADSPResult)DALSYS_GetDALPropertyHandleStr(devId, hProp)))
   {
      // error handle
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "DevCfg: Error establishing avtimer dev handles returned %d", rc);
   }
   else
   {
      // read property ptr
      if (ADSP_FAILED(rc = (ADSPResult)DALSYS_GetPropertyValue(hProp, "GenericclkPropStructPtr", 0, &PropVar)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "DevCfg: Error getting Genericclk Property Struct Ptr returned %d", rc);
      }
      else
      {
         pgGnericclkProp = (HwdGenericClkPropertyType *)PropVar.Val.pStruct;
         *pDev = *pgGnericclkProp;
      }
   }

   return rc;
}

static ADSPResult HwdDevCfg_ReadAvtimerConfigData(HwdAvtimerPropertyType *pDev)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   ADSPResult rc = ADSP_EOK;
   HwdAvtimerPropertyType *pAvtProp;
   char *devId = "AVTIMER";

   // get device handle
   if (ADSP_FAILED(rc = (ADSPResult)DALSYS_GetDALPropertyHandleStr(devId, hProp)))
   {
      // error handle
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "DevCfg: Error establishing avtimer dev handles returned %d", rc);
   }
   else
   {
      // read property ptr
      if (ADSP_FAILED(rc = (ADSPResult)DALSYS_GetPropertyValue(hProp, "AvtimerPropStructPtr", 0, &PropVar)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "DevCfg: Error getting Avtimer Property Struct Ptr returned %d", rc);
      }
      else
      {
         pAvtProp = (HwdAvtimerPropertyType *)PropVar.Val.pStruct;

         pDev->baseRegAddr = pAvtProp->baseRegAddr;
         pDev->regSize = pAvtProp->regSize;

         memscpy((uint32_t *)pDev->avtimer_instance_info, sizeof(pDev->avtimer_instance_info),
                 (uint32_t *)pAvtProp->avtimer_instance_info, sizeof(pAvtProp->avtimer_instance_info));

         pDev->audsync_offset_addr = pAvtProp->audsync_offset_addr;
         pDev->isrHookPinNum = pAvtProp->isrHookPinNum;
         pDev->hw_revision = pAvtProp->hw_revision;
      }
   }

   return rc;
}

static ADSPResult HwdDevCfg_ReadResamplerConfigData(HwdResamplerPropertyType *pDev)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   ADSPResult rc = ADSP_EOK;
   HwdResamplerPropertyType *pRsProp;
   char *devId = "RESAMPLER";

   // get device handle
   if (ADSP_FAILED(rc = (ADSPResult)DALSYS_GetDALPropertyHandleStr(devId, hProp)))
   {
      // error handle
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "DevCfg: Error establishing resampler dev handles returned %d", rc);
   }
   else
   {
      // read property ptr
      if (ADSP_FAILED(rc = (ADSPResult)DALSYS_GetPropertyValue(hProp, "ResamplerPropStructPtr", 0, &PropVar)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "DevCfg: Error getting Resampler Property Struct Ptr returned %d", rc);
      }
      else
      {
         pRsProp = (HwdResamplerPropertyType *)PropVar.Val.pStruct;

         pDev->baseRegAddr = pRsProp->baseRegAddr;
         pDev->regSize = pRsProp->regSize;
         pDev->isrHookPinNum = pRsProp->isrHookPinNum;
         pDev->hw_revision = pRsProp->hw_revision;
      }
   }

   return rc;
}

static ADSPResult HwdDevCfg_ReadSpdifConfigData(lpasshwio_prop_spdif_struct_t *pDev)
{
   DALSYSPropertyVar                prop_var;
   lpasshwio_prop_spdif_struct_t    *p_spdif_prop_dev_cfg = NULL;
   const char                        *dev_id = "SPDIFTX";

   DALSYS_PROPERTY_HANDLE_DECLARE(p_handle);

   /* Get device handle */
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(dev_id, p_handle))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: Error getting SPDIF device handle");
      return ADSP_EFAILED;
   }
   else
   {
      /* Read property ptr */
      if (DAL_SUCCESS != DALSYS_GetPropertyValue(p_handle, "SpdifTxPropStructPtr", 0, &prop_var))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: Error getting SPDIF Property Struct Ptr");
         return ADSP_EFAILED;
      }
      else
      {
         p_spdif_prop_dev_cfg = (lpasshwio_prop_spdif_struct_t *)prop_var.Val.pStruct;

         pDev->hw_revision = p_spdif_prop_dev_cfg->hw_revision;
      }
   }

   return ADSP_EOK;
}

static ADSPResult HwdDevCfg_ReadHdmiOutputConfigData(lpasshwio_prop_hdmi_output_struct_t *pDev)
{
   DALSYSPropertyVar prop_var;
   lpasshwio_prop_hdmi_output_struct_t  *hdmi_output_prop_dev_cfg_ptr = NULL;
   const char *dev_id = "HDMITX";

   DALSYS_PROPERTY_HANDLE_DECLARE(dal_hdl);

   /* Get device handle */
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(dev_id, dal_hdl))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg Error getting the DAL device handle for HDMI output");
      return ADSP_EFAILED;
   }
   else
   {
      /* Read property ptr */
      if (DAL_SUCCESS != DALSYS_GetPropertyValue(dal_hdl, "HdmiOutputPropStructPtr", 0, &prop_var))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg Error getting HDMI output Property Struct Ptr");
         return ADSP_EFAILED;
      }
      else
      {
         hdmi_output_prop_dev_cfg_ptr = (lpasshwio_prop_hdmi_output_struct_t *)prop_var.Val.pStruct;

         pDev->lpass_reg_base_addr = hdmi_output_prop_dev_cfg_ptr->lpass_reg_base_addr;          // lpass hdmi output reg base addr
         pDev->lpass_reg_size = hdmi_output_prop_dev_cfg_ptr->lpass_reg_size;               // lpass hdmi output reg page size
         pDev->lpass_reset_reg_base_addr = hdmi_output_prop_dev_cfg_ptr->lpass_reset_reg_base_addr;    // lpass hdmi output reset reg base addr
         pDev->lpass_reset_reg_size = hdmi_output_prop_dev_cfg_ptr->lpass_reset_reg_size;         // lpass hdmi output reset reg page size
         pDev->hdmi_core_reg_base_addr = hdmi_output_prop_dev_cfg_ptr->hdmi_core_reg_base_addr;      // hdmi core output reg base addr
         pDev->hdmi_core_reg_size = hdmi_output_prop_dev_cfg_ptr->hdmi_core_reg_size;           // hdmi core output reg page size
         pDev->hw_revision = hdmi_output_prop_dev_cfg_ptr->hw_revision;
      }
   }

   return ADSP_EOK;
}

