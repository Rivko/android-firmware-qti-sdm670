# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen.mbn.utils import _BackupMbnParsegen
from sectools.common.parsegen.mbn.versions.v6 import MBN_HDR_VERSION_6
from sectools.common.parsegen.mbn.versions.v6.utils import V6Utils
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_3_0
from sectools.common.parsegen.mbn.versions.v6.parsegen_mbn import ParseGenMbnV6
from ..elf_mbn_v6.mbn_v6 import MbnV6
from ..elf_mbn_v3_base.mbn_v3_base import SECBOOT_MBN_HDR_VERSIONS


SECBOOT_MBN_HDR_VERSIONS[SECBOOT_VERSION_3_0] = MBN_HDR_VERSION_6

class MbnV6PersistEncr(MbnV6):

    utils = V6Utils

    def __init__(self, data, parsegen_mbn_class=ParseGenMbnV6, **kwargs):
            super(MbnV6, self).__init__(data, parsegen_mbn_class=parsegen_mbn_class, **kwargs)

    #--------------------------------------------------------------------------
    # Multiple authority-related overrides
    #--------------------------------------------------------------------------
    @property
    def pad_max_ep_size(self):
        if self.encdec is not None:
            return len(self.encdec.get_encryption_parameters_blob())
        elif self.encryption_params:
            return len(self.encryption_params)
        return 0

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

        if self.encryption_params:
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