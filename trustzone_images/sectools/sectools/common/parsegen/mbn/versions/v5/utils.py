# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen.mbn.utils import extract_segment
from sectools.common.utils.c_logging import logger
from . import MBN_HDR_VERSION_5
from .headers import MbnHdr40BV5
from ..v3_encdec.utils import V3EncDecUtils


class V5Utils(V3EncDecUtils):

    @classmethod
    def update_mbn_header(cls, header, image_dest_ptr, mbn_version):
        header.version = mbn_version
        header.sig_size_qti = 0
        header.cert_chain_size_qti = 0

    @classmethod
    def extract_sign_qti(cls, header, data):
        offset = header.code_size
        size = header.sig_size_qti
        return extract_segment(data, offset, size)

    @classmethod
    def extract_cert_chain_qti(cls, header, data):
        offset = header.code_size + header.sig_size_qti
        size = header.cert_chain_size_qti
        return extract_segment(data, offset, size)

    @classmethod
    def extract_sign(cls, header, data):
        offset = header.code_size
        size = header.sig_size
        if header.supports_qti_signing():
            offset += header.sig_size_qti + header.cert_chain_size_qti
        data, seg = extract_segment(data, offset, size)
        return data, seg

    @classmethod
    def extract_cert_chain(cls, header, data):
        offset = header.code_size + header.sig_size
        size = header.cert_chain_size
        if header.supports_qti_signing():
            offset += header.sig_size_qti + header.cert_chain_size_qti
        data, seg = extract_segment(data, offset, size)
        return data, seg

    @classmethod
    def _get_mbn_header_info_core(cls, data):
        success, hdr_ver, hdr_size = super(V5Utils, cls)._get_mbn_header_info_core(data)
        if success:
            return success, hdr_ver, hdr_size
        if hdr_ver in [MBN_HDR_VERSION_5]:
            return True, MBN_HDR_VERSION_5, MbnHdr40BV5.get_size()
        return False, hdr_ver, None

    @classmethod
    def upscale_mbn_header(cls, in_ver, out_ver, data):
        logger.info("Upscaling MBN header v{0} to MBN header v{1}"
                    .format(in_ver, out_ver))
        mbn_hdr = MbnHdr40BV5(data)
        mbn_hdr.cert_chain_size_qti = 0
        mbn_hdr.sig_size_qti = 0
        return mbn_hdr.pack()
