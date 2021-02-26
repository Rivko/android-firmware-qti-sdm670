# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.parsegen.base_encdec import SecParseGenBaseEncDec

from .elf_v3_encdec import ElfV3EncDec
from .mbn_v3_encdec import MbnV3EncDec


class ElfMbnV3EncDec(ElfV3EncDec, MbnV3EncDec, SecParseGenBaseEncDec):
    """ ELF MBN Version 3 with Encryption and Decryption """

