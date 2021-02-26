# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.utils.c_misc import create_mismatch_table
from sectools.common.parsegen.mbn.utils import _BackupMbnParsegen
from sectools.common.parsegen.mbn.versions.v6 import MBN_HDR_VERSION_6
from sectools.common.parsegen.mbn.versions.v6.utils import V6Utils
from sectools.common.parsegen.mbn.versions.v6.parsegen_mbn import ParseGenMbnV6
from sectools.features.isc.defines import AUTHORITY_OEM
from sectools.features.isc.advanced_defines import AUTHORITY_QTI
from sectools.features.isc.advanced_defines import SECBOOT_VERSION_3_0

from ..elf_mbn_v3_base.mbn_v3_base import SECBOOT_MBN_HDR_VERSIONS
from ..elf_mbn_v5.mbn_v5 import MbnV5
from ..elf_mbn_v5.mbn_v5 import V5DataToSignCtxMgr

SECBOOT_MBN_HDR_VERSIONS[SECBOOT_VERSION_3_0] = MBN_HDR_VERSION_6


class MbnV6(MbnV5):

    utils = V6Utils

    def __init__(self, data, parsegen_mbn_class=ParseGenMbnV6, **kwargs):
        super(MbnV6, self).__init__(
            data, parsegen_mbn_class=parsegen_mbn_class, **kwargs)

    def get_signing_assets(self):
        retval = []
        authority = self.authority
        if self.oem_signing_enabled:
            self.authority = AUTHORITY_OEM
            retval.append((AUTHORITY_OEM, self.data_to_sign, self._data_signature,
                           self._cert_chain,
                           self.get_hash_segment_metadata(AUTHORITY_OEM)))
        if self.qti_signing_enabled:
            self.authority = AUTHORITY_QTI
            retval.append((AUTHORITY_QTI, self.data_to_sign, self._data_signature_qti,
                           self._cert_chain_qti,
                           self.get_hash_segment_metadata(AUTHORITY_QTI)))
        self.authority = authority
        return retval

    def get_hash_segment_metadata(self, authority=None):
        if authority is None:
            authority = self.authority
        header = self._mbn_parsegen.header
        if getattr(header, "supports_metadata", lambda: False)():
            if authority == AUTHORITY_OEM:
                return header.get_metadata()
            else:
                return header.get_metadata_qti()
        return None

    def validate_hash_segment_metadata(self, authority=None):
        """ Validate metedata for unsigned image

        Raise exception if parsegen's hash segment contains signing attributes
        that do not match the configured values.
        """
        from sectools.features.isc.signer.signerutils.attributes import SigningAttributes
        from sectools.features.isc.signer.signerutils.attribute_extractor import \
            AttributeExtractor

        # If image is signed, validation is done in signer.
        if not self.is_signed(authority):
            hash_segment_metadata = self.get_hash_segment_metadata(authority)
            if hash_segment_metadata:
                errstr = []
                extracted_image_attributes = AttributeExtractor(
                    hash_segment_metadata=hash_segment_metadata,
                    attributes=SigningAttributes()).attributes
                attr_config = SigningAttributes()
                attr_config.update_from_image_info_attrs(self.imageinfo)
                mismatches = extracted_image_attributes.compare(
                    attr_config, extracted_image_attributes.EXCLUDE_HASH_SEGMENT_METADATA)
                create_mismatch_table(mismatches, errstr, operation="signing",
                                      data_type_to_compare="Attribute",
                                      image_region="Hash Segment")
                if errstr:
                    raise RuntimeError(
                        'Following validations failed for the image:\n       ' +
                        '\n       '.join([(str(authority + 1) + '. ' + e) for
                                          authority, e in enumerate(errstr)]))

        # Ensure metadata sw_ids match
        hash_segment_metadata = self.get_hash_segment_metadata(AUTHORITY_OEM)
        hash_segment_metadata_qti = self.get_hash_segment_metadata(AUTHORITY_QTI)
        if (hash_segment_metadata and hash_segment_metadata_qti and
                hash_segment_metadata["sw_id"] != hash_segment_metadata_qti["sw_id"]):
            errstr = []
            mismatches = [('sw_id', [hex(hash_segment_metadata["sw_id"])],
                           [hex(hash_segment_metadata_qti["sw_id"])])]
            create_mismatch_table(
                mismatches, errstr, operation="signing", data_type_to_compare="Attribute",
                image_region="Metadata", image_region2="Metadata QTI")
            if errstr:
                raise RuntimeError(
                    'Following validations failed for the image:\n       ' +
                    '\n       '.join([(str(authority + 1) + '. ' + e) for
                                      authority, e in enumerate(errstr)]))

    @property
    def data_to_sign(self):
        # Backup the parsegen
        backup = _BackupMbnParsegen(
            self._mbn_parsegen, "sign", "cert_chain", "encryption_params",
            "sign_qti", "cert_chain_qti")

        with V5DataToSignCtxMgr(self) as ctx:
            pass

        # Mask metadata and sig/cert sizes of other authority
        header = self.utils.mask_header_values(ctx.header, self.authority)
        retval = header.pack() + self._mbn_parsegen.code

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
