# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
from past.builtins import long
from six import ensure_binary

from sectools.common.parsegen import PAD_BYTE_1
from sectools.common.parsegen.mbn import get_header
from sectools.common.parsegen.mbn.versions.v3.headers import MbnHdr40B
from sectools.common.parsegen.mbn.versions.v3.headers import MbnHdr80B
from sectools.common.utils.c_misc import properties_repr
from sectools.common.utils.c_misc import store_debug_data_to_file
from sectools.features.isc.defines import ALIGNED_IMAGE_SIZE
from sectools.features.isc.defines import MBN_PTR_MAX
from . import MBN_HDR_VERSION_3
from .utils import V3Utils
from ... import MBN_HDRS
from ... import defines

MBN_HDR_OBJS = [MbnHdr40B, MbnHdr80B]
for m in MBN_HDR_OBJS:
    MBN_HDRS[(m.get_size(), m.get_version())] = m

get_header[MBN_HDR_VERSION_3] = \
    lambda size, version, data=None: MBN_HDRS[(size, version)](data)


class V3GetHdrCntxMgr(object):

    def __init__(self, mbn):
        self.mbn = mbn

    def __enter__(self):
        self.hdr = self.mbn.header

        # Update the pointer & sizes
        self.hdr.code_size = len(self.mbn.code)
        self.sig_size, self.cert_chain_size =\
            len(self.mbn.sign), len(self.mbn.cert_chain)
        self.hdr.sig_size, self.hdr.cert_chain_size =\
            self.sig_size, self.cert_chain_size

        return self

    def __exit__(self, *args):
        # Invalidate pointers
        if self.mbn.invalidate_pointers:
            if hasattr(self.hdr, 'image_dest_ptr'):
                self.hdr.image_dest_ptr = MBN_PTR_MAX
            self.hdr.sig_ptr = MBN_PTR_MAX
            self.hdr.cert_chain_ptr = MBN_PTR_MAX
        elif self.hdr.get_version() in [MBN_HDR_VERSION_3]:
            self.hdr.sig_ptr = self.hdr.image_dest_ptr + self.hdr.code_size
            self.hdr.cert_chain_ptr = self.hdr.sig_ptr + self.hdr.sig_size

        # Calculate & byte align the total image size
        # Update the image size in the header
        self.hdr.image_size = ALIGNED_IMAGE_SIZE(self.hdr.code_size +
                                                 self.sig_size +
                                                 self.cert_chain_size)


class ParseGenMbnV3(object):

    UNKNOWN_ALGORITHM = "Unknown Algorithm"
    NONE_ALGORITHM = "None"

    utils = V3Utils

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

        # Extract the cert chain
        self.code, self.cert_chain = self.utils.extract_cert_chain(self.header, self.code)
        self.store_debug_data(defines.FILE_CERT_CHAIN_REMOVED, self.code)
        self.store_debug_data(defines.FILE_CERT_CHAIN_IN, self.cert_chain)

        # Extract the signature
        self.code, self.sign = self.utils.extract_sign(self.header, self.code)
        self.store_debug_data(defines.FILE_SIGN_REMOVED, self.code)
        self.store_debug_data(defines.FILE_SIGN_IN, self.sign)

        # After this point, self.code contains the remaining data.
        self.store_debug_data(defines.FILE_CODE_IN, self.code)

    def _repr_properties(self):
        properties = [('Header Size', str(self.header_size) + 'B'),
                      ('Hash Algorithm', self.extracted_segment_hash_algorithm)]
        return properties

    def __repr__(self):
        properties = properties_repr(self._repr_properties()) + '\n' +\
                     'Header: ' + '\n' + repr(self.get_header())
        return properties

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

    def set_pad_info(self, sig_size, cert_chain_size):
        self.pad_sig_size = sig_size
        self.pad_cert_chain_size = cert_chain_size

    def get_header(self, authority=None, imageinfo=None,
                      validating=False, signing=False,
                      add_signing_attr_to_hash_seg=False):
        with V3GetHdrCntxMgr(self) as v3:
            if (v3.sig_size, v3.cert_chain_size) == (0, 0):
                v3.sig_size, v3.cert_chain_size = \
                    self.pad_sig_size, self.pad_cert_chain_size
                v3.hdr.sig_size, v3.hdr.cert_chain_size = \
                    v3.sig_size, v3.cert_chain_size
        return v3.hdr

    def validate_header(self, hdr):
        pass

    def get_data(self, authority=None, imageinfo=None, validating=False,
                    signing=False, add_signing_attr_to_hash_seg=False):
        hdr = self.get_header(authority, imageinfo, validating, signing,
                             add_signing_attr_to_hash_seg)
        return (hdr.pack() + self.code + ensure_binary(self.sign) + ensure_binary(self.cert_chain)).ljust(
            hdr.get_size() + hdr.image_size, PAD_BYTE_1)

    # --------------------------------------------------------------------------
    # Get, Set & Validate functions for 'sign'
    # --------------------------------------------------------------------------
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

    # --------------------------------------------------------------------------
    # Get, Set & Validate functions for 'cert_chain'
    # --------------------------------------------------------------------------
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

    # --------------------------------------------------------------------------
    # Get, Set & Validate functions for 'header_size'
    # --------------------------------------------------------------------------
    @property
    def header_size(self):
        return self._header_size

    @header_size.setter
    def header_size(self, value):
        self.validate_header_size(value, self.version)
        self._header_size = value

    @staticmethod
    def validate_header_size(size, version):
        if not isinstance(size, (int, long)):
            raise AttributeError(
                'Header size must be integer. Header size provided: ' +
                str(size))
        sizes = [s for s, v in MBN_HDRS.keys()]

        if version in [MBN_HDR_VERSION_3] and size not in sizes:
            sorted_size_of_version = sorted(
                [s for s, v in MBN_HDRS.keys() if v == version])
            raise AttributeError(
                'Invalid header size provided: {0}\n    '
                'Supported header sizes for MBN version {1} are: {2}'
                    .format(size, version, sorted_size_of_version))


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
