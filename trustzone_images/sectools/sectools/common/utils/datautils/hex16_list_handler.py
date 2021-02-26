# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import json

from .data_detail import DataType
from .data_detail import DataDetail
from .hex_handler import TypeHex
from .hex_handler import HexHandler
from .hex_handler import HexDetailer
from .hex16_handler import Hex16Normalizer


class TypeHex16List(TypeHex):
    """Custom type to specify 16-bit Hex strings separated by space"""
    pass


# Global instance of the 16-bit TypeHex16List class.
type_hex_16_list = TypeHex16List()


class Hex16ListNormalizer(Hex16Normalizer):

    def normalize(self, i_value, length=4):
        errors = []
        normalized_values = []
        try:
            i_value = str(json.loads(i_value))
        except ValueError:
            pass
        for value in i_value.split():
            try:
                normalized_values.append(
                    super(Hex16ListNormalizer, self).normalize(value, length=length))
            except Exception as e:
                errors.append(str(e))
        if errors:
            raise ValueError("Error parsing %r\n%s" %
                             (i_value, "\n".join(["  " + x for x in errors])))
        return " ".join(normalized_values)


class Hex16ListDetailer(HexDetailer):

    def detail(self, i_format):
        """See :meth:`.Data_Handler.BaseDetailer.detail`"""
        assert isinstance(i_format, TypeHex16List)
        return DataDetail(DataType.Hex16List, self.data_handler, i_format)


class Hex16ListHandler(HexHandler):

    def __init__(self, data_handler):
        """Initialized the various features."""
        super(Hex16ListHandler, self).__init__(data_handler)
        self._i_detailer = Hex16ListDetailer(self)
        self._i_normalizer = Hex16ListNormalizer(self)

    #--------------------------------------------------------------------------
    # Methods that must be implemented by the data handler
    #--------------------------------------------------------------------------
    @classmethod
    def get_data_handler_data_type(cls):
        """Returns the data type that is handled by this data handler"""
        return TypeHex16List

    @classmethod
    def get_data_handler(cls):
        """Returns the class reference of this data handler"""
        return Hex16ListHandler
