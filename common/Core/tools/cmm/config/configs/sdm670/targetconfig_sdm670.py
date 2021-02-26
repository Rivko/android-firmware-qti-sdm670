#===============================================================================
#
# targetconfig_sdm845.py
#
# GENERAL DESCRIPTION
#    Target configuration file.
#
# Copyright (c) 2015 - 2017 by QUALCOMM Technologies Incorporated.
# All Rights Reserved.
# This file is confidential and propietary.
#
# QUALCOMM Proprietary
#
#-------------------------------------------------------------------------------
#

#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who         what, where, why
# --------   ---         ---------------------------------------------------------
# 05/24/2016 JBILLING    Created
#==============================================================================


import os
import sys

def GenerateTargetConfig():

        #### Instantiate an instance of the base class ####
        #from targetconfigurator import *

        sys.path.append(os.path.join(os.path.dirname(__file__),".."))
        targetconfigurator=__import__('TargetConfigurator')
        TargetConfigInstance=targetconfigurator.TargetConfig()

        #### Now update target specific config variables ####
        TargetConfigInstance.TargetAliasList=['sdm670','670','warlock']
        TargetConfigInstance.TargetName='SDM670'
        TargetConfigInstance.CHIPSET='SDM670'
        TargetConfigInstance.BootProcessor='APPS0'
        TargetConfigInstance.T32StartFile='sdm670.ts2'
        #These are listed in 8998.ts2 file
        TargetConfigInstance.APIPortDict={
        'RPM'     : '25400',
        'APPS0'   : '25370',
        'APPS1'   : '25372',
        'APPS2'   : '25373',
        'APPS3'   : '25374',
        'MPSS'    : '25401',
        'ADSP'    : '25402',
     #   'SLPI'    : '25403',
        'ARM32SIM': '10000',
        'ARM64SIM': '10001',
        'Q6Sim'   : '10002',
        'QDSP6SIM'   : '10002',
        'QDSP6Sim'   : '10002',
        }

        TargetConfigInstance.MasterWindow='APPS0'
        TargetConfigInstance.T32Container    = '\"//Root/DAP/Podbus Device Chain/Power Trace Ethernet/'
        TargetConfigInstance.T32SimContainer = '\"//Root/Simulator/'
        TargetConfigInstance.StartDelay = 15

        TargetConfigInstance.Protocols = ['SWD','JTAG']
        TargetConfigInstance.Modes = ['EUD','DONGLE','ETHERNET']
        TargetConfigInstance.PrimaryMode = ['DONGLE']
        TargetConfigInstance.PrimaryProtocol = ['SWD']

        TargetConfigInstance.CoreList = ['APPS0','APPS1','APPS2','APPS3','APPS4',\
                                        'APPS5','APPS6','APPS7','AOP','MPSS',\
                                        'ADSP','CDSP','VSS',
                                        'ARM32SIM','ARM64SIM','QDSP6SIM',
                                        ]
        #This maps aliases of images to a clean software image name
        TargetConfigInstance.SessionCoreName_AliasMap= {
                        #RPM/AOSS core
                        'AOP' : ['RPM','AOP','RPM-CORTEXM3','AOSS'],

                        #Apps cores

                        'APPS0-{HLOS}' : ['APPS0','LA','LINUX','WP','WINDOWS','APPS0-WP','APPS0-LA'],

                        'APPS1-{HLOS}' : ['APPS1','APPS1-LA','APPS1-WP'],
                        'APPS2-{HLOS}' : ['APPS2','APPS2-LA','APPS2-WP'],
                        'APPS3-{HLOS}' : ['APPS3','APPS3-LA','APPS3-WP'],
                        'APPS4-{HLOS}' : ['APPS4','APPS4-LA','APPS4-WP'],
                        'APPS5-{HLOS}' : ['APPS5','APPS5-LA','APPS5-WP'],
                        'APPS6-{HLOS}' : ['APPS6','APPS6-LA','APPS6-WP'],
                        'APPS7-{HLOS}' : ['APPS7','APPS7-LA','APPS7-WP'],

                        'APPSC0-{HLOS}': ['APPS_CLUSTER0','APPSC0','APPSC0-{HLOS}'],
                        'APPSC1-{HLOS}': ['APPS_CLUSTER1','APPSC1','APPSC1-{HLOS}'],
                        #Q6 cores
                        'MPSS' : ['MPSS','MODEM','MPSS-Q6','MODEM-Q6','MSS-Q6'],
                        'ADSP' : ['ADSP','LPASS','ADSP-Q6','LPASS-Q6','AUDIO'],
                        #'SLPI' : ['SLPI','SSC','SLPI-Q6','SSC-Q6','SENSOR','SENSORS'],
                        'CDSP' : ['CDSP','CSS','CDSP-Q6','CSS-Q6','TURING','COMPUTE'],

                        #Other cores
                        'VSS'  : ['VSS','VIDEO','VENUS','VSS-CORTEXM3'],
                        'VPU'  : ['VPU','TITAN','VSS-CORTEXM3'],
                        'DDR'  : ['DDR','SHRM','DDR-TENSILICA'],
                        #SIM cores
                        'ARM32SIM' : ['ARM32SIM','RPMSIM','IPASIM','SSCM3SIM'],
                        'ARM64SIM' : ['ARM64SIM','APPSSIM','APPSIM','APSSSIM'],
                        'QDSP6SIM' : ['QDSP6SIM','MSSSIM','MODEMSIM','Q6SIM'],

                        }

        TargetConfigInstance.hwsubsystem_to_apiport_map={

                'APPS0'   : '25370',
                'APPS1'   : '25371',
                'APPS2'   : '25372',
                'APPS3'   : '25373',
                'APPS4'   : '25374',
                'APPS5'   : '25375',
                'APPS6'   : '25376',
                'APPS7'   : '25377',
                'APPSC0'  : '25378',
                'APPSC1'  : '25379',

                'AOP'     : '25400',
                'MPSS'    : '25401',
                'ADSP'    : '25402',
                #'SLPI'    : '25403',
                'CDSP'    : '25404',

                 'VSS'  : '25405',
                'ARM32SIM': '10000',
                'ARM64SIM': '10001',
                'QDSP6SIM'   : '10002',

                }
        TargetConfigInstance.hwsubsystem_to_intercomport_map={
                'APPS0'   : '15370',
                'APPS1'   : '15371',
                'APPS2'   : '15372',
                'APPS3'   : '15373',
                'APPS4'   : '15374',
                'APPS5'   : '15375',
                'APPS6'   : '15376',
                'APPS7'   : '15377',
                'APPSC0'  : '15378',
                'APPSC1'  : '15379',

                #'AOP'     : '15400',
                'AOP'     : '1008',
                'MPSS'    : '15401',
                'ADSP'    : '15402',
                #'SLPI'    : '15403',
                'CDSP'    : '15404',

                 'VSS'  : '15405',
                'ARM32SIM': '10000',
                'ARM64SIM': '10001',
                'QDSP6SIM'   : '10002',

        }
        TargetConfigInstance.SessionCoreNameDict={
                        'RPM':'{T32Container}RPM',
                        'APPS0' : '{T32Container}APPS0',
                        'APPS1' : '{T32Container}APPS1',
                        'APPS2' : '{T32Container}APPS2',
                        'APPS3' : '{T32Container}APPS3',
                        'APPS3' : '{T32Container}APPS4',
                        'APPS3' : '{T32Container}APPS5',
                        'APPS3' : '{T32Container}APPS6',
                        'APPS3' : '{T32Container}APPS7',
                        'MPSS' : '{T32Container}MPSS',
                        'ADSP' : '{T32Container}ADSP',
                        #'SLPI' : '{T32Container}SLPI',
                        'CDSP' : '{T32Container}CDSP',
                        'VSS'  : '{T32Container}VSS',


                        'ARM32SIM' : '{T32SimContainer}ARM32SIM',
                        'ARM64SIM' : '{T32SimContainer}ARM64SIM',
                        'QDSP6SIM' : '{T32SimContainer}QDSP6Sim',

                        }
        TargetConfigInstance.hwsubsystem_to_instancenum_map={
                'APPS0'   : 0,
                'APPS1'   : 1,
                'APPS2'   : 2,
                'APPS3'   : 3,
                'APPS4'   : 4,
                'APPS5'   : 5,
                'APPS6'   : 6,
                'APPS7'   : 7,
                'APPSC0'  : 8,
                'APPSC1'  : 9,

                'AOP'     : 10,
                'MPSS'    : 11,
                'ADSP'    : 12,
                #'SLPI'    : 13,
                'CDSP'    : 14,

                 'VSS'    : 15,
                'ARM32SIM': None,
                'ARM64SIM': None,
                'QDSP6SIM': None,

        }
        TargetConfigInstance.alias_to_t32simimage_map={
                        'RPM':'ARM32SIM',
                        'MPSS':'QDSP6Sim',
                        'ADSP':'QDSP6Sim',
                        #'SLPI':'QDSP6Sim',
                        'APPS':'ARM64SIM',
                        'APPS0':'ARM64SIM',
                        'APPS1':'ARM64SIM',
                        'APPS2':'ARM64SIM',
                        'APPS3':'ARM64SIM',
                        'APPS4':'ARM64SIM',
                        'APPS5':'ARM64SIM',
                        'APPS6':'ARM64SIM',
                        'APPS7':'ARM64SIM',
                        'VSS':'ARM32SIM',
                        'ARM32SIM':'ARM32SIM',
                        'QDSP6Sim':'QDSP6Sim',
                        'ARM64SIM':'ARM64SIM',
                        }
        TargetConfigInstance.HWCoreNames = ['RPM-CORTEXM3','MSS-Q6','LPASS-Q6','SSC-Q6','APPS-ARMV8']
        TargetConfigInstance.T32MasterWindow = 'APPS0'
        TargetConfigInstance.T32ConfigFilePath='\\common\\Core\\t32\\msm8998.ts2'
        TargetConfigInstance.SoftwareImagesList=['RPM','MPSS','LA','WP','ADSP','TZ','XBL','VSS']
        TargetConfigInstance.DebugImagesList=['RPM','MPSS','LA','WP','ADSP','TZ','XBL']

        TargetConfigInstance.T32BinaryDict={
                        'APPS0':'t32marm64.exe',
                        'APPS1':'t32marm64.exe',
                        'APPS2':'t32marm64.exe',
                        'APPS3':'t32marm64.exe',
                        'APPS4':'t32marm64.exe',
                        'APPS5':'t32marm64.exe',
                        'APPS6':'t32marm64.exe',
                        'APPS7':'t32marm64.exe',
                        'ADSP':'t32mqdsp6.exe',
                        'MPSS':'t32mqdsp6.exe',
                        #'SLPI':'t32mqdsp6.exe',
                        'CDSP':'t32mqdsp6.exe',

                        'AOP':'t32marm.exe',
                        'VSS':'t32marm.exe',
                        'SP':'t32marm.exe',
                        'SSCM3':'t32marm.exe',

                        'QDSP6SIM':'t32mqdsp6.exe',
                        'ARM32SIM':'t32mqdsp6.exe',
                        'ARM64SIM':'t32mqdsp6.exe',
                        }

        TargetConfigInstance.hwsubsystem_to_t32arg_map={
                        'APPS0':'APPS 0' ,
                        'APPS1':'APPS 1' ,
                        'APPS2':'APPS 2' ,
                        'APPS3':'APPS 3' ,
                        'APPS4':'APPS 4' ,
                        'APPS5':'APPS 5' ,
                        'APPS6':'APPS 6' ,
                        'APPS7':'APPS 7' ,
                        'MPSS' :'MPSS 0' ,
                        'ADSP' :'ADSP 0' ,
                        'CDSP' :'CDSP 0' ,
                        #'SLPI' :'SLPI 0' ,
                        'VSS'  :'VSS 0'  ,
                        'AOP' :'RPM 0'  ,
                        'ARM32SIM':'APPS',
                        'ARM64SIM':'APPS',
                        'QDSP6SIM':'APPS',
        }
        TargetConfigInstance.alias_to_image_map=TargetConfigInstance.SessionCoreName_AliasMap

        TargetConfigInstance.image_to_hwsubsystem_map={

                        #RPM/AOSS core
                        'AOP' : ['RPM','AOP','RPM-CORTEXM3'],

                        #Apps cores

                        'APPS0' : ['APPS0','APPS0-{HLOS}','APPS0-WP','APPS0-LA'],

                        'APPS1' : ['APPS1','APPS1-{HLOS}'],
                        'APPS2' : ['APPS2','APPS2-{HLOS}'],
                        'APPS3' : ['APPS3','APPS3-{HLOS}'],
                        'APPS4' : ['APPS4','APPS4-{HLOS}'],
                        'APPS5' : ['APPS5','APPS5-{HLOS}'],
                        'APPS6' : ['APPS6','APPS6-{HLOS}'],
                        'APPS7' : ['APPS7','APPS7-{HLOS}'],
                        'APPSC0': ['APPS-CLUSTER0','APPSC0-{HLOS}','APPSC0'],
                        'APPSC1': ['APPS-CLUSTER1','APPSC1-{HLOS}','APPSC1'],

                        #Q6 cores
                        'MPSS' : ['MPSS','MSS-Q6'],
                        'ADSP' : ['ADSP','LPASS-Q6'],
                        #'SLPI' : ['SLPI','SSC','SSC-Q6'],
                        'CDSP' : ['CDSP','CSS','CSS-Q6',],

                        #Other cores
                        'VSS'  : ['VSS','VSS-CORTEXM3'],
                        'VPU'  : ['VPU'],
                        'DDR'  : ['DDR','SHRM','DDR-TENSILICA'],
                        #SIM cores
                        'ARM32SIM' : ['ARM32SIM'],
                        'ARM64SIM' : ['ARM64SIM'],
                        'QDSP6SIM' : ['QDSP6SIM'],
        }

        TargetConfigInstance.image_to_debugoption_map={
                        'RPM':'rpm',
                        'MPSS':'mpss',
                        'LA':'appsboot',
                        'WP':'appsboot',
                        'TZ':'tz',
                        'XBL':'xbl',
                        'ADSP':'adsp',
                        'SLPI':'slpi',
                        'MBA':'mba',
        }

        TargetConfigInstance.hwsubsystem_to_t32sessioncfg_map=TargetConfigInstance.SessionCoreNameDict

        TargetConfigInstance.image_to_entrypoint_map={
                        'MPSS':'main',
                        'ADSP':'main',
                        'SLPI':'main',
                        'LA':'0x80000000',
                        'WP':'0x80000000',
                        'RPM':'DALSYS_PropsInit',
                        'XBL':'sbl1_main_ctl',
                        'TZ':'Image$$TZBSP_CODE$$Base'
        }

        return TargetConfigInstance














