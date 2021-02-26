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
from sectools.common.utils.c_logging import logger
from sectools.features.fuse_common.parsegen.fb_config.defines import FEC_DATA_56BIT_MASK, \
                    FEC_DATA_64BIT_MASK, FEC_IN_ROW, DIFF_FEC_REG, DIFF_FEC_REG_NAZGUL, FEC_DATA_32BIT_MASK, \
                    ROT_SUPPORTED, FEC_SHADOW, MRC_2_0_CHIPSETS
from sectools.features.fuse_common.parsegen.fb_config.rules import generate_fec, \
                    get_field_value, generate_sha256_hash_from_file, set_field_value, \
                    get_keydata_from_file, find_region_entry_in_datamodel, update_fec_15_10

from sectools.features.fuse_common.parsegen.fb_config.defines import IPQ_CHIPSET


class _OEM_CONFIG(object):
    '''
    OEM_CONFIG rule class that takes in user config and generates a fuse region object
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
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        '''
        Fuseblower - Secboot Mode Rules :
        QFPROM_RAW_OEM_CONFIG_ROW1_LSB
        Bits     Field Name
        31: 16   OEM_HW_ID
        15: 8    ANTI_ROLLBACK_FEATURE_EN
        7: 4     SPARE0
        3: 0     ROOT_CERT_TOTAL_NUM

        QFPROM_RAW_OEM_CONFIG_ROW1_MSB
        Bits     Field Name     Field Values
        31: 16   SPARE0         READ ONLY
        15: 0    OEM_PRODUCT_ID

        QFPROM_RAW_OEM_CONFIG_ROW0_MSB
        Bit     Field Name
        31        WDOG_EN
        20        DISABLE_ROT_TRANSFER
        1         APPS_NIDEN_DISABLE
        0         APPS_DBGEN_DISABLE

        '''
        if not self.entries:
            return

        try:
            if self.chipset in IPQ_CHIPSET:
                region_id = 'QFPROM_RAW_OEM_CONFIG'
                oem_hwid_9to0 = 0
                oem_hwid_15to10 = 0
                oem_productid_3to0 = 0
                oem_productid_5to4 = 0
                oem_productid_13to6 = 0
                oem_productid_15to14 = 0

                for entry in self.entries:
                    if entry['name'] == 'oem_hwid':
                        oem_hwid = entry['value']
                        oem_hwid = int(oem_hwid, 16)
                        oem_hwid_9to0 = oem_hwid & 0x3FF
                        oem_hwid_15to10 = (oem_hwid >> 10) & 0x3F

                        set_field_value(data_model, region_id, field_id='OEM_HW_ID_9_0', value=oem_hwid_9to0)
                        set_field_value(data_model, region_id, field_id='OEM_HW_ID_15_10', value=oem_hwid_15to10)

                    elif entry['name'] == 'oem_productid':
                        oem_productid = entry['value']
                        oem_productid = int(oem_productid, 16)
                        oem_productid_3to0 = oem_productid & 0xF
                        oem_productid_5to4 = (oem_productid >> 4) & 0x3
                        oem_productid_13to6 = (oem_productid >> 6) & 0xFF
                        oem_productid_15to14 = (oem_productid >> 14) & 0x3

                        set_field_value(data_model, region_id, field_id='OEM_PRODUCT_ID_3_0', value=oem_productid_3to0)
                        set_field_value(data_model, region_id, field_id='OEM_PRODUCT_ID_5_4', value=oem_productid_5to4)
                        set_field_value(data_model, region_id, field_id='OEM_PRODUCT_ID_13_6', value=oem_productid_13to6)
                        set_field_value(data_model, region_id, field_id='OEM_PRODUCT_ID_15_14', value=oem_productid_15to14)

                    elif entry['name'] == 'watchdog_enable':
                        watchdog_enable = 0
                        if entry['value'] == 'true': watchdog_enable = 1
                        set_field_value(data_model, region_id, field_id='WDOG_EN', value=watchdog_enable)

                # Generate FEC bits
                spare0 = get_field_value(data_model, region_id, 'SPARE0')
                spare0 = int(spare0, 16)
                data0_row0 = update_fec_15_10(data_model, oem_hwid_9to0, region_id, 'FEC_VALUE0', 2)
                data1_row0 = update_fec_15_10(data_model, oem_hwid_15to10 + oem_productid_3to0, region_id, 'FEC_VALUE1', 2)
                data2_row0 = update_fec_15_10(data_model, oem_productid_5to4 + oem_productid_13to6, region_id, 'FEC_VALUE2', 2)
                data3_row0 = update_fec_15_10(data_model, oem_productid_15to14 + spare0, region_id, 'FEC_VALUE3', 2)

            else:
                # Fuseblower - Secboot Mode Rules
                for entry in self.entries:
                    if entry['name'] == 'oem_hw_id':
                        oem_hw_id = str(entry['value'])
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='OEM_HW_ID', value=oem_hw_id)

                    elif entry['name'] == 'mrc_cert_count':
                        # CR 612309 - translate mrc_cert_count in config file into MRC count index
                        if '0x' in entry['value']:
                            mrc_cert_count = int(entry['value'],16)
                        else:
                            mrc_cert_count = int(entry['value'])

                        # CR 612309 - adjust blow value of ROOT_CERT_TOTAL_NUM
                        if(mrc_cert_count > 0):
                            mrc_cert_count -= 1

                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='ROOT_CERT_TOTAL_NUM', value=mrc_cert_count)

                        if self.chipset in ROT_SUPPORTED and self.chipset not in MRC_2_0_CHIPSETS:
                            # CR 713126 - DO NOT blow write disable for CPK1 if mrc count is > 1, means ROOT_CERT_TOTAL_NUM blow value is > 0
                            cpk1_write_disable = 1
                            if(mrc_cert_count > 0):
                                cpk1_write_disable = 0
                            set_field_value(data_model, region_id='QFPROM_RAW_RD_WR_PERM', field_id='CUST_PRIV_KEY1', value=cpk1_write_disable)

                    elif entry['name'] == 'oem_product_id':
                        oem_product_id = str(entry['value'])
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='OEM_PRODUCT_ID', value=oem_product_id)

                    elif entry['name'] == 'apps_debug_disabled':
                        apps_debug_disabled = 0
                        if entry['value'] == 'true': apps_debug_disabled = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='APPS_DBGEN_DISABLE', value=apps_debug_disabled)

                    elif entry['name'] == 'allow_bus_dcvs':
                        apps_niden_disabled = 0
                        # if enable bus dcvs is false (disabled), only then will APPS_NIDEN_DISABLED be set,
                        if entry['value'] == 'false': apps_niden_disabled = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='APPS_NIDEN_DISABLE', value=apps_niden_disabled)

                    # Implementation - FR 23467
                    elif entry['name'] == 'watchdog_enable':
                        watchdog_enable = 0
                        if entry['value'] == 'true': watchdog_enable = 1
                        set_field_value(data_model=data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='WDOG_EN', value=watchdog_enable)

                    elif entry['name'] == 'disable_rot_transfer':
                        disable_rot_transfer = 0
                        if entry['value'] == 'true': disable_rot_transfer = 1
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='DISABLE_ROT_TRANSFER', value=disable_rot_transfer)
                        set_field_value(data_model, region_id='QFPROM_RAW_WR_PERM', field_id='WR_PERM', value=disable_rot_transfer)
                        set_field_value(data_model, region_id='QFPROM_RAW_WR_PERM', field_id='FEC_EN', value=disable_rot_transfer)
                        set_field_value(data_model, region_id='QFPROM_RAW_WR_PERM', field_id='IMAGE_ENCR_KEY1', value=disable_rot_transfer)
                        set_field_value(data_model, region_id='QFPROM_RAW_WR_PERM', field_id='PK_HASH1', value=disable_rot_transfer)

                    elif entry['name'] == 'sp_nvm_ar_config':
                        if '0x' in entry['value']:
                            sp_nvm_ar_config = int(entry['value'],16)
                        else:
                            sp_nvm_ar_config = int(entry['value'])
                        if (sp_nvm_ar_config >= 0 and sp_nvm_ar_config < 3):
                            set_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id='SP_NVM_AR_CONFIG', value=sp_nvm_ar_config)
                        else:
                            raise RuntimeError('Invalid sp_nvm_ar_config value. Allowed values are 0, 1, 2')

        except Exception, e:
            raise RuntimeError('Error from: %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_OEM_CONFIG', field_id=field_id )
        print value

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'mrc_cert_count':
            if int(value,16) > 0:
                user_entry.value = str(int(value,16)+1)
            else:
                # root_cert_num value is 0
                user_entry.value = '0'
        elif name == 'apps_debug_disabled':
            if int(value,16) == 1:
                user_entry.value = 'true'
            else:
                user_entry.value = 'false'

        elif name == 'allow_bus_dcvs':
            # enable_bus_dcvs affects APPS_NIDEN_DISABLE inversely
            # if APPS_NIDEN_DISABLE value is 1, the 'enable_bus_dcvs' user entry should be false
            if int(value,16) == 1:
                user_entry.value = 'false'
            else:
                user_entry.value = 'true'

        # Implementation - FR 23467
        elif name == 'watchdog_enable':
            if int(value,16) == 1:
                user_entry.value = 'true'
            else:
                user_entry.value = 'false'

        elif name == 'disable_rot_transfer':
            if int(value,16) == 1:
                user_entry.value = 'true'
            else:
                user_entry.value = 'false'

        else:
            user_entry.value = value


class _MRC_2_0(object):
    '''
    MRC 2.0 rule class that takes in user config and generates a fuse region object
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
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        '''
        Fuseblower - MRC 2.0 Rules

        '''
        if not self.entries:
            return

        try:
            # below condition is for chipsets supporting MRC 2.0
            if self.chipset in MRC_2_0_CHIPSETS:
                region_id = 'QFPROM_RAW_MRC_2_0'
                for entry in self.entries:

                    if entry['name'] == 'root_cert_activate_index':
                        if '0x' in entry['value']:
                            root_cert_activate_index = int(entry['value'],16)
                        else:
                            root_cert_activate_index = int(entry['value'])
                        root_cert_activate_index = 1 << root_cert_activate_index

                        set_field_value(data_model, region_id, field_id='ROOT_CERT_ACTIVATION_LIST', value=root_cert_activate_index)

                    elif entry['name'] == 'root_cert_revoke_index':
                        if '0x' in entry['value']:
                            root_cert_revoke_index = int(entry['value'],16)
                        else:
                            root_cert_revoke_index = int(entry['value'])
                        root_cert_revoke_index = 1 << root_cert_revoke_index

                        set_field_value(data_model, region_id, field_id='ROOT_CERT_REVOCATION_LIST', value=root_cert_revoke_index)

        except Exception, e:
            raise RuntimeError('Error from: %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_MRC_2_0', field_id=field_id )
        print value

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break
        user_entry.value = value


class _OEM_SECURE(object):
    '''
    OEM_SECURE rule class that takes in user config and generates a fuse region object
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
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        '''
        Fuseblower - OEM_SECURE Rules :

        '''
        if not self.entries:
            return

        try:
            # below condition is for Napier QCA chipset
            if self.chipset == '6290':
                region_id = 'QFPROM_RAW_OEM_SECURE'
                for entry in self.entries:
                    if entry['name'] == 'oem_hw_id':
                        oem_hw_id = str(entry['value'])
                        set_field_value(data_model, region_id, field_id='OEM_HW_ID', value=oem_hw_id)

                    elif entry['name'] == 'oem_product_id':
                        oem_product_id = str(entry['value'])
                        set_field_value(data_model, region_id, field_id='OEM_PRODUCT_ID', value=oem_product_id)

                    elif entry['name'] == 'watchdog_enable':
                        watchdog_enable = 0
                        if entry['value'] == 'true': watchdog_enable = 1
                        set_field_value(data_model, region_id, field_id='WDOG_EN', value=watchdog_enable)

                    elif entry['name'] == 'total_rot_num':
                        if '0x' in entry['value']:
                            total_rot_num = int(entry['value'],16)
                        else:
                            total_rot_num = int(entry['value'])

                        if(total_rot_num > 0):
                            total_rot_num -= 1

                        set_field_value(data_model, region_id, field_id='TOTAL_ROT_NUM', value=total_rot_num)

                    elif entry['name'] == 'rot_activation':
                        if '0x' in entry['value']:
                            rot_activation = int(entry['value'],16)
                        else:
                            rot_activation = int(entry['value'])
                        rot_activation = 1 << rot_activation

                        set_field_value(data_model, region_id, field_id='ROT_ACTIVATION', value=rot_activation)

                    elif entry['name'] == 'rot_revocation':
                        if '0x' in entry['value']:
                            rot_revocation = int(entry['value'],16)
                        else:
                            rot_revocation = int(entry['value'])
                        rot_revocation = 1 << rot_revocation

                        set_field_value(data_model, region_id, field_id='ROT_REVOCATION', value=rot_revocation)

                    elif entry['name'] == 'SECURE_BOOT0_use_serial_num':
                        SECURE_BOOT0_use_serial_num = 0
                        if entry['value'] == 'true': SECURE_BOOT0_use_serial_num = 1

                        set_field_value(data_model, region_id, field_id='SECURE_BOOT0_USE_SERIAL_NUM', value=SECURE_BOOT0_use_serial_num)

        except Exception, e:
            raise RuntimeError('Error from: %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_OEM_SECURE', field_id=field_id )
        print value

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'total_rot_num':
            if int(value,16) > 0:
                user_entry.value = str(int(value,16)+1)
            else:
                # root_cert_num value is 0
                user_entry.value = '0'
        # Implementation - FR 23467
        elif name == 'watchdog_enable':
            if int(value,16) == 1:
                user_entry.value = 'true'
            else:
                user_entry.value = 'false'
        else:
            user_entry.value = value




class _OEM_SEC_BOOT(object):
    '''
    OEM_SEC_BOOT rule class that takes in user config and generates a fuse region object
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
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        '''
        OEM fuse settings for this secure boot configuration. This can be overridden with the Qualcomm settings if necessary.
        Bit 7: Reserved
        Bit 6: Use Serial Num for secure boot authentication (0: Use OEM ID (Default), 1: Use Serial Num)
        Bit 5: Authentication Enable (0: no auth, 1: auth required)
        Bit 4: PK Hash in Fuse (0: SHA-256 hash of root cert is ROM, 1: SHA-256 hash of root cert to use is in OEM_PK_HASH)
        Bits 3-0: ROM PK Hash Index (If PK Hash in Fuse is 0, then this index selects which of 16 keys in ROM to use)

        MSB
        Bits     Field Name
        30: 24   FEC_VALUE
        23: 16   SEC_BOOT7
        15:  8   SEC_BOOT6
         7:  0   SEC_BOOT5

        LSB
        Bits     Field Name
        31: 24   SEC_BOOT4
        23: 16   SEC_BOOT3
        15:  8   SEC_BOOT2
         7:  0   SEC_BOOT1
        '''
        if not self.entries:

            if self.chipset in IPQ_CHIPSET:
                return

            # To support same rules for fuseblower - Secboot & RoT mode
            elif find_region_entry_in_datamodel(data_model, 'QFPROM_RAW_OEM_SEC_BOOT') == 'QFPROM_RAW_OEM_SEC_BOOT':
                # Just Generate FEC bits
                logger.debug2('No USER config OEM_SEC_BOOT entries to set, just generate FEC for OEM_SEC_BOOT')
                fuse_value = generate_fec(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', row=0)
                # Lastly, set FEC_EN bit
                # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
                if self.chipset in FEC_IN_ROW:
                    if int(fuse_value,16):
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='OEM_SEC_BOOT_ROW0_FEC_EN', value=0x1)
                elif self.chipset in DIFF_FEC_REG_NAZGUL:
                    # set FEC_EN region bit for Region20 (OEM_SEC_BOOT region)
                    if fuse_value: set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION20_FEC_EN', value=0x1)
                else:
                    # set FEC_EN region bit for Region24 (OEM_SEC_BOOT region)
                    if fuse_value: set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION24_FEC_EN', value=0x1)
                return

            else:
                return

        try:
            value = 0
            sec_boot1_use_serial_num = 0
            sec_boot2_use_serial_num = 0
            sec_boot3_use_serial_num = 0
            sec_boot1_rom_pk_hash_index = 0
            sec_boot2_rom_pk_hash_index = 0
            sec_boot3_rom_pk_hash_index = 0
            sec_boot1_pk_hash_in_fuse = 0
            sec_boot2_pk_hash_in_fuse = 0
            sec_boot3_pk_hash_in_fuse = 0

            for entry in self.entries:
                value = 0
                if 'use_serial_num' in entry['name']:
                    if entry['value'] == 'true': value = 1

                    if 'SEC_BOOT1' in entry['name']:
                        sec_boot1_use_serial_num = value
                    elif 'SEC_BOOT2' in entry['name']:
                        sec_boot2_use_serial_num = value
                    elif 'SEC_BOOT3' in entry['name']:
                        sec_boot3_use_serial_num = value

                elif 'rom_pk_hash_index' in entry['name']:
                    if '0x' in entry['value']:
                        value = int(entry['value'], 16)
                    else:
                        value = int(entry['value'])

                    if 'SEC_BOOT1' in entry['name']:
                        sec_boot1_rom_pk_hash_index = value
                    elif 'SEC_BOOT2' in entry['name']:
                        sec_boot2_rom_pk_hash_index = value
                    elif 'SEC_BOOT3' in entry['name']:
                        sec_boot3_rom_pk_hash_index = value

                elif 'PK_Hash_in_Fuse' in entry['name']:
                    if entry['value'] == 'true' : value = 1

                    if 'SEC_BOOT1' in entry['name']:
                        sec_boot1_pk_hash_in_fuse = value
                    elif 'SEC_BOOT2' in entry['name']:
                        sec_boot2_pk_hash_in_fuse = value
                    elif 'SEC_BOOT3' in entry['name']:
                        sec_boot3_pk_hash_in_fuse = value


            # set SEC_BOOT1
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='SEC_BOOT1_ROM_PK_Hash_Index', value=sec_boot1_rom_pk_hash_index)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='SEC_BOOT1_PK_Hash_in_Fuse', value=sec_boot1_pk_hash_in_fuse)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='SEC_BOOT1_Use_Serial_Num', value=sec_boot1_use_serial_num)

            # set SEC_BOOT2
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='SEC_BOOT2_ROM_PK_Hash_Index', value=sec_boot2_rom_pk_hash_index)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='SEC_BOOT2_PK_Hash_in_Fuse', value=sec_boot2_pk_hash_in_fuse)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='SEC_BOOT2_Use_Serial_Num', value=sec_boot2_use_serial_num)

            # set SEC_BOOT3
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='SEC_BOOT3_ROM_PK_Hash_Index', value=sec_boot3_rom_pk_hash_index)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='SEC_BOOT3_PK_Hash_in_Fuse', value=sec_boot3_pk_hash_in_fuse)
            set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='SEC_BOOT3_Use_Serial_Num', value=sec_boot3_use_serial_num)

            if self.chipset in IPQ_CHIPSET:
                return

            # Generate FEC bits
            fuse_value = generate_fec(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', row=0)

            # Lastly, set FEC_EN bit
            # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
            if self.chipset in FEC_IN_ROW:
                if int(fuse_value,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id='OEM_SEC_BOOT_ROW0_FEC_EN', value=0x1)
            elif self.chipset in DIFF_FEC_REG:
                # set FEC_EN region bit for Region11 (OEM_SEC_BOOT region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION11_FEC_EN', value=0x1)
                # In case of MDM9x07/9x06 target, blow corresponding Shadow bit as well
                if self.chipset in FEC_SHADOW:
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION11_FEC_EN_REDUNDANCY', value=0x1)
            elif self.chipset in DIFF_FEC_REG_NAZGUL:
                # set FEC_EN region bit for Region20 (OEM_SEC_BOOT region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION20_FEC_EN', value=0x1)
            else:
                # set FEC_EN region bit for Region24 (OEM_SEC_BOOT region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION24_FEC_EN', value=0x1)

        except Exception, e:
            raise RuntimeError('Error from: %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_OEM_SEC_BOOT', field_id=field_id )
        print value

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'use_serial_num':
            if int(value, 16):
                user_entry.value = 'true'
            else:
                user_entry.value = 'false'
        elif name == 'rom_pk_hash_index':
            user_entry.value = str(int(value,16))


class _SPARE_REG19(object):
    '''
    _SPARE_REG19 rule class that takes in user config and generates a fuse region object
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
        if entry['ignore'] == False:
            self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        '''
        Bits     Field Name
        31: 24   ROOT_CERT_PK_HASH_INDEX
        23: 16   SPARE0
        15: 0    PIL_SUBSYSTEM_47_32

        31:24  ROOT_CERT_PK_HASH_INDEX values
        0:             PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED
        15(0xF):       DEVICE_FIXED_TO_CERTIFICATE_15
        30(0x1E):      DEVICE_FIXED_TO_CERTIFICATE_14
        45(0x2D):      DEVICE_FIXED_TO_CERTIFICATE_13
        60(0x3C):      DEVICE_FIXED_TO_CERTIFICATE_12
        75(0x4B):      DEVICE_FIXED_TO_CERTIFICATE_11
        90(0x5A):      DEVICE_FIXED_TO_CERTIFICATE_10
        105(0x69):     DEVICE_FIXED_TO_CERTIFICATE_9
        120(0x78):     DEVICE_FIXED_TO_CERTIFICATE_8
        135(0x87):     DEVICE_FIXED_TO_CERTIFICATE_7
        150(0x96):     DEVICE_FIXED_TO_CERTIFICATE_6
        165(0xA5):     DEVICE_FIXED_TO_CERTIFICATE_5
        180(0xB4):     DEVICE_FIXED_TO_CERTIFICATE_4
        195(0xC3):     DEVICE_FIXED_TO_CERTIFICATE_3
        210(0xD2):     DEVICE_FIXED_TO_CERTIFICATE_2
        225(0xE1):     DEVICE_FIXED_TO_CERTIFICATE_1
        240(0xF0):     PRODUCTION_DEVICE_FIXED_TO_CERTIFICATE_0

        '''
        if not self.entries:
            return

        cert = defines.ROOT_CERT_PK_HASH_INDEX['PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED']

        try:
            for entry in self.entries:
                if entry['name'] == 'mrc_pk_hash_index':
                    if '0x' in entry['value']:
                        index = str(int(entry['value'], 16))
                    else:
                        index = entry['value']
                    if int(index) == 0:
                        cert = defines.ROOT_CERT_PK_HASH_INDEX['PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED']
                    else:
                        cert = defines.ROOT_CERT_PK_HASH_INDEX['DEVICE_FIXED_TO_CERTIFICATE_'+index]

            set_field_value(data_model, region_id='QFPROM_RAW_SPARE_REG19', field_id='ROOT_CERT_PK_HASH_INDEX', value=cert)

        except Exception, e:
            raise RuntimeError('Error from: %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_SPARE_REG19', field_id=field_id )
        print value

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'mrc_pk_hash_index':
            user_entry.value = str(int(value,16) & 0xF)

class _SPARE_REG18(object):
    '''
    _SPARE_REG18 rule class that takes in user config and generates a fuse region object
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
        if entry['ignore'] == False:
            self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''

        '''
        Bits     Field Name
        31: 16   WIFI COUNTRY CODE
        15: 8    WIFI BOARD ID
        7: 0     SAFE SWITCH


        '''
        if not self.entries:
            return

        try:
            for entry in self.entries:
                if entry['name'] == 'wifi_board_id':
                    if '0x' in entry['value']:
                        id = int(entry['value'], 16)
                    else:
                        id = int(entry['value'])
                    if int(id) < 0 or int(id) > 0xFF:
                        raise RuntimeError('Wifi Board ID cannot be less than 0 or greater than 0xFF')
                    else:
                        set_field_value(data_model, region_id='QFPROM_RAW_SPARE_REG18', field_id='WIFI_BOARD_ID', value=id)

        except Exception, e:
            raise RuntimeError('Error from: %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_SPARE_REG18', field_id=field_id )
        print value

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'wifi_board_id':
            user_entry.value = str(int(value,16) & 0xFF)

class _ANTI_ROLLBACK_2(object):
    '''
    _ANTI_ROLLBACK_2 rule class that takes in user config and generates a fuse region object
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
        if entry['ignore'] == False:
            self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value

        Bits     Field Name
        63:56   ROOT_CERT_PK_HASH_INDEX

        63:56  ROOT_CERT_PK_HASH_INDEX values
        0:             PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED
        15(0xF):       DEVICE_FIXED_TO_CERTIFICATE_15
        30(0x1E):      DEVICE_FIXED_TO_CERTIFICATE_14
        45(0x2D):      DEVICE_FIXED_TO_CERTIFICATE_13
        60(0x3C):      DEVICE_FIXED_TO_CERTIFICATE_12
        75(0x4B):      DEVICE_FIXED_TO_CERTIFICATE_11
        90(0x5A):      DEVICE_FIXED_TO_CERTIFICATE_10
        105(0x69):     DEVICE_FIXED_TO_CERTIFICATE_9
        120(0x78):     DEVICE_FIXED_TO_CERTIFICATE_8
        135(0x87):     DEVICE_FIXED_TO_CERTIFICATE_7
        150(0x96):     DEVICE_FIXED_TO_CERTIFICATE_6
        165(0xA5):     DEVICE_FIXED_TO_CERTIFICATE_5
        180(0xB4):     DEVICE_FIXED_TO_CERTIFICATE_4
        195(0xC3):     DEVICE_FIXED_TO_CERTIFICATE_3
        210(0xD2):     DEVICE_FIXED_TO_CERTIFICATE_2
        225(0xE1):     DEVICE_FIXED_TO_CERTIFICATE_1
        240(0xF0):     PRODUCTION_DEVICE_FIXED_TO_CERTIFICATE_0

        '''
        if not self.entries:
            return

        cert = defines.ROOT_CERT_PK_HASH_INDEX['PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED']

        try:
            for entry in self.entries:
                if entry['name'] == 'mrc_pk_hash_index':
                    if '0x' in entry['value']:
                        index = str(int(entry['value'], 16))
                    else:
                        index = entry['value']
                    if int(index) == 0:
                        cert = defines.ROOT_CERT_PK_HASH_INDEX['PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED']
                    else:
                        cert = defines.ROOT_CERT_PK_HASH_INDEX['DEVICE_FIXED_TO_CERTIFICATE_'+index]

            set_field_value(data_model, region_id='QFPROM_RAW_ANTI_ROLLBACK_2', field_id='ROOT_CERT_PK_HASH_INDEX', value=cert)

        except Exception, e:
            raise RuntimeError('Error from: %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_ANTI_ROLLBACK_2', field_id=field_id )
        print value

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'mrc_pk_hash_index':
            user_entry.value = str(int(value,16) & 0xF)


class _ANTI_ROLLBACK_3(object):
    '''
    _ANTI_ROLLBACK_3 rule class that takes in user config and generates a fuse region object
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
        if entry['ignore'] == False:
            self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value

        Bits     Field Name
        39:32   MODEM_ROOT_CERT_PK_HASH_INDEX

        39:32  MODEM_ROOT_CERT_PK_HASH_INDEX values
        0:             PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED
        15(0xF):       DEVICE_FIXED_TO_CERTIFICATE_15
        30(0x1E):      DEVICE_FIXED_TO_CERTIFICATE_14
        45(0x2D):      DEVICE_FIXED_TO_CERTIFICATE_13
        60(0x3C):      DEVICE_FIXED_TO_CERTIFICATE_12
        75(0x4B):      DEVICE_FIXED_TO_CERTIFICATE_11
        90(0x5A):      DEVICE_FIXED_TO_CERTIFICATE_10
        105(0x69):     DEVICE_FIXED_TO_CERTIFICATE_9
        120(0x78):     DEVICE_FIXED_TO_CERTIFICATE_8
        135(0x87):     DEVICE_FIXED_TO_CERTIFICATE_7
        150(0x96):     DEVICE_FIXED_TO_CERTIFICATE_6
        165(0xA5):     DEVICE_FIXED_TO_CERTIFICATE_5
        180(0xB4):     DEVICE_FIXED_TO_CERTIFICATE_4
        195(0xC3):     DEVICE_FIXED_TO_CERTIFICATE_3
        210(0xD2):     DEVICE_FIXED_TO_CERTIFICATE_2
        225(0xE1):     DEVICE_FIXED_TO_CERTIFICATE_1
        240(0xF0):     PRODUCTION_DEVICE_FIXED_TO_CERTIFICATE_0

        '''
        if not self.entries:
            return

        cert = defines.ROOT_CERT_PK_HASH_INDEX['PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED']

        try:
            for entry in self.entries:
                if entry['name'] == 'modem_mrc_pk_hash_index':
                    if '0x' in entry['value']:
                        index = str(int(entry['value'], 16))
                    else:
                        index = entry['value']
                    if int(index) == 0:
                        cert = defines.ROOT_CERT_PK_HASH_INDEX['PRODUCTION_DEVICE_NO_CERTIFICATE_SELECTED']
                    else:
                        cert = defines.ROOT_CERT_PK_HASH_INDEX['DEVICE_FIXED_TO_CERTIFICATE_'+index]

            set_field_value(data_model, region_id='QFPROM_RAW_ANTI_ROLLBACK_3', field_id='MODEM_ROOT_CERT_PK_HASH_INDEX', value=cert)

        except Exception, e:
            raise RuntimeError('Error from: %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        field_id = defines.FIELD_ID_MAP[name]

        value = get_field_value(data_model, region_id='QFPROM_RAW_ANTI_ROLLBACK_3', field_id=field_id )
        print value

        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        if name == 'modem_mrc_pk_hash_index':
            user_entry.value = str(int(value,16) & 0xF)


class _OEM_PK_HASH(object):
    '''
    _OEM_PK_HASH rule class that takes in user config and generates a fuse region object
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

        root_cert_hash = None

        try:
            for entry in self.entries:
                if entry['name'] == 'root_cert_hash':
                    root_cert_hash = entry['value']
                elif entry['name'] == 'root_cert_file':
                    root_cert_file = entry['value']
                    root_cert_hash = generate_sha256_hash_from_file(root_cert_file)
                    root_cert_hash_int = 0
                    index = 0
                    for value in root_cert_hash:
                        if index == 0:
                            root_cert_hash_int = ord(value)
                        else:
                            root_cert_hash_int = (root_cert_hash_int << 8) | ord(value)
                        index+=1

                    root_cert_hash = hex(root_cert_hash_int)[2:].rstrip('L')

            if len(root_cert_hash) > 64:
                raise RuntimeError('root_cert_hash is more than 256 bits [ 64 chars], len = '+str(len(root_cert_hash)))

            root_cert_hash = int(root_cert_hash, 16)

            if root_cert_hash == 0:
                logger.debug2('root_cert_hash is all 0, skipping')
                return

            root_cert_hash_old = hex(root_cert_hash)[2:].rstrip('L').rjust(64,'0')
            root_cert_hash = ''
            for idx in range(len(root_cert_hash_old)/2):
                root_cert_hash += root_cert_hash_old[-2:]
                root_cert_hash_old = root_cert_hash_old[:-2]
            root_cert_hash = int(root_cert_hash, 16)

            if self.chipset == '6290':
                hash_row0 = root_cert_hash & FEC_DATA_64BIT_MASK
                hash_row1 = (root_cert_hash >> 64) & FEC_DATA_64BIT_MASK
                hash_row2 = (root_cert_hash >> 128) & FEC_DATA_64BIT_MASK
                hash_row3 = (root_cert_hash >> 192) & FEC_DATA_64BIT_MASK
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW0', value=hash_row0)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW1', value=hash_row1)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW2', value=hash_row2)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW3', value=hash_row3)

            else:
                hash_row0 = root_cert_hash & FEC_DATA_56BIT_MASK
                hash_row1 = (root_cert_hash >> 56) & FEC_DATA_56BIT_MASK
                hash_row2 = (root_cert_hash >> 112) & FEC_DATA_56BIT_MASK
                hash_row3 = (root_cert_hash >> 168) & FEC_DATA_56BIT_MASK
                hash_row4 = (root_cert_hash >> 224) & FEC_DATA_32BIT_MASK  # 32 bit mask for ROW4

                # merge into data model
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW0', value=hash_row0)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW1', value=hash_row1)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW2', value=hash_row2)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW3', value=hash_row3)
                set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW4', value=hash_row4)

                # Generate FEC bits
                row0 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', row=0)
                row1 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', row=1)
                row2 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', row=2)
                row3 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', row=3)
                row4 = generate_fec(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', row=4)

                # Lastly, set FEC_EN bit
                # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
                if self.chipset in FEC_IN_ROW:
                    if int(row0, 16):
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='PK_HASH_ROW_FEC_EN0', value=0x1)
                    if int(row1, 16):
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='PK_HASH_ROW_FEC_EN1', value=0x1)
                    if int(row2, 16):
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='PK_HASH_ROW_FEC_EN2', value=0x1)
                    if int(row3, 16):
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='PK_HASH_ROW_FEC_EN3', value=0x1)
                    if int(row4, 16):
                        set_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='PK_HASH_ROW_FEC_EN4', value=0x1)

                elif self.chipset in DIFF_FEC_REG:
                    # set FEC_EN region bit for Region13 (OEM_PK_HASH region)
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION13_FEC_EN', value=0x1)
                    # In case of MDM9x07/9206 target, blow corresponding Shadow bit as well
                    if self.chipset in FEC_SHADOW:
                        set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION13_FEC_EN_REDUNDANCY', value=0x1)
                else:
                    # set FEC_EN region bit for Region23 (OEM_PK_HASH region)
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION23_FEC_EN', value=0x1)

        except Exception, e:
            raise RuntimeError('Error from configure(): %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        user_entry = None
        for entry in entries:
            if entry.name == name:
                user_entry = entry
                break

        try:
            if name == 'root_cert_hash':
                row0 = get_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW0' )
                row1 = get_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW1' )
                row2 = get_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW2' )
                row3 = get_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW3' )
                row4 = get_field_value(data_model, region_id='QFPROM_RAW_OEM_PK_HASH', field_id='HASH_DATA_ROW4' )

                root_cert_hash_temp = (int(row4, 16) << 224) +\
                                      (int(row3, 16) << 168) +\
                                      (int(row2, 16) << 112) +\
                                      (int(row1, 16) <<  56) +\
                                      int(row0, 16)

                logger.debug2('root_cert_hash from data model:\n%s' %hex(root_cert_hash_temp).rstrip('L'))
                # reverse the byte order
                root_cert_hash_old = hex(root_cert_hash_temp)[2:].rstrip('L')
                root_cert_hash = ''

                for idx in range(len(root_cert_hash_old)/2):
                    root_cert_hash += root_cert_hash_old[-2:]
                    root_cert_hash_old = root_cert_hash_old[:-2]

                root_cert_hash = root_cert_hash.rjust(64,'0')

                logger.debug2('update_entry(): root_cert_hash updated in root config:\n%s' %root_cert_hash.rstrip('L'))
                user_entry.value = root_cert_hash.rstrip('L')

        except Exception, e:
            raise RuntimeError('Error from update_entry(): %s, error=%s' + self.__class__.__name__, str(e))


class _PK_HASH0(object):
    '''
    _PK_HASH0 rule class that takes in user config and generates a fuse region object
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

        pk_hash0 = None

        try:
            for entry in self.entries:
                if entry['name'] == 'root_cert_hash0':
                    pk_hash0 = entry['value']
                elif entry['name'] == 'root_cert_hash0_file':
                    pk_hash0_file = entry['value']
                    pk_hash0 = generate_sha256_hash_from_file(pk_hash0_file)
                    pk_hash0_int = 0
                    index = 0
                    for value in pk_hash0:
                        if index == 0:
                            pk_hash0_int = ord(value)
                        else:
                            pk_hash0_int = (pk_hash0_int << 8) | ord(value)
                        index+=1

                    pk_hash0 = hex(pk_hash0_int)[2:].rstrip('L')

            if len(pk_hash0) > 64:
                raise RuntimeError('pk_hash0 is more than 256 bits [ 64 chars], len = '+str(len(pk_hash0)))

            pk_hash0 = int(pk_hash0, 16)

            if pk_hash0 == 0:
                logger.debug2('root_cert_hash is all 0, skipping')
                return

            pk_hash0_old = hex(pk_hash0)[2:].rstrip('L').rjust(64,'0')
            pk_hash0 = ''
            for idx in range(len(pk_hash0_old)/2):
                pk_hash0 += pk_hash0_old[-2:]
                pk_hash0_old = pk_hash0_old[:-2]
            pk_hash0 = int(pk_hash0, 16)

            pkhash0_row0 = pk_hash0 & FEC_DATA_64BIT_MASK
            pkhash0_row1 = (pk_hash0 >> 64) & FEC_DATA_64BIT_MASK
            pkhash0_row2 = (pk_hash0 >> 128) & FEC_DATA_64BIT_MASK
            pkhash0_row3 = (pk_hash0 >> 192) & FEC_DATA_64BIT_MASK

            # merge into data model
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH0', field_id='HASH0_DATA_ROW0', value=pkhash0_row0)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH0', field_id='HASH0_DATA_ROW1', value=pkhash0_row1)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH0', field_id='HASH0_DATA_ROW2', value=pkhash0_row2)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH0', field_id='HASH0_DATA_ROW3', value=pkhash0_row3)

        except Exception, e:
            raise RuntimeError('Error from configure(): %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        # Need to be Implemented
        return


class _PK_HASH1(object):
    '''
    _PK_HASH1 rule class that takes in user config and generates a fuse region object
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
        self.entries.append(entry)

    def configure(self, data_model):
        '''
        configures fuse/fields based on name/value
        '''
        if not self.entries:
            return

        pk_hash1 = None

        try:
            for entry in self.entries:
                if entry['name'] == 'current_sw_rot_apps':
                    current_sw_rot_apps = int(entry['value'])
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='CURRENT_SW_ROT_APPS', value=current_sw_rot_apps)
                elif entry['name'] == 'current_sw_rot_modem':
                    current_sw_rot_modem = int(entry['value'])
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='CURRENT_SW_ROT_MODEM', value=current_sw_rot_modem)
                elif entry['name'] == 'pk_hash1':
                    pk_hash1 = entry['value']
                elif entry['name'] == 'pk_hash1_file':
                    pk_hash1_file = entry['value']
                    pk_hash1 = generate_sha256_hash_from_file(pk_hash1_file)
                    pk_hash1_int = 0
                    index = 0
                    for value in pk_hash1:
                        if index == 0:
                            pk_hash1_int = ord(value)
                        else:
                            pk_hash1_int = (pk_hash1_int << 8) | ord(value)
                        index+=1

                    pk_hash1 = hex(pk_hash1_int)[2:].rstrip('L')

            if len(pk_hash1) > 64:
                raise RuntimeError('pk_hash1 is more than 256 bits [ 64 chars], len = '+str(len(pk_hash1)))

            pk_hash1 = int(pk_hash1, 16)

            if pk_hash1 == 0:
                logger.debug2('root_cert_hash is all 0, skipping')
                row4 = generate_fec(data_model, region_id='QFPROM_RAW_PK_HASH1', row=4)
                if self.chipset in FEC_IN_ROW:
                    if int(row4, 16):
                        set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='PKHASH1_ROW_FEC_EN4', value=0x1)

                elif self.chipset in DIFF_FEC_REG_NAZGUL:
                    # set FEC_EN region bit for Region25 (PK_HASH1 region)
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION25_FEC_EN', value=0x1)

                else:
                    # set FEC_EN region bit for Region30 (PK_HASH1 region)
                    set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION30_FEC_EN', value=0x1)
                return

            pk_hash1_old = hex(pk_hash1)[2:].rstrip('L').rjust(64,'0')
            pk_hash1= ''
            for idx in range(len(pk_hash1_old)/2):
                pk_hash1 += pk_hash1_old[-2:]
                pk_hash1_old = pk_hash1_old[:-2]
            pk_hash1 = int(pk_hash1, 16)

            pkhash1_row0 = pk_hash1 & FEC_DATA_56BIT_MASK
            pkhash1_row1 = (pk_hash1 >> 56) & FEC_DATA_56BIT_MASK
            pkhash1_row2 = (pk_hash1 >> 112) & FEC_DATA_56BIT_MASK
            pkhash1_row3 = (pk_hash1 >> 168) & FEC_DATA_56BIT_MASK
            pkhash1_row4 = (pk_hash1 >> 224) & FEC_DATA_32BIT_MASK  # 32 bit mask for ROW4

            # merge into data model
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='HASH1_DATA_ROW0', value=pkhash1_row0)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='HASH1_DATA_ROW1', value=pkhash1_row1)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='HASH1_DATA_ROW2', value=pkhash1_row2)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='HASH1_DATA_ROW3', value=pkhash1_row3)
            set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='HASH1_DATA_ROW4', value=pkhash1_row4)

            # Generate FEC bits
            row0 = generate_fec(data_model, region_id='QFPROM_RAW_PK_HASH1', row=0)
            row1 = generate_fec(data_model, region_id='QFPROM_RAW_PK_HASH1', row=1)
            row2 = generate_fec(data_model, region_id='QFPROM_RAW_PK_HASH1', row=2)
            row3 = generate_fec(data_model, region_id='QFPROM_RAW_PK_HASH1', row=3)
            row4 = generate_fec(data_model, region_id='QFPROM_RAW_PK_HASH1', row=4)

            # Lastly, set FEC_EN bit
            # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
            if self.chipset in FEC_IN_ROW:
                if int(row0,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='PKHASH1_ROW_FEC_EN0', value=0x1)
                if int(row1,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='PKHASH1_ROW_FEC_EN1', value=0x1)
                if int(row2,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='PKHASH1_ROW_FEC_EN2', value=0x1)
                if int(row3,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='PKHASH1_ROW_FEC_EN3', value=0x1)
                if int(row4,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_PK_HASH1', field_id='PKHASH1_ROW_FEC_EN4', value=0x1)

            elif self.chipset in DIFF_FEC_REG_NAZGUL:
                # set FEC_EN region bit for Region25 (PK_HASH1 region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION25_FEC_EN', value=0x1)

            else:
                # set FEC_EN region bit for Region30 (PK_HASH1 region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION30_FEC_EN', value=0x1)

        except Exception, e:
            raise RuntimeError('Error from configure(): %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        # Need to be Implemented
        return


class _IMAGE_ENCR_KEY1(object):
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

        image_encr_key1 = None

        try:
            for entry in self.entries:
                if entry['name'] == 'image_encr_key1':
                    image_encr_key1 = entry['value']
                elif entry['name'] == 'image_encr_key1_file':
                    image_encr_key1_file = entry['value']
                    image_encr_key1 = get_keydata_from_file(image_encr_key1_file)

            if len(image_encr_key1) > 32:
                raise RuntimeError('image_encr_key1 is more than 128 bits [ 32 chars], len = '+str(len(image_encr_key1)))

            image_encr_key1 = int(image_encr_key1, 16)

            if image_encr_key1 == 0:
                logger.debug2('image_encr_key1 is all 0, skipping')
                return

            image_encr_key1_old = hex(image_encr_key1)[2:].rstrip('L').rjust(32,'0')
            image_encr_key1 = ''
            for idx in range(len(image_encr_key1_old)/2):
                image_encr_key1 += image_encr_key1_old[-2:]
                image_encr_key1_old = image_encr_key1_old[:-2]
            image_encr_key1 = int(image_encr_key1, 16)

            imageencrkey1_row0 = image_encr_key1 & FEC_DATA_56BIT_MASK
            imageencrkey1_row1 = (image_encr_key1 >> 56) & FEC_DATA_56BIT_MASK
            imageencrkey1_row2 = (image_encr_key1 >> 112) & 0xFFFF

            # merge into data model
            set_field_value(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', field_id='KEY_DATA_ROW0', value=imageencrkey1_row0)
            set_field_value(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', field_id='KEY_DATA_ROW1', value=imageencrkey1_row1)
            set_field_value(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', field_id='KEY_DATA_ROW2', value=imageencrkey1_row2)

            # Generate FEC bits
            row0 = generate_fec(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', row=0)
            row1 = generate_fec(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', row=1)
            row2 = generate_fec(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', row=2)

            # Lastly, set FEC_EN bit
            # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
            if self.chipset in FEC_IN_ROW:
                if int(row0,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', field_id='IMAGEENCRKEY1_ROW0_FEC_EN0', value=0x1)
                if int(row1,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', field_id='IMAGEENCRKEY1_ROW1_FEC_EN1', value=0x1)
                if int(row2,16):
                    set_field_value(data_model, region_id='QFPROM_RAW_IMAGE_ENCR_KEY1', field_id='IMAGEENCRKEY1_ROW2_FEC_EN2', value=0x1)

            elif self.chipset in DIFF_FEC_REG_NAZGUL:
                # set FEC_EN region bit for Region24 (IMAGE_ENCR_KEY1 region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION24_FEC_EN', value=0x1)

            else:
                # set FEC_EN region bit for Region29 (IMAGE_ENCR_KEY1 region)
                set_field_value(data_model, region_id='QFPROM_RAW_FEC_EN', field_id='REGION29_FEC_EN', value=0x1)

        except Exception, e:
            raise RuntimeError('Error from configure(): %s, error=%s' + self.__class__.__name__, str(e))

    def update_entry(self, data_model, entries, name):
        '''
        retrieves the field value from data_model and update the relevant config entries
        '''
        # Need to be Implemented
        return


class _OEM_PKHASH(object):
    '''
    _OEM_PKHASH rule class that takes in user config and generates a fuse region object
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

        root_certhash = None

        try:
            for entry in self.entries:
                if entry['name'] == 'root_certhash':
                    root_certhash = entry['value']
                elif entry['name'] == 'root_certfile':
                    root_certfile = entry['value']
                    root_certhash = generate_sha256_hash_from_file(root_certfile)
                    root_certhash_int = 0
                    index = 0
                    for value in root_certhash:
                        if index == 0:
                            root_certhash_int = ord(value)
                        else:
                            root_certhash_int = (root_certhash_int << 8) | ord(value)
                        index+=1

                    root_certhash = hex(root_certhash_int)[2:].rstrip('L')

            if len(root_certhash) > 64:
                raise RuntimeError('root_certhash is more than 256 bits [ 64 chars], len = '+str(len(root_certhash)))

            root_certhash = int(root_certhash, 16)

            if root_certhash == 0:
                logger.debug2('root_certhash is all 0, skipping')
                return

            root_certhash_old = hex(root_certhash)[2:].rstrip('L').rjust(64,'0')
            root_certhash = ''
            for idx in range(len(root_certhash_old)/2):
                root_certhash += root_certhash_old[-2:]
                root_certhash_old = root_certhash_old[:-2]
            root_certhash = int(root_certhash, 16)

            hash_data0_row0 = root_certhash & 0x3FF
            hash_data1_row0 = (root_certhash >> 10) & 0x3FF
            hash_data2_row0 = (root_certhash >> 20) & 0x3
            hash_data3_row0 = (root_certhash >> 22) & 0xFF
            hash_data4_row0 = (root_certhash >> 30) & 0x3FF

            hash_data0_row1 = (root_certhash >> 40) & 0x3FF
            hash_data1_row1 = (root_certhash >> 50) & 0x3FF
            hash_data2_row1 = (root_certhash >> 60) & 0x3
            hash_data3_row1 = (root_certhash >> 62) & 0xFF
            hash_data4_row1 = (root_certhash >> 70) & 0x3FF

            hash_data0_row2 = (root_certhash >> 80) & 0x3FF
            hash_data1_row2 = (root_certhash >> 90) & 0x3FF
            hash_data2_row2 = (root_certhash >> 100) & 0x3
            hash_data3_row2 = (root_certhash >> 102) & 0xFF
            hash_data4_row2 = (root_certhash >> 110) & 0x3FF

            hash_data0_row3 = (root_certhash >> 120) & 0x3FF
            hash_data1_row3 = (root_certhash >> 130) & 0x3FF
            hash_data2_row3 = (root_certhash >> 140) & 0x3
            hash_data3_row3 = (root_certhash >> 142) & 0xFF
            hash_data4_row3 = (root_certhash >> 150) & 0x3FF

            hash_data0_row4 = (root_certhash >> 160) & 0x3FF
            hash_data1_row4 = (root_certhash >> 170) & 0x3FF
            hash_data2_row4 = (root_certhash >> 180) & 0x3
            hash_data3_row4 = (root_certhash >> 182) & 0xFF
            hash_data4_row4 = (root_certhash >> 190) & 0x3FF

            hash_data0_row5 = (root_certhash >> 200) & 0x3FF
            hash_data1_row5 = (root_certhash >> 210) & 0x3FF
            hash_data2_row5 = (root_certhash >> 220) & 0x3
            hash_data3_row5 = (root_certhash >> 222) & 0xFF
            hash_data4_row5 = (root_certhash >> 230) & 0x3FF

            hash_data0_row6 = (root_certhash >> 240) & 0x3FF
            hash_data1_row6 = (root_certhash >> 250) & 0x3F

            # merge into data model
            region_id = 'QFPROM_RAW_OEM_PKHASH'
            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW0', value=hash_data0_row0)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW0', value=hash_data1_row0)
            set_field_value(data_model, region_id, field_id='HASH_DATA2_ROW0', value=hash_data2_row0)
            set_field_value(data_model, region_id, field_id='HASH_DATA3_ROW0', value=hash_data3_row0)
            set_field_value(data_model, region_id, field_id='HASH_DATA4_ROW0', value=hash_data4_row0)

            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW1', value=hash_data0_row1)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW1', value=hash_data1_row1)
            set_field_value(data_model, region_id, field_id='HASH_DATA2_ROW1', value=hash_data2_row1)
            set_field_value(data_model, region_id, field_id='HASH_DATA3_ROW1', value=hash_data3_row1)
            set_field_value(data_model, region_id, field_id='HASH_DATA4_ROW1', value=hash_data4_row1)

            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW2', value=hash_data0_row2)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW2', value=hash_data1_row2)
            set_field_value(data_model, region_id, field_id='HASH_DATA2_ROW2', value=hash_data2_row2)
            set_field_value(data_model, region_id, field_id='HASH_DATA3_ROW2', value=hash_data3_row2)
            set_field_value(data_model, region_id, field_id='HASH_DATA4_ROW2', value=hash_data4_row2)

            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW3', value=hash_data0_row3)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW3', value=hash_data1_row3)
            set_field_value(data_model, region_id, field_id='HASH_DATA2_ROW3', value=hash_data2_row3)
            set_field_value(data_model, region_id, field_id='HASH_DATA3_ROW3', value=hash_data3_row3)
            set_field_value(data_model, region_id, field_id='HASH_DATA4_ROW3', value=hash_data4_row3)

            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW4', value=hash_data0_row4)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW4', value=hash_data1_row4)
            set_field_value(data_model, region_id, field_id='HASH_DATA2_ROW4', value=hash_data2_row4)
            set_field_value(data_model, region_id, field_id='HASH_DATA3_ROW4', value=hash_data3_row4)
            set_field_value(data_model, region_id, field_id='HASH_DATA4_ROW4', value=hash_data4_row4)

            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW5', value=hash_data0_row5)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW5', value=hash_data1_row5)
            set_field_value(data_model, region_id, field_id='HASH_DATA2_ROW5', value=hash_data2_row5)
            set_field_value(data_model, region_id, field_id='HASH_DATA3_ROW5', value=hash_data3_row5)
            set_field_value(data_model, region_id, field_id='HASH_DATA4_ROW5', value=hash_data4_row5)

            set_field_value(data_model, region_id, field_id='HASH_DATA0_ROW6', value=hash_data0_row6)
            set_field_value(data_model, region_id, field_id='HASH_DATA1_ROW6', value=hash_data1_row6)

            # Generate FEC bits
            data0_row0 = update_fec_15_10(data_model, hash_data0_row0, region_id, 'FEC_VALUE0_ROW0', 0)
            data1_row0 = update_fec_15_10(data_model, hash_data1_row0, region_id, 'FEC_VALUE1_ROW0', 0)
            data2_row0 = update_fec_15_10(data_model, ((0x3FF & hash_data3_row0) << 2)| hash_data2_row0, region_id, 'FEC_VALUE2_ROW0', 0)
            data3_row0 = update_fec_15_10(data_model, hash_data4_row0, region_id, 'FEC_VALUE3_ROW0', 0)

            data0_row1 = update_fec_15_10(data_model, hash_data0_row1, region_id, 'FEC_VALUE0_ROW1', 1)
            data1_row1 = update_fec_15_10(data_model, hash_data1_row1, region_id, 'FEC_VALUE1_ROW1', 1)
            data2_row1 = update_fec_15_10(data_model, ((0x3FF & hash_data3_row1) <<2) | hash_data2_row1, region_id, 'FEC_VALUE2_ROW1', 1)
            data3_row1 = update_fec_15_10(data_model, hash_data4_row1, region_id, 'FEC_VALUE3_ROW1', 1)

            data0_row2 = update_fec_15_10(data_model, hash_data0_row2, region_id,'FEC_VALUE0_ROW2', 2)
            data1_row2 = update_fec_15_10(data_model, hash_data1_row2, region_id,'FEC_VALUE1_ROW2', 2)
            data2_row2 = update_fec_15_10(data_model, ((0x3FF & hash_data3_row2) <<2) | hash_data2_row2, region_id,'FEC_VALUE2_ROW2', 2)
            data3_row2 = update_fec_15_10(data_model, hash_data4_row2, region_id,'FEC_VALUE3_ROW2', 2)

            data0_row3 = update_fec_15_10(data_model, hash_data0_row3, region_id,'FEC_VALUE0_ROW3', 3)
            data1_row3 = update_fec_15_10(data_model, hash_data1_row3, region_id,'FEC_VALUE1_ROW3', 3)
            data2_row3 = update_fec_15_10(data_model, ((0x3FF & hash_data3_row3) <<2) | hash_data2_row3, region_id,'FEC_VALUE2_ROW3', 3)
            data3_row3 = update_fec_15_10(data_model, hash_data4_row3, region_id,'FEC_VALUE3_ROW3', 3)

            data0_row4 = update_fec_15_10(data_model, hash_data0_row4, region_id,'FEC_VALUE0_ROW4', 4)
            data1_row4 = update_fec_15_10(data_model, hash_data1_row4, region_id,'FEC_VALUE1_ROW4', 4)
            data2_row4 = update_fec_15_10(data_model, ((0x3FF & hash_data3_row4) <<2) | hash_data2_row4, region_id,'FEC_VALUE2_ROW4', 4)
            data3_row4 = update_fec_15_10(data_model, hash_data4_row4, region_id,'FEC_VALUE3_ROW4', 4)

            data0_row5 = update_fec_15_10(data_model, hash_data0_row5, region_id,'FEC_VALUE0_ROW5', 5)
            data1_row5 = update_fec_15_10(data_model, hash_data1_row5, region_id,'FEC_VALUE1_ROW5', 5)
            data2_row5 = update_fec_15_10(data_model, ((0x3FF & hash_data3_row5) <<2) | hash_data2_row5, region_id,'FEC_VALUE2_ROW5', 5)
            data3_row5 = update_fec_15_10(data_model, hash_data4_row5, region_id,'FEC_VALUE3_ROW5', 5)

            data0_row6 = update_fec_15_10(data_model, hash_data0_row6, region_id,'FEC_VALUE0_ROW6', 6)
            data1_row6 = update_fec_15_10(data_model, hash_data1_row6, region_id,'FEC_VALUE1_ROW6', 6)

            # Lastly, set FEC_EN bit
            # some chipsets have the FEC_EN bit in the row instead of having FEC_EN region
            if self.chipset in IPQ_CHIPSET:
                if int(data0_row0,16) or int(data1_row0,16) or int(data2_row0,16) or int(data3_row0,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW0', value=0x7)
                if int(data0_row1,16) or int(data1_row1,16) or int(data2_row1,16) or int(data3_row1,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW1', value=0x7)
                if int(data0_row2,16) or int(data1_row2,16) or int(data2_row2,16) or int(data3_row2,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW2', value=0x7)
                if int(data0_row3,16) or int(data1_row3,16) or int(data2_row3,16) or int(data3_row3,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW3', value=0x7)
                if int(data0_row4,16) or int(data1_row4,16) or int(data2_row4,16) or int(data3_row4,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW4', value=0x7)
                if int(data0_row5,16) or int(data1_row5,16) or int(data2_row5,16) or int(data3_row5,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW5', value=0x7)
                if int(data0_row6,16) or int(data1_row6,16):
                    set_field_value(data_model, region_id, field_id='FEC_EN_ROW6', value=0x7)

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
        self.regions['QFPROM_RAW_MRC_2_0'] = _MRC_2_0(chipset)
        self.regions['QFPROM_RAW_OEM_SECURE'] = _OEM_SECURE(chipset)
        self.regions['QFPROM_RAW_OEM_CONFIG'] = _OEM_CONFIG(chipset)
        self.regions['QFPROM_RAW_SPARE_REG19'] = _SPARE_REG19(chipset)
        self.regions['QFPROM_RAW_OEM_PK_HASH'] = _OEM_PK_HASH(chipset)
        self.regions['QFPROM_RAW_OEM_SEC_BOOT'] = _OEM_SEC_BOOT(chipset)
        self.regions['QFPROM_RAW_ANTI_ROLLBACK_2'] = _ANTI_ROLLBACK_2(chipset)
        self.regions['QFPROM_RAW_ANTI_ROLLBACK_3'] = _ANTI_ROLLBACK_3(chipset)
        self.regions['QFPROM_RAW_PK_HASH0'] = _PK_HASH0(chipset)
        self.regions['QFPROM_RAW_PK_HASH1'] = _PK_HASH1(chipset)
        self.regions['QFPROM_RAW_IMAGE_ENCR_KEY1'] = _IMAGE_ENCR_KEY1(chipset)
        self.regions['QFPROM_RAW_OEM_PKHASH'] = _OEM_PKHASH(chipset)
        self.regions['QFPROM_RAW_SPARE_REG18'] = _SPARE_REG18(chipset)
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

