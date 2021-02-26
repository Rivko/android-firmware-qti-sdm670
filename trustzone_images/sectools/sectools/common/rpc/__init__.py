# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
import base64
import copy

from six import ensure_str, ensure_binary


class RPCError(RuntimeError):
    pass


class BaseServer(object):
    @classmethod
    def expose(cls, func):
        pass


class BaseClient(object):
    @classmethod
    def connect(cls, func):
        pass


class BinString(str):
    def __new__(cls, content):
        return super(BinString, cls).__new__(cls, ensure_str(content, encoding='latin-1'))

    def __init__(self, value):
        str.__init__(ensure_str(value, encoding='latin-1'))


def encode_bin_strings(data):
    data = copy.deepcopy(data)
    data['bin_fields'] = []
    for key, val in data.items():
        if isinstance(val, BinString):
            data['bin_fields'].append(key)
            data[key] = ensure_str(base64.b64encode(ensure_binary(val, encoding='latin-1')))
        elif isinstance(val, dict):
            data[key] = encode_bin_strings(val)
    return data


def decode_bin_strings(data):
    data = copy.deepcopy(data)

    for key, val in data.items():
        if isinstance(val, dict):
            data[key] = decode_bin_strings(val)

    if 'bin_fields' not in data:
        return data

    for field in data['bin_fields']:
        data[field] = BinString(base64.b64decode(ensure_binary(data[field])))

    # Cleanup this field and delete it so that we pass the right number of arguments to our methods
    del data['bin_fields']

    return data
