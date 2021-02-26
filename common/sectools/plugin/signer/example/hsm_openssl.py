# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os

from plugin.signer import HSMSigner
from plugin.signer.example import CUR_DIR, launch_command, \
    read_file_contents, save_file_contents


class HSMOpensslSigner(HSMSigner):
    """Example signer class that uses openssl to simulate HSM behavior.

    The class simulates following usecase:
        1. OEM uses the HSM to store the root and the ca cert.
        2. OEM uses the HSM to dynamically generate the attestation cert.
        3. OEM uses the HSM to generate the signature using the attestation
           cert generated in #2.

    The class provides all four secure assets:
        root cert, ca cert, attestation cert, signature.
    """

    @classmethod
    def is_plugin(cls):
        """Registers the class with the signer plugin manager. This allows
        the signer to be selected by the user from config/command line.

        Returns:
            is_plugin   (bool): True if the signer should be enabled,
                                False if the signer should be disabled.
        """
        return True

    @classmethod
    def signer_id(cls):
        """Specifies the unique id of the signer. This is the id that the user
        will use to select the signer from config/command line.

        Returns:
            signer_id   (str): ID of the signer.
        """
        return 'example_hsm_openssl'

    def sign_req(self, signing_attrs, attest_cert_dict, image_hash):
        """Signs the image_hash, generates the attestation certificate using
        the attest_cert_dict and signing_attrs and optionally returns the
        root certificate and the ca certificate.

        Args:
            signing_attrs  (object): An object containing the following signing
                                     attributes:
                                         - key_size: Size of the key for
                                                     attestation key.
                                         - exponent: Exponent for attestation
                                                     key.
            attest_cert_dict (dict): Dictionary containing the parameters to be
                                     put in the attestation certificate.
            image_hash        (str): The image hash binary blob to be signed.

        Returns:
            A tuple containing the root certificate, ca certificate, attestation
            certificate and the signature corresponsding to the image hash.

            root_cert    (str): - Binary blob of the root certificate.
                                - None if root certificate is provided under:
                                    resources/data_prov_assets/Signing/Local
            ca_cert      (str): - Binary blob of the ca certificate.
                                - None if ca certificate is provided under:
                                    resources/data_prov_assets/Signing/Local
            attest_cert  (str): Binary blob of the attestation certificate.
            signature    (str): Binary blob of the signature.
        """
        #-----------------------------------------------------------------
        # 1. Simulate secure storage in HSM containing the root and the ca
        # certificates.
        #-----------------------------------------------------------------
        # Set the paths for the resources directory.
        resources = os.path.join(CUR_DIR, 'resources')

        openssl_config_path = os.path.join(resources, 'openssl.cfg')
        extfile_path = os.path.join(resources, 'v3_attest.ext')

        root_cert_path = os.path.join(resources, 'root.cer')
        ca_cert_path = os.path.join(resources, 'attestca.cer')
        ca_priv_key_path = os.path.join(resources, 'attestca.key')

        # Temporary scratch work directory.
        tmp = os.path.join(CUR_DIR, 'tmp')
        if not os.path.exists(tmp):
            os.makedirs(tmp)

        # Read the root and the ca cert.
        root_cert = read_file_contents(root_cert_path)
        ca_cert = read_file_contents(ca_cert_path)

        #-----------------------------------------------------------------
        # 2. Simulate the creation of attestation key by the HSM.
        #-----------------------------------------------------------------
        # Create the openssl command for generation of the attestation key
        command = [self.openssl_path, 'genrsa']
        if signing_attrs.exponent == 3:
            command.append('-3')
        elif signing_attrs.exponent == 65537:
            command.append('-f4')
        else:
            raise RuntimeError('Invalid exponent value: ' + str(signing_attrs.exponent))
        command.append(str(signing_attrs.key_size))

        # Generate the attestation key
        attest_priv_key = launch_command(command)

        #-----------------------------------------------------------------
        # 3. Simulate the creation of attestation certificate request by HSM.
        #-----------------------------------------------------------------
        # Generate new certificate
        subject_params = self.get_openssl_subject_params(attest_cert_dict)
        attest_priv_key_path = os.path.join(tmp, 'attest.key')
        save_file_contents(attest_priv_key_path, attest_priv_key)
        command = [self.openssl_path, 'req', '-new', '-sha256',
                   '-key', attest_priv_key_path, '-subj', subject_params,
                   '-config', openssl_config_path,
                   '-set_serial', str(self.CERT_SERIAL),
                   '-days', str(self.CERT_DAYS)]
        attest_req = launch_command(command)

        #-----------------------------------------------------------------
        # 4. Simulate the signing of attestation certificate request by the
        # ca certificate / key using HSM.
        #-----------------------------------------------------------------
        # Sign the attestation certificate
        attest_req_path = os.path.join(tmp, 'attest_req.cer')
        save_file_contents(attest_req_path, attest_req)
        command = [self.openssl_path, 'x509', '-req', '-sha256',
                   '-in', attest_req_path, '-CA', ca_cert_path,
                   '-CAkey', ca_priv_key_path,
                   '-set_serial', str(self.CERT_SERIAL),
                   '-extfile', extfile_path,
                   '-days', str(self.CERT_DAYS)]
        attest_cert = launch_command(command)

        #-----------------------------------------------------------------
        # 5. Simulate the generation of the signature by HSM.
        #-----------------------------------------------------------------
        # Generate signature
        image_hash_path = os.path.join(tmp, 'hash.bin')
        save_file_contents(image_hash_path, image_hash)
        command = [self.openssl_path, 'pkeyutl', '-sign',
                   '-inkey', attest_priv_key_path,
                   '-in', image_hash_path, '-pkeyopt', 'rsa_padding_mode:pkcs1']
        signature = launch_command(command)

        # Return the required secure assets
        return root_cert, ca_cert, attest_cert, signature
