# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen import PAD_BYTE_1
from sectools.common.parsegen.mbn.utils import _BackupMbnParsegen
from sectools.common.parsegen.mbn.versions.v5 import MBN_HDR_VERSION_5
from sectools.common.parsegen.mbn.versions.v5.utils import V5Utils
from sectools.common.parsegen.mbn.versions.v5.parsegen_mbn import ParseGenMbnV5
from sectools.features.isc.defines import AUTHORITY_OEM, MAX_SIG_SIZE
from sectools.features.isc.advanced_defines import AUTHORITY_QTI
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_2_0

from ..elf_mbn_v3_base.mbn_v3_base import V3DataToSignCtxMgr
from ..elf_mbn_v3_base.mbn_v3_base import SECBOOT_MBN_HDR_VERSIONS
from ..elf_mbn_v3_encdec.mbn_v3_encdec import MbnV3EncDec

SECBOOT_MBN_HDR_VERSIONS[SECBOOT_VERSION_2_0] = MBN_HDR_VERSION_5


class V5DataToSignCtxMgr(V3DataToSignCtxMgr):
    """ MBN version 5 data_to_sign API context manager. """
    def __enter__(self):
        # Set the current attributes
        if self.mbn.authority == AUTHORITY_QTI:
            self.mbn._mbn_parsegen.sign = self.mbn._data_signature
            self.mbn._mbn_parsegen.cert_chain = self.mbn._cert_chain

        self.mbn._mbn_parsegen.sign_qti = self.mbn._data_signature_qti
        self.mbn._mbn_parsegen.cert_chain_qti = self.mbn._cert_chain_qti

        return super(V5DataToSignCtxMgr, self).__enter__()


class MbnV5(MbnV3EncDec):

    utils = V5Utils

    def __init__(self, data, parsegen_mbn_class=ParseGenMbnV5, **kwargs):

        # Private variables
        self._data_signature_qti = ''
        self._cert_chain_qti = ''

        super(MbnV5, self).__init__(
            data, parsegen_mbn_class=parsegen_mbn_class, **kwargs)

    #--------------------------------------------------------------------------
    # Multiple authority-related overrides
    #--------------------------------------------------------------------------
    def authority_signing(self):
        # Backup authority
        authority = self.authority

        # Set the QTI signature/QTI cert chain if QTI signing is enabled
        # Needs to be done before getting OEM data
        self.authority = AUTHORITY_QTI
        self.data_signature = self._mbn_parsegen.sign_qti
        self.cert_chain = self._mbn_parsegen.cert_chain_qti

        # Set the OEM signature/OEM cert chain
        self.authority = AUTHORITY_OEM
        self.data_signature = self._mbn_parsegen.sign
        self.cert_chain = self._mbn_parsegen.cert_chain

        # Restore authority
        self.authority = authority

    @property
    def padding_size_per_authority(self):
        if self.qti_signing_enabled and self.oem_signing_enabled:
            return (MAX_SIG_SIZE, self.get_cert_chain_size(AUTHORITY_OEM))
        else:
            return (0, 0)

    def validate_image_sig(self):
        # If sig or cert chain are present when corresponding authority permissions are disabled, raise error
        if not self.oem_signing_enabled and (self._mbn_parsegen.sign or self._mbn_parsegen.cert_chain):
            raise RuntimeError('OEM signing is not enabled but OEM signature/cert chain are detected in image.')
        if not self.qti_signing_enabled and (self._mbn_parsegen.sign_qti or self._mbn_parsegen.cert_chain_qti):
            raise RuntimeError('QTI signing is not enabled but QTI signature/cert chain are detected in image.')

    @property
    def data_signature(self):
        # Get the signature based on the authority
        if self.authority == AUTHORITY_QTI:
            return self._data_signature_qti
        return self._data_signature

    @data_signature.setter
    def data_signature(self, signature):
        # Validate the signature
        if signature:
            self.validate_data_signature(signature)
            self.validate_authority()

        # Set the signature based on the authority
        if self.authority == AUTHORITY_QTI:
            self._data_signature_qti = signature
        else:
            if signature and self.qti_signing_enabled and not self._data_signature_qti:
                raise RuntimeError('Image must be signed by QTI before signing as OEM')
            self._data_signature = signature

    @property
    def cert_chain(self):
        # Get the cert chain based on the authority
        if self.authority == AUTHORITY_QTI:
            return self._cert_chain_qti
        return self._cert_chain

    @cert_chain.setter
    def cert_chain(self, cert_chain):
        # Validate the cert chain
        if cert_chain:
            self.validate_cert_chain(cert_chain)
            self.validate_authority()
            if self.pad_cert_chain:
                cert_chain = cert_chain.ljust(self.get_cert_chain_size(self.authority), PAD_BYTE_1)

        # Set the cert chain based on the authority
        if self.authority == AUTHORITY_QTI:
            self._cert_chain_qti = cert_chain
        else:
            self._cert_chain = cert_chain

    def is_signed(self, authority=None):
        qti_signed = (self._data_signature_qti, self._cert_chain_qti) != ('', '')
        oem_signed = (self._data_signature, self._cert_chain) != ('', '')
        if authority == AUTHORITY_OEM:
            return oem_signed
        elif authority == AUTHORITY_QTI:
            return qti_signed
        return qti_signed or oem_signed

    @property
    def pad_max_ep_size(self):
        if hasattr(self, "encdec") and self.encdec is not None:
            return len(self.encdec.get_encryption_parameters_blob())
        return 0

    def get_signing_assets(self):
        retval = []
        authority = self.authority
        if self.oem_signing_enabled:
            self.authority = AUTHORITY_OEM
            retval.append((AUTHORITY_OEM, self.data_to_sign, self._data_signature,
                           self._cert_chain, None))
        if self.qti_signing_enabled:
            self.authority = AUTHORITY_QTI
            retval.append((AUTHORITY_QTI, self.data_to_sign, self._data_signature_qti,
                           self._cert_chain_qti, None))
        self.authority = authority
        return retval

    @property
    def data_to_sign(self):
        # Backup the parsegen
        backup = _BackupMbnParsegen(
            self._mbn_parsegen, "sign", "cert_chain", "encryption_params",
            "sign_qti", "cert_chain_qti")

        with V5DataToSignCtxMgr(self) as ctx:
            pass

        retval = ctx.header.pack() + self._mbn_parsegen.code

        # Clear padding info
        self._mbn_parsegen.set_pad_info(0, 0)

        # Restore the parsegen
        backup.restore(
            self._mbn_parsegen, "sign", "cert_chain", "encryption_params",
            "sign_qti", "cert_chain_qti")

        return retval

    @property
    def data_to_sign_format(self):
        # Backup the parsegen
        backup = _BackupMbnParsegen(
            self._mbn_parsegen, "sign", "cert_chain", "encryption_params",
            "sign_qti", "cert_chain_qti")

        # Get the format of the data to sign
        with V5DataToSignCtxMgr(self) as ctx:
            pass
        fmt = ctx.header.get_format_description()

        # Clear padding info
        self._mbn_parsegen.set_pad_info(0, 0)

        # Restore the parsegen
        backup.restore(
            self._mbn_parsegen, "sign", "cert_chain", "encryption_params",
            "sign_qti", "cert_chain_qti")

        return fmt

    def validate_authority(self):
        if not self.oem_signing_enabled and self.authority == AUTHORITY_OEM:
            raise RuntimeError('Image operation has not been enabled for OEM')
        if self.authority == AUTHORITY_QTI:
            if not self.qti_signing_enabled:
                raise RuntimeError('Image operation has not been enabled for QTI')

    def _get_data_int(self, sign, encrypt):
        # Backup the parsegen
        backup = _BackupMbnParsegen(
            self._mbn_parsegen, "sign", "cert_chain", "encryption_params",
            "sign_qti", "cert_chain_qti")
        backup_encrypt_pad = self._mbn_parsegen.pad_max_encr_params_size

        # Update the security attributes per the flags
        if sign:
            self._mbn_parsegen.sign = self._data_signature
            self._mbn_parsegen.cert_chain = self._cert_chain
            self._mbn_parsegen.sign_qti = self._data_signature_qti
            self._mbn_parsegen.cert_chain_qti = self._cert_chain_qti

        if encrypt or (self.oem_signing_enabled and self.qti_signing_enabled):
            self._mbn_parsegen.encryption_params = self.encryption_params
        else:
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
        backup.restore(
            self._mbn_parsegen, "sign", "cert_chain", "encryption_params",
            "sign_qti", "cert_chain_qti")
        self._mbn_parsegen.pad_max_encr_params_size = backup_encrypt_pad

        return retval
