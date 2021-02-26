# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import abc

from sectools.common.utils.c_logging import logger
from sectools.features.isc.signer.local_v2_signer import LocalV2Signer


class BaseRemoteClientSigningRequest(object):
    def __init__(self, signer, hash_to_sign, data_to_sign, imageinfo):
        self.signer = signer
        self.hash_to_sign = hash_to_sign
        self.data_to_sign = data_to_sign
        self.imageinfo = imageinfo

    @abc.abstractmethod
    def execute(self):
        pass


class BaseRemoteClientSigner(LocalV2Signer):
    def __init__(self, config):
        super(BaseRemoteClientSigner, self).__init__(config)

    @classmethod
    def is_plugin(cls):
        return False

    @classmethod
    def signer_id(cls):
        """ Returns the unique name of the signer id

        :return: Name of the signer id
        """
        return 'base_remote_client'

    def initialize(self, imageinfo, **kwargs):
        """
        This is the method that drives the remote signing process.

        The following functions are called before returning to the LocalV2Signer Sign() and must be
        implemented by any class that is derived from BaseRemoteClientSigner:

        1) create_signing_request()
        2) SigningRequest.execute()
        3) process_signature_response()
        4) validate_response() --> only required to be overridden
           if validation specific to the sub-classed remote signer is required.
           Call BaseRemoteClientSigner's validate_response() in the subclass's validate_response() when overriding.

        :return None
        """
        # Call create_signing_request() to create signing request
        signing_request = self.create_signing_request(self.hash_to_sign, self.data_to_sign, imageinfo)

        # Call exec_signing_request() to execute signing
        self.exec_signing_request(signing_request)

        # Call process_signature_response() to extract data returned by server
        signer_output = self.process_signature_response()

        # Call validate_response() to validate signing completed successfully
        self.validate_response(signer_output)

    @abc.abstractmethod
    def create_signing_request(self, hash_to_sign, data_to_sign, imageinfo):
        """ Handles creation of sign request to be sent to server.
        Needs to return a SigningRequest.

        :return: SigningRequest - Class with execute() method that will communicate with server to sign to_sign
        """
        raise NotImplementedError

    @classmethod
    def exec_signing_request(cls, request):
        """ Executes sign request by calling execute() method of request object

        :return: None
        """
        request.execute()

    @abc.abstractmethod
    def process_signature_response(self):
        """ Handles extracting and processing the response from the remote server.

        :return: SignerOutput
        """
        raise NotImplementedError

    def validate_response(self, signer_output):
        """ Handles any necessary validation of remote_signer_output and sets signer certificate values.

        :return: None
        """
        if signer_output is None:
            raise RuntimeError(self.__class__.__name__ + " returned a null output from process_signature_response()")

        if signer_output.attestation_cert is None or signer_output.signature is None:
            raise RuntimeError("Attestation Certificate or Signature are missing from signer_output")

        # Set our own internal fields and let the signer continue
        if signer_output.root_cert is not None:
            logger.info(self.__class__.__name__ + " provided a root certificate")
            self.certs[self.ROOT].cert = signer_output.root_cert

        if signer_output.attestation_ca_cert is not None and self.CA in self.certs:
            logger.info(self.__class__.__name__ + " provided a CA certificate")
            self.certs[self.CA].cert = signer_output.attestation_ca_cert

        logger.info(self.__class__.__name__ + " provided an attest certificate")
        self.certs[self.ATTEST].cert = signer_output.attestation_cert

        logger.info(self.__class__.__name__ + " provided a signature")
        self.signature = signer_output.signature
