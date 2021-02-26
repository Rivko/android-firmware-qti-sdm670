# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen.mbn.versions.v5.parsegen_mbn import ParseGenMbnV5
from sectools.features.isc.advanced_defines import AUTHORITY_QTI
from sectools.features.isc.defines import AUTHORITY_OEM, MAX_SIG_SIZE
from ..elf_mbn_v3_encdec.elf_v3_encdec import ElfV3EncDec


class ElfV5(ElfV3EncDec):

    def __init__(self, data, parsegen_mbn_class=ParseGenMbnV5, imageinfo=None, **kwargs):

        # set qti_signing_enabled
        if imageinfo is None:
            self.qti_signing_enabled = False
        else:
            try:
                self.qti_signing_enabled = imageinfo.general_properties.qti_sign
            except AttributeError:
                self.qti_signing_enabled = False

        super(ElfV5, self).__init__(
            data, parsegen_mbn_class=parsegen_mbn_class, imageinfo=imageinfo, **kwargs)

    def get_other_authority(self, authority):
        if authority == AUTHORITY_OEM:
            return AUTHORITY_QTI
        elif authority == AUTHORITY_QTI:
            return AUTHORITY_OEM
        else:
            raise RuntimeError("Cannot get other authority for unknown authority")

    def _compute_hash_segment_size(self, sign, encrypt):
        hash_table_len = ((len(self._get_hash('')) * 2) +
                          len(self.get_hash_table()))

        # Check configuration
        signatures = 1
        if self.oem_signing_enabled and self.qti_signing_enabled:
            signatures += 1

        if (sign and signatures == 2 and self.oem_encryption_enabled) or encrypt:
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

    #--------------------------------------------------------------------------
    # Get function for cert_chain_size
    #--------------------------------------------------------------------------
    def get_cert_chain_size(self, authority):
        if authority is None:
            raise RuntimeError("Cannot get certificate chain size of unknown authority")
        num_certs_in_certchain = (self.num_certs_in_certchain if self.max_num_certs_in_certchain is None
                                  else self.max_num_certs_in_certchain)
        # get cert_chain_size of qti chert chain
        if authority == AUTHORITY_QTI:
            # qti sign does not currently support MRC
            return self.max_cert_size * num_certs_in_certchain
        # get cert_chain_size of oem chert chain
        else:
            num_root_certs = self.num_root_certs if self.max_num_root_certs is None else self.max_num_root_certs
            return self.max_cert_size * (num_certs_in_certchain + num_root_certs - 1)
