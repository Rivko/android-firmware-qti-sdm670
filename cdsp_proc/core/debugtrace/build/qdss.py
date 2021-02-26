#=================================================================================
#   File Name: debugtrace.py
#
#   Python Script for debugtrace
#
#   Copyright (c) 2015 QUALCOMM Technologies Incorporated.
#   All rights reserved. Qualcomm Proprietary and Confidential.
#
#---------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/build/qdss.py#1 $
#
#=================================================================================

# qdss.BuildTags(['rpm', 'apps', 'modem', 'adsp', 'wcn', 'sensor2', 'sensor', 'audio', 'venus', 'gss', 'vpu'])
# qdss.BuildTags(['modem', 'adsp', 'wcn'])
# List of build tags for image_tags keys given in image_list
def BuildTags(image_list):
    tag_names = []
    image_tags = {
        'rpm'          : ['RPM_IMAGE', 'RPM', 'rpm'],
        'apps'         : ['APPS_PROC', 'APPS_IMAGE', 'CBSP_APPS_IMAGE'],
        'modem'        : ['CORE_MODEM', 'MODEM_IMAGE', 'CBSP_MODEM_IMAGE', 'QDSP6_SW_IMAGE', 'MCFG_SW'],
        'cdsp'         : ['CORE_CDSP_ROOT'],
        'adsp'         : ['CORE_ADSP_ROOT'],
        'wcn'          : ['WCN_IMAGE', 'CBSP_WCN_IMAGE', 'CORE_WCN'],
        'gss'          : ['CORE_GSS'],
        'sensor'       : ['CORE_SLPI_ROOT'],
        'audio'        : ['CORE_QDSP6_AUDIO_SW','CORE_AVS_ADSP_USER'],
        'sensor2'      : ['CORE_SSC_ADSP_USER', 'CORE_SSC_SLPI_USER'],
        'venus'        : ['VENUS_VSS'],
        'vpu'          : ['VPU-TBD'],
        }
    for ii in image_list:
        image_key = ii
        tag_names.extend(image_tags[image_key])
    return tag_names

