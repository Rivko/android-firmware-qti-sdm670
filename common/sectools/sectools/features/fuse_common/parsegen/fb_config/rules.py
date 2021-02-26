# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Mar 11, 2014

@author: francisg
'''


import defines


from __secfile__ import DIR_PATH
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger


def set_field_value(data_model, region_id, field_id, value ):
    '''
    helper function to find the field and set its value
    '''
    if not isinstance(value, str):
        value = hex(value).rstrip('L')

    for region in data_model.module[0].fuse_region:
        if region.id == region_id:
            for fuse in region.fuse:
                for field in fuse.field:
                    if field.id == field_id:
                        field.value = value
                        logger.debug2('USER config set region: '+region.id+', field: '+field.id+', value:'+field.value)
                        return True, field.value

    # got to this point, not found
    raise RuntimeError("USER config entry not found in OEM config: "+region_id+' : '+field_id)

def get_field_value(data_model, region_id, field_id ):
    '''
    helper function to get the field value from data model
    '''
    for region in data_model.module[0].fuse_region:
        if region.id == region_id:
            for fuse in region.fuse:
                for field in fuse.field:
                    if field.id == field_id:
                        logger.debug2('USER config get region: '+region.id+', field: '+field.id+', value:'+field.value)
                        return field.value

    # got to this point, not found
    raise RuntimeError("USER config entry not found in OEM config: "+region_id+' : '+field_id)

def compute_fec(data):
    '''
    computes 7 fec bits from a 56 bit data
    '''
    LFSR = 0
    temp = 0

    if not isinstance(data, int) and not isinstance(data, long):
        raise RuntimeError('compute_fec: invalid input data type, INT expected'+data)

    for bit_position in range(0,56):
        # Xor of FEC linear shift feedback register bit 0 with bit current bit position of data
        temp = (LFSR & 0x1) ^ ((data >> bit_position) & 0x1)

        '''
        #  LFSR[0] = LFSR[1] Xor temp
        #  LFSR[1] = LFSR[2]
        #  LFSR[2] = LFSR[3]
        #  LFSR[3] = LFSR[4]
        #  LFSR[4] = LFSR[5] Xor temp
        #  LFSR[5] = LFSR[6]
        #  LFSR[6] = temp
        '''
        LFSR = ( (((LFSR >> 1) & 0x1) ^ temp) << 0 ) \
                 | ( ( (LFSR >> 2) & 0x1)  << 1 )        \
                 | ( ( (LFSR >> 3) & 0x1)  << 2 )        \
                 | ( ( (LFSR >> 4) & 0x1)  << 3 )        \
                 | ( (((LFSR >> 5) & 0x1) ^ temp) << 4 ) \
                 | ( ( (LFSR >> 6) & 0x1)  << 5 )        \
                 | (temp << 6)

    return (LFSR & defines.FEC_BIT_MASK)

def generate_fec(data_model, region_id, row):
    '''
    helper function to generate fec from fuse region ID
    '''
    try:
        for region in data_model.module[0].fuse_region:
            if region.id == region_id:
                for fuse in region.fuse:
                    if fuse.n == row:
                        fec = compute_fec(int(fuse.value, 16) & defines.FEC_DATA_56BIT_MASK)
                        # now that we got fec, find the fec field in this row and set it
                        for field in fuse.field:
                            if field.id == 'FEC_VALUE':
                                field.value = hex(fec)
                                logger.debug2( 'generate_fec for '+region_id+', row '+str(row)+' : '
                                             +hex(fec)+' - '+hex(int(fuse.value, 16) & defines.FEC_DATA_56BIT_MASK) )
                                return fuse.value

        # got to this point, not found
        raise RuntimeError("USER config FEC_VALUE field not found: "+region_id+' : row '+row)
        raise

    except:
        raise RuntimeError("Error from generate_fec(): "+region_id)

def compute_fec_15_10(data):
    '''
    computes 5 fec bits from a 10 bit data
    '''
    LFSR = 0
    temp = 0

    if not isinstance(data, int) and not isinstance(data, long):
        raise RuntimeError('compute_fec: invalid input data type, INT expected'+data)

    for bit_position in range(0,10):
        # Xor of FEC linear shift feedback register bit 0 with bit current bit position of data
        temp = (LFSR & 0x1) ^ ((data >> bit_position) & 0x1)

        '''
        #  LFSR[0] = LFSR[1] Xor temp
        #  LFSR[1] = LFSR[2]
        #  LFSR[2] = LFSR[3] Xor temp
        #  LFSR[3] = LFSR[4]
        #  LFSR[4] = temp
        '''
        LFSR = ( (((LFSR >> 1) & 0x1) ^ temp) << 0 ) \
                 | ( ( (LFSR >> 2) & 0x1)  << 1 )        \
                 | ( (((LFSR >> 3) & 0x1) ^ temp) << 2 ) \
                 | ( ( (LFSR >> 4) & 0x1)  << 3 )        \
                 | (temp << 4)

    return (LFSR & defines.FEC_15_10_BIT_MASK)

def update_fec_15_10(data_model, data, region_id, field_name, row):

    try:
        for region in data_model.module[0].fuse_region:
            if region.id == region_id:
                for fuse in region.fuse:
                    if fuse.n == row:
                        fec = compute_fec_15_10(data)
                        # now that we got fec, find the fec field in this row and set it
                        for field in fuse.field:
                            if field.id == field_name:
                                field.value = hex(fec)
                                logger.debug2( 'generate_fec for '+region_id+', row '+str(row)+' : '
                                             +hex(fec)+' - '+hex(data) )
                                return fuse.value

        # got to this point, not found
        raise RuntimeError("USER config" +field_name+ " field not found: "+region_id+' : row '+row)
        raise

    except:
        raise RuntimeError('Error from generate_fec(): '+region_id)

def generate_sha256_hash_from_file(filepath):
    import hashlib
    import os

    filepath = c_path.normalize(os.path.join(DIR_PATH, filepath))

    try:
        cert_file = open(filepath, 'rb')
        file_data = cert_file.read()
        cert_file.close()

    except Exception, e:
        error_str='file operation failed. {0}'.format(e)
        raise RuntimeError(error_str)
        raise e

    sha256 = hashlib.sha256()
    sha256.update(file_data)
    hash_str = sha256.digest()

    return hash_str

def get_keydata_from_file(filepath):
    import os
    import binascii

    filepath = c_path.normalize(os.path.join(DIR_PATH, filepath))

    with open(filepath, 'rb') as f:
        f.seek(0)
        file_data = binascii.hexlify(f.read())
        f.close()
        return file_data

def find_region_entry_in_datamodel(data_model, region):

    for fuse_reg in data_model.module[0].fuse_region:
        if fuse_reg.id == region:
            return region

    return None
