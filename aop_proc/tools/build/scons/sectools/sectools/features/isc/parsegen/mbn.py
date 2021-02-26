# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Nov 25, 2013

@author: hraghav
'''

from sectools.common.parsegen import mbn, PAD_BYTE_1
from sectools.common.parsegen.mbn.header import copy_header, mask_header_values, MBN_HDR_VERSION_3, MBN_HDR_VERSION_5, MBN_HDR_VERSION_6
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import multi_image_string
from sectools.features.isc import defines
from sectools.features.isc.parsegen.base import SecParseGenBase

MBN_FILE_TYPE = 'mbn'
MAX_SIG_SIZE = 512
SECBOOT_MBN_HDR = {
    defines.SECBOOT_VERSION_1_0: MBN_HDR_VERSION_3,
    defines.SECBOOT_VERSION_2_0: MBN_HDR_VERSION_5,
    defines.SECBOOT_VERSION_3_0: MBN_HDR_VERSION_6
}


class SecParseGenMbn(SecParseGenBase):

    def __init__(self, data, imageinfo=None, mbn_properties=None,
                 general_properties=None,
                 encdec=None,
                 debug_dir=None,
                 debug_prefix=None,
                 debug_suffix=None,
                 validating=False,
                 signing=False,
                 parsegens=None,
                 sign_attr=False):
        SecParseGenBase.__init__(self, data, imageinfo, general_properties,
                                 encdec, debug_dir, debug_prefix, debug_suffix, validating, signing, parsegens, sign_attr)

        # Check the arguments
        if imageinfo is not None:
            mbn_properties = imageinfo.image_type.mbn_properties
        if mbn_properties is None:
            raise RuntimeError('MBN properties must not be None.')

        # Set padding based on authorities
        if self.qti_signing_enabled and self.oem_signing_enabled:
            pad_max_sig_size, pad_max_cc_size = MAX_SIG_SIZE, self.get_cert_chain_size(defines.AUTHORITY_OEM)
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
                                             pad_max_sig_size,
                                             pad_max_cc_size,
                                             pad_max_ep_size
                                             )

        self.code_size = self._mbn_parsegen.code_size

        # Sig or cert chain should not exist when corresponding authority permissions are disabled
        self.validate_image_sig()

        # Private variables
        self._data_signature_qti = ''
        self._cert_chain_qti = ''

        # Extract encryption params
        self.encryption_params = self._mbn_parsegen.encryption_params

        # Backup authority
        authority = self.authority

        # Set the QTI signature/QTI cert chain if QTI signing is enabled
        # Needs to be done before getting OEM data
        self.authority = defines.AUTHORITY_QTI
        self.data_signature = self._mbn_parsegen.sign_qti
        self.cert_chain = self._mbn_parsegen.cert_chain_qti

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
        if self.authority == defines.AUTHORITY_QTI:
            if not self.qti_signing_enabled:
                raise RuntimeError('Image operation has not been enabled for QTI')
            if self.secboot_version not in [defines.SECBOOT_VERSION_2_0, defines.SECBOOT_VERSION_3_0]:
                raise RuntimeError('QTI cannot operate secboot version ' + str(self.secboot_version) + ' images.')

    def validate_image_sig(self):
        # If sig or cert chain are present when corresponding authority permissions are disabled, raise error
        if not self.oem_signing_enabled and (self._mbn_parsegen.sign or self._mbn_parsegen.cert_chain):
            raise RuntimeError('OEM signing is not enabled but OEM signature/cert chain are detected in image.')
        if not self.qti_signing_enabled and (self._mbn_parsegen.sign_qti or self._mbn_parsegen.cert_chain_qti):
            raise RuntimeError('QTI signing is not enabled but QTI signature/cert chain are detected in image.')

    @property
    def data_signature(self):
        # Get the signature based on the authority
        if self.authority == defines.AUTHORITY_QTI:
            return self._data_signature_qti
        return self._data_signature

    @data_signature.setter
    def data_signature(self, signature):
        # Validate the signature
        if signature:
            self.validate_data_signature(signature)
            self.validate_authority()

        # Set the signature based on the authority
        if self.authority == defines.AUTHORITY_QTI:
            self._data_signature_qti = signature
        else:
            if signature and self.qti_signing_enabled and not self._data_signature_qti:
                raise RuntimeError('Image must be signed by QTI before signing as OEM')
            self._data_signature = signature

    @property
    def cert_chain(self):
        # Get the cert chain based on the authority
        if self.authority == defines.AUTHORITY_QTI:
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
        if self.authority == defines.AUTHORITY_QTI:
            self._cert_chain_qti = cert_chain
        else:
            self._cert_chain = cert_chain

    def is_signed(self, authority=None):
        qti_signed = (self._data_signature_qti, self._cert_chain_qti) != ('', '')
        oem_signed = (self._data_signature, self._cert_chain) != ('', '')
        # if qti_signed and oem_signed: raise RuntimeError("Image is double signed but it shouldn't be")
        if authority == defines.AUTHORITY_OEM:
            return oem_signed
        elif authority == defines.AUTHORITY_QTI:
            return qti_signed
        return qti_signed or oem_signed

    def get_signing_assets(self):
        retval = []
        authority = self.authority
        if self.oem_signing_enabled:
            self.authority = defines.AUTHORITY_OEM
            retval.append((defines.AUTHORITY_OEM, self.data_to_sign, self._data_signature, self._cert_chain, self.get_hash_segment_metadata(defines.AUTHORITY_OEM)))
        if self.qti_signing_enabled:
            self.authority = defines.AUTHORITY_QTI
            retval.append((defines.AUTHORITY_QTI, self.data_to_sign, self._data_signature_qti, self._cert_chain_qti, self.get_hash_segment_metadata(defines.AUTHORITY_QTI)))
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

    def get_multi_image_entry_data(self, authority):
        logger.debug("Getting data to hash for " + authority + " " + multi_image_string() + " entry")
        try:
            if self.is_signed():
                return self.data_to_sign
            else:
                header = self._mbn_parsegen.header
                # Copy the MBN header so that masking doesn't alter original header
                header = copy_header(header)
                # Mask metadata and sig/cert sizes of other authority
                header = mask_header_values(header, self.authority)
                return header.pack() + self._mbn_parsegen.code
        except:
            return None

    def supports_hash_segment_metadata(self):
        return self._mbn_parsegen.header.supports_metadata()

    def get_hash_segment_metadata(self, authority=None):
        if authority is None:
            authority = self.authority
        header = self._mbn_parsegen.header
        if header.supports_metadata():
            if authority == defines.AUTHORITY_OEM:
                return header.get_metadata()
            else:
                return header.get_metadata_qti()
        return None

    def validate_hash_segment_metadata(self, authority=None):
        from sectools.common.utils.c_misc import create_mismatch_table
        # Validate metedata for unsigned image.
        # If image is signed, validation is done in signer.
        if not self.is_signed(authority):
            hash_segment_metadata = self.get_hash_segment_metadata(authority)
            if hash_segment_metadata is not None:
                from sectools.features.isc.signer.signerutils.attribute_extractor import AttributeExtractor
                from sectools.features.isc.signer.signerutils.attributes import SigningAttributes
                errstr = []
                extracted_image_attributes = AttributeExtractor(hash_segment_metadata=hash_segment_metadata).attributes
                attr_config = SigningAttributes()
                attr_config.update_from_image_info_attrs(self.imageinfo)
                mismatches = extracted_image_attributes.compare(attr_config, extracted_image_attributes.EXCLUDE_HASH_SEGMENT_METADATA)
                create_mismatch_table(mismatches, errstr, operation="signing", data_type_to_compare="Attribute", image_region="Hash Segment")
                if errstr:
                    raise RuntimeError('Following validations failed for the image:\n       ' +
                                       '\n       '.join([(str(authority + 1) + '. ' + e) for authority, e in enumerate(errstr)]))

        # Ensure metadata sw_ids match
        hash_segment_metadata = self.get_hash_segment_metadata(defines.AUTHORITY_OEM)
        hash_segment_metadata_qti = self.get_hash_segment_metadata(defines.AUTHORITY_QTI)
        if hash_segment_metadata and hash_segment_metadata_qti and hash_segment_metadata["sw_id"] != hash_segment_metadata_qti["sw_id"]:
            errstr = []
            mismatches = [('sw_id', [hex(hash_segment_metadata["sw_id"])], [hex(hash_segment_metadata_qti["sw_id"])])]
            create_mismatch_table(mismatches, errstr, operation="signing", data_type_to_compare="Attribute", image_region="Metadata", image_region2="Metadata QTI")
            if errstr:
                raise RuntimeError('Following validations failed for the image:\n       ' +
                                   '\n       '.join([(str(authority + 1) + '. ' + e) for authority, e in enumerate(errstr)]))

    @property
    def data_to_sign(self):
        # Backup the parsegen
        backup = _BackupMbnParsegen(self._mbn_parsegen)

        # Set the current attributes
        if self.authority == defines.AUTHORITY_QTI:
            self._mbn_parsegen.sign = self._data_signature
            self._mbn_parsegen.cert_chain = self._cert_chain

        self._mbn_parsegen.sign_qti = self._data_signature_qti
        self._mbn_parsegen.cert_chain_qti = self._cert_chain_qti

        # Set padding info
        self._mbn_parsegen.set_pad_info(self.sig_size, self.get_cert_chain_size(self.authority))

        # Update version number before applying signature #
        if SECBOOT_MBN_HDR[self.secboot_version] == self._mbn_parsegen.header.get_version():
            logger.debug("Updating version to " + str(self._mbn_parsegen.header.get_version()))
            self._mbn_parsegen.header.set_version_in_bin(self._mbn_parsegen.header.get_version())
        else:
            raise RuntimeError('Version check failed. MBN header version (%s) does not match expected version (%s).'
                               % (str(self._mbn_parsegen.header.get_version()), str(SECBOOT_MBN_HDR[self.secboot_version])))

        # Get the data to sign (header + code)
        header = self._mbn_parsegen.get_header(self.authority, self.imageinfo, self.validating, self.signing, self.add_signing_attr_to_hash_seg)
        # Copy the MBN header so that masking doesn't alter original header
        header = copy_header(header)
        # Mask metadata and sig/cert sizes of other authority
        header = mask_header_values(header, self.authority)
        retval = header.pack() + self._mbn_parsegen.code

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
            self._mbn_parsegen.sign_qti = self._data_signature_qti
            self._mbn_parsegen.cert_chain_qti = self._cert_chain_qti

        if encrypt or (self.oem_signing_enabled and self.qti_signing_enabled):
            self._mbn_parsegen.encryption_params = self.encryption_params
        else:
            self._mbn_parsegen.pad_max_encr_params_size = 0

        # Set padding info
        if sign:
            self._mbn_parsegen.set_pad_info(self.sig_size, self.get_cert_chain_size(self.authority))

        # Get the signed data
        retval = self._mbn_parsegen.get_data(self.authority, self.imageinfo, self.validating, self.signing, self.add_signing_attr_to_hash_seg)

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
        self.signature_qti = parsegen.sign_qti
        self.cert_chain_qti = parsegen.cert_chain_qti
        self.signature = parsegen.sign
        self.cert_chain = parsegen.cert_chain
        self.encryption_params = parsegen.encryption_params

        # Clear the original
        parsegen.sign_qti = ''
        parsegen.cert_chain_qti = ''
        parsegen.sign = ''
        parsegen.cert_chain = ''
        parsegen.encryption_params = ''

        return parsegen

    def restore(self, parsegen):
        # Restore the original
        parsegen.sign_qti = self.signature_qti
        parsegen.cert_chain_qti = self.cert_chain_qti
        parsegen.sign = self.signature
        parsegen.cert_chain = self.cert_chain
        parsegen.encryption_params = self.encryption_params

        # Clear self
        self.signature_qti = ''
        self.cert_chain_qti = ''
        self.signature = ''
        self.cert_chain = ''
        self.encryption_params = ''

        return parsegen
