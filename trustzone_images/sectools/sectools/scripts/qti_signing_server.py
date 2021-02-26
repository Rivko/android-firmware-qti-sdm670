# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
""" This file launches the server for signing:
  1. Listen for client requests to sign
  2. Download the tosign package
  3. Generate the sig package
  4. Return the sig package
"""
import datetime
import os
import sys
import traceback

# Add sectools directory in path to allow sectools import
DIR_PATH = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(DIR_PATH, '..', '..'))

from sectools.common.crypto import cert, utils
from sectools.common.rpc import BinString
from sectools.common.rpc.http import HTTPServer
from sectools.common.utils import c_path
from sectools.common.utils.c_base import dynamicToolStatus, CoreOptionParser
from sectools.common.utils.c_logging import logger
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_3_0, MULTI_SERIAL_NUMBERS_TAG
from sectools.features.isc.api.func_container import launch_secimage
from sectools.features.isc.cfgparser.auto_gen_xml_config import complex_multi_serial_numbers
from sectools.features.isc.secimage.secimage_cfgparser import SecImageConfigParser
from sectools.features.isc.signer import signer_mgr
from sectools.features.isc.signer.qti_remote_signer import QtiRemoteSigner, CONFIG_STRUCTURE_GENERAL_PROPERTIES

CMD_ARG_TOOL_NAME = 'qti_signing_server'

# Name & version of the tool
QTI_SIGNING_SERVER_TOOL_NAME = 'QtiSigningServer'
QTI_SIGNING_SERVER_TOOL_VERSION = '1.1'

# Path definitions
QTI_SIGNING_SERVER_OUTPUT_DIR_NAME = CMD_ARG_TOOL_NAME + '_output'
QTI_SIGNING_SERVER_OUTPUT_DIR_PATH = c_path.join(dynamicToolStatus.toolDir, QTI_SIGNING_SERVER_OUTPUT_DIR_NAME)

# Version string
__version__ = QTI_SIGNING_SERVER_TOOL_NAME + ' ' + QTI_SIGNING_SERVER_TOOL_VERSION

# IP & PORT ADDRESS OF THE SERVER
DEFAULT_HOST = '0.0.0.0'
DEFAULT_PORT = 50050
DEFAULT_SIGNER = 'cass'
DEFAULT_CONFIG_NAME = 'server.xml'
DEFAULT_CONFIG_PATH = c_path.join(dynamicToolStatus.toolDir, DEFAULT_CONFIG_NAME)


class QtiSigningServerOptionParser(CoreOptionParser):
    """Parses and validates the command line arguments specific to QtiSigningServer."""

    @property
    def help_on_no_args(self):
        return False

    @property
    def c_description(self):
        """(str) Returns the description of the program."""
        return 'This program launches a http server to allow signing remotely with server configuration.'

    @property
    def c_version(self):
        """(str) Returns the version of the program."""
        return __version__

    def c_add_options(self):
        """Adds the command line args supported."""
        # Server configuration
        server_group = self.add_option_group('Server configuration')
        server_group.add_option('-i', '--host', metavar='<host_name>',
                                help='The hostname that the server should use to start with. DEFAULT: "' +
                                     DEFAULT_HOST + '"',
                                default=DEFAULT_HOST)
        server_group.add_option('-p', '--port', metavar='<port number>', type="int",
                                help='The port that the server should start on. DEFAULT: "' + str(DEFAULT_PORT) + '"',
                                default=DEFAULT_PORT)
        server_group.add_option('-c', '--config', metavar='<file_path>',
                                help='The secimage config that should be used for signing. DEFAULT: "./' +
                                     DEFAULT_CONFIG_NAME + '"',
                                default=DEFAULT_CONFIG_PATH)
        server_group.add_option('-s', '--signer', metavar='<signer_name>',
                                help='The signer that should be used for signing. DEFAULT: "' +
                                     str(DEFAULT_SIGNER) + '"',
                                default=DEFAULT_SIGNER)
        server_group.add_option('--cass_capability', metavar='<cass_capability>',
                                help='The CASS capability that should be used for signing.')

        # Specifying the output location
        output_group = self.add_option_group('Specifying output location')
        output_group.add_option('-o', '--output_dir', metavar='<dir>',
                                help='directory to store output files. DEFAULT: "./' +
                                     QTI_SIGNING_SERVER_OUTPUT_DIR_NAME + '"',
                                default=QTI_SIGNING_SERVER_OUTPUT_DIR_PATH)

    def c_validate(self):
        """Validates the command line args provided by the user.

        :raises: RuntimeError
        """
        args = self.parsed_args
        err = []

        # Check the signer is valid
        if args.signer not in signer_mgr.get_map().keys():
            err.append('Invalid signer: ' + args.signer)

        # Check and sanitize any file paths for read access
        for path in ['config']:
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


class QtiSigningServer(HTTPServer):
    """ Starts a http server
    """

    def __init__(self, host, port, signer, cass_capability, config, output_dir, verbose=False, debug=False, quiet=False):
        """Initializes the basic settings for SecImageAPIIntf and sets the server settings.
        """
        # Initialize the base
        HTTPServer.__init__(self, host, port)

        # Save other properties
        self.signer = signer
        self.cass_capability = cass_capability
        self.config = config
        self.output_dir = output_dir

        # Save the verbosity settings
        self.verbose, self.debug, self.quiet = verbose, debug, quiet

    def update_config(self, sign_id, signing_attrs, dest_config):
        cfg_path = self.config
        cfg = SecImageConfigParser(cfg_path)
        sign_id_config = cfg.get_config_for_sign_id(sign_id)
        sign_id_config.pil_split = False
        if signing_attrs['secboot_version'] == SECBOOT_VERSION_3_0:
            sign_id_config.image_type = 'cass_bin'
        else:
            sign_id_config.image_type = 'hash_to_sign'
        for key in CONFIG_STRUCTURE_GENERAL_PROPERTIES:
            if key in QtiRemoteSigner.SIGNING_ATTR_EXCLUDE:
                continue
            if key not in signing_attrs:
                logger.warning("Unrecognized signing attribute \"{0}\" received by QTI Signing Server".format(key))
                continue
            value = signing_attrs[key]
            if key in ['soc_vers'] and isinstance(value, list):
                value = ' '.join(value)
            if key == MULTI_SERIAL_NUMBERS_TAG:
                value = complex_multi_serial_numbers(value)
            setattr(sign_id_config.general_properties_overrides, key, value)
        cfg.generate(dest_config)

    @HTTPServer.expose
    def sign_hash(self, request):
        # Decode the signing request
        machine = str(request['machine'])
        sign_id = str(request['sign_id'])
        to_sign = str(request['to_sign'])
        signing_attrs = request['signing_attributes']
        authority = str(request['authority'])

        # Set return value
        retcode = 0
        errstr = ''
        sig_pkg = {}

        # Objects to be cleaned at the end
        output_dir = c_path.join(self.output_dir, datetime.datetime.now().strftime('%y_%m_%d-%H_%M_%S-%f') + '_' +
                                 machine.lower())
        c_path.create_dir(output_dir)

        try:
            # Save to_sign to a temp file
            to_sign_file = c_path.join(output_dir, 'to_sign.bin')
            c_path.store_data_to_file(to_sign_file, to_sign)

            # Update config file
            temp_config_file = c_path.join(output_dir, 'signature_package_config.xml')
            self.update_config(sign_id, signing_attrs, temp_config_file)

            # Launch secimage
            il = launch_secimage(config=temp_config_file, output_dir=output_dir,
                                 sign_id=sign_id, imagefile=to_sign_file,
                                 signer=self.signer, sign=True,
                                 verbose=self.verbose, debug=True, quiet=self.quiet, cfg_params={"cass_capability": self.cass_capability}, authority=authority)

            # Verify the signature package was generated
            sig_pkg['root'] = il[0].dest_image.debug_file_signer_root_cert
            sig_pkg['ca'] = il[0].dest_image.debug_file_signer_attestation_ca_cert
            sig_pkg['attest'] = il[0].dest_image.debug_file_signer_attestation_cert
            for k, v in sig_pkg.items():
                sig_pkg[k] = BinString(cert.get_cert_in_format(c_path.load_data_from_file(v), utils.FORMAT_DER))
            sig_pkg['signature'] = BinString(c_path.load_data_from_file(il[0].dest_image.debug_file_signer_signature))

        except Exception as e:
            retcode = 1
            errors = ['\nException while running secimage: ' + str(e)]
            last_error = logger.lastError
            if last_error:
                errors.append('Last logged error: ' + last_error)
            errstr = "\n".join(errors)

        finally:
            if not self.debug:
                c_path.clean_dir(output_dir, 1, recreate=False)

        # Set the signing response
        return {'retcode': retcode, 'errstr': errstr, 'sig_pkg': sig_pkg}


def main(args):
    """Parses the command line arguments, performs any basic operations based on
    the parsed arguments and starts processing.
    """
    # Log to file
    logger.log_to_file(QTI_SIGNING_SERVER_TOOL_NAME, args.output_dir)

    # Print the tool's launch command
    logger.debug('\n\n    QtiSigningServer launched as: "' + ' '.join(sys.argv) + '"\n')

    # Start the server
    server = QtiSigningServer(host=args.host, port=args.port, signer=args.signer, cass_capability=args.cass_capability, config=args.config,
                               output_dir=args.output_dir, verbose=args.verbose, debug=args.debug, quiet=args.quiet)
    server.start()


def parse_args(argv):
    return QtiSigningServerOptionParser(argv).parsed_args


def start(host=DEFAULT_HOST, port=DEFAULT_PORT,
          signer=DEFAULT_SIGNER, cass_capability=None,
          config=DEFAULT_CONFIG_PATH,
          output_dir=QTI_SIGNING_SERVER_OUTPUT_DIR_PATH):
    qti_signing_server = QtiSigningServer(
        host, port, signer, cass_capability,
        config, output_dir)
    qti_signing_server.start()


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
