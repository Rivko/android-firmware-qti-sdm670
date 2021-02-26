# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
import base64
import copy


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
    def __init__(self, value):
        str.__init__(value)


def encode_bin_strings(data):
    data = copy.deepcopy(data)
    data['bin_fields'] = []
    for key, val in data.items():
        if isinstance(val, BinString):
            data['bin_fields'].append(key)
            data[key] = base64.b64encode(val)
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
        data[field] = BinString(base64.b64decode(data[field]))

    # Cleanup this field and delete it so that we pass the right number of arguments to our methods
    del data['bin_fields']

    return data
