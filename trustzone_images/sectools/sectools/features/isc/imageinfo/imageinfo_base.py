# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.utils import c_path
from sectools.features.isc import defines
from .imageinfo_utils import ImagePath
from .imageinfo_utils import StatusInfo
from ..cfgparser import ConfigParser
from ..parsegen.config.parser import ParsegenCfgParser


def debug_only(func):
    def decorated(self):
        if self._debug_enable:
            return func(self)
    return decorated


class DestImagePathBase(ImagePath):

    def __init__(self, image_path=''):
        super(DestImagePathBase, self).__init__(image_path=image_path)
        self._debug_enable = False
        self._mid = ''

    @property
    def debug_enable(self):
        return self._debug_enable

    @debug_enable.setter
    def debug_enable(self, value):
        self._debug_enable = value

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
    @debug_only
    def debug_dir(self):
        return c_path.join(self.image_dir, defines.DEST_DEBUG_DIR)

    @property
    @debug_only
    def debug_dir_parsegen(self):
        return c_path.join(self.debug_dir, self._mid, defines.DEST_DEBUG_DIR_PARSEGEN)

    @property
    @debug_only
    def debug_dir_signer(self):
        return c_path.join(self.debug_dir, self._mid, defines.DEST_DEBUG_DIR_SIGNER)

    @property
    @debug_only
    def debug_file_parsegen_unsigned(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_UNSIGNED +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name)

    @property
    @debug_only
    def debug_file_parsegen_tosign(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_TOSIGN +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name)

    @property
    @debug_only
    def debug_file_parsegen_cert_chain(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_CERT_CHAIN +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name)

    @property
    @debug_only
    def debug_file_parsegen_signature(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_SIGNATURE +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name)

    @property
    @debug_only
    def debug_file_parsegen_integrity_check(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_INTEGRITY_CHECK +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name)

    @property
    @debug_only
    def debug_file_parsegen_signed(self):
        file_name = (self.image_name_base +
                     defines.DEST_DEBUG_FILE_PARSEGEN_SIGNED +
                     self.image_name_ext)
        return c_path.join(self.debug_dir_parsegen, file_name)

    @property
    @debug_only
    def debug_file_signer_root_cert(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_ROOT_CERT)

    @property
    @debug_only
    def debug_file_signer_root_key(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_ROOT_KEY)

    @property
    @debug_only
    def debug_file_signer_attestation_ca_cert(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_ATTESTATION_CA_CERT)

    @property
    @debug_only
    def debug_file_signer_attestation_ca_key(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_ATTESTATION_CA_KEY)

    @property
    @debug_only
    def debug_file_signer_attestation_cert(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_ATTESTATION_CERT)

    @property
    @debug_only
    def debug_file_signer_attestation_key(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_ATTESTATION_KEY)

    @property
    @debug_only
    def debug_file_signer_signature(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_SIGNATURE)

    @property
    @debug_only
    def debug_file_signer_cert_chain(self):
        return c_path.join(self.debug_dir_signer, defines.DEST_DEBUG_FILE_SIGNER_CERT_CHAIN)


class ImageStatusBase(object):

    def __init__(self):
        self.parsegen = StatusInfo()
        self.integrity_check = StatusInfo()
        self.sign = StatusInfo()
        self.postprocess = StatusInfo()
        self.validate_parsegen = StatusInfo()
        self.validate_integrity_check = StatusInfo()
        self.validate_sign = StatusInfo()
        self.overall = StatusInfo()


class SignerConfig(object):

    def __init__(self, imageinfo):
        self.general_properties = imageinfo.general_properties
        self.data_provisioning = imageinfo.config.data_provisioning
        self.metadata = imageinfo.config.metadata


class ImageInfoBase(object):

    image_status_class = ImageStatusBase

    def __init__(self, image_path, sign_id, img_config_block, img_config_parser,
                 parsegen_config, authority=None,
                 agoc=None, pgagoc=None, cfgdef=None, run_before_sanitize=None, **kwargs):

        assert isinstance(sign_id, str)
        assert isinstance(img_config_parser, ConfigParser)
        assert isinstance(parsegen_config, ParsegenCfgParser)

        # Public attributes
        self.config_parser = img_config_parser
        self.parsegen_config_parser = parsegen_config

        self.config = self.config_parser.config_data
        self.parsegen_config = self.parsegen_config_parser.config_data

        self.src_image = ImagePath(image_path) if image_path is not None else ImagePath()
        self.name = self.src_image.image_name
        self.sign_id = sign_id
        self.status = self.image_status_class()
        self.chipset = self.config_parser.chipset
        self.status.overall.state = StatusInfo.ERROR
        self.authority = authority

        # Properties to be looked up in the config file
        self.image_type = pgagoc.Cfg_Image_Type()
        self.general_properties = agoc.Cfg_General_Properties()
        self.signing_attributes = self.general_properties

        # Properties obtained from the config file
        self.general_properties_overrides = img_config_block.general_properties_overrides

        self.cfgdef = cfgdef

        # Populate the properties from the config file
        self._populate_image_type(img_config_block.image_type)
        self._populate_general_properties()

        # Hash segment address, will be obtained from Parsegen
        self.hash_seg_address = None

        if run_before_sanitize:
            run_before_sanitize()

        self._sanitize_general_properties()

        # Construct image-specific config object
        self.signer_config = SignerConfig(self)

        # If a cert_config is specified for a certain image, use that instead
        if img_config_block.cert_config is None:
            # If it is None, just use the selected_cert_config from general properties
            self.cert_config = self.general_properties.selected_cert_config
        else:
            self.cert_config = img_config_block.cert_config

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
        self.image_type = self.parsegen_config_parser.transfer_from_node_to_obj(
                            image_type,
                            self.image_type,
                            self.cfgdef.CONFIG_STRUCTURE_IMAGE_TYPE,
                            self.cfgdef.CONFIG_KEY_IMAGE_TYPE)

    def _populate_general_properties(self):
        # Transfer the contents of the base into the object
        self.general_properties = self.config_parser.transfer_from_node_to_obj(
                             self.config.general_properties,
                             self.general_properties,
                             self.cfgdef.CONFIG_STRUCTURE_GENERAL_PROPERTIES,
                             self.cfgdef.CONFIG_KEY_GENERAL_PROPERTIES)

        for tag in self.cfgdef.CONFIG_STRUCTURE_GENERAL_PROPERTIES.keys():
            self._override_value(self.general_properties_overrides, self.general_properties, tag)

    def _override_value(self, block_from, block_to, key):
        override = getattr(block_from, key, None)
        if override is not None:
            setattr(block_to, key, override)

    @property
    def signer_config(self):
        return self._signer_config

    @signer_config.setter
    def signer_config(self, value):
        self._signer_config = value

    def _add_error(self, error):
        self.errors.append("\nsign_id={0}: ".format(self.sign_id) + error)

    def _sanitize_general_properties(self):
        raise NotImplementedError

    def _validate_properties(self):
        raise NotImplementedError
