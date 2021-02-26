# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.signer.signerutils.attributes import SigningAttributes


class AttributeExtractor(object):

    def __init__(self, cert_data=None, hash_segment_metadata=None):

        self.image_region = "Attestation Cert"

        self.attributes = SigningAttributes()

        if (cert_data, hash_segment_metadata) == (None, None):
            raise RuntimeError("cert_data and hash_segment_data cannot both be None")

        if hash_segment_metadata is not None:
            self._init_using_hash_segment_and_cert(hash_segment_metadata, cert_data)
            self.image_region = "Hash Segment/Attestation Cert"
        else:
            self._init_using_cert(cert_data)

    def _init_using_hash_segment_and_cert(self, hash_segment_metadata, cert_data):
        self.attributes.update_from_hash_segment_and_attest_cert(hash_segment_metadata, cert_data)

    def _init_using_cert(self, cert_data):
        self.attributes.update_from_attest_cert(cert_data)

    def __repr__(self):
        return str(self.attributes)
