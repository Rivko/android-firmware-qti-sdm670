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

        def add_error(sign_id, error):
            retval[0] = False
            errors.append("\nsign_id={0}: ".format(sign_id) + error)

        def validate_hex_list(sign_id, hex_list, max_num_allowed, list_name, item_name, length_of_item):
            if len(hex_list) > max_num_allowed:
                add_error(sign_id, "{0} provided {1}s exceeds allowed maximum of {2}".format(len(hex_list), item_name, max_num_allowed))

            for val in hex_list:
                try:
                    if val[: 2] != "0x" or len(val) != length_of_item or (int(val, 16) == 0):
                        raise ValueError("malformed")
                    int(val, 16)
                except ValueError:
                    add_error(sign_id, "Provided {0} value \"{1}\" is not a valid {2}".format(list_name, val, item_name))

            duplicates = get_duplicates(hex_list)
            if len(duplicates) > 0:
                if len(duplicates) == 1:
                    error_str = "{0} contains a duplicate of the following value: {1}".format(list_name, ", ".join(duplicates))
                else:
                    error_str = "{0} contains duplicates of the following values: ".format(list_name, ", ".join(duplicates))
                add_error(sign_id, error_str)

        # expect args[0] to be instance of signing
        # the following default signing attributes are checked if signing is TCG
        assert(isinstance(args[0], complex_general_properties))
        general_properties = args[0]
        default_sw_id = general_properties.get_sw_id()
        default_app_id = general_properties.get_app_id()
        default_crash_dump = general_properties.get_crash_dump()
        default_msm_part = general_properties.get_msm_part()
        default_soc_hw_version = general_properties.get_soc_hw_version()
        default_soc_vers = general_properties.get_soc_vers()
        default_mask_soc_hw_version = general_properties.get_mask_soc_hw_version()
        default_in_use_soc_hw_version = general_properties.get_in_use_soc_hw_version()
        default_use_serial_number_in_signing = general_properties.get_use_serial_number_in_signing()
        default_serial_number = general_properties.get_serial_number()
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
        default_client_id = general_properties.get_client_id()
        default_lib_id = general_properties.get_lib_id()

        assert(isinstance(args[1], complex_metadata))
        metadata = args[1]
        chipset = metadata.get_chipset()

        for image in image_list:
            sign_id = image.get_sign_id()
            image_type = image.get_image_type()
            overrides = image.get_general_properties_overrides()

            sw_id = overrides.get_sw_id() if overrides.get_sw_id() is not None else default_sw_id
            app_id = overrides.get_app_id() if overrides.get_app_id() is not None else default_app_id
            crash_dump = overrides.get_crash_dump() if overrides.get_crash_dump() is not None else default_crash_dump
            msm_part = overrides.get_msm_part() if overrides.get_msm_part() is not None else default_msm_part
            soc_hw_version = overrides.get_soc_hw_version() if overrides.get_soc_hw_version() is not None else default_soc_hw_version
            soc_vers = overrides.get_soc_vers() if overrides.get_soc_vers() is not None else default_soc_vers
            mask_soc_hw_version = overrides.get_mask_soc_hw_version() if overrides.get_mask_soc_hw_version() is not None else default_mask_soc_hw_version
            in_use_soc_hw_version = overrides.get_in_use_soc_hw_version() if overrides.get_in_use_soc_hw_version() is not None else default_in_use_soc_hw_version
            use_serial_number_in_signing = overrides.get_use_serial_number_in_signing() if overrides.get_use_serial_number_in_signing() is not None else default_use_serial_number_in_signing
            serial_number = overrides.get_serial_number() if overrides.get_serial_number() is not None else default_serial_number
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
            client_id = overrides.get_client_id() if overrides.get_client_id() is not None else default_client_id
            lib_id = overrides.get_lib_id() if overrides.get_lib_id() is not None else default_lib_id

            # Secboot version checking
            if secboot_version is None or (image_type in SECBOOT_1_IMAGE_TYPES and secboot_version != SECBOOT_VERSION_1_0):
                # general properties update occurs in imageinfo's _sanitize_general_properties (like for all other config value updates)
                secboot_version = SECBOOT_VERSION_1_0
            if num_root_certs > 1 and secboot_version != SECBOOT_VERSION_1_0 and chipset not in (SECBOOT_2_0_CHIPSETS + SECBOOT_3_0_CHIPSETS):
                # Multiple root certs not allowed with secboot 2.0 and greater multi-party signing
                add_error(sign_id, "Multiple root certs are not supported for secboot {0} chipset {1}.".format(str(secboot_version), chipset))

            # Default hash algorithm to sha256
            if hash_algorithm is None:
                hash_algorithm = "sha256"

            # Backward compatibility: dsa_type is optional, so if it does not exist assume RSA
            if dsa_type is None:
                dsa_type = "rsa"

            # Manually check that ecdsa / rsa arguments exist when the corresponding
            if dsa_type == "rsa":
                if exponent is None or key_size is None:
                    add_error(sign_id, "You must specify exponent and key size when using RSA.")
            if dsa_type == "ecdsa":
                general_properties.exponent = None
                general_properties.key_size = None
                if ecdsa_curve is None:
                    add_error(sign_id, "You must specify the ecdsa curve when using ECDSA.")

                # Check that ecdsa value is only used with supported chipset
                if chipset not in ECDSA_CHIPSETS:
                    add_error(sign_id, "ECDSA is not supported for chipset {0}.".format(chipset))
            else:
                # RSAPSS requires SHA256 or SHA384
                hmac = hmac or hmac is None
                if rsa_padding == "pss":
                    if hash_algorithm not in ["sha256", "sha384"]:
                        add_error(sign_id, "RSAPSS requires SHA256 or SHA384 hash. hash_algorithm={0}.".format(hash_algorithm))
                    if hmac:
                        add_error(sign_id, "RSAPSS cannot be used with HMAC. hmac must be set as False. hmac={0}.".format(hmac))
                else:
                    if not hmac:
                        add_error(sign_id, "RSAPKCS should be used with HMAC.")

            if secboot_version == SECBOOT_VERSION_3_0:
                # Secboot 3 only support sha256 and sha384
                if hash_algorithm not in ["sha256", "sha384"]:
                    add_error(sign_id, "Secboot {0} requires SHA256 or SHA384 hash.".format(secboot_version))
                # Secboot 3 HMAC has restriction due to soc_hw_version and other values being removed from MBN header
                if hmac:
                    if mask_soc_hw_version is not None:
                        add_error(sign_id, "Secboot {0} requires that mask_soc_hw_version not be set when HMAC is set.".format(secboot_version))
                    if in_use_soc_hw_version == 1:
                        add_error(sign_id, "Secboot {0} requires in_use_soc_hw_version to be set to 0 when HMAC is set.".format(secboot_version))
                    if use_serial_number_in_signing == 1:
                        if serial_number is None or not multi_serial_numbers:
                            add_error(sign_id, "Secboot {0} requires that serial_number and first serial in multi_serial_numbers match when HMAC and use_serial_number_in_signing are set.".format(secboot_version))
                        if serial_number is not None and multi_serial_numbers and serial_number != multi_serial_numbers[0]:
                            add_error(sign_id, "Secboot {0} requires that serial_number and first serial in multi_serial_numbers match when HMAC and use_serial_number_in_signing are set.\n\t"
                                      "serial_number={1} serial={2}".format(secboot_version, serial_number, multi_serial_numbers[0]))

            # TZ apps rule, must have app_id set
            if (int(sw_id, 16) & 0xFFFFFFFF) == 0xC:
                if app_id is None or int(app_id, 16) == 0:
                    add_error(sign_id, "app_id is not set for TZ apps: sw_id={0}.".format(sw_id))
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
                add_error(sign_id, "crash_dump 32bit LSB must be 0 or 1: crash_dump={0}.".format(crash_dump))

            if secboot_version == SECBOOT_VERSION_3_0:
                # msm_part must be provided for secboot 3 images
                if msm_part is None:
                    add_error(sign_id, "MSM_PART must be used to sign Secboot {0} images but MSM_PART is missing.".format(secboot_version))

                # Make sure soc_vers is provided when in_use_soc_hw_version is set to 1
                if in_use_soc_hw_version == 1 and soc_vers is None:
                    add_error(sign_id, "IN_USE_SOC_HW_VERSION specifies SOC_VERS is used but SOC_VERS tag is missing.")
            else:
                # At least one of MSM_PART or SOC_HW_VERSION must be specified
                if msm_part is None and soc_hw_version is None:
                    add_error(sign_id, "MSM_PART and SOC_HW_VERSION are missing from config. At least one must exist.")

                # Only use Family and Device number for soc_hw_version:
                if soc_hw_version is not None and len(soc_hw_version) != 10:
                    add_error(sign_id, "soc_hw_version value {0} is not valid. Value must start with 0x and be 8 bytes.".format(soc_hw_version))

                # Check if any sign_ids in SOC-chipset need to sign with JTAG ID
                if (soc_hw_version is not None) and (chipset in JTAGID_SIGN_IDS) and (sign_id in JTAGID_SIGN_IDS[chipset]) and (msm_part is None):
                    add_error(sign_id, "MSM_PART must be used to sign this image but MSM_PART is missing.")

                # Assure in_use_soc_hw_version exists if both msm_part and soc_hw_version are given
                if soc_hw_version is not None and msm_part is not None and in_use_soc_hw_version is None:
                    add_error(sign_id, "IN_USE_SOC_HW_VERSION must exist to chose between MSM_PART and SOC_HW_VERSION.")

                # in_use_soc_hw_version must exist with soc_hw_version
                if soc_hw_version is not None and in_use_soc_hw_version is None:
                    add_error(sign_id, "IN_USE_SOC_HW_VERSION must be set when using SOC_HW_VERSION.")

                # Make sure in_use_soc_hw_version's specification exists
                if in_use_soc_hw_version == 1 and soc_hw_version is None:
                    add_error(sign_id, "IN_USE_SOC_HW_VERSION specifies SOC_HW_VERSION is used but SOC_HW_VERSION tag is missing.")
                elif in_use_soc_hw_version == 0 and msm_part is None:
                    add_error(sign_id, "IN_USE_SOC_HW_VERSION specifies SOC_HW_VERSION is NOT used but MSM_PART tag is missing.")

                # mask_soc_hw_version may only exist if soc_hw_version exists
                if soc_hw_version is None:
                    if mask_soc_hw_version is not None:
                        add_error(sign_id, "MASK_SOC_HW_VERSION can not exist without the SOC_HW_VERSION tag.")

                if mask_soc_hw_version is not None and in_use_soc_hw_version == 1 and self.mask_warning is True:
                    logger.warning("The mask_soc_hw_version field is set and will mask the soc_hw_version during signing. Please ensure this is the desired result.")
                    self.mask_warning = False

            # Validate soc_vers
            if soc_vers is not None:
                soc_vers_list = soc_vers.split()
                validate_hex_list(sign_id, soc_vers_list, MAX_NUM_SOC_VERS_MAP[secboot_version], "soc_vers", "soc_ver", 6)

            # use_serial_number_in_signing rule: serial number must be set
            if use_serial_number_in_signing == 1:
                if serial_number is None or int(serial_number, 16) == 0:
                    add_error(sign_id, "serial_number must be set when use_serial_number_in_signing is enabled.")
                elif len(serial_number) > 10:
                    add_error("sign_id={0}: serial_number value must be 10 characters or less in length".format(sign_id))

            # Ensure MRC targets are properly configured to avoid OU field conflicts
            misconfigured_mrc_chipsets = get_duplicates(MRC_1_0_CHIPSETS + MRC_2_0_CHIPSETS)
            if misconfigured_mrc_chipsets:
                raise RuntimeError("MRC 1.0 and MRC 2.0 chipsets must be mutually exclusive. The following chipsets are configured incorrectly: {0}.".format(", ".join(misconfigured_mrc_chipsets)))

            # Validate MRC cert values
            if chipset in MRC_2_0_CHIPSETS and chipset in SECBOOT_2_0_DOUBLE_SIGN_CHIPSETS + SECBOOT_3_0_DOUBLE_SIGN_CHIPSETS and max_num_root_certs is None:
                add_error(sign_id, "max_num_root_certs must be provided for chipset \"{0}\".".format(chipset))

            # MRC 1.0 support up to 4 roots, MRC 2.0 supports 1 or 4 roots
            if chipset in MRC_1_0_CHIPSETS or chipset in MRC_2_0_CHIPSETS:
                if max_num_root_certs is not None and num_root_certs > max_num_root_certs:
                    add_error(sign_id, "num_root_certs must be less than or equal to max_num_root_certs: num_root_certs={0}, max_num_root_certs={1}.".format(num_root_certs, max_num_root_certs))
                if chipset in MRC_1_0_CHIPSETS and num_root_certs not in [1, 2, 3, 4]:
                        add_error(sign_id, "num_root_certs must be in range [1-4].")
                if chipset in MRC_2_0_CHIPSETS and num_root_certs not in [1, 4]:
                    add_error(sign_id, "num_root_certs must be either 1 or 4.")
            # Legacy MRC supports up to 16 roots
            elif num_root_certs < 1 or num_root_certs > 16:
                add_error(sign_id, "num_root_certs must be in range [1-16].")

            if num_root_certs > 1 and mrc_index is None:
                add_error(sign_id, "mrc_index must be provided when num_root_certs is greater than 1.")

            if mrc_index is not None and mrc_index >= num_root_certs:
                add_error(sign_id, "Index out of range: mrc_index={0}, num_root_certs={1}.".format(mrc_index, num_root_certs))

            # Format and validate debug
            if debug is not None:
                if len(debug) > 18:
                    add_error(sign_id, "Debug value must be 18 characters or less in length.")
                elif secboot_version != SECBOOT_VERSION_3_0 and len(debug) < 18:
                    padding_len = 18 - len(debug)
                    debug = debug[:2] + "0"*padding_len + debug[2:]

            # Validate the multi_serial_numbers
            if len(multi_serial_numbers) > 0:
                if secboot_version == SECBOOT_VERSION_2_0:
                    if debug is None:
                        add_error(sign_id, "Debug serials were provides but debug field was not provided.")
                    elif debug[:-8] in multi_serial_numbers:
                        add_error(sign_id, "Duplicate serial value of {0} in debug and multi_serial_numbers.".format(debug[:-8]))
                    elif int(debug[:-8], 16) != 0:
                        multi_serial_numbers.append(debug[:-8])
                validate_hex_list(sign_id, multi_serial_numbers, MAX_NUM_SERIALS_MAP[secboot_version], "multi_serial_numbers", "serial", 10)

            # TCG rules
            if oid_min is not None and oid_max is None:
                add_error(sign_id, "{0} min is set, must also set max.".format(default_oid.name))
            elif oid_max is not None and oid_min is None:
                add_error(sign_id, "{0} max is set, must also set min.".format(default_oid.name))
            elif default_oid is not None:
                # Move the min > max checking to signer. It should be validated after valid 32
                # bit integer is checked. Otherwise, int() conversion will throw an exception

                oid_min_config_str = oid_min
                oid_max_config_str = oid_max
                if oid_min_config_str and oid_max_config_str:
                    oid_min = int(oid_min, 16) if "0x" in oid_min else int(oid_min)
                    oid_max = int(oid_max, 16) if "0x" in oid_max else int(oid_max)

                    if oid_min > oid_max:
                        add_error(sign_id, "{0} min must be less than max, min={1} max={2}.".format(default_oid.name, oid_min_config_str, oid_max_config_str))
                if int(sw_id, 16) != 0:
                    add_error(sign_id, "For {0}, sw_id must be 0, sw_id = {1}.".format(default_oid.name, sw_id))
                if int(msm_part, 16) != 0:
                    add_error(sign_id, "For {0}, msm_part must be 0, msm_part = {1}.".format(default_oid.name, msm_part))
                if int(oem_id, 16) != 0:
                    add_error(sign_id, "For {0}, oem_id must be 0, oem_id = {1}.".format(default_oid.name, oem_id))
                if int(model_id, 16) != 0:
                    add_error(sign_id, "For {0}, model_id must be 0, model_id = {1}.".format(default_oid.name, model_id))
                if int(debug, 16) != 2:
                    add_error(sign_id, "For {0}, debug must be 2, debug = {1}.".format(default_oid.name, debug))

            if client_id is not None and int(client_id, 16) == 0:
                    add_error(sign_id, "client_id must be a non-zero value, client_id={0}.".format(client_id))
            if lib_id is not None and int(lib_id, 16) == 0:
                add_error(sign_id, "lib_id must be a non-zero value, lib_id={0}.".format(lib_id))

        return retval[0], "".join(remove_duplicates(errors))


