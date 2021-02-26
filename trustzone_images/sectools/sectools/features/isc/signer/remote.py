# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import abc

from sectools.common.utils import c_path
from sectools.common.utils.c_misc import abstractclassmethod
from sectools.features.isc.signer.local_v2_signer import LocalV2Signer
from sectools.features.isc.signer.signerutils.package import Package


class RemoteSignerError(RuntimeError):
    E_GENERIC = 1
    E_SIGN_WITH_REMOTE = 2
    E_INVALID_SIGN = 3
    E_PACKAGE_MISSING = 4
    E_OVERRIDES = 5

    ERR_STR_MAP = {
        E_GENERIC:          'Generic error',
        E_SIGN_WITH_REMOTE: 'Sign package with remote signer',
        E_INVALID_SIGN:     'Remote signed package signature is invalid',
        E_PACKAGE_MISSING:  'Expected zip package is missing',
        E_OVERRIDES:        'Error processing signing overrides',
    }

    def __init__(self, message, code=E_GENERIC):
        RuntimeError.__init__(self, message)
        self.code = code

    @property
    def code_str(self):
        return self.ERR_STR_MAP.get(self.code, '')


class RemoteSignerNote(RemoteSignerError):
    pass


class RemoteSigner(LocalV2Signer):

    @classmethod
    def is_plugin(cls):
        return False

    @abc.abstractmethod
    def get_class_ToSignPackageFiles(self):
        pass

    @abc.abstractmethod
    def get_class_SignaturePackageFiles(self):
        pass

    @abc.abstractmethod
    def update_tosign_data(self, hash_to_sign, binary_to_sign, imageinfo, pf):
        pass

    @abc.abstractmethod
    def use_tosign_data(self, pf):
        pass

    @abc.abstractmethod
    def update_signature_package(self, signature, attestation_cert, attestation_ca_cert, root_cert_list, pf):
        pass

    @abc.abstractmethod
    def use_signature_package(self, pf):
        pass

    @abstractclassmethod
    def get_to_sign_package_name(self):
        pass

    @abstractclassmethod
    def get_signature_package_name(self):
        pass

    @classmethod
    def get_to_sign_package_path(cls, imageinfo):
        return c_path.join(imageinfo.dest_image.image_dir, cls.get_to_sign_package_name())

    @classmethod
    def get_signature_package_path(cls, imageinfo):
        return c_path.join(imageinfo.dest_image.image_dir, cls.get_signature_package_name())

    def sign_hash(self, hash_to_sign, imageinfo, binary_to_sign, debug_dir=None, hash_algo=None):
        packages_folder = imageinfo.dest_image.image_dir

        # Find the signature_package
        to_sign_package = Package(packages_folder, self.get_class_ToSignPackageFiles())
        signature_package = Package(packages_folder, self.get_class_SignaturePackageFiles())

        try:
            if signature_package.package is None:
                # Save the to_sign package and exit
                to_sign_package.package = self.get_to_sign_package_path(imageinfo)
                self.update_tosign_data(hash_to_sign, binary_to_sign, imageinfo, to_sign_package.pf)
                to_sign_package.save_package()

                raise RemoteSignerNote('Please sign the to_sign package ' + to_sign_package.package +
                                       ' and place the signature package in ' + packages_folder,
                                       RemoteSignerError.E_SIGN_WITH_REMOTE)

            # Read the signature package
            signature_package.update_data()
            signature, attestation_cert, attestation_ca_cert, root_cert_list = self.use_signature_package(
                signature_package.pf)
            self.signature = signature
            cert_chain_list = [attestation_cert]
            if attestation_ca_cert is not None:
                cert_chain_list.append(attestation_ca_cert)
            cert_chain_list += root_cert_list

            # Validate the cert chain
            if self.validate_sig(binary_to_sign, signature, cert_chain_list) is False:
                raise RemoteSignerError(
                    'Zip file validation failed. Please sign to_sign package ' + to_sign_package.package +
                    ' and replace file ' + signature_package.package,
                    RemoteSignerError.E_INVALID_SIGN)
        except RemoteSignerError:
            raise

        except RuntimeError as e:
            raise RemoteSignerError(str(e))

        # TODO: Use debug to determine if the cleanup should be done
        # Cleanup
        # os.remove(to_sign_package.package)
        # os.remove(signature_package.package)

        return self._get_signer_output(signature, cert_chain_list)

    def validate_config(self, imageinfo):
        pass
