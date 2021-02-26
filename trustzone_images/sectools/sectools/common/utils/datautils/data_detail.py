# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


class DataType(object):

    class Bool: pass
    class Int: pass
    class Hex: pass
    class Hex16: pass
    class Hex32: pass
    class Hex64: pass
    class Hex16List: pass
    class String: pass
    class Path: pass
    class List: pass
    class Complex: pass
    class Enumerated: pass


_data_type_str = {
    DataType.Bool : 'boolean',
    DataType.Int : 'integer',
    DataType.Hex : 'hex',
    DataType.Hex16 : '16-bit hex',
    DataType.Hex32 : '32-bit hex',
    DataType.Hex64 : '64-bit hex',
    DataType.Hex16List : 'space-separated list of 16-bit hex',
    DataType.String : 'string',
    DataType.Path : 'path',
    DataType.List : 'list',
    DataType.Complex : 'complex',
    DataType.Enumerated: "string",
}


class DataDetail(object):

    SIMPLE_TYPES = [DataType.Bool,
                    DataType.Int,
                    DataType.Hex,
                    DataType.Hex16,
                    DataType.Hex32,
                    DataType.Hex64,
                    DataType.Hex16List,
                    DataType.String,
                    DataType.Path,
                    DataType.Enumerated,
                    ]

    def __init__(self, data_type, data_handler, data_format):
        self.type = data_type
        self.is_optional = False
        self.is_choice = False
        self.choices = []
        self.children = None

        self._handler = data_handler
        self._format = data_format
        self._value = None
        self.value_set = False

    @property
    def is_simple(self):
        return self.type in self.SIMPLE_TYPES

    @property
    def value(self):
        return self._value

    @value.setter
    def value(self, value):
        if value is None and not self.is_optional:
            raise RuntimeError('must not be empty.')
        if self.is_choice and value not in self.choices:
            raise RuntimeError('must be one of: ' + str(self.choices))
        self._value = self._handler.validate(value, self._format)
        self.value_set = True

    def set_string_value(self, value):
        if value == '':
            value = None
        elif self.type == DataType.Int:
            try:
                value = int(value, 10)
            except ValueError:
                raise RuntimeError('must be an integer.')
        self.value = value

    @property
    def type_str(self):
        return _data_type_str[self.type]

    def __repr__(self):
        return ('Type: ' + str(self.type) + '\n'
                'Is_optional: ' + str(self.is_optional) + '\n'
                'Is_choice: ' + str(self.is_choice) + '\n'
                'Choices: ' + str(self.choices) + '\n'
                'Children: ' + str(self.children) + '\n'
                'Value: ' + str(self._value)
                )
