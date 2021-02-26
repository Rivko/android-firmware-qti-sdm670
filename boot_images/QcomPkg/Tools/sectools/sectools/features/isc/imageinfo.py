# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Feb 8, 2014

@author: hraghav
'''

import os

import defines
from cfgparser import ConfigParser
from cfgparser import defines as cfgdef
from sectools.common.utils import c_path
from sectools.common.utils.c_base import ValPolicy
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import multi_image_string
from sectools.features.isc import defines
from sectools.features.isc.cfgparser.defines import JTAGID_SIGN_IDS, MRC_1_0_CHIPSETS, MRC_2_0_CHIPSETS, SECBOOT_2_0_DOUBLE_SIGN_CHIPSETS, SECBOOT_3_0_DOUBLE_SIGN_CHIPSETS, SECBOOT_1_IMAGE_TYPES
from sectools.features.isc.defines import SECBOOT_VERSION_1_0, SECBOOT_VERSION_3_0


POLICY_OEM_ID_0 = ValPolicy(ValPolicy.WARN)
MULTI_IMAGE_FILENAME = multi_image_string() + " image"
OEM_MULTI_IMAGE_SIGN_ID = "multi_image"
QTI_MULTI_IMAGE_SIGN_ID = "multi_image_qti"
MULTI_IMAGE_SIGN_ID = {
    defines.AUTHORITY_OEM: OEM_MULTI_IMAGE_SIGN_ID,
    defines.AUTHORITY_QTI: QTI_MULTI_IMAGE_SIGN_ID,
}


class ImagePath(object):

    def __init__(self, image_path=''):
        self._image_dir_base = os.path.dirname(image_path)
        self._image_dir_ext = ''
        self._image_name = os.path.basename(image_path)

    @property
    def image_dir_base(self):
        return self._image_dir_base

    @image_dir_base.setter
    def image_dir_base(self, image_dir_base):
        image_dir_base = c_path.normalize(image_dir_base)
        self._image_dir_base = image_dir_base

    @property
    def image_dir_ext(self):
        return self._image_dir_ext

    @image_dir_ext.setter
    def image_dir_ext(self, image_dir_ext):
        self._image_dir_ext = image_dir_ext

    @property
    def image_dir(self):
        return c_path.join(self.image_dir_base, self.image_dir_ext)

    @property
    def image_name(self):
        return self._image_name

    @image_name.setter
    def image_name(self, image_name):
        self._image_name = image_name

    @property
    def image_path(self):
        return c_path.join(self.image_dir, self.image_name)

    @property
    def image_name_base(self):
        return os.path.splitext(self.image_name)[0]

    @property
    def image_name_ext(self):
        return os.path.splitext(self.image_name)[1]


class StatusInfo(object):

    NOT_EXECUTED = 'NA'
    SUCCESS = 'T'
    ERROR = 'F'

    def __init__(self):
        self.state = self.NOT_EXECUTED
        self.error = ''


class ImageStatus(object):

    def __init__(self):
        self.parsegen = StatusInfo()
        self.integrity_check = StatusInfo()
        self.sign = StatusInfo()
        self.encrypt = StatusInfo()
        self.postprocess = StatusInfo()
        self.validate_parsegen = StatusInfo()
        self.validate_integrity_check = StatusInfo()
        self.validate_sign = StatusInfo()
        self.validate_encrypt = StatusInfo()
        self.overall = StatusInfo()


class DestImagePath(ImagePath):

    def __init__(self, image_path=''):
        ImagePath.__init__(self, image_path=image_path)
        self._debug_enable = False
        self._mid = ''

    @property
    def debug_enable(self):
        return self._debug_enable

    @debug_enable.setter
    def debug_enable(self, value):
        self._debug_enable = value

    @property
    def decrypted_file(self):
        file_name = (self.image_name_base +
                     defines.DEST_FILE_DECRYPTED +
                     self.image_name_ext)
        return c_path.join(self.image_dir, file_name)

    @property
    def to_sign(self):
        file_name = (self.image_name_base +
                     defines.DEST_FILE_TO_SIGN +
                     self.image_name_ext)
        return c_path.join(self.image_dir, file_name)

    @property
    def cert_folder(self):
        dir_name = defines.DEST_DIR_CERT_FOLDER
        return c_path.join(self.image_dir, dir_name)

    @property
    def debug_dir(self):
        return c_path.join(self.image_dir, defines.DEST_DEBUG_DIR) if self._debug_enable else None

    @property
    def debug_dir_parsegen(self):
        return c_path.join(self.debug_dir, self._mid, defines.DEST_DEBUG_DIR_PARSEGEN) if self._debug_enable else None

    @property
    def debug_dir_signer(self):
        return c_path.join(self.debug_dir, self._mid, defines.DEST_DEBUG_DIR_SIGNER) if self._debug_enable else None

    @property
    def debug_dir_encdec(self):
        return c_path.join(self.debug_dir, self._mid, defines.DEST_DEBUG_DIR_ENCDEC) if self._debug_enable else None

    @property
    def debug_file_parsegen_unsigned(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_UNSIGNED +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name) if self._debug_enable else None

    @property
    def debug_file_parsegen_tosign(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_TOSIGN +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name) if self._debug_enable else None

    @property
    def debug_file_parsegen_cert_chain(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_CERT_CHAIN +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name) if self._debug_enable else None

    @property
    def debug_file_parsegen_signature(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_SIGNATURE +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name) if self._debug_enable else None

    @property
    def debug_file_parsegen_integrity_check(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_INTEGRITY_CHECK +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name) if self._debug_enable else None

    @property
    def debug_file_parsegen_signed(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_SIGNED +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name) if self._debug_enable else None

    @property
    def debug_file_parsegen_encrypted(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_ENCRYPTED +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name) if self._debug_enable else None

    @property
    def debug_file_signer_root_cert(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_ROOT_CERT) if self._debug_enable else None

    @property
    def debug_file_signer_root_key(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_ROOT_KEY) if self._debug_enable else None

    @property
    def debug_file_signer_attestation_ca_cert(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_ATTESTATION_CA_CERT) if self._debug_enable else None

    @property
    def debug_file_signer_attestation_ca_key(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_ATTESTATION_CA_KEY) if self._debug_enable else None

    @property
    def debug_file_signer_attestation_cert(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_ATTESTATION_CERT) if self._debug_enable else None

    @property
    def debug_file_signer_attestation_key(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_ATTESTATION_KEY) if self._debug_enable else None

    @property
    def debug_file_signer_signature(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_SIGNATURE) if self._debug_enable else None

    @property
    def debug_file_signer_cert_chain(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_CERT_CHAIN) if self._debug_enable else None

    @property
    def debug_file_signer_qti_signature(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_QTI_SIGNATURE) if self._debug_enable else None

    @property
    def debug_file_signer_qti_cert_chain(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_QTI_CERT_CHAIN) if self._debug_enable else None


class ImageInfo(object):

    def __init__(self, image_path, sign_id, img_config_block, img_config_parser, parsegen_config, gen_multi_image=False, authority=None):
        import cfgparser.auto_gen_obj_config as agoc
        import parsegen.config.auto_gen_obj_config as pgagoc
        from parsegen.config.parser import ParsegenCfgParser

        if not gen_multi_image:
            assert isinstance(image_path, str)
        assert isinstance(sign_id, str)
        assert isinstance(img_config_parser, ConfigParser)
        assert isinstance(parsegen_config, ParsegenCfgParser)

        # Public attributes
        self.config_parser = img_config_parser
        self.parsegen_config_parser = parsegen_config

        self.config = self.config_parser.config_data
        self.parsegen_config = self.parsegen_config_parser.config_data

        self.src_image = ImagePath(image_path) if image_path is not None else ImagePath()
        self.dest_image = DestImagePath()
        self.image_under_operation = self.src_image.image_path if self.src_image.image_path else MULTI_IMAGE_FILENAME
        self.name = self.src_image.image_name
        self.sign_id = sign_id
        self.status = ImageStatus()
        self.chipset = self.config_parser.chipset
        self.encdec = None
        self.status.overall.state = StatusInfo.ERROR
        self.validation_root_cert_hash = None
        self.authority = authority
        self.is_multi_image = self.sign_id in MULTI_IMAGE_SIGN_ID.values()

        # Properties obtained from the config file
        self.pil_split = img_config_block.pil_split
        self.post_process_commands = img_config_block.post_process_commands
        self.general_properties_overrides = img_config_block.general_properties_overrides

        # Properties to be looked up in the config file
        self.image_type = pgagoc.Cfg_Image_Type()
        self.general_properties = agoc.Cfg_General_Properties()
        self.general_properties.ignored_general_properties = {}
        self.signing_attributes = self.general_properties

        # Populate the properties from the config file
        self._populate_image_type(img_config_block.image_type)
        self._populate_general_properties()
        self._sanitize_general_properties()

        # If a cert_config is specified for a certain image, use that instead
        if img_config_block.cert_config is not None:
            self.cert_config = img_config_block.cert_config
        # If not, just use the selected_cert_config from general properties
        else:
            self.cert_config = self.general_properties.selected_cert_config

        # Define data_provisioner base path for image
        self.data_prov_basepath = self.config.data_provisioning.base_path

        # Perform any validations
        self._validate_properties()

    #--------------------------------------------------------------------------
    # Helper functions
    #--------------------------------------------------------------------------
    def _populate_image_type(self, image_type_id):
        assert isinstance(image_type_id, str)

        # Check if config has parsegen
        parsegen_override = []
        try:
            parsegen_override = self.config.parsegen.image_types_list.image_type
        except Exception:
            pass

        # Search the id in the config
        for image_type in (parsegen_override + self.parsegen_config.image_types_list.image_type):
            if image_type.id == image_type_id:
                break
        else:
            raise RuntimeError('Config does not contain the image id: ' + image_type_id)

        # Transfer the contents of the config into the object
        from parsegen.config import defines as prsgendef
        self.image_type = self.parsegen_config_parser.transfer_from_node_to_obj(
                            image_type,
                            self.image_type,
                            prsgendef.CONFIG_STRUCTURE_IMAGE_TYPE,
                            prsgendef.CONFIG_KEY_IMAGE_TYPE)

    def _sanitize_general_properties(self):

        def _sanitize_mrc_value(value, num_root_certs, secboot_version):
            sanitized_value = value
            if value is not None and secboot_version != SECBOOT_VERSION_3_0:
                lower_8_bytes = int(value, 16) & 0xFFFFFFFF
                # if not MRC use case, to pass validation, enforce value to be zero because attest cert will not have it
                if num_root_certs == 1:
                    sanitized_value = "0x0000000000000000"
                # If last 8 bytes == 0 (disabled), then override value to None (since OU field wont be present)
                elif lower_8_bytes == 0:
                    sanitized_value = None
            return sanitized_value

        # Sanitize secboot_version
        if self.general_properties.secboot_version is None or (self.image_type.id in SECBOOT_1_IMAGE_TYPES and self.general_properties.secboot_version != defines.SECBOOT_VERSION_1_0):
            self.general_properties.secboot_version = SECBOOT_VERSION_1_0

        # Sanitize sw_id
        if self.general_properties.secboot_version != SECBOOT_VERSION_3_0 and self.general_properties.anti_rollback_version is not None:
            if (int(self.general_properties.sw_id, 16) & 0xFFFFFFFF00000000) == 0:
                hex_sw_id = hex(int(self.general_properties.sw_id, 16) | (int(self.general_properties.anti_rollback_version, 16) << 32))
                self.general_properties.sw_id = hex_sw_id[:-1] if "L" in hex_sw_id else hex_sw_id
                self.general_properties.anti_rollback_version = None

        if len(self.general_properties.sw_id) < 18:
            padding_len = 18 - len(self.general_properties.sw_id)
            self.general_properties.sw_id = self.general_properties.sw_id[:2] + "0" * padding_len + self.general_properties.sw_id[2:]

        # Sanitize soc_vers and handle interdependence with in_use_soc_hw_version
        if self.general_properties.soc_vers is not None:
            if self.general_properties.in_use_soc_hw_version == 1:
                self.general_properties.soc_vers = self.general_properties.soc_vers.split()
            else:
                self.general_properties.soc_vers = None
                self.general_properties.ignored_general_properties["soc_vers"] = True
        else:
            self.general_properties.ignored_general_properties["soc_vers"] = False

        # soc_hw_version only uses Family and Device number
        if self.general_properties.soc_hw_version is not None:
            self.general_properties.soc_hw_version = self.general_properties.soc_hw_version[:6] + "0000"
            # Disable in_use_soc_hw_version if chipset needs to be signed with JTAG ID
            if (self.chipset in JTAGID_SIGN_IDS) and (self.sign_id in JTAGID_SIGN_IDS[self.chipset]):
                self.general_properties.in_use_soc_hw_version = 0

        # Sanitize debug
        if self.general_properties.debug is not None and len(self.general_properties.debug) < 18:
                padding_len = 18 - len(self.general_properties.debug)
                self.general_properties.debug = self.general_properties.debug[:2] + "0"*padding_len + self.general_properties.debug[2:]

        # Sanitize multi_serial_numbers
        if self.general_properties.multi_serial_numbers is not None:
            self.general_properties.multi_serial_numbers.serial = [serial for serial in self.general_properties.multi_serial_numbers.serial if int(serial, 16) != 0]

        # Apply OEM independent rule: if oem_id_independent == 1, make oem_id=1 regardless
        if self.general_properties.oem_id_independent == 1:
            self.general_properties.oem_id = '0x0001'
            self.general_properties.model_id = '0x0000'

        # Sanitize revocation_enablement
        self.general_properties.revocation_enablement = _sanitize_mrc_value(self.general_properties.revocation_enablement, self.general_properties.num_root_certs, self.general_properties.secboot_version)

        # Sanitize activation_enablement
        self.general_properties.activation_enablement = _sanitize_mrc_value(self.general_properties.activation_enablement, self.general_properties.num_root_certs, self.general_properties.secboot_version)

        # Sanitize root_revoke_activate_enable
        self.general_properties.root_revoke_activate_enable = _sanitize_mrc_value(self.general_properties.root_revoke_activate_enable, self.general_properties.num_root_certs, self.general_properties.secboot_version)

        # Sanitize uie_key_switch_enable
        if self.general_properties.uie_key_switch_enable is not None and (int(self.general_properties.uie_key_switch_enable, 16) & 0xFFFFFFFF) == 0:
            # If last 8 bytes == 0 (disabled), then override value to None (since OU field wont be present)
            self.general_properties.uie_key_switch_enable = None

        # Sanitize hash_algorithm
        if self.general_properties.hash_algorithm is None:
            self.general_properties.hash_algorithm = "sha256"

        # Sanitize segment hash_algorithm
        if self.general_properties.segment_hash_algorithm is None:
            self.general_properties.segment_hash_algorithm = "sha256"

    def _populate_general_properties(self):
        # Transfer the contents of the base into the object
        self.general_properties = self.config_parser.transfer_from_node_to_obj(
                             self.config.general_properties,
                             self.general_properties,
                             cfgdef.CONFIG_STRUCTURE_GENERAL_PROPERTIES,
                             cfgdef.CONFIG_KEY_GENERAL_PROPERTIES)

        for tag in cfgdef.CONFIG_STRUCTURE_GENERAL_PROPERTIES.keys():
            self._override_value(self.general_properties_overrides, self.general_properties, tag)

        # Private overrides
        if self.config_parser.spec_overrides is not None:
            for tag, override in self.config_parser.spec_overrides.get_properties(has_value=True).items():
                if tag == 'cass_password':
                    self.config.signing.signer_attributes.cass_signer_attributes.user_identity.token_password = override.value

    def _override_value(self, block_from, block_to, key):
        override = getattr(block_from, key, None)
        if override is not None:
            setattr(block_to, key, override)

    def _add_error(self, error):
        self.errors.append("\nsign_id={0}: ".format(self.sign_id) + error)

    def _validate_serial_bound_value(self,
                                     value,
                                     value_name,
                                     chipset=None,
                                     allowed_chipsets=None,
                                     num_root_certs=None,
                                     ignore_num_root_certs=False,
                                     disable_in_value_binding=False,
                                     additional_serials_are_bindable=False,
                                     allowed_lsb_values=None):
        if value is not None:
            if chipset is not None and allowed_chipsets is not None and chipset not in allowed_chipsets:
                self._add_error("{0} is only allowed to be set for chipsets: {1}.".format(value_name, ", ".join(allowed_chipsets)))
            elif ignore_num_root_certs or num_root_certs > 1:
                upper_8_bytes = int(value, 16) & 0xFFFFFFFF00000000
                lower_8_bytes = int(value, 16) & 0xFFFFFFFF

                if allowed_lsb_values is None:
                    allowed_lsb_values = [0, 1, 2]
                allowed_lsb_hex_values = ", ".join([hex(x) for x in allowed_lsb_values])
                if disable_in_value_binding:
                    if int(value, 16) not in allowed_lsb_values:
                        self._add_error("{0} must be be set to one of the following: {1}. {0} = {2}.".format(value_name, allowed_lsb_hex_values, value))
                    if upper_8_bytes != 0 or lower_8_bytes == allowed_lsb_values[len(allowed_lsb_values) - 1] and not additional_serials_are_bindable:
                        self._add_error("to enable {0} serial number binding, add serial to multi_serial_numbers.".format(value_name))
                else:
                    if lower_8_bytes not in allowed_lsb_values:
                        self._add_error("{0} must end with one of the following: {1}. {0} = {2}.".format(value_name, ", ".join(str(v) for v in allowed_lsb_values), value))
                    # if last 8 bytes == 1, enabling with no serial binding so use as is
                    # if last 8 bytes == 2, enabling with serial binding so check upper 8 bytes are valid serial
                    if upper_8_bytes == 0 and lower_8_bytes == allowed_lsb_values[len(allowed_lsb_values) - 1]:
                        self._add_error("to enable {0} with binding to serial number, upper 8 bytes must contain serial number. {0} = {1}.".format(value_name, value))

    def _validate_properties(self):
        self.errors = []
        sa = self.signing_attributes

        if not POLICY_OEM_ID_0.is_ignore():
            if int(sa.oem_id, 16) == 0:
                POLICY_OEM_ID_0.run('OEM ID is set to 0 for sign_id "' + str(self.sign_id) + '"')

        # Secboot v1 requires oem permissions only
        if sa.secboot_version == SECBOOT_VERSION_1_0:
            if not sa.oem_sign:
                # oem_sign cannot be false for secboot v1
                self._add_error('OEM operations cannot be disabled for a secboot version ' + str(SECBOOT_VERSION_1_0) + ' image.')
            if sa.qti_sign:
                # qti_sign cannot be true for secboot v1
                self._add_error('Cannot perform QTI operations on a secboot version ' + str(SECBOOT_VERSION_1_0) + ' image.')
        # If all authority permissions are disabled, throw error
        elif not sa.qti_sign and not sa.oem_sign:
            self._add_error('Cannot perform image operations because OEM and QTI operations are disabled for image.')

        # Raise warnings for Secboot v2.1 and greater chipsets
        if self.chipset in SECBOOT_2_0_DOUBLE_SIGN_CHIPSETS + SECBOOT_3_0_DOUBLE_SIGN_CHIPSETS:
            if sa.rot_en is not None and int(sa.rot_en, 16) == 1:
                logger.warning("rot_en should not be set to \"{0}\" for chipset \"{1}\".".format(sa.rot_en, self.chipset))
            if sa.hash_algorithm == "sha1":
                logger.warning("hash_algorithm should not be set to \"{0}\" for chipset \"{1}\".".format(sa.hash_algorithm, self.chipset))
            if sa.exponent == 3:
                logger.warning("exponent should not be set to \"{0}\" for chipset \"{1}\".".format(sa.exponent, self.chipset))

            if sa.dsa_type == "ecdsa":
                if sa.rsa_padding is not None:
                    logger.warning("rsa_padding should not be set for chipset \"{0}\" when ECDSA signing.".format(self.chipset))
                    logger.warning("rsa_padding and hmac will be ignored.".format(self.chipset))
                    sa.rsa_padding = None
                    sa.hmac = None
            else:
                if sa.rsa_padding is None or sa.rsa_padding == "pkcs":
                    logger.warning("rsa_padding should not be set to RSAPKCS for chipset \"{0}\".".format(self.chipset))

        # Ignore max_num_root_certs value if target is not MRC v2.0 double sign target
        if not (self.chipset in MRC_2_0_CHIPSETS and sa.qti_sign and sa.oem_sign):
            sa.max_num_root_certs = None

        if self.is_multi_image and sa.multi_image_segment_phys_addr is None:
            self._add_error(multi_image_string() + " operation requires multi_image_segment_phys_addr config value.")

        # Based on secboot_version allowed formats differ for sw_id, anti_rollback_version, rot_en, uie_key_switch_enable, root_revoke_activate_enable, & debug.
        # Validation must happen outside of rule.py because images of different secboot versions can coexist in the same config file.

        # Only Secboot 3 supports anti_rollback_version as its own value
        if sa.secboot_version == SECBOOT_VERSION_3_0:
            if sa.anti_rollback_version is None:
                self._add_error("Secboot {0} requires anti_rollback_version field.".format(sa.secboot_version))
            if (int(sa.sw_id, 16) & 0xFFFFFFFF00000000) != 0:
                self._add_error("Provide anti-rollback version in anti_rollback_version field for secboot {0} chipset.".format(sa.secboot_version))
                self._add_error("sw_id value {0} exceeds maximum allowed length of 10 characters.".format(sa.sw_id))
            if sa.anti_rollback_version is not None and (int(sa.anti_rollback_version, 16) & 0xFFFFFFFF00000000) != 0:
                self._add_error("anti_rollback_version value {0} exceeds maximum allowed length of 10 characters.".format(sa.anti_rollback_version))
            # sha1 is disallowed for Secboot 3
            if sa.hash_algorithm == "sha1":
                self._add_error("sha1 hash_algorithm is not allowed for secboot {0} images.".format(sa.secboot_version))
            if sa.segment_hash_algorithm == "sha1":
                self._add_error("sha1 segment_hash_algorithm is not allowed for secboot {0} images.".format(sa.secboot_version))

        # rot_en rule, LSB 32bits must not be greater than 1
        multi_serial_numbers = sa.multi_serial_numbers.serial if sa.multi_serial_numbers is not None else []
        self._validate_serial_bound_value(sa.rot_en,
                                          "rot_en",
                                          self.chipset,
                                          ignore_num_root_certs=True,
                                          disable_in_value_binding=sa.secboot_version == SECBOOT_VERSION_3_0,
                                          additional_serials_are_bindable=len(multi_serial_numbers) > 0,
                                          allowed_lsb_values=[0, 1])

        # Validate uie_key_switch_enable
        self._validate_serial_bound_value(sa.uie_key_switch_enable,
                                          "uie_key_switch_enable",
                                          self.chipset,
                                          ignore_num_root_certs=True,
                                          disable_in_value_binding=sa.secboot_version == SECBOOT_VERSION_3_0,
                                          additional_serials_are_bindable=len(multi_serial_numbers) > 0)

        # MRC 1.0 use case
        # Validate revocation_enablement
        self._validate_serial_bound_value(sa.revocation_enablement,
                                          "revocation_enablement",
                                          self.chipset,
                                          MRC_1_0_CHIPSETS,
                                          sa.num_root_certs)
        # Validate activation_enablement
        self._validate_serial_bound_value(sa.activation_enablement,
                                          "activation_enablement",
                                          self.chipset,
                                          MRC_1_0_CHIPSETS,
                                          sa.num_root_certs)
        # MRC 2.0 use case
        # Validate root_revoke_activate_enable
        self._validate_serial_bound_value(sa.root_revoke_activate_enable,
                                          "root_revoke_activate_enable",
                                          self.chipset,
                                          MRC_2_0_CHIPSETS,
                                          sa.num_root_certs,
                                          disable_in_value_binding=sa.secboot_version == SECBOOT_VERSION_3_0,
                                          additional_serials_are_bindable=len(multi_serial_numbers) > 0)

        self._validate_serial_bound_value(sa.debug,
                                          "debug",
                                          self.chipset,
                                          ignore_num_root_certs=True,
                                          disable_in_value_binding=sa.secboot_version == SECBOOT_VERSION_3_0,
                                          additional_serials_are_bindable=len(multi_serial_numbers) > 0,
                                          allowed_lsb_values=[0, 2, 3] if sa.secboot_version != SECBOOT_VERSION_3_0 else None)

        if self.errors:
            raise RuntimeError("".join(self.errors))
