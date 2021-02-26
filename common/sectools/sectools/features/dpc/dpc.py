# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""
:Authors: sectools

Main interface to the DebugPolicy functionalities. Supports APIs for
command line interface, UI interface and test interface.
"""

import datetime
import os
import shutil

import defines
from sectools.common.core.feature import FeatureBase_Abs
from sectools.common.parsegen.dbgp.dbgpelf.format import ParseGenDbgpElf
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import store_data_to_file, \
    PROGRESS_CB_PYPASS, ProgressNotifier, load_data_from_file
from sectools.common.utils import c_path
from sectools.features.dpc.cfgparsegen.configfile import ConfigParser
from sectools.features.isc import SecImageCore

DEBUGPOLICY_ID = 'debugpolicy'


class DebugPolicyCore(FeatureBase_Abs):
    """Provides interface for user to set up parameters (including input config
    files, output directory, etc) and generate debugpolicy elf files.
    """

    def __init__(self, debug=False):
        """Initializes the internal properties."""
        # debugpolicy config file
        self._config_parser = None

        # Secimage config file
        self._secimage_config = None

        # Input files list
        self._input_files_list = []

        # Path to the output directory
        self._output_dir = None

        # Public properties
        self.debug = debug

        # Sign authority
        self.authority = None

    @classmethod
    def _feature_id(cls):
        return DEBUGPOLICY_ID

    @classmethod
    def is_plugin(cls):
        return True

    @property
    def dbgp_config(self):
        """(str) Path to the debugpolicy configuration file.
        """
        if self._config_parser is None:
            raise RuntimeError('debugpolicy config is not set')
        return self._config_parser.config_path

    @dbgp_config.setter
    def dbgp_config(self, config_path):
        config_path = c_path.normalize(config_path)
        if not c_path.validate_file(config_path):
            raise RuntimeError('debugpolicy config is inaccessible at given path: ' + str(config_path))
        self._config_parser = ConfigParser(config_path)

    @property
    def secimage_config(self):
        """(str) Secimage config path that is selected for the current run.
        """
        if self._secimage_config is None:
            raise RuntimeError('Secimage config is not set')
        return self._secimage_config

    @secimage_config.setter
    def secimage_config(self, config_path):
        config_path = c_path.normalize(config_path)
        if not c_path.validate_file(config_path):
            raise RuntimeError('Secimage config is inaccessible at given path: ' + str(config_path))
        self._secimage_config = config_path

    @property
    def input_file_list(self):
        """(str) List of input files.
        """
        return self._input_files_list

    def set_input_file(self, i_file):
        """(str) Path to the input debugpolicy elf file.
        """
        i_file = c_path.normalize(i_file)
        if not c_path.validate_file(i_file):
            raise RuntimeError('debugpolicy elf file is inaccessible at given path: ' + str(i_file))
        self._input_files_base = os.path.dirname(i_file)
        self._input_files_list = [i_file]

    @property
    def output_dir(self):
        """(str) Path to the output directory to use for saving logs, output
        files, debug_files.
        """
        if self._output_dir is None:
            raise RuntimeError('Output directory is not set')
        return self._output_dir

    @output_dir.setter
    def output_dir(self, output_dir):
        output_dir = c_path.normalize(output_dir)
        try:
            c_path.create_dir(output_dir)
        except Exception as e:
            raise RuntimeError('Cannot write in the given output_dir: ' + output_dir + '\n'
                               '    ' + 'Error: ' + str(e))
        self._output_dir = output_dir
        logger.info('Output dir is set to: ' + output_dir)

    def _verify_generate(self):
        """Verify that the current arguments are sufficient for generation of
        debugpolicy elf.
        :returns: Error strings.
        :rtype: list[str]
        """
        error_list = []
        if self._config_parser is None:
            error_list.append('Generation: Missing the debugpolicy config file.')
        if self._secimage_config is None:
            error_list.append('Generation: Missing the secimage config file.')
        if self._output_dir is None:
            error_list.append('Generation: Missing the output directory.')
        return error_list

    def _verify_sign(self, generate):
        """Verify that the current arguments are sufficient for signing of
        debugpolicy elf.
        :returns: Error strings.
        :rtype: list[str]
        """
        error_list = []
        if not generate:
            if not self._input_files_list:
                error_list.append('Signing: Missing the input debugpolicy elf files.')
            if self._output_dir is None:
                error_list.append('Signing: Missing the output directory.')
        return error_list

    def _verify_validate(self, generate, sign):
        """Verify that the current arguments are sufficient for validation of
        debugpolicy elf file.
        :returns: Error strings.
        :rtype: list[str]
        """
        return [err.replace('Signing', 'Validation') for err in self._verify_sign(generate)]

    def verify_setup(self, generate, sign, validate):
        """Verify that the current arguments are sufficient for validation of
        debugpolicy elf file.
        :returns: (Valid?, Error if not valid)
        :rtype: tuple (bool, str)
        """
        # Ensure the correct set of operations is provided
        if not (generate or sign or validate):
            raise RuntimeError('Please specify one or more operations to perform.')

        error_list = []
        if generate:
            error_list += self._verify_generate()
        if sign:
            error_list += self._verify_sign(generate)
        if validate:
            error_list += self._verify_validate(generate, sign)

        if error_list:
            raise RuntimeError('Setup is not complete:' + '\n'
                               '    ' + '\n    '.join(error_list))

    def generate(self, sign_id=None):
        """Generate the debugpolicy elf file based on the configs which have been setup.
        """
        # Set the debug params
        if self.debug:
            debug_dir = c_path.join(self.output_dir, defines.DEST_DEBUG_DIR)
            debug_prefix, debug_suffix = os.path.splitext(os.path.basename(c_path.join(self.output_dir, 'dp.dat')))
        else:
            debug_dir, debug_prefix, debug_suffix = None, None, None

        dbgpelfparser = ParseGenDbgpElf(version = self._config_parser.root.file_properties.revision,
                                        debug_dir = debug_dir,
                                        debug_prefix = debug_prefix,
                                        debug_suffix = debug_suffix,
                                        elf_class = self._config_parser.root.file_properties.elf.elf_class)

        # Set version number to be revision number
        dbgpelfparser.dbgpparser.header.version = self._config_parser.root.file_properties.revision

        # Set serial number start and end values
        if not (self._config_parser.root.file_properties.serial_number_start is None or \
                self._config_parser.root.file_properties.serial_number_end is None):
            self.versionAttrCheck(dbgpelfparser.dbgpparser.header, "set_sernum", ["serial_number_start", "serial_number_end"])
            dbgpelfparser.dbgpparser.header.set_sernum(self._config_parser.root.file_properties.serial_number_start, self._config_parser.root.file_properties.serial_number_end)

        # Validate flag bits and set flags
        self.flagBitsCheck(dbgpelfparser.dbgpparser.header.INVALID_FLAG_BITS, self._config_parser.root.file_properties.flags)
        dbgpelfparser.dbgpparser.header.flags = self._config_parser.root.file_properties.flags

        # Set imagebit_map
        if self._config_parser.root.file_properties.image_bit_map is not None:
            self.versionAttrCheck(dbgpelfparser.dbgpparser.header, "imagebit_map", "image_bit_map")
            dbgpelfparser.dbgpparser.header.imagebit_map = self._config_parser.root.file_properties.image_bit_map

        # Set Image ID Info
        if self._config_parser.root.file_properties.image_id_list is not None:
            self.versionAttrCheck(dbgpelfparser.dbgpparser.header, "imgid_array", "image_id_list")
            dbgpelfparser.dbgpparser.header.imgid_array = self._config_parser.root.file_properties.image_id_list.image_id

        # Initialize rootcerthash array
        dbgpelfparser.dbgpparser.header.rootcerthash_array = self._config_parser.root.file_properties.root_cert_hash_list.root_cert_hash

        # Set Serial Num List
        if self._config_parser.root.file_properties.serial_num_list is not None:
            self.versionAttrCheck(dbgpelfparser.dbgpparser.header, "serial_num_array", "serial_num_list")
            dbgpelfparser.dbgpparser.header.serial_num_array = self._config_parser.root.file_properties.serial_num_list.serial_num

        # Initialize rootcerthash_qti array
        if self._config_parser.root.file_properties.root_cert_hash_qti_list is not None:
            self.versionAttrCheck(dbgpelfparser.dbgpparser.header, "rootcerthash_qti_array", "root_cert_hash_qti_list")
            dbgpelfparser.dbgpparser.header.rootcerthash_qti_array = self._config_parser.root.file_properties.root_cert_hash_qti_list.root_cert_hash_qti

        # Set physical address
        dbgpelfparser.phys_addr = self._config_parser.root.file_properties.elf.phys_addr

        # Validate header version contents
        dbgpelfparser.dbgpparser.header.validate_ranges()

        dbgpelf_data = dbgpelfparser.get_data()
        dp_unsigned = c_path.join(self.output_dir, 'dp_unsigned.mbn')
        store_data_to_file(dp_unsigned, dbgpelf_data)
        logger.info('Generated unsigned debugpolicy elf file at: ' + dp_unsigned + ', date & time: ' + datetime.datetime.now().strftime('%c'))
        dp_unsigned_repr = c_path.join(self.output_dir, 'dbgpelf_repr.txt')
        store_data_to_file(dp_unsigned_repr, repr(dbgpelfparser))
        logger.info('Dumped debugpolicy elf repr at: ' + dp_unsigned_repr  + ', date & time: ' + datetime.datetime.now().strftime('%c'))

        self.sign(dp_unsigned, sign_id)

    def versionAttrCheck(self, versionObject, attribute, config_attr=None):
        try:
            func = getattr(versionObject, attribute)
        except AttributeError:
            if config_attr is None:
                raise RuntimeError("Debugpolicy version " + str(versionObject.version) + " does not support the field: " + str(attribute))
            raise RuntimeError("Debugpolicy version " + str(versionObject.version) + " does not support the fields: " + str(config_attr))

    def flagBitsCheck(self, invalid_flag_bits, flags):

        if flags > 0xFFFFFFFFFFFFFFFF:
            raise RuntimeError("The <flags> field only supports 64 bit positions. Bit_pos outside [0, 63] cannot be set.")

        # Define mask for dbgp version
        bitmask = flags & invalid_flag_bits
        invalid_flags = []
        bit_pos = 0

        # If restricted bit is set, add position to invalid flag list
        while(bitmask != 0):
            if bitmask & 1:
                invalid_flags.append(bit_pos)
            bit_pos += 1
            bitmask = bitmask >> 1

        # Go through invalid flag list and construct error message
        if invalid_flags:
            error_message = ''
            for bit_pos in invalid_flags:
                error_message = error_message + '\n' + 'Flag with bit_pos ' + str(bit_pos) + " is set but bit area is restricted."
            raise RuntimeError(error_message)

    def sign(self, dp_file, sign_id=None):
        """Sign the given debugpolicy elf file.
        """

        try:
            self.input_file_list.pop()
        except:
            pass

        # Initialize SecImageCore
        isc = SecImageCore(debug=self.debug)
        isc.config_path = self.secimage_config
        if self.authority is not None:
            isc.authority = self.authority

        # Get the supported sign_id list
        sign_id_list = isc._img_config_parser.sign_id_list

        # Generating signed images for sign_ids available in sign_id list
        if sign_id is None:
            for sign in range(len(sign_id_list)):
                isc.set_image_path(dp_file, sign_id_list[sign])
                isc.output_dir = os.path.dirname(dp_file)
                isc.image_info_list[0].dest_image.image_dir_ext = ''

                # Process the debug policy
                isc.process(sign=True)

                # Check if the signing is done or not
                if not isc.image_info_list[0].status.sign.state == isc.image_info_list[0].status.sign.SUCCESS:
                    raise RuntimeError('Failed to sign the debugpolicy elf file: ' + isc.image_info_list[0].image_under_operation)
                logger.info('Signed debugpolicy elf file at: ' + isc.image_info_list[0].image_under_operation)
                self.input_file_list.append(isc.image_info_list[0].image_under_operation)

                if os.path.isfile(c_path.join(self.output_dir, 'SecImage_log.txt')):
                    shutil.move(c_path.join(self.output_dir, 'SecImage_log.txt'), c_path.join(self.output_dir, (sign_id_list[sign]+'_secimage_log.txt')))

        # Generating signed image for given sign_id
        else:
            if sign_id not in sign_id_list:
                raise RuntimeError('Received sign_id "' + sign_id + '" as input.' + '\n'
                                    "       Supported sign_id's are: " + str(sign_id_list))

            isc.set_image_path(dp_file, sign_id)
            isc.output_dir = os.path.dirname(dp_file)
            isc.image_info_list[0].dest_image.image_dir_ext = ''

            # Process the debug policy
            isc.process(sign=True)

            # Check if the signing is done or not
            if not isc.image_info_list[0].status.sign.state == isc.image_info_list[0].status.sign.SUCCESS:
                raise RuntimeError('Failed to sign the debugpolicy elf file: ' + isc.image_info_list[0].image_under_operation)
            logger.info('Signed debugpolicy elf file at: ' + isc.image_info_list[0].image_under_operation)
            self.input_file_list.append(isc.image_info_list[0].image_under_operation)

            if os.path.isfile(c_path.join(self.output_dir, 'SecImage_log.txt')):
                shutil.move(c_path.join(self.output_dir, 'SecImage_log.txt'), c_path.join(self.output_dir, (sign_id + '_secimage_log.txt')))


    def _validate_generation(self, dp_file):
        """Validate the generation
        """
        reterr = []

        # Set the debug params
        if self.debug:
            debug_dir = c_path.join(self.output_dir, defines.DEST_DEBUG_DIR)
            debug_prefix, debug_suffix = os.path.splitext(os.path.basename(c_path.join(self.output_dir, 'dp.dat')))
        else:
            debug_dir, debug_prefix, debug_suffix = None, None, None

        dbgpelfparser = ParseGenDbgpElf(data = load_data_from_file(dp_file),
                                        version = self._config_parser.root.file_properties.revision,
                                        debug_dir = debug_dir,
                                        debug_prefix = debug_prefix,
                                        debug_suffix = debug_suffix,
                                        elf_class = None)

        # Validate against the debugpolicy config file
        if self._config_parser:
            if dbgpelfparser.dbgpparser.header.version != self._config_parser.root.file_properties.revision:
                reterr.append('Revision mismatch '
                              '' + 'Debugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.version) +
                              '' + ' Config File: ' + str(self._config_parser.root.file_properties.revision))
                logger.error(reterr)
                return

            if getattr(dbgpelfparser.dbgpparser.header, 'sernum_start', None) is not None and \
                dbgpelfparser.dbgpparser.header.sernum_start != self._config_parser.root.file_properties.serial_number_start:
                reterr.append('Serial Number Start mismatch '
                              '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.sernum_start) +
                              '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.serial_number_start))

            if getattr(dbgpelfparser.dbgpparser.header, 'sernum_end', None) is not None and \
                dbgpelfparser.dbgpparser.header.sernum_end != self._config_parser.root.file_properties.serial_number_end:
                reterr.append('Serial Number End mismatch '
                              '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.sernum_end) +
                              '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.serial_number_end))

            if dbgpelfparser.dbgpparser.header.flags != self._config_parser.root.file_properties.flags:
                reterr.append('Flags mismatch '
                              '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.flags) +
                              '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.flags))

            if self._config_parser.root.file_properties.image_bit_map is not None:
                if dbgpelfparser.dbgpparser.header.imagebit_map != self._config_parser.root.file_properties.image_bit_map:
                        reterr.append('Image Bit Map mismatch '
                                  '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.imagebit_map) +
                                  '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.image_bit_map))

            if self._config_parser.root.file_properties.image_bit_map is None:
                if getattr(dbgpelfparser.dbgpparser.header, 'imagebit_map', None) is not None:
                    if dbgpelfparser.dbgpparser.header.imagebit_map !=0:
                        if dbgpelfparser.dbgpparser.header.imagebit_map != self._config_parser.root.file_properties.image_bit_map:
                            reterr.append('Image Bit Map mismatch '
                                          '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.imagebit_map) +
                                          '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.image_bit_map))

            if self._config_parser.root.file_properties.image_id_list is not None:
                if len(self._config_parser.root.file_properties.image_id_list.image_id) == 0:
                    for i in range(len(list(dbgpelfparser.dbgpparser.header.imgid_array))):
                        if list(dbgpelfparser.dbgpparser.header.imgid_array)[i] != 0:
                            reterr.append('Image ID Array mismatch '
                                          '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.imgid_array) +
                                          '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.image_id_list.image_id + list([0] * (32-len(self._config_parser.root.file_properties.image_id_list.image_id)))))
                            break;
                else:
                    for i in range(len(self._config_parser.root.file_properties.image_id_list.image_id)):
                        if list(dbgpelfparser.dbgpparser.header.imgid_array)[i]!= self._config_parser.root.file_properties.image_id_list.image_id[i]:
                            reterr.append('Image ID Array mismatch '
                                          '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.imgid_array) +
                                          '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.image_id_list.image_id + list([0] * (32-len(self._config_parser.root.file_properties.image_id_list.image_id)))))
                            break;

            if self._config_parser.root.file_properties.image_id_list is None:
                if getattr(dbgpelfparser.dbgpparser.header, 'imgid_count', None) is not None:
                    if dbgpelfparser.dbgpparser.header.imgid_count!=0:
                        reterr.append('Image ID Array mismatch '
                                      '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.imgid_array) +
                                      '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.image_id_list))

            if dbgpelfparser.dbgpparser.header.rootcerthash_array[:dbgpelfparser.dbgpparser.header.rootcerthash_count] != \
                [x.lower() for x in self._config_parser.root.file_properties.root_cert_hash_list.root_cert_hash]:
                reterr.append('Root Certificate Hash Array mismatch '
                              '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.rootcerthash_array) +
                              '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.root_cert_hash_list.root_cert_hash))

            if getattr(dbgpelfparser.dbgpparser.header, 'serial_num_array', None) is not None:
                if len(self._config_parser.root.file_properties.serial_num_list.serial_num) == 0:
                        reterr.append('Serial Number Array mismatch '
                                      '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.serial_num_array) +
                                      '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.serial_num_list.serial_num + list([0] * (200-len(self._config_parser.root.file_properties.serial_num_list.serial_num)))))
                else:
                    for i in range(len(self._config_parser.root.file_properties.serial_num_list.serial_num)):
                        if list(dbgpelfparser.dbgpparser.header.serial_num_array)[i] != self._config_parser.root.file_properties.serial_num_list.serial_num[i]:
                            reterr.append('Serial Number Array mismatch '
                                          '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.serial_num_array) +
                                          '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.serial_num_list.serial_num + list([0] * (200-len(self._config_parser.root.file_properties.serial_num_list.serial_num)))))
                            break;

            if self._config_parser.root.file_properties.root_cert_hash_qti_list is not None:
                if len(self._config_parser.root.file_properties.root_cert_hash_qti_list.root_cert_hash_qti) == 0:
                    for i in range(len(list(dbgpelfparser.dbgpparser.header.rootcerthash_qti_array))):
                        if list(dbgpelfparser.dbgpparser.header.rootcerthash_qti_array)[i] != 0:
                            reterr.append('Root Certificate Hash QTI Array mismatch '
                                          '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.rootcerthash_qti_array) +
                                          '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.root_cert_hash_qti_list.root_cert_hash_qti + list([0] * (4 - len(self._config_parser.root.file_properties.root_cert_hash_qti_list.root_cert_hash_qti)))))
                            break;
                else:
                    for i in range(len(self._config_parser.root.file_properties.root_cert_hash_qti_list.root_cert_hash_qti)):
                        if list(dbgpelfparser.dbgpparser.header.rootcerthash_qti_array)[i].lower() != self._config_parser.root.file_properties.root_cert_hash_qti_list.root_cert_hash_qti[i].lower():
                            reterr.append('Root Certificate Hash QTI Array mismatch '
                                          '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.rootcerthash_qti_array) +
                                          '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.root_cert_hash_qti_list.root_cert_hash_qti + list([0] * (4 - len(self._config_parser.root.file_properties.root_cert_hash_qti_list.root_cert_hash_qti)))))
                            break;

            if self._config_parser.root.file_properties.root_cert_hash_qti_list is None:
                if getattr(dbgpelfparser.dbgpparser.header, 'rootcerthash_qti_count', None) is not None:
                    if dbgpelfparser.dbgpparser.header.rootcerthash_qti_count != 0:
                        reterr.append('Root Certificate Hash QTI Array mismatch '
                                      '' + ':- \nDebugpolicy File: ' + str(dbgpelfparser.dbgpparser.header.rootcerthash_qti_array) +
                                      '' + '\nConfig File: ' + str(self._config_parser.root.file_properties.root_cert_hash_qti_list))

        if reterr:
            if len(reterr) > 1:
                reterr = [('  ' + str(idx + 1) + '. ' + error) for idx, error in enumerate(reterr)]
                reterr = 'Following errors were found during validation:\n\n' + '\n'.join(reterr)
            else:
                reterr = reterr[0]
            logger.error(reterr)

        else:
            logger.info( 'Validation of debugpolicy file against debugpolicy config file is Successful!' )

    def _validate_sign(self, dp_file, rootcerthash=None, sign_id=None):
        """Validate the given debugpolicy elf file.
        """
        # Initialize SecImageCore
        isc = SecImageCore(debug=self.debug)
        isc.config_path = self.secimage_config
        if self.authority is not None:
            isc.authority = self.authority

        # Get the supported sign_id list
        sign_id_list = isc._img_config_parser.sign_id_list

        # Validating signed images & signed image(in case of one image) for sign_ids available in sign_id list
        if sign_id is None:
            if len(dp_file) > 1:
                for sign in range(len(dp_file)):
                    logger.info('\nValidating debugpolicy signed file against ' + sign_id_list[sign] + ' signature..')
                    isc.set_image_path(dp_file[sign], sign_id_list[sign])
                    isc.output_dir = os.path.dirname(dp_file[sign])
                    isc.image_info_list[0].dest_image.image_dir_ext = ''

                    # Process the signed elf
                    isc.process(val_sign=True, root_cert_hash=rootcerthash)

                    if os.path.isfile(c_path.join(self.output_dir, 'SecImage_log.txt')):
                        shutil.move(c_path.join(self.output_dir, 'SecImage_log.txt'), c_path.join(self.output_dir, ('val_' + sign_id_list[sign] +'_secimage_log.txt')))
            else:
                for sign in range(len(sign_id_list)):
                    logger.info('\nValidating debugpolicy signed file against ' + sign_id_list[sign] + ' signature..')
                    isc.set_image_path(dp_file[0], sign_id_list[sign])
                    isc.output_dir = os.path.dirname(dp_file[0])
                    isc.image_info_list[0].dest_image.image_dir_ext = ''

                    # Process the signed elf
                    isc.process(val_sign=True, root_cert_hash=rootcerthash)

                    if os.path.isfile(c_path.join(self.output_dir, 'SecImage_log.txt')):
                        shutil.move(c_path.join(self.output_dir, 'SecImage_log.txt'),
                                    c_path.join(self.output_dir, ('val_' + sign_id_list[sign] + '_secimage_log.txt')))

        # Validating signed image for given sign_id
        else:
            if sign_id not in sign_id_list:
                raise RuntimeError('Received sign_id "' + sign_id + '" as input.' + '\n'
                                    "       Supported sign_id's are: " + str(sign_id_list))

            logger.info('\nValidating debugpolicy signed file against ' + sign_id + ' signature..')
            isc.set_image_path(dp_file[0], sign_id)
            isc.output_dir = os.path.dirname(dp_file[0])
            isc.image_info_list[0].dest_image.image_dir_ext = ''

            # Process the signed elf
            isc.process(val_sign=True, root_cert_hash=rootcerthash)

            if os.path.isfile(c_path.join(self.output_dir, 'SecImage_log.txt')):
                shutil.move(c_path.join(self.output_dir, 'SecImage_log.txt'),
                            c_path.join(self.output_dir, ('val_' + sign_id + '_secimage_log.txt')))



    def validate(self, rootcerthash=None, sign_id=None):
        """Validate the given debugpolicy elf file.
        """
        self._validate_sign(self.input_file_list, rootcerthash, sign_id)
        logger.info('Validating debugpolicy file against debug policy config file..')
        self._validate_generation(self.input_file_list[0])

    def process(self, verify_setup=False, generate=False, sign=False, validate=False, rch=None, sign_id=None,
                progress_cb=PROGRESS_CB_PYPASS):
        """Performs the debugpolicy operations specified from the params.

        :param bool verify_setup: Verify that the configuration of the object
            is correct and return. This will ignore any other params.
        :param bool generate: Generate the debugpolicy elf using the config files.
        :param bool validate: Validate the debugpolicy elf
        :param cb progress_cb: Callback method to get a status update during
            processing.

            Callback method should have this format:
            ::
                def progress_cb(status_string, progress_percent):
                    \"""
                    :param str status_string: the current status.
                    :param int progress_percent: the progress (in percent)
                    \"""
                    ...

        """
        # Verify setup
        if verify_setup:
            self.verify_setup(generate, sign, validate)
            logger.note('The inputs provided (config, cmd args) are valid.')
            return

        # Total number of stages. This is used for progress notification via the
        # callback.
        _PROGRESS_TOTAL_STAGES = 3

        # Start processing debugpolicy
        progress = ProgressNotifier(1, progress_cb, _PROGRESS_TOTAL_STAGES)
        progress.status = 'Processing..'

        # Create debug dirs
        self.create_debug_dir(self.output_dir)

        # Generate debugpolicy elf
        if generate:
            self.generate(sign_id)
            progress.push()

        # Sign debugpolicy elf
        if sign:
            self.sign(self.input_file_list[0], sign_id)
            progress.push()

        # Validate debugpolicy file
        if validate:
            self.validate(rch, sign_id)
            progress.push()

        # End processing
        progress.complete()

    def create_debug_dir(self, output_dir):
        """Creates the debug directory structure.
        """
        if not self.debug:
            return

        if not output_dir:
            logger.note('Output directory not available to dump debug logs')
            return

        try:
            debug_dir = c_path.join(output_dir, defines.DEST_DEBUG_DIR)
            try:
                c_path.create_dir(debug_dir)
            except Exception as e:
                raise RuntimeError('Failed to create debug dir: ' + str(e))

        except Exception as e:
            logger.warning('Failed to store debug logs: ' + str(e))
