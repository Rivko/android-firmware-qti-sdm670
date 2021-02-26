# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Feb 8, 2014

@author: hraghav
'''

import os

from cfgparser import ConfigParser
from cfgparser import defines as cfgdef
import defines
from sectools.common.utils import c_path
from sectools.common.utils.c_base import ValPolicy


POLICY_OEM_ID_0 = ValPolicy(ValPolicy.WARN)


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
    def debug_file_signer_qc_signature(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_QC_SIGNATURE) if self._debug_enable else None

    @property
    def debug_file_signer_qc_cert_chain(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_QC_CERT_CHAIN) if self._debug_enable else None

class ImageInfo(object):

    def __init__(self, image_path, sign_id, img_config_block, img_config_parser, parsegen_config):
        import cfgparser.auto_gen_obj_config as agoc
        import parsegen.config.auto_gen_obj_config as pgagoc
        from parsegen.config.parser import ParsegenCfgParser

        assert isinstance(image_path, str)
        assert isinstance(sign_id, str)
        assert isinstance(img_config_parser, ConfigParser)
        assert isinstance(parsegen_config, ParsegenCfgParser)

        # Public attributes
        self.config_parser = img_config_parser
        self.parsegen_config_parser = parsegen_config

        self.config = self.config_parser.config_data
        self.parsegen_config = self.parsegen_config_parser.config_data

        self.src_image = ImagePath(image_path)
        self.dest_image = DestImagePath()
        self.image_under_operation = self.src_image.image_path
        self.name = self.src_image.image_name
        self.sign_id = sign_id
        self.status = ImageStatus()
        self.chipset = self.config_parser.chipset
        self.encdec = None
        self.status.overall.state = StatusInfo.ERROR
        self.validation_root_cert_hash = None
        self.authority = None

        # Properties obtained from the config file
        self.pil_split = img_config_block.pil_split
        self.post_process_commands = img_config_block.post_process_commands
        self.general_properties_overrides = img_config_block.general_properties_overrides

        # Properties to be looked up in the config file
        self.image_type = pgagoc.Cfg_Image_Type()
        self.general_properties = agoc.Cfg_General_Properties()
        self.signing_attributes = self.general_properties

        # Populate the properties from the config file
        self._populate_image_type(img_config_block.image_type)
        self._populate_general_properties()

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

    def _validate_properties(self):
        if not POLICY_OEM_ID_0.is_ignore():
            if int(self.signing_attributes.oem_id, 16) == 0:
                POLICY_OEM_ID_0.run('OEM ID is set to 0 for sign_id "' + str(self.sign_id) + '"')

        # If all authority permissions are disabled, throw error #
        if not self.signing_attributes.qc_sign and not self.signing_attributes.oem_sign:
            raise RuntimeError('Cannot perform image operations. No permissions enabled.')

        # Secboot v1 requires oem permissions only
        if self.signing_attributes.secboot_version == defines.SECBOOT_VERSION_1_0:
            if not self.signing_attributes.oem_sign:
                # oem_sign cannot be false for secboot v1 #
                raise RuntimeError('OEM operations cannot be disabled for a secboot version ' + str(defines.SECBOOT_VERSION_1_0) + ' image.')
            if self.signing_attributes.qc_sign:
                # qc_sign cannot be true for secboot v1 #
                raise RuntimeError('Cannot perform qc_exclusive operations on a secboot version ' + str(defines.SECBOOT_VERSION_1_0) + ' image.')
