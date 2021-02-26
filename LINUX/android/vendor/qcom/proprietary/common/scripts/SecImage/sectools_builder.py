# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
sectools_builder.py

GENERAL DESCRIPTION
   Contains build process utilities for calling sectools
'''

import os
from optparse import OptionParser
import sys

# Name & version of the tool
SECTOOLS_BUILDER_TOOL_NAME = 'Sectools_Builder'
SECTOOLS_BUILDER_TOOL_VERSION = '2.4'

__version__ = SECTOOLS_BUILDER_TOOL_NAME + ' ' + SECTOOLS_BUILDER_TOOL_VERSION

SECTOOLS_DIR = os.path.dirname(os.path.realpath(__file__))

sys.path.append(SECTOOLS_DIR)

from sectools.features.isc.builder import scons_secimage_builder
from sectools.features.isc.builder import secimage_builder

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return env.Detect('sectools_builder')

def generate(env):
    import SCons
    sectools_mbn_generate(env)

#===============================================================================
# PBN build rules
#===============================================================================
def sectools_mbn_generate(env):
    #-------------------------------------------------------------------------------
    # def builder for PBN
    #
    #import pdb;pdb.set_trace()
    env.AddMethod(secimage_builder.build, "SectoolBuilder")
    env.AddMethod(secimage_builder.install, "SectoolInstall")
    env.AddMethod(secimage_builder.get_msm_jtag_dict_from_file, "SectoolGetMSMToJTAGDict")
    env.AddMethod(secimage_builder.isEnabled, "SectoolIsEnabled")
    env.AddMethod(secimage_builder.getUnsignedInstallPath, "SectoolGetUnsignedInstallPath")

    sectools_signer_act = env.GetBuilderAction(scons_secimage_builder.action)
    sectools_signer_bld = env.Builder(
                          action = sectools_signer_act,
                          emitter = scons_secimage_builder.emitter,
                          suffix = '.mbn')

    env.Append(BUILDERS = {scons_secimage_builder.FUNC_HANDLER_NAME : sectools_signer_bld})


#------------------------------------------------------------------------------
# Parse command line arguments
#------------------------------------------------------------------------------
def parse_args():
    usage = "usage: %prog [options]"
    version = "%prog " + __version__

    parser = OptionParser(usage=usage, version=version)
    parser.add_option("-t", "--target_base_dir", metavar="<directory>", dest="target_base_dir",
                      help="Target base directory to store signed image outputs")
    parser.add_option("-i", "--image_file", metavar="<file>", dest="source",
                      help="Source file to sign")
    parser.add_option("-g", "--sign_id", metavar="<id>", dest="sign_id",
                      help="sign_id to execute")
    parser.add_option("--signer", metavar="<id>", dest="signer",
                      help="use different signer")
    parser.add_option("--qc_sign", metavar="<boolean>", dest="qc_sign",
                      action='store_true', default=False,
                      help="enable qc signing")
    parser.add_option("--install_base_dir", metavar="<dir>", dest="sectools_install_base_dir", default=None,
                      help="Base directory for installation. No installation will be performed if not specified.")
    parser.add_option("--install_file_name", metavar="<file>", default=None,
                      help="File name for installation. Original filename will be used if not specified.")
    parser.add_option("--msmid", metavar="<str>", dest="msmid", default=None,
                      help="Msmid to sign images. Msmid maps to JTAG ID defined in msm_jtag_mapping_file.'Default' msmid mapped to 0x00000000 will be used if both msmid and msm_jtagid_dict are not specified.")
    parser.add_option("--msm_jtagid_dict", metavar="<str>", dest="msmid_jtagid_dict", default = None,
                      help="MSM to JTAGID mapping dictionary. Default to {'default':'0x00000000'} if both msmid and msm_jtagid_dict are not specified.")
    parser.add_option("--msm_jtag_mapping_file", metavar="<file>", dest="msm_jtag_mapping_file",
                      default = secimage_builder.DEFAULT_MSM_JTAG_MAPPING_FILE,
                      help="Path for MSM to JTAG mapping file.")
    parser.add_option("--jtag_id", dest="jtag_id", default=None,
                      help="JTAG_ID to sign images. Cannot be used with SOC_HW_VERSION, JTAG_ID=0x00000000 will be used if neither JTAG_ID and SOC_HW_VERSION is specified.", metavar="<str>")
    parser.add_option("--soc_hw_version", metavar="<str>", dest="soc_hw_version", default = None,
                      help="SOC_HW_VERSION to sign image. Cannot be used with JTAG_ID. JTAG_ID=0x00000000 will be used if neither JTAG_ID and SOC_HW_VERSION is specified.")
    parser.add_option("--soc_vers", metavar="<str>", dest="soc_vers", default=None,
                      help="SOC_VERS is used to sign image for binary compatibility, up to 10 values")
    parser.add_option("--app_id", metavar="<str>", dest="app_id", default=None,
                      help="app_id to sign TZapps images")
    parser.add_option("--disable_featureflags", metavar="<str>", dest="disable_featureflags", action="append",
                      default = secimage_builder.DEFAULT_DISABLE_FEATURE_FLAGS,
                      help="Feature flags to disable signer. Support multiple feature flags.")
    parser.add_option("--config", metavar="<file>", dest="config",
                      default = secimage_builder.DEFAULT_CONFIG_FILE,
                      help="path to the secimage config file. Default: " + secimage_builder.DEFAULT_CONFIG_FILE)
    parser.add_option("--policy_file", metavar="<file>", dest="policy_file",
                      default = secimage_builder.DEFAULT_POLICY_FILE,
                      help='path to the sectools policy file.')

    (options, args) = parser.parse_args()

    if options.target_base_dir is None:
        print "Type sectools_builder.py -h for help."
        parser.error("target_base_dir is not specified.")
    if options.source is None:
        parser.error("source is not specified")
    if options.sign_id is None:
        parser.error("sign_id is not specified")
    if options.jtag_id is not None and options.soc_hw_version is not None:
        parser.error("either JTAG_ID or SOC_HW_VERSION should be specified, not both")

    return (options, args)

#------------------------------------------------------------------------------
# main
#------------------------------------------------------------------------------
if __name__ == "__main__":
    # get args from cmd line
    (options, args) = parse_args()

    env = None

    secimage_builder.build(
            env,
            target_base_dir=options.target_base_dir,
            source=options.source,
            sign_id=options.sign_id,
            signer=options.signer,
            qc_sign=options.qc_sign,
            sectools_install_base_dir=options.sectools_install_base_dir,
            install_file_name=options.install_file_name,
            msmid=options.msmid,
            msmid_jtagid_dict=options.msmid_jtagid_dict,
            msm_jtag_mapping_file=options.msm_jtag_mapping_file,
            jtag_id=options.jtag_id,
            soc_hw_version=options.soc_hw_version,
            soc_vers=options.soc_vers,
            app_id=options.app_id,
            disable_featureflags=options.disable_featureflags,
            config=options.config,
            policy_file=options.policy_file
            )
