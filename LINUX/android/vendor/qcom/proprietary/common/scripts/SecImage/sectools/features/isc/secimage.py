# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""Provides a command line interface to the services provided by isc.

.. data:: SECIMAGE_TOOL_NAME

    Name of the tool

.. data:: SECIMAGE_TOOL_VERSION

    Version of the tool

.. data:: DEF_SECIMAGE_OUTPUT_DIR_NAME

    Name of the default output directory

.. data:: DEF_SECIMAGE_OUTPUT_DIR_PATH

    Absolute path of the default output directory
"""

import optparse
import sys
import traceback

from sectools.common.utils import c_path
from sectools.common.utils.c_base import dynamicToolStatus, CoreOptionParser
from sectools.common.utils.c_config import CfgOverrides
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import TablePrinter
from sectools.features.isc import SecImageCore
from sectools.features.isc.cfgparser.defines import \
    CONFIG_STRUCTURE_GENERAL_PROPERTIES, SPECIAL_OVERRIDES
from sectools.features.isc.defines import AUTHORITY_QC


# Tool name for command arg
CMD_ARG_TOOL_NAME = 'secimage'

# Name & version of the tool
SECIMAGE_TOOL_NAME = 'SecImage'
SECIMAGE_TOOL_VERSION = '4.4'

# Path definitions
DEF_SECIMAGE_OUTPUT_DIR_NAME = CMD_ARG_TOOL_NAME + '_output'
DEF_SECIMAGE_OUTPUT_DIR_PATH = c_path.join(dynamicToolStatus.toolDir, DEF_SECIMAGE_OUTPUT_DIR_NAME)


__version__ = SECIMAGE_TOOL_NAME + ' ' + SECIMAGE_TOOL_VERSION


class SecImageParser(CoreOptionParser):
    """Parser for command line arguments supported by SecImage."""

    _LIST_TAG = 'LIST'

    @property
    def c_description(self):
        """(str) Returns the description of the program."""
        return 'This program signs, encrypts & validates images'

    @property
    def c_version(self):
        """(str) Returns the version of the program."""
        return __version__

    @property
    def init_overrides(self):
        return CfgOverrides(CONFIG_STRUCTURE_GENERAL_PROPERTIES)

    @property
    def init_spec_overrides(self):
        return CfgOverrides(SPECIAL_OVERRIDES)

    @property
    def override_prefix(self):
        """The prefix used for config overrides. """
        return 'cfg_'

    @property
    def spec_override_prefix(self):
        """The prefix used for special overrides. """
        return 'priv_'

    @property
    def c_epilog(self):
        """(str) Returns the epilog for the program."""
        prog = self.c_prog
        _list = self._LIST_TAG
        return ('\n'
                '  Advanced - Overriding properties:' + '\n'
                '    --cfg_<name_of_property>=<value_to_override_with>' + '\n' + '\n'
                '    For example:  --cfg_sw_id=0x00000002  would override sw_id with given value' + '\n'
                '    For a list of overridable properties, see extended usage below.' + '\n'
                '\n'
                'Extended Usage: ' + '\n'
                '1. To get a list of available chipsets: ' + '\n'
                '      ' + prog + ' -h --chipset ' + _list + '\n'
                '2. To get a list of available sign ids for a certain chipset: ' + '\n'
                '      ' + prog + ' -h --chipset <id> --sign_id ' + _list + '\n'
                '3. To get a list of available sign ids for a certain config file: ' + '\n'
                '      ' + prog + ' -h --config_path <path> --sign_id ' + _list + '\n'
                '4. To get a list of overridable properties: ' + '\n'
                '      ' + prog + ' -h --overrides ' + '\n')

    def c_add_options(self):
        """Adds the command line args supported by secimage."""
        # Signing individual image
        img_group = self.add_option_group('Signing individual image')
        img_group.add_option('--overrides', action='store_true', help=optparse.SUPPRESS_HELP)
        img_group.add_option('-i', '--image_file', metavar='<file>',
                             help='path to the image file.')
        img_group.add_option('-g', '--sign_id', metavar='<id>',
                             help='sign id corresponding to the image_file provided.')
        img_group.add_option('-p', '--chipset', metavar='<id>',
                             help='id of the chipset corresponding to the image_file.')
        img_group.add_option('-c', '--config_path', metavar='<file>',
                             help='path to the secimage config file.')
        img_group.add_option('--qc_signing', action='store_true',
                             default=False, help=optparse.SUPPRESS_HELP)

        # Signing metabuild
        meta_group = self.add_option_group('Signing images from metabuild')
        meta_group.add_option('-m', '--meta_build', metavar='<dir>',
                             help='path to the meta-build to be used for obtaining the images to sign.')

        # Specifying the output location
        output_group = self.add_option_group('Specifying output location')
        output_group.add_option('-o', '--output_dir', metavar='<dir>',
                                help='directory to store output files. DEFAULT: "./' + DEF_SECIMAGE_OUTPUT_DIR_NAME + '"',
                                default=DEF_SECIMAGE_OUTPUT_DIR_PATH)
        output_group.add_option('-n', '--mini_build', metavar='<dir>',
                                help='path to the minimized build to store the signed images to. '
                                'This option works with the meta_build option.')

        # Specifying the operation
        operations_group = self.add_option_group('Operations')
        operations_group.add_option('-t', '--integrity_check', action='store_true',
                                    default=False, help='add hash table segment.')
        operations_group.add_option('-s', '--sign', action='store_true',
                                    default=False, help='sign the image.')
        operations_group.add_option('-e', '--encrypt', action='store_true',
                                    default=False, help='encrypt the image.')
        operations_group.add_option('--decrypt', action='store_true',
                                    default=False, help=optparse.SUPPRESS_HELP)
        operations_group.add_option('-a', '--validate', action='store_true',
                                    default=False, help='validate the image.')
        operations_group.add_option('-l', '--verify_inputs', action='store_true',
                                    default=False,
                                    help='verify the command line options.')

        # Other options
        other_group = self.add_option_group('Other options')
        other_group.add_option('--rch', '--root_cert_hash', metavar='<hash>',
                               help='root cert hash to validate the signed image cert chain against. '
                               'This option can only be used with the validate operation.')

        # Populate override options
        for tag, override in self.overrides.get_properties().items():
            operations_group.add_option('--' + self.override_prefix + tag,
                                        metavar='<' + override.type_str + '_value>',
                                        help=optparse.SUPPRESS_HELP)

        # Populate special override options
        for tag, override in self.spec_overrides.get_properties().items():
            operations_group.add_option('--' + self.spec_override_prefix + tag,
                                        metavar='<' + override.type_str + '_value>',
                                        help=optparse.SUPPRESS_HELP)

    @property
    def c_epilog_from_params(self):
        """(str) Based on the arguments provided, return the extended epilog
        message.
        """
        args = self.parsed_args
        help_obj = SecImageCore()
        epilog = ''

        # Extended Feature 1
        if args.chipset == self._LIST_TAG:
            # Compile a list of available chipsets
            epilog += '\n' + 'Chipsets available: ' + '\n'
            for idx, chipset in enumerate(help_obj.available_chipsets):
                epilog += str(idx + 1) + '. ' + chipset + '\n'

        # Extended Features 2 & 3
        elif (args.sign_id == self._LIST_TAG and
                args.chipset != self._LIST_TAG and
                args.config_path != self._LIST_TAG):

            # Extended Feature 2
            if args.chipset:
                # Compile a list of available sign ids for a chipset
                help_obj.chipset = args.chipset
                epilog += '\n' + 'Sign-ids available for chipset "' + args.chipset + '": ' + '\n'
                for idx, sign_id in enumerate(help_obj.available_sign_ids):
                    epilog += str(idx + 1) + '. ' + sign_id + '\n'

            # Extended Feature 3
            elif args.config_path:
                # Compile a list of available sign ids for a config file
                help_obj.config_path = args.config_path
                epilog += '\n' + 'Sign-ids available for config_path "' + args.config_path + '": ' + '\n'
                for idx, sign_id in enumerate(help_obj.available_sign_ids):
                    epilog += str(idx + 1) + '. ' + sign_id + '\n'

        # Extended Feature 4
        elif args.overrides:
            cert_configs = '\n    - ' + '\n    - '.join(
                            ['qc_presigned_certs',
                            'default',
                            'mrc_presigned_certs',
                            'sha1_certs',
                            'sha1_mrc_certs',
                            'mrc_certs_manual',
                            'rot_presigned_certs',
                            ])

            # This should always contain all the available options
            order = [('selected_signer', ''),
                     ('selected_encryptor', ''),
                     ('selected_cert_config', cert_configs),
                     ('cass_capability', ''),
                     ('hash_algorithm', ''),
                     ('segment_hash_algorithm', ''),
                     ('cass_attest_cert', ''),
                     ('', ''),
                     ('msm_part', ''),
                     ('oem_id', ''),
                     ('model_id', ''),
                     ('debug', ''),
                     ('sw_id', ''),
                     ('', ''),
                     ('key_size', ''),
                     ('exponent', ''),
                     ('', ''),
                     ('dsa_type', ''),
                     ('ecdsa_curve', ''),
                     ('', ''),
                     ('app_id', ''),
                     ('crash_dump', ''),
                     ('rot_en', ''),
                     ('', ''),
                     ('num_root_certs', '[1-16]'),
                     ('mrc_index', '[0-15]'),
                     ('', ''),
                     ('max_cert_size', ''),
                     ('num_certs_in_certchain', ''),
                     ('', ''),
                     ('soc_hw_version', ''),
                     ('soc_vers', "ex \"0x0001 0x0002 0x0003\""),
                     ('in_use_soc_hw_version', ''),
                     ('use_serial_number_in_signing', ''),
                     ('serial_number', ''),
                     ('oem_id_independent', ''),
                     ('', ''),
                     ('testsig_serialnum', ''),
                     ('hash_pageseg_as_segment', ''),
                     ('hmac', ''),
                     ('rsa_padding', ''),
                     ('', ''),
                     ('secboot_version', ''),
                     ('qc_sign', ''),
                     ('oem_sign', ''),
                     ('', ''),
                     ('UIE_key', ''),
                     ('UIE_capability', ''),
                     ('UIE_root_key_type', ''),
                     ('', ''),
                     ('revocation_enablement', ''),
                     ('activation_enablement', ''),
                     ]

            properties = self.overrides.get_properties()

            # Remove attributes excluded from order list
            del properties['mask_soc_hw_version']

            # Check all options are correct
            for tag in properties.keys():
                if not tag in [t for t, m in order]:
                    raise RuntimeError(tag + ' is not provided in user interface order.')
            for tag, msg in order:
                if tag and not tag in properties.keys():
                    raise RuntimeError(tag + ' is extra in user interface order.')

            # Update the epilog
            epilog += ('Supported Parameters: ' + '\n\n')

            # Available options
            idx = 1
            for tag, msg in order:
                if tag:
                    override = properties[tag]
                    # Start of the option string
                    start = ('%2d' % idx) + '. ' + self.override_prefix + tag + ' (' + override.type_str + ')'

                    # Help message
                    msg = ' : ' + msg if msg else ''

                    # Choices
                    choices = override.choices
                    if () in choices:
                        choices.remove(())
                    choices_string = ' : ' + str(choices) if choices else ''
                    if len(start + choices_string) > 80:
                        choices_string = ' :\n    - ' + '\n    - '.join(choices)

                    # Put all together
                    epilog += start + msg + choices_string + '\n'
                    idx += 1
                else:
                    epilog += '\n'

            epilog += ('\n\nSpecial Parameters: ' + '\n\n')

            for tag, override in self.spec_overrides.get_properties().items():
                #Start of the option string
                epilog += ('%2d' % idx) + '. ' + self.spec_override_prefix + tag + ' (' + override.type_str + ')\n'
                idx += 1

        return epilog

    def c_validate(self):
        """Validates the command line args provided by the user.

        :raises: RuntimeError if any error occurs.
        """
        args = self.parsed_args
        err = []

        # Check if the meta build supports sign id
        meta_supports_sign_id = False
        if args.meta_build:
            meta_supports_sign_id = SecImageCore.meta_supports_sign_id(args.meta_build)

        # Check the input files
        if ((args.image_file and args.meta_build) or
                (not args.image_file and not args.meta_build)):
            err.append('Provide either image_file or a meta_build for processing.')

        # Check that --override flag is not given without help flag
        if args.overrides and not args.help:
            err.append('-h flag must accompany --overrides flag to view overridable properties')

        # Check the configuration option and output dir
        if args.image_file or (args.meta_build and not meta_supports_sign_id):
            if ((args.chipset and args.config_path) or
                    (not args.chipset and not args.config_path)):
                err.append('Provide either chipset or a config_path to process images.')
            if not args.output_dir:
                err.append('Provide the output_dir for storing the output.')
        elif args.meta_build and not meta_supports_sign_id:
            if not args.output_dir and not args.mini_build:
                err.append('Provide either output_dir or mini_build for storing the output.')

        # Check the operations
        if not (args.integrity_check or args.sign or args.encrypt or
                args.decrypt or args.validate or args.verify_inputs):
            err.append('Specify one or more operations to perform.')

        # Check other options:
        if args.rch and not args.validate:
            err.append('Root Cert Hash can only be given when "--validate" operation is provided.')

        # Check and sanitize any paths for read access
        for path in ['image_file', 'config_path']:
            path_val = getattr(args, path, None)
            if path_val:
                path_val = c_path.normalize(path_val)
                if not c_path.validate_file(path_val):
                    err.append('Cannot access ' + path + ' at: ' + path_val)
                setattr(args, path, path_val)

        # Check and sanitize any paths for read dir access
        for path in ['meta_build']:
            path_val = getattr(args, path, None)
            if path_val:
                path_val = c_path.normalize(path_val)
                if not c_path.validate_dir(path_val):
                    err.append('Cannot access ' + path + ' at: ' + path_val)
                setattr(args, path, path_val)

        # Check and sanitize paths for write access
        for path in ['output_dir', 'mini_build']:
            path_val = getattr(args, path, None)
            if path_val:
                path_val = c_path.normalize(path_val)
                try:
                    c_path.create_dir(path_val)
                except Exception as e:
                    err.append('Cannot write at: ' + path_val + '\n'
                               '    ' + 'Error: ' + str(e))
                setattr(args, path, path_val)

        # Raise error if any
        if err:
            if len(err) > 1:
                err = [('  ' + str(idx + 1) + '. ' + error) for idx, error in enumerate(err)]
                err = 'Please check the command line args:\n\n' + '\n'.join(err)
            else:
                err = err[0]
            raise RuntimeError(err)

    def c_update_overrides(self):
        # Update override options
        for tag, override in self.overrides.root.children.items():
            if not override.is_simple:
                continue
            value_to_set = getattr(self.parsed_args, self.override_prefix + tag)
            if value_to_set is not None:
                try:
                    override.set_string_value(value_to_set)
                except RuntimeError as e:
                    raise RuntimeError(tag + ' ' + str(e))

        # Update special overrides
        for tag, override in self.spec_overrides.root.children.items():
            value_to_set = getattr(self.parsed_args, self.spec_override_prefix + tag)
            if value_to_set is not None:
                try:
                    override.set_string_value(value_to_set)
                except RuntimeError as e:
                    raise RuntimeError(tag + ' ' + str(e))

def print_summary(args, image_info_list):
    """Prints the summary of the actions performed by SecImage"""
    actions = []

    # Check which actions were performed
    if args.integrity_check:
        actions.append('integrity_check')
    if args.sign:
        actions.append('sign')
    if args.encrypt:
        actions.append('encrypt')
    if args.validate:
        actions.append('validate')
    if not actions:
        return

    # Figure out the output directory
    output_print = (('Output is saved at: ' + args.output_dir + '\n') if args.output_dir else
                    ('Minimized build was updated at: ' + args.mini_build + '\n') if args.mini_build else
                    '\n')

    # Log the actions and output directory
    logger.info('SUMMARY:' + '\n' +
                'Following actions were performed: "' + ', '.join(actions) + '"' + '\n' +
                output_print)

    # Table information
    summary_table = TablePrinter([1])
    # summary_table = TablePrinter([0])
    COLUMN_IDX = 0
    COLUMN_SIGN_ID = 1
    COLUMN_PARSE = 2
    COLUMN_INTEGRITY_CHECK = 3
    COLUMN_SIGN = 4
    COLUMN_ENCRYPT = 5
    COLUMN_VAL_PARSE = 6
    COLUMN_VAL_INTEGRITY_CHECK = 7
    COLUMN_VAL_SIGN = 8
    COLUMN_VAL_ENCRYPT = 9

    # First row
    summary_table.insert_data(0, COLUMN_IDX, 'Idx')
    summary_table.insert_data(0, COLUMN_SIGN_ID, 'SignId')
    summary_table.insert_data(0, COLUMN_PARSE, 'Parse')
    summary_table.insert_data(0, COLUMN_INTEGRITY_CHECK, 'Integrity')
    summary_table.insert_data(0, COLUMN_SIGN, 'Sign')
    summary_table.insert_data(0, COLUMN_ENCRYPT, 'Encrypt')
    summary_table.insert_data(0, COLUMN_VAL_PARSE, 'Validate', column_end=COLUMN_VAL_ENCRYPT)

    # Second row
    summary_table.insert_data(1, COLUMN_VAL_PARSE, 'Parse')
    summary_table.insert_data(1, COLUMN_VAL_INTEGRITY_CHECK, 'Integrity')
    summary_table.insert_data(1, COLUMN_VAL_SIGN, 'Sign')
    summary_table.insert_data(1, COLUMN_VAL_ENCRYPT, 'Encrypt')

    # Data rows
    for idx, image in enumerate(image_info_list):
        idx += 2
        summary_table.insert_data(idx, COLUMN_IDX, str(idx - 1) + '.')
        summary_table.insert_data(idx, COLUMN_SIGN_ID, image.sign_id)
        summary_table.insert_data(idx, COLUMN_PARSE, image.status.parsegen.state)
        summary_table.insert_data(idx, COLUMN_INTEGRITY_CHECK, image.status.integrity_check.state)
        summary_table.insert_data(idx, COLUMN_SIGN, image.status.sign.state)
        summary_table.insert_data(idx, COLUMN_ENCRYPT, image.status.encrypt.state)
        summary_table.insert_data(idx, COLUMN_VAL_PARSE, image.status.validate_parsegen.state)
        summary_table.insert_data(idx, COLUMN_VAL_INTEGRITY_CHECK, image.status.validate_integrity_check.state)
        summary_table.insert_data(idx, COLUMN_VAL_SIGN, image.status.validate_sign.state)
        summary_table.insert_data(idx, COLUMN_VAL_ENCRYPT, image.status.validate_encrypt.state)

    # TODO: put the error and image paths

    logger.info(summary_table.get_data())

def main(args, return_isc=False):
    """Parses the command line arguments, performs any basic operations based on
    the parsed arguments and starts processing using the isc module.
    """
    # Log to file
    flids = logger.log_to_file(SECIMAGE_TOOL_NAME, args.output_dir)

    try:
        # Print the tool's launch command
        logged_args = CoreOptionParser.mask_private_args(sys.argv, args._c_spec_override_prefix)
        logger.info('\n\n    SecImage launched as: "' + ' '.join(logged_args) + '"\n')

        # Initialize SecImageCore
        isc = SecImageCore(debug=args.debug)

        # Configure image signer
        if args.image_file or (args.meta_build and not
                               SecImageCore.meta_supports_sign_id(args.meta_build)):
            if args.chipset:
                isc.set_chipset(args.chipset, args._c_overrides, args._c_spec_overrides)
            elif args.config_path:
                isc.set_config_path(args.config_path, args._c_overrides, args._c_spec_overrides)

        # Set the input
        if args.image_file:
            isc.set_image_path(args.image_file, args.sign_id)
        elif args.meta_build:
            isc.set_meta_build_path(args.meta_build, [] if args.sign_id is None else [args.sign_id])

        # Set the output
        if args.mini_build:
            isc.mini_build_path = args.mini_build
        elif args.output_dir:
            isc.output_dir = args.output_dir

        if args.qc_signing:
            isc.authority = AUTHORITY_QC

        # Process the images
        isc.process(verify_setup=args.verify_inputs,
                    integrity_check=args.integrity_check,
                    sign=args.sign,
                    encrypt=args.encrypt,
                    decrypt=args.decrypt,
                    val_image=args.validate,
                    val_integrity_check=args.validate,
                    val_sign=args.validate,
                    val_encrypt=args.validate,
                    root_cert_hash=args.rch)

        # Print the summary
        print_summary(args, isc.image_info_list)

        if return_isc:
            return isc
        else:
            return isc.image_info_list

    finally:
        # Clear all log handlers
        logger.removeFileLogger(flids)


def parse_args(argv):
    parser = SecImageParser(argv)
    setattr(parser.parsed_args, '_c_overrides', parser.overrides)
    setattr(parser.parsed_args, '_c_spec_overrides', parser.spec_overrides)
    setattr(parser.parsed_args, '_c_spec_override_prefix', parser.spec_override_prefix)
    return parser.parsed_args

if __name__ == '__main__':
    try:
        # Call the main of the tool
        main(parse_args(sys.argv))

    except Exception:
        logger.debug(traceback.format_exc())
        logger.error(sys.exc_info()[1])
        sys.exit(1)

    except KeyboardInterrupt:
        print
        logger.error('Keyboard Interrupt Received. Exiting!')
        sys.exit(1)

    sys.exit(0)

