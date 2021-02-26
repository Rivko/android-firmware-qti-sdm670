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


class TypeHex64(TypeHex):
    """Custom type to specify 64-bit Hex"""
    pass


# Global instance of the 64-bit TypeHex class.
type_hex_64 = TypeHex64()


class Hex64Normalizer(Hex16Normalizer):

    def normalize(self, i_value, length=16):
        return super(Hex64Normalizer, self).normalize(i_value, length=length)


class Hex64Detailer(HexDetailer):

    def detail(self, i_format):
        """See :meth:`.Data_Handler.BaseDetailer.detail`"""
        assert isinstance(i_format, TypeHex64)
        return DataDetail(DataType.Hex64, self.data_handler, i_format)


class Hex64Handler(HexHandler):

    def __init__(self, data_handler):
        """Initialized the various features."""
        super(Hex64Handler, self).__init__(data_handler)
        self._i_detailer = Hex64Detailer(self)
        self._i_normalizer = Hex64Normalizer(self)

    #--------------------------------------------------------------------------
    # Methods that must be implemented by the data handler
    #--------------------------------------------------------------------------
    @classmethod
    def get_data_handler_data_type(cls):
        """Returns the data type that is handled by this data handler"""
        return TypeHex64

    @classmethod
    def get_data_handler(cls):
        """Returns the class reference of this data handler"""
        return Hex64Handler
