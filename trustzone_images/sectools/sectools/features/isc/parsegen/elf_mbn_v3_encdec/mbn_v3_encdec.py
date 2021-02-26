# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen.mbn.utils import _BackupMbnParsegen
from sectools.common.parsegen.mbn.versions.v3_encdec.utils import V3EncDecUtils
from sectools.common.parsegen.mbn.versions.v3_encdec.parsegen_mbn import ParseGenMbnV3EncDec
from sectools.features.isc.parsegen.base import SecParseGenBase

from ..elf_mbn_v3_base.mbn_v3_base import MbnV3Base


class MbnV3EncDec(MbnV3Base):

    utils = V3EncDecUtils

    def __init__(self, data, parsegen_mbn_class=ParseGenMbnV3EncDec, **kwargs):
        super(MbnV3EncDec, self).__init__(
            data, parsegen_mbn_class=parsegen_mbn_class, **kwargs)

        # Extract encryption params
        self.encryption_params = self._mbn_parsegen.encryption_params

        # If incoming image is signed or encrypted, validate the header version
        if self.is_signed() or self.is_encrypted():
            self._mbn_parsegen.header.validate()

    def get_data(self, integrity_check=None, sign=None, encrypt=None):
        # Resolve the operation
        integrity_check = self.integrity_check if integrity_check is None else integrity_check
        sign = self.sign if sign is None else sign
        encrypt = self.encrypt if encrypt is None else encrypt

        # Allow base to do any checks
        SecParseGenBase.get_data(self, integrity_check, sign, encrypt=encrypt)
        if integrity_check:
            raise RuntimeError('Mbn Images do not support integrity check.')
        if encrypt:
            raise RuntimeError('Mbn Images do not support encryption.')
        return self._get_data_int(sign, False)

    def _get_data_int(self, sign, encrypt):
        # Backup the parsegen
        backup = _BackupMbnParsegen(
            self._mbn_parsegen, "sign", "cert_chain", "encryption_params")
        backup_encrypt_pad = self._mbn_parsegen.pad_max_encr_params_size

        # Update the security attributes per the flags
        if sign:
            self._mbn_parsegen.sign = self._data_signature
            self._mbn_parsegen.cert_chain = self._cert_chain

        if encrypt:
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
            self._mbn_parsegen, "sign", "cert_chain", "encryption_params")
        self._mbn_parsegen.pad_max_encr_params_size = backup_encrypt_pad

        return retval
