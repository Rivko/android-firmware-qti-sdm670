# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import json

from sectools.features.isc.signer.openssl_signer import OpenSSLSigner
from sectools.features.isc.signer.remote import RemoteSignerError
from sectools.features.isc.signer.secimage_remote_client import SecimageRemoteClientSigner as src
from sectools.features.isc.signer.signerutils.attributes import SigningAttributes
from sectools.features.isc.signer.signerutils.package import Package


class SecimageRemoteServerSigner(OpenSSLSigner):

    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def signer_id(cls):
        return 'remote'

    def sign_hash(self, hash_to_sign, imageinfo, binary_to_sign=None,
        debug_dir=None, sha_algo=None, binary_to_sign_len=None):
        # Find the hash_package
        packages_folder = imageinfo.dest_image.image_dir
        to_sign_package = Package(packages_folder, src.get_class_ToSignPackageFiles())

        try:
            if to_sign_package.package is None:
                raise RemoteSignerError('Please place the to_sign package in ' + packages_folder,
                                        RemoteSignerError.E_PACKAGE_MISSING)

            # Read the hash package
            to_sign_package.update_data()
            to_sign, signing_config = src.use_tosign_data(to_sign_package.pf)
            signing_config = json.loads(signing_config)

            # Get the binary length
            binary_to_sign_len = int(signing_config['binary_len'])

            # Use the signing overrides
            signing_attributes = signing_config.get('signing_attributes', None)
            if signing_attributes is not None:
                signer_attributes = imageinfo.config.signing.signer_attributes.remote_signer_attributes
                if signer_attributes and signer_attributes.allow_signing_overrides:
                    sa = SigningAttributes()
                    sa.update_from_json(signing_attributes)
                    sa.update_image_info_attrs(imageinfo.signing_attributes)
                else:
                    raise RemoteSignerError('Signing attributes override is not accepted.',
                                            RemoteSignerError.E_PACKAGE_MISSING)

        except RemoteSignerError:
            raise

        except RuntimeError as e:
            raise RemoteSignerError(e.message)

        # Sign using openssl locally
        signer_output = OpenSSLSigner.sign_hash(self, hash_to_sign, imageinfo,
                                                binary_to_sign=binary_to_sign,
                                                debug_dir=debug_dir, sha_algo=sha_algo,
                                                binary_to_sign_len=binary_to_sign_len)

        # Generate the signature package
        signature_package = Package(packages_folder,
                                    src.get_class_SignaturePackageFiles())
        src.update_signature_package(signer_output.signature,
                                     signer_output.attestation_cert,
                                     signer_output.attestation_ca_cert,
                                     [signer_output.root_cert],
                                     signature_package.pf)
        signature_package.package = src.get_signature_package_path(imageinfo)
        signature_package.save_package()

        return signer_output
