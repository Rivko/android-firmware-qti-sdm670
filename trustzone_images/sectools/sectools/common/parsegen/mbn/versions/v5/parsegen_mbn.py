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
from sectools.common.parsegen.mbn import get_header
from sectools.common.parsegen.mbn.versions.v3.parsegen_mbn import MBN_HDR_OBJS
from sectools.common.parsegen.mbn.versions.v5 import MBN_HDR_VERSION_5
from sectools.common.parsegen.mbn.versions.v5.headers import MbnHdr40BV5
from sectools.common.utils.c_misc import create_mismatch_table
from sectools.common.utils.c_misc import hex_addr
from sectools.features.isc.advanced_defines import AUTHORITY_QTI
from sectools.features.isc.defines import ALIGNED_IMAGE_SIZE
from sectools.features.isc.defines import MAX_CERT_SIZE
from sectools.features.isc.defines import MBN_PTR_MAX
from .utils import V5Utils
from ..v3.parsegen_mbn import V3GetHdrCntxMgr
from ..v3_encdec.parsegen_mbn import ParseGenMbnV3EncDec

FILE_CERT_CHAIN_QTI_REMOVED = 'mbn_in_5_cert_chain_qti_removed'
FILE_CERT_CHAIN_QTI_IN      = 'mbn_in_5_cert_chain_qti'

FILE_SIGN_QTI_REMOVED       = 'mbn_in_6_sign_qti_removed'
FILE_SIGN_QTI_IN            = 'mbn_in_6_sign_qti'

if MbnHdr40BV5 not in MBN_HDR_OBJS:
    MBN_HDR_OBJS.append(MbnHdr40BV5)
    MBN_HDRS[(MbnHdr40BV5.get_size(), MbnHdr40BV5.get_version())] = MbnHdr40BV5

get_header[MBN_HDR_VERSION_5] = \
    lambda size, version, data=None: MBN_HDRS[(size, version)](data)


class V5GetHdrCntxMgr(V3GetHdrCntxMgr):

    def __init__(self, mbn, authority):
        super(V5GetHdrCntxMgr, self).__init__(mbn)
        self.authority = authority

    def qti_signing(self, single_signing):

        signing = self.mbn.pad_sig_size != 0

        # MBN v5 and v6 usecases: QTI.
        if self.hdr.supports_qti_signing():
            sig_size_qti, cert_chain_size_qti =\
                len(self.mbn.sign_qti), len(self.mbn.cert_chain_qti)
            self.hdr.sig_size_qti, self.hdr.cert_chain_size_qti =\
                sig_size_qti, cert_chain_size_qti
            if self.authority == AUTHORITY_QTI:
                self.hdr.sig_size, self.hdr.cert_chain_size = 0, 0

            # Single signing / exclusive usecase
            if single_signing:
                # Both QTI and OEM data cannot be set
                if self.sig_size and sig_size_qti:
                    raise RuntimeError(
                        'Both QTI & OEM signature must not be set')

                # If both are empty, use padding for data to sign
                if (self.sig_size, sig_size_qti) == (0, 0):
                    if self.authority == AUTHORITY_QTI:
                        sig_size_qti, cert_chain_size_qti =\
                            self.mbn.pad_sig_size, self.mbn.pad_cert_chain_size
                        self.hdr.sig_size_qti, self.hdr.cert_chain_size_qti =\
                            sig_size_qti, cert_chain_size_qti
                    else:
                        self.sig_size, self.cert_chain_size =\
                            self.mbn.pad_sig_size, self.mbn.pad_cert_chain_size
                        self.hdr.sig_size, self.hdr.cert_chain_size =\
                            self.sig_size, self.cert_chain_size

            # Double signing
            else:
                # Both QTI & OEM are empty, use padding to return to sign
                if (self.sig_size, sig_size_qti) == (0, 0):
                    if self.authority == AUTHORITY_QTI:
                        sig_size_qti, cert_chain_size_qti =\
                            self.mbn.pad_sig_size, self.mbn.pad_cert_chain_size
                        self.hdr.sig_size_qti, self.hdr.cert_chain_size_qti =\
                            sig_size_qti, cert_chain_size_qti

                        if signing:
                            self.sig_size = self.mbn.pad_max_sig_size
                            self.cert_chain_size = self.mbn.pad_max_cert_chain_size
                    else:
                        self.sig_size, self.cert_chain_size =\
                            self.mbn.pad_sig_size, self.mbn.pad_cert_chain_size
                        self.hdr.sig_size, self.hdr.cert_chain_size =\
                            self.sig_size, self.cert_chain_size

                        if signing:
                            sig_size_qti = self.mbn.pad_max_sig_size
                            cert_chain_size_qti = self.mbn.pad_max_cert_chain_size

                # QTI is empty
                elif (sig_size_qti, cert_chain_size_qti) == (0, 0):
                    if signing:
                        sig_size_qti = self.mbn.pad_max_sig_size
                        cert_chain_size_qti = self.mbn.pad_max_cert_chain_size
                    if self.authority == AUTHORITY_QTI:
                        self.hdr.sig_size_qti = self.mbn.pad_sig_size
                        self.hdr.cert_chain_size_qti = self.mbn.pad_cert_chain_size
                    else:
                        self.sig_size, self.cert_chain_size =\
                            self.mbn.pad_sig_size, self.mbn.pad_cert_chain_size
                        self.hdr.sig_size, self.hdr.cert_chain_size =\
                            self.sig_size, self.cert_chain_size

                # OEM is empty
                elif (self.sig_size, self.cert_chain_size) == (0, 0):
                    if signing:
                        self.sig_size = self.mbn.pad_max_sig_size
                        self.cert_chain_size = self.mbn.pad_max_cert_chain_size
                    if self.authority == AUTHORITY_QTI:
                        sig_size_qti, cert_chain_size_qti =\
                            self.mbn.pad_sig_size, self.mbn.pad_cert_chain_size
                        self.hdr.sig_size_qti, self.hdr.cert_chain_size_qti =\
                            sig_size_qti, cert_chain_size_qti
                    else:
                        self.hdr.sig_size = self.mbn.pad_sig_size
                        self.hdr.cert_chain_size = self.mbn.pad_cert_chain_size

                # Both are non empty
                else:
                    self.sig_size += self.mbn.pad_max_sig_size - self.sig_size

            # Add QTI sizes for the full size to be used for image size
            self.sig_size += sig_size_qti
            self.cert_chain_size += cert_chain_size_qti


class ParseGenMbnQtiSupport(object):

    def __init__(self):
        self._sign_qti = ''
        self._cert_chain_qti = ''

        if self.header.supports_qti_signing():
            # Extract the QTI cert chain
            data, self.cert_chain_qti = self.utils.extract_cert_chain_qti(
                self.header, self.code)
            self.store_debug_data(FILE_CERT_CHAIN_QTI_REMOVED, self.code)
            self.store_debug_data(FILE_CERT_CHAIN_QTI_IN, self.cert_chain_qti)

            # Extract the QTI signature
            self.code, self.sign_qti = self.utils.extract_sign_qti(self.header, self.code)
            self.store_debug_data(FILE_SIGN_QTI_REMOVED, self.code)
            self.store_debug_data(FILE_SIGN_QTI_IN, self.sign_qti)

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


class ParseGenMbnV5(ParseGenMbnV3EncDec, ParseGenMbnQtiSupport):

    utils = V5Utils

    def get_header(self, authority=None, imageinfo=None,
                      validating=False, signing=False,
                      add_signing_attr_to_hash_seg=False):
        with V5GetHdrCntxMgr(self, authority) as v5:
            v5.qti_signing(v5.mbn.pad_max_sig_size == 0)
        return v5.hdr

    def _compute_possible_image_sizes(self, hdr):
        possible_image_sizes = {}
        aligned_size = ALIGNED_IMAGE_SIZE(hdr.code_size + hdr.sig_size + hdr.cert_chain_size)
        possible_image_sizes[aligned_size] = None
        if hdr.supports_qti_signing():
            aligned_size = ALIGNED_IMAGE_SIZE(hdr.code_size + hdr.sig_size_qti + hdr.cert_chain_size_qti)
            possible_image_sizes[aligned_size] = None
            num_certs = int(hdr.cert_chain_size_qti / MAX_CERT_SIZE)
            for max_num_root_certs in (1, 2, 3, 4):
                cert_chain_size = MAX_CERT_SIZE * (num_certs + max_num_root_certs - 1)
                aligned_size = ALIGNED_IMAGE_SIZE(
                    hdr.code_size +
                    hdr.sig_size + hdr.cert_chain_size +
                    hdr.sig_size_qti + hdr.cert_chain_size_qti +
                    self.pad_max_sig_size - hdr.sig_size +
                    cert_chain_size - hdr.cert_chain_size)
                possible_image_sizes[aligned_size] = max_num_root_certs
        return possible_image_sizes

    @property
    def config_max_num_root_certs(self):
        if self.general_properties is None:
            return 0
        return self.general_properties.max_num_root_certs

    def validate_header(self, hdr):
        possible_image_sizes = self._compute_possible_image_sizes(hdr)
        try:
            extracted_max_num_root_certs = possible_image_sizes[hdr.image_size]
            if (self.config_max_num_root_certs and extracted_max_num_root_certs is not None and
                    extracted_max_num_root_certs != self.config_max_num_root_certs):
                error_str = []
                create_mismatch_table(
                    [("max_num_root_certs",
                      extracted_max_num_root_certs, self.config_max_num_root_certs)],
                    error_str, operation="signing", image_region="Hash Segment")
                raise RuntimeError("\n".join(error_str))
        except KeyError:
            # Check for QTI sign asset related errors
            qti_segment, qti_err_string = '', ''
            if hdr.supports_qti_signing():
                qti_segment = ('    ' + 'QTI Signature Size:       ' + hex_addr(hdr.sig_size_qti) + '\n'
                               '    ' + 'QTI Cert Chain Size:      ' + hex_addr(
                    hdr.cert_chain_size_qti) + '\n')

                # Handles case where incoming data header contains residual data in QTI sign/cc fields
                if (hdr.image_size + hdr.sig_size_qti + hdr.cert_chain_size_qti in possible_image_sizes or
                        hdr.cert_chain_size_qti == MBN_PTR_MAX):
                    qti_err_string = ('Unsigned hash table header must not contain junk data in '
                                      'QTI attribute fields when double-signing.' + '\n\n')
            raise RuntimeError("""MBN header verification failed. Image size does not match the sum of its segments:

    Code Size:                {header_code_size}
{qti_segment}
    Signature Size:           {signature_size}
    Cert Chain Size:          {cert_chain_size}
    Image Size (expected):   *{expected_image_size}
    Image Size (from file):  *{image_size}
    {qti_err_string}
Header:
{header}
    """.format(header_code_size=hex_addr(hdr.code_size), qti_segment=qti_segment,
               signature_size=hex_addr(hdr.sig_size),
               cert_chain_size=hex_addr(hdr.cert_chain_size),
               expected_image_size=(str([hex_addr(s) for s in possible_image_sizes]) +
                                    ' (Code + Signature + Cert Chain)'),
               image_size=hex_addr(hdr.image_size),
               qti_err_string=qti_err_string, header=repr(hdr)))

    def get_data(self, authority=None, imageinfo=None, validating=False,
                           signing=False, add_signing_attr_to_hash_seg=False):
        hdr = self.get_header(authority, imageinfo, validating, signing,
                             add_signing_attr_to_hash_seg)
        # add encryption params if they exist
        if self.encryption_params:
            encr_params_size = len(self.encryption_params)
        # pad space for encryption params if image is signed
        elif len(self.sign_qti) > 0 or len(self.sign) > 0:
            encr_params_size = self.pad_max_encr_params_size
        # don't pad space for encryption params if image is integrity image
        else:
            encr_params_size = 0

        return (hdr.pack() + self.code + ensure_binary(self.sign_qti) + ensure_binary(self.cert_chain_qti) +
                ensure_binary(self.sign) + ensure_binary(self.cert_chain) + ensure_binary(self.encryption_params)).ljust(
            int(hdr.get_size() + hdr.image_size + encr_params_size), PAD_BYTE_1)
