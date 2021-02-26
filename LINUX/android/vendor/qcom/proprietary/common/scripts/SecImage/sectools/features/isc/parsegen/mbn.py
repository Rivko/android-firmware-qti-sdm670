# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Nov 25, 2013

@author: hraghav
'''

from sectools.common.parsegen import mbn, PAD_BYTE_0, PAD_BYTE_1
from sectools.common.parsegen.mbn.header import MBN_HDR_VERSION_3, MBN_HDR_VERSION_5
from sectools.common.utils.c_logging import logger
from sectools.features.isc import defines
from sectools.features.isc.parsegen.base import SecParseGenBase

MBN_FILE_TYPE = 'mbn'
MAX_SIG_SIZE = 512
SECBOOT_MBN_HDR = {
    defines.SECBOOT_VERSION_1_0: MBN_HDR_VERSION_3,
    defines.SECBOOT_VERSION_2_0: MBN_HDR_VERSION_5
}


class SecParseGenMbn(SecParseGenBase):

    def __init__(self, data, imageinfo=None, mbn_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,):
        SecParseGenBase.__init__(self, data, imageinfo, general_properties,
                                 encdec, debug_dir, debug_prefix, debug_suffix)

        # Check the arguments
        if imageinfo is not None:
            mbn_properties = imageinfo.image_type.mbn_properties
        if mbn_properties is None:
            raise RuntimeError('MBN properties must not be None.')

        # Set padding based on authorities
        if self.qc_signing_enabled and self.oem_signing_enabled:
            pad_max_sig_size, pad_max_cc_size = MAX_SIG_SIZE, self.cert_chain_size
        else:
            pad_max_sig_size, pad_max_cc_size = 0, 0
        pad_max_ep_size = len(self.encdec.get_encryption_parameters_blob()) if self.encdec is not None else 0

        # Initialize the mbn parsegen
        self._mbn_parsegen = mbn.ParseGenMbn(data,
                                             mbn_properties.header_size,
                                             SECBOOT_MBN_HDR[self.secboot_version],
                                             self.debug_dir,
                                             self.debug_prefix,
                                             self.debug_suffix,
                                             pad_max_sig_size, pad_max_cc_size, pad_max_ep_size
                                             )

        # Sig or cert chain should not exist when corresponding authority permissions are disabled
        self.validate_image_sig()

        # Private variables
        self._data_signature_qc = ''
        self._cert_chain_qc = ''

        # Extract encryption params
        self.encryption_params = self._mbn_parsegen.encryption_params

        # Backup authority
        authority = self.authority

        # Set the QC signature/QC cert chain if QC signing is enabled
        # Needs to be done before getting OEM data
        self.authority = defines.AUTHORITY_QC
        self.data_signature = self._mbn_parsegen.sign_qc
        self.cert_chain = self._mbn_parsegen.cert_chain_qc

        # Set the OEM signature/OEM cert chain
        self.authority = defines.AUTHORITY_OEM
        self.data_signature = self._mbn_parsegen.sign
        self.cert_chain = self._mbn_parsegen.cert_chain

        # Restore authority
        self.authority = authority

        # If incoming image is signed or encrypted, validate the header version
        if self.is_signed() or self.is_encrypted():
            self._mbn_parsegen.header.validate()

    def __repr__(self):
        return ('Base Properties: ' + '\n' + SecParseGenBase.__repr__(self) + '\n'
                'MBN Properties: ' + '\n' + repr(self._mbn_parsegen))

    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def file_type(cls):
        return MBN_FILE_TYPE

    def validate_authority(self):
        # Check invalid signing
        if not self.oem_signing_enabled and self.authority == defines.AUTHORITY_OEM:
            raise RuntimeError('Image operation has not been enabled for OEM')
        if self.authority == defines.AUTHORITY_QC:
            if not self.qc_signing_enabled:
                raise RuntimeError('Image operation has not been enabled for QC')
            if self.secboot_version != defines.SECBOOT_VERSION_2_0:
                raise RuntimeError('QC cannot operate secboot version ' + str(self.secboot_version) + ' images.')

    def validate_image_sig(self):
        # If sig or cert chain are present when corresponding authority permissions are disabled, raise error
        if not self.oem_signing_enabled and (self._mbn_parsegen.sign or self._mbn_parsegen.cert_chain):
            raise RuntimeError('OEM signing is not enabled but OEM signature/cert chain are detected in image.')
        if not self.qc_signing_enabled and (self._mbn_parsegen.sign_qc or self._mbn_parsegen.cert_chain_qc):
            raise RuntimeError('QC signing is not enabled but QC signature/cert chain are detected in image.')

    @property
    def data_signature(self):
        # Get the signature based on the authority
        if self.authority == defines.AUTHORITY_QC:
            return self._data_signature_qc
        return self._data_signature

    @data_signature.setter
    def data_signature(self, signature):
        # Validate the signature
        if signature:
            self.validate_data_signature(signature)
            self.validate_authority()

        # Set the signature based on the authority
        if self.authority == defines.AUTHORITY_QC:
            self._data_signature_qc = signature
        else:
            if signature and self.qc_signing_enabled and not self._data_signature_qc:
                raise RuntimeError('Image must be signed by QC before signing as OEM')
            self._data_signature = signature

    @property
    def cert_chain(self):
        # Get the cert chain based on the authority
        if self.authority == defines.AUTHORITY_QC:
            return self._cert_chain_qc
        return self._cert_chain

    @cert_chain.setter
    def cert_chain(self, cert_chain):
        # Validate the cert chain
        if cert_chain:
            self.validate_cert_chain(cert_chain)
            self.validate_authority()
            if self.pad_cert_chain:
                cert_chain = cert_chain.ljust(self.cert_chain_size, PAD_BYTE_1)

        # Set the cert chain based on the authority
        if self.authority == defines.AUTHORITY_QC:
            self._cert_chain_qc = cert_chain
        else:
            self._cert_chain = cert_chain

    def is_signed(self, authority=None):
        qc_signed = (self._data_signature_qc, self._cert_chain_qc) != ('', '')
        oem_signed = (self._data_signature, self._cert_chain) != ('', '')
        if authority == defines.AUTHORITY_OEM:
            return oem_signed
        elif authority == defines.AUTHORITY_QC:
            return qc_signed
        return qc_signed or oem_signed

    def get_signing_assets(self):
        retval = []
        authority = self.authority
        if self.oem_signing_enabled:
            self.authority = defines.AUTHORITY_OEM
            retval.append((defines.AUTHORITY_OEM, self.data_to_sign, self._data_signature, self._cert_chain))
        if self.qc_signing_enabled:
            self.authority = defines.AUTHORITY_QC
            retval.append((defines.AUTHORITY_QC, self.data_to_sign, self._data_signature_qc, self._cert_chain_qc))
        self.authority = authority
        return retval

    def get_data(self, integrity_check=None, sign=None, encrypt=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt

        # Allow base to do any checks
        SecParseGenBase.get_data(self, integrity_check, sign, encrypt)
        if integrity_check:
            raise RuntimeError('Mbn Images do not support integrity check.')
        if encrypt:
            raise RuntimeError('Mbn Images do not support encryption.')
        return self._get_data_int(sign, False)

    @property
    def data_to_sign(self):
        # Backup the parsegen
        backup = _BackupMbnParsegen(self._mbn_parsegen)

        # Set the current attributes
        if self.authority == defines.AUTHORITY_QC:
            self._mbn_parsegen.sign = self._data_signature
            self._mbn_parsegen.cert_chain = self._cert_chain

        self._mbn_parsegen.sign_qc = self._data_signature_qc
        self._mbn_parsegen.cert_chain_qc = self._cert_chain_qc

        # Set padding info
        self._mbn_parsegen.set_pad_info(self.sig_size, self.cert_chain_size)

        # Update version number before applying signature #
        if SECBOOT_MBN_HDR[self.secboot_version] == self._mbn_parsegen.header.get_version():
            logger.debug("Updating version to " + str(self._mbn_parsegen.header.get_version()))
            self._mbn_parsegen.header.set_version_in_bin(self._mbn_parsegen.header.get_version())
        else:
            raise RuntimeError('Version check failed. MBN header version (%s) does not match expected version (%s).'
                               % (str(self._mbn_parsegen.header.get_version()), str(SECBOOT_MBN_HDR[self.secboot_version])))

        # Get the data to sign (header + code)
        retval = self._mbn_parsegen.get_header(self.authority).pack() + self._mbn_parsegen.code

        # Clear padding info
        self._mbn_parsegen.set_pad_info(0, 0)

        # Restore the parsegen
        backup.restore(self._mbn_parsegen)

        return retval

    def contains_integrity_check(self):
        return False

    #--------------------------------------------------------------------------
    # Helper methods
    #--------------------------------------------------------------------------
    def _get_data_int(self, sign, encrypt):
        # Backup the parsegen
        backup = _BackupMbnParsegen(self._mbn_parsegen)
        backup_encrypt_pad = self._mbn_parsegen.pad_max_encr_params_size

        # Update the security attributes per the flags
        if sign:
            self._mbn_parsegen.sign = self._data_signature
            self._mbn_parsegen.cert_chain = self._cert_chain
            self._mbn_parsegen.sign_qc = self._data_signature_qc
            self._mbn_parsegen.cert_chain_qc = self._cert_chain_qc

        if encrypt or (self.oem_signing_enabled and self.qc_signing_enabled):
            self._mbn_parsegen.encryption_params = self.encryption_params
        else:
            self._mbn_parsegen.pad_max_encr_params_size = 0

        # Set padding info
        if sign:
            self._mbn_parsegen.set_pad_info(self.sig_size, self.cert_chain_size)

        # Get the signed data
        retval = self._mbn_parsegen.get_data(self.authority)

        # Clear padding info
        self._mbn_parsegen.set_pad_info(0, 0)

        # Restore the parsegen
        backup.restore(self._mbn_parsegen)
        self._mbn_parsegen.pad_max_encr_params_size = backup_encrypt_pad

        return retval


class _BackupMbnParsegen(object):

    def __init__(self, parsegen):
        self.backup(parsegen)

    def backup(self, parsegen):
        # Backup the original
        self.signature_qc = parsegen.sign_qc
        self.cert_chain_qc = parsegen.cert_chain_qc
        self.signature = parsegen.sign
        self.cert_chain = parsegen.cert_chain
        self.encryption_params = parsegen.encryption_params

        # Clear the original
        parsegen.sign_qc = ''
        parsegen.cert_chain_qc = ''
        parsegen.sign = ''
        parsegen.cert_chain = ''
        parsegen.encryption_params = ''

        return parsegen

    def restore(self, parsegen):
        # Restore the original
        parsegen.sign_qc = self.signature_qc
        parsegen.cert_chain_qc = self.cert_chain_qc
        parsegen.sign = self.signature
        parsegen.cert_chain = self.cert_chain
        parsegen.encryption_params = self.encryption_params

        # Clear self
        self.signature_qc = ''
        self.cert_chain_qc = ''
        self.signature = ''
        self.cert_chain = ''
        self.encryption_params = ''

        return parsegen
