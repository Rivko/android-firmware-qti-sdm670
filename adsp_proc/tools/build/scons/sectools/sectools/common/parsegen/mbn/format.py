#===============================================================================
#
# Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================

'''
Created on Mar 31, 2014

@author: hraghav
'''

import defines as df
from sectools.common.parsegen import hex_addr
from sectools.common.parsegen.mbn.header import extract_header, MBN_HDRS, \
    create_empty_header
from sectools.common.utils.c_misc import store_debug_data_to_file, \
    properties_repr


ALIGNED_IMAGE_SIZE      = lambda x : x + ((4 - (x % 4)) if x % 4 else 0)
MBN_PTR_MAX             = 0xFFFFFFFF


class ParseGenMbn(object):

    def __init__(self, data=None,
                 header_size=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 ):

        # Param Checks
        if header_size is None:
            raise RuntimeError('Header size must be given.')

        # Create empty mbn file if data is None
        if data is None:
            data = create_empty_mbn(header_size)

        # Initialize internal properties
        self._header_size = 0
        self._sign = ''
        self._cert_chain = ''
        self._encryption_params = ''

        # Public properties
        self.debug_dir = debug_dir
        self.debug_prefix = debug_prefix
        self.debug_suffix = debug_suffix
        self.invalidate_pointers = False

        """
        Extract the various sections of the data into the following components:
        1. header
        2. signature
        3. cert_chain
        4. bin
        """
        # Public attributes
        self.header = None
        self.code = None

        # Set properties
        self.header_size = header_size

        # Store the original image
        self.store_debug_data(df.FILE_DATA_IN, data)

        # Extract the header
        data, self.header = extract_header(data, self.header_size)
        self.validate_header(self.header)
        self.store_debug_data(df.FILE_HDR_REMOVED, data)
        self.store_debug_data(df.FILE_HDR_IN, self.header.pack())
        self.store_debug_data(df.FILE_HDR_IN_REPR, repr(self.header), suffix=df.FILE_HDR_IN_REPR_SUFFIX)

        # Extract the encryption params
        data, self.encryption_params = extract_encryption_params(data, self.header)
        self.store_debug_data(df.FILE_ENCRYPTION_PARAMS_REMOVED, data)
        self.store_debug_data(df.FILE_ENCRYPTION_PARAMS_IN, self.encryption_params)

        # Extract the cert chain
        data, self.cert_chain = extract_cert_chain(data, self.header)
        self.store_debug_data(df.FILE_CERT_CHAIN_REMOVED, data)
        self.store_debug_data(df.FILE_CERT_CHAIN_IN, self.cert_chain)

        # Extract the signature
        data, self.sign = extract_sign(data, self.header)
        self.store_debug_data(df.FILE_SIGN_REMOVED, data)
        self.store_debug_data(df.FILE_SIGN_IN, self.sign)

        # Save the remaining as code
        self.code = data
        self.store_debug_data(df.FILE_CODE_IN, self.code)

    def _repr_properties(self):
        properties = [
                      ('Header Size', str(self.header_size) + 'B'),
                     ]
        return properties

    def __repr__(self):
        return (properties_repr(self._repr_properties()) + '\n' +
                'Header: ' + '\n' +
                repr(self.header))

    def store_debug_data(self, file_name, data, prefix=None, suffix=None):
        if prefix is None:
            prefix = self.debug_prefix
        if suffix is None:
            suffix = self.debug_suffix
        if prefix is not None and suffix is not None:
            store_debug_data_to_file(prefix + '_' + file_name + suffix,
                                     data, self.debug_dir)

    def validate_configuration(self):
        # Validate the header
        self.validate_header(self.header)

    def get_header(self):
        hdr = self.header

        # Update the pointer & sizes
        hdr.code_size = len(self.code)
        hdr.sig_size = len(self.sign)
        hdr.cert_chain_size = len(self.cert_chain)

        if self.invalidate_pointers:
            hdr.image_dest_ptr = MBN_PTR_MAX
            hdr.sig_ptr = MBN_PTR_MAX
            hdr.cert_chain_ptr = MBN_PTR_MAX
        else:
            hdr.sig_ptr = hdr.image_dest_ptr + hdr.code_size
            hdr.cert_chain_ptr = hdr.sig_ptr + hdr.sig_size

        # Calculate & byte align the total image size
        image_size = ALIGNED_IMAGE_SIZE(hdr.code_size +
                                        hdr.sig_size +
                                        hdr.cert_chain_size)

        # Update the image size in the header
        hdr.image_size = image_size

        return hdr

    def validate_header(self, hdr):
        expected_image_size = ALIGNED_IMAGE_SIZE(hdr.code_size +
                                                 hdr.sig_size +
                                                 hdr.cert_chain_size)

        if hdr.image_size != expected_image_size:
            raise RuntimeError('Mbn header verification failed. '
                               'Image size does not match the sum of its segments:' + '\n'
                               '\n'
                               '    ' + 'Code Size:               ' + hex_addr(hdr.code_size) + '\n'
                               '    ' + 'Signature Size:          ' + hex_addr(hdr.sig_size) + '\n'
                               '    ' + 'Cert Chain Size:         ' + hex_addr(hdr.cert_chain_size) + '\n'
                               '    ' + 'Image Size (expected):  *' + hex_addr(expected_image_size) + ' (Code + Signature + Cert Chain)' + '\n'
                               '    ' + 'Image Size (from file): *' + hex_addr(hdr.image_size) + '\n'
                               '\n'
                               'Header:' + '\n' +
                                repr(hdr)
                              )

    def get_data(self):
        return (self.get_header().pack() + self.code +
                self.sign + self.cert_chain + self.encryption_params)

    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'sign'
    #--------------------------------------------------------------------------
    @property
    def sign(self):
        return self._sign

    @sign.setter
    def sign(self, sign):
        if sign:
            self.validate_sign(sign)
        self._sign = sign

    def validate_sign(self, sign):
        pass

    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'cert_chain'
    #--------------------------------------------------------------------------
    @property
    def cert_chain(self):
        return self._cert_chain

    @cert_chain.setter
    def cert_chain(self, cert_chain):
        if cert_chain:
            self.validate_cert_chain(cert_chain)
        self._cert_chain = cert_chain

    def validate_cert_chain(self, cert_chain):
        pass

    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'encryption_params'
    #--------------------------------------------------------------------------
    @property
    def encryption_params(self):
        return self._encryption_params

    @encryption_params.setter
    def encryption_params(self, encryption_params):
        if encryption_params:
            self.validate_encryption_params(encryption_params)
        self._encryption_params = encryption_params

    def validate_encryption_params(self, encryption_params):
        pass

    #--------------------------------------------------------------------------
    # Get, Set & Validate functions for 'header_size'
    #--------------------------------------------------------------------------
    @property
    def header_size(self):
        return self._header_size

    @header_size.setter
    def header_size(self, value):
        self.validate_header_size(value)
        self._header_size = value

    def validate_header_size(self, value):
        if not isinstance(value, (int, long)):
            raise AttributeError('Header size must be integer. Header size provided: ' + str(value))
        if value not in MBN_HDRS.keys():
            raise AttributeError('Invalid header size provided: ' + str(value) + '\n'
                                 '    ' + 'Supported header sizes are: ' + str(MBN_HDRS.keys()))

def _extract_segment(data, offset, size):
    seg = ''
    if (offset > 0 and offset < len(data) and size > 0):
        seg = data[offset : offset + size]
        data = data[: offset]
    return data, seg

def extract_sign(data, header):
    offset = header.code_size
    size = header.sig_size
    return _extract_segment(data, offset, size)

def extract_cert_chain(data, header):
    offset = header.code_size + header.sig_size
    size = header.cert_chain_size
    return _extract_segment(data, offset, size)

def extract_encryption_params(data, header):
    offset = header.code_size + header.sig_size + header.cert_chain_size
    size = len(data) - offset
    return _extract_segment(data, offset, size)

def create_empty_mbn(header_size):
    header = create_empty_header(header_size)
    return header
