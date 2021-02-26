# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import struct

from sectools.common.utils.c_logging import logger
from sectools.features.isc.defines import AUTHORITY_OEM
from . import MBN_HDR_VERSION_6
from .headers import MbnHdrV6
from ..v3 import MBN_HDR_VERSION_3
from ..v5.utils import V5Utils


class V6Utils(V5Utils):

    @classmethod
    def update_mbn_header(cls, header, image_dest_ptr, mbn_version):
        super(V6Utils, cls).update_mbn_header(
            header, image_dest_ptr, mbn_version)
        header.metadata_size_qti = 0
        header.metadata_size = 0

    @classmethod
    def mask_qti_metadata_and_sizes(cls, header):
        # Mask qti metadata if qti metadata exists in header
        if header.get_metadata_qti():
            header.set_metadata_qti(None)
            logger.debug("Masked QTI metadata")
        # Mask qti sig size and cert size
        header.sig_size_qti = 0
        header.cert_chain_size_qti = 0
        logger.debug("Masked QTI sig size and cert chain size")

    @classmethod
    def mask_oem_metadata_and_sizes(cls, header):
        # Mask oem metadata if oem metadata exists in header
        if header.get_metadata():
            header.set_metadata(None)
            logger.debug("Masked OEM metadata")
        # Mask oem sig size and cert size
        header.sig_size = 0
        header.cert_chain_size = 0
        logger.debug("Masked OEM sig size and cert chain size")

    @classmethod
    def mask_header_values(cls, header, authority):
        if getattr(header, "supports_metadata", lambda: False)():
            # Mask header fields of other authority
            if authority == AUTHORITY_OEM:
                cls.mask_qti_metadata_and_sizes(header)
            else:
                cls.mask_oem_metadata_and_sizes(header)
        return header

    @classmethod
    def _get_mbn_header_info_core(cls, data):
        success, hdr_ver, hdr_size = super(V6Utils, cls)._get_mbn_header_info_core(data)
        if success:
            return success, hdr_ver, hdr_size
        if hdr_ver in [MBN_HDR_VERSION_6]:
            # Since MbnHdrV6 method get_size() does not return a constant number,
            # and MBN v6 cannot be upscaled to a higher version of MBN header,
            # return None for its size, so that downstream knows upscaling of
            # MBN v6 image is not supported.
            return True, MBN_HDR_VERSION_6, None
        return False, hdr_ver, None

    @classmethod
    def upscale_mbn_header(cls, in_ver, out_ver, data):
        logger.info("Upscaling MBN header v{0} to MBN header v{1}".format(in_ver, out_ver))
        # Append metadata_size and metadata_size_qti with values 0 for MBNv6 header
        data += struct.pack("II", 0, 0)
        mbn_hdr = MbnHdrV6(data)
        if in_ver == MBN_HDR_VERSION_3:
            mbn_hdr.cert_chain_size_qti = 0
            mbn_hdr.sig_size_qti = 0
        return mbn_hdr.pack()
