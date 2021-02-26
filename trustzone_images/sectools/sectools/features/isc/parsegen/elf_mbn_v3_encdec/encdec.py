# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.common.parsegen.elf import segment
from sectools.common.parsegen.elf.format import ParseGenElfDelegate
from sectools.common.parsegen.elf.segment.type import PT_NOTE
from sectools.features.isc.encryption_service.base.encdec import BaseEncdec

# Segments not encrypted
NON_ENCRYPT_SEGMENTS = [PT_NOTE]


class ParseGenEncDec(object):

    class ParseGenEncDecBase(ParseGenElfDelegate):

        def __init__(self, parsegen, store_debug_data, encrypt_segment):
            super(ParseGenEncDec.ParseGenEncDecBase, self).__init__(parsegen)
            self.orig_delegate = parsegen.delegate
            self.store_debug_data = store_debug_data
            self.encrypt_segment = encrypt_segment
            self.segment_number = -1

        def segment_to_put(self, phdr):
            if not self.increase_if_processing:
                self.segment_number += 1
            return self.orig_delegate.segment_to_put(phdr)

        def process_segment_data(self, phdr, data):
            if (phdr.f_os_segment_type != segment.flags.PF_OS_SEGMENT_HASH and
                        phdr.p_type not in NON_ENCRYPT_SEGMENTS and
                        phdr.f_os_segment_type != segment.flags.PF_OS_SEGMENT_L4BSP):
                if self.increase_if_processing:
                    self.segment_number += 1
                data = self.encrypt_segment(data, self.segment_number)
                self.store_debug_data('encr_' + str(self.segment_number), data)
            return data

        def decrypt(self, decryptor):
            seg = self.decrypt_seg_number
            increase_if_processing = self.increase_if_processing
            for phdr in self.parsegen.phdrs:
                if (self.parsegen.delegate.segment_to_put(phdr) and
                            phdr.f_os_segment_type != segment.flags.PF_OS_SEGMENT_HASH and
                            phdr.p_type not in NON_ENCRYPT_SEGMENTS and
                            phdr.f_os_segment_type != segment.flags.PF_OS_SEGMENT_L4BSP):
                    data = decryptor.decrypt_segment(self.parsegen.segments[phdr], seg)
                    self.store_debug_data('decr_' + str(seg), data)
                    self.parsegen.segments[phdr] = data
                    if increase_if_processing:
                        seg += 1
                if not increase_if_processing:
                    seg += 1

        @property
        def increase_if_processing(self):
            return False

        @property
        def decrypt_seg_number(self):
            return 0

    class ParseGenEncDecZeroBased(ParseGenEncDecBase):

        @property
        def increase_if_processing(self):
            return True

        @property
        def decrypt_seg_number(self):
            return 0

    class ParseGenEncDecTwoBased(ParseGenEncDecBase):

        @property
        def increase_if_processing(self):
            return False

        @property
        def decrypt_seg_number(self):
            return 2

    ParseGenEncDecMap = \
        {
            BaseEncdec.SegmentNumberScheme.ZERO_BASED: ParseGenEncDecZeroBased,
            BaseEncdec.SegmentNumberScheme.TWO_BASED: ParseGenEncDecTwoBased,
        }

    def __init__(self, store_debug_data, encrypt_segment):
        self.store_debug_data = store_debug_data
        self.encrypt_segment = encrypt_segment
        self.orig_delegate = None

    # Used for encryption
    def update_parsegen(self, segment_scheme, parsegen):
        self.orig_delegate = parsegen.delegate
        parsegen.delegate = self.ParseGenEncDecMap[segment_scheme](parsegen, self.store_debug_data,
                                                                   self.encrypt_segment)

    # Used for encryption
    def revert_parsegen(self, parsegen):
        parsegen.delegate = self.orig_delegate
        self.orig_delegate = None

    # Used for decryption
    def decrypt(self, segment_scheme, parsegen, decryptor):
        delegate = self.ParseGenEncDecMap[segment_scheme](parsegen, self.store_debug_data, self.encrypt_segment)
        delegate.decrypt(decryptor)
