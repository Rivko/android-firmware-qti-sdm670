# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from plugin.signer import HSMSigner
from sectools.common.utils.c_data import comma_separated_string
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import hexdump


class HSMOEMSigner(HSMSigner):
    """Class to enable signing via OEM's HSM"""

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
        return 'hsm_oem'

    def sign_req_data(self, signing_attrs, attest_cert_dict, image_hash, data_to_hash):
        """This API replaces sign_req and enables inspection of the data being signed.

        Args:
            signing_attrs              (object): An object containing the following signing attributes:
                                                 - key_size: Size of the key for attestation key
                                                 - exponent: Exponent for attestation key
            attest_cert_dict             (dict): Dictionary containing the parameters to be put in the attestation
                                                 certificate.
            image_hash                    (str): The image hash binary blob to be signed.
            data_to_hash        (StructDynamic): An object which encapsulates the format of the
                                                 authentication-relevant fields which were hashed to create
                                                 image_hash. The authentication-relevant fields available for
                                                 interrogation can be retrieved via the "fields" member.

        Returns:
            A tuple containing the root certificate, ca certificate, attestation
            certificate and the signature corresponding to the image hash.

            root_cert    (str): - Binary blob of the root certificate.
                                - None if root certificate is provided under:
                                    resources/data_prov_assets/Signing/Local
            ca_cert      (str): - Binary blob of the ca certificate.
                                - None if ca certificate is provided under:
                                    resources/data_prov_assets/Signing/Local
            attest_cert  (str): Binary blob of the attestation certificate.
            signature    (str): Binary blob of the signature.
        """
        # Demonstrate how to inspect the data being signed
        logger.info("Data being signed:\n" + hexdump(data_to_hash.binary_data))
        logger.info("Format of authentication-relevant data being signed: " + data_to_hash.format)
        logger.info("Authentication-relevant fields within data being signed: " +
                    comma_separated_string(data_to_hash.fields, "and"))
        logger.info("SW ID of image being signed: " + str(data_to_hash.sw_id))

        # Initialize the return variables
        root_cert, ca_cert, attest_cert, signature = None, None, None, None

        # OEM to fill in this part
        raise NotImplementedError('OEM to implement hsm signer')

        # Return the required secure assets
        return root_cert, ca_cert, attest_cert, signature

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
            certificate and the signature corresponding to the image hash.

            root_cert    (str): - Binary blob of the root certificate.
                                - None if root certificate is provided under:
                                    resources/data_prov_assets/Signing/Local
            ca_cert      (str): - Binary blob of the ca certificate.
                                - None if ca certificate is provided under:
                                    resources/data_prov_assets/Signing/Local
            attest_cert  (str): Binary blob of the attestation certificate.
            signature    (str): Binary blob of the signature.
        """
        # Initialize the return variables
        root_cert, ca_cert, attest_cert, signature = None, None, None, None

        # OEM to fill in this part
        raise NotImplementedError('OEM to implement hsm signer')

        # Return the required secure assets
        return root_cert, ca_cert, attest_cert, signature
