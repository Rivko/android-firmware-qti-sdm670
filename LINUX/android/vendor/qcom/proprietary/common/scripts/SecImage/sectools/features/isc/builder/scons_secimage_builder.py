# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.builder.generic_secimage_builder import GenericSecImageBuilder
import os

FUNC_HANDLER_NAME = "SectoolSignerInternalBuilder"

class InputParam(object):
    SIGN_ID = "SIGN_ID"
    SIGNER = "SIGNER"
    QC_SIGN = "QC_SIGN"
    JTAG_ID = "JTAG_ID"
    SOC_HW_VERSION = "SOC_HW_VERSION"
    SOC_VERS = "SOC_VERS"
    APP_ID = 'APP_ID'
    CONFIG = "CONFIG"
    TARGET_BASE_DIR = "TARGET_BASE_DIR"
    SEC_IMAGE_POLICY = "SEC_IMAGE_POLICY"
    IMAGE_ENTRY = "IMAGE_ENTRY"
    RELOCATABLE = "RELOCATABLE"

def get_func_handler(env):
    return env.SectoolSignerInternalBuilder

def action(target, source, env):
    """get inputparams passed from SCONS ENV and call generic builder to sign the image"""
    genericbuilder = GenericSecImageBuilder(env)
    return genericbuilder.build(
            str(target[0]),
            str(source[0]),
            sign_id=env.get(InputParam.SIGN_ID, None),
            signer=env.get(InputParam.SIGNER, None),
            qc_sign=env.get(InputParam.QC_SIGN, False),
            jtag_id=env.get(InputParam.JTAG_ID, None),
            soc_hw_version=env.get(InputParam.SOC_HW_VERSION, None),
            soc_vers=env.get(InputParam.SOC_VERS, None),
            app_id=env.get(InputParam.APP_ID, None),
            config=env.get(InputParam.CONFIG, None),
            target_base_dir=env.get(InputParam.TARGET_BASE_DIR, None),
            sec_image_policy=env.get(InputParam.SEC_IMAGE_POLICY, None),
            image_entry=env.get(InputParam.IMAGE_ENTRY, None),
            relocatable=env.get(InputParam.RELOCATABLE, False))

def emitter(target, source, env):
    target_file = os.path.basename(str(target[0]))
    target_base_dir = env.get(InputParam.TARGET_BASE_DIR, None)

    for root, dirnames, filenames in os.walk(target_base_dir):
        for filename in filenames:
            if filename != target_file:
                env.Clean(target, os.path.join(root, filename))

    return (target, source)
