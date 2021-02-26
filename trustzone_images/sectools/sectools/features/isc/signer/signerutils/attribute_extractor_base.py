# ===============================================================================
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from .attributes_base import SigningAttributesBase


class AttributeExtractorBase(object):

    def __init__(self, cert_data=None, attributes=None):
        self.initialize(attributes)

        if cert_data is None:
            raise RuntimeError("cert_data cannot be None")
        self.attributes.update_from_attest_cert(
            cert_data, excluded=SigningAttributesBase.INJECTED_SIGNATURE_ATTRIBUTES)

    def initialize(self, attributes):
        assert attributes is not None
        self.attributes = attributes
        self.image_region = "Image"

    def __repr__(self):
        return str(self.attributes)
