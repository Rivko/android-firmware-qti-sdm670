#******************************************************************************
#  @file    qcril_header_parser.py
#  @brief   Parses ril.h and generates qcril_feature_def.h
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
#  Qualcomm Technologies Proprietary and Confidential.
#  ---------------------------------------------------------------------------
#******************************************************************************/

import sys
import string
import os.path
import re

out_header_dir = string.lstrip(str(sys.argv[2]))
dir_exist = os.path.isdir(out_header_dir)
if not dir_exist:
    os.makedirs(out_header_dir)

out_generated = os.path.isfile(out_header_dir + '/qcril_features_def.h');

if out_generated:
    out_header = open(out_header_dir + '/qcril_features_def.h','r+');
else:
    out_header = open(out_header_dir + '/qcril_features_def.h','w+');

ril_header = open(string.lstrip(str(sys.argv[1])) + '/hardware/ril/include/telephony/ril.h', 'r');
out_last_line = '#endif /* QCRIL_FEATURES_DEF_H */'
is_file_updated = 0;
max_req_id_str   = '#define QCRIL_FEATURES_DEF_MAX_REQUEST_ID '
max_unsol_id_str = '#define QCRIL_FEATURES_DEF_MAX_UNSOL_ID   '
max_err_id_str   = '#define QCRIL_FEATURES_DEF_MAX_ERROR_ID   '

# buffer to hold all definitions
feature_def_buffer = '\n'

#==================================================================================
# Helper functions
#==================================================================================

def check_for_symbol(file_fd, symbol):
    cur_pos = file_fd.tell();
    file_fd.seek(0);
    exist = False;
    for line in file_fd:
        if symbol in line:
            exist = True;
            file_fd.seek(cur_pos);
            return exist;

    file_fd.seek(cur_pos);
    return exist;

def check_for_symbol_ril_header(symbol):
    return check_for_symbol(ril_header, symbol);

def check_for_symbol_out_header(symbol):
    return check_for_symbol(out_header, symbol);

def append_feature_def_to_file(line):
    out_header.write(line + '\n');

def append_feature_def_to_buffer(line):
    global feature_def_buffer
    feature_def_buffer += line + '\n'

def get_max_request_id_from_out_header():
    cur_pos = out_header.tell()
    out_header.seek(0);
    found = False
    max_id = 0
    for line in out_header:
        req_id = re.findall(r'^\s*#define\s+QCRIL_FEATURES_DEF_MAX_REQUEST_ID\s+(\d*)', line)
        if req_id:
            max_id = int (req_id[0]);
            found = True
            break;
    out_header.seek(cur_pos)
    return max_id if found else None;

def get_max_unsol_id_from_out_header():
    cur_pos = out_header.tell()
    out_header.seek(0)
    found = False
    max_id = 0
    for line in out_header:
        unsol_id = re.findall(r'^\s*#define\s+QCRIL_FEATURES_DEF_MAX_UNSOL_ID\s+(\d*)', line)
        if unsol_id:
            max_id = int (unsol_id[0])
            found = True
            break;
    out_header.seek(cur_pos)
    return max_id if found else None;

def get_max_error_id_from_out_header():
    cur_pos = out_header.tell()
    out_header.seek(0)
    found = False
    max_error = 0
    for line in out_header:
        err_id = re.findall(r'^\s*#\s*define\s+QCRIL_FEATURES_DEF_MAX_ERROR_ID\s+(\d*)', line)
        if err_id:
            max_error = int(err_id[0])
            found = True
    out_header.seek(cur_pos)
    return max_error if found else None;

def get_max_request_id_from_ril_header():
    ril_header.seek(0)
    max_req = 0
    for line in ril_header:
        req_id = re.findall(r'^\s*#define\s+RIL_REQUEST_\w+\s+(\d*)', line)
        if req_id and (max_req < int(req_id[0])):
            max_req = int(req_id[0])
    return max_req;

def get_max_error_id_from_ril_header():
    ril_header.seek(0)
    max_error = 0
    for line in ril_header:
        err_id = re.findall(r'^\s*RIL_E_\w+\s*=\s*([0-9a-fA-F]*).*', line)
        if err_id and (max_error < int(err_id[0])):
            max_error = int(err_id[0])
    return max_error;

def get_max_unsol_id_from_ril_header():
    ril_header.seek(0)
    max_unsol = 0
    for line in ril_header:
        unsol_id = re.findall(r'^\s*#define\s+RIL_UNSOL_\w+\s+(\d*)', line)
        if unsol_id and (max_unsol < int(unsol_id[0])):
            max_unsol = int(unsol_id[0])
    return max_unsol;

#==================================================================================
# Main Routine
#==================================================================================

# feature definition for extended error call cause
if check_for_symbol_ril_header('CALL_FAIL_INVALID_TRANSIT_NW_SELECTION'):
    append_feature_def_to_buffer('#define EXTENDED_FAIL_ERROR_CAUSE_FOR_VOICE_CALL (1)')
    if not check_for_symbol_out_header('EXTENDED_FAIL_ERROR_CAUSE_FOR_VOICE_CALL'):
        is_file_updated = 1;

if not check_for_symbol_ril_header('RADIO_TECH_LTE_CA'):
    append_feature_def_to_buffer('#define NOT_SUPPORTED_LTE_CA (1)')
    append_feature_def_to_buffer('#define RADIO_TECH_LTE_CA (RADIO_TECH_LTE)')
    if not check_for_symbol_out_header('RADIO_TECH_LTE_CA'):
        is_file_updated = 1;

# Error codes for HOLD_RESUME
if not check_for_symbol_ril_header('RIL_E_HOLD_RESUME_FAILED'):
    append_feature_def_to_buffer('#define HOLD_RESUME_ERROR_CODES (1)')
    if not check_for_symbol_out_header('HOLD_RESUME_ERROR_CODES'):
        is_file_updated = 1;

# MAX REQ ID
out_hdr_max_req = get_max_request_id_from_out_header()
ril_hdr_max_req = get_max_request_id_from_ril_header()
append_feature_def_to_buffer(max_req_id_str + "%8d" % ril_hdr_max_req)
if out_hdr_max_req == None or\
        out_hdr_max_req != ril_hdr_max_req:
    is_file_updated = 1

# MAX UNSOL ID
out_hdr_max_unsol = get_max_unsol_id_from_out_header()
ril_hdr_max_unsol = get_max_unsol_id_from_ril_header()
append_feature_def_to_buffer(max_unsol_id_str + "%8d" % ril_hdr_max_unsol)
if out_hdr_max_unsol == None or\
        out_hdr_max_unsol != ril_hdr_max_unsol:
    is_file_updated = 1

# MAX ERROR ID
out_hdr_max_error = get_max_error_id_from_out_header()
ril_hdr_max_error = get_max_error_id_from_ril_header()
append_feature_def_to_buffer(max_err_id_str + "%8d" % ril_hdr_max_error)
if out_hdr_max_error == None or\
        out_hdr_max_error != ril_hdr_max_error:
    is_file_updated = 1

if not check_for_symbol_ril_header('RIL_REQUEST_GET_ADN_RECORD'):
    append_feature_def_to_buffer('#define RIL_REQUEST_GET_ADN_RECORD (300)')
    if not check_for_symbol_out_header('RIL_REQUEST_GET_ADN_RECORD'):
        is_file_updated = 1;

if not check_for_symbol_ril_header('RIL_REQUEST_UPDATE_ADN_RECORD'):
    append_feature_def_to_buffer('#define RIL_REQUEST_UPDATE_ADN_RECORD (301)')
    if not check_for_symbol_out_header('RIL_REQUEST_UPDATE_ADN_RECORD'):
        is_file_updated = 1;

if not check_for_symbol_ril_header('RIL_UNSOL_RESPONSE_ADN_INIT_DONE'):
    append_feature_def_to_buffer('#define RIL_UNSOL_RESPONSE_ADN_INIT_DONE (1046)')
    if not check_for_symbol_out_header('RIL_UNSOL_RESPONSE_ADN_INIT_DONE'):
        is_file_updated = 1;

if not check_for_symbol_ril_header('RIL_UNSOL_RESPONSE_ADN_RECORDS'):
    append_feature_def_to_buffer('#define RIL_UNSOL_RESPONSE_ADN_RECORDS (1047)')
    if not check_for_symbol_out_header('RIL_UNSOL_RESPONSE_ADN_RECORDS'):
        is_file_updated = 1;

if not check_for_symbol_ril_header('RIL_NUM_ADN_RECORDS'):
    append_feature_def_to_buffer('#define RIL_NUM_ADN_RECORDS (10)')
    if not check_for_symbol_out_header('RIL_NUM_ADN_RECORDS'):
        is_file_updated = 1;

if not check_for_symbol_ril_header('RIL_MAX_NUM_AD_COUNT'):
    append_feature_def_to_buffer('#define RIL_MAX_NUM_AD_COUNT (4)')
    if not check_for_symbol_out_header('RIL_MAX_NUM_AD_COUNT'):
        is_file_updated = 1;

if not check_for_symbol_ril_header('RIL_MAX_NUM_EMAIL_COUNT'):
    append_feature_def_to_buffer('#define RIL_MAX_NUM_EMAIL_COUNT (2)')
    if not check_for_symbol_out_header('RIL_MAX_NUM_EMAIL_COUNT'):
        is_file_updated = 1;

if not check_for_symbol_ril_header('RIL_AdnRecordInfo'):
    append_feature_def_to_buffer('typedef struct {\
                                  \n    int       record_id;\
                                  \n    char*     name;\
                                  \n    char*     number;\
                                  \n    int       email_elements;\
                                  \n    char*     email[RIL_MAX_NUM_EMAIL_COUNT];\
                                  \n    int       anr_elements;\
                                  \n    char*     ad_number[RIL_MAX_NUM_AD_COUNT];\
                                  \n} RIL_AdnRecordInfo;')
    if not check_for_symbol_out_header('RIL_AdnRecordInfo'):
        is_file_updated = 1;
elif check_for_symbol_ril_header('RIL_AdnRecordInfo'):
    if check_for_symbol_out_header('RIL_AdnRecordInfo'):
        is_file_updated = 1;

if not check_for_symbol_ril_header('RIL_AdnRecord_v1'):
    print "Not found RIL_AdnRecord_v1 RILH"
    append_feature_def_to_buffer('typedef struct {\
                                  \n    int               record_elements;\
                                  \n    RIL_AdnRecordInfo adn_record_info[RIL_NUM_ADN_RECORDS];\
                                  \n} RIL_AdnRecord_v1;')
    if not check_for_symbol_out_header('RIL_AdnRecord_v1'):
        print "Not found RIL_AdnRecord_v1 OUTH"
        is_file_updated = 1;
elif check_for_symbol_ril_header('RIL_AdnRecord_v1'):
    if check_for_symbol_out_header('RIL_AdnRecord_v1'):
        is_file_updated = 1;

# Write to the out header file if required
if is_file_updated:
    # Truncate and re-write all definitions
    out_header.seek(0)
    out_header.truncate()
    # Push File header
    append_feature_def_to_file('/******************************************************************************')
    append_feature_def_to_file('  @file    qcril_features_def.h')
    append_feature_def_to_file('  @brief   qcril feature definition information')
    append_feature_def_to_file('')
    append_feature_def_to_file('  DESCRIPTION')
    append_feature_def_to_file('')
    append_feature_def_to_file('  ---------------------------------------------------------------------------')
    append_feature_def_to_file('')
    append_feature_def_to_file('  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.')
    append_feature_def_to_file('  Qualcomm Technologies Proprietary and Confidential.')
    append_feature_def_to_file('  ---------------------------------------------------------------------------')
    append_feature_def_to_file('******************************************************************************/')

    append_feature_def_to_file('#ifndef QCRIL_FEATURES_DEF_H')
    append_feature_def_to_file('#define QCRIL_FEATURES_DEF_H')

    append_feature_def_to_file(feature_def_buffer)

    append_feature_def_to_file('#endif /* QCRIL_FEATURES_DEF_H */')

ril_header.close();
out_header.close();
