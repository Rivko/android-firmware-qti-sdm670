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
from .hex_handler import HexNormalizer


class TypeHex16(TypeHex):
    """Custom type to specify 16-bit Hex"""
    pass


# Global instance of the 16-bit TypeHex16 class.
type_hex_16 = TypeHex16()


class Hex16Normalizer(HexNormalizer):

    def normalize(self, i_value, length=4):
        o_value = super(Hex16Normalizer, self).normalize(i_value)
        hex_str = ("%%0%dX" % length) % int(o_value, 16)
        hex_str = hex_str[:-1] if hex_str.endswith('L') else hex_str
        if len(hex_str) != length:
            raise ValueError("Hex string is too long: 0x" + hex_str)
        return "0x" + hex_str


class Hex16Detailer(HexDetailer):

    def detail(self, i_format):
        """See :meth:`.Data_Handler.BaseDetailer.detail`"""
        assert isinstance(i_format, TypeHex16)
        return DataDetail(DataType.Hex16, self.data_handler, i_format)


class Hex16Handler(HexHandler):

    def __init__(self, data_handler):
        """Initialized the various features."""
        super(Hex16Handler, self).__init__(data_handler)
        self._i_detailer = Hex16Detailer(self)
        self._i_normalizer = Hex16Normalizer(self)

    #--------------------------------------------------------------------------
    # Methods that must be implemented by the data handler
    #--------------------------------------------------------------------------
    @classmethod
    def get_data_handler_data_type(cls):
        """Returns the data type that is handled by this data handler"""
        return TypeHex16

    @classmethod
    def get_data_handler(cls):
        """Returns the class reference of this data handler"""
        return Hex16Handler
