# ===============================================================================
#
#  Copyright (c) 2016, 2019 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import atexit
from optparse import OptionParser
import sys

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('sectools_glue')

def generate(env):
    import SCons
    env.AddMethod(build, "SectoolBuilderGlue")
    if (SCons.Script.ARGUMENTS.get('SIGNTYPE') == 'CASS'):
        if (SCons.Script.ARGUMENTS.get('SIGNSERVER') == None):
            print("Error: For SIGNTYPE CASS SIGNSERVER must be specified as \"SIGNSERVER = <hostname> \" ")
            exit(1)
        else:
            os.environ['SECTOOLS_SIGNER_HOST'] = "http://" + SCons.Script.ARGUMENTS.get('SIGNSERVER')
            if (SCons.Script.ARGUMENTS.get('SIGNPORT') != None):
                os.environ['SECTOOLS_SIGNER_PORT'] = SCons.Script.ARGUMENTS.get('SIGNPORT')

def build(env,
        glue_target_base_dir,
        glue_source,
        glue_sign_id,
        glue_signer=None,
        glue_qti_sign=False,
        glue_sectools_install_base_dir=None,
        glue_install_file_name=None,
        glue_msmid=None,
        glue_msmid_jtagid_dict=None,
        glue_jtag_id=None,
        glue_config = None,
        glue_config_qti = None,
        glue_soc_hw_version=None,
        glue_app_id=None,
        glue_soc_vers = None,
        glue_max_num_root_certs = None,
        glue_is_step1 = False):

    if glue_is_step1:
        if 'USES_SEC_POLICY_STEP1_QC_SIGN' in env:
            if 'USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN' in env:
                del env['USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN']
            env['USES_SEC_POLICY_DEFAULT_SIGN'] = True
        sectools_signed_mbn_step1 = env.SectoolBuilder(
                      target_base_dir = glue_target_base_dir,
                      source=glue_source,
                      sign_id=glue_sign_id,
                      signer = glue_signer,
                      qti_sign = glue_qti_sign,
                      sectools_install_base_dir = glue_sectools_install_base_dir,
                      install_file_name = glue_install_file_name,
                      #config = glue_config_qti  if glue_qti_sign else glue_config,
                      config = glue_config,
                      soc_hw_version=glue_soc_hw_version,
                      soc_vers=glue_soc_vers,
                      target_image_type_filter = env.SectoolImageTypeSign(),
                      max_num_root_certs = glue_max_num_root_certs)
        return  sectools_signed_mbn_step1
    else:
        if 'USES_SEC_POLICY_STEP2_OEM_SIGN' in env:
            if 'USES_SEC_POLICY_DEFAULT_SIGN' in env:
               del env['USES_SEC_POLICY_DEFAULT_SIGN']
            env['USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN'] = True
        sectools_signed_mbn_step2 = env.SectoolBuilder(
                      target_base_dir = glue_target_base_dir,
                      source=glue_source,
                      sign_id=glue_sign_id,
                      signer = glue_signer,
                      qti_sign = glue_qti_sign,
                      sectools_install_base_dir = glue_sectools_install_base_dir,
                      install_file_name = glue_install_file_name,
                      config= glue_config,
                      soc_hw_version=glue_soc_hw_version,
                      soc_vers=glue_soc_vers,
                      max_num_root_certs = glue_max_num_root_certs)
        return sectools_signed_mbn_step2
#------------------------------------------------------------------------------
# main
#------------------------------------------------------------------------------
