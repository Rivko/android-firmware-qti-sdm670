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

from sectools.features.fuse_common.parsegen.fb_config.defines import FEC_DATA_56BIT_MASK, FEC_IN_ROW, DIFF_FEC_REG, \
                                        DIFF_FEC_REG_NAZGUL, KP_FEC_REG
from sectools.common.utils.c_logging import logger
from sectools.features.fuse_common.parsegen.fb_config.rules import generate_fec,  \
                                        get_keydata_from_file,  set_field_value


class _CUSTKEY1(object):
    '''
    _CUSTKEY_ONE rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None', encryption_key='None',
                 encryption_file = 'None', region_name='None', row_name='None' ):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []
        self.encryption_key = encryption_key
        self.encryption_file = encryption_file
        self.region_name = region_name
        if row_name == 'CUSTKEY1_L1_ROW0':
            self.row0 = 'CUSTKEY1_L1_ROW0'
            self.row1 = 'CUSTKEY1_L1_ROW1'
            self.row2 = 'CUSTKEY1_L1_ROW2'
        elif row_name == 'CUSTKEY1_ROT_ROW0':
            self.row0 = 'CUSTKEY1_ROT_ROW0'
            self.row1 = 'CUSTKEY1_ROT_ROW1'
            self.row2 = 'CUSTKEY1_ROT_ROW2'
        else:
            raise


    def addEntry(self, entry):
        '''
        adds user entries
        '''
        if len(self.entries):
            raise RuntimeError('Cannot add '+entry['name']+', '+self.entries[0]['name']+' already exists!')
            raise

        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''
        if not self.entries:
            return

        encryption_key = None

        try:
            for entry in self.entries:
                if entry['name'] == self.encryption_key:
                    encryption_key = entry['value']
                elif entry['name'] == self.encryption_file:
                    encryption_file = entry['value']
                    encryption_key = get_keydata_from_file(encryption_file)

            if len(encryption_key) > 32:
                raise RuntimeError(str(self.encryption_key) + ' is more than 128 bits [ 32 chars], len = '+str(len(encryption_key)))

            encryption_key = int(encryption_key, 16)

            if encryption_key == 0:
                logger.debug2('encryption_key is all 0, skipping')
                return

            encryption_key_old = hex(encryption_key)[2:].rstrip('L').rjust(32,'0')
            encryption_key = ''
            for idx in range(len(encryption_key_old)/2):
                encryption_key += encryption_key_old[-2:]
                encryption_key_old = encryption_key_old[:-2]
            encryption_key = int(encryption_key, 16)

            custkey_row0 = encryption_key & FEC_DATA_56BIT_MASK
            custkey_row1 = (encryption_key >> 56) & FEC_DATA_56BIT_MASK
            custkey_row2 = (encryption_key >> 112) & 0xFFFF

            # merge into data model
            set_field_value(data_model, region_id=self.region_name , field_id=self.row0, value=custkey_row0)
            set_field_value(data_model, region_id=self.region_name , field_id=self.row1, value=custkey_row1)
            set_field_value(data_model, region_id=self.region_name , field_id=self.row2, value=custkey_row2)

            # Generate FEC bits
            row0 = generate_fec(data_model, region_id=self.region_name , row=0)
            row1 = generate_fec(data_model, region_id=self.region_name , row=1)
            row2 = generate_fec(data_model, region_id=self.region_name , row=2)

            # Lastly, set FEC_EN bit
            # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
            if self.chipset in FEC_IN_ROW:
                if int(row0,16):
                    set_field_value(data_model, region_id=self.region_name , field_id='CUSTKEY1_ROW0_FEC_EN0', value=0x1)
                if int(row1,16):
                    set_field_value(data_model, region_id=self.region_name , field_id='CUSTKEY1_ROW1_FEC_EN1', value=0x1)
                if int(row2,16):
                    set_field_value(data_model, region_id=self.region_name , field_id='CUSTKEY1_ROW2_FEC_EN2', value=0x1)

            elif self.chipset in DIFF_FEC_REG:
                if self.row0 == 'CUSTKEY1_L1_ROW0':
                    if self.chipset in KP_FEC_REG:
                        # set FEC_EN region bit for SPARE 19, 20, 21 REGION
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION19_FEC_EN', value=0x1)
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION20_FEC_EN', value=0x1)
                    else:
                        # set FEC_EN region bit for SPARE 21, 22, 23 REGION
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION22_FEC_EN', value=0x1)
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION23_FEC_EN', value=0x1)

                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION21_FEC_EN', value=0x1)

                else:
                    if self.chipset in KP_FEC_REG:
                        # set FEC_EN region bit for SPARE 22, 23, 24 REGION
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION22_FEC_EN', value=0x1)
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION23_FEC_EN', value=0x1)
                    else:
                        # set FEC_EN region bit for SPARE 24, 25, 26 REGION
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION25_FEC_EN', value=0x1)
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION26_FEC_EN', value=0x1)

                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION24_FEC_EN', value=0x1)

            else:
                # set FEC_EN region bit for Region31 (CUST_KEY1 region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION31_FEC_EN', value=0x1)

        except Exception, e:
            raise RuntimeError('Error from configure(): %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        # Need to be Implemented
        return


class _OEM_IMAGE_ENCR_KEY(object):
    '''
    _IMAGE_ENCR_KEY1 rule class that takes in user config and generates a fuse region object
    based on set rules
    '''
    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.entries = []

    def addEntry(self, entry):
        '''
        adds user entries
        '''
        if len(self.entries):
            raise RuntimeError('Cannot add '+entry['name']+', '+self.entries[0]['name']+' already exists!')
            raise

        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''
        if not self.entries:
            return

        oem_image_encr_key = None

        try:
            for entry in self.entries:
                if entry['name'] == 'oem_image_encr_key':
                    oem_image_encr_key = entry['value']
                elif entry['name'] == 'oem_image_encr_key_file':
                    oem_image_encr_key_file = entry['value']
                    oem_image_encr_key = get_keydata_from_file(oem_image_encr_key_file)

            if len(oem_image_encr_key) > 32:
                raise RuntimeError('oem_image_encr_key is more than 128 bits [ 32 chars], len = '+str(len(oem_image_encr_key)))

            oem_image_encr_key = int(oem_image_encr_key, 16)

            if oem_image_encr_key == 0:
                logger.debug2('oem_image_encr_key is all 0, skipping')
                return

            oem_image_encr_key_old = hex(oem_image_encr_key)[2:].rstrip('L').rjust(32,'0')
            oem_image_encr_key = ''
            for idx in range(len(oem_image_encr_key_old)/2):
                oem_image_encr_key += oem_image_encr_key_old[-2:]
                oem_image_encr_key_old = oem_image_encr_key_old[:-2]
            oem_image_encr_key = int(oem_image_encr_key, 16)

            oem_imageencrkey_row0 = oem_image_encr_key & FEC_DATA_56BIT_MASK
            oem_imageencrkey_row1 = (oem_image_encr_key >> 56) & FEC_DATA_56BIT_MASK
            oem_imageencrkey_row2 = (oem_image_encr_key >> 112) & 0xFFFF

            # merge into data model
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_IMAGE_ENCR_KEY', field_id='ENCR_KEY_ROW0', value=oem_imageencrkey_row0)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_IMAGE_ENCR_KEY', field_id='ENCR_KEY_ROW1', value=oem_imageencrkey_row1)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_IMAGE_ENCR_KEY', field_id='ENCR_KEY_ROW2', value=oem_imageencrkey_row2)

            # Generate FEC bits
            row0 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_IMAGE_ENCR_KEY', row=0)
            row1 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_IMAGE_ENCR_KEY', row=1)
            row2 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_IMAGE_ENCR_KEY', row=2)

            # Lastly, set FEC_EN bit
            # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
            if self.chipset in FEC_IN_ROW:
                if int(row0,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_OEM_IMAGE_ENCR_KEY', field_id='OEMIMAGEENCRKEY_ROW0_FEC_EN0', value=0x1)
                if int(row1,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_OEM_IMAGE_ENCR_KEY', field_id='OEMIMAGEENCRKEY_ROW1_FEC_EN1', value=0x1)
                if int(row2,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_OEM_IMAGE_ENCR_KEY', field_id='OEMIMAGEENCRKEY_ROW2_FEC_EN2', value=0x1)

            elif self.chipset in DIFF_FEC_REG_NAZGUL:
                # set FEC_EN region bit for Region19 (OEM_IMAGE_ENCR_KEY region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION19_FEC_EN', value=0x1)

            else:
                # set FEC_EN region bit for Region23 (OEM_IMAGE_ENCR_KEY region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION23_FEC_EN', value=0x1)

        except Exception, e:
            raise RuntimeError('Error from configure(): %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        # Need to be Implemented
        return


class _RULES(object):
    '''
    takes in the user entries and routes them to their proper class for handling
    '''

    def __init__(self, chipset='None'):
        '''
        Constructor
        '''
        self.chipset = chipset
        self.regions = {}
        self.regions['QFPROM_RAW_CUST_KEY1_L1'] = _CUSTKEY1(chipset, 'L1_encryption_key', 'L1_encryption_file', 'QFPROM_RAW_CUST_KEY1_L1', 'CUSTKEY1_L1_ROW0')
        self.regions['QFPROM_RAW_CUST_KEY1_ROT'] = _CUSTKEY1(chipset, 'ROT_encryption_key', 'ROT_encryption_file', 'QFPROM_RAW_CUST_KEY1_ROT', 'CUSTKEY1_ROT_ROW0')
        self.regions['QFPROM_RAW_OEM_IMAGE_ENCR_KEY'] = _OEM_IMAGE_ENCR_KEY(chipset)
        pass


    def process(self, entries, data_model):
        '''
        handles the entries according to their fuse region class
        '''
        try:
            # push the entries to their corresponding regions
            for entry in entries:
                if entry['ignore'] == False:
                    # find the region that the entry belongs to and add that entry
                    region_id = self.find_region_from_entry(data_model, entry['name'])

                    if region_id is not None:
                        self.regions[region_id].addEntry(entry)
                    else:
                        raise RuntimeError('UNSUPPORTED user entry: ' + entry['name'] + ', please check USER.xml file')

            # configure regions based on entries
            for region in self.regions:
                self.regions[region].configure(data_model)

        except Exception, e:
            raise RuntimeError('Error from: %s, error=%s' + self.__class__.__name__, str(e))

    def update_from_data_model(self, data_model, entries):

        try:
            for entry in entries:
                if entry.ignore == False:
                    name = entry.name
                    region_id = self.find_region_from_entry(data_model, name)

                    if region_id is not None:
                        self.regions[region_id].update_entry(data_model, entries, name)
                    else:
                        raise RuntimeError('UNSUPPORTED user entry: ' + name + ', please check USER.xml file')

        except Exception, e:
            raise RuntimeError('Error from: %s, error=%s' + self.__class__.__name__, str(e))

    def find_region_from_entry(self, data_model, name):
        field_id = defines.FIELD_ID_MAP[name]
        for region in data_model.module[0].fuse_region:
            for fuse in region.fuse:
                for field in fuse.field:
                    if field.id == field_id:
                        return region.id

        # got to this point, not found
        raise RuntimeError("USER config entry field not found in data_model: "+field_id)
