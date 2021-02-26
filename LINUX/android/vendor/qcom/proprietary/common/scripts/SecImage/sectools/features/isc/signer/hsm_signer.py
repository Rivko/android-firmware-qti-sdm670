# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.utils.c_logging import logger
from sectools.features.isc.signer.base_signer_v2 import BaseSignerV2


class HSMSigner(BaseSignerV2):

    @classmethod
    def is_plugin(cls):
        return False

    def sign_req(self, signing_attrs, attest_cert_dict, image_hash):
        """ Handles the HSM sign() request. Needs to return three dictionaries and a signature
        which will be used in the sectools signer. The output is a class called HSMSignerOutput
        which can be found above

        :param dict attest_cert_dict: Attestation Certificate Dictionary
        :param str image_hash: The image hash to sign
        :return: HSMSignerOutput - contains the certs and the signature
        """
        raise NotImplementedError()

    def initialize(self, imageinfo):
        """
        This is the first method to be called in our sign() method. You can set all of the self._cert
        data here if you have all of the keypairs and the certificates already available

        If the following variables are all set, than the below methods will never be called, and you
        simply need to override this one method

        self.certs[BaseSignerV2.ROOT].cert
        self.certs[BaseSignerV2.CA].cert
        self.certs[BaseSignerV2.ATTEST].cert
        self.signature

        :return None
        """
        # Call the Base Signer v2 to call DataProv
        BaseSignerV2.initialize(self, imageinfo)

        # Call the HSM signer method
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
        self.signature = signature

        # Validation
        for key, value in self.certs.items():
            if value.cert is None:
                raise RuntimeError("Certificate " + key + " is missing from HSM Signer")
