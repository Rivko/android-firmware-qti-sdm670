# ===============================================================================
#
#  Copyright (c) 2013-2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import glob
import hashlib
import os
import sys
import traceback

from six import ensure_binary

from sectools.common import crypto
from sectools.common.parsegen.elf.format import PF_OS_SEGMENT_HASH
from sectools.common.parsegen.elf.format import ParseGenElf
from sectools.common.parsegen.elf.format import pack_phdrs
from sectools.common.parsegen.elf.segment.flags import PF_OS_SEGMENT_L4BSP
from sectools.common.parsegen.elf.segment.type import PT_LOAD
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import PROGRESS_CB_PYPASS
from sectools.common.utils.c_misc import ProgressNotifier
from sectools.common.utils.c_misc import is_TA
from sectools.common.utils.c_misc import load_data_from_file
from sectools.common.utils.c_misc import store_data_to_file
from sectools.features.isc.advanced_defines import AUTHORITY_QTI
from sectools.features.isc.advanced_defines import multi_image_string
from sectools.features.isc.defines import AUTHORITY_OEM
from sectools.features.isc.defines import DEFAULT_CONFIG_DIR
from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from sectools.features.isc.encryption_service import get_encdec
from sectools.features.isc.encryption_service.unified.encrypted_key_provider_mgr import \
    encrypted_key_provider_id_supported
from sectools.features.isc.imageinfo import StatusInfo
from sectools.features.isc.imageinfo.secimageinfo import DestImagePath
from sectools.features.isc.imageinfo.secimageinfo import ImageInfo
from sectools.features.isc.imageinfo.serial_binding_features import \
    evaluate_serial_binding_attributes
from sectools.features.isc.imageinfo.serial_binding_features import update_image_serial_bound_notes
from sectools.features.isc.parsegen.get_parser import get_parser
from sectools.features.isc.secimage_base import CustomError
from sectools.features.isc.secimage_base import PROGRESS_TOTAL_STAGES
from sectools.features.isc.secimage_base import SecImageCoreBase
from sectools.features.isc.secpolicy import create_security_policy_list
from sectools.features.isc.signer import get_signer
from sectools.features.isc.signer.remote import RemoteSignerNote
from sectools.features.isc.stager.image_paths_stager import ImagePathsStager
from sectools.features.isc.stager.meta_build_stager import MetaBuildStager
from sectools.features.isc.stager.meta_build_tmp_stager import MetaBuildTmpStager
from .secimage_cfgparser import SecImageConfigParser
from .secimage_cfgparser import secimage_cfgparser
from ..parsegen.config import auto_gen_obj_config as parsegencfg_auto_gen_obj_config
from ..parsegen.config import auto_gen_xml_config as parsegencfg_auto_gen_xml_config
from ..parsegen.config import defines as parsegencfg_defines
from ..parsegen.config.parser import ParsegenCfgParser

PARSEGEN_PATH = c_path.join(
    os.path.realpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")),
    'parsegen', 'config', 'parsegen_config.xml')

NON_ENCRYPT_SEGMENT_OS_TYPE = [PF_OS_SEGMENT_L4BSP]

parsegen_config = ParsegenCfgParser(
     PARSEGEN_PATH, defines=parsegencfg_defines,
     auto_gen_xml_config=parsegencfg_auto_gen_xml_config,
     auto_gen_obj_config=parsegencfg_auto_gen_obj_config)


class SecImageCore(SecImageCoreBase):
    """ Derived SecImageCore class used by secimage feature.

    Provides interface for user to set up parameters (including input files,
    output directory, etc) and perform various operations (signing, encryption).


    User can select the config file using:

    - :meth:`chipset`
    - :meth:`config_path`

    User can specify images for processing using:

    - :meth:`set_image_path`
    - :meth:`set_meta_build_path`

    User can specify output using:

    - :meth:`output_dir`
    - :meth:`mini_build_path`

    User can finally perform the following operations:

    - :meth:`verify_inputs`
    - :meth:`sign`
    - :meth:`encrypt`
    - :meth:`validate`

    User can also perform multiple operations using :meth:`process`

    :param str config_dir: Path to the directory containing the config files
        for the supported chipsets.
    :param bool debug: If all the underlying modules should work in debug mode
        (save debug logs, etc).

    .. data:: debug

        (bool) Is debug mode enabled. The debug logs will be stored in :meth:`output_dir`

    """
    AUTHORITIES_SUPPORTED = [AUTHORITY_OEM, AUTHORITY_QTI]

    dest_image_path_class = DestImagePath

    def __init__(self, config_dir=DEFAULT_CONFIG_DIR, parsegen_config=parsegen_config,
                 cfgparser=None, debug=False, verbatim_config=False,
                 disable_serial_binding=False, platform_binding=None):

        if cfgparser is None:
            cfgparser = secimage_cfgparser(verbatim_config=verbatim_config, platform_binding=platform_binding)

        super(SecImageCore, self).__init__(
            config_dir=config_dir, parsegen_config=parsegen_config,
            cfgparser=cfgparser, debug=debug)

        self.verbatim_config = verbatim_config
        self.disable_serial_binding = disable_serial_binding
        self.platform_binding = platform_binding

    @property
    def multi_image_imageinfo_dict(self):
        """A dictionary that maps :class:`str` chipsets to objects of type :class:`imageinfo.ImageInfo` that correspond to the
        Multi-Image Sign & Integrity image to be operated on for the corresponding chipset.
        """
        if self._stager is not None:
            return self._stager.multi_image_imageinfo_dict
        else:
            raise RuntimeError(multi_image_string() + ' Image Info is not set. '
                               'Set one of image_path or meta_build_path.')

    @property
    def mini_build_path(self):
        """(str) Path to the minimized build directory to use for saving the
        logs, output files.
        """
        if self._stager is not None:
            return self._stager.mini_build_path
        else:
            raise RuntimeError('Image list is not set. '
                               'Set one of image_path or meta_build_path. '
                               'Then retry querying/setting minimized build.')

    @mini_build_path.setter
    def mini_build_path(self, mini_build_path):
        # Log if the mini_build_path is changed
        selected_mini_build_path = self.mini_build_path
        if selected_mini_build_path:
            logger.note('Switching mini_build_path: ' + '\n'
                        '    ' + 'from: ' + selected_mini_build_path + '\n'
                        '    ' + ' to: ' + mini_build_path)

        # Update the mini_build_path
        self._stager.mini_build_path = mini_build_path
        logger.info('Minimized Build is set to: ' + mini_build_path)

        # Clear the output dir setting
        try:
            self._stager.output_dir = ''
        except Exception:
            pass

    @property
    def output_dir(self):
        return super(SecImageCore, self).output_dir

    @output_dir.setter
    def output_dir(self, output_dir):
        # Log if the output_dir is changed
        try:
            selected_output_dir = self.output_dir
            if selected_output_dir:
                logger.note('Switching output_dir: ' + '\n'
                            '    ' + 'from: ' + selected_output_dir + '\n'
                            '    ' + ' to: ' + output_dir)
        except Exception:
            pass

        # Update the output_dir
        self._stager.output_dir = output_dir
        logger.debug('Output dir is set to: ' + output_dir)

        # Clear the minimized build setting
        try:
            self._stager.mini_build_path = ''
        except Exception:
            pass

    # Helper method to override prefix of debug dumped files for images with multiple security policies
    @staticmethod
    def _get_prefix_override(existing_prefix, file_type):
        if existing_prefix is None:
            return file_type + "_wrapped"
        else:
            return existing_prefix + file_type + "_wrapped"

    def validate_authority_settings(self, authority, secboot_version, qti_sign, oem_sign, num_root_certs, encrypt=False):
        if secboot_version != SECBOOT_VERSION_1_0:
            if encrypt and authority != self.highest_authority([oem_sign, qti_sign]):
                # Reserve encryption for the highest active authority #
                raise RuntimeError("Encryption is not allowed for: " + str(authority))
        if num_root_certs != 1 and authority == AUTHORITY_QTI:
            raise RuntimeError("num_root_certs must be set to \"1\" for: " + str(authority))

    def dump_signer_debug_data(self, image, sign_assets, parsegen,
                               more_debug_logs=[]):
        # Backup parsegen authority
        authority = parsegen.authority

        # QTI Signature and Cert Chain
        parsegen.authority = AUTHORITY_QTI
        data_signature_qti = parsegen.data_signature
        cert_chain_qti = parsegen.cert_chain

        # Restore authority
        parsegen.authority = authority

        qti_debug_logs = [
            (data_signature_qti, "debug_file_signer_qti_signature"),
            (cert_chain_qti, "debug_file_signer_qti_cert_chain")]

        super(SecImageCore, self).dump_signer_debug_data(
            image, sign_assets, parsegen, more_debug_logs=qti_debug_logs)

    def set_image_path(self, image_path, sign_id=None, gen_multi_image=False, multi_image_path=None,
                       imageinfo_class=ImageInfo):
        """Sets the image that will be processed.

        :param str image_path: Path to the image to be processed
        :param str sign_id: sign_id corresponding to the image_path. This must
            be one of the :meth:`available_sign_ids`
        :param bool gen_multi_image: Determines whether a new Multi-Image Sign & Integrity image will be generated & operated on
        :param str multi_image_path: Path to the existing Multi-Image Sign & Integrity image to be operated on
        """
        if self._img_config_parser is not None and self._parsegen_config is not None:
            self._stager = ImagePathsStager(
                image_path, self._img_config_parser, self._parsegen_config, self.authority,
                sign_id=sign_id, gen_multi_image=gen_multi_image, multi_image_path=multi_image_path,
                imageinfo_class=imageinfo_class, verbatim_config=self.verbatim_config,
                disable_serial_binding=self.disable_serial_binding, platform_binding=self.platform_binding)
        else:
            raise RuntimeError('Set chipset or config Path before setting the image path.')

    def set_meta_build_path(self, meta_build_path, sign_id_list=[], gen_multi_image=False):
        """Sets the meta build path from which the images to be processed will
        be queried.

        :param str meta_build_path: Path to the meta build.
        :param bool gen_multi_image: Determines whether a new Multi-Image Sign &
                Integrity image will be generated & operated on
        """
        if self.meta_supports_sign_id(meta_build_path):
            self._stager = MetaBuildStager(
                meta_build_path, self._config_dir_obj, self._parsegen_config,
                self.authority, sign_id_list, gen_multi_image, platform_binding=self.platform_binding)
        else:
            self._stager = MetaBuildTmpStager(
                meta_build_path, self._config_dir_obj, self._img_config_parser,
                self._parsegen_config, self.authority, sign_id_list, gen_multi_image,
                platform_binding=self.platform_binding)

    def set_chipset(self, chipset, overrides=None, spec_overrides=None):
        # Log if the chipset is changed
        try:
            selected_chipset = self.chipset
            if selected_chipset:
                logger.note('Switching chipset from "' + selected_chipset + '" to "' + chipset + '"')
        except Exception:
            pass

        # Update the config path based on the given chipset
        config_path = self._config_dir_obj.get_chipset_config_path(chipset)
        if config_path is None:
            raise RuntimeError('Config file is not found for chipset: ' + str(chipset))
        self.set_config_path(config_path, overrides, spec_overrides)
        logger.debug('Chipset is set to: ' + chipset)

    def set_config_path(self, config_path, overrides=None, spec_overrides=None):
        # Log if the config_path is changed
        try:
            selected_config_path = self.config_path
            if selected_config_path:
                logger.note('Switching config_path: ' + '\n'
                            '    ' + 'from: ' + selected_config_path + '\n'
                            '    ' + ' to: ' + config_path)
        except Exception:
            pass

        # Update the config_path
        self._img_config_parser = SecImageConfigParser(
            config_path, overrides=overrides, spec_overrides=spec_overrides,
            verbatim_config=self.verbatim_config, platform_binding=self.platform_binding)
        logger.info('Config path is set to: ' + config_path)

        # Reset the stager at this point
        if self._stager is not None:
            logger.note('Resetting image list due to config path change.')
            self._stager = None

    @property
    def image_path_list(self):
        """(list[str]) List of source image paths that are being operated on.
        This is updated once one of :meth:`set_image_path`,
        :meth:`set_meta_build_path` is used to specify the images to be
        processed.
        """
        if self._stager is not None:
            return self._stager.image_path_list
        else:
            raise RuntimeError('Image list is not set. '
                               'Set one of image_path or meta_build_path.')

    @image_path_list.setter
    def image_path_list(self, image_path_list):
        raise RuntimeError('Image path list cannot be set directly. '
                           'Use set_image_path or set_meta_build to set the image paths.')

    @property
    def image_info_list(self):
        """(list[ImageInfo]) A list of :class:`imageinfo.ImageInfo` objects that are being
        operated on.
        """
        if self._stager is not None:
            return self._stager.image_info_list
        else:
            raise RuntimeError('Image Info list is not set. '
                               'Set one of image_path or meta_build_path.')

    def dump_parsegen_debug_data(self, image, parsegen):
        try:
            self.debug_files.append(
                (parsegen.get_data(True, True, True),
                 image.dest_image.debug_file_parsegen_encrypted))
        except Exception:
            pass
        super(SecImageCore, self).dump_parsegen_debug_data(image, parsegen)

    @classmethod
    def meta_supports_sign_id(self, meta_build_path):
        return MetaBuildStager.meta_supports_sign_id(meta_build_path)

    @staticmethod
    def pil_split(src, prefix):
        # Load the data
        p_obj = ParseGenElf(load_data_from_file(src))

        # File names
        elfhdr = prefix + '.mdt'

        # Cleanup
        for eachpattern in [elfhdr, (prefix + '.b*')]:
            for eachfile in glob.glob(eachpattern):
                try: os.remove(eachfile)
                except Exception: pass

        # Create the mdt data
        elfdata = p_obj.ehdr.pack() + pack_phdrs(p_obj.phdrs)

        # Dump the segments
        for idx, phdr in enumerate(p_obj.phdrs):
            path = prefix + ('.b%02d' % idx)
            store_data_to_file(path, p_obj.segments[phdr])
            if phdr.f_os_segment_type == PF_OS_SEGMENT_HASH:
                elfdata += p_obj.segments[phdr]

        # Dump the final elfhdr mdt
        store_data_to_file(elfhdr, elfdata)

    def _post_process(self, image, pil_splitter_path, meta_build_path):
        ''' Replacement tags in postsign commands for images. '''
        TAG_OUTPUT_DIR = '$(OUTPUT_DIR)'
        TAG_PIL_SPLITTER = '$(PIL_SPLITTER)'
        TAG_IMAGE_FILE = '$(IMAGE_FILE)'
        REPL_META_PATH = '$(META_BUILD)'

        if image.pil_split:
            image_file = image.image_under_operation
            SecImageCore.pil_split(image_file, os.path.splitext(image_file)[0])

        if image.post_process_commands:
            # Ensure pil splitter is available
            if pil_splitter_path.find(REPL_META_PATH) != -1:
                if not meta_build_path:
                    raise RuntimeError('Metabuild path is not available for pil splitter')
                pil_splitter_path = pil_splitter_path.replace(REPL_META_PATH, meta_build_path)

            if not c_path.validate_file(pil_splitter_path):
                raise RuntimeError('Cannot access pil splitter at: ' + pil_splitter_path)

            # Run all the commands
            for cmd in [c.strip() for c in image.post_process_commands.split()]:
                # Perform any needed replacements
                cmd = cmd.replace(TAG_OUTPUT_DIR, os.path.dirname(image.image_under_operation))
                cmd = cmd.replace(TAG_PIL_SPLITTER, pil_splitter_path)
                cmd = cmd.replace(TAG_IMAGE_FILE, image.image_under_operation)

                logger.info('Running postsign command: ' + cmd)
                err = os.system(cmd)
                logger.info('Result: ' + str(err))

    def _sign_image(self, image, parsegen):
        # Check all other authorities and display if they have signed
        for authority in [x for x in self.AUTHORITIES_SUPPORTED if x != self.authority]:
            if parsegen.is_signed(authority):
                logger.info(image.image_under_operation + ' is already ' + str(authority) + '-signed.')

        # Prevent re-signing by authorities that previously signed that aren't the most recently signed authority
        previously_signed_authorities = []
        for authority in self.AUTHORITIES_SUPPORTED:
            if parsegen.is_signed(authority):
                previously_signed_authorities.append(authority)
        last_signed_authority = self.highest_authority([AUTHORITY_OEM in previously_signed_authorities,
                                                        AUTHORITY_QTI in previously_signed_authorities])
        if self.authority != last_signed_authority and self.authority in previously_signed_authorities:
            raise RuntimeError("Cannot re-sign as {0} because image is already signed by {1}"
                               .format(self.authority, last_signed_authority))

        # Display sign status of signing authority
        logger.info(image.image_under_operation + ' is already ' + self.authority + '-signed. '
                    'Rerunning ' + self.authority + ' sign.' if parsegen.is_signed(self.authority)
                    else 'Performing ' + self.authority + ' sign on image: ' + image.image_under_operation)

        # Sign image and retry if signing assets violate device requirements
        num_tries = 1
        while num_tries <= 10:
            signer = get_signer(image.signer_config)
            sign_assets = signer.sign(parsegen.data_to_sign, image, debug_dir=image.dest_image.debug_dir_signer,
                                      is_hash=parsegen.is_data_hash, parsegen=parsegen)
            # Ensure signing assets meet device requirements or max number of retries has been reached
            if signer.validate_ecdsa_signer_output(sign_assets) or num_tries == 10:
                parsegen.data_signature = sign_assets.signature
                parsegen.cert_chain = sign_assets.cert_chain
                self.dump_signer_debug_data(image, sign_assets, parsegen)
                break
            logger.warning("Retrying signing because R or S value of ECDSA signature was of unsupported length.")
            num_tries += 1

    def _process_validation(self, image, progress, security_policy_list,
                            i_val_image, i_val_integrity_check, i_val_sign,
                            i_val_encrypt, idx=0, prefix_override=None):

        # Check bounds
        if len(security_policy_list) == 0:
            raise RuntimeError('Security policy list must not be empty.')
        elif len(security_policy_list) <= idx:
            raise RuntimeError('Security policy list length must be more than index.')

        # Get the current security policy
        file_type = security_policy_list[idx].file_type
        val_image = i_val_image
        val_integrity_check = security_policy_list[idx].integrity_check and i_val_integrity_check
        val_sign = security_policy_list[idx].sign and i_val_sign
        val_encrypt = security_policy_list[idx].encrypt and i_val_encrypt

        # Backup the source path here
        src_image_dir_base = image.src_image.image_dir_base
        src_image_dir_ext = image.src_image.image_dir_ext
        src_image_name = image.src_image.image_name

        # Update the source image path
        image.src_image.image_dir_base = os.path.dirname(image.image_under_operation)
        image.src_image.image_dir_ext = ''
        image.src_image.image_name = os.path.basename(image.image_under_operation)

        try:
            # Create the encryptor object
            encdec = None
            if image.general_properties.selected_encryptor:
                c_path.create_debug_dir(image.dest_image.debug_dir_encdec)
                encdec = get_encdec(image, True)

            # Create the parsegen object
            file_type_backup = image.image_type.file_type
            encdec_backup = image.encdec
            image.image_type.file_type = file_type
            image.encdec = encdec
            try:
                parsegen = self._status_updater(self._create_parsegen_obj,
                                                image.status.validate_parsegen,
                                                progress,
                                                True,
                                                image,
                                                True,
                                                False)
            finally:
                image.image_type.file_type = file_type_backup
                image.encdec = encdec_backup

            # Populating hash segment address for validation in parsegen object which is already present in image
            if hasattr(parsegen, 'physical_address_hash_seg'):
                parsegen.physical_address_hash_seg = image.hash_seg_address

            # Validate the authority settings
            self.validate_authority_settings(self.authority,
                                             image.general_properties.secboot_version,
                                             image.general_properties.qti_sign,
                                             image.general_properties.oem_sign,
                                             image.general_properties.num_root_certs,
                                             parsegen.is_encrypted())

            # Prevent validation when using an encrypted key provider
            encrypted_key_provider_id = image.signing_attributes.UIE_key
            if parsegen.is_encrypted() and encrypted_key_provider_id_supported(encrypted_key_provider_id):
                raise RuntimeError("Validation is not supported when using an encrypted key provider.\n"
                                   "Try again with validation disabled.")

            # Set the security mechanisms
            parsegen.integrity_check = parsegen.contains_integrity_check()
            parsegen.sign = parsegen.is_signed()
            parsegen.encrypt = parsegen.is_encrypted()

            # Validate parsegen
            if val_image:
                self._status_updater(self._validate_parsegen,
                                     image.status.validate_parsegen,
                                     progress, False,
                                     image, parsegen)

            # Validate integrity check
            if val_integrity_check:
                self._status_updater(self._validate_integrity_check,
                                     image.status.validate_integrity_check,
                                     progress, False,
                                     image, parsegen)

            # Validate sign
            if val_sign:
                self._status_updater(self._validate_sign,
                                     image.status.validate_sign,
                                     progress, False,
                                     image, parsegen)

            # Validate encrypt
            if val_encrypt:
                self._status_updater(self._validate_encrypt,
                                     image.status.validate_encrypt,
                                     progress, False,
                                     image, parsegen)

            # Take serial-binding notes on the image.
            for authority, signing_enabled in ((AUTHORITY_QTI, "qti_signing_enabled"),
                                               (AUTHORITY_OEM, "oem_signing_enabled")):
                if not getattr(parsegen, signing_enabled, False):
                    continue
                if security_policy_list[idx].sign and parsegen.has_mbn_parsegen:
                    update_image_serial_bound_notes(authority, parsegen, image)
                    try:
                        evaluate_serial_binding_attributes(authority, image)
                    except RuntimeError as e:
                        logger.error(str(e))
                        if parsegen.is_signed(authority=authority):
                            image.status.validate_sign.state = StatusInfo.ERROR
                        else:
                            image.status.validate_integrity_check.state = StatusInfo.ERROR

            # Dump any debug data
            self.dump_parsegen_debug_data(image, parsegen)

            # If the security policy list contains more formats, call them
            if idx < len(security_policy_list) - 1:
                tmp_file_path = c_path.create_tmp_file(data=parsegen.get_wrapped_data())

                # Backup the image_under_operation here
                image_under_operation_int = image.image_under_operation
                image.image_under_operation = tmp_file_path

                try:
                    self._process_validation(image, progress, security_policy_list,
                                             i_val_image, i_val_integrity_check, i_val_sign, i_val_encrypt, idx + 1)
                finally:
                    image.image_under_operation = image_under_operation_int

                os.remove(tmp_file_path)

        finally:
            image.src_image.image_dir_base = src_image_dir_base
            image.src_image.image_dir_ext = src_image_dir_ext
            image.src_image.image_name = src_image_name

        return parsegen

    def _get_encrypted_segments_index_list(self, phdrs):
        encrypted_segments_indices = []
        found_not_encrypt_segments = False
        for indx, phdr in enumerate(phdrs):
            if phdr.p_type == PT_LOAD and phdr.f_os_segment_type not in NON_ENCRYPT_SEGMENT_OS_TYPE:
                encrypted_segments_indices.append(indx+2)
            elif phdr.f_os_segment_type in NON_ENCRYPT_SEGMENT_OS_TYPE:
                found_not_encrypt_segments = True
                logger.debug("LOAD segment " + str(indx+2) + " won't be encrypted")
        return (found_not_encrypt_segments, encrypted_segments_indices)

    def _create_parsegen(self, image, progress, security_policy_list, idx=0, prefix_override=None):
        # Check bounds
        if len(security_policy_list) == 0:
            raise RuntimeError('Security policy list must not be empty.')
        elif len(security_policy_list) <= idx:
            raise RuntimeError('Security policy list length must be more than index.')
        file_type = security_policy_list[idx].file_type

        # Backup the source path here
        src_image_dir_base = image.src_image.image_dir_base
        src_image_dir_ext = image.src_image.image_dir_ext
        src_image_name = image.src_image.image_name

        # Update the souce image path
        image.src_image.image_dir_base = os.path.dirname(image.image_under_operation)
        image.src_image.image_dir_ext = ''
        image.src_image.image_name = os.path.basename(image.image_under_operation)

        try:
            # Create the encryptor object
            encdec = None
            if image.general_properties.selected_encryptor:
                c_path.create_debug_dir(image.dest_image.debug_dir_encdec)
                encdec = get_encdec(image, True)

            # Create the parsegen object
            file_type_backup = image.image_type.file_type
            encdec_backup = image.encdec
            image.image_type.file_type = file_type
            image.encdec = encdec
            try:
                parsegen = self._status_updater(self._create_parsegen_obj,
                                                image.status.parsegen,
                                                progress,
                                                True,
                                                image,
                                                validating=False,
                                                signing=False)
            finally:
                image.image_type.file_type = file_type_backup
                image.encdec = encdec_backup

            # Validate the authority settings
            self.validate_authority_settings(self.authority,
                                             image.general_properties.secboot_version,
                                             image.general_properties.qti_sign,
                                             image.general_properties.oem_sign,
                                             image.general_properties.num_root_certs,
                                             parsegen.is_encrypted())

            # Set the security mechanisms
            parsegen.integrity_check = parsegen.contains_integrity_check()
            parsegen.sign = parsegen.is_signed()
            parsegen.encrypt = parsegen.is_encrypted()

            # Dump any debug data
            self.dump_parsegen_debug_data(image, parsegen)

            # If the security policy list contains more formats, call them
            if idx < len(security_policy_list) - 1:
                tmp_file_path = c_path.create_tmp_file(data=parsegen.get_wrapped_data())

                # Backup the image_under_operation here
                image_under_operation_int = image.image_under_operation
                image.image_under_operation = tmp_file_path

                try:
                    self._create_parsegen(image, progress, security_policy_list, idx + 1)
                finally:
                    image.image_under_operation = image_under_operation_int

                os.remove(tmp_file_path)

        finally:
            image.src_image.image_dir_base = src_image_dir_base
            image.src_image.image_dir_ext = src_image_dir_ext
            image.src_image.image_name = src_image_name

        return parsegen

    def _create_parsegen_obj(self, image, validating=False, signing=False, prefix_override=None, file_exists=True,
                             parsegens=None, sign_attr=False):
        c_path.create_debug_dir(image.dest_image.debug_dir_parsegen)
        parsegen = get_parser(image, validating, signing, prefix_override, file_exists, parsegens, sign_attr)

        # Check if parsegen authority settings are correct
        parsegen.authority = self.authority
        parsegen.validate_authority()
        return parsegen

    def _process_secure_operation(self, image, progress, security_policy_list,
                                  sign_attr, i_integrity_check, i_sign, i_encrypt,
                                  i_decrypt, idx=0, prefix_override=None,
                                  gen_multi_image=False, parsegens=None):
        # Check bounds
        if len(security_policy_list) == 0:
            raise RuntimeError('Security policy list must not be empty.')
        elif len(security_policy_list) <= idx:
            raise RuntimeError('Security policy list length must be more than index.')

        # Get the current security policy
        file_type = security_policy_list[idx].file_type
        integrity_check = security_policy_list[idx].integrity_check and i_integrity_check
        sign = security_policy_list[idx].sign and i_sign
        encrypt = security_policy_list[idx].encrypt and i_encrypt
        decrypt = security_policy_list[idx].encrypt and i_decrypt

        # Create the encryptor object
        encdec = None
        if image.general_properties.selected_encryptor:
            c_path.create_debug_dir(image.dest_image.debug_dir_encdec)
            encdec = get_encdec(image, False)

        # Create the parsegen object
        file_type_backup = image.image_type.file_type
        encdec_backup = image.encdec
        image.image_type.file_type = file_type
        image.encdec = encdec
        try:
            parsegen = self._status_updater(self._create_parsegen_obj,
                                            image.status.parsegen,
                                            progress,
                                            True,
                                            image,
                                            False,
                                            sign,
                                            prefix_override,
                                            not gen_multi_image,
                                            parsegens,
                                            sign_attr)
        finally:
            image.image_type.file_type = file_type_backup
            image.encdec = encdec_backup

        # Validate the authority settings
        self.validate_authority_settings(self.authority,
                                         image.general_properties.secboot_version,
                                         image.general_properties.qti_sign,
                                         image.general_properties.oem_sign,
                                         image.general_properties.num_root_certs,
                                         encrypt)

        # Do not allow signed unencrypted elf images to be encrypted as the
        # Sign will not longer match the encrypted image's tosign
        if parsegen.file_type() == 'elf' and parsegen.is_signed() and not sign and not parsegen.is_encrypted() and encrypt:
            raise RuntimeError('Cannot encrypt a signed unencrypted image without re-signing '
                               'as the sign no longer matches the format change.')

        # Prevent updating signing attributes (without re-signing) by current authority
        # if image is already signed by authority because old signature will not match
        if parsegen.is_signed(self.authority) and sign_attr and not i_sign:
            raise RuntimeError('Cannot update hash segment\'s signing attributes without re-signing because signature will become invalid.')

        # Set the security mechanisms
        parsegen.integrity_check = security_policy_list[idx].integrity_check and (
                parsegen.contains_integrity_check() or integrity_check)
        parsegen.sign = security_policy_list[idx].sign and (parsegen.is_signed() or sign)
        parsegen.encrypt = security_policy_list[idx].encrypt and (False if decrypt else (parsegen.is_encrypted() or encrypt))
        # Set the parsegen.encrypt false if parsegen is already signed and encrypted and requested only for re-signing
        if hasattr(parsegen, 'persist_encr') and parsegen.persist_encr:
            if parsegen.is_signed(self.authority) and parsegen.is_encrypted() and sign and not encrypt:
                parsegen.encrypt = False

        # Populating hash segment address in parsegen object which is already present in image only if image is double signed otherwise re-sign with new hash segment address for all kind of images
        # Physical address of the hash segment will be calculated according to the following conditions:-
        # 1.) For OEM signing/re-signing on double signed images - Same hash segment address will be used
        # 2.) For QTI signing/re-signing on double signable images which is not oem signed - Address will be recalculated
        # 3.) For OEM signing/re-signing on only OEM signed images - Address will be recalculated
        # 4.) For QTI exclusive signing/re-signing - Address will be recalculated
        if parsegen.is_signed(AUTHORITY_QTI) and self.authority == AUTHORITY_OEM:
            parsegen.physical_address_hash_seg = image.hash_seg_address

        # Get blob if populating encryption parameters:
        if parsegen.encrypt:
            if encdec is None:
                raise RuntimeError('Encryptor is not set')
            parsegen.encryption_params = encdec.get_encryption_parameters_blob()
            # check if input image is a TA, if yes re-initialize encdec object with updated bitmap for manifest segment
            if is_TA(image.signing_attributes.sw_id):
                non_encrypt_segment_found, encrypted_segments_indices =\
                    self._get_encrypted_segments_index_list(parsegen._elf_parsegen.phdrs)
                if non_encrypt_segment_found and len(encrypted_segments_indices) > 0:
                    logger.info("Re-initializing encdec object for updated segment bitmap")
                    encdec = get_encdec(image, False, encrypted_segments_indices)
                    parsegen.encryption_params = encdec.get_encryption_parameters_blob()
                    parsegen.encdec = encdec
        elif parsegen.encryption_params and parsegen.encdec is not None:
            parsegen.encdec.update_encryption_parameters(parsegen.encryption_params)

        # Dump any debug data
        self.dump_parsegen_debug_data(image, parsegen)

        # If the security policy list contains more formats, call them
        if idx < len(security_policy_list) - 1:
            tmp_file_path = c_path.create_tmp_file(data=parsegen.get_wrapped_data())

            # Backup the source path here
            src_image_dir_base = image.src_image.image_dir_base
            src_image_dir_ext = image.src_image.image_dir_ext
            src_image_name = image.src_image.image_name

            image.src_image.image_dir_base = os.path.dirname(tmp_file_path)
            image.src_image.image_dir_ext = ''
            image.src_image.image_name = os.path.basename(tmp_file_path)

            # Override debug dumped file prefix
            prefix_override = SecImageCore._get_prefix_override(prefix_override, file_type)

            try:
                data = self._process_secure_operation(image, progress, security_policy_list, sign_attr,
                                                      i_integrity_check, i_sign, i_encrypt, i_decrypt, idx + 1, prefix_override)
            finally:
                image.src_image.image_dir_base = src_image_dir_base
                image.src_image.image_dir_ext = src_image_dir_ext
                image.src_image.image_name = src_image_name

            os.remove(tmp_file_path)

            parsegen.set_wrapped_data(data)

        # Sign the image
        if sign:
            self._status_updater(self._sign_image,
                                 image.status.sign,
                                 progress, True,
                                 image, parsegen)

        # Package and generate the output image file
        data = parsegen.get_data()
        if integrity_check:
            image.status.integrity_check.state = StatusInfo.SUCCESS
        if encrypt:
            image.status.encrypt.state = StatusInfo.SUCCESS

        if idx != 0:
            return data

        if decrypt:
            encryption_params_backup = parsegen.encryption_params
            parsegen.encryption_params = ''
            try:
                store_data_to_file(image.dest_image.decrypted_file, parsegen.get_data())
            finally:
                parsegen.encryption_params = encryption_params_backup

        else:
            store_data_to_file(image.dest_image.image_path, data)

        logger.info(('Signed ' if parsegen.sign else '') +
                    ('& ' if parsegen.sign and parsegen.encrypt else '') +
                    ('Encrypted ' if parsegen.encrypt else '') +
                    'image is stored at ' + image.dest_image.image_path,
                    color=logger.GREEN)
        image.image_under_operation = image.dest_image.image_path

        # Do any post processing
        self._status_updater(self._post_process,
                             image.status.postprocess,
                             progress, True,
                             image, image.config.post_process.pil_splitter,
                             getattr(self._stager, '_meta_build_path', None))

        return parsegen

    def process(self,
                verify_setup=False,
                sign_attr=False,
                integrity_check=False,
                sign=False,
                encrypt=False,
                decrypt=False,
                no_op=False,
                val_image=False,
                val_integrity_check=False,
                val_sign=False,
                val_encrypt=False,
                m_sign_attr=False,
                m_integrity_check=False,
                m_sign=False,
                m_encrypt=False,
                m_decrypt=False,
                m_val_image=False,
                m_val_integrity_check=False,
                m_val_sign=False,
                m_val_encrypt=False,
                gen_multi_image=False,
                root_cert_hash=None,
                progress_cb=PROGRESS_CB_PYPASS,
                enforce_signed=False,
                enforce_encrypted=False):
        """Performs the secure-image related operations specified from the params.

        :param bool verify_setup: Verify that the configuration of the object is correct and return. This will ignore any other params.
        :param bool sign_attr: Add signing attributes to hash segment of images.
        :param bool integrity_check: Add integrity check to the image.
        :param bool sign: Sign the images. (Re-sign if image is already signed)
        :param bool encrypt: Encrypt the images. (Re-encrypt if image is already encrypted)
        :param bool no_op: Perform no operation on the images when adding their entries to the Multi-Image Sign & Integrity image.
        :param bool val_image: Validate the integrity of the image against the config file.
        :param bool val_integrity_check: Validate the integrity check in the image.
        :param bool val_sign: Validate that the image is signed and validate the integrity of the signing related data.
        :param bool val_encrypt: Validate that the image is encrypted and validate the integrity of the encryption related data.
        :param bool m_sign_attr: Add signing attributes to hash segment of the Multi-Image Sign & Integrity image.
        :param bool m_integrity_check: Add integrity check to the Multi-Image Sign & Integrity image.
        :param bool m_sign: Sign the Multi-Image Sign & Integrity image. (Re-sign if image is already signed)
        :param bool m_encrypt: Encrypt the Multi-Image Sign & Integrity image. (Re-encrypt if image is already encrypted)
        :param bool m_val_image: Validate the integrity of the Multi-Image Sign & Integrity image against the config file.
        :param bool m_val_integrity_check: Validate the integrity check in the Multi-Image Sign & Integrity image.
        :param bool m_val_sign: Validate that the Multi-Image Sign & Integrity image is signed and validate the integrity of the signing related data.
        :param bool m_val_encrypt: Validate that the Multi-Image Sign & Integrity image is encrypted and validate the integrity of the encryption related data.
        :param bool gen_multi_image: Perform multi-image sign and integrity operations.
        :param cb progress_cb: Callback method to get a status update during processing.
        :param bool enforce_signed: If image is not signed, validation error is raised.
        :param bool enforce_encrypted: If image is not encrypted, validation error is raised.

            Callback method should have this format:
            ::
                def progress_cb(status_string, progress_percent):
                    \"""
                    :param str status_string: the current status.
                    :param int progress_percent: the progress (in percent)
                    \"""
                    ...
        """
        self.enforce_signed = enforce_signed
        self.enforce_encrypted = enforce_encrypted

        # Ensure that one or more image files is provided for processing
        if self._stager is None or not self.image_path_list:
            raise RuntimeError('Please specify one or more images for processing')

        # Ensure the correct set of operations is provided
        if integrity_check or sign or encrypt:
            val_image = True
            val_integrity_check = True
            val_sign = True
            val_encrypt = True
        elif not (verify_setup or decrypt or no_op or val_image or val_integrity_check or val_sign or val_encrypt):
            raise RuntimeError('Please specify one or more operations to perform.')

        # Ensure the correct Multi-Image set of operations is provided
        if self.multi_image_imageinfo_dict and not (m_integrity_check or m_sign or m_encrypt or m_decrypt or m_val_image or m_val_integrity_check or m_val_sign or m_val_encrypt):
            raise RuntimeError('Please specify one or more ' + multi_image_string() + ' operations to perform.')

        if m_integrity_check or m_sign or m_encrypt:
            m_val_image = True
            m_val_integrity_check = True
            m_val_sign = True
            m_val_encrypt = True

        # Print the openssl path
        version = ''
        path_info = 'is unavailable. Please run "which openssl" and "openssl version" to check openssl version info, and upgrade to required version'
        try:
            version = 'v' + '.'.join([str(x) for x in crypto.openssl.OPENSSL_VERSION_MIN]) + ' or greater '
            path_info = 'is available at: "' + crypto.discovery_factory.get_impl(crypto.modules.MOD_OPENSSL) + '"'
        except Exception as e:
            pass
        logger.debug('Openssl ' + version + path_info)

        if verify_setup:
            logger.note('The inputs provided (config, cmd args) are valid.')
            return

        # Start processing images
        individual_image_count = len(self.image_info_list)
        total_image_count = individual_image_count
        if self.multi_image_imageinfo_dict:
            total_image_count += len(self.multi_image_imageinfo_dict)
        progress = ProgressNotifier(total_image_count, progress_cb, PROGRESS_TOTAL_STAGES)

        # If Multi-Image operations need to be performed, save parsegens of individual images for each target
        parsegens = dict()

        def _process(idx, image, sign_attr, integrity_check, sign, encrypt, decrypt, no_op, val_image,
                     val_integrity_check, val_sign, val_encrypt, gen_multi_image=False,
                     is_multi_image=False, save_parsegen=False):
            assert isinstance(image, ImageInfo)

            logger.info('------------------------------------------------------')
            if not is_multi_image:
                status_string = ('Processing ' + str(idx + 1) + '/' + str(total_image_count) + ': ' + image.image_under_operation)
            else:
                # Creating a new Multi-Image image
                if gen_multi_image:
                    status_prefix = 'Creating & Processing '
                    status_suffix = ': ' + image.image_under_operation
                # Modifying an existing Multi-Image image
                else:
                    status_prefix = 'Processing '
                    status_suffix = ' (' + multi_image_string() + ' image): ' + image.image_under_operation
                status_string = (status_prefix + str(idx + 1) + '/' + str(total_image_count) + status_suffix)
            logger.info(status_string + '\n')

            # Send a progress notification to the toplevel
            progress.status = status_string
            progress.cur = idx
            progress.cur_stage = 0

            file_logger_id = None
            image.authority = self.authority

            try:
                # Create the required directory structure for this image
                image_output_dir = image.dest_image.image_dir
                try:
                    c_path.create_dir(image_output_dir)
                except Exception as e:
                    raise RuntimeError('Could not create output directory: ' + image_output_dir + '\n'
                                       '    ' + 'Error: ' + str(e))

                # Enable/Disable debug
                image.dest_image.debug_enable = self.debug
                c_path.create_debug_dir(image.dest_image.debug_dir)

                # Set the root cert hash
                image.validation_root_cert_hash = root_cert_hash

                # Enable file logging to the directory
                file_logger_id = logger.add_file_logger(c_path.join(image_output_dir, 'SecImage_log.txt'), logger.verbosity)

                # Create the security policies list for this image
                security_policy_list = create_security_policy_list(image)

                # Parsegen object
                parsegen = None

                # For secure operations
                if integrity_check or sign or encrypt or decrypt:
                    parsegen = self._process_secure_operation(
                        image, progress, security_policy_list, sign_attr, integrity_check, sign, encrypt, decrypt,
                        gen_multi_image=gen_multi_image, parsegens=parsegens.get(image.chipset, list()))

                # For validation
                if val_image or val_integrity_check or val_sign or val_encrypt:
                    # Bypass validation when encrypted key provider because image cannot be decrypted
                    if parsegen is not None and parsegen.is_encrypted() and encrypted_key_provider_id_supported(image.general_properties.UIE_key):
                        logger.warning("Skipping validation because encrypted key provider is in use")
                    # Bypass validation when using UIE 2.0 because decryption is only supported on device
                    elif parsegen is not None and parsegen.is_encrypted() and not parsegen.encdec.supports_decryption():
                        logger.warning("Skipping validation because decryption/validation is not supported for UIE 2.0")
                    else:
                        parsegen = self._process_validation(
                            image, progress, security_policy_list, val_image, val_integrity_check, val_sign, val_encrypt)

                # For no-op
                if no_op:
                    parsegen = self._create_parsegen(image, progress, security_policy_list)

                # Print the image data
                if parsegen is not None:
                    logger.info('\n' + str(parsegen))
                    # Save parsegen for multi_image operations
                    if save_parsegen:
                        if getattr(parsegen, "supports_multi_image", False):
                            if image.chipset in parsegens:
                                parsegens[image.chipset].append(parsegen)
                            else:
                                parsegens[image.chipset] = [parsegen]
                        else:
                            logger.warning(multi_image_not_supported_error_message.format(image.image_under_operation))

                # Set overall processing to true
                if not ((val_image and image.status.validate_parsegen.state == StatusInfo.ERROR) or
                        (val_integrity_check and image.status.validate_integrity_check.state == StatusInfo.ERROR) or
                        (val_sign and image.status.validate_sign.state == StatusInfo.ERROR) or
                        (val_encrypt and image.status.validate_encrypt.state == StatusInfo.ERROR)):
                    image.status.overall.state = StatusInfo.SUCCESS

            except RemoteSignerNote as e:
                logger.info('NOTE: ' + str(e), color=logger.YELLOW)
                # Image operation failed for individual image that was to be added to Multi-Image Signing & Integrity image
                if save_parsegen:
                    logger.error(adding_image_error_message.format(image.image_under_operation))
            except Exception:
                logger.error(traceback.format_exc())
                logger.error(sys.exc_info()[1])
                # Image operation failed for individual image that was to be added to Multi-Image Signing & Integrity image
                if save_parsegen:
                    logger.error(adding_image_error_message.format(image.image_under_operation))
            finally:
                if file_logger_id is not None:
                    logger.removeFileLogger(file_logger_id)

            logger.info('------------------------------------------------------\n')

        adding_image_error_message = "Image operation failed for image {0}\n\t" + multi_image_string() + " image won't contain entries for failed images."
        multi_image_not_supported_error_message = "Image {0} does not support being added to " + multi_image_string() + " image."

        for idx, image in enumerate(self.image_info_list):
            save_parsegen = bool(self.multi_image_imageinfo_dict)
            if save_parsegen and is_TA(image.general_properties.sw_id):
                # Skip TA images if a Multi-Image operation is prescribed.
                logger.warning("Skipped adding %s entry for Trusted Application" % multi_image_string())
                save_parsegen = False

            # Set dir, prefix and suffix arguments to be used by create_tmp_file.
            # Specifically, dir is the dest_image base directory, suffix is sha384 hash of
            # dest_image file path, and prefix is PID.
            # This is to ensure all temp files have unique file paths.
            c_path.tmp_file_params.update(
                dir=image.dest_image.image_dir_base,
                prefix="pid-%d-" % os.getpid(),
                suffix=hashlib.sha384(ensure_binary(image.dest_image.image_path)).hexdigest()[:20])

            _process(idx, image, sign_attr, integrity_check, sign, encrypt, decrypt,
                     no_op, val_image, val_integrity_check, val_sign, val_encrypt,
                     save_parsegen=save_parsegen)

        # Handle multi-image operations
        if self.multi_image_imageinfo_dict:
            image_index = 0
            for target, imageinfo in self.multi_image_imageinfo_dict.items():
                # Set dir, prefix and suffix arguments for create_tmp_file.
                c_path.tmp_file_params.update(
                    dir=imageinfo.dest_image.image_dir_base,
                    prefix="pid-%d-" % os.getpid(),
                    suffix=hashlib.sha384(ensure_binary(imageinfo.dest_image.image_path)).hexdigest()[:20])

                _process(individual_image_count+image_index, imageinfo,
                         m_sign_attr, m_integrity_check, m_sign, m_encrypt, m_decrypt, no_op,
                         m_val_image, m_val_integrity_check, m_val_sign, m_val_encrypt,
                         gen_multi_image=gen_multi_image, is_multi_image=True)
                image_index += 1

        progress.complete()

        # Reset tmp_file_params.
        c_path.tmp_file_params = {}

    def _validate_encrypt(self, image, parsegen):
        if parsegen.is_encrypted():
            logger.info('Image ' + image.image_under_operation + ' is encrypted')
        else:
            message = 'Image ' + image.image_under_operation + ' is not encrypted'
            if image in self.image_info_list and self.enforce_encrypted:
                raise RuntimeError(message)
            raise CustomError(message)

    def _validate_integrity_check(self, image, parsegen):
        parsegen.validate_hash_segment_metadata(self.authority)
