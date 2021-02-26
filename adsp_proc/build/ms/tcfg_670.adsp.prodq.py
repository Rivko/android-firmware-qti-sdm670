#==============================================================================
# Target Build ID Config Script.
#
# Copyright (c) 2014 QUALCOMM Technologies Inc.  All Rights Reserved
#==============================================================================

def exists(env):
    return env.Detect('tcfg_670.adsp.prodQ_data')

def generate(env):
    # Save the tcfg_bid_data in the env
    env['TCFG_BID_IMAGE'] = 'MODEM_PROC'

    env.AddUsesFlags('USES_ADSPPM_INTEGRATION_ENABLED', from_builds_file = True)
    env.AddUsesFlags('USES_BLAST', from_builds_file = True)
    env.AddUsesFlags('USES_CHRE', from_builds_file = True)
    env.AddUsesFlags('USES_DAL', from_builds_file = True)
    env.AddUsesFlags('USES_DEVCFG_MULTIPLE_CONFIG', from_builds_file = True)
    env.AddUsesFlags('USES_DISABLE_WLAN', from_builds_file = True)
    env.AddUsesFlags('USES_DISPLAY_MEMORY_FOOTPRINT', from_builds_file = True)
    env.AddUsesFlags('USES_DOG_STUBS', from_builds_file = True)
    env.AddUsesFlags('USES_EFS_QMI_PUT_GET_CLIENT', from_builds_file = True)
    env.AddUsesFlags('USES_ERR_INJECT_CRASH', from_builds_file = True)
    env.AddUsesFlags('USES_FEATURE_DYNAMIC_LOADING_GLOBAL', from_builds_file = True)
    env.AddUsesFlags('USES_FEATURE_DYNAMIC_LOADING_SENSORS', from_builds_file = True)
    env.AddUsesFlags('USES_FEATURE_QSHRINK_ENABLED', from_builds_file = True)
    env.AddUsesFlags('USES_ISLAND_FEATURE', from_builds_file = True)
    env.AddUsesFlags('USES_MBNTOOLS', from_builds_file = True)
    env.AddUsesFlags('USES_NO_STRIP_NO_ODM', from_builds_file = True)
    env.AddUsesFlags('USES_PLATFORM_IMAGE_INTEGRITY_CHECK', from_builds_file = True)
    env.AddUsesFlags('USES_QURT', from_builds_file = True)
    env.AddUsesFlags('USES_RELOC_ENABLE_AND_ALIGN', from_builds_file = True)
    env.AddUsesFlags('USES_REMOTE_HEAP', from_builds_file = True)
    env.AddUsesFlags('USES_RFS_USE_TFTP', from_builds_file = True)
    env.AddUsesFlags('USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN', from_builds_file = True)
    env.AddUsesFlags('USES_SLPI_ADSP_IMG', from_builds_file = True)
    env.AddUsesFlags('USES_SSC_SDC_IMG', from_builds_file = True)
    env.AddUsesFlags('USES_TFTP_CLIENT', from_builds_file = True)
    env.AddUsesFlags('USES_TFTP_CLIENT_DEBUG_FLAGS', from_builds_file = True)
    env.AddUsesFlags('USES_TOUCH_INTEGRATION_ENABLED', from_builds_file = True)
    env.AddUsesFlags('USES_UNIFIED_DEVCFG', from_builds_file = True)
