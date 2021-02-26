# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
import defines as df
from sectools.common.parsegen import hex_addr, PAD_BYTE_1
from sectools.common.parsegen.mbn.header import extract_header, MBN_HDRS, \
    create_empty_header, MBN_HDR_VERSION_3
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import store_debug_data_to_file, \
    properties_repr


ALIGNED_IMAGE_SIZE = lambda x: x + ((4 - (x % 4)) if x % 4 else 0)
MBN_PTR_MAX = 0xFFFFFFFF
AUTHORITY_QC = 'QC'


class ParseGenMbn(object):

    def __init__(self, data=None,
                 header_size=None,
                 version=MBN_HDR_VERSION_3,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 pad_max_sig_size=0,
                 pad_max_cert_chain_size=0,
                 pad_max_encr_params_size=0,
                 ):

        # Param Checks
        if header_size is None:
            raise RuntimeError('Header size must be given.')

        # Create empty mbn file if data is None
        if data is None:
            data = create_empty_mbn(header_size, version)

        # Initialize internal properties
        self._header_size = 0
        self._sign = ''
        self._cert_chain = ''
        self._sign_qc = ''
        self._cert_chain_qc = ''
        self._encryption_params = ''

        # Public properties
        self.debug_dir = debug_dir
        self.debug_prefix = debug_prefix
        self.debug_suffix = debug_suffix
        self.invalidate_pointers = False
        self.version = version

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
        self.code = None

        # Set properties
        self.header_size = header_size

        # Store the original image
        self.store_debug_data(df.FILE_DATA_IN, data)

        # Extract the header
        data, self.header = extract_header(data, self.header_size, self.version)
        self.validate_header(self.header)
        self.store_debug_data(df.FILE_HDR_REMOVED, data)
        self.store_debug_data(df.FILE_HDR_IN, self.header.pack())
        self.store_debug_data(df.FILE_HDR_IN_REPR, repr(self.header), suffix=df.FILE_HDR_IN_REPR_SUFFIX)

        # Extract the encryption params
        data, self.encryption_params = extract_encryption_params(data, self.header,
                                                                 self.pad_max_sig_size,
                                                                 self.pad_max_cert_chain_size,
                                                                 self.pad_max_encr_params_size)
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

        if self.header.supports_qc_signing():
            # Extract the QC cert chain
            data, self.cert_chain_qc = extract_cert_chain_qc(data, self.header)
            self.store_debug_data(df.FILE_CERT_CHAIN_QC_REMOVED, data)
            self.store_debug_data(df.FILE_CERT_CHAIN_QC_IN, self.cert_chain_qc)

            # Extract the QC signature
            data, self.sign_qc = extract_sign_qc(data, self.header)
            self.store_debug_data(df.FILE_SIGN_QC_REMOVED, data)
            self.store_debug_data(df.FILE_SIGN_QC_IN, self.sign_qc)

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
                repr(self.get_header()))

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

    def get_header(self, authority=None):
        hdr = self.header

        # Set some flags
        single_signing = self.pad_max_sig_size == 0
        signing = self.pad_sig_size != 0

        # Update the pointer & sizes
        hdr.code_size = len(self.code)
        sig_size, cert_chain_size = len(self.sign), len(self.cert_chain)
        hdr.sig_size, hdr.cert_chain_size = sig_size, cert_chain_size

        if hdr.supports_qc_signing():
            sig_size_qc, cert_chain_size_qc = len(self.sign_qc), len(self.cert_chain_qc)
            hdr.sig_size_qc, hdr.cert_chain_size_qc = sig_size_qc, cert_chain_size_qc
            if authority == AUTHORITY_QC:
                hdr.sig_size, hdr.cert_chain_size = 0, 0

            # Single sign / Exclusive usecase
            if single_signing:
                # Both QC and OEM data cannot be set
                if sig_size and sig_size_qc:
                    raise RuntimeError('Both QC & OEM signature must not be set')

                # If both are empty, use padding for data to sign
                if (sig_size, sig_size_qc) == (0, 0):
                    if authority == AUTHORITY_QC:
                        sig_size_qc, cert_chain_size_qc = self.pad_sig_size, self.pad_cert_chain_size
                        hdr.sig_size_qc, hdr.cert_chain_size_qc = sig_size_qc, cert_chain_size_qc
                    else:
                        sig_size, cert_chain_size = self.pad_sig_size, self.pad_cert_chain_size
                        hdr.sig_size, hdr.cert_chain_size = sig_size, cert_chain_size

            # Double signing
            else:
                # Both QC & OEM are empty, use padding to return to sign
                if (sig_size, sig_size_qc) == (0, 0):
                    if authority == AUTHORITY_QC:
                        sig_size_qc, cert_chain_size_qc = self.pad_sig_size, self.pad_cert_chain_size
                        hdr.sig_size_qc, hdr.cert_chain_size_qc = sig_size_qc, cert_chain_size_qc

                        if signing:
                            sig_size = self.pad_max_sig_size
                            cert_chain_size = self.pad_max_cert_chain_size
                    else:
                        sig_size, cert_chain_size = self.pad_sig_size, self.pad_cert_chain_size
                        hdr.sig_size, hdr.cert_chain_size = sig_size, cert_chain_size

                        if signing:
                            sig_size_qc = self.pad_max_sig_size
                            cert_chain_size_qc = self.pad_max_cert_chain_size

                # QC is empty
                elif (sig_size_qc, cert_chain_size_qc) == (0, 0):
                    if signing:
                        sig_size_qc = self.pad_max_sig_size
                        cert_chain_size_qc = self.pad_max_cert_chain_size
                    if authority == AUTHORITY_QC:
                        hdr.sig_size_qc = self.pad_sig_size
                        hdr.cert_chain_size_qc = self.pad_cert_chain_size
                    else:
                        sig_size, cert_chain_size = self.pad_sig_size, self.pad_cert_chain_size
                        hdr.sig_size, hdr.cert_chain_size = sig_size, cert_chain_size

                # OEM is empty
                elif (sig_size, cert_chain_size) == (0, 0):
                    if signing:
                        sig_size = self.pad_max_sig_size
                        cert_chain_size = self.pad_max_cert_chain_size
                    if authority == AUTHORITY_QC:
                        sig_size_qc, cert_chain_size_qc = self.pad_sig_size, self.pad_cert_chain_size
                        hdr.sig_size_qc, hdr.cert_chain_size_qc = sig_size_qc, cert_chain_size_qc
                    else:
                        hdr.sig_size = self.pad_sig_size
                        hdr.cert_chain_size = self.pad_cert_chain_size

                # Both are non empty
                else:
                    sig_size += self.pad_max_sig_size - sig_size

            # Add QC sizes for the full size to be used for image size
            sig_size += sig_size_qc
            cert_chain_size += cert_chain_size_qc
        else:
            if (sig_size, cert_chain_size) == (0, 0):
                sig_size, cert_chain_size = self.pad_sig_size, self.pad_cert_chain_size
                hdr.sig_size, hdr.cert_chain_size = sig_size, cert_chain_size

        # Invalidate pointers
        if self.invalidate_pointers:
            if hasattr(hdr, 'image_dest_ptr'):
                hdr.image_dest_ptr = MBN_PTR_MAX
            hdr.sig_ptr = MBN_PTR_MAX
            hdr.cert_chain_ptr = MBN_PTR_MAX
        elif not hdr.supports_qc_signing():
            hdr.sig_ptr = hdr.image_dest_ptr + hdr.code_size
            hdr.cert_chain_ptr = hdr.sig_ptr + hdr.sig_size

        # Calculate & byte align the total image size
        image_size = ALIGNED_IMAGE_SIZE(hdr.code_size + sig_size + cert_chain_size)

        # Update the image size in the header
        hdr.image_size = image_size

        return hdr

    def validate_header(self, hdr):
        sign_assets_size = [hdr.sig_size + hdr.cert_chain_size]
        if hdr.supports_qc_signing():
            sign_assets_size += [hdr.sig_size_qc + hdr.cert_chain_size_qc,
                                 hdr.sig_size + hdr.cert_chain_size +
                                 hdr.sig_size_qc + hdr.cert_chain_size_qc +
                                 self.pad_max_sig_size - hdr.sig_size +
                                 self.pad_max_cert_chain_size - hdr.cert_chain_size]
        expected_image_size = [ALIGNED_IMAGE_SIZE(hdr.code_size + ss) for ss in sign_assets_size]

        if hdr.image_size not in expected_image_size:
            # Check for QC sign asset related errors
            qc_segment, qc_err_string = '', ''
            if hdr.supports_qc_signing():
                qc_segment = ('    ' + 'QC Signature Size:       ' + hex_addr(hdr.sig_size_qc) + '\n'
                              '    ' + 'QC Cert Chain Size:      ' + hex_addr(hdr.cert_chain_size_qc) + '\n')

                # Handles case where incoming data header contains residual data in QC sign/cc fields
                if (hdr.image_size + hdr.sig_size_qc + hdr.cert_chain_size_qc in expected_image_size or
                        hdr.cert_chain_size_qc == MBN_PTR_MAX):
                    qc_err_string = ('Unsigned hash table header must not contain junk data in '
                                     'QC attribute fields when double-signing.' + '\n\n')

            raise RuntimeError('Mbn header verification failed. '
                               'Image size does not match the sum of its segments:' + '\n'
                               '\n'
                               '    ' + 'Code Size:               ' + hex_addr(hdr.code_size) + '\n' + qc_segment +
                               '    ' + 'Signature Size:          ' + hex_addr(hdr.sig_size) + '\n'
                               '    ' + 'Cert Chain Size:         ' + hex_addr(hdr.cert_chain_size) + '\n'
                               '    ' + 'Image Size (expected):  *' + str([hex_addr(s) for s in expected_image_size]) +
                               ' (Code + Signature + Cert Chain)' + '\n'
                               '    ' + 'Image Size (from file): *' + hex_addr(hdr.image_size) + '\n'
                               '\n' + qc_err_string +
                               'Header:' + '\n' + repr(hdr))

    def get_data(self, authority=None):
        hdr = self.get_header(authority)
        encr_params_size = (len(self.encryption_params) if self.encryption_params else self.pad_max_encr_params_size)
        return (hdr.pack() + self.code + self.sign_qc + self.cert_chain_qc + self.sign + self.cert_chain +
                self.encryption_params).ljust(hdr.get_size() + hdr.image_size + encr_params_size, PAD_BYTE_1)

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
    # Get, Set & Validate functions for 'sign_qc'
    # --------------------------------------------------------------------------
    @property
    def sign_qc(self):
        return self._sign_qc

    @sign_qc.setter
    def sign_qc(self, sign_qc):
        if sign_qc:
            if not self.header.supports_qc_signing():
                raise RuntimeError('QC Signature is not supported.')
            self.validate_sign_qc(sign_qc)
        self._sign_qc = sign_qc

    def validate_sign_qc(self, sign_qc):
        pass

    # --------------------------------------------------------------------------
    # Get, Set & Validate functions for 'cert_chain_qc'
    # --------------------------------------------------------------------------
    @property
    def cert_chain_qc(self):
        return self._cert_chain_qc

    @cert_chain_qc.setter
    def cert_chain_qc(self, cert_chain_qc):
        if cert_chain_qc:
            if not self.header.supports_qc_signing():
                raise RuntimeError('QC Cert Chain is not supported.')
            self.validate_cert_chain_qc(cert_chain_qc)
        self._cert_chain_qc = cert_chain_qc

    def validate_cert_chain_qc(self, cert_chain_qc):
        pass

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

    # --------------------------------------------------------------------------
    # Get, Set & Validate functions for 'header_size'
    # --------------------------------------------------------------------------
    @property
    def header_size(self):
        return self._header_size

    @header_size.setter
    def header_size(self, value):
        self.validate_header_size(value)
        self._header_size = value

    @staticmethod
    def validate_header_size(value):
        if not isinstance(value, (int, long)):
            raise AttributeError('Header size must be integer. Header size provided: ' + str(value))
        if value not in [s for s, v in MBN_HDRS.keys()]:
            raise AttributeError('Invalid header size provided: ' + str(value) + '\n'
                                 '    ' + 'Supported header sizes are: ' + str(MBN_HDRS.keys()))


def _extract_segment(data, offset, size):
    seg = ''
    if 0 < offset < len(data) and size > 0:
        seg = data[offset: offset + size]
        data = data[: offset]
    return data, seg


def extract_sign_qc(data, header):
    offset = header.code_size
    size = header.sig_size_qc
    return _extract_segment(data, offset, size)


def extract_cert_chain_qc(data, header):
    offset = header.code_size + header.sig_size_qc
    size = header.cert_chain_size_qc
    return _extract_segment(data, offset, size)


def extract_sign(data, header):
    offset = header.code_size
    size = header.sig_size
    if header.supports_qc_signing():
        offset += header.sig_size_qc + header.cert_chain_size_qc
    data, seg = _extract_segment(data, offset, size)
    return data, seg


def extract_cert_chain(data, header):
    offset = header.code_size + header.sig_size
    size = header.cert_chain_size
    if header.supports_qc_signing():
        offset += header.sig_size_qc + header.cert_chain_size_qc
    data, seg = _extract_segment(data, offset, size)
    return data, seg


def extract_encryption_params(data, header, pad_max_sig_size=0, pad_max_cert_chain_size=0,
                              pad_max_encr_params_size=0):
    sig_size, cert_chain_size = header.sig_size, header.cert_chain_size
    offset = header.code_size + sig_size + cert_chain_size
    size = len(data) - offset
    if header.supports_qc_signing():
        sig_size_qc, cert_chain_size_qc = header.sig_size_qc, header.cert_chain_size_qc
        offset += sig_size_qc + cert_chain_size_qc
        size = len(data) - offset

        # QC & OEM are zero, then encryption params must be zero
        if (sig_size, cert_chain_size, sig_size_qc, cert_chain_size_qc) == (0, 0, 0, 0) and size != 0:
            raise RuntimeError('Encryption params must not exist for unsigned file.')

        # QC is zero or OEM is zero
        elif (0, 0) in [(sig_size, cert_chain_size), (sig_size_qc, cert_chain_size_qc)]:
            # Exclusive use case
            if size in [0, pad_max_encr_params_size]:
                pass
            # Double sign - 1st stage
            elif size == pad_max_sig_size + pad_max_cert_chain_size + pad_max_encr_params_size:
                # Verify padding
                _, seg = _extract_segment(data, offset, size)
                if seg != PAD_BYTE_1 * size:
                    logger.warning('Invalid padding data in file')
                size = 0
            else:
                raise RuntimeError('Invalid size for encryption params: ' + str(size))

        # Double signed image
        else:
            # Second sign did not use max signature size
            potential_padding_size = pad_max_sig_size - min(sig_size, sig_size_qc)
            if potential_padding_size and (size == pad_max_encr_params_size + potential_padding_size):
                # Verify padding
                _, seg = _extract_segment(data, offset + pad_max_encr_params_size, potential_padding_size)
                if seg != PAD_BYTE_1 * potential_padding_size:
                    raise RuntimeError('Invalid padding data in file')
                size -= potential_padding_size
            elif size != pad_max_encr_params_size:
                raise RuntimeError('Invalid size for encryption params: ' + str(size))

            # Check if there's padding
            _, seg = _extract_segment(data, offset, size)
            if seg == PAD_BYTE_1 * size:
                size = 0

    # Extract encryption params
    data, seg = _extract_segment(data, offset, size)
    return data, seg


def create_empty_mbn(header_size, version=MBN_HDR_VERSION_3):
    header = create_empty_header(header_size, version)
    return header
