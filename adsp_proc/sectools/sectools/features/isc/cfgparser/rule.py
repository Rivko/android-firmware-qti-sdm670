# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
Created on Mar 18, 2013

@author: francisg

This module provides a rule class for secimage config rules.
"""

from auto_gen_xml_config import complex_metadata, complex_signing, complex_images_list, complex_general_properties
from sectools.common.utils import c_path
from sectools.common.utils.c_misc import is_TA
from sectools.common.utils.c_data import get_duplicates, remove_duplicates
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_rule import CoreRuleBase
from sectools.features.isc.cfgparser.defines import JTAGID_SIGN_IDS, MRC_1_0_CHIPSETS, MRC_2_0_CHIPSETS, SECBOOT_2_0_CHIPSETS, SECBOOT_3_0_CHIPSETS, SECBOOT_2_0_DOUBLE_SIGN_CHIPSETS, SECBOOT_3_0_DOUBLE_SIGN_CHIPSETS, ECDSA_CHIPSETS, SECBOOT_1_IMAGE_TYPES
from sectools.features.isc.defines import SECBOOT_VERSION_1_0, SECBOOT_VERSION_2_0, SECBOOT_VERSION_3_0

MAX_NUM_SOC_VERS_10 = 10
MAX_NUM_SOC_VERS_12 = 12
MAX_NUM_SOC_VERS_MAP = {
    SECBOOT_VERSION_1_0: MAX_NUM_SOC_VERS_10,
    SECBOOT_VERSION_2_0: MAX_NUM_SOC_VERS_10,
    SECBOOT_VERSION_3_0: MAX_NUM_SOC_VERS_12
}
MAX_NUM_SERIALS_201 = 201
MAX_NUM_SERIALS_8 = 8
MAX_NUM_SERIALS_MAP = {
    SECBOOT_VERSION_1_0: MAX_NUM_SERIALS_201,
    SECBOOT_VERSION_2_0: MAX_NUM_SERIALS_201,
    SECBOOT_VERSION_3_0: MAX_NUM_SERIALS_8
}

class ConfigRulesManager(CoreRuleBase):
    """
    This is the main SecImage config rules manager that runs the rules
    """

    def __init__(self):
        self.configs = {}
        self.configs['signing'] = _Signing()
        self.configs['images_list'] = _ImageList()
        self.configs['general_properties'] = _GeneralProperties()
        self.configs['metadata'] = None
        self.configs['parsegen'] = None
        self.configs['post_process'] = None
        pass

    def validate(self, data, data_dict):
        retval = True
        error_str = ''

        # based on the dictionary structure, go through each section from root
        for name in data_dict:
            # check if there's a registered rule object for this config section
            if name in self.configs and self.configs[name] is not None:
                config = getattr(data.root, name)
                if name == 'general_properties':
                    ret, error = self.configs[name].validate(config, getattr(data.root, 'signing'))
                elif name == 'images_list':
                    ret, error = self.configs[name].validate(config, getattr(data.root, 'general_properties'), getattr(data.root, 'metadata'))
                else:
                    ret, error = self.configs[name].validate(config, config)

                # accumulate the return values
                if ret == False:
                    retval &= ret
                    error_str += '\n\n<%s>%s' % (name, error)

        if retval == False:
            raise RuntimeError('\nSecImage config validation failed with following error(s): %s' % error_str)



class _Signing(object):
    """
    Defines the rules for signing default attributes
    """

    def __init__(self):
        pass

    def validate(self, signing, *args):
        assert(isinstance(signing, complex_signing))

        retval = True
        error_str = ''

        # signing paths for trust_keystore and keystore_file from cass_signer_attributes
        cass_signer_attr = signing.get_signer_attributes().get_cass_signer_attributes()
        if cass_signer_attr is not None:
            # keystore rule, they must all exists
            if cass_signer_attr.get_server():
                self.trust_keystore = cass_signer_attr.get_server().get_trust_keystore()
                if c_path.validate_file(self.trust_keystore) is False:
                    retval = False
                    error_str += '\n trust_keystore is invalid, path = %s' % self.trust_keystore

            self.keystore_file = cass_signer_attr.get_user_identity().get_keystore_file()
            if self.keystore_file and c_path.validate_file(self.keystore_file) is False:
                retval = False
                error_str += '\n keystore_file is invalid, path = %s' % self.keystore_file

        return retval, error_str


class _GeneralProperties(object):
    """
    Defines the rules for general properties
    """

    def __init__(self):
        pass

    def validate(self, general_properties, *args):
        assert(isinstance(general_properties, complex_general_properties))

        retval = True
        error_str = ''

        return retval, error_str


class _ImageList(object):
    """
    Defines the rules for image parameters to be signed
    """

    def __init__(self):
        self.mask_warning = True
        self.version_warning = True

    def validate(self, images, *args):
        assert(isinstance(images, complex_images_list))

        image_list = images.get_image()
        retval = [True]
        errors = []

        def add_error(error):
            retval[0] = False
            errors.append("\n"+error)

        def validate_hex_list(hex_list, max_num_allowed, list_name, item_name, length_of_item, zero_is_invalid=True):
            if len(hex_list) > max_num_allowed:
                add_error("{0} provided {1}s exceeds allowed maximum of {2}".format(len(hex_list), item_name, max_num_allowed))

            for val in hex_list:
                try:
                    if val[: 2] != "0x" or len(val) != length_of_item or (int(val, 16) == 0 and zero_is_invalid):
                        raise ValueError("malformed")
                    int(val, 16)
                except ValueError:
                    add_error("Provided {0} value \"{1}\" is not a valid {2}".format(list_name, val, item_name))

            duplicates = get_duplicates(hex_list)
            if len(duplicates) > 0:
                if len(duplicates) == 1:
                    error_str = "{0} contains a duplicate of the following value: {1}".format(list_name, ", ".join(duplicates))
                else:
                    error_str = "{0} contains duplicates of the following values: ".format(list_name, ", ".join(duplicates))
                add_error(error_str)

        def validate_serial_bound_value(value,
                                        value_name,
                                        sign_id,
                                        chipset=None,
                                        allowed_chipsets=None,
                                        num_root_certs=None,
                                        ignore_num_root_certs=False,
                                        disable_in_value_binding=False,
                                        additional_serials_are_bindable=False,
                                        allowed_lsb_values=None):
            if value is not None:
                if chipset is not None and allowed_chipsets is not None and chipset not in allowed_chipsets:
                    add_error("{0} is only allowed to be set for chipsets: {1}".format(value_name, ", ".join(allowed_chipsets)))
                elif ignore_num_root_certs or num_root_certs > 1:
                    upper_8_bytes = int(value, 16) & 0xFFFFFFFF00000000
                    lower_8_bytes = int(value, 16) & 0xFFFFFFFF

                    if allowed_lsb_values is None:
                        allowed_lsb_values = [0, 1, 2]
                    allowed_lsb_hex_values = ", ".join([hex(x) for x in allowed_lsb_values])
                    if disable_in_value_binding:
                        if int(value, 16) not in allowed_lsb_values:
                            add_error("sign_id={0}: {1} must be be set to one of the following: {2}. {1} = {3}".format(sign_id, value_name, allowed_lsb_hex_values, value))
                        if upper_8_bytes != 0 or lower_8_bytes == allowed_lsb_values[len(allowed_lsb_values)-1] and not additional_serials_are_bindable:
                            add_error("sign_id={0}: to enable {1} serial number binding, add serial to multi_serial_numbers".format(sign_id, value_name))
                    else:
                        if lower_8_bytes not in allowed_lsb_values:
                            add_error("sign_id={0}: {1} must end with one of the following: {2}. {1} = {3}".format(sign_id, value_name, ", ".join(str(v) for v in allowed_lsb_values), value))
                        # if last 8 bytes == 1, enabling with no serial binding so use as is
                        # if last 8 bytes == 2, enabling with serial binding so check upper 8 bytes are valid serial
                        if upper_8_bytes == 0 and lower_8_bytes == allowed_lsb_values[len(allowed_lsb_values)-1]:
                            add_error("sign_id={0}: to enable {1} with binding to serial number, upper 8 bytes must contain serial number. {1} = {2}".format(sign_id, value_name, value))

        # expect args[0] to be instance of signing
        # the following default signing attributes are checked if signing is TCG
        assert(isinstance(args[0], complex_general_properties))
        general_properties = args[0]
        default_sw_id = general_properties.get_sw_id()
        default_anti_rollback_version = general_properties.get_anti_rollback_version()
        default_app_id = general_properties.get_app_id()
        default_rot_en = general_properties.get_rot_en()
        default_crash_dump = general_properties.get_crash_dump()
        default_msm_part = general_properties.get_msm_part()
        default_soc_hw_version = general_properties.get_soc_hw_version()
        default_soc_vers = general_properties.get_soc_vers()
        default_mask_soc_hw_version = general_properties.get_mask_soc_hw_version()
        default_in_use_soc_hw_version = general_properties.get_in_use_soc_hw_version()
        default_use_serial_number_in_signing = general_properties.get_use_serial_number_in_signing()
        default_serial_number = general_properties.get_serial_number()
        default_revocation_enablement = general_properties.get_revocation_enablement()
        default_activation_enablement = general_properties.get_activation_enablement()
        default_root_revoke_activate_enable = general_properties.get_root_revoke_activate_enable()
        default_oem_id = general_properties.get_oem_id()
        default_model_id = general_properties.get_model_id()
        default_debug = general_properties.get_debug()
        default_multi_serial_numbers = general_properties.get_multi_serial_numbers().get_serial() if general_properties.get_multi_serial_numbers() is not None else []
        default_oid = general_properties.get_object_id()
        default_oid_min = default_oid.min if default_oid is not None else None
        default_oid_max = default_oid.max if default_oid is not None else None
        default_hash_algorithm = general_properties.get_hash_algorithm()
        default_hmac = general_properties.get_hmac()
        default_rsa_padding = general_properties.get_rsa_padding()
        default_num_root_certs = general_properties.get_num_root_certs()
        default_max_num_root_certs = general_properties.get_max_num_root_certs()
        default_mrc_index = general_properties.get_mrc_index()
        default_secboot_version = general_properties.get_secboot_version()
        default_dsa_type = general_properties.get_dsa_type()
        default_ecdsa_curve = general_properties.get_ecdsa_curve()
        default_key_size = general_properties.get_key_size()
        default_exponent = general_properties.get_exponent()
        default_uie_key_switch_enable = general_properties.get_uie_key_switch_enable()
        default_client_id = general_properties.get_client_id()
        default_lib_id = general_properties.get_lib_id()
        default_UIE_server_cert_path = general_properties.get_UIE_server_cert_path()

        assert(isinstance(args[1], complex_metadata))
        metadata = args[1]
        chipset = metadata.get_chipset()

        for image in image_list:
            sign_id = image.get_sign_id()
            image_type = image.get_image_type()
            overrides = image.get_general_properties_overrides()

            sw_id = overrides.get_sw_id() if overrides.get_sw_id() is not None else default_sw_id
            anti_rollback_version = overrides.get_anti_rollback_version() if overrides.get_anti_rollback_version() is not None else default_anti_rollback_version
            app_id = overrides.get_app_id() if overrides.get_app_id() is not None else default_app_id
            rot_en = overrides.get_rot_en() if overrides.get_rot_en() is not None else default_rot_en
            crash_dump = overrides.get_crash_dump() if overrides.get_crash_dump() is not None else default_crash_dump
            msm_part = overrides.get_msm_part() if overrides.get_msm_part() is not None else default_msm_part
            soc_hw_version = overrides.get_soc_hw_version() if overrides.get_soc_hw_version() is not None else default_soc_hw_version
            soc_vers = overrides.get_soc_vers() if overrides.get_soc_vers() is not None else default_soc_vers
            mask_soc_hw_version = overrides.get_mask_soc_hw_version() if overrides.get_mask_soc_hw_version() is not None else default_mask_soc_hw_version
            in_use_soc_hw_version = overrides.get_in_use_soc_hw_version() if overrides.get_in_use_soc_hw_version() is not None else default_in_use_soc_hw_version
            use_serial_number_in_signing = overrides.get_use_serial_number_in_signing() if overrides.get_use_serial_number_in_signing() is not None else default_use_serial_number_in_signing
            serial_number = overrides.get_serial_number() if overrides.get_serial_number() is not None else default_serial_number
            revocation_enablement = overrides.get_revocation_enablement() if overrides.get_revocation_enablement() is not None else default_revocation_enablement
            activation_enablement = overrides.get_activation_enablement() if overrides.get_activation_enablement() is not None else default_activation_enablement
            root_revoke_activate_enable = overrides.get_root_revoke_activate_enable() if overrides.get_root_revoke_activate_enable() is not None else default_root_revoke_activate_enable
            oem_id = overrides.get_oem_id() if overrides.get_oem_id() is not None else default_oem_id
            model_id = overrides.get_model_id() if overrides.get_model_id() is not None else default_model_id
            debug = overrides.get_debug() if overrides.get_debug() is not None else default_debug
            multi_serial_numbers = list(overrides.get_multi_serial_numbers().get_serial() if overrides.get_multi_serial_numbers() is not None else default_multi_serial_numbers)
            override_oid = overrides.get_object_id() if overrides.get_object_id() is not None else default_oid
            oid_min = override_oid.min if override_oid is not None else default_oid_min
            oid_max = override_oid.max if override_oid is not None else default_oid_max
            hmac = overrides.get_hmac() if overrides.get_hmac() is not None else default_hmac
            rsa_padding = overrides.get_rsa_padding() if overrides.get_rsa_padding() is not None else default_rsa_padding
            hash_algorithm = overrides.get_hash_algorithm() if overrides.get_hash_algorithm() is not None else default_hash_algorithm
            num_root_certs = overrides.get_num_root_certs() if overrides.get_num_root_certs() is not None else default_num_root_certs
            max_num_root_certs = overrides.get_max_num_root_certs() if overrides.get_max_num_root_certs() is not None else default_max_num_root_certs
            mrc_index = overrides.get_mrc_index() if overrides.get_mrc_index() is not None else default_mrc_index
            secboot_version = overrides.get_secboot_version() if overrides.get_secboot_version() is not None else default_secboot_version
            dsa_type = overrides.get_dsa_type() if overrides.get_dsa_type() is not None else default_dsa_type
            ecdsa_curve = overrides.get_ecdsa_curve() if overrides.get_ecdsa_curve() is not None else default_ecdsa_curve
            key_size = overrides.get_key_size() if overrides.get_key_size() is not None else default_key_size
            exponent = overrides.get_exponent() if overrides.get_exponent() is not None else default_exponent
            uie_key_switch_enable = overrides.get_uie_key_switch_enable() if overrides.get_uie_key_switch_enable() is not None else default_uie_key_switch_enable
            client_id = overrides.get_client_id() if overrides.get_client_id() is not None else default_client_id
            lib_id = overrides.get_lib_id() if overrides.get_lib_id() is not None else default_lib_id
            UIE_server_cert_path = overrides.get_UIE_server_cert_path() if overrides.get_UIE_server_cert_path() is not None else default_UIE_server_cert_path

            # Secboot version checking
            if secboot_version is None or (image_type in SECBOOT_1_IMAGE_TYPES and secboot_version != SECBOOT_VERSION_1_0):
                # general properties update occurs in imageinfo's _sanitize_general_properties (like for all other config value updates)
                secboot_version = SECBOOT_VERSION_1_0
            if num_root_certs > 1 and secboot_version != SECBOOT_VERSION_1_0 and chipset not in (SECBOOT_2_0_CHIPSETS + SECBOOT_3_0_CHIPSETS):
                # Multiple root certs not allowed with secboot 2.0 and greater multi-party signing
                add_error("Multiple root certs are not supported for secboot {0} chipset {1}".format(str(secboot_version), chipset))

            # Only Secboot 3 supports anti_rollback_version as its own value
            if secboot_version == SECBOOT_VERSION_3_0:
                if anti_rollback_version is None:
                    add_error("Secboot {0} requires anti_rollback_version field. sign_id={1}".format(secboot_version, sign_id))
                if (int(sw_id, 16) & 0xFFFFFFFF00000000) != 0:
                    add_error("Provide anti-rollback version in anti_rollback_version field for secboot {0} chipset. sign_id={1}".format(secboot_version, sign_id))
            else:
                if anti_rollback_version is not None:
                    if int(sw_id, 16) & 0xFFFFFFFF00000000 == 0:
                        hex_sw_id = hex(int(sw_id, 16) | (int(anti_rollback_version, 16) << 32))
                        hex_sw_id = hex_sw_id[:-1] if "L" in hex_sw_id else hex_sw_id
                        sw_id = hex_sw_id
                    else:
                        add_error("Anti-rollback version provided in both sw_id field and anti_rollback_version field."
                                  "\nRemove anti_rollback_version field for secboot {0} chipset. sign_id={1}".format(secboot_version, sign_id))

            # Default hash algorithm to sha256
            if hash_algorithm is None:
                hash_algorithm = "sha256"

            # Backward compatibility: dsa_type is optional, so if it does not exist assume RSA
            if dsa_type is None:
                dsa_type = "rsa"

            # Manually check that ecdsa / rsa arguments exist when the corresponding
            if dsa_type == "rsa":
                if exponent is None or key_size is None:
                    add_error("You must specify exponent and key size when using RSA")
            if dsa_type == "ecdsa":
                general_properties.exponent = None
                general_properties.key_size = None
                if ecdsa_curve is None:
                    add_error("You must specify the ecdsa curve when using ECDSA")

                # Check that ecdsa value is only used with supported chipset
                if chipset not in ECDSA_CHIPSETS:
                    add_error("ECDSA is not supported for chipset {0}".format(chipset))
            else:
                # RSAPSS requires SHA256 or SHA384
                hmac = hmac or hmac is None
                if rsa_padding == "pss":
                    if hash_algorithm not in ["sha256", "sha384"]:
                        add_error("RSAPSS requires SHA256 or SHA384 hash. sign_id={0}, hash_algorithm={1}".format(sign_id, hash_algorithm))
                    if hmac:
                        add_error("RSAPSS cannot be used with HMAC. hmac must be set as False. sign_id={0}, hmac={1}".format(sign_id, hmac))
                else:
                    if not hmac:
                        add_error("RSAPKCS should be used with HMAC. sign_id={0}".format(sign_id))

            # Secboot 3 only support sha256 and sha384
            if secboot_version == SECBOOT_VERSION_3_0:
                if hash_algorithm not in ["sha256", "sha384"]:
                    add_error("Secboot {0} requires SHA256 or SHA384 hash".format(secboot_version))

            # TZ apps rule, must have app_id set
            if is_TA(sw_id):
                if app_id is None or int(app_id, 16) == 0:
                    add_error("app_id is not set for TZ apps: sign_id={0}, sw_id={1}".format(sign_id, sw_id))
            #TODO: app_id is image specific, current rule checking will cover all the images in the config file
            """
            # other than tz, app_id must not be present
            else:
                if app_id is not None:
                    retval = False
                    error_str += '\n app_id is set for Non-TZ image: sign_id=%s, sw_id=%s, app_id=%s' % (sign_id, sw_id, app_id)
            """

            # crash_dump rule, LSB 32bits must not be greater than 1
            if crash_dump is not None and (int(crash_dump, 16) & 0xFFFFFFFF) > 1:
                add_error("crash_dump 32bit LSB must be 0 or 1: sign_id={0}, crash_dump={1}".format(sign_id, crash_dump))

            # rot_en rule, LSB 32bits must not be greater than 1
            validate_serial_bound_value(rot_en,
                                        "rot_en",
                                        sign_id,
                                        chipset,
                                        ignore_num_root_certs=True,
                                        disable_in_value_binding=secboot_version == SECBOOT_VERSION_3_0,
                                        additional_serials_are_bindable=len(multi_serial_numbers) > 0,
                                        allowed_lsb_values=[0, 1])

            # At least one of MSM_PART or SOC_HW_VERSION must be specified
            if msm_part is None and soc_hw_version is None:
                add_error("sign_id={0}: MSM_PART and SOC_HW_VERSION are missing from config. At least one must exist".format(sign_id))

            # Only use Family and Device number for soc_hw_version:
            if soc_hw_version is not None and len(soc_hw_version) != 10:
                add_error("soc_hw_version value {0} is not valid. Value must start with 0x and be 8 bytes: sign_id={1}".format(soc_hw_version, sign_id))

            # Check if any sign_ids in SOC-chipset need to sign with JTAG ID
            if (soc_hw_version is not None) and (chipset in JTAGID_SIGN_IDS) and (sign_id in JTAGID_SIGN_IDS[chipset]) and (msm_part is None):
                add_error("sign_id={0}: MSM_PART must be used to sign this image but MSM_PART is missing".format(sign_id))

            # Assure in_use_soc_hw_version exists if both msm_part and soc_hw_version are given
            if soc_hw_version is not None and msm_part is not None and in_use_soc_hw_version is None:
                add_error("sign_id={0}: IN_USE_SOC_HW_VERSION must exist to chose between MSM_PART and SOC_HW_VERSION".format(sign_id))

            # in_use_soc_hw_version must exist with soc_hw_version
            if soc_hw_version is not None and in_use_soc_hw_version is None:
                add_error("sign_id={0}: IN_USE_SOC_HW_VERSION must be set when using SOC_HW_VERSION".format(sign_id))

            # Make sure in_use_soc_hw_version's specification exists
            if in_use_soc_hw_version == 1 and soc_hw_version is None:
                add_error("sign_id={0}: IN_USE_SOC_HW_VERSION specifies SOC_HW_VERSION is used but SOC_HW_VERSION tag is missing".format(sign_id))
            elif in_use_soc_hw_version == 0 and msm_part is None:
                add_error("sign_id={0}: IN_USE_SOC_HW_VERSION specifies SOC_HW_VERSION is NOT used but MSM_PART tag is missing".format(sign_id))

            # Validate soc_vers
            if soc_vers is not None and in_use_soc_hw_version == 1:
                soc_vers_list = soc_vers.split()
                validate_hex_list(soc_vers_list, MAX_NUM_SOC_VERS_MAP[secboot_version], "soc_vers", "soc_ver", 6)

            # mask_soc_hw_version may only exist if soc_hw_version exists
            if soc_hw_version is None:
                if mask_soc_hw_version is not None:
                    add_error("sign_id={0}: MASK_SOC_HW_VERSION can not exist without the SOC_HW_VERSION tag".format(sign_id))

            if mask_soc_hw_version is not None and in_use_soc_hw_version == 1 and self.mask_warning is True:
                logger.warning("The mask_soc_hw_version field is set and will mask the soc_hw_version during signing. Please ensure this is the desired result.")
                self.mask_warning = False

            # use_serial_number_in_signing rule: serial number must be set
            if use_serial_number_in_signing == 1:
                if serial_number is None or int(serial_number, 16) == 0:
                    add_error("sign_id={0}: serial_number must be set when use_serial_number_in_signing is enabled".format(sign_id))
                elif len(serial_number) > 10:
                    add_error("sign_id={0}: serial_number value must be 10 characters or less in length".format(sign_id))

            # Validate uie_key_switch_enable
            validate_serial_bound_value(uie_key_switch_enable,
                                        "uie_key_switch_enable",
                                        sign_id,
                                        chipset,
                                        ignore_num_root_certs=True,
                                        disable_in_value_binding=secboot_version == SECBOOT_VERSION_3_0,
                                        additional_serials_are_bindable=len(multi_serial_numbers) > 0)

            # Ensure MRC targets are properly configured to avoid OU field conflicts
            misconfigured_mrc_chipsets = get_duplicates(MRC_1_0_CHIPSETS + MRC_2_0_CHIPSETS)
            if misconfigured_mrc_chipsets:
                raise RuntimeError("MRC 1.0 and MRC 2.0 chipsets must be mutually exclusive. The following chipsets are configured incorrectly: {0}".format(", ".join(misconfigured_mrc_chipsets)))

            # MRC 1.0 use case
            # Validate revocation_enablement
            validate_serial_bound_value(revocation_enablement,
                                        "revocation_enablement",
                                        sign_id,
                                        chipset,
                                        MRC_1_0_CHIPSETS,
                                        num_root_certs)
            # Validate activation_enablement
            validate_serial_bound_value(activation_enablement,
                                        "activation_enablement",
                                        sign_id,
                                        chipset,
                                        MRC_1_0_CHIPSETS,
                                        num_root_certs)

            # MRC 2.0 use case
            # Validate root_revoke_activate_enable
            validate_serial_bound_value(root_revoke_activate_enable,
                                        "root_revoke_activate_enable",
                                        sign_id,
                                        chipset,
                                        MRC_2_0_CHIPSETS,
                                        num_root_certs,
                                        disable_in_value_binding=secboot_version == SECBOOT_VERSION_3_0,
                                        additional_serials_are_bindable=len(multi_serial_numbers) > 0)

            # Validate MRC cert values
            if chipset in MRC_2_0_CHIPSETS and chipset in SECBOOT_2_0_DOUBLE_SIGN_CHIPSETS + SECBOOT_3_0_DOUBLE_SIGN_CHIPSETS and max_num_root_certs is None:
                add_error("max_num_root_certs must be provided for chipset \"{0}\"".format(chipset))

            # MRC 1.0 support up to 4 roots, MRC 2.0 supports 1 or 4 roots
            if chipset in MRC_1_0_CHIPSETS or chipset in MRC_2_0_CHIPSETS:
                if max_num_root_certs is not None and num_root_certs > max_num_root_certs:
                    add_error("num_root_certs must be less than or equal to max_num_root_certs: num_root_certs={0}, max_num_root_certs={1}".format(num_root_certs, max_num_root_certs))
                if chipset in MRC_1_0_CHIPSETS + MRC_2_0_CHIPSETS and num_root_certs not in [1, 2, 3, 4]:
                        add_error("num_root_certs must be in range [1-4]")
            # Legacy MRC supports up to 16 roots
            elif num_root_certs < 1 or num_root_certs > 16:
                add_error("num_root_certs must be in range [1-16]")

            if num_root_certs > 1 and mrc_index is None:
                add_error("mrc_index must be provided when num_root_certs is greater than 1")

            if mrc_index is not None and mrc_index >= num_root_certs:
                add_error("Index out of range: mrc_index={0}, num_root_certs={1}".format(mrc_index, num_root_certs))

            # Format and validate debug
            if debug is not None:
                if len(debug) > 18:
                    add_error("Debug value must be 18 characters or less in length")
                # Secboot 3 only supports serial numbers in multi_serial_numbers, not in debug
                elif secboot_version == SECBOOT_VERSION_3_0:
                    validate_serial_bound_value(debug,
                                                "debug",
                                                sign_id,
                                                chipset,
                                                ignore_num_root_certs=True,
                                                disable_in_value_binding=secboot_version == SECBOOT_VERSION_3_0,
                                                additional_serials_are_bindable=len(multi_serial_numbers) > 0,
                                                allowed_lsb_values=[0, 1, 2])
                elif len(debug) < 18:
                    padding_len = 18 - len(debug)
                    debug = debug[:2] + "0"*padding_len + debug[2:]

            # Validate the multi_serial_numbers
            if len(multi_serial_numbers) > 0:
                if chipset not in SECBOOT_2_0_CHIPSETS + SECBOOT_3_0_CHIPSETS:
                    if debug is None:
                        add_error("Debug serials were provides but debug field was not provided")
                    elif debug[:-8] in multi_serial_numbers:
                        add_error("Duplicate serial value of {0} in debug and multi_serial_numbers".format(debug[:-8]))
                    elif int(debug[:-8], 16) == 0:
                        add_error("Debug value {0} is invalid when providing debug serials. The top half of debug cannot equal zero".format(debug))
                    else:
                        multi_serial_numbers.append(debug[:-8])
                validate_hex_list(multi_serial_numbers, MAX_NUM_SERIALS_MAP[secboot_version], "multi_serial_numbers", "serial", 10, False)

            # TCG rules
            if oid_min is not None and oid_max is None:
                add_error("In sign_id = {0}, {1} min is set, must also set max".format(sign_id, default_oid.name))
            elif oid_max is not None and oid_min is None:
                add_error("In sign_id = {0}, {1} max is set, must also set min".format(sign_id, default_oid.name))
            elif default_oid is not None:
                # Move the min > max checking to signer. It should be validated after valid 32
                # bit integer is checked. Otherwise, int() conversion will throw an exception

                oid_min_config_str = oid_min
                oid_max_config_str = oid_max
                if oid_min_config_str and oid_max_config_str:
                    oid_min = int(oid_min, 16) if "0x" in oid_min else int(oid_min)
                    oid_max = int(oid_max, 16) if "0x" in oid_max else int(oid_max)

                    if oid_min > oid_max:
                        add_error("For sign_id = {0}, {1} min must be less than max, min={2} max={3}".format(sign_id, default_oid.name, oid_min_config_str, oid_max_config_str))
                if int(sw_id, 16) != 0:
                    add_error("For {0} sign_id = {1}, sw_id must be 0, sw_id = {2}".format(default_oid.name, sign_id, sw_id))
                if int(msm_part, 16) != 0:
                    add_error("For {0} sign_id = {1}, msm_part must be 0, msm_part = {2}".format(default_oid.name, sign_id, msm_part))
                if int(oem_id, 16) != 0:
                    add_error("For {0} sign_id = {1}, oem_id must be 0, oem_id = {2}".format(default_oid.name, sign_id, oem_id))
                if int(model_id, 16) != 0:
                    add_error("For {0} sign_id = {1}, model_id must be 0, model_id = {2}".format(default_oid.name, sign_id, model_id))
                if int(debug, 16) != 2:
                    add_error("For {0} sign_id = {1}, debug must be 2, debug = {2}".format(default_oid.name, sign_id, debug))

            if client_id is not None and int(client_id, 16) == 0:
                    add_error("client_id must be a non-zero value, sign_id={0}, client_id={1}".format(sign_id, client_id))
            if lib_id is not None and int(lib_id, 16) == 0:
                add_error("lib_id must be a non-zero value, sign_id={0}, lib_id={1}".format(sign_id, lib_id))

            if UIE_server_cert_path and c_path.validate_file(UIE_server_cert_path) is False:
                add_error(sign_id, "UIE_server_cert_path is invalid, path={0}".format(UIE_server_cert_path))

        return retval[0], "".join(remove_duplicates(errors))


