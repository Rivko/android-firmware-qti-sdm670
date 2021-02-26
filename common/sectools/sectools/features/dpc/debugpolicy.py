# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""Provides a command line interface to the services provided by debugpolicy.

.. data:: DEBUGPOLICY_TOOL_NAME

    Name of the tool

.. data:: DEBUGPOLICY_TOOL_VERSION

    Version of the tool

.. data:: DEBUGPOLICY_OUTPUT_DIR_NAME

    Name of the default output directory

.. data:: DEBUGPOLICY_OUTPUT_DIR_PATH

    Absolute path of the default output directory
"""
# Tool name for command arg

import sys
import traceback
import optparse

from sectools.common.utils import c_path
from sectools.common.utils.c_base import dynamicToolStatus, CoreOptionParser
from sectools.common.utils.c_logging import logger
from sectools.features.dpc.cfgparsegen.configfile import get_chipset_mode_config
from sectools.features.dpc.dpc import DEBUGPOLICY_ID, DebugPolicyCore
from sectools.features.isc.defines import AUTHORITY_QTI


CMD_ARG_TOOL_NAME = DEBUGPOLICY_ID

# Name & version of the tool
DEBUGPOLICY_TOOL_NAME = 'DebugPolicy'
DEBUGPOLICY_TOOL_VERSION = '4.0'

# Path definitions
DEBUGPOLICY_OUTPUT_DIR_NAME = CMD_ARG_TOOL_NAME + '_output'
DEBUGPOLICY_OUTPUT_DIR_PATH = c_path.join(dynamicToolStatus.toolDir, DEBUGPOLICY_OUTPUT_DIR_NAME)

__version__ = DEBUGPOLICY_TOOL_NAME + ' ' + DEBUGPOLICY_TOOL_VERSION


class DebugPolicyOptionParser(CoreOptionParser):
    """Parses and validates the command line arguments specific to Debugpolicy."""

    @property
    def c_description(self):
        """(str) Returns the description of the program."""
        return 'This program generates, signs & validates the Debug Policy files.'

    @property
    def c_version(self):
        """(str) Returns the version of the program."""
        return __version__

    def c_add_options(self):
        """Adds the command line args supported by DebugPolicy."""
        # Generating debugpolicy elf
        gen_group = self.add_option_group('Generating/Validating Debugpolicy files')
        gen_group.add_option('-c', '--dbgp_config_path', metavar='<file>',
                             help='path to the debugpolicy config file.')
        gen_group.add_option('-p', '--platform', metavar='<platform>',
                             help='specify the platform, to get the config files as input related to platform')

        # Signing debugpolicy elf
        sign_group = self.add_option_group('Signing/Validating debugpolicy elf')
        sign_group.add_option('-f', '--input_file', metavar='<file>',
                              help='The unsigned/signed debugpolicy elf file to sign, validate.')
        sign_group.add_option('-e', '--secimage_config_path', metavar='<file>',
                             help='path to the dbgp secimage config file.')
        sign_group.add_option('-i', '--sign_id', metavar='<id>',
                             help='sign id corresponding to the image_file provided.')

        # Specifying the output location
        output_group = self.add_option_group('Specifying output location')
        output_group.add_option('-o', '--output_dir', metavar='<dir>',
                                help='directory to store output files. DEFAULT: "./' + DEBUGPOLICY_OUTPUT_DIR_NAME + '"',
                                default=DEBUGPOLICY_OUTPUT_DIR_PATH)

        # Specifying the operation
        operations_group = self.add_option_group('Operations')
        operations_group.add_option('-g', '--generate', action='store_true',
                                    default=False, help='generates the unsgined and signed(AP & MSA) debugpolicy elf files.')
        operations_group.add_option('-s', '--sign', action='store_true',
                                    default=False, help='signs the unsigned/signed debugpolicy elf file.')
        operations_group.add_option('-a', '--validate', action='store_true',
                                    default=False, help='validate the unsigned/signed debugpolicy elf file.')
        operations_group.add_option('-l', '--verify_inputs', action='store_true',
                                    default=False, help='verify the command line options.')
        operations_group.add_option('--qti_signing', action='store_true',
                                    default=False, help=optparse.SUPPRESS_HELP)

        # Other options
        other_group = self.add_option_group('Other options')
        other_group.add_option('--rch', '--root_cert_hash', metavar='<hash>',
                               help='root cert hash to validate the signed image cert chain against. '
                               'This option can only be used with the validate operation.')

    def c_validate(self):
        """Validates the command line args provided by the user.

        :raises: RuntimeError
        """
        args = self.parsed_args
        err = []

        # Check the operations
        if not (args.generate or args.sign or args.validate or args.verify_inputs):
            err.append('Please specify one or more operations to perform.')

        if args.generate and not ((args.platform) or (args.dbgp_config_path and args.secimage_config_path)):
            err.append('Please provide both debugpolicy config file and dbgp secimage config file OR\
                        \n provide the platform details using -p <platform> for generation.')

        elif args.generate and args.platform and (args.dbgp_config_path or args.secimage_config_path):
            err.append('Please provide either config files or platform as a input. '
                       'Please do not provide both types of input for generation.')

        elif args.sign and not (args.generate or (args.input_file and (args.secimage_config_path or args.platform))):
            err.append('Please provide both unsigned/signed debugpolicy elf file and dbgp secimage config file OR\
                        \n provide both unsigned/signed debugpolicy elf file and platform details using -p <platform> for signing/resigning.')

        elif args.sign and (args.secimage_config_path and args.platform):
            err.append('Please provide either config files or platform as a input. '
                        'Please do not provide both types of input for signing/resigning.')

        elif args.validate and not (args.generate or args.sign or args.input_file):
            err.append('Please specify the unsigned/signed debugpolicy elf file for validation.')

        elif args.validate and not ((args.platform) or (args.dbgp_config_path and args.secimage_config_path)):
            err.append('Please provide both debugpolicy config file and dbgp secimage config file OR\
                        \n provide the platform details using -p <platform> for validation.')

        elif args.validate and args.platform and (args.dbgp_config_path or args.secimage_config_path):
            err.append('Please provide either config files or platform as a input. '
                       'Please do not provide both types of input for validation.')

        # Check other options:
        elif args.rch and not args.validate:
            err.append('Root Cert Hash can only be given when "--validate" operation is provided.')

        elif args.generate and args.sign:
            args.sign = False

        # Check and sanitize any file paths for read access
        for path in ['dbgp_config_path', 'input_file', 'secimage_config_path']:
            path_val = getattr(args, path, None)
            if path_val:
                path_val = c_path.normalize(path_val)
                if not c_path.validate_file(path_val):
                    err.append('Cannot access ' + path + ' at: ' + path_val)
                setattr(args, path, path_val)

        # Check and sanitize paths for write access
        for path in ['output_dir']:
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

def log_to_file(folder):
    """Configures the logger to log to filesystem

    :param str folder: Directory to generate the logs in.
    """
    folder = c_path.normalize(folder)
    try:
        c_path.create_dir(folder)
    except Exception as e:
        raise RuntimeError('Unable to create directory for logging: ' + folder + '\n'
                           '    ' + 'Error: ' + str(e))
    logger.enable_file_logging(DEBUGPOLICY_TOOL_NAME, num_logs=1, log_dir=folder)

def print_summary():
    """Prints the summary of the actions performed by DebugPolicy"""
    # TODO: Print the summary
    pass

def main(args):
    """Parses the command line arguments, performs any basic operations based on
    the parsed arguments and starts processing using the dpc module.
    """
    # Log to file
    log_to_file(args.output_dir)

    # Print the tool's launch command
    logger.debug('\n\n    DebugPolicy launched as: "' + ' '.join(sys.argv) + '"\n')

    # Initialize DebugPolicyCore
    dpc = DebugPolicyCore(debug=args.debug)

    # Configure DebugPolicyCore
    if args.dbgp_config_path:
        dpc.dbgp_config = args.dbgp_config_path

    if args.secimage_config_path:
        dpc.secimage_config = args.secimage_config_path

    if args.platform:
        dpc.dbgp_config, dpc.secimage_config = get_chipset_mode_config(args.platform)

    if args.input_file:
        dpc.set_input_file(args.input_file)

    if args.output_dir:
        dpc.output_dir = args.output_dir

    if args.qti_signing:
        dpc.authority = AUTHORITY_QTI

    # Process
    dpc.process(verify_setup=args.verify_inputs,
                 generate=args.generate,
                 sign=args.sign,
                 validate=args.validate,
                 rch=args.rch,
                 sign_id=args.sign_id)

    # Print summary of operations
    print_summary()

def parse_args(argv):
    return DebugPolicyOptionParser(argv).parsed_args

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


