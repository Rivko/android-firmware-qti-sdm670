# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
from sectools.common.parsegen import PAD_BYTE_0
from sectools.common.utils.c_logging import logger
from sectools.common.utils.struct_base import StructDynamic
from sectools.features.isc.defines import AUTHORITY_OEM
from sectools.features.isc.signer.local_v2_signer import LocalV2Signer


class HSMSigner(LocalV2Signer):

    @classmethod
    def is_plugin(cls):
        return False

    def sign_req_data(self, signing_attrs, attest_cert_dict, image_hash, data_to_hash):
        """ This API replaces sign_req and allows the derived class to inspect the data being signed.

        :param signing_attrs: Signing parameters
        :param dict attest_cert_dict: Attestation Certificate Dictionary
        :param str image_hash: The image hash to sign
        :param StructDynamic data_to_hash: An object which encapsulates the format of the authentication-relevant
                                           fields which were hashed to create image_hash. The
                                           authentication-relevant fields available for interrogation can be
                                           retrieved via the "fields" member.
        :return: HSMSignerOutput - contains the certs and the signature
        """
        raise NotImplementedError()

    def sign_req(self, signing_attrs, attest_cert_dict, image_hash):
        """ Handles the HSM sign() request. Needs to return three dictionaries and a signature
        which will be used in the sectools signer. The output is a class called HSMSignerOutput
        which can be found above

        :param signing_attrs: Signing parameters
        :param dict attest_cert_dict: Attestation Certificate Dictionary
        :param str image_hash: The image hash to sign
        :return: HSMSignerOutput - contains the certs and the signature
        """
        raise NotImplementedError()

    def initialize(self, imageinfo, **kwargs):
        """
        This is the first method to be called in our sign() method. You can set all of the self._cert
        data here if you have all of the keypairs and the certificates already available

        If the following variables are all set, than the below methods will never be called, and you
        simply need to override this one method

        self.certs[self.ROOT].cert
        self.certs[self.CA].cert
        self.certs[self.ATTEST].cert
        self.signature

        :return None
        """
        # Call the Base Signer v2 to call DataProv
        LocalV2Signer.initialize(self, imageinfo, **kwargs)

        data_to_hash = StructDynamic(self.data_to_sign if self.data_to_sign else self.hash_to_sign,
                                     self.data_to_sign_format)
        # Call the HSM signer method
        try:
            # Call HSM signer API which provides the format of the data to sign if it is implemented
            root_cert, ca_cert, attest_cert, signature = \
                self.sign_req_data(self.signing_attributes,
                                   self.create_subject_params(self.ATTEST, self.certs[self.ATTEST].params.params),
                                   self.hash_to_sign, data_to_hash)
        except NotImplementedError:
            # Call legacy HSM signer API
            root_cert, ca_cert, attest_cert, signature = \
                self.sign_req(self.signing_attributes,
                              self.create_subject_params(self.ATTEST, self.certs[self.ATTEST].params.params),
                              self.hash_to_sign)

        # Make sure the attest and the signature are here
        if attest_cert is None or signature is None:
            raise RuntimeError("Attestation Certificate or Signature are missing from hsm_signer_output")

        # Set our own internal fields and let the signer continue
        if root_cert is not None:
            logger.info("HSM Signer provided a root certificate")
            self.certs[self.ROOT].cert = root_cert

        if ca_cert is not None and self.CA in self.certs:
            logger.info("HSM Signer provided a CA certificate")
            self.certs[self.CA].cert = ca_cert

        logger.info("HSM Signer provided an attest certificate")
        self.certs[self.ATTEST].cert = attest_cert

        logger.info("HSM Signer provided a signature")

        # Pad signature to size in the header depending on authority since ECDSA signatures can be of variable size
        parsegen = kwargs.get("parsegen", None)
        authority = parsegen.authority if parsegen else AUTHORITY_OEM
        try:
            padded_length = data_to_hash.sig_size if authority == AUTHORITY_OEM else data_to_hash.sig_size_qti
        except RuntimeError:
            padded_length = len(signature)
        self.signature = signature.ljust(padded_length, PAD_BYTE_0)

        # Validation
        for key, value in self.certs.items():
            if value.cert is None:
                raise RuntimeError("Certificate " + key + " is missing from HSM Signer")
