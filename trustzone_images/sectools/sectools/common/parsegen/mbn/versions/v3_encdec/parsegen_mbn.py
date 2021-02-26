# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
from six import ensure_binary

from sectools.common.parsegen import PAD_BYTE_1
from sectools.common.parsegen.mbn import MBN_HDRS
from sectools.common.parsegen.mbn import defines
from sectools.common.parsegen.mbn import get_header
from sectools.common.parsegen.mbn.versions.v3 import MBN_HDR_VERSION_3
from sectools.common.parsegen.mbn.versions.v3.parsegen_mbn import ParseGenMbnV3
from .utils import V3EncDecUtils

FILE_ENCRYPTION_PARAMS_REMOVED = 'mbn_in_2_encryption_params_removed'
FILE_ENCRYPTION_PARAMS_IN = 'mbn_in_2_encryption_params'


class ParseGenMbnV3EncDec(ParseGenMbnV3):

    utils = V3EncDecUtils

    def __init__(self, data=None,
                 header_size=None,
                 version=MBN_HDR_VERSION_3,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 pad_max_sig_size=0,
                 pad_max_cert_chain_size=0,
                 pad_max_encr_params_size=0,
                 general_properties=None):

        # Param Checks
        if header_size is None:
            raise RuntimeError('Header size must be given.')

        # Create empty mbn file if data is None
        if data is None:
            self.code = create_empty_mbn(header_size, version)
        else:
            self.code = data

        # Initialize internal properties
        self._header_size = 0
        self._sign = ''
        self._cert_chain = ''
        self._encryption_params = ''

        # Public properties
        self.general_properties = general_properties
        self.debug_dir = debug_dir
        self.debug_prefix = debug_prefix
        self.debug_suffix = debug_suffix
        self.invalidate_pointers = False
        self.version = version
        self.extracted_segment_hash_algorithm = self.NONE_ALGORITHM

        # Padding sizes
        self.pad_sig_size = 0
        self.pad_cert_chain_size = 0
        self.pad_max_sig_size = pad_max_sig_size
        self.pad_max_cert_chain_size = pad_max_cert_chain_size
        self.pad_max_encr_params_size = pad_max_encr_params_size

        """
        Extract the various sections of the data into the following components:
        1. header
        2. signature
        3. cert_chain
        4. bin
        """
        # Public attributes
        self.header = None

        # Store the original image
        self.store_debug_data(defines.FILE_DATA_IN, self.code)

        # Extract the header
        self.code, self.header = extract_header(self.code, header_size, self.version)

        # Set properties
        self.header_size = self.header.get_size()

        self.validate_header(self.header)
        self.store_debug_data(defines.FILE_HDR_REMOVED, self.code)
        self.store_debug_data(defines.FILE_HDR_IN, self.header.pack())
        self.store_debug_data(defines.FILE_HDR_IN_REPR, repr(self.header),
                              suffix=defines.FILE_HDR_IN_REPR_SUFFIX)

        # Extract the code size
        self.code_size = self.header.code_size

        # Extract the encryption params
        self.code, self.encryption_params = self.utils.extract_encryption_params(self.code)
        self.store_debug_data(FILE_ENCRYPTION_PARAMS_REMOVED, self.code)
        self.store_debug_data(FILE_ENCRYPTION_PARAMS_IN, self.encryption_params)

        # Extract the cert chain
        self.code, self.cert_chain = self.utils.extract_cert_chain(self.header, self.code)
        self.store_debug_data(defines.FILE_CERT_CHAIN_REMOVED, self.code)
        self.store_debug_data(defines.FILE_CERT_CHAIN_IN, self.cert_chain)

        # Extract the signature
        self.code, self.sign = self.utils.extract_sign(self.header, self.code)
        self.store_debug_data(defines.FILE_SIGN_REMOVED, self.code)
        self.store_debug_data(defines.FILE_SIGN_IN, self.sign)

        # It is correct to use ParseGenMbnV3 here, because its super is object class
        # or another class in multiple inheritance, skipping ParsegenMbnV3.__init__(),
        #  which is overridden by ParseGenMbnV3EncDec.
        super(ParseGenMbnV3, self).__init__()

        # After this point, self.code contains the remaining data.
        self.store_debug_data(defines.FILE_CODE_IN, self.code)

    def get_data(self, authority=None, imageinfo=None, validating=False,
                    signing=False, add_signing_attr_to_hash_seg=False):
        hdr = self.get_header(authority, imageinfo, validating, signing,
                             add_signing_attr_to_hash_seg)
        # add encryption params if they exist
        if self.encryption_params:
            encr_params_size = len(self.encryption_params)
        # pad space for encryption params if image is signed
        elif len(self.sign) > 0:
            encr_params_size = self.pad_max_encr_params_size
        # don't pad space for encryption params if image is integrity image
        else:
            encr_params_size = 0
        return (hdr.pack() + self.code + ensure_binary(self.sign) + ensure_binary(self.cert_chain) +
                ensure_binary(self.encryption_params)).ljust(
            hdr.get_size() + hdr.image_size + encr_params_size, PAD_BYTE_1)

    # --------------------------------------------------------------------------
    # Get, Set & Validate functions for 'encryption_params'
    # --------------------------------------------------------------------------
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


def extract_header(data, header_size, version=MBN_HDR_VERSION_3):
    try:
        header = get_header[version](header_size, version, data)
    except KeyError:
        raise RuntimeError(
            'Unexpected header size, version provided:\n' +
            '    Header size - ' + str(header_size) + ', version - ' +
            str(version) + '\n' + '    Supported header types: ' +
            str(MBN_HDRS.keys()))
    header_size = header.get_original_size()
    data = data[header_size:]
    return data, header


def create_empty_header(header_size, version=MBN_HDR_VERSION_3):
    try:
        header = get_header[version](header_size, version)
    except KeyError:
        raise RuntimeError(
            'Unexpected header size, version provided:\n' +
            '    Header size - ' + str(header_size) + ', version - ' +
            str(version) + '\n' + '    Supported header types: ' +
            str(MBN_HDRS.keys()))
    return header.pack()


def create_empty_mbn(header_size, version=MBN_HDR_VERSION_3):
    header = create_empty_header(header_size, version)
    return header
