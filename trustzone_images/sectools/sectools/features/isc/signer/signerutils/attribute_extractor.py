# ===============================================================================
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from .attribute_extractor_base import AttributeExtractorBase
from .attributes_base import SigningAttributesBase


class AttributeExtractor(AttributeExtractorBase):

    def __init__(self, cert_data=None, hash_segment_metadata=None, attributes=None):
        self.initialize(attributes)

        if (cert_data, hash_segment_metadata) == (None, None):
            raise RuntimeError("cert_data and hash_segment_data cannot both be None")

        if hash_segment_metadata is not None:
            self.attributes.update_from_hash_segment_and_attest_cert(
                hash_segment_metadata, cert_data,
                excluded=SigningAttributesBase.INJECTED_SIGNATURE_ATTRIBUTES)
        else:
            self.attributes.update_from_attest_cert(
                cert_data, excluded=SigningAttributesBase.INJECTED_SIGNATURE_ATTRIBUTES)
