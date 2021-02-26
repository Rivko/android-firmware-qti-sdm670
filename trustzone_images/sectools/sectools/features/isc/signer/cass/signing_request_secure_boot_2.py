# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.utils.c_logging import logger
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_2_0
from sectools.features.isc.signer.cass.signing_request_base import BaseCASSSigningRequest
from sectools.features.isc.signer.signerutils.certificate import SignAttr


class SecureBoot2SigningRequest(BaseCASSSigningRequest):

    AUTH_SN = "AUTH_SN"
    DEBUG_SERIAL = "DEBUG_SERIAL"
    DEBUG_TYPE = "DEBUG_TYPE"
    HARDWARE_ID = "HARDWARE_ID"

    def __init__(self, hash_to_sign, data_to_sign, image_path, signing_attributes):
        logger.debug("Creating CASS Secure Boot 2 signing request...")
        self.signing_attributes = signing_attributes
        BaseCASSSigningRequest.__init__(self, hash_to_sign, data_to_sign, image_path, signing_attributes)

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def get_plugin_id(cls):
        return "secure_boot_2_cass_signing_request"

    @classmethod
    def get_supported_capabilities(cls):
        return ["qsee_production_debugpolicy",
                "qsee_production_nazgul",
                "qsee_test_nazgul",
                "qsee_test_starlord",
                "qsee_test_napali",
                "spss_test_nazgul",
                "xblsec_nazgul",
                "xblsec_presilicon",
                "qsee_test_presilicon",
                "qsee_prod_presilicon",
                "spss_test_presilicon",
                "spss_prod_nazgul",
                "spss_prod_nazgul_with_serial_number",
                "spss_test_presilicon",
                "spss_prod_presilicon",
                "spu_prod_v1_sdm855_apps_release",
                "spu_test_v1_apps_release",
                "spu_prod_v1_sdm855_kernel_release",
                "spu_test_v1_kernel_release",
                "spu_prod_v1_sdm855_apps_serial",
                "spu_test_v1_apps_serial",
                "spu_prod_v1_sdm855_kernel_serial",
                "spu_test_v1_kernel_serial",
                "quartz_patch",
                "quartz_patch_with_serial",
                "secboot_sha2_pss_subca1",
                "qtee_prod_v1_sdm670_qtee_serial",
                "qtee_prod_v1_sdm845_qtee_serial"]

    @classmethod
    def get_secboot_version(cls):
        return SECBOOT_VERSION_2_0

    def _create_attributes_to_add(self):
        additions = {}
        sa = self.signing_attributes

        # Add required attributes

        # HARDWARE_ID
        if sa.in_use_soc_hw_version == 1:
            additions.update({SecureBoot2SigningRequest.HARDWARE_ID: sa.soc_hw_version})
            logger.debug("Using soc_hw_version for HARDWARE_ID value.")
        else:
            additions.update({SecureBoot2SigningRequest.HARDWARE_ID: sa.msm_part})
            logger.debug("Using msm_part for HARDWARE_ID value.")

        # Add optional attributes

        # SOC_VERS
        if sa.soc_vers:
            soc_vers = [soc_ver[2:] for soc_ver in sa.soc_vers]
            additions.update({SignAttr.SOC_VERS: ",".join(soc_vers)})

        # IN_USE_SOC_HW_VERSION
        if sa.in_use_soc_hw_version == 1:
            additions.update({SignAttr.IN_USE_SOC_HW_VERSION: "TRUE"})
        else:
            additions.update({SignAttr.IN_USE_SOC_HW_VERSION: "FALSE"})

        # USE_SERIAL_NUMBER_IN_SIGNING
        if sa.use_serial_number_in_signing == 1:
            additions.update({SignAttr.USE_SERIAL_NUMBER_IN_SIGNING: "TRUE"})
        else:
            additions.update({SignAttr.USE_SERIAL_NUMBER_IN_SIGNING: "FALSE"})

        # OEM_ID_INDEPENDENT
        if sa.oem_id_independent == 1:
            additions.update({SignAttr.OEM_ID_INDEPENDENT: "TRUE"})
        else:
            additions.update({SignAttr.OEM_ID_INDEPENDENT: "FALSE"})

        # AUTH_SN
        if sa.multi_serial_numbers is not None and len(sa.multi_serial_numbers.serial) > 0:
            serials = [serial[2:] for serial in sa.multi_serial_numbers.serial]
            additions.update({SecureBoot2SigningRequest.AUTH_SN: ",".join(serials)})

        # DEBUG_SERIAL
        # DEBUG_TYPE
        additions.update({SecureBoot2SigningRequest.DEBUG_SERIAL: sa.debug[0:10]})
        additions.update({SecureBoot2SigningRequest.DEBUG_TYPE: "0x" + sa.debug[10:18]})

        # EXPONENT
        # if sa.exponent is not None:
        #     additions.update({BaseCASSSigningRequest.EXPONENT: hex(sa.exponent)})

        # UIE_KEY_SWITCH_ENABLE
        if int(sa.uie_key_switch_enable, 16):
            additions.update({SignAttr.UIE_KEY_SWITCH_ENABLE: sa.uie_key_switch_enable})

        return additions

    def _create_attributes_to_override(self):
        overrides = dict()

        # Split serial_number between OEM_ID AND MODEL_ID
        # This is required because CASS API does not allow serial_number to be sent to the server
        if self.signing_attributes.use_serial_number_in_signing == 1:
            serial = "{0:#0{1}x}".format(int(self.signing_attributes.serial_number, 16), 10)
            # OEM_ID
            overrides[SignAttr.OEM_ID] = {SignAttr.OEM_ID: serial[:6]}
            # MODEL_ID
            overrides[SignAttr.MODEL_ID] = {SignAttr.MODEL_ID: "0x" + serial[6:]}

        return overrides

    def _create_attributes_to_remove(self):
        return [SignAttr.HW_ID]
