# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""
:Authors: sectools

Main interface to the secimage functionalities. Supports APIs for
command line interface, UI interface and test interface.
"""

import hashlib
import os
import sys

from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import load_data_from_file
from sectools.common.utils.c_misc import store_data_to_file
from sectools.features.isc.cfgparser import ConfigDir
from sectools.features.isc.cfgparser import ConfigParser
from sectools.features.isc.defines import AUTHORITY_OEM
from sectools.features.isc.imageinfo import StatusInfo
from sectools.features.isc.parsegen.elf_mbn_v3_base import ElfMbnV3Base
from sectools.features.isc.signer import get_signer

PROGRESS_TOTAL_STAGES = 7


class SecImageCoreBase(object):
    """Provides interface for user to set up parameters (including input files,
    output directory, etc) and perform various operations, such as signing.


    User can select the config file using:

    - :meth:`chipset`
    - :meth:`config_path`

    User can specify images for processing using:

    - :meth:`set_image_path`

    User can specify output using:

    - :meth:`output_dir`

    User can finally perform the following operations:

    - :meth:`verify_inputs`
    - :meth:`sign`
    - :meth:`validate`

    User can also perform multiple operations using :meth:`process`

    :param str config_dir: Path to the directory containing the config files
        for the supported chipsets.
    :param bool debug: If all the underlying modules should work in debug mode
        (save debug logs, etc).

    .. data:: debug

        (bool) Is debug mode enabled. The debug logs will be stored in :meth:`output_dir`

    """
    AUTHORITIES_SUPPORTED = [AUTHORITY_OEM]

    dest_image_path_class = None

    def __init__(self, config_dir, parsegen_config, cfgparser=None, debug=False):
        # Initialize the internal properties
        self._config_dir_obj = ConfigDir(config_dir, cfgparser=cfgparser)
        self._img_config_parser = None
        self._stager = None
        self._authority = AUTHORITY_OEM
        self._parsegen_config = parsegen_config

        # ConfigParser named tuple
        self.cfgparser = cfgparser

        # Initialize the public properties
        self.debug = debug
        self.enforce_signed = False

        # Debug Logs
        self.debug_files = []

    def highest_authority(self, auth_enabled_list):
        auths = [k for i, k in enumerate(self.AUTHORITIES_SUPPORTED) if auth_enabled_list[i]]
        return auths[0] if auths else ''

    @property
    def config_dir(self):
        """(str) Path to the config directory provided in params. The config_dir
        path cannot be set directly.
        """
        return self._config_dir_obj.config_dir

    @config_dir.setter
    def config_dir(self, config_dir):
        raise RuntimeError('Config dir cannot be set.')

    @property
    def available_chipsets(self):
        """(list[str]) A list of chipsets for which config files are available
        in the :meth:`config_dir`. The list of available chipsets cannot be set
        directly.
        """
        return self._config_dir_obj.chipsets

    @available_chipsets.setter
    def available_chipsets(self, chipset_list):
        raise RuntimeError('Available chipsets cannot be set.')

    @property
    def available_sign_ids(self):
        """(list[str]) A list of sign_ids available from the selected
        :meth:`config_path`/:meth:`chipset`.
        """
        if self._img_config_parser is not None:
            return self._img_config_parser.sign_id_list
        else:
            raise RuntimeError('Set chipset or config Path before querying the available sign ids.')

    @available_sign_ids.setter
    def available_sign_ids(self, chipset_list):
        raise RuntimeError('Available sign ids cannot be set.')

    @property
    def chipset(self):
        """(str) Chipset that is selected for the current run. The chipset
        selection will update the :meth:`config_path`
        """
        if self._img_config_parser is not None:
            return self._img_config_parser.chipset
        else:
            raise RuntimeError('Chipset Path is not set.')

    @chipset.setter
    def chipset(self, chipset):
        self.set_chipset(chipset)

    def set_chipset(self, chipset, overrides=None):
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
        self.set_config_path(config_path, overrides)
        logger.debug('Chipset is set to: ' + chipset)

    @property
    def config_path(self):
        """(str) Config path that is selected for the current run. The
        config_path selection will update the :meth:`chipset`
        """
        if self._img_config_parser is not None:
            return self._img_config_parser.config_path
        else:
            raise RuntimeError('Config Path is not set.')

    @config_path.setter
    def config_path(self, config_path):
        self.set_config_path(config_path)

    def set_config_path(self, config_path, overrides=None):
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
        self._img_config_parser = ConfigParser(
            config_path, cfgparser=self.cfgparser, overrides=overrides)
        logger.debug('Config path is set to: ' + config_path)

        # Reset the stager at this point
        if self._stager is not None:
            logger.note('Resetting image list due to config path change.')
            self._stager = None

    @property
    def authority(self):
        """(str) Config path that is selected for the current run. The
        config_path selection will update the :meth:`chipset`
        """
        return self._authority

    @authority.setter
    def authority(self, authority):
        if authority not in self.AUTHORITIES_SUPPORTED:
            raise RuntimeError('Unknown authority for secure operations: ' + str(authority))
        self._authority = authority

    @property
    def image_path_list(self):
        """(list[str]) List of source image paths that are being operated on.
        Use :meth:`set_image_path` to specify the images to be processed.
        """
        if self._stager is not None:
            return self._stager.image_path_list
        else:
            raise RuntimeError('Image list is not set. Set one of image_path.')

    @image_path_list.setter
    def image_path_list(self, image_path_list):
        raise RuntimeError('Image path list cannot be set directly. '
                           'Use set_image_path to set the image paths.')

    @property
    def image_info_list(self):
        """(list[ImageInfo]) A list of :class:`imageinfo.ImageInfo` objects that are being
        operated on.
        """
        if self._stager is not None:
            return self._stager.image_info_list
        else:
            raise RuntimeError('Image Info list is not set. Set one of image_path.')

    @image_info_list.setter
    def image_info_list(self, image_info_list):
        raise RuntimeError('Image Info list cannot be set directly.')

    @property
    def output_dir(self):
        """(str) Path to the output directory to use for saving the logs, output
        files.
        """
        if self._stager is not None:
            return self._stager.output_dir
        else:
            raise RuntimeError('Image list is not set. '
                               'Set one of image_path or meta_build_path. '
                               'Then retry setting output directory.')

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

    def dump_parsegen_debug_data(self, image, parsegen):
        if image.dest_image.debug_dir_parsegen is None:
            return

        so = parsegen
        fp = image.dest_image
        assert isinstance(fp, self.dest_image_path_class)

        try:
            self.debug_files.append((so.get_data(False, False, False),
                                     fp.debug_file_parsegen_unsigned))
        except Exception:
            pass
        try:
            self.debug_files.append((so.data_to_sign,
                                     fp.debug_file_parsegen_tosign))
        except Exception:
            pass
        try:
            self.debug_files.append((so.cert_chain,
                                     fp.debug_file_parsegen_cert_chain))
        except Exception:
            pass
        try:
            self.debug_files.append((so.data_signature,
                                     fp.debug_file_parsegen_signature))
        except Exception:
            pass
        try:
            self.debug_files.append((so.get_data(True, False, False),
                                     fp.debug_file_parsegen_integrity_check))
        except Exception:
            pass
        try:
            self.debug_files.append((so.get_data(True, True, False),
                                     fp.debug_file_parsegen_signed))
        except Exception:
            pass

        for data, debug_file in self.debug_files:
            try:
                store_data_to_file(debug_file, data)
            except Exception:
                logger.debug2('Failed to save debug file ' + debug_file + '\n'
                              '    ' + str(sys.exc_info()[1]))

    def dump_signer_debug_data(self, image, sign_assets, parsegen, more_debug_logs=[]):
        if image.dest_image.debug_dir_signer is None:
            return
        c_path.create_debug_dir(image.dest_image.debug_dir_signer)

        sa = sign_assets
        fp = image.dest_image
        assert isinstance(fp, self.dest_image_path_class)

        # Backup parsegen authority
        authority = parsegen.authority

        # OEM Signature and Cert Chain
        parsegen.authority = AUTHORITY_OEM
        data_signature = parsegen.data_signature
        cert_chain = parsegen.cert_chain

        # Restore authority
        parsegen.authority = authority

        debug_logs = [(sa.root_cert, fp.debug_file_signer_root_cert),
                      (sa.attestation_ca_cert, fp.debug_file_signer_attestation_ca_cert),
                      (sa.attestation_cert, fp.debug_file_signer_attestation_cert),
                      (data_signature, fp.debug_file_signer_signature),
                      (cert_chain, fp.debug_file_signer_cert_chain)]

        for dbg_log in more_debug_logs:
            debug_logs.append((dbg_log[0], getattr(fp, dbg_log[1])))

        # Save the private attributes too
        debug_logs += [(sa.root_key, fp.debug_file_signer_root_key),
                       (sa.attestation_ca_key, fp.debug_file_signer_attestation_ca_key),
                       (sa.attestation_key, fp.debug_file_signer_attestation_key)]

        for data, debug_file in debug_logs:
            try:
                store_data_to_file(debug_file, data)
            except Exception:
                logger.debug2('Failed to save debug file ' + debug_file + '\n'
                              '    ' + str(sys.exc_info()[1]))

    def _process_secure_operation(self, image, progress, security_policy_list,
                                  i_integrity_check, i_sign, idx=0, parsegens=None):
        # Check bounds
        if len(security_policy_list) == 0:
            raise RuntimeError('Security policy list must not be empty.')
        elif len(security_policy_list) <= idx:
            raise RuntimeError('Security policy list length must be more than index.')

        # Get the current security policy
        file_type = security_policy_list[idx].file_type
        integrity_check = security_policy_list[idx].integrity_check and i_integrity_check
        sign = security_policy_list[idx].sign and i_sign

        # Create the parsegen object
        file_type_backup = image.image_type.file_type
        image.image_type.file_type = file_type
        try:
            parsegen = self._status_updater(self._create_parsegen_obj,
                                            image.status.parsegen,
                                            progress,
                                            True,
                                            image,
                                            validating=False,
                                            signing=sign,
                                            parsegens=parsegens)
        finally:
            image.image_type.file_type = file_type_backup

        # Set the security mechanisms
        parsegen.integrity_check = security_policy_list[idx].integrity_check and (parsegen.contains_integrity_check() or integrity_check)
        parsegen.sign = security_policy_list[idx].sign and (parsegen.is_signed() or sign)

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

            try:
                data = self._process_secure_operation(image, progress, security_policy_list,
                                                      i_integrity_check, i_sign, idx=idx + 1)
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

        if idx != 0:
            return data

        store_data_to_file(image.dest_image.image_path, data)

        logger.info(('Signed ' if parsegen.sign else '') +
                    'image is stored at ' + image.dest_image.image_path,
                    color=logger.GREEN)
        image.image_under_operation = image.dest_image.image_path

        return parsegen

    def _process_validation(self, image, progress, security_policy_list,
                            i_val_image, i_val_integrity_check, i_val_sign, idx=0):
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

        # Backup the source path here
        src_image_dir_base = image.src_image.image_dir_base
        src_image_dir_ext = image.src_image.image_dir_ext
        src_image_name = image.src_image.image_name

        # Update the souce image path
        image.src_image.image_dir_base = os.path.dirname(image.image_under_operation)
        image.src_image.image_dir_ext = ''
        image.src_image.image_name = os.path.basename(image.image_under_operation)

        try:
            # Create the parsegen object
            file_type_backup = image.image_type.file_type
            image.image_type.file_type = file_type
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

            # Set the security mechanisms
            parsegen.integrity_check = parsegen.contains_integrity_check()
            parsegen.sign = parsegen.is_signed()

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
                                             i_val_image, i_val_integrity_check, i_val_sign, idx + 1)
                finally:
                    image.image_under_operation = image_under_operation_int

                os.remove(tmp_file_path)

        finally:
            image.src_image.image_dir_base = src_image_dir_base
            image.src_image.image_dir_ext = src_image_dir_ext
            image.src_image.image_name = src_image_name

        return parsegen

    def _create_parsegen(self, image, progress, security_policy_list, idx=0):
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
            # Create the parsegen object
            file_type_backup = image.image_type.file_type
            image.image_type.file_type = file_type
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

            # Set the security mechanisms
            parsegen.integrity_check = parsegen.contains_integrity_check()
            parsegen.sign = parsegen.is_signed()

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

    def _create_parsegen_obj(self, image, validating=False, signing=False, file_exists=True,
                             parsegens=None, sign_attr=False):
        c_path.create_debug_dir(image.dest_image.debug_dir_parsegen)

        image_path = image.src_image.image_path
        image_name = os.path.basename(image_path)
        image_base, image_ext = os.path.splitext(image_name)

        data = load_data_from_file(image_path) if file_exists else None
        parsegen = ElfMbnV3Base(data,
                                imageinfo=image,
                                debug_dir=image.dest_image.debug_dir_parsegen,
                                debug_prefix=image_base,
                                debug_suffix=image_ext,
                                validating=validating,
                                signing=signing,
                                parsegens=parsegens,
                                sign_attr=sign_attr)

        # Check if parsegen authority settings are correct
        parsegen.authority = self.authority
        parsegen.validate_authority()

        return parsegen

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
        last_signed_authority = self.highest_authority([AUTHORITY_OEM in previously_signed_authorities])
        if self.authority != last_signed_authority and self.authority in previously_signed_authorities:
            raise RuntimeError("Cannot resign as {0} because image is already signed by {1}"
                               .format(self.authority, last_signed_authority))

        # Display sign status of signing authority
        logger.info(image.image_under_operation + ' is already signed. '
                    'Re-signing image.' if parsegen.is_signed(self.authority)
                    else 'Signing image: ' + image.image_under_operation)

        signer = get_signer(image.signer_config)

        sign_assets = signer.sign(
            parsegen.data_to_sign, image, debug_dir=image.dest_image.debug_dir_signer,
            is_hash=parsegen.is_data_hash, parsegen=parsegen)

        parsegen.data_signature = sign_assets.signature
        parsegen.cert_chain = sign_assets.cert_chain

        # Dump any debug information
        self.dump_signer_debug_data(image, sign_assets, parsegen)

    def _validate_parsegen(self, image, parsegen):
        pass

    def _validate_integrity_check(self, image, parsegen):
        pass

    def _validate_sign(self, image, parsegen):
        if parsegen.is_signed():
            signer = get_signer(image.signer_config)
            if signer.validate(parsegen, imageinfo=image):
                logger.info('Image ' + image.image_under_operation + ' signature is valid')
            else:
                raise RuntimeError('Image ' + image.image_under_operation + ' signature is not valid')
        else:
            message = 'Image ' + image.image_under_operation + ' is not signed'
            if image in self.image_info_list and self.enforce_signed:
                raise RuntimeError(message)
            raise CustomError(message)

    def _status_updater(self, method, status, progress, raise_err, *args, **kwargs):
        try:
            retval = method(*args, **kwargs)
            status.state = StatusInfo.SUCCESS
            return retval
        except CustomError as e:
            # No change in imageinfo status.
            logger.info(str(e))
        except Exception as e:
            status.state = StatusInfo.ERROR
            status.error = str(e)
            if raise_err:
                raise
            else:
                logger.error(status.error)
        finally:
            progress.push()


class CustomError(RuntimeError):
    """
    CustomError class to exercise different processing than RuntimeError.
    """
    pass
