# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.advanced_defines import AUTHORITY_QTI
from . import MBN_HDR_VERSION_6
from .headers import MbnHdrV6
from .utils import V6Utils
from ..v5.parsegen_mbn import ParseGenMbnV5
from ..v5.parsegen_mbn import V5GetHdrCntxMgr
from ... import get_header

get_header[MBN_HDR_VERSION_6] = lambda size, version, data=None: MbnHdrV6(data)


class V6GetHdrCntxMgr(V5GetHdrCntxMgr):

    def __init__(self, mbn, authority=None, imageinfo=None, validating=False,
                 signing=False, add_signing_attr_to_hash_seg=False):
        super(V6GetHdrCntxMgr, self).__init__(mbn, authority)
        self.imageinfo = imageinfo
        self.validating = validating
        self.signing = signing
        self.add_signing_attr_to_hash_seg = add_signing_attr_to_hash_seg

    def metadata_support(self, single_signing):
        # Set header metadata, only applicable to v6.
        if getattr(self.hdr, "supports_metadata", lambda: False)():
            # Don't modify the internal header object if user is validating
            # image otherwise header metadata contents log will not reflect
            # metadata of image being validated but rather image metadata
            # being validated against
            if self.validating:
                self.hdr = MbnHdrV6(self.hdr.pack())
                if not single_signing and self.authority == AUTHORITY_QTI:
                    self.hdr.set_metadata(None)
            elif self.signing or self.add_signing_attr_to_hash_seg:
                if single_signing:
                    if self.authority == AUTHORITY_QTI:
                        self.hdr.set_metadata_qti(self.imageinfo)
                    else:
                        self.hdr.set_metadata(self.imageinfo)
                else:
                    if self.authority == AUTHORITY_QTI:
                        self.hdr.set_metadata_qti(self.imageinfo)
                        self.hdr.set_metadata(None)
                    else:
                        # Prevent OEM from adding metadata before QTI for
                        # double-signable images
                        if not self.hdr.get_metadata_qti():
                            raise RuntimeError(
                                "Image must contain QTI signing attributes "
                                "before adding OEM signing attributes")
                        else:
                            self.hdr.set_metadata(self.imageinfo)


class ParseGenMbnV6(ParseGenMbnV5):

    utils = V6Utils

    def get_header(self, authority=None, imageinfo=None, validating=False,
                   signing=False, add_signing_attr_to_hash_seg=False):
        with V6GetHdrCntxMgr(
                self, authority=authority, imageinfo=imageinfo,
                validating=validating, signing=signing,
                add_signing_attr_to_hash_seg=add_signing_attr_to_hash_seg) as v6:
            single_signing = self.pad_max_sig_size == 0
            v6.metadata_support(single_signing)
            v6.qti_signing(single_signing)
        return v6.hdr
