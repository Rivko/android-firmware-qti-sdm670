# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import json

from sectools.features.isc.signer.remote import RemoteSigner
from sectools.features.isc.signer.signerutils.attributes import SigningAttributes
from sectools.features.isc.signer.signerutils.package import ToSignPackageFiles, SignaturePackageFiles


class SecimageRemoteClientSigner(RemoteSigner):
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def signer_id(cls):
        return 'remote_client'

    @classmethod
    def get_to_sign_package_name(cls):
        return 'remote_to_sign_package.zip'

    @classmethod
    def get_signature_package_name(cls):
        return 'remote_signature_package.zip'

    @classmethod
    def get_class_ToSignPackageFiles(cls):
        pf = ToSignPackageFiles()
        pf.package_glob = cls.get_to_sign_package_name()
        return pf

    @classmethod
    def get_class_SignaturePackageFiles(cls):
        pf = SignaturePackageFiles()
        pf.package_glob = cls.get_signature_package_name()
        return pf

    @classmethod
    def update_tosign_data(cls, hash_to_sign, binary_to_sign, imageinfo, pf):
        pf.file_data['to_sign']['data'] = hash_to_sign
        signing_config = \
            {
                'binary_len': len(binary_to_sign),
                'sign_id': imageinfo.sign_id,
                'chipset': imageinfo.chipset,
            }

        # Check if signing attributes should be sent as well
        signer_attributes = imageinfo.config.signing.signer_attributes.remote_client_signer_attributes
        if signer_attributes and signer_attributes.send_signing_overrides:
            attrs = SigningAttributes()
            attrs.update_from_image_info_attrs(imageinfo.signing_attributes)
            signing_config['signing_attributes'] = attrs.get_json()

        pf.file_data['signing_config']['data'] = json.dumps(signing_config)

    @classmethod
    def use_tosign_data(cls, pf):
        # Set the return values
        to_sign = pf.file_data['to_sign']['data']
        signing_config = pf.file_data['signing_config']['data']
        return to_sign, signing_config

    @classmethod
    def update_signature_package(cls, signature, attestation_cert, attestation_ca_cert, root_cert_list, pf):
        pf.file_data['signature']['data'] = signature
        pf.file_data['attestation_cert']['data'] = attestation_cert
        pf.file_data['attestation_ca_cert']['data'] = attestation_ca_cert
        pf.file_data['root_cert_list']['data'] = root_cert_list

    @classmethod
    def use_signature_package(cls, pf):
        signature = pf.file_data['signature']['data']
        attestation_cert = pf.file_data['attestation_cert']['data']
        attestation_ca_cert = pf.file_data['attestation_ca_cert']['data']
        root_cert_list = pf.file_data['root_cert_list']['data']
        return signature, attestation_cert, attestation_ca_cert, root_cert_list
