# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen import PAD_BYTE_1
from sectools.common.parsegen.mbn import MBN_HDRS
from sectools.common.parsegen.mbn import get_header
from sectools.common.parsegen.mbn.utils import _BackupMbnParsegen
from sectools.common.parsegen.mbn.versions.v3 import MBN_HDR_VERSION_3
from sectools.common.parsegen.mbn.versions.v3.parsegen_mbn import ParseGenMbnV3
from sectools.common.parsegen.mbn.versions.v3.utils import V3Utils
from sectools.common.utils.c_logging import logger
from sectools.features.isc.defines import AUTHORITY_OEM
from sectools.features.isc.defines import SECBOOT_VERSION_1_0
from sectools.features.isc.parsegen.base import SecParseGenBase

SECBOOT_MBN_HDR_VERSIONS = {
    SECBOOT_VERSION_1_0: MBN_HDR_VERSION_3
}


def copy_header(header):
    # mbn v3 header has fixed sizes 40/80 bytes.
    size = header.get_size()
    version = header.get_version()
    data = header.pack()
    try:
        return get_header[version](size, version, data)
    except KeyError:
        raise RuntimeError(
            'Unexpected header size, version provided:\n' +
            '    Header size - ' + str(size) + ', version - ' +
            str(version) + '\n' + '    Supported header types: ' +
            str(MBN_HDRS.keys()))


class V3DataToSignCtxMgr(object):
    """ MBN version 3 data_to_sign API context manager. """
    def __init__(self, mbn):
        self.mbn = mbn

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        # Set padding info
        if self.mbn.validating:
            # While validating, use the size/length of the actual image signature.
            sig_size = len(self.mbn.data_signature)
        else:
            # Otherwise, use the signature size derived from config.
            sig_size = self.mbn.sig_size

        self.mbn._mbn_parsegen.set_pad_info(
            sig_size, self.mbn.get_cert_chain_size(self.mbn.authority))

        # Get the data to sign (header + code)
        header = self.mbn._mbn_parsegen.get_header(
            self.mbn.authority, self.mbn.imageinfo, self.mbn.validating,
            self.mbn.signing, self.mbn.add_signing_attr_to_hash_seg)
        # Copy the MBN header so that masking doesn't alter original header
        self.header = copy_header(header)


class MbnV3Base(object):

    utils = V3Utils

    def __init__(self, data,
                 parsegen_mbn_class=ParseGenMbnV3,
                 imageinfo=None,
                 mbn_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 validating=False,
                 signing=False,
                 parsegens=None,
                 sign_attr=False):

        super(MbnV3Base, self).__init__(data,
                                        imageinfo=imageinfo,
                                        general_properties=general_properties,
                                        encdec=encdec,
                                        debug_dir=debug_dir,
                                        debug_prefix=debug_prefix,
                                        debug_suffix=debug_suffix,
                                        validating=validating,
                                        signing=signing,
                                        parsegens=parsegens,
                                        sign_attr=sign_attr)

        # Check the arguments
        if imageinfo is not None:
            mbn_properties = imageinfo.image_type.mbn_properties
        if mbn_properties is None:
            raise RuntimeError('MBN properties must not be None.')

        # Set padding based on authorities
        pad_max_sig_size, pad_max_cc_size = self.padding_size_per_authority

        mbn_version = self.secboot_to_mbn_header_version
        logger.debug("Expected MBN header version is " + str(mbn_version))
        # MBN version check for validation use case
        (valid_mbn_hdr_version_found,
         extracted_mbn_header_ver,
         extracted_mbn_header_size) = self.utils.get_mbn_header_info(data)
        if mbn_version != extracted_mbn_header_ver:
            if self.validating:
                raise RuntimeError(
                    "Expected MBNv{0} does not match extracted MBNv{1} from the image"
                        .format(mbn_version, extracted_mbn_header_ver))

            logger.debug("Extracted MBN header version is " + str(extracted_mbn_header_ver))
            if mbn_version < extracted_mbn_header_ver:
                # Downscaling is not supported.
                raise RuntimeError("Downscaling MBNv{0} image to MBNv{1} image is not supported"
                                   .format(extracted_mbn_header_ver, mbn_version))
            # Upscale MBN version if expected MBN version is greater than
            # the extracted MBN version while signing and other use cases
            if not valid_mbn_hdr_version_found:
                # Upscaling to an unknown MBN version is not supported
                raise RuntimeError("Upscaling MBNv{0} header to MBNv{1} header is not supported"
                                   .format(extracted_mbn_header_ver, mbn_version))
            # Supported upscaling
            logger.debug("Expected MBNv{0} does not match extracted MBNv{1} from the image"
                         .format(mbn_version, extracted_mbn_header_ver))
            if len(data) < extracted_mbn_header_size:
                raise RuntimeError("Hash segment size of {0}B is smaller than expected size of {1}B"
                                   .format(len(data), extracted_mbn_header_size))
            else:
                hdr = data[0:extracted_mbn_header_size]
                body = data[extracted_mbn_header_size:]
                hdr = self.utils.upscale_mbn_header(extracted_mbn_header_ver, mbn_version, hdr)
                data = hdr + body

        # Initialize the mbn parsegen
        self._mbn_parsegen = parsegen_mbn_class(data=data,
                                                header_size=mbn_properties.header_size,
                                                version=mbn_version,
                                                debug_dir=debug_dir,
                                                debug_prefix=debug_prefix,
                                                debug_suffix=debug_suffix,
                                                pad_max_sig_size=pad_max_sig_size,
                                                pad_max_cert_chain_size=pad_max_cc_size,
                                                pad_max_encr_params_size=self.pad_max_ep_size,
                                                general_properties=general_properties)

        self.code_size = self._mbn_parsegen.code_size

        # Sig or cert chain should not exist when corresponding authority permissions are disabled
        self.validate_image_sig()

        self.authority_signing()

        # If incoming image is signed, validate the header version
        if self.is_signed():
            self._mbn_parsegen.header.validate()

    #--------------------------------------------------------------------------
    # Methods used by __init__() constructor
    #--------------------------------------------------------------------------
    def authority_signing(self):
        # Backup authority
        authority = self.authority

        # Set the OEM signature/OEM cert chain
        self.authority = AUTHORITY_OEM
        self.data_signature = self._mbn_parsegen.sign
        self.cert_chain = self._mbn_parsegen.cert_chain

        # Restore authority
        self.authority = authority

    @property
    def padding_size_per_authority(self):
        return (0, 0)

    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    def validate_image_sig(self):
        # If sig or cert chain are present when corresponding authority permissions are disabled, raise error
        if not self.oem_signing_enabled and (self._mbn_parsegen.sign or self._mbn_parsegen.cert_chain):
            raise RuntimeError('OEM signing is not enabled but OEM signature/cert chain are detected in image.')

    @property
    def data_signature(self):
        # Get the signature based on the authority
        return self._data_signature

    @data_signature.setter
    def data_signature(self, signature):
        # Validate the signature
        if signature:
            self.validate_data_signature(signature)
            self.validate_authority()
        self._data_signature = signature

    @property
    def cert_chain(self):
        return self._cert_chain

    @cert_chain.setter
    def cert_chain(self, cert_chain):
        # Validate the cert chain
        if cert_chain:
            self.validate_cert_chain(cert_chain)
            self.validate_authority()
            if self.pad_cert_chain:
                cert_chain = cert_chain.ljust(self.get_cert_chain_size(self.authority), PAD_BYTE_1)

        self._cert_chain = cert_chain

    def is_signed(self, authority=None):
        # Only OEM authority signing is supported and expected.
        return (self._data_signature, self._cert_chain) != ('', '')

    @property
    def pad_max_ep_size(self):
        return 0

    def get_signing_assets(self):
        retval = []
        authority = self.authority
        if self.oem_signing_enabled:
            self.authority = AUTHORITY_OEM
            retval.append((AUTHORITY_OEM, self.data_to_sign, self._data_signature,
                           self._cert_chain, None))
        self.authority = authority
        return retval

    def get_data(self, integrity_check=None, sign=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign

        # Allow base to do any checks
        SecParseGenBase.get_data(self, integrity_check, sign)
        if integrity_check:
            raise RuntimeError('Mbn Images do not support integrity check.')
        return self._get_data_int(sign)

    @property
    def data_to_sign(self):
        # Backup the parsegen
        backup = _BackupMbnParsegen(self._mbn_parsegen, "sign", "cert_chain")

        with V3DataToSignCtxMgr(self) as ctx:
            pass

        retval = ctx.header.pack() + self._mbn_parsegen.code

        # Clear padding info
        self._mbn_parsegen.set_pad_info(0, 0)

        # Restore the parsegen
        backup.restore(self._mbn_parsegen, "sign", "cert_chain")

        return retval

    @property
    def data_to_sign_format(self):
        # Backup the parsegen
        backup = _BackupMbnParsegen(self._mbn_parsegen, "sign", "cert_chain")

        # Get the format of the data to sign
        with V3DataToSignCtxMgr(self) as ctx:
            pass
        fmt = ctx.header.get_format_description()

        # Clear padding info
        self._mbn_parsegen.set_pad_info(0, 0)

        # Restore the parsegen
        backup.restore(self._mbn_parsegen, "sign", "cert_chain")

        return fmt

    def validate_authority(self):
        if self.authority != AUTHORITY_OEM:
            raise RuntimeError('Invalid authority for Secure Boot %s images: %s' %
                               (self.secboot_version, self.authority))
        elif not self.oem_signing_enabled:
            raise RuntimeError('Image operation is not been enabled for OEM')

    def contains_integrity_check(self):
        return False

    #--------------------------------------------------------------------------
    # Helper methods
    #--------------------------------------------------------------------------
    @property
    def has_mbn_parsegen(self):
        return hasattr(self, "_mbn_parsegen")

    @property
    def secboot_to_mbn_header_version(self):
        return SECBOOT_MBN_HDR_VERSIONS[self.secboot_version]

    def _update_mbn_header_(self, header, image_dest_ptr, mbn_version):
        try:
            self.utils.update_mbn_header(header, image_dest_ptr, mbn_version)
        except KeyError:
            raise RuntimeError('Unsupported MBN header version: ' + str(mbn_version))

    def _get_data_int(self, sign):
        # Backup the parsegen
        backup = _BackupMbnParsegen(self._mbn_parsegen)

        # Update the security attributes per the flags
        if sign:
            self._mbn_parsegen.sign = self._data_signature
            self._mbn_parsegen.cert_chain = self._cert_chain

        self._mbn_parsegen.pad_max_encr_params_size = 0

        # Set padding info
        if sign:
            self._mbn_parsegen.set_pad_info(
                self.sig_size, self.get_cert_chain_size(self.authority))

        # Get the signed data
        retval = self._mbn_parsegen.get_data(
            self.authority, self.imageinfo, self.validating, self.signing,
            self.add_signing_attr_to_hash_seg)

        # Clear padding info
        self._mbn_parsegen.set_pad_info(0, 0)

        # Restore the parsegen
        backup.restore(self._mbn_parsegen, "sign", "cert_chain")

        return retval

    def update_invalidate_pointers_state(self, update_state):
        self._mbn_parsegen.invalidate_pointers = update_state
