import sys
import struct
from optparse import OptionParser
import os

parser = OptionParser()
parser.add_option("--output", dest="output", help="SMD packet BIN file", metavar="FILE")
parser.add_option("--msg_type", dest="msg_type", help="message type: 'req' or 'cmd'")
parser.add_option("--set", dest="set", help="set 0: ACTIVE SET, 1: SLEEP SET, 2: NEXT ACTIVE SET")
parser.add_option("--msg_id", dest="msg_id", help="any non-zero value", default=None)
parser.add_option("--resource", dest="resource", help="resource type and id in the following format: type,id", default=None)
parser.add_option("--kvps", dest="kvps", help="kvps in the following format: key(,size,value)+(;key(,size,value)+)*")

(options, args) = parser.parse_args()

msg_hdr = {
        'msg_type': options.msg_type,
        'len_with_header': None,
        'msg_id': options.msg_id,
        'set': options.set,
        'resource_type': None if options.resource is None else options.resource.split(',')[0],
        'resource_id': None if options.resource is None else options.resource.split(',')[1],
        'data_len': None,
        }

kvps = []
datalen = 0
for kvp_csv in options.kvps.split(';'):
    kvp = {}
    val_len = 0
    size = None
    for i, e in enumerate(kvp_csv.split(',')):
        e = e.strip()
        if i == 0:
            kvp['key'] = e
            datalen += 4
        elif i % 2 == 1:
            if i == 1:
                datalen += 4
            size = int(e, 0)
            val_len += size
            datalen += size
        else:
            if not kvp.has_key('value'):
                kvp['value'] = []
            kvp['value'].append((size, int(e, 0)))
            size = None

    kvp['len'] = val_len
    kvps.append(kvp)


if options.msg_type == 'req':
    msg_hdr['data_len'] = datalen if msg_hdr['data_len'] is None else int(msg_hdr['data_len'], 0)
    msg_hdr['len_with_header'] = datalen + 20 if msg_hdr['len_with_header'] is None else int(msg_hdr['len_with_header'], 0)

    hdr_data = struct.pack('<7I',
            struct.unpack('<I', msg_hdr['msg_type']+'\x00'*(4-len(msg_hdr['msg_type'])))[0],
            msg_hdr['len_with_header'],
            int(msg_hdr['msg_id'], 0),
            int(msg_hdr['set'], 0),
            struct.unpack('<I', msg_hdr['resource_type']+'\x00'*(4-len(msg_hdr['resource_type'])))[0],
            int(msg_hdr['resource_id'], 0),
            msg_hdr['data_len'],
            )
elif options.msg_type == 'cmd':
    msg_hdr['len_with_header'] = datalen if msg_hdr['len_with_header'] is None else int(msg_hdr['len_with_header'], 0)

    hdr_data = struct.pack('<2I',
            struct.unpack('<I', msg_hdr['msg_type']+'\x00'*(4-len(msg_hdr['msg_type'])))[0],
            msg_hdr['len_with_header'],
            )

kvp_data = ''
for kvp in kvps:
    data = struct.pack('<2I',
        struct.unpack('<I', kvp['key']+'\x00'*(4-len(kvp['key'])))[0],
        kvp['len'],
        )

    for size, val in kvp['value']:
        if size == 4:
            fmt = '<I'
        elif size == 8:
            fmt = '<Q'
        data += struct.pack(fmt, val)
    kvp_data += data

data = ''.join([hdr_data, kvp_data])

with open(os.path.abspath(options.output), 'wb') as f:
    f.write(data)



