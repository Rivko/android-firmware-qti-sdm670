# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from sectools.features.isc.parsegen.base_encdec import SecParseGenBaseEncDec

from .elf_mbn_v6.mbn_v6 import MbnV6
from .elf_mbn_v5.mbn_v5 import MbnV5
from .elf_mbn_v3_encdec.mbn_v3_encdec import MbnV3EncDec


class SecParseGenMbn(object):

    def __init__(self, *args, **kwargs):
        self.qti_signing_enabled = False
        super(SecParseGenMbn, self).__init__(*args, **kwargs)

    def __repr__(self):
        return ('Base Properties: ' + '\n' + SecParseGenBaseEncDec.__repr__(self) + '\n'
                'MBN Properties: ' + '\n' + repr(self._mbn_parsegen))

    #--------------------------------------------------------------------------
    # Mandatory Properties overloaded from the base class
    #--------------------------------------------------------------------------
    @classmethod
    def is_plugin(cls):
        return True

    @classmethod
    def file_type(cls):
        return "mbn"

    @classmethod
    def get_plugin_id(cls):
        return cls.__name__


class MbnV6(SecParseGenMbn, MbnV6, SecParseGenBaseEncDec):
    """ MBN Version 6 """


class MbnV5(SecParseGenMbn, MbnV5, SecParseGenBaseEncDec):
    """ MBN Version 5 """


class MbnV3(SecParseGenMbn, MbnV3EncDec, SecParseGenBaseEncDec):
    """ MBN Version 3 """
