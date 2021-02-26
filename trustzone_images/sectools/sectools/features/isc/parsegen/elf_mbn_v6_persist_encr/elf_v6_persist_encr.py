# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen.mbn.versions.v5.parsegen_mbn import ParseGenMbnV5
from sectools.features.isc.advanced_defines import AUTHORITY_QTI
from sectools.features.isc.parsegen.elf_mbn_v3_base import ElfV3Base
from sectools.features.isc.defines import MAX_SIG_SIZE
from ..elf_mbn_v6.elf_v6 import ElfV6


class ElfV6PersistEncr(ElfV6):

    def __init__(self, data, parsegen_mbn_class=ParseGenMbnV5, **kwargs):

        # Check the arguments
        if kwargs['imageinfo'] is not None:
            elf_properties = kwargs['imageinfo'].image_type.elf_properties
        else:
            raise RuntimeError('imageinfo cannot be None')
        if elf_properties is None:
            raise RuntimeError('ELF properties must not be None.')

        self.persist_encr = elf_properties.persist_encr

        super(ElfV6PersistEncr, self).__init__(data, parsegen_mbn_class=parsegen_mbn_class, **kwargs)

    def _compute_hash_segment_size(self, sign, encrypt):
        if not (self.is_encrypted() and self.is_signed()):
            return super(ElfV6PersistEncr, self)._compute_hash_segment_size(sign, encrypt=encrypt)
        else:
            hash_table_len = ((len(self._get_hash('')) * 2) +
                              len(self.get_hash_table()))

            # Check configuration
            signatures = 1
            if self.oem_signing_enabled and self.qti_signing_enabled:
                signatures += 1

            if self.encryption_params and self.persist_encr:
                encrypt_params = len(self.encryption_params)
            elif (sign and signatures == 2 and self.oem_encryption_enabled) or encrypt:
                encrypt_params = len(self.encdec.get_encryption_parameters_blob())
            else:
                encrypt_params = 0

            cert_chain_size = 0
            if sign:
                if signatures == 1:
                    cert_chain_size = self.get_cert_chain_size(self.authority)
                else:
                    cert_chain_size = (self.get_cert_chain_size(self.authority) +
                                       self.get_cert_chain_size(
                                           self.get_other_authority(self.authority)))

            sig_size = MAX_SIG_SIZE * (signatures - 1) if sign else 0
            if sign:
                if ((self.authority != AUTHORITY_QTI and self._data_signature_qti) or
                        (self._data_signature_qti and self._data_signature)):
                    sig_size += len(self._data_signature_qti)
                else:
                    sig_size += self.sig_size

            # Sum
            return (self._mbn_parsegen.get_header(self.authority,
                                                  self.imageinfo,
                                                  self.validating,
                                                  self.signing,
                                                  self.add_signing_attr_to_hash_seg)
                    .get_size() + hash_table_len + sig_size + cert_chain_size +
                    encrypt_params)

    def get_hash_segment(self, sign, encrypt=None):
        if not (self.is_encrypted() and self.is_signed()):
            return super(ElfV6PersistEncr, self).get_hash_segment(sign, encrypt=encrypt)
        else:
            return self._get_data_int(sign, encrypt)

    def get_hash_segment_data_to_sign(self):
        if not (self.is_encrypted() and self.is_signed()):
            return super(ElfV6PersistEncr, self).get_hash_segment_data_to_sign()
        else:
            # Return data_to_sign of MBN hash segment, instead of that of ELF.
            return super(ElfV3Base, self).data_to_sign

    def check_if_file_encrypted(self, general_properties):
        if not (self.is_encrypted() and self.is_signed()):
            return super(ElfV6PersistEncr, self).check_if_file_encrypted(general_properties)
        else:
            #simply return to ensure decryption is not done for this use case
            pass

    def get_hash_segment_format(self):
        if not(self.is_encrypted() and self.is_signed()):
            return super(ElfV6PersistEncr, self).get_hash_segment_format()
        else:
            # Return format of the Hash Table Segment
            return super(ElfV3Base, self).data_to_sign_format