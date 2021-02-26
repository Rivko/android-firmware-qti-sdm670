# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
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
from sectools.common.utils.c_data import get_duplicates
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_rule import CoreRuleBase
from sectools.features.isc.cfgparser.defines import JTAGID_SIGN_IDS, IOT_MRC_CHIPSETS, ECDSA_CHIPSETS
from sectools.features.isc.defines import SECBOOT_VERSION_1_0, SECBOOT_VERSION_2_0

MAX_NUM_SERIALS = 201
MAX_NUM_SOC_VERS = 10

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
                    error_str += '\n\n <%s>%s' % (name, error)

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
        '''
        self.debug = debug
        '''

        # debug rule
        # self.debug = default_attr.get_debug()
        # if (self.debug is None) or (not int(self.debug, 16)):
        #    retval = False
        #    error_str += '\n debug is not set: %s' %self.debug

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

        # check dependency rule for num_root_certs
        # need to make sure the multirootcert's index doesn't go beyond avail num_root_certs
        num_root_certs = general_properties.get_num_root_certs()

        if num_root_certs == 0:
            retval = False
            error_str += '\n num_root_certs must not be 0'

        if num_root_certs > 16:
            retval = False
            error_str += '\n num_root_certs must not be greater than 16'

        if not (general_properties.mrc_index < num_root_certs):
            retval = False
            error_str += '\n Index out of range: mrc_index=%s, num_root_certs=%s' % (general_properties.mrc_index, num_root_certs)

        return retval, error_str


class _ImageList(object):
    """
    Defines the rules for image parameters to be signed
    """

    def __init__(self):
        self.mask_warning = True
        self.version_warning = True

    @staticmethod
    def validate_hex_list(hex_list, max_num_allowed, list_name, item_name, length_of_item, retval, error_str):
        if len(hex_list) > max_num_allowed:
            retval = False
            error_str += '\n %d provided %ss exceeds allowed maximum of %d' % (len(hex_list), item_name, max_num_allowed)

        for val in hex_list:
            try:
                if val[: 2] != '0x' or len(val) != length_of_item or int(val, 16) == 0:
                    raise ValueError('malformed')
                int(val, 16)
            except ValueError:
                retval = False
                error_str += '\n Provided %s value %s is not a valid %s' % (list_name, val, item_name)

        duplicates = get_duplicates(hex_list)
        if len(duplicates) > 0:
            retval = False
            if len(duplicates) == 1:
                error_str += '\n %s contains a duplicate of the following value: ' % list_name
            else:
                error_str += '\n %s contains duplicates of the following values: ' % list_name
            error_str += ', '.join(duplicates)

        return retval, error_str

    def validate(self, images, *args):
        assert(isinstance(images, complex_images_list))

        image_list = images.get_image()
        retval = True
        error_str = ''

        # expect args[0] to be instance of signing
        # the following default signing attributes are checked if signing is TCG
        assert(isinstance(args[0], complex_general_properties))
        general_properties = args[0]
        default_sw_id = general_properties.get_sw_id()
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
        default_oem_id = general_properties.get_oem_id()
        default_model_id = general_properties.get_model_id()
        default_debug = general_properties.get_debug()
        default_debug_serials = general_properties.get_debug_serials().get_serial() if general_properties.get_debug_serials() is not None else []
        default_oid = general_properties.get_object_id()
        default_oid_min = default_oid.min if default_oid is not None else None
        default_oid_max = default_oid.max if default_oid is not None else None
        default_hash_algorithm = general_properties.get_hash_algorithm()
        default_oem_id_independent = general_properties.get_oem_id_independent()
        default_hmac = general_properties.get_hmac()
        default_rsa_padding = general_properties.get_rsa_padding()
        default_num_root_certs = general_properties.get_num_root_certs()
        default_secboot_version = general_properties.get_secboot_version()
        default_dsa_type = general_properties.get_dsa_type()
        default_ecdsa_curve = general_properties.get_ecdsa_curve()
        default_key_size = general_properties.get_key_size()
        default_exponent = general_properties.get_exponent()

        assert(isinstance(args[1], complex_metadata))
        metadata = args[1]
        chipset = metadata.get_chipset()

        for image in image_list:
            sign_id = image.get_sign_id()
            overrides = image.get_general_properties_overrides()

            sw_id = overrides.get_sw_id() if overrides.get_sw_id() is not None else default_sw_id
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
            oem_id = overrides.get_oem_id() if overrides.get_oem_id() is not None else default_oem_id
            model_id = overrides.get_model_id() if overrides.get_model_id() is not None else default_model_id
            debug = overrides.get_debug() if overrides.get_debug() is not None else default_debug
            debug_serials = list(overrides.get_debug_serials().get_serial() if overrides.get_debug_serials() is not None else default_debug_serials)
            override_oid = overrides.get_object_id() if overrides.get_object_id() is not None else default_oid
            oid_min = override_oid.min if override_oid is not None else default_oid_min
            oid_max = override_oid.max if override_oid is not None else default_oid_max
            oem_id_independent = overrides.get_oem_id_independent() if overrides.get_oem_id_independent() is not None else default_oem_id_independent
            hmac = overrides.get_hmac() if overrides.get_hmac() is not None else default_hmac
            rsa_padding = overrides.get_rsa_padding() if overrides.get_rsa_padding() is not None else default_rsa_padding
            hash_algorithm = overrides.get_hash_algorithm() if overrides.get_hash_algorithm() is not None else default_hash_algorithm
            num_root_certs = overrides.get_num_root_certs() if overrides.get_num_root_certs() is not None else default_num_root_certs
            secboot_version = overrides.get_secboot_version() if overrides.get_secboot_version() is not None else default_secboot_version
            dsa_type = overrides.get_dsa_type() if overrides.get_dsa_type() is not None else default_dsa_type
            ecdsa_curve = overrides.get_ecdsa_curve() if overrides.get_ecdsa_curve() is not None else default_ecdsa_curve
            key_size = overrides.get_key_size() if overrides.get_key_size() is not None else default_key_size
            exponent = overrides.get_exponent() if overrides.get_exponent() is not None else default_exponent

            # Secboot version checking #
            if secboot_version == SECBOOT_VERSION_2_0:
                if num_root_certs > 1:
                    # Multiple root certs not allowed with multi-party signing #
                    raise RuntimeError('Multiple root certs are not allowed to be used with secboot version ' + SECBOOT_VERSION_2_0 + ' operations.')
            else:
                if secboot_version is None:
                    image.general_properties_overrides.secboot_version = SECBOOT_VERSION_1_0

            #backward compatibility: dsa_type is optional, so if it does not exist assume RSA
            if dsa_type is None:
                dsa_type = 'rsa'

            #manually check that ecdsa / rsa arguments exist when the corresponding
            if dsa_type == 'rsa':
                if exponent is None or key_size is None:
                    retval = False
                    error_str += '\nYou must specify exponent and key size when using RSA'
            if dsa_type == 'ecdsa':
                general_properties.exponent = None
                general_properties.key_size = None
                if ecdsa_curve is None:
                    retval = False
                    error_str += '\nYou must specify the ecdsa curve when using ECDSA'

                # Check that ecdsa value is only used with supported chipset
                if chipset not in ECDSA_CHIPSETS:
                    retval = False
                    error_str += '\nECDSA is not supported for chipset {0}'.format(chipset)
            else:
                # RSAPSS requires SHA256 or SHA384
                hmac = hmac or hmac is None
                if rsa_padding == 'pss':
                    if hash_algorithm is not None and hash_algorithm not in ['sha256', 'sha384']:
                        retval = False
                        error_str += '\n RSAPSS requires SHA256 or SHA384 hash. sign_id=%s, hash_algorithm=%s' % (sign_id, hash_algorithm)
                    if hmac:
                        retval = False
                        error_str += '\n RSAPSS cannot be used with HMAC. hmac must be set as False. sign_id=%s, hmac=%s' % (sign_id, hmac)
                else:
                    if not hmac:
                        retval = False
                        error_str += '\n RSAPKCS should be used with HMAC. sign_id=%s' % sign_id

            # TZ apps rule, must have app_id set
            if (int(sw_id, 16) & 0xFFFFFFFF) == 0xC:
                if app_id is None or int(app_id, 16) == 0:
                    retval = False
                    error_str += '\n app_id is not set for TZ apps: sign_id=%s, sw_id=%s' % (sign_id, sw_id)
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
                retval = False
                error_str += '\n crash_dump 32bit LSB must be 0 or 1: sign_id=%s, crash_dump=%s' % (sign_id, crash_dump)

            # rot_en rule, LSB 32bits must not be greater than 1
            if rot_en is not None and (int(rot_en, 16) & 0xFFFFFFFF) > 1:
                retval = False
                error_str += '\n rot_en 32bit LSB must be 0 or 1: sign_id=%s, rot_en=%s' % (sign_id, rot_en)

            # At least one of MSM_PART or SOC_HW_VERSION must be specified
            if msm_part is None and soc_hw_version is None:
                retval = False
                error_str += "\nsign_id=%s: MSM_PART and SOC_HW_VERSION are missing from config. At least one must exist" % (sign_id)

            # Only use Family and Device number for soc_hw_version:
            if soc_hw_version is not None:
                if len(soc_hw_version) == 10:
                    image.general_properties_overrides.soc_hw_version = soc_hw_version[:6] + '0000'
                else:
                    retval = False
                    error_str += '\n soc_hw_version value %s is not valid. Value must start with 0x and be 8 bytes: sign_id=%s' % (soc_hw_version, sign_id)

            # Check if any sign_ids in SOC-chipset need to sign with JTAG ID
            if soc_hw_version is not None and (chipset in JTAGID_SIGN_IDS):
                if sign_id in JTAGID_SIGN_IDS[chipset]:

                    # Use JTAG instead of SOC_HW_VERSION #
                    if msm_part is None:
                        retval = False
                        error_str += "\nsign_id=%s: MSM_PART must be used to sign this image but MSM_PART is missing" % (sign_id)

                    image.general_properties_overrides.in_use_soc_hw_version = 0

            # Assure in_use_soc_hw_version exists if both msm_part and soc_hw_version are given
            if soc_hw_version is not None and msm_part is not None:
                if in_use_soc_hw_version is None:
                    retval = False
                    error_str += "\nsign_id=%s: IN_USE_SOC_HW_VERSION must exist to chose between MSM_PART and SOC_HW_VERSION" % (sign_id)

            # in_use_soc_hw_version must exist with soc_hw_version
            if soc_hw_version is not None and in_use_soc_hw_version is None:
                retval = False
                error_str += "\nsign_id=%s: IN_USE_SOC_HW_VERSION must be set when using SOC_HW_VERSION" % (sign_id)

            # Make sure in_use_soc_hw_version's specification exists
            if in_use_soc_hw_version == 1 and soc_hw_version is None:
                retval = False
                error_str += "\nsign_id=%s: IN_USE_SOC_HW_VERSION specifies SOC_HW_VERSION is used but SOC_HW_VERSION tag is missing" % (sign_id)
            elif in_use_soc_hw_version == 0 and msm_part is None:
                retval = False
                error_str += "\nsign_id=%s: IN_USE_SOC_HW_VERSION specifies SOC_HW_VERSION is NOT used but MSM_PART tag is missing" % (sign_id)

            # Validate soc_vers
            if soc_vers is not None and in_use_soc_hw_version == 1:
                soc_vers_list = soc_vers.split()
                retval, error_str = _ImageList.validate_hex_list(soc_vers_list, MAX_NUM_SOC_VERS, "soc_vers", "soc_ver", 6, retval, error_str)
                image.general_properties_overrides.soc_vers = soc_vers_list
            else:
                image.general_properties_overrides.soc_vers = []

            # mask_soc_hw_version may only exist if soc_hw_version exists
            if soc_hw_version is None:
                if mask_soc_hw_version is not None:
                    retval = False
                    error_str += "\nsign_id=%s: MASK_SOC_HW_VERSION can not exist without the SOC_HW_VERSION tag" % (sign_id)

            if mask_soc_hw_version is not None and in_use_soc_hw_version == 1 and self.mask_warning is True:
                logger.warning("The mask_soc_hw_version field is set and will mask the soc_hw_version during signing. Please ensure this is the desired result.")
                self.mask_warning = False

            # use_serial_number_in_signing rule: serial number must be set
            if use_serial_number_in_signing == 1:
                if serial_number is None or int(serial_number, 16) == 0:
                    retval = False
                    error_str += '\nsign_id=%s: serial_number must be set when use_serial_number_in_signing is enabled' % (sign_id)

            # IoT MRC use case
            if revocation_enablement is not None:
                if chipset not in IOT_MRC_CHIPSETS:
                    raise RuntimeError("revocation_enablement is only allowed to be set in: " + ', '.join(IOT_MRC_CHIPSETS))

                # if not MRC use case, to pass validation, enforce it to be zero as attest cert will not have it
                if num_root_certs == 1:
                    image.general_properties_overrides.revocation_enablement = '0x0000000000000000'
                else:
                    if int(revocation_enablement, 16) & 0xFFFFFFFF not in [0, 1, 2]:
                        retval = False
                        error_str += '\nsign_id=%s: revocation_enablement must ends with 0, 1 or 2. revocation_enablement = %s' % (sign_id, revocation_enablement)

                    # If last 8 bytes = 0 (disabled), then override revocation field to None (since OU field wont be present)
                    elif int(revocation_enablement, 16) & 0xFFFFFFFF == 0:
                        image.general_properties_overrides.revocation_enablement = None

                    # if last 8 bytes = 1, for enabling revocation without binding it to the serial number, use as is
                    # if last 8 bytes = 2, for enabling revocation with binding it to the serial number, check upper 8 bytes
                    elif int(revocation_enablement, 16) & 0xFFFFFFFF00000000 == 0 and int(revocation_enablement, 16) & 0xFFFFFFFF == 2:
                        retval = False
                        error_str += '\nsign_id=%s: to enable revocation with binding to serial number, upper 8 bytes must contain serial number. revocation_enablement = %s' \
                                        % (sign_id, revocation_enablement)

            if activation_enablement is not None:
                if chipset not in IOT_MRC_CHIPSETS:
                    raise RuntimeError("activation_enablement is only allowed to be set in: " + ', '.join(IOT_MRC_CHIPSETS))

                # if not MRC use case, to pass validation, enforce it to be zero as attest cert will not have it
                if num_root_certs == 1:
                    image.general_properties_overrides.activation_enablement = '0x0000000000000000'
                else:
                    if int(activation_enablement, 16) & 0xFFFFFFFF not in [0, 1, 2]:
                        retval = False
                        error_str += '\nsign_id=%s: activation_enablement must ends with 0, 1 or 2. activation_enablement = %s' % (sign_id, activation_enablement)

                    # If last 8 bytes = 0 (disabled), then override activation field to None (since OU field wont be present)
                    elif int(activation_enablement, 16) & 0xFFFFFFFF == 0:
                        image.general_properties_overrides.activation_enablement = None

                    # if last 8 bytes = 1, for enabling activation without binding it to the serial number, use as is
                    # if last 8 bytes = 2, for enabling activation with binding it to the serial number, check upper 8 bytes
                    elif int(activation_enablement, 16) & 0xFFFFFFFF00000000 == 0 and int(activation_enablement, 16) & 0xFFFFFFFF == 2:
                        retval = False
                        error_str += '\nsign_id=%s: to enable activation with binding to serial number, upper 8 bytes must contain serial number. activation_enablement = %s' \
                                        % (sign_id, activation_enablement)

            # oem independent rule: if oem_id_independent = 1 make oem_id=1 regardless
            if oem_id_independent == 1:
                image.general_properties_overrides.oem_id = '0x0001'
                image.general_properties_overrides.model_id = '0x0000'

            # Format and validate debug
            if debug is not None:
                if len(debug) > 18:
                    retval = False
                    error_str += '\n Debug value must be 18 characters or less in length'
                elif len(debug) < 18:
                    padding_len = 18 - len(debug)
                    debug = debug[:2] + "0"*padding_len + debug[2:]
                    image.general_properties_overrides.debug = debug

            # Validate the debug_serials
            if len(debug_serials) > 0:
                if debug is None:
                    retval = False
                    error_str += '\n Debug serials were provides but debug field was not provided'
                elif debug[:-8] in debug_serials:
                    retval = False
                    error_str += '\n Duplicate serial value of %s in debug and debug_serials' % debug[:-8]
                elif int(debug[:-8], 16) == 0:
                    retval = False
                    error_str += '\n Debug value %s is invalid when providing debug serials. The top half of debug cannot equal zero' % debug
                else:
                    debug_serials.append(debug[:-8])

                retval, error_str = _ImageList.validate_hex_list(debug_serials, MAX_NUM_SERIALS, "debug_serials", "serial", 10, retval, error_str)

            # TCG rules
            if oid_min is not None and oid_max is None:
                retval = False
                error_str += '\n In sign_id = %s, %s min is set, must also set max' % (sign_id, default_oid.name)
            elif oid_max is not None and oid_min is None:
                retval = False
                error_str += '\n In sign_id = %s, %s max is set, must also set min' % (sign_id, default_oid.name)
            elif default_oid is not None:
                # Move the min > max checking to signer. It should be validated after valid 32
                # bit integer is checked. Otherwise, int() conversion will throw an exception

                oid_min_config_str = oid_min
                oid_max_config_str = oid_max
                if oid_min_config_str and oid_max_config_str:
                    oid_min = int(oid_min, 16) if '0x' in oid_min else int(oid_min)
                    oid_max = int(oid_max, 16) if '0x' in oid_max else int(oid_max)

                    if oid_min > oid_max:
                        retval = False
                        error_str += '\n For sign_id = %s, %s min must be less than max, min=%s max=%s' \
                                        % (sign_id, default_oid.name, oid_min_config_str, oid_max_config_str)
                if int(sw_id, 16) != 0:
                    retval = False
                    error_str += '\n For %s sign_id = %s, sw_id must be 0, sw_id = %s' \
                                    % (default_oid.name, sign_id, sw_id)
                if int(msm_part, 16) != 0:
                    retval = False
                    error_str += '\n For %s sign_id = %s, msm_part must be 0, msm_part = %s' \
                                    % (default_oid.name, sign_id, msm_part)
                if int(oem_id, 16) != 0:
                    retval = False
                    error_str += '\n For %s sign_id = %s, oem_id must be 0, oem_id = %s' \
                                    % (default_oid.name, sign_id, oem_id)
                if int(model_id, 16) != 0:
                    retval = False
                    error_str += '\n For %s sign_id = %s, model_id must be 0, model_id = %s' \
                                    % (default_oid.name, sign_id, model_id)
                if int(debug, 16) != 2:
                    retval = False
                    error_str += '\n For %s sign_id = %s, debug must be 2, debug = %s' \
                                    % (default_oid.name, sign_id, debug)

        return retval, error_str


