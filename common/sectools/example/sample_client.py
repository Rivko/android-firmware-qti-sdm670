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

This file simulates the client signer:
  1. Create tosign package
  2. Upload tosign package to the shared location
  3. Trigger server to sign with server hosted keys
  4. Download sig package from the shared location
  5. Generate signed image with sig package

To use this file, please configure the SERVER_SETTINGS at the start of the
code and the TEST_SETTINGS at the end of the code, before executing this file.
"""

import json
import os
import shutil
import socket
import sys

# Add sectools directory in path to allow sectools import
DIR_PATH = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(DIR_PATH, '..'))

from sectools.features.isc.api.intf import SecImageAPIIntf

"""SERVER_SETTINGS: Following global variables define the server settings

1. HOST    : The IP address of the server to connect to.
2. PORT    : The port of the server to connect to for socket communication.

3. SHARED_LOCATION: A network location where both the client and the server have
                    write permissions. This is the location where the hash
                    package and the signature package will be exchanged
                    between the server and the client.

4. SEND_SIGN_ATTRS: Flag to send signing attributes from the client to the
                    server. Setting this flag to True will override the
                    server signing attributes with the client signing
                    attributes. Setting this flag to True requires that the
                    server's "ACCEPT_SIGN_ATTRS" is also set to True. Otherwise,
                    error will be returned from server on signing requests.
"""
# PLEASE SET THE IP ADDRESS OF THE SERVER HERE
HOST = 'localhost'

# PLEASE SET THE PORT NUMBER OF THE SERVER HERE
PORT = 50050

# PLEASE SET THE PATH TO THE LOCATION SHARED WITH THE SERVER
SHARED_LOCATION = os.path.join(os.path.abspath(DIR_PATH), 'shared')

# PLEASE SET THIS SETTING AS PER THE SERVER SETTING FOR "ACCEPT_SIGN_ATTRS"
SEND_SIGN_ATTRS = True

"""Following global variables are used for socket communication protocol."""
REQ = 'request'
REQ_INFO = 'request_info'
REQ_VAL_SIGN = 'sign'

RESP = 'response'
RESP_INFO = 'response_info'
RESP_VAL_INVALID = 'invalid'


class SampleClientSigner(SecImageAPIIntf):
    """Overrides the SecImageAPIIntf to allow one-shot signing of image files.
    """

    def __init__(self, verbose=False, debug=False, quiet=False):
        """Initializes the basic settings for SecImageAPIIntf, sets the
        server settings and creates the shared network location if it doesnt
        already exist.
        """
        # Initialize the base
        SecImageAPIIntf.__init__(self, send_signattrs=SEND_SIGN_ATTRS,
                                 verbose=verbose, debug=debug, quiet=quiet)

        # Set the server information
        self.host = HOST
        self.port = PORT

        # Create the network location if it doesnt exist
        self.shared_location = SHARED_LOCATION
        if not os.path.exists(self.shared_location):
            # Create the shared location
            print 'Creating the shared location: ' + str(self.shared_location)
            os.makedirs(self.shared_location)
        print 'Shared location set to: ' + str(self.shared_location)

    def signwithserver(self, chipset, output_dir, sign_id, hash_package,
                       verbose=False, debug=False, quiet=False):
        """Signs the hash package with the server. See documentation from
        SecImageAPIIntf.signwithserver for API information.

        1. Uploads the hash package to the server by copying the file.
        2. Sends a signing request message to the server over socket.
        3. Downloads the signature package from the server.
        """
        retcode = 0
        errstr = ''
        sig_package = ''

        try:
            # Create a directory to share this package
            hash_package_shared_dir = os.path.join(self.shared_location, sign_id)
            if not os.path.exists(hash_package_shared_dir):
                print 'Creating directory for sharing hash package: ' + str(hash_package_shared_dir)
                os.makedirs(hash_package_shared_dir)

            # Remove existing hash package
            hash_package_shared = os.path.join(hash_package_shared_dir,
                                               os.path.basename(hash_package))
            if os.path.exists(hash_package_shared):
                print 'Removing existing shared hash package: ' + str(hash_package_shared)
                os.remove(hash_package_shared)

            # Upload to the shared location
            print 'Uploading hash package:'
            print '  from: ' + str(hash_package)
            print '  to: ' + str(hash_package_shared)
            shutil.copy(hash_package, hash_package_shared)

            # Set the signing request
            request = REQ_VAL_SIGN
            request_info = {}
            request_info['hash_package'] = hash_package_shared

            # Create the request packet
            request_dict = {}
            request_dict[REQ] = request
            request_dict[REQ_INFO] = request_info
            request_data = json.dumps(request_dict)

            # Create the socket & send the request
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print 'Connecting to server at host: ' + (self.host) + ' port: ' + str(self.port)
            s.connect((self.host, self.port))
            print 'Sending signing request to the server'
            s.sendall(request_data)
            print 'Waiting for server response'
            response_data = s.recv(10 * 1024)
            print 'Got server response'
            s.close()

            # Decode the server response
            response_dict = json.loads(response_data)
            response = response_dict[RESP]
            response_info = response_dict[RESP_INFO]

            if response == REQ_VAL_SIGN:
                # Decode the signing response
                retcode = response_info['retcode']
                errstr = str(response_info['errstr'])
                sig_package_shared = str(response_info['sig_package'])

                if retcode == 0:
                    # Remove existing signature package
                    sig_package = os.path.join(os.path.dirname(hash_package),
                                               os.path.basename(sig_package_shared))
                    if os.path.exists(sig_package):
                        print 'Removing existing signature package: ' + str(sig_package)
                        os.remove(sig_package)

                    # Download signature package from the shared location
                    print 'Downloading signature package:'
                    print '  from: ' + str(sig_package_shared)
                    print '  to: ' + str(sig_package)
                    shutil.copy(sig_package_shared, sig_package)

            # Invalid response
            else:
                retcode = 1
                errstr = 'Invalid server response: ' + response
                sig_package = ''

        except Exception as e:
            print e
            retcode = 1
            errstr = 'Exception occurred - ' + str(e)
            sig_package = ''

        return retcode, errstr, sig_package

    def sign_metabuild(self, chipset, metabuild, output_dir):
        # Get the images from the metabuild
        retcode, errstr, images = self.getmetabuildimages(chipset, metabuild,
                                                          **self._classmethod_kwargs)

        if retcode != 0:
            print 'Error getting metabuild images. ' + errstr
        else:
            # Print the images from the metabuild
            print 'Got the following from the metabuild'
            print json.dumps(images, sort_keys=True, indent=4, separators=(',', ': '))

            # Sign the images from the metabuild
            for sign_id, image_info in images.items():
                if image_info['retcode'] == 0:
                    print 'Signing ' + sign_id
                    i_retcode, i_errstr, signed_image, expected_path = \
                        self.sign(chipset, output_dir, sign_id=sign_id,
                                  metabuild=metabuild)
                    if i_retcode == 0:
                        print ('Signed ' + sign_id + '\n'
                               '  unsigned image at: ' + image_info['path'] + '\n'
                               '  signed image at: ' + signed_image + '\n'
                               '  expected path at: ' + expected_path)
                    else:
                        print 'Signing ' + sign_id + ' failed. ' + i_errstr
                else:
                    print 'Sign id ' + sign_id + ' could not be found from metabuild. ' + image_info['errstr']


if __name__ == '__main__':
    """TEST_SETTINGS: Following variables define the testing settings
    """
    chipset = '8994'
    sign_id = 'pmic'
    imagefile = os.path.join(DIR_PATH, 'pmic.mbn')
    output_dir = os.path.join(DIR_PATH, 'client_output')
    root_cert_hash = '8ecf3eaa03f772e28479fa2f0bbae2141ccad6f106b384d1c46263edb5b02838'

    # Call the signer to sign
    client = SampleClientSigner(verbose=False, debug=False, quiet=False)
    retcode, errstr, signed_image, expected_path = \
        client.sign(chipset, output_dir, sign_id, imagefile)

    # Print the result
    print 'Signing result: ' + str(retcode)
    if retcode != 0:
        print 'Error: ' + str(errstr)
    else:
        print 'Signed File: ' + str(signed_image)
        print 'Expected Path: ' + str(expected_path)
