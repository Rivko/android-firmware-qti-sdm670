# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
import defines as df
from sectools.common.parsegen import hex_addr, PAD_BYTE_1
from sectools.common.parsegen.mbn.header import extract_header, copy_header, MBN_HDRS, \
    create_empty_header, MBN_HDR_VERSION_3, MBN_HDR_VERSION_6
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import store_debug_data_to_file, \
    properties_repr


ALIGNED_IMAGE_SIZE = lambda x: x + ((4 - (x % 4)) if x % 4 else 0)
MBN_PTR_MAX = 0xFFFFFFFF
AUTHORITY_QTI = 'QTI'


class ParseGenMbn(object):

    UNKNOWN_ALGORITHM = "Unknown Algorithm"
    NONE_ALGORITHM = "None"

    def __init__(self, data=None,
                 header_size=None,
                 version=MBN_HDR_VERSION_3,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 pad_max_sig_size=0,
                 pad_max_cert_chain_size=0,
                 pad_max_encr_params_size=0
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
        self._sign_qti = ''
        self._cert_chain_qti = ''
        self._encryption_params = ''

        # Public properties
        self.debug_dir = debug_dir
        self.debug_prefix = debug_prefix
        self.debug_suffix = debug_suffix
        self.invalidate_pointers = False
        self.version = version
        self.extracted_segment_hash_algorithm = ParseGenMbn.NONE_ALGORITHM

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

        # Store the original image
        self.store_debug_data(df.FILE_DATA_IN, data)

        # Extract the header
        data, self.header = extract_header(data, header_size, self.version)

        # Set properties
        self.header_size = self.header.get_size()

        self.validate_header(self.header)
        self.store_debug_data(df.FILE_HDR_REMOVED, data)
        self.store_debug_data(df.FILE_HDR_IN, self.header.pack())
        self.store_debug_data(df.FILE_HDR_IN_REPR, repr(self.header), suffix=df.FILE_HDR_IN_REPR_SUFFIX)

        # Extract the code size
        self.code_size = self.header.code_size

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

        if self.header.supports_qti_signing():
            # Extract the QTI cert chain
            data, self.cert_chain_qti = extract_cert_chain_qti(data, self.header)
            self.store_debug_data(df.FILE_CERT_CHAIN_QTI_REMOVED, data)
            self.store_debug_data(df.FILE_CERT_CHAIN_QTI_IN, self.cert_chain_qti)

            # Extract the QTI signature
            data, self.sign_qti = extract_sign_qti(data, self.header)
            self.store_debug_data(df.FILE_SIGN_QTI_REMOVED, data)
            self.store_debug_data(df.FILE_SIGN_QTI_IN, self.sign_qti)

        # Save the remaining as code
        self.code = data
        self.store_debug_data(df.FILE_CODE_IN, self.code)

    def _repr_properties(self):
        properties = [('Header Size', str(self.header_size) + 'B'),
                      ('Hash Algorithm', self.extracted_segment_hash_algorithm)]
        return properties

    def __repr__(self):
        properties = properties_repr(self._repr_properties()) + '\n' + 'Header: ' + '\n' + repr(self.get_header())
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

    def get_header(self, authority=None, imageinfo=None, validating=False, signing=False, add_signing_attr_to_hash_seg=False):
        hdr = self.header

        single_signing = self.pad_max_sig_size == 0

        # Set header metadata
        if hdr.supports_metadata():
            # Don't modify the internal header object if user is validating image otherwise
            # header metadata contents log will not reflect metadata of image being validated
            # but rather image metadata being validated against
            if validating:
                hdr = copy_header(hdr)
                if not single_signing and authority == AUTHORITY_QTI:
                    hdr.set_metadata(None)
            elif signing or add_signing_attr_to_hash_seg:
                if single_signing:
                    if authority == AUTHORITY_QTI:
                        hdr.set_metadata_qti(imageinfo)
                    else:
                        hdr.set_metadata(imageinfo)
                else:
                    if authority == AUTHORITY_QTI:
                        hdr.set_metadata_qti(imageinfo)
                        hdr.set_metadata(None)
                    else:
                        # Prevent OEM from adding metadata before QTI for double-signable images
                        if not hdr.get_metadata_qti():
                            raise RuntimeError("Image must contain QTI signing attributes before adding OEM signing attributes")
                        else:
                            hdr.set_metadata(imageinfo)

        signing = self.pad_sig_size != 0

        # Update the pointer & sizes
        hdr.code_size = len(self.code)
        sig_size, cert_chain_size = len(self.sign), len(self.cert_chain)
        hdr.sig_size, hdr.cert_chain_size = sig_size, cert_chain_size

        # MBN v5 and v6 usecases
        if hdr.supports_qti_signing():
            sig_size_qti, cert_chain_size_qti = len(self.sign_qti), len(self.cert_chain_qti)
            hdr.sig_size_qti, hdr.cert_chain_size_qti = sig_size_qti, cert_chain_size_qti
            if authority == AUTHORITY_QTI:
                hdr.sig_size, hdr.cert_chain_size = 0, 0

            # Single signing / exclusive usecase
            if single_signing:
                # Both QTI and OEM data cannot be set
                if sig_size and sig_size_qti:
                    raise RuntimeError('Both QTI & OEM signature must not be set')

                # If both are empty, use padding for data to sign
                if (sig_size, sig_size_qti) == (0, 0):
                    if authority == AUTHORITY_QTI:
                        sig_size_qti, cert_chain_size_qti = self.pad_sig_size, self.pad_cert_chain_size
                        hdr.sig_size_qti, hdr.cert_chain_size_qti = sig_size_qti, cert_chain_size_qti
                    else:
                        sig_size, cert_chain_size = self.pad_sig_size, self.pad_cert_chain_size
                        hdr.sig_size, hdr.cert_chain_size = sig_size, cert_chain_size

            # Double signing
            else:
                # Both QTI & OEM are empty, use padding to return to sign
                if (sig_size, sig_size_qti) == (0, 0):
                    if authority == AUTHORITY_QTI:
                        sig_size_qti, cert_chain_size_qti = self.pad_sig_size, self.pad_cert_chain_size
                        hdr.sig_size_qti, hdr.cert_chain_size_qti = sig_size_qti, cert_chain_size_qti

                        if signing:
                            sig_size = self.pad_max_sig_size
                            cert_chain_size = self.pad_max_cert_chain_size
                    else:
                        sig_size, cert_chain_size = self.pad_sig_size, self.pad_cert_chain_size
                        hdr.sig_size, hdr.cert_chain_size = sig_size, cert_chain_size

                        if signing:
                            sig_size_qti = self.pad_max_sig_size
                            cert_chain_size_qti = self.pad_max_cert_chain_size

                # QTI is empty
                elif (sig_size_qti, cert_chain_size_qti) == (0, 0):
                    if signing:
                        sig_size_qti = self.pad_max_sig_size
                        cert_chain_size_qti = self.pad_max_cert_chain_size
                    if authority == AUTHORITY_QTI:
                        hdr.sig_size_qti = self.pad_sig_size
                        hdr.cert_chain_size_qti = self.pad_cert_chain_size
                    else:
                        sig_size, cert_chain_size = self.pad_sig_size, self.pad_cert_chain_size
                        hdr.sig_size, hdr.cert_chain_size = sig_size, cert_chain_size

                # OEM is empty
                elif (sig_size, cert_chain_size) == (0, 0):
                    if signing:
                        sig_size = self.pad_max_sig_size
                        cert_chain_size = self.pad_max_cert_chain_size
                    if authority == AUTHORITY_QTI:
                        sig_size_qti, cert_chain_size_qti = self.pad_sig_size, self.pad_cert_chain_size
                        hdr.sig_size_qti, hdr.cert_chain_size_qti = sig_size_qti, cert_chain_size_qti
                    else:
                        hdr.sig_size = self.pad_sig_size
                        hdr.cert_chain_size = self.pad_cert_chain_size

                # Both are non empty
                else:
                    sig_size += self.pad_max_sig_size - sig_size

            # Add QTI sizes for the full size to be used for image size
            sig_size += sig_size_qti
            cert_chain_size += cert_chain_size_qti
        # MBN v3 usecases. This is legacy OEM signing usecase
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
        elif not hdr.supports_qti_signing():
            hdr.sig_ptr = hdr.image_dest_ptr + hdr.code_size
            hdr.cert_chain_ptr = hdr.sig_ptr + hdr.sig_size

        # Calculate & byte align the total image size
        image_size = ALIGNED_IMAGE_SIZE(hdr.code_size + sig_size + cert_chain_size)

        # Update the image size in the header
        hdr.image_size = image_size

        return hdr

    def validate_header(self, hdr):
        sign_assets_size = [hdr.sig_size + hdr.cert_chain_size]
        if hdr.supports_qti_signing():
            sign_assets_size += [hdr.sig_size_qti + hdr.cert_chain_size_qti,
                                 hdr.sig_size + hdr.cert_chain_size +
                                 hdr.sig_size_qti + hdr.cert_chain_size_qti +
                                 self.pad_max_sig_size - hdr.sig_size +
                                 self.pad_max_cert_chain_size - hdr.cert_chain_size]
        expected_image_size = [ALIGNED_IMAGE_SIZE(hdr.code_size + ss) for ss in sign_assets_size]

        if hdr.image_size not in expected_image_size:
            # Check for QTI sign asset related errors
            qti_segment, qti_err_string = '', ''
            if hdr.supports_qti_signing():
                qti_segment = ('    ' + 'QTI Signature Size:       ' + hex_addr(hdr.sig_size_qti) + '\n'
                               '    ' + 'QTI Cert Chain Size:      ' + hex_addr(hdr.cert_chain_size_qti) + '\n')

                # Handles case where incoming data header contains residual data in QTI sign/cc fields
                if (hdr.image_size + hdr.sig_size_qti + hdr.cert_chain_size_qti in expected_image_size or
                        hdr.cert_chain_size_qti == MBN_PTR_MAX):
                    qti_err_string = ('Unsigned hash table header must not contain junk data in '
                                      'QTI attribute fields when double-signing.' + '\n\n')

            raise RuntimeError('Mbn header verification failed. '
                               'Image size does not match the sum of its segments:' + '\n'
                               '\n'
                               '    ' + 'Code Size:               ' + hex_addr(hdr.code_size) + '\n' + qti_segment +
                               '    ' + 'Signature Size:          ' + hex_addr(hdr.sig_size) + '\n'
                               '    ' + 'Cert Chain Size:         ' + hex_addr(hdr.cert_chain_size) + '\n'
                               '    ' + 'Image Size (expected):  *' + str([hex_addr(s) for s in expected_image_size]) +
                               ' (Code + Signature + Cert Chain)' + '\n'
                               '    ' + 'Image Size (from file): *' + hex_addr(hdr.image_size) + '\n'
                               '\n' + qti_err_string +
                               'Header:' + '\n' + repr(hdr))

    def get_data(self, authority=None, imageinfo=None, validating=False, signing=False, add_signing_attr_to_hash_seg=False):
        hdr = self.get_header(authority, imageinfo, validating, signing, add_signing_attr_to_hash_seg)
        # add encryption params if they exist
        if self.encryption_params:
            encr_params_size = len(self.encryption_params)
        # pad space for encryption params if image is signed
        elif len(self.sign_qti) > 0 or len(self.sign) > 0:
            encr_params_size = self.pad_max_encr_params_size
        # don't pad space for encryption params if image is integrity image
        else:
            encr_params_size = 0
        return (hdr.pack() + self.code + self.sign_qti + self.cert_chain_qti + self.sign + self.cert_chain +
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
    # Get, Set & Validate functions for 'sign_qti'
    # --------------------------------------------------------------------------
    @property
    def sign_qti(self):
        return self._sign_qti

    @sign_qti.setter
    def sign_qti(self, sign_qti):
        if sign_qti:
            if not self.header.supports_qti_signing():
                raise RuntimeError('QTI Signature is not supported.')
            self.validate_sign_qti(sign_qti)
        self._sign_qti = sign_qti

    def validate_sign_qti(self, sign_qti):
        pass

    # --------------------------------------------------------------------------
    # Get, Set & Validate functions for 'cert_chain_qti'
    # --------------------------------------------------------------------------
    @property
    def cert_chain_qti(self):
        return self._cert_chain_qti

    @cert_chain_qti.setter
    def cert_chain_qti(self, cert_chain_qti):
        if cert_chain_qti:
            if not self.header.supports_qti_signing():
                raise RuntimeError('QTI Cert Chain is not supported.')
            self.validate_cert_chain_qti(cert_chain_qti)
        self._cert_chain_qti = cert_chain_qti

    def validate_cert_chain_qti(self, cert_chain_qti):
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
        self.validate_header_size(value, self.version)
        self._header_size = value

    @staticmethod
    def validate_header_size(value, version):
        if not isinstance(value, (int, long)):
            raise AttributeError('Header size must be integer. Header size provided: ' + str(value))
        if version != MBN_HDR_VERSION_6 and value not in [s for s, v in MBN_HDRS.keys()]:
            raise AttributeError('Invalid header size provided: {0}\n    '
                                 'Supported header sizes for MBN version {1} are: {2}'.format(value, version, sorted([s for s, v in MBN_HDRS.keys() if v == version])))


def _extract_segment(data, offset, size):
    seg = ''
    if 0 < offset < len(data) and size > 0:
        seg = data[offset: offset + size]
        data = data[: offset]
    return data, seg


def extract_sign_qti(data, header):
    offset = header.code_size
    size = header.sig_size_qti
    return _extract_segment(data, offset, size)


def extract_cert_chain_qti(data, header):
    offset = header.code_size + header.sig_size_qti
    size = header.cert_chain_size_qti
    return _extract_segment(data, offset, size)


def extract_sign(data, header):
    offset = header.code_size
    size = header.sig_size
    if header.supports_qti_signing():
        offset += header.sig_size_qti + header.cert_chain_size_qti
    data, seg = _extract_segment(data, offset, size)
    return data, seg


def extract_cert_chain(data, header):
    offset = header.code_size + header.sig_size
    size = header.cert_chain_size
    if header.supports_qti_signing():
        offset += header.sig_size_qti + header.cert_chain_size_qti
    data, seg = _extract_segment(data, offset, size)
    return data, seg


def extract_encryption_params(data, header, pad_max_sig_size=0, pad_max_cert_chain_size=0,
                              pad_max_encr_params_size=0):
    sig_size, cert_chain_size = header.sig_size, header.cert_chain_size
    offset = header.code_size + sig_size + cert_chain_size
    size = len(data) - offset
    if header.supports_qti_signing():
        sig_size_qti, cert_chain_size_qti = header.sig_size_qti, header.cert_chain_size_qti
        offset += sig_size_qti + cert_chain_size_qti
        size = len(data) - offset

        # QTI & OEM are zero, then encryption params must be zero
        if (sig_size, cert_chain_size, sig_size_qti, cert_chain_size_qti) == (0, 0, 0, 0) and size != 0 and header.get_version() < MBN_HDR_VERSION_6:
            raise RuntimeError('Encryption params must not exist for unsigned file.')

        # QTI is zero or OEM is zero
        elif (0, 0) in [(sig_size, cert_chain_size), (sig_size_qti, cert_chain_size_qti)]:
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
            potential_padding_size = pad_max_sig_size - min(sig_size, sig_size_qti)
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
