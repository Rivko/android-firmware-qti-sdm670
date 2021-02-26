# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

from .data_detail import DataType
from .data_detail import DataDetail
from .hex_handler import TypeHex
from .hex_handler import HexHandler
from .hex_handler import HexDetailer
from .hex16_handler import Hex16Normalizer


class TypeHex32(TypeHex):
    """Custom type to specify 32-bit Hex"""
    pass


# Global instance of the 32-bit TypeHex32 class.
type_hex_32 = TypeHex32()


class Hex32Normalizer(Hex16Normalizer):

    def normalize(self, i_value, length=8):
        return super(Hex32Normalizer, self).normalize(i_value, length=length)


class Hex32Detailer(HexDetailer):

    def detail(self, i_format):
        """See :meth:`.Data_Handler.BaseDetailer.detail`"""
        assert isinstance(i_format, TypeHex32)
        return DataDetail(DataType.Hex32, self.data_handler, i_format)


class Hex32Handler(HexHandler):

    def __init__(self, data_handler):
        """Initialized the various features."""
        super(Hex32Handler, self).__init__(data_handler)
        self._i_detailer = Hex32Detailer(self)
        self._i_normalizer = Hex32Normalizer(self)

    #--------------------------------------------------------------------------
    # Methods that must be implemented by the data handler
    #--------------------------------------------------------------------------
    @classmethod
    def get_data_handler_data_type(cls):
        """Returns the data type that is handled by this data handler"""
        return TypeHex32

    @classmethod
    def get_data_handler(cls):
        """Returns the class reference of this data handler"""
        return Hex32Handler
