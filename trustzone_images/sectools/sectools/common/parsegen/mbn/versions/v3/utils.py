# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import struct

from sectools.common.parsegen.mbn.utils import extract_segment
from . import MBN_HDR_VERSION_3
from .headers import MBN_80_MAGIC
from .headers import MbnHdr40B
from .headers import MbnHdr80B


class V3Utils(object):

    @classmethod
    def update_mbn_header(cls, header, image_dest_ptr, mbn_version):
        header.version = mbn_version
        header.image_src = 0
        header.image_dest_ptr = image_dest_ptr

    @classmethod
    def extract_sign(cls, header, data):
        offset = header.code_size
        size = header.sig_size
        data, seg = extract_segment(data, offset, size)
        return data, seg

    @classmethod
    def extract_cert_chain(cls, header, data):
        offset = header.code_size + header.sig_size
        size = header.cert_chain_size
        data, seg = extract_segment(data, offset, size)
        return data, seg

    @classmethod
    def mask_header_values(cls, header, authority):
        return header

    @classmethod
    def get_mbn_header_info(cls, data):
        """ Wrapper function for protected member _get_mbn_header_info_core().

        It factors out the front and tail-end assertions for all MBN versions.
        """
        if len(data) < 8:
            raise RuntimeError("Length of MBN header found to be {0}B which is too small to get version."
                               .format(len(data)))
        success, mbn_header_version, mbn_header_size = cls._get_mbn_header_info_core(data)
        return success, mbn_header_version, mbn_header_size

    @classmethod
    def _get_mbn_header_info_core(cls, data):
        hdr_ver, = struct.unpack("I", data[4:8])  # extract version from MBN header
        if hdr_ver in [MBN_HDR_VERSION_3]:
            return True, MBN_HDR_VERSION_3, MbnHdr40B.get_size()
        if hdr_ver in [MBN_80_MAGIC]:
            return True, MBN_HDR_VERSION_3, MbnHdr80B.get_size()
        return False, hdr_ver, None
