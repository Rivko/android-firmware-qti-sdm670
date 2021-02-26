# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from xml.etree import ElementTree

from sectools.common.utils.c_logging import logger
from sectools.features.isc.signer.base_signer import ExternalSignerError
from signature_response import SignatureResponse


class SignaturePackage(object):
    def __init__(self, signature_package, signing_package_digest, signature_result_file):
        self.signature_package = signature_package
        self.signing_package_digest = signing_package_digest
        self.signature_result_file = signature_result_file
        self.root = None
        self.count = 0
        self.digest = None
        self.signature_response = None

        self._initialize()
        self._validate()
        self._save()

    def _initialize(self):
        if not self.signature_package:
            raise RuntimeError("A non-empty XML string must be provided to the constructor of signature package.")

        self.root = ElementTree.fromstring(self.signature_package)
        encoding = self.root.attrib['encoding']

        for child in self.root:
            if child.tag == 'Count':
                self.count = int(child.text)
            elif child.tag == 'SPDigest':
                if child.attrib['digestAlgorithm'] != "SHA-256":
                    raise RuntimeError("digestAlgorithm {0} is not supported".format(child.attrib['digestAlgorithm']))
                self.digest = child.text
            elif child.tag == 'SignatureResponse':
                self.signature_response = SignatureResponse(child, child.attrib['sequenceId'], encoding, self.signature_result_file)

    def _validate(self):
        if self.count != 1:
            raise RuntimeError("Signature package has an invalid count of {0}.".format(self.count))

        if self.digest is None:
            raise RuntimeError("Signature package digest is missing.")
        else:
            logger.debug("Signature package digest is: \"{0}\"".format(self.digest))

        if self.digest.lower() != self.signing_package_digest.lower():
            raise ExternalSignerError("Signing package digest mismatched signature package digest.")

    def _save(self):
        self.signature_response.save()
        logger.debug("Signature result file saved to {0}".format(self.signature_result_file))
