# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""
#===============================================================================
#
# Qualcomm Technologies, Inc. is NOT responsible for any misusage of the example
# codes. OEMs should implement their own client and server infrastructure.
#
#===============================================================================

This file simulates the server signer:
  1. Listen for client requests to sign
  2. Download the tosign package from the shared location
  3. Generate the sig package
  4. Upload the sig package to the shared location

To use this file, please configure the SERVER_SETTINGS at the start of the
code and the TEST_SETTINGS at the end of the code, before executing this file.
"""

import json
import sys
import os
import shutil
import socket

# Add sectools directory in path to allow sectools import
DIR_PATH = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(DIR_PATH, '..'))

from sectools.features.isc.api.intf import SecImageAPIIntf

"""SERVER_SETTINGS: Following global variables define the server settings

1. HOST    : The IP address of the server to connect to.
2. PORT    : The port of the server to connect to for socket communication.

3. OUTPUT_DIR: A location to store the server output. This location will be used
               as a scratch area by sectools.

4. ACCEPT_SIGN_ATTRS: Flag to allow signing attributes from the client to be
                      used for signing instead of the server signing attributes.
                      Setting this flag to True will enable clients to set
                      "SEND_SIGN_ATTRS" to True.
"""
# PLEASE SET THE IP ADDRESS OF THE SERVER HERE
HOST = ''

# PLEASE SET THE PORT NUMBER OF THE SERVER HERE
PORT = 50050

# PLEASE SET THE PATH TO THE OUTPUT LOCATION OF THE SERVER
OUTPUT_DIR = os.path.join(DIR_PATH, 'server_output')

# PLEASE SET THIS SETTING TO ALLOW CLIENTS TO OVERRIDE THE SERVER SIGNING ATTRS
ACCEPT_SIGN_ATTRS = True

"""Following global variables are used for socket communication protocol."""
REQ = 'request'
REQ_INFO = 'request_info'
REQ_VAL_SIGN = 'sign'

RESP = 'response'
RESP_INFO = 'response_info'
RESP_VAL_INVALID = 'invalid'


class SampleServerSigner(SecImageAPIIntf):
    """Overrides the SecImageAPIIntf to allow server signing.
    """

    def __init__(self, verbose=False, debug=False, quiet=False):
        """Initializes the basic settings for SecImageAPIIntf, sets the
        server settings and creates the output location if it doesnt
        already exist.
        """
        # Initialize the base
        SecImageAPIIntf.__init__(self, verbose=verbose, debug=debug, quiet=quiet)

        # Set the server information
        self.host = ''
        self.port = PORT
        self.accept_signattrs = ACCEPT_SIGN_ATTRS

        # Create the network location if it doesnt exist
        self.output_dir = OUTPUT_DIR
        if not os.path.exists(self.output_dir):
            # Create the output location
            print 'Creating the output location: ' + str(self.output_dir)
            os.makedirs(self.output_dir)
        print 'Output location set to: ' + str(self.output_dir)

    def start_server(self):
        """Starts the socket server and listens & processes client requests
        for signing.
        """
        # Create a socket
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print 'Starting the server at host: ' + (self.host) + ' port: ' + str(self.port)
        s.bind((self.host, self.port))
        s.listen(1)

        while True:
            # Create a new connection
            print 'Listening for client requests...'
            conn, addr = s.accept()
            print 'Connected to: ' + str(addr)

            # Get the data
            request_data = conn.recv(10 * 1024)
            print 'Got message: ' + str(request_data)

            # Get the json object
            try:
                # Decode the request packet
                request_dict = json.loads(request_data)
                request = request_dict[REQ]
                request_info = request_dict[REQ_INFO]
                response = request
                response_info = {}

                try:
                    # Check if the request is to sign
                    if request == REQ_VAL_SIGN:
                        # Decode the signing request
                        hash_package_shared = str(request_info['hash_package'])

                        # Remove existing hash package
                        hash_package = os.path.join(self.output_dir,
                                                    os.path.basename(hash_package_shared))
                        if os.path.exists(hash_package):
                            print 'Removing existing hash package: ' + str(hash_package)
                            os.remove(hash_package)

                        # Download the hash package
                        print 'Downloading hash package:'
                        print '  from: ' + str(hash_package_shared)
                        print '  to: ' + str(hash_package)
                        shutil.copy(hash_package_shared, hash_package)

                        # Sign the hash package
                        print 'Generating the signature package'
                        retcode, errstr, sig_package_shared = \
                            self.generatesigpack(self.output_dir, hash_package,
                                                 self.accept_signattrs,
                                                 ** self._classmethod_kwargs)

                        # Upload the signature package
                        sig_package = ''
                        if retcode == 0:
                            print 'Generated the signature package successfully'

                            # Remove existing signature package
                            sig_package = os.path.join(os.path.dirname(hash_package_shared),
                                                       os.path.basename(sig_package_shared))
                            if os.path.exists(sig_package):
                                print 'Removing existing signature package: ' + str(sig_package)
                                os.remove(sig_package)

                            # Upload the signature package
                            print 'Uploading signature package:'
                            print '  from: ' + str(sig_package_shared)
                            print '  to: ' + str(sig_package)
                            shutil.copy(sig_package_shared, sig_package)
                        else:
                            print 'Failed to generate the signature package'

                        # Set the signing response
                        response_info['retcode'] = retcode
                        response_info['errstr'] = errstr
                        response_info['sig_package'] = sig_package

                    # Invalid request
                    else:
                        raise RuntimeError('Invalid request: ' + request)

                except Exception as e:
                    print e
                    response = RESP_VAL_INVALID

                # Create the response packet
                response_dict = {}
                response_dict[RESP] = response
                response_dict[RESP_INFO] = response_info
                response_data = json.dumps(response_dict)
                conn.sendall(response_data)
                print 'Signed and responded to client: ' + str(response_data)

            except Exception as e:
                print e

            finally:
                # Close the connection
                conn.close()


if __name__ == '__main__':
    """TEST_SETTINGS: Following define the testing settings
    """
    # Start the server
    server = SampleServerSigner(verbose=False, debug=False, quiet=False)
    server.start_server()

