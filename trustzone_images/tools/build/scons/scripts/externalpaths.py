#===============================================================================
#
# External paths to CoreBSP
#
# GENERAL DESCRIPTION
#    Contains external paths to CoreBSP which are needed by some CoreBSP units
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.tz/2.0.2/build/scons/scripts/externalpaths.py#1 $
#  $DateTime: 2019/02/07 03:06:10 $
#  $Author: pwbldsvc $
#  $Change: 18261931 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/04/09   cjb     Created
# 05/15/09   wd      Merge from SCMM
#
#===============================================================================
import os
import subprocess
import string
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('defaults')

def generate(env):
   env.Replace(PLATFORM_ROOT = env['PLAT_ROOT'])
   env.Replace(SRCROOT = env['INC_ROOT'])
   env.Replace(ROOT = env['INC_ROOT'])
   
   #print "PLATFORM_ROOT >" + env['PLAT_ROOT'] + "<"
   #print "SRCROOT >" + env['SRCROOT'] + "<"
   #print "ROOT >" + env['ROOT'] + "<"
   
   env.Replace(BREW_EXTERNAL = [
      "${BREWERY_OEM_SRC}",
      "${BREWMP_MEDIA_IMAGEDECODER_INC}",
      "${BREWMP_OS_AEE}",
      "${BREWMP_OS_AEE}/OEM",
      "${BREWMP_OS_INC}",
      "${BREWMP_DEPRECATED_CONTROL_INC}",
      "${BREWMP_DEPRECATED_HEADERS_INC}",
      "${BREWMP_DEPRECATED_SOUNDPLAYER_INC}",
      "${BREWMP_MULTIMEDIA_LEGACY_DIR}/inc",
      "${BREWMP_CONNECTIVITY_NETWORKING_INC}",
      "${BREWMP_UI_DISPLAY_INC}",
      "${BREWMP_MEDIA_MULTIMEDIA_INC}",
      "${BREWMP_OS_DEPRECATED_INC}",
      "${BREWMP_OS_INC_DEPRECATED}",
      "${COREAPP}",
      "${COREAPP_RPC}",
      "${INC_ROOT}/apps/brew/inc",
      "${INC_ROOT}/apps/brew/sdk/inc",
      "${INC_ROOT}/apps/brew/pk/inc",
      "${PLAT_ROOT}/libstd/inc",
      "${BREWMP_NETWORKING_NETWORKING_INC}",
      "${BREWMP_NETWORKING_NETWORKING_INC}/deprecated",
      "${PLAT_ROOT}/brewmp/system/settings/inc",
      "${PLAT_ROOT}/brewmp/hardware/sio/inc",
      "${BUILD_ROOT}/asic/apps/staticextensions/inc",
      "${PLAT_ROOT}/brewmp/multimedia/legacymm/inc",
      "${BREWMP_SYSTEM_SETTINGS_DEPRECATED_INC}",
      "${MULTIMEDIA_ROOT}/common/Utils/inc",
      "${PLAT_ROOT}/brewmp/system/brewcore/inc",
   ])

   env.Replace(CS_ROOT = "${PLAT_ROOT}")
   env.Replace(CS_EXTERNAL = [
      "${CS_INC}",
      "${CS_INC_OEM}",
      "${CS_ROOT}/libstd/inc",
      "${PLAT_ROOT}/cs/inc",
      "${PLAT_ROOT}/cs/inc/pro",
      "${CS_OBSOLETE_FS_INC}",
      "${CS_OBSOLETE_PORT}",
      "${CS_OBSOLETE_QIDL_INC}",
      "${PLAT_ROOT}/cs/src/OEM/OEMFS/msm",
      "${PLAT_ROOT}/cs/imports/settings/inc",
      "${CS_DEPRECATED_BID}",
   ])

   env.Replace(DSM_EXTERNAL = [
      "${DSM}",
      "${DSM_INC}",
      "${DSM_SRC}",
      "${DSMUTIL_INC}",
      "${API_MCS}",
      "${DSMUTIL_SRC}",
      "${AU_MODEM_API}/mcs",
      "${AU_MODEM_API}/utils",
   ])

   env.Replace(MODEM_API = [
      "${HDR_API}",
      "${API_MCS}",
      "${GERAN_API}",
      "${API_1X}",
      "${WCDMA_API}",
      "${MODEM_API_GERAN}",
   ])

   # must end in external to be available to UseExternalApi() legacy function
   env.Replace(MODEM_API_EXTERNAL = [
      "${INC_ROOT}/modem/api/rfa",
      "${INC_ROOT}/modem/api/datacommon",
      "${INC_ROOT}/modem/api/rfa",
      "${MODEM_DATACOMMON_API}",
      "${MMODE_API}",
      "${CGPS_API}",      
      "${DATA_API}",      
      "${NAS_API}",
      "${HDR_API}",
      "${API_MCS}",
      "${GERAN_API}",
      "${API_1X}",
      "${UIM_API}",
      "${WCDMA_API}",
      "${MODEM_API_WMS}",
      "${AU_MODEM_API}/data",
      "${AU_MODEM_API}/datacommon",
      "${AU_MODEM_API}/ecall",
      "${AU_MODEM_API}/geran",
      "${AU_MODEM_API}/mmode",
      "${AU_MODEM_API}/nas",
      "${AU_MODEM_API}/pbm",
      "${AU_MODEM_API}/uim",
      "${AU_MODEM_API}/wcdma",
      "${AU_MODEM_API}/wms",
      "${AU_MODEM_API}/utils",
      "${AU_MODEM_API}/mcs",
      "${AU_MODEM_API}/rfa",       
      "${WPLT}",
      "${AU_MODEM_API}/uim",
      "${AU_MODEM_API}/rapi/mmode/inc",
      "${AU_MODEM_API}/rapi/pbm/inc",
      "${AU_MODEM_API}/rapi/data/inc",
      "${AU_MODEM_API}/rapi/gps/inc",
      "${AU_MODEM_API}/rapi/uim/inc",
      "${AU_MODEM_API}/rapi/wms/inc",
      "${AU_MODEM_API}/rapi/hdr/inc",
      "${AU_MODEM_API}/rapi/rfa/inc", 
      "${MODEM_API_GERAN}",
   ])

   env.Replace(MODEM_SLEEPCTL_EXTERNAL = [
      "${SLEEPCTL_INC}",
      "${API_MCS}",
      "${AU_MODEM_API}/mcs",
   ])

   env.Replace(MODEM_MCS_EXTERNAL = [
      "${API_MCS}",
      "${MCS_API}",      
      "${TXCOMGR_INC}",
      "${AU_MODEM_API}/mcs",
      "${MCS_MSGR_INC}",
      "${MCS_APPMGR_INC}",
   ])

   env.Replace(MODEM_LTE_EXTERNAL = [
      "${LTE_ML1_INC}",
      "${LTE_API_INC}",
      "${LTE_COMMON_INC}",
      "${LTE_LL1_INTF_INC}",
      "${LTE_L2_MAC_INC}",
      "${LTE_L2_COMMON_INC}",
      "${LTE_L2_RLC_INC}",
      "${A2_DRIVERS_INC}",
      "${A2_DRIVERS_SRC}",
      "${FW_LTE_INTF_INC}",
      "${FC_UTIL_INC}",
      "${FC_UTIL_SRC}",
      "${MODEM_API_UTIL}",
      "${OSS_UTILS_INC}",
   ])

   env.Replace(MODEM_ADIE_EXTERNAL = [
      "${ADIE}",
   ])

   env.Replace(MODEM_HDR_EXTERNAL = [
      "${HDR}",
      "${HDR_API}",
      "${HDRBC}/inc",
      "${HDRCOMMON}/inc",
      "${HDRCP}/inc",
      "${HDRDRIVERS}/inc",
      "${HDRHIT}/inc",
      "${HDRMAC}/inc",
      "${HDRSRCH}/inc",
      "${HDRTAP}/inc",
      "${TPROF}",
      "${MDSPFW_HDR}",
      "${HDRDRIVERS}/inc}",
   ])

   env.Replace(MODEM_MDSP_EXTERNAL = [
      "${MDSPCDMA}/inc",
      "${MDSPCDMA}",
      "${MDSP}",
      "${MDSPSVC_INC}",
      "${MDSPSVC_SRC}",
      "${MDSPSVC}",
      "${MDSPBUILD}",
      "${MDSPBUILD}/src",
      "${GERAN_API}",
      "${MDSPFW_WCDMA}",
      "${MDSPFW_GERAN}",
      "${MDSPFW_TARGET}",
      "${MDSPFW_IRAT}",
      "${MODEM_API_GERAN}",
      "${MDSPFW_TARGET}",
   ])

   env.Replace(MODEM_DATA_EXTERNAL = [
      "${MODEM_DATACOMMON_API}",
      '${BUILD_ROOT}/modem/datacommon/api',
      "${AU_MODEM_API}/datacommon",
      "${AU_MODEM_API}/data",
      "${DATA}",
      "${DATA}/inc",
      "${DATA}/1xrlp/inc",
      "${DATA_REMOTE_APIS}",
      "${DATA_REMOTE_APIS}/inc",
      "${DATA_1X_1XRLP_INC}",
      "${DATA_1X_707_INC}",
      "${DATA_1X_BCDB_INC}",
      "${DATA_1X_BCMCS_INC}",
      "${DATA_1X_DOQOS_INC}",
      "${DATA_1X_HDRDATA_INC}",
      "${DATA_1X_MIP_INC}",
      "${DS_MULTI_PROC}",
      "${DS_MULTI_PROC}/inc",
      "${DSMGR}",
      "${DSMGR_INC}",
      "${COMMONATCOP_INC}",
      "${CDPS}",
      "${DSMP_UMTS_RPC_INC}",
      "${DSMP_UMTS_INC}",
      "${UMTSATCOP_INC}",
      "${PDCP_INC}",
      "${UMTS_PS_INC}",
      "${UMTS_CS_INC}",
      "${MODEM_DATACOMMON_DSS_INC}",
      "${MODEM_DATACOMMON_HC_INC}",
      "${MODEM_DATACOMMON_IFACECTLS_INC}",
      "${MODEM_DATACOMMON_INET_INC}",
      "${MODEM_DATACOMMON_LINKLAYER_INC}",
      "${MODEM_DATACOMMON_NETIFACE_INC}",
      "${MODEM_DATACOMMON_QMI_INC}",
      "${MODEM_DATACOMMON_RMIFACECTLS_INC}",
      "${MODEM_DATACOMMON_STACORE_INC}",
      "${ONCRPC}",
      "${MODEM_DATACOMMON_STADATA_INC}",
      "${MODEM_DATACOMMON_STADSM_INC}",
      "${MODEM_DATACOMMON_UTILS_INC}",
      "${MODEM_DATACOMMON_API_LINKLAYER}",
      "${MODEM_API_DATACOMMON_LINKLAYER}",
      "${MODEM_DATACOMMON_DSSOCK_INC}",
      "${AU_MODEM_API}/data",
      "${AU_MODEM_API}/datacommon",
      "${DATA_API}",
   ])

   env.Replace(MODEM_GERAN_EXTERNAL = [
      "${GERAN_API}",
      "${MODEM_API_GERAN}",
   ])

   env.Replace(SRCH_EXTERNAL = [
      "${SRCH}",
      "${SRCH_INC}",
      "${SRCH_COMMON}",
      "${SRCH_IDLE}",
      "${SRCH_INIT}",
      "${SRCH_TRAFFIC}",
   ])

   env.Replace(MODEM_1X_EXTERNAL = [
      "${API_1X}",
      "${SRCH_EXTERNAL}",
      "${MUX}",
      "${MUX_INC}",
      "${CP}/inc",
      "${V_1X_DRIVERS_INC}",
      "${V_1X_DIAG_INC}",
      "${V_1X_CUST_INC}",
      "${V_1XAPI_INC}",
      "${ENC}",
      "${DEC}",
   ])

   env.Replace(MODEM_PMIC_EXTERNAL = [
      "${CHG}",
      "${CHG_RPC}",
      "${PMIC_CMI_INC}",
      "${PMIC_APP_INC}",
      "${PMIC_GEN_INC}",
      "${PMIC_DRIVER_INC}",
      "${PMIC_DRIVER_SRC}",
      "${PMIC_MODEM_APP_INC}",
      "${PMIC_MODEM_DRIVER_INC}",
      "${PMIC_DRIVER_RPC_INC}",
      "${PMIC_GEN_RPC_INC}",
      "${PMIC_CHG_APP_INC}",
      "${PMAPP_GEN_SW}",
      "${PMAPP_GEN_SW_RPC}",
      "${PMAPP_OTG_SW}",
      "${PMAPP_OTG_SW_RPC}",
      "${PMLIB_INTERFACE}",
      "${PMLIB_SW}",
      "${PMLIB_SW_RPC}",
      "${VBATT}",
      "${VBATT_RPC}",
      "${PMIC_VBATT_APP_INC}",
   ])

   env.Replace(MODEM_RF_EXTERNAL = [
      "${RF_PATH}",
      "${RFA_PATHS}",
      "${RFA_1X_SRC}",  
      "${RFA_NV_INC}",
      "${RFA_MC_INC}",
      "${RF_MC}",
      "${INC_ROOT}/modem/api/rfa",
   ])

   env.Replace(MODEM_FTM_EXTERNAL = [
      "${FTM_PATH}",
      "${FTM}/core/inc",
      "${FTM}/gps/agps",
      "${FTM}/pmic",
      "${FTM}/rf/common",
      "${FTM}/rf/1x",
      "${FTM}/rf/egprs",
      "${FTM}/rf/gsm",
      "${FTM}/rf/wcdma",
      "${FTM}/system/common",
      "${FTM}/system/1x",
      "${FTM}/system/egprs",
      "${FTM}/system/gsm",
      "${FTM}/system/wcdma",
      "${FTM_HWTC}",
   ])


   env.Replace(MODEM_UIM_EXTERNAL = [
      "${UIM}",
      "${GSTK}",
      "${GSTK_INC}",
      "${GSTK_RPC}",
      "${GSTK_SRC}",
      "${GSTK_SRC}/rpc",
      "${UIMDRV_INC}",
      "${UIMDRV_RPC}",
      "${UIMDRV_SRC}",
      "${UIMDRV_SRC}/rpc",
      "${MMGSDI}",
      "${MMGSDI_INC}",
      "${MMGSDI_SRC}/rpc",
      "${MMGSDI_RPC}",
      "${ICARD_INC}",
      "${ISTK_INC}",
   ])

   env.Replace(MODEM_NAS_EXTERNAL = [
      "${CB}",
      "${CB_INC}",
      "${CNM}",
      "${MM}",
      "${MM_INC}",
      "${MN}",
      "${MBMSM}",
      "${RABM}",
      "${REG}",
      "${SM}",
      "${TC}",
      "${NAS_SERVICES}",
      "${NAS_SERVICES_INC}",
   ])

   env.Replace(MODEM_MMODE_EXTERNAL = [
      "${CM}",
      "${CM_INC}",
      "${SD}",
      "${SD_INC}",
      "${MMOC}",
      "${MMOC_INC}",
      "${AU_MODEM_API}/mmode",
   ])

   env.Replace(MODEM_SERVICES_EXTERNAL = [
      "${CM}",
      "${CM_INC}",
      "${ECALL_PATH}",
      "${SD}",
      "${MMOC}",
      "${MMOC_INC}",
      "${FC}",
      "${PBM}",
      "${PBM_RPC}",
      "${FTM}/system/1x",
      "${WMS}/inc",
      "${WMS}/inc_deprecated",
      "${INC_ROOT}/modem/unused",
      "${SAM}",
      "${TCXOMGR}",
      "${TCXOMGR_INC}",      
      "${AU_MODEM_API}/mmode",
      "${HDR_API}",
      "${AU_MODEM_API}/uim",
      "${AU_MODEM_API}/nas",
      "${AU_MODEM_API}/pbm",
      "${AU_MODEM_API}/wms",
      "${AU_MODEM_API}/wcdma",
    ])

   env.Replace(MODEM_GPS_EXTERNAL = [
      "${CGPS_INC}",
      "${CGPS_RPC_INC}",
   ])

   env.Replace(MODEM_WCDMA_EXTERNAL = [
      "${FC}",      
      "${MAC}",
      "${MAC_INC}",
      "${OSS}",
      "${RLC}",
      "${RRC_RVCT}",
      "${SRM}",
      "${WCDMAAPI_INC}",
      "${WCDMA_DIAG}",
      "${WCDMA_DIAG_INC}",
      "${WCDMA_L1}",
      "${WCDMA_L2}",
      "${WCDMA_L2_INC}",
      "${WPLT}",
      "${P2_COMMON_DRV}",
      "${OSS_INC}",								# ossship.h
      "${RLC_INC}",								# rlcrrcif.h
      "${RRC}",									# rrc_rr_types.h
      "${RRC_INC}",								# rrc_rr_types.h
      "${RRC_INC_RVCT}",						        # rrcasn1_rvct.h
      "${WCDMA_L1_INC}",						        # seq.h
      "${SRCROOT}/modem/utils/oss/oss_asn1_rvds21/include",                     # missing OSS path
      "${SRCROOT}/modem/wcdma/oss/oss_asn1_rvds21/include",                     # missing OSS path
      "${MODEM_MEM_INC}",
      "${RRC_INC_Q6}",
   ])
   
   env.Replace(MULTIMEDIA_AUDIO_ROOT = "${INC_ROOT}/multimedia/audio")
   env.Replace(MULTIMEDIA_AUDIO_EXTERNAL = [
      "${AUDBT}",
      "${AUDDECDRV}",
      "${AUDENC}",
      "${AUDIO_API}",
      "${MMAUDIO_API}",
      "${MULTIMEDIA_API}",
      "${APR_API}",
      "${MMUAUDIO_API}",
      "${INC_ROOT}/multimedia/api/audio2/audioinit",
      "${AUDIORPC}",
      "${AUDSVC}",
      "${VOCSVC}",
      "${AVS}",
      "${MVS}",
      "${VS}",
      "${XMF}",
      "${VOCSVC}",
      "${MULTIMEDIA_AUDIO_ROOT}/vs/src",
      "${MULTIMEDIA_AUDIO_ROOT}/audiosbc/src",
      "${MULTIMEDIA_AUDIO_ROOT}/amrsup/src",
      "${MULTIMEDIA_AUDIO_ROOT}/mvs/src",
      "${MULTIMEDIA_AUDIO_ROOT}/drivers/coreaudiodrivers/defs/inc",
      "${MULTIMEDIA_AUDIO_ROOT}/drivers/CoreAudioDrivers/auddrv/media/inc",
      "${MULTIMEDIA_AUDIO_ROOT}/Cadal/src",
      "${MULTIMEDIA_AUDIO_ROOT}/drivers/CoreAudioDrivers/auddrv/FW/FW_controller/inc",
      "${INC_ROOT}/multimedia/audio/cmx/src",
      "${INC_ROOT}/multimedia/audio/audsa/inc",
      "${INC_ROOT}/services/adsp",
   ])

   env.Replace(MULTIMEDIA_CAMERA_ROOT = "${INC_ROOT}/multimedia/camera")
   env.Replace(MULTIMEDIA_CAMERA_EXTERNAL = [
      "${MULTIMEDIA_CAMERA_ROOT}/cameradrivetask/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/SensorDriver/SensorDriver/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/Common/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/cameraosservices/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/CameraPlatform/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/SensorDriver/SensorDriver/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/API/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/CameraDevice/CameraDevice/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/TuningData/TuningDataUtil/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/TuningData/TuningDataConfig/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/TuningData/TuningData/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/TuningData/BestShot/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/TuningData/ChromatixDataMod/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/TuningData/ChromatixDataModUtil/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/chromatix/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/VFEDriver/VFEDriver7x30/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/FlashService/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/Flashdriver/Flashdriver/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/FlashDriver/WhiteLEDFlashDriver/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/SensorDriver/Foxconn_5M_Bayer/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/SensorDriver/Micron_MT9D112_2M_YUV/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/SensorDriver/Micron_MT9P012_5M_Bayer/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/SensorDriver/Micron_MT9T013_3M_Bayer/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/SensorDriver/Micron_MT9V112_VGA_YUV/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/SensorDriver/ST_VX6953_5M_Bayer/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/CommonService/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/SensorService/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/FlashELFS/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/AEC/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/AF/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/AWB/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/HJR/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/Luma/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/VFEService/VFEService/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/ELFSComponent/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/RuntimeTuning/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/QMobiCaT/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/CameraLIBC/CameraLIBCWin/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/VFEService/VFEService7x30/src",
      "${MULTIMEDIA_CAMERA_ROOT}/VFEService/VFEService7x30/src/Configurations/ConfigurationManager",
      "${MULTIMEDIA_CAMERA_ROOT}/Settings/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/CameraEventLog/CameraEventLog/inc",
      "${MULTIMEDIA_CAMERA_ROOT}/CameraEventLog/CameraEventLogWin/inc",
      "${CAMERA_FLASHINC}",
   ])

   env.Replace(MULTIMEDIA_DISPLAY_QDI_ROOT = "${INC_ROOT}/multimedia/Display")
   env.Replace(MULTIMEDIA_DISPLAY_HAL_ROOT = "${INC_ROOT}/hal/display")
   env.Replace(MULTIMEDIA_DISPLAY_QDI_EXTERNAL = [
   "${MULTIMEDIA_DISPLAY_QDI_ROOT}/qdi/qdi_api/inc",
   "${MULTIMEDIA_DISPLAY_QDI_ROOT}/qdi/osal/common/inc",
   "${MULTIMEDIA_DISPLAY_QDI_ROOT}/qdi/osal/Rex/inc",
   "${MULTIMEDIA_DISPLAY_HAL_ROOT}/mdp/inc",
   "${MULTIMEDIA_DISPLAY_QDI_ROOT}/qdi/ds/inc",
   "${MULTIMEDIA_DISPLAY_QDI_ROOT}/qdi/di/inc"
   ])

   env.Replace(MULTIMEDIA_CORE_ROOT = "${INC_ROOT}/multimedia/core")
   env.Replace(MULTIMEDIA_CORE_EXTERNAL = [
      "${MULTIMEDIA_CORE_ROOT}/qdspservices",
   ])

   env.Replace(MULTIMEDIA_DISPLAY_ROOT = "${INC_ROOT}/multimedia/display")
   env.Replace(MULTIMEDIA_DISPLAY_EXTERNAL = [
      "${MULTIMEDIA_ROOT}/common/Utils/inc",
      "${LCD}",
      "${MDDI}",
      "${DISP}",
      "${DISP_INC}",
      "${DISP_SRC}",
      "${DISPBL}",
      "${MDP_COMMON}",
      "${MULTIMEDIA_ROOT}/display/mddi/src",
      "${MULTIMEDIA_ROOT}/display/mddi/inc",
      "${MULTIMEDIA_DISPLAY_ROOT}/disp_task/inc",
      "${MULTIMEDIA_DISPLAY_ROOT}/dispbl/inc",
      "${MULTIMEDIA_DISPLAY_ROOT}/lcd/inc",
      "${MULTIMEDIA_DISPLAY_ROOT}/lcd/src",
      "${MULTIMEDIA_DISPLAY_ROOT}/mdp/mdp_api/inc",
      "${MULTIMEDIA_DISPLAY_ROOT}/mdp/mdp_api/src",
      "${MULTIMEDIA_DISPLAY_ROOT}/mdp/mdp/inc",
      "${MULTIMEDIA_DISPLAY_ROOT}/ioverlay/inc",
      "${MULTIMEDIA_DISPLAY_QDI_ROOT}/qdi/qdi_api/inc",
      "${MULTIMEDIA_DISPLAY_QDI_ROOT}/qdi/ds/inc",
      "${MULTIMEDIA_DISPLAY_QDI_ROOT}/qdi/osal/common/inc",
      "${MULTIMEDIA_DISPLAY_QDI_ROOT}/qdi/osal/Rex/inc",
      "${INC_ROOT}/multimedia/api/display",
   ])

   env.Replace(MULTIMEDIA_ROOT = "${INC_ROOT}/multimedia")
   env.Replace(MULTIMEDIA_EXTERNAL = [
      "${ADSPINFO_API}",
      "${MULTIMEDIA_ROOT}/api",
      "${MULTIMEDIA_ROOT}/api/display",
      "${MULTIMEDIA_ROOT}/api/dspdiag",
      "${MULTIMEDIA_ROOT}/api/adsp_diag",
      "${MULTIMEDIA_ROOT}/dsp/services/adsp",
      "${MULTIMEDIA_ROOT}/ims/fwapi/src",
      "${MULTIMEDIA_ROOT}/ims/videoshare/inc",
      "${MULTIMEDIA_ROOT}/ims/qipcall/src",
      "${MULTIMEDIA_ROOT}/ims/qipcall/inc",
      "${MULTIMEDIA_ROOT}/ims/hom/src",
      "${MULTIMEDIA_ROOT}/ims/imsprocess/inc",
      "${MULTIMEDIA_ROOT}/ims/dpl/inc",
      "${MULTIMEDIA_ROOT}/video/decoder/test/inc",
      "${MULTIMEDIA_ROOT}/mmservices/mpeg4fileformat/parser/inc",
      "${MULTIMEDIA_ROOT}/qtv/utils/task/inc",
      "${MULTIMEDIA_ROOT}/qtv/player/playerif/inc",
      "${MULTIMEDIA_ROOT}/videophone/csvt/qvpapp/inc",
      "${MULTIMEDIA_ROOT}/videophone/vt_codec/codec_api/inc",
      "${MULTIMEDIA_ROOT}/common/OSAbstraction/inc",
      "${MULTIMEDIA_ROOT}/common/Utils/inc",
      "${ADSPRTOSSVC}",
      "${ADSPRTOSSVCAPI}",
      "${ADSPRTOSSVCRPC}",
      "${ADSP}",
      "${ADSPINFO}",
      "${QDSPAPI}",
      "${QDSP}",
   ])

   env.Replace(MULTIMEDIA_GRAPHICS_ROOT = "${INC_ROOT}/multimedia/graphics")
   env.Replace(MULTIMEDIA_GRAPHICS_EXTERNAL = [
      "${GRAPH}",
      "${SVG}",
      "${GFX}",
      "${GRAPHICS_INC}",
      "${MULTIMEDIA_GRAPHICS_ROOT}/graph",
      "${MULTIMEDIA_GRAPHICS_ROOT}/svg/services/interface/inc",
      "${INC_ROOT}/multimedia/stillimage/jpeg/jpegdecoder/inc",
      "${INC_ROOT}/multimedia/stillimage/jpeg/exif/inc",
   ])


   env.Replace(WCONNECT_ROOT = "${INC_ROOT}/wconnect")
   env.Replace(WCONNECT_EXTERNAL = [
      "${WCONNECT_ROOT}/bthost/core/inc",
      "${WCONNECT_ROOT}/bthost/core/src",
      "${WCONNECT_ROOT}/bthost/core/hci/protected",
      "${WCONNECT_ROOT}/bthost/pf/inc",
      "${WCONNECT_ROOT}/bthost/pf/src",
      "${WCONNECT_ROOT}/bthost/soccfg/src",
      "${WCONNECT_ROOT}/bthost/bm3/sdk/include",
      "${WCONNECT_ROOT}/bthost/bm3/sdk/include/profiles",
      "${WCONNECT_ROOT}/fmhost/api",
      "${WCONNECT_PATHS}",
      "${BT_EXTERNAL}",
      "${WLAN_EXTERNAL}",
   ])

   env.Replace(BT_EXTERNAL = [
      "${BTHOST_INC_PATHS}",
      "${BTHOSTCORE_INC}",
      "${BTHOSTCORE_SRC}",
      "${BTHOSTCORERPC_INC}",
      "${BTHOSTCORERPC_SRC}",
      "${BTHOSTFTM_INC}",
      "${BTFTM_INC}",
      "${BTHOSTFTMRPC_INC}",
      "${BTHOSTSOCCFG_INC}",
      "${BTHOSTSOCCFG_SRC}",
      "${BTHOSTPF_INC}",
      "${BTHOSTPF_SRC}",
      "${BTHOSTPFRPC_INC}",
      "${BTHOSTCOREHCI_PROTECTED}",
   ])

   env.Replace(WLAN_EXTERNAL = [
      "${WLAN_ADP}",
      "${WLAN_CP}",
      "${WLAN_SEC}",
      "${WLAN_TRP}",
      "${WLAN_WPS}",
      "${WLANHS_RAPI_INC}",
      "${WLANHS_API}",
      "${WLAN_API}",
      "${WLAN_ADP_INC}",
      "${WLAN_CP_INC}",
      "${WLAN_SEC_INC}",
      "${WLAN_TRP_INC}",
      "${WLAN_WPS_INC}",
   ])

   env.Replace(WIRELESSCONNECTIVITY_EXTERNAL = [
      "${BT_EXTERNAL}",
      "${WLAN_EXTERNAL}",
   ])

   env.Replace(USB_EXTERNAL = [
      "${USB}",
      "${USB_CARKIT}",
   ])
   
   env.Replace(SME_EXTERNAL = [
      "${SME_CRYPTO_INC}",
      "${SME_CRYPTO_SRC}",
      "${SME_SMETEST_TEST_CRYPTO_SRC}",
      "${SME_SSL_INC}",
      "${SME_SSL_SRC}",
   ])
 
   env.Replace(CTA_EXTERNAL = [
       "${CTAROOT}",
       "${CTAROOT}/cta_core",
       "${CTAROOT}/fta_common",
   ])
    
   return None

