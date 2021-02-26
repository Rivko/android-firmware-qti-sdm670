# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""Contains constants related to the cfgparser package.

Constants relate to:

#. Common constants
#. Auto-generated FB XML Parser - auto_gen_xml_config
"""


from sectools.features.fuse_common.datamodel import defines as dm_def


#------------------------------------------------------------------------------
# Config Data Model Structure
#------------------------------------------------------------------------------
OBJ_STRUCTURE = \
{
    'metadata' :
    {
        'chipset' : '',
        'version' : '',
    },

    'module' : dm_def.OBJ_STRUCTURE['module'],
}


ENTRIES_STRUCTURE = {
                        'metadata' :
                        {
                            'chipset' : '',
                            'version' : '',
                        },

                        'secdat' :
                        {
                            'file_info' : '',
                            'file_version' : (0,),
                            'fuse_version' : (0,),
                        },

                        'module' :
                        {
                            'entry':
                            [
                                {
                                    'description':'',
                                    'name' : '',
                                    'value' : '',
                                    'ignore' : (True,),
                                }
                            ]
                        }
                    }

# FEC bits are 7 bits long, mask 0x7F : 0111 1111
FEC_BIT_MASK=0x7F

# FEC DATA region bit mask, 32 lower bits, mask 0xFFFFFFFF
FEC_DATA_32BIT_MASK=0xFFFFFFFF

# FEC DATA region bit mask, 56 lower bits, mask 0xFFFFFFFFFFFFFF
FEC_DATA_56BIT_MASK=0xFFFFFFFFFFFFFF

# FEC DATA region bit mask, 64 lower bits, mask 0xFFFFFFFFFFFFFFFF
FEC_DATA_64BIT_MASK=0xFFFFFFFFFFFFFFFF

# FEC bits are 5 bits long, mask 0x1F : 0001 1111
FEC_15_10_BIT_MASK=0x1F

# chipsets that have FEC_EN bit in row instead of FEC_EN region
FEC_IN_ROW = ['8016','8916','8039','8939','8909','8929','8952','401x','8909W', '8905', '8009lat']

# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region & different from
# other 8084, 8994 chipsets and so on.
# These chipsets have:
# OEM_SEC_BOOT FEC bit in region11
# OEM_PK_HASH FEC bit in region13
DIFF_FEC_REG = ['8976','8937','8953','8076','8940','9607','8917','8920','807x','9206', 'sdm450', '9x50', '9206tx', '8053lat']

# Same as DIFF_FEC_REG where additionally shadow region also needs to be set
FEC_SHADOW = ['9607', '9206', '9206tx']

# For Fuseblower & Keyprovision: chipsets that have FEC_EN bit in FEC_EN region and match that of Nazgul. This includes
# Nazgul (8998), Starlord (sdm660) and Starhawk (sdm630) and so on.
# These chipsets have:
# OEM_SEC_BOOT FEC bit in region20
# PK_HASH1 FEC bit in region25
# IMAGE_ENCR_KEY1 FEC bit in region21
DIFF_FEC_REG_NAZGUL = ['8998', 'sdm660', 'sdm630', 'sdm845', 'sdm670', '8998-sdx50m']

# For Keyprovision: chipsets that have FEC_EN bit in FEC_EN region & different from other 8976, 8937 and so on.
KP_FEC_REG = ['8953','8937','8940','8917','8920','sdm450', '8053lat']

# chipsets that support ROT feature
ROT_SUPPORTED = ['8992','8994','8996','8998', '8998-sdx50m']

IPQ_CHIPSET = ['806x']

# MRC 2.0 supported chipsets
MRC_2_0_CHIPSETS = ['sdm845', 'sdm670']




