/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P H O N E B O O K _ M A N A G E R _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the pbm service Data structures.

  Copyright (c) 2010-2014, 2016 - 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/4.7/pbm/src/phonebook_manager_service_v01.c#3 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Apr 13 2017 (Spin 0)
   From IDL File: phonebook_manager_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "phonebook_manager_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t pbm_phonebook_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_phonebook_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_phonebook_info_type_v01, pb_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_capability_basic_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_type_v01, pb_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_type_v01, used_records),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_type_v01, max_num_len),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_type_v01, max_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_group_capability_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_group_capability_type_v01, max_grp),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_group_capability_type_v01, max_grp_tag_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_ad_num_capability_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_type_v01, max_ad_num),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_type_v01, max_ad_num_len),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_type_v01, max_ad_num_tag_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_email_capability_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_email_capability_type_v01, max_email),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_email_capability_type_v01, max_email_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_gas_capability_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_capability_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_capability_type_v01, used_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_capability_type_v01, max_gas_string_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_capability_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_capability_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_capability_type_v01, used_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_capability_type_v01, max_aas_string_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_access_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(pbm_access_info_type_v01, protection_method),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(pbm_access_info_type_v01, pin_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_capability_basic_info_array_elem_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_elem_type_v01, pb_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_elem_type_v01, used_records),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_elem_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_elem_type_v01, max_num_len),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_elem_type_v01, max_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_capability_basic_info_array_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_type_v01, session_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_type_v01, capability_basic_info),
  QMI_PBM_MAX_NUM_PBS_V01,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_type_v01, capability_basic_info) - QMI_IDL_OFFSET8(pbm_capability_basic_info_array_type_v01, capability_basic_info_len),
  QMI_IDL_TYPE88(0, 8),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_group_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_group_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_group_capability_with_session_type_v01, max_grp),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_group_capability_with_session_type_v01, max_grp_tag_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_ad_num_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_with_session_type_v01, max_ad_num),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_with_session_type_v01, max_ad_num_len),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_with_session_type_v01, max_ad_num_tag_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_email_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_email_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_email_capability_with_session_type_v01, max_email),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_email_capability_with_session_type_v01, max_email_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_second_name_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_second_name_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_second_name_capability_with_session_type_v01, max_second_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_hidden_records_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_hidden_records_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_hidden_records_capability_with_session_type_v01, is_hidden_entry_supported),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_gas_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_gas_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_capability_with_session_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_capability_with_session_type_v01, used_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_capability_with_session_type_v01, max_gas_string_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_capability_with_session_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_capability_with_session_type_v01, used_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_capability_with_session_type_v01, max_aas_string_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_info_type_v01, pb_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_info_type_v01, record_start_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_info_type_v01, record_end_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_information_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, phonebook_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, record_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, num_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, num_plan),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, number),
  QMI_PBM_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, number) - QMI_IDL_OFFSET8(pbm_record_information_type_v01, number_len),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, name),
  QMI_PBM_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, name) - QMI_IDL_OFFSET8(pbm_record_information_type_v01, name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_ad_num_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_ad_num_info_type_v01, num_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_ad_num_info_type_v01, num_plan),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_info_type_v01, ad_number),
  QMI_PBM_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_ad_num_info_type_v01, ad_number) - QMI_IDL_OFFSET8(pbm_ad_num_info_type_v01, ad_number_len),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_info_type_v01, ad_num_tag_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_email_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_email_info_type_v01, email_address),
  QMI_PBM_EMAIL_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_email_info_type_v01, email_address) - QMI_IDL_OFFSET8(pbm_email_info_type_v01, email_address_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_num_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_num_info_type_v01, num_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_num_info_type_v01, num_plan),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_num_info_type_v01, number),
  QMI_PBM_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_num_info_type_v01, number) - QMI_IDL_OFFSET8(pbm_num_info_type_v01, number_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_email_info_ext_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_email_info_ext_type_v01, email_address),
  QMI_PBM_EMAIL_MAX_LENGTH_EXT_V01,
  QMI_IDL_OFFSET8(pbm_email_info_ext_type_v01, email_address) - QMI_IDL_OFFSET8(pbm_email_info_ext_type_v01, email_address_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_base_record_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_base_record_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_base_record_info_type_v01, pb_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_base_record_info_type_v01, record_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_id_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_id_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_id_type_v01, pb_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_id_type_v01, record_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_update_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_update_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_update_info_type_v01, pb_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_update_info_type_v01, operation),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_update_info_type_v01, record_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_refresh_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_refresh_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_refresh_info_type_v01, pb_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_refresh_info_type_v01, status),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_emer_num_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_emer_num_type_v01, emer_num),
  QMI_PBM_EMER_NUM_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_emer_num_type_v01, emer_num) - QMI_IDL_OFFSET8(pbm_emer_num_type_v01, emer_num_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_ecc_emer_num_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ecc_emer_num_type_v01, cat),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ecc_emer_num_type_v01, emer_num),
  QMI_PBM_EMER_NUM_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_ecc_emer_num_type_v01, emer_num) - QMI_IDL_OFFSET8(pbm_ecc_emer_num_type_v01, emer_num_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_card_emer_num_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_card_emer_num_type_v01, session_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_card_emer_num_type_v01, emer_nums),
  QMI_PBM_MAX_NUM_CARD_ECC_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_card_emer_num_type_v01, emer_nums) - QMI_IDL_OFFSET8(pbm_card_emer_num_type_v01, emer_nums_len),
  QMI_IDL_TYPE88(0, 28),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_network_emer_num_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_network_emer_num_type_v01, session_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_network_emer_num_type_v01, emer_nums),
  QMI_PBM_MAX_NUM_NETWORK_ECC_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_network_emer_num_type_v01, emer_nums) - QMI_IDL_OFFSET8(pbm_network_emer_num_type_v01, emer_nums_len),
  QMI_IDL_TYPE88(0, 28),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_phonebook_ready_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_phonebook_ready_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_phonebook_ready_info_type_v01, pb_bit_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_usim_field_capability_array_elem_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(pbm_usim_field_capability_array_elem_type_v01, field_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(pbm_usim_field_capability_array_elem_type_v01, mapping_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_usim_field_capability_array_elem_type_v01, num_of_records),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_usim_field_capability_array_elem_type_v01, num_free_records),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_usim_field_capability_array_elem_type_v01, record_length),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_usim_field_capability_array_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_usim_field_capability_array_type_v01, pb_set),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_usim_field_capability_array_type_v01, usim_field_capability),
  QMI_PBM_MAX_NUM_USIM_FIELDS_IN_RECORD_V01,
  QMI_IDL_OFFSET8(pbm_usim_field_capability_array_type_v01, usim_field_capability) - QMI_IDL_OFFSET8(pbm_usim_field_capability_array_type_v01, usim_field_capability_len),
  QMI_IDL_TYPE88(0, 32),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_usim_field_capabilities_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_usim_field_capabilities_type_v01, session_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_usim_field_capabilities_type_v01, pbset_cap_instances),
  QMI_PBM_MAX_NUM_PBSET_CAP_INSTANCE_V01,
  QMI_IDL_OFFSET8(pbm_usim_field_capabilities_type_v01, pbset_cap_instances) - QMI_IDL_OFFSET8(pbm_usim_field_capabilities_type_v01, pbset_cap_instances_len),
  QMI_IDL_TYPE88(0, 33),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_instance_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, record_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, num_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, num_plan),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, number),
  QMI_PBM_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, number) - QMI_IDL_OFFSET8(pbm_record_instance_type_v01, number_len),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, name),
  QMI_PBM_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, name) - QMI_IDL_OFFSET8(pbm_record_instance_type_v01, name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_basic_record_data_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_basic_record_data_type_v01, seq_num),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_basic_record_data_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_basic_record_data_type_v01, pb_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_basic_record_data_type_v01, record_instances),
  QMI_PBM_MAX_NUM_BASIC_RECORD_INSTANCE_V01,
  QMI_IDL_OFFSET8(pbm_basic_record_data_type_v01, record_instances) - QMI_IDL_OFFSET8(pbm_basic_record_data_type_v01, record_instances_len),
  QMI_IDL_TYPE88(0, 35),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_sname_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_sname_info_type_v01, record_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_sname_info_type_v01, sname),
  QMI_PBM_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_sname_info_type_v01, sname) - QMI_IDL_OFFSET8(pbm_sname_info_type_v01, sname_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_ad_num_record_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_record_info_type_v01, record_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_ad_num_record_info_type_v01, ad_nums),
  QMI_PBM_MAX_NUM_AD_NUM_COUNT_V01,
  QMI_IDL_OFFSET8(pbm_ad_num_record_info_type_v01, ad_nums) - QMI_IDL_OFFSET8(pbm_ad_num_record_info_type_v01, ad_nums_len),
  QMI_IDL_TYPE88(0, 19),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_grp_id_record_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_grp_id_record_info_type_v01, record_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_grp_id_record_info_type_v01, grp_id),
  QMI_PBM_GRP_ID_MAX_V01,
  QMI_IDL_OFFSET8(pbm_grp_id_record_info_type_v01, grp_id) - QMI_IDL_OFFSET8(pbm_grp_id_record_info_type_v01, grp_id_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_email_record_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_email_record_info_type_v01, record_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_email_record_info_type_v01, email),
  QMI_PBM_EMAIL_INFO_MAX_V01,
  QMI_IDL_OFFSET8(pbm_email_record_info_type_v01, email) - QMI_IDL_OFFSET8(pbm_email_record_info_type_v01, email_len),
  QMI_IDL_TYPE88(0, 20),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_hidden_record_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_hidden_record_info_type_v01, record_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_hidden_record_info_type_v01, is_hidden),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_instance_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_instance_ext_type_v01, record_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_instance_ext_type_v01, num_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_instance_ext_type_v01, num_plan),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_instance_ext_type_v01, number),
  QMI_PBM_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_record_instance_ext_type_v01, number) - QMI_IDL_OFFSET8(pbm_record_instance_ext_type_v01, number_len),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_instance_ext_type_v01, name),
  QMI_PBM_NAME_MAX_LENGTH_EXT_V01,
  QMI_IDL_OFFSET8(pbm_record_instance_ext_type_v01, name) - QMI_IDL_OFFSET8(pbm_record_instance_ext_type_v01, name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_basic_record_data_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_basic_record_data_ext_type_v01, seq_num),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_basic_record_data_ext_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_basic_record_data_ext_type_v01, pb_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_basic_record_data_ext_type_v01, record_instances),
  QMI_PBM_MAX_NUM_BASIC_RECORD_INSTANCE_V01,
  QMI_IDL_OFFSET8(pbm_basic_record_data_ext_type_v01, record_instances) - QMI_IDL_OFFSET8(pbm_basic_record_data_ext_type_v01, record_instances_len),
  QMI_IDL_TYPE88(0, 42),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_sname_info_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_sname_info_ext_type_v01, record_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_sname_info_ext_type_v01, sname),
  QMI_PBM_NAME_MAX_LENGTH_EXT_V01,
  QMI_IDL_OFFSET8(pbm_sname_info_ext_type_v01, sname) - QMI_IDL_OFFSET8(pbm_sname_info_ext_type_v01, sname_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_email_record_info_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_email_record_info_ext_type_v01, record_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_email_record_info_ext_type_v01, email),
  QMI_PBM_EMAIL_INFO_MAX_V01,
  QMI_IDL_OFFSET8(pbm_email_record_info_ext_type_v01, email) - QMI_IDL_OFFSET8(pbm_email_record_info_ext_type_v01, email_len),
  QMI_IDL_TYPE88(0, 22),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_groups_instance_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_groups_instance_type_v01, grp_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_groups_instance_type_v01, grp_name),
  QMI_PBM_GROUP_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_groups_instance_type_v01, grp_name) - QMI_IDL_OFFSET8(pbm_groups_instance_type_v01, grp_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_groups_data_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_groups_data_type_v01, session_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_groups_data_type_v01, grp_array),
  QMI_PBM_MAX_NUM_GROUP_COUNT_V01,
  QMI_IDL_OFFSET8(pbm_groups_data_type_v01, grp_array) - QMI_IDL_OFFSET8(pbm_groups_data_type_v01, grp_array_len),
  QMI_IDL_TYPE88(0, 46),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_grp_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_grp_info_type_v01, session_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_grp_info_type_v01, operation),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_grp_info_type_v01, group_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_grp_info_type_v01, grp_name),
  QMI_PBM_GROUP_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_grp_info_type_v01, grp_name) - QMI_IDL_OFFSET8(pbm_grp_info_type_v01, grp_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_grp_id_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_grp_id_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_grp_id_type_v01, group_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_phonebook_state_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_phonebook_state_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_phonebook_state_type_v01, pb_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_phonebook_state_type_v01, state),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_hidden_status_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_hidden_status_type_v01, session_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_hidden_status_type_v01, status),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_instance_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_instance_type_v01, aas_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_instance_type_v01, alpha),
  QMI_PBM_AAS_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_aas_instance_type_v01, alpha) - QMI_IDL_OFFSET8(pbm_aas_instance_type_v01, alpha_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_data_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_data_type_v01, session_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_aas_data_type_v01, aas_array),
  QMI_PBM_MAX_NUM_AAS_COUNT_V01,
  QMI_IDL_OFFSET8(pbm_aas_data_type_v01, aas_array) - QMI_IDL_OFFSET8(pbm_aas_data_type_v01, aas_array_len),
  QMI_IDL_TYPE88(0, 52),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_info_type_v01, session_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_info_type_v01, operation),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_info_type_v01, aas_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_info_type_v01, alpha),
  QMI_PBM_AAS_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_aas_info_type_v01, alpha) - QMI_IDL_OFFSET8(pbm_aas_info_type_v01, alpha_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_id_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_id_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_id_type_v01, aas_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_update_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_update_info_type_v01, session_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_update_info_type_v01, operation),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_update_info_type_v01, aas_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_update_info_type_v01, alpha),
  QMI_PBM_AAS_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_aas_update_info_type_v01, alpha) - QMI_IDL_OFFSET8(pbm_aas_update_info_type_v01, alpha_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_gas_update_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_gas_update_info_type_v01, session_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_gas_update_info_type_v01, operation),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_update_info_type_v01, gas_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_update_info_type_v01, grp_name),
  QMI_PBM_GROUP_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_gas_update_info_type_v01, grp_name) - QMI_IDL_OFFSET8(pbm_gas_update_info_type_v01, grp_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_instance_undecoded_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_instance_undecoded_type_v01, record_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_instance_undecoded_type_v01, num_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_instance_undecoded_type_v01, num_plan),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_instance_undecoded_type_v01, number),
  QMI_PBM_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_record_instance_undecoded_type_v01, number) - QMI_IDL_OFFSET8(pbm_record_instance_undecoded_type_v01, number_len),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_instance_undecoded_type_v01, name),
  QMI_PBM_NAME_MAX_LENGTH_EXT_UNDECODED_V01,
  QMI_IDL_OFFSET8(pbm_record_instance_undecoded_type_v01, name) - QMI_IDL_OFFSET8(pbm_record_instance_undecoded_type_v01, name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_basic_record_data_undecoded_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_basic_record_data_undecoded_type_v01, seq_num),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_basic_record_data_undecoded_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_basic_record_data_undecoded_type_v01, pb_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_basic_record_data_undecoded_type_v01, record_instances),
  QMI_PBM_MAX_NUM_BASIC_RECORD_INSTANCE_V01,
  QMI_IDL_OFFSET8(pbm_basic_record_data_undecoded_type_v01, record_instances) - QMI_IDL_OFFSET8(pbm_basic_record_data_undecoded_type_v01, record_instances_len),
  QMI_IDL_TYPE88(0, 58),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_sname_info_undecoded_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_sname_info_undecoded_type_v01, record_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_sname_info_undecoded_type_v01, sname),
  QMI_PBM_NAME_MAX_LENGTH_EXT_UNDECODED_V01,
  QMI_IDL_OFFSET8(pbm_sname_info_undecoded_type_v01, sname) - QMI_IDL_OFFSET8(pbm_sname_info_undecoded_type_v01, sname_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_email_info_undecoded_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_email_info_undecoded_type_v01, email_address),
  QMI_PBM_EMAIL_MAX_LENGTH_EXT_UNDECODED_V01,
  QMI_IDL_OFFSET8(pbm_email_info_undecoded_type_v01, email_address) - QMI_IDL_OFFSET8(pbm_email_info_undecoded_type_v01, email_address_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_email_record_info_undecoded_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_email_record_info_undecoded_type_v01, record_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_email_record_info_undecoded_type_v01, email),
  QMI_PBM_EMAIL_INFO_MAX_V01,
  QMI_IDL_OFFSET8(pbm_email_record_info_undecoded_type_v01, email) - QMI_IDL_OFFSET8(pbm_email_record_info_undecoded_type_v01, email_len),
  QMI_IDL_TYPE88(0, 61),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_adn_caps_info_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_adn_caps_info_v01, max_adn_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_adn_caps_info_v01, used_adn_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_adn_caps_info_v01, max_adn_num_len),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_adn_caps_info_v01, max_adn_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_capability_ext_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_capability_ext_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_capability_ext_type_v01, used_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_capability_ext_type_v01, max_record_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_usim_capability_ext_type_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_usim_capability_ext_type_v01, record_caps),
  QMI_IDL_TYPE88(0, 64),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_usim_capability_ext_type_v01, mapping_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_pb_set_cap_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, pb_set_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, adn_caps),
  QMI_IDL_TYPE88(0, 63),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, email_caps),
  QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01,
  QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, email_caps) - QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, email_caps_len),
  QMI_IDL_TYPE88(0, 65),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, ad_num_caps),
  QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01,
  QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, ad_num_caps) - QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, ad_num_caps_len),
  QMI_IDL_TYPE88(0, 65),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, sne_caps),
  QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01,
  QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, sne_caps) - QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, sne_caps_len),
  QMI_IDL_TYPE88(0, 65),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, grp_caps),
  QMI_PBM_MAX_FIELDS_PER_USIM_TYPE_V01,
  QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, grp_caps) - QMI_IDL_OFFSET8(pbm_pb_set_cap_info_type_v01, grp_caps_len),
  QMI_IDL_TYPE88(0, 65),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_pbr_cap_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_pbr_cap_info_type_v01, num_of_pb_sets),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_pbr_cap_info_type_v01, aas_caps),
  QMI_IDL_TYPE88(0, 64),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_pbr_cap_info_type_v01, gas_caps),
  QMI_IDL_TYPE88(0, 64),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_pbr_cap_info_type_v01, pbset_caps),
  QMI_PBM_MAX_PB_SETS_V01,
  QMI_IDL_OFFSET8(pbm_pbr_cap_info_type_v01, pbset_caps) - QMI_IDL_OFFSET8(pbm_pbr_cap_info_type_v01, pbset_caps_len),
  QMI_IDL_TYPE88(0, 66),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_pb_set_cap_info_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_pb_set_cap_info_with_session_type_v01, session_type),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_pb_set_cap_info_with_session_type_v01, pb_set_info),
  QMI_IDL_TYPE88(0, 67),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_mcc_emer_num_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_mcc_emer_num_type_v01, emer_nums),
  QMI_PBM_MAX_NUM_MCC_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_mcc_emer_num_type_v01, emer_nums) - QMI_IDL_OFFSET8(pbm_mcc_emer_num_type_v01, emer_nums_len),
  QMI_IDL_TYPE88(0, 27),
  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t pbm_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(pbm_indication_register_req_msg_v01, reg_mask)
};

static const uint8_t pbm_indication_register_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_indication_register_resp_msg_v01, reg_mask) - QMI_IDL_OFFSET8(pbm_indication_register_resp_msg_v01, reg_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(pbm_indication_register_resp_msg_v01, reg_mask)
};

static const uint8_t pbm_get_pb_capabilities_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_req_msg_v01, phonebook_info),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t pbm_get_pb_capabilities_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, capability_basic_info) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, capability_basic_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, capability_basic_info),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, group_capability) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, group_capability_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, group_capability),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, ad_num_capability) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, ad_num_capability_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, ad_num_capability),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, email_capability) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, email_capability_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, email_capability),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, max_second_name_len) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, max_second_name_len_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, max_second_name_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, is_hidden_entry_supported) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, is_hidden_entry_supported_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, is_hidden_entry_supported),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, gas_capability) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, gas_capability_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, gas_capability),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, aas_capability) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, aas_capability_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, aas_capability),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, write_access_info) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, write_access_info_valid)),
  0x18,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, write_access_info),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, num_pb_sets) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, num_pb_sets_valid)),
  0x19,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, num_pb_sets),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, adn_pb_set_info) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, adn_pb_set_info_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, adn_pb_set_info),
  QMI_IDL_TYPE88(0, 67)
};

/*
 * pbm_get_all_pb_capabilities_req_msg is empty
 * static const uint8_t pbm_get_all_pb_capabilities_req_msg_data_v01[] = {
 * };
 */

static const uint8_t pbm_get_all_pb_capabilities_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_all_pb_capabilities_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_all_pb_capabilities_resp_msg_v01, capability_basic_info_array) - QMI_IDL_OFFSET8(pbm_get_all_pb_capabilities_resp_msg_v01, capability_basic_info_array_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_all_pb_capabilities_resp_msg_v01, capability_basic_info_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET8(pbm_get_all_pb_capabilities_resp_msg_v01, capability_basic_info_array) - QMI_IDL_OFFSET8(pbm_get_all_pb_capabilities_resp_msg_v01, capability_basic_info_array_len),
  QMI_IDL_TYPE88(0, 9),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, group_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, group_capability_array_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, group_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, group_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, group_capability_array_len),
  QMI_IDL_TYPE88(0, 10),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, ad_num_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, ad_num_capability_array_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, ad_num_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, ad_num_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, ad_num_capability_array_len),
  QMI_IDL_TYPE88(0, 11),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, email_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, email_capability_array_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, email_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, email_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, email_capability_array_len),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, second_name_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, second_name_capability_array_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, second_name_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, second_name_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, second_name_capability_array_len),
  QMI_IDL_TYPE88(0, 13),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, hidden_records_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, hidden_records_capability_array_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, hidden_records_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, hidden_records_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, hidden_records_capability_array_len),
  QMI_IDL_TYPE88(0, 14),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, gas_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, gas_capability_array_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, gas_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, gas_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, gas_capability_array_len),
  QMI_IDL_TYPE88(0, 15),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, aas_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, aas_capability_array_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, aas_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, aas_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, aas_capability_array_len),
  QMI_IDL_TYPE88(0, 16),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, pb_set_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, pb_set_info_array_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, pb_set_info_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, pb_set_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, pb_set_info_array_len),
  QMI_IDL_TYPE88(0, 68)
};

static const uint8_t pbm_read_pbset_caps_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_read_pbset_caps_req_msg_v01, session_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_read_pbset_caps_req_msg_v01, start_id) - QMI_IDL_OFFSET8(pbm_read_pbset_caps_req_msg_v01, start_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_read_pbset_caps_req_msg_v01, start_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_read_pbset_caps_req_msg_v01, end_id) - QMI_IDL_OFFSET8(pbm_read_pbset_caps_req_msg_v01, end_id_valid)),
  0x11,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_read_pbset_caps_req_msg_v01, end_id)
};

static const uint8_t pbm_read_pbset_caps_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_read_pbset_caps_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_read_pbset_caps_resp_msg_v01, num_of_pbsets) - QMI_IDL_OFFSET8(pbm_read_pbset_caps_resp_msg_v01, num_of_pbsets_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_read_pbset_caps_resp_msg_v01, num_of_pbsets)
};

static const uint8_t pbm_read_records_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_read_records_req_msg_v01, record_info),
  QMI_IDL_TYPE88(0, 17)
};

static const uint8_t pbm_read_records_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_read_records_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_read_records_resp_msg_v01, num_of_recs) - QMI_IDL_OFFSET8(pbm_read_records_resp_msg_v01, num_of_recs_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_read_records_resp_msg_v01, num_of_recs)
};

static const uint8_t pbm_read_records_ext_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_read_records_ext_req_msg_v01, record_info),
  QMI_IDL_TYPE88(0, 17)
};

static const uint8_t pbm_read_records_ext_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_read_records_ext_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_read_records_ext_resp_msg_v01, num_of_recs) - QMI_IDL_OFFSET8(pbm_read_records_ext_resp_msg_v01, num_of_recs_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_read_records_ext_resp_msg_v01, num_of_recs)
};

static const uint8_t pbm_write_record_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_write_record_req_msg_v01, record_information),
  QMI_IDL_TYPE88(0, 18),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, sname) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, sname_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_req_msg_v01, sname),
  QMI_PBM_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, sname) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, sname_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, ad_num_info) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, ad_num_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_req_msg_v01, ad_num_info),
  QMI_PBM_AD_NUM_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, ad_num_info) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, ad_num_info_len),
  QMI_IDL_TYPE88(0, 19),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, grp_id) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, grp_id_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_req_msg_v01, grp_id),
  QMI_PBM_GRP_ID_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, grp_id) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, grp_id_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, email_info) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, email_info_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_req_msg_v01, email_info),
  QMI_PBM_EMAIL_INFO_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, email_info) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, email_info_len),
  QMI_IDL_TYPE88(0, 20),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, is_hidden) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, is_hidden_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_req_msg_v01, is_hidden)
};

static const uint8_t pbm_write_record_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_write_record_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_write_record_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_write_record_resp_msg_v01, record_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_write_record_resp_msg_v01, record_id)
};

static const uint8_t pbm_write_record_ext_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_write_record_ext_req_msg_v01, record_info),
  QMI_IDL_TYPE88(0, 23),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_write_record_ext_req_msg_v01, num) - QMI_IDL_OFFSET8(pbm_write_record_ext_req_msg_v01, num_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_write_record_ext_req_msg_v01, num),
  QMI_IDL_TYPE88(0, 21),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_write_record_ext_req_msg_v01, name) - QMI_IDL_OFFSET8(pbm_write_record_ext_req_msg_v01, name_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_write_record_ext_req_msg_v01, name),
  QMI_PBM_NAME_MAX_LENGTH_EXT_V01,
  QMI_IDL_OFFSET8(pbm_write_record_ext_req_msg_v01, name) - QMI_IDL_OFFSET8(pbm_write_record_ext_req_msg_v01, name_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, sname) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, sname_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_ext_req_msg_v01, sname),
  QMI_PBM_NAME_MAX_LENGTH_EXT_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, sname) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, sname_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, ad_num_info) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, ad_num_info_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_ext_req_msg_v01, ad_num_info),
  QMI_PBM_AD_NUM_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, ad_num_info) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, ad_num_info_len),
  QMI_IDL_TYPE88(0, 19),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, grp_id) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, grp_id_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_ext_req_msg_v01, grp_id),
  QMI_PBM_GRP_ID_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, grp_id) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, grp_id_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, email_info) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, email_info_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_ext_req_msg_v01, email_info),
  QMI_PBM_EMAIL_INFO_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, email_info) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, email_info_len),
  QMI_IDL_TYPE88(0, 22),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, is_hidden) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_ext_req_msg_v01, is_hidden_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_ext_req_msg_v01, is_hidden)
};

static const uint8_t pbm_write_record_ext_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_write_record_ext_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_write_record_ext_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_write_record_ext_resp_msg_v01, record_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_write_record_ext_resp_msg_v01, record_id)
};

static const uint8_t pbm_delete_record_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_delete_record_req_msg_v01, record_info),
  QMI_IDL_TYPE88(0, 24)
};

static const uint8_t pbm_delete_record_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_delete_record_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_delete_record_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_delete_record_resp_msg_v01, record_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_delete_record_resp_msg_v01, record_id)
};

static const uint8_t pbm_delete_all_pb_records_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_delete_all_pb_records_req_msg_v01, phonebook_info),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t pbm_delete_all_pb_records_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_delete_all_pb_records_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t pbm_search_records_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, search_info),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, number) - QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, number_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, number),
  QMI_PBM_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, number) - QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, number_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, name) - QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, name_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, name),
  QMI_PBM_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, name) - QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, name_len)
};

static const uint8_t pbm_search_records_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_search_records_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_search_records_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_search_records_resp_msg_v01, record_id_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_search_records_resp_msg_v01, record_id),
  ((QMI_PBM_REC_IDS_MAX_V01) & 0xFF), ((QMI_PBM_REC_IDS_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(pbm_search_records_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_search_records_resp_msg_v01, record_id_len)
};

static const uint8_t pbm_search_records_ext_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_search_records_ext_req_msg_v01, search_info),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_search_records_ext_req_msg_v01, number) - QMI_IDL_OFFSET8(pbm_search_records_ext_req_msg_v01, number_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_search_records_ext_req_msg_v01, number),
  QMI_PBM_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_search_records_ext_req_msg_v01, number) - QMI_IDL_OFFSET8(pbm_search_records_ext_req_msg_v01, number_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_search_records_ext_req_msg_v01, name) - QMI_IDL_OFFSET8(pbm_search_records_ext_req_msg_v01, name_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_search_records_ext_req_msg_v01, name),
  QMI_PBM_NAME_MAX_LENGTH_EXT_V01,
  QMI_IDL_OFFSET8(pbm_search_records_ext_req_msg_v01, name) - QMI_IDL_OFFSET8(pbm_search_records_ext_req_msg_v01, name_len)
};

static const uint8_t pbm_search_records_ext_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_search_records_ext_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_search_records_ext_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_search_records_ext_resp_msg_v01, record_id_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_search_records_ext_resp_msg_v01, record_id),
  ((QMI_PBM_REC_IDS_MAX_V01) & 0xFF), ((QMI_PBM_REC_IDS_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(pbm_search_records_ext_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_search_records_ext_resp_msg_v01, record_id_len)
};

static const uint8_t pbm_record_update_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_record_update_ind_msg_v01, record_update_info),
  QMI_IDL_TYPE88(0, 25)
};

static const uint8_t pbm_refresh_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_refresh_ind_msg_v01, refresh_info),
  QMI_IDL_TYPE88(0, 26)
};

static const uint8_t pbm_pb_ready_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_pb_ready_ind_msg_v01, phonebook_ready_info),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t pbm_emergency_list_ind_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, emer_nums),
  QMI_PBM_MAX_NUM_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, emer_nums) - QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, emer_nums_len),
  QMI_IDL_TYPE88(0, 27),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, nv_emer_nums) - QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, nv_emer_nums_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, nv_emer_nums),
  QMI_PBM_MAX_NUM_NV_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, nv_emer_nums) - QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, nv_emer_nums_len),
  QMI_IDL_TYPE88(0, 27),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, card_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, card_emer_nums_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_emergency_list_ind_msg_v01, card_emer_nums),
  QMI_PBM_MAX_NUM_CARD_EMER_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, card_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, card_emer_nums_len),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, network_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, network_emer_nums_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_emergency_list_ind_msg_v01, network_emer_nums),
  QMI_PBM_MAX_NUM_NETWORK_EMER_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, network_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, network_emer_nums_len),
  QMI_IDL_TYPE88(0, 30),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, emer_nums_extended) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, emer_nums_extended_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_emergency_list_ind_msg_v01, emer_nums_extended),
  QMI_PBM_MAX_NUM_EMER_NUM_EXTENDED_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, emer_nums_extended) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, emer_nums_extended_len),
  QMI_IDL_TYPE88(0, 27),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, nv_emer_nums_extended) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, nv_emer_nums_extended_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_emergency_list_ind_msg_v01, nv_emer_nums_extended),
  QMI_PBM_MAX_NUM_NV_EMER_NUM_EXTENDED_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, nv_emer_nums_extended) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, nv_emer_nums_extended_len),
  QMI_IDL_TYPE88(0, 27),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, emer_nums_extended_cat) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, emer_nums_extended_cat_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_emergency_list_ind_msg_v01, emer_nums_extended_cat),
  QMI_PBM_MAX_NUM_EMER_NUM_EXTENDED_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, emer_nums_extended_cat) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, emer_nums_extended_cat_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, nv_emer_nums_extended_cat) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, nv_emer_nums_extended_cat_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_emergency_list_ind_msg_v01, nv_emer_nums_extended_cat),
  QMI_PBM_MAX_NUM_NV_EMER_NUM_EXTENDED_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, nv_emer_nums_extended_cat) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, nv_emer_nums_extended_cat_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, mcc_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, mcc_emer_nums_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_emergency_list_ind_msg_v01, mcc_emer_nums),
  QMI_PBM_MAX_NUM_SLOT_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, mcc_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, mcc_emer_nums_len),
  QMI_IDL_TYPE88(0, 69)
};

static const uint8_t pbm_all_pb_init_done_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_all_pb_init_done_ind_msg_v01, phonebook_ready_info),
  QMI_PBM_MAX_NUM_PHONEBOOK_READY_INFO_V01,
  QMI_IDL_OFFSET8(pbm_all_pb_init_done_ind_msg_v01, phonebook_ready_info) - QMI_IDL_OFFSET8(pbm_all_pb_init_done_ind_msg_v01, phonebook_ready_info_len),
  QMI_IDL_TYPE88(0, 31)
};

static const uint8_t pbm_pbset_cap_read_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_pbset_cap_read_ind_msg_v01, usim_field_capabilities),
  QMI_IDL_TYPE88(0, 34)
};

static const uint8_t pbm_record_read_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_record_read_ind_msg_v01, basic_record_data),
  QMI_IDL_TYPE88(0, 36),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, sname_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, sname_info_array_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ind_msg_v01, sname_info_array),
  QMI_PBM_MAX_NUM_SNAME_INFO_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, sname_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, sname_info_array_len),
  QMI_IDL_TYPE88(0, 37),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, ad_num_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, ad_num_info_array_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ind_msg_v01, ad_num_info_array),
  QMI_PBM_MAX_NUM_AD_NUM_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, ad_num_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, ad_num_info_array_len),
  QMI_IDL_TYPE88(0, 38),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, grp_id_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, grp_id_info_array_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ind_msg_v01, grp_id_info_array),
  QMI_PBM_MAX_NUM_GRP_ID_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, grp_id_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, grp_id_info_array_len),
  QMI_IDL_TYPE88(0, 39),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, email_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, email_info_array_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ind_msg_v01, email_info_array),
  QMI_PBM_MAX_NUM_EMAIL_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, email_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, email_info_array_len),
  QMI_IDL_TYPE88(0, 40),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, hidden_record_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, hidden_record_info_array_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ind_msg_v01, hidden_record_info_array),
  QMI_PBM_MAX_NUM_HIDDEN_INFO_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, hidden_record_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, hidden_record_info_array_len),
  QMI_IDL_TYPE88(0, 41)
};

static const uint8_t pbm_record_read_ext_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_record_read_ext_ind_msg_v01, basic_record_data),
  QMI_IDL_TYPE88(0, 43),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, sname_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, sname_info_array_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ext_ind_msg_v01, sname_info_array),
  QMI_PBM_MAX_NUM_SNAME_INFO_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, sname_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, sname_info_array_len),
  QMI_IDL_TYPE88(0, 44),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, ad_num_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, ad_num_info_array_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ext_ind_msg_v01, ad_num_info_array),
  QMI_PBM_MAX_NUM_AD_NUM_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, ad_num_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, ad_num_info_array_len),
  QMI_IDL_TYPE88(0, 38),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, grp_id_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, grp_id_info_array_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ext_ind_msg_v01, grp_id_info_array),
  QMI_PBM_MAX_NUM_GRP_ID_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, grp_id_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, grp_id_info_array_len),
  QMI_IDL_TYPE88(0, 39),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, email_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, email_info_array_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ext_ind_msg_v01, email_info_array),
  QMI_PBM_MAX_NUM_EMAIL_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, email_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, email_info_array_len),
  QMI_IDL_TYPE88(0, 45),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, hidden_record_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, hidden_record_info_array_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ext_ind_msg_v01, hidden_record_info_array),
  QMI_PBM_MAX_NUM_HIDDEN_INFO_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, hidden_record_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_ind_msg_v01, hidden_record_info_array_len),
  QMI_IDL_TYPE88(0, 41)
};

/*
 * pbm_get_emergency_list_req_msg is empty
 * static const uint8_t pbm_get_emergency_list_req_msg_data_v01[] = {
 * };
 */

static const uint8_t pbm_get_emergency_list_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_emer_nums) - QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_emer_nums_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_emer_nums),
  QMI_PBM_MAX_NUM_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_emer_nums) - QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_emer_nums_len),
  QMI_IDL_TYPE88(0, 27),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_nv_emer_nums) - QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_nv_emer_nums_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_nv_emer_nums),
  QMI_PBM_MAX_NUM_NV_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_nv_emer_nums) - QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_nv_emer_nums_len),
  QMI_IDL_TYPE88(0, 27),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, card_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, card_emer_nums_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_emergency_list_resp_msg_v01, card_emer_nums),
  QMI_PBM_MAX_NUM_CARD_EMER_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, card_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, card_emer_nums_len),
  QMI_IDL_TYPE88(0, 29),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, network_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, network_emer_nums_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_emergency_list_resp_msg_v01, network_emer_nums),
  QMI_PBM_MAX_NUM_NETWORK_EMER_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, network_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, network_emer_nums_len),
  QMI_IDL_TYPE88(0, 30),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, emer_nums_extended) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, emer_nums_extended_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_emergency_list_resp_msg_v01, emer_nums_extended),
  QMI_PBM_MAX_NUM_EMER_NUM_EXTENDED_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, emer_nums_extended) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, emer_nums_extended_len),
  QMI_IDL_TYPE88(0, 27),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, nv_emer_nums_extended) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, nv_emer_nums_extended_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_emergency_list_resp_msg_v01, nv_emer_nums_extended),
  QMI_PBM_MAX_NUM_NV_EMER_NUM_EXTENDED_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, nv_emer_nums_extended) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, nv_emer_nums_extended_len),
  QMI_IDL_TYPE88(0, 27),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, emer_nums_extended_cat) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, emer_nums_extended_cat_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_emergency_list_resp_msg_v01, emer_nums_extended_cat),
  QMI_PBM_MAX_NUM_EMER_NUM_EXTENDED_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, emer_nums_extended_cat) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, emer_nums_extended_cat_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, nv_emer_nums_extended_cat) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, nv_emer_nums_extended_cat_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_emergency_list_resp_msg_v01, nv_emer_nums_extended_cat),
  QMI_PBM_MAX_NUM_NV_EMER_NUM_EXTENDED_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, nv_emer_nums_extended_cat) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, nv_emer_nums_extended_cat_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, mcc_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, mcc_emer_nums_valid)),
  0x18,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_emergency_list_resp_msg_v01, mcc_emer_nums),
  QMI_PBM_MAX_NUM_SLOT_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, mcc_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, mcc_emer_nums_len),
  QMI_IDL_TYPE88(0, 69)
};

/*
 * pbm_get_all_groups_req_msg is empty
 * static const uint8_t pbm_get_all_groups_req_msg_data_v01[] = {
 * };
 */

static const uint8_t pbm_get_all_groups_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_all_groups_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_all_groups_resp_msg_v01, groups_data) - QMI_IDL_OFFSET8(pbm_get_all_groups_resp_msg_v01, groups_data_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_all_groups_resp_msg_v01, groups_data),
  QMI_PBM_MAX_NUM_GROUPS_DATA_V01,
  QMI_IDL_OFFSET8(pbm_get_all_groups_resp_msg_v01, groups_data) - QMI_IDL_OFFSET8(pbm_get_all_groups_resp_msg_v01, groups_data_len),
  QMI_IDL_TYPE88(0, 47)
};

static const uint8_t pbm_set_group_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_group_info_req_msg_v01, grp_info),
  QMI_IDL_TYPE88(0, 48)
};

static const uint8_t pbm_set_group_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_group_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_set_group_info_resp_msg_v01, grp_id) - QMI_IDL_OFFSET8(pbm_set_group_info_resp_msg_v01, grp_id_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_group_info_resp_msg_v01, grp_id),
  QMI_IDL_TYPE88(0, 49)
};

static const uint8_t pbm_get_pb_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_state_req_msg_v01, phonebook_info),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t pbm_get_pb_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_state_resp_msg_v01, phonebook_state) - QMI_IDL_OFFSET8(pbm_get_pb_state_resp_msg_v01, phonebook_state_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_state_resp_msg_v01, phonebook_state),
  QMI_IDL_TYPE88(0, 50)
};

static const uint8_t pbm_read_all_hidden_records_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_read_all_hidden_records_req_msg_v01, session_type)
};

static const uint8_t pbm_read_all_hidden_records_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_read_all_hidden_records_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_read_all_hidden_records_resp_msg_v01, num_of_recs) - QMI_IDL_OFFSET8(pbm_read_all_hidden_records_resp_msg_v01, num_of_recs_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_read_all_hidden_records_resp_msg_v01, num_of_recs)
};

static const uint8_t pbm_read_all_hidden_records_ext_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_read_all_hidden_records_ext_req_msg_v01, session_type)
};

static const uint8_t pbm_read_all_hidden_records_ext_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_read_all_hidden_records_ext_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_read_all_hidden_records_ext_resp_msg_v01, num_of_recs) - QMI_IDL_OFFSET8(pbm_read_all_hidden_records_ext_resp_msg_v01, num_of_recs_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_read_all_hidden_records_ext_resp_msg_v01, num_of_recs)
};

static const uint8_t pbm_hidden_record_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_hidden_record_status_ind_msg_v01, status_info),
  QMI_IDL_TYPE88(0, 51)
};

static const uint8_t pbm_get_next_empty_record_id_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_next_empty_record_id_req_msg_v01, record_info),
  QMI_IDL_TYPE88(0, 23)
};

static const uint8_t pbm_get_next_empty_record_id_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_next_empty_record_id_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_next_empty_record_id_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_get_next_empty_record_id_resp_msg_v01, record_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_get_next_empty_record_id_resp_msg_v01, record_id)
};

static const uint8_t pbm_get_next_non_empty_record_id_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_next_non_empty_record_id_req_msg_v01, record_info),
  QMI_IDL_TYPE88(0, 23)
};

static const uint8_t pbm_get_next_non_empty_record_id_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_next_non_empty_record_id_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_next_non_empty_record_id_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_get_next_non_empty_record_id_resp_msg_v01, record_id_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_get_next_non_empty_record_id_resp_msg_v01, record_id)
};

/*
 * pbm_get_all_aas_req_msg is empty
 * static const uint8_t pbm_get_all_aas_req_msg_data_v01[] = {
 * };
 */

static const uint8_t pbm_get_all_aas_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_all_aas_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_all_aas_resp_msg_v01, aas_data) - QMI_IDL_OFFSET8(pbm_get_all_aas_resp_msg_v01, aas_data_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_all_aas_resp_msg_v01, aas_data),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET8(pbm_get_all_aas_resp_msg_v01, aas_data) - QMI_IDL_OFFSET8(pbm_get_all_aas_resp_msg_v01, aas_data_len),
  QMI_IDL_TYPE88(0, 53)
};

static const uint8_t pbm_set_aas_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_aas_req_msg_v01, aas_info),
  QMI_IDL_TYPE88(0, 54)
};

static const uint8_t pbm_set_aas_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_aas_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_set_aas_resp_msg_v01, aas_id) - QMI_IDL_OFFSET8(pbm_set_aas_resp_msg_v01, aas_id_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_aas_resp_msg_v01, aas_id),
  QMI_IDL_TYPE88(0, 55)
};

static const uint8_t pbm_aas_update_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_aas_update_ind_msg_v01, aas_update_info),
  QMI_IDL_TYPE88(0, 56),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_aas_update_ind_msg_v01, aas_ext) - QMI_IDL_OFFSET16RELATIVE(pbm_aas_update_ind_msg_v01, aas_ext_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_aas_update_ind_msg_v01, aas_ext),
  QMI_PBM_NAME_MAX_LENGTH_EXT_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_aas_update_ind_msg_v01, aas_ext) - QMI_IDL_OFFSET16RELATIVE(pbm_aas_update_ind_msg_v01, aas_ext_len)
};

static const uint8_t pbm_gas_update_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_gas_update_ind_msg_v01, gas_update_info),
  QMI_IDL_TYPE88(0, 57),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_gas_update_ind_msg_v01, gas_ext) - QMI_IDL_OFFSET16RELATIVE(pbm_gas_update_ind_msg_v01, gas_ext_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_gas_update_ind_msg_v01, gas_ext),
  QMI_PBM_NAME_MAX_LENGTH_EXT_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_gas_update_ind_msg_v01, gas_ext) - QMI_IDL_OFFSET16RELATIVE(pbm_gas_update_ind_msg_v01, gas_ext_len)
};

static const uint8_t pbm_bind_subscription_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_bind_subscription_req_msg_v01, subs_type)
};

static const uint8_t pbm_bind_subscription_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_bind_subscription_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * pbm_get_subscription_binding_req_msg is empty
 * static const uint8_t pbm_get_subscription_binding_req_msg_data_v01[] = {
 * };
 */

static const uint8_t pbm_get_subscription_binding_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_subscription_binding_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_subscription_binding_resp_msg_v01, subs_type) - QMI_IDL_OFFSET8(pbm_get_subscription_binding_resp_msg_v01, subs_type_valid)),
  0x10,
   QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_get_subscription_binding_resp_msg_v01, subs_type)
};

static const uint8_t pbm_sim_init_done_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(pbm_sim_init_done_ind_msg_v01, slot_id)
};

static const uint8_t pbm_read_records_ext_undecoded_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_read_records_ext_undecoded_req_msg_v01, record_info),
  QMI_IDL_TYPE88(0, 17)
};

static const uint8_t pbm_read_records_ext_undecoded_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_read_records_ext_undecoded_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_read_records_ext_undecoded_resp_msg_v01, num_of_recs) - QMI_IDL_OFFSET8(pbm_read_records_ext_undecoded_resp_msg_v01, num_of_recs_valid)),
  0x10,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_read_records_ext_undecoded_resp_msg_v01, num_of_recs)
};

static const uint8_t pbm_record_read_ext_undecoded_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_record_read_ext_undecoded_ind_msg_v01, basic_record_data),
  QMI_IDL_TYPE88(0, 59),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, sname_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, sname_info_array_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ext_undecoded_ind_msg_v01, sname_info_array),
  QMI_PBM_MAX_NUM_SNAME_INFO_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, sname_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, sname_info_array_len),
  QMI_IDL_TYPE88(0, 60),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, ad_num_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, ad_num_info_array_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ext_undecoded_ind_msg_v01, ad_num_info_array),
  QMI_PBM_MAX_NUM_AD_NUM_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, ad_num_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, ad_num_info_array_len),
  QMI_IDL_TYPE88(0, 38),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, grp_id_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, grp_id_info_array_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ext_undecoded_ind_msg_v01, grp_id_info_array),
  QMI_PBM_MAX_NUM_GRP_ID_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, grp_id_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, grp_id_info_array_len),
  QMI_IDL_TYPE88(0, 39),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, email_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, email_info_array_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ext_undecoded_ind_msg_v01, email_info_array),
  QMI_PBM_MAX_NUM_EMAIL_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, email_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, email_info_array_len),
  QMI_IDL_TYPE88(0, 62),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, hidden_record_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, hidden_record_info_array_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ext_undecoded_ind_msg_v01, hidden_record_info_array),
  QMI_PBM_MAX_NUM_HIDDEN_INFO_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, hidden_record_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ext_undecoded_ind_msg_v01, hidden_record_info_array_len),
  QMI_IDL_TYPE88(0, 41)
};

static const uint8_t pbm_set_configuration_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(pbm_set_configuration_req_msg_v01, pbm_caching_status_mask)
};

static const uint8_t pbm_set_configuration_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_configuration_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t pbm_get_configuration_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_configuration_req_msg_v01, pbm_feature_status_mask) - QMI_IDL_OFFSET8(pbm_get_configuration_req_msg_v01, pbm_feature_status_mask_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(pbm_get_configuration_req_msg_v01, pbm_feature_status_mask)
};

static const uint8_t pbm_get_configuration_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_configuration_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_configuration_resp_msg_v01, centralized_ecc) - QMI_IDL_OFFSET8(pbm_get_configuration_resp_msg_v01, centralized_ecc_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_get_configuration_resp_msg_v01, centralized_ecc)
};

/* Type Table */
static const qmi_idl_type_table_entry  pbm_type_table_v01[] = {
  {sizeof(pbm_phonebook_info_type_v01), pbm_phonebook_info_type_data_v01},
  {sizeof(pbm_capability_basic_info_type_v01), pbm_capability_basic_info_type_data_v01},
  {sizeof(pbm_group_capability_type_v01), pbm_group_capability_type_data_v01},
  {sizeof(pbm_ad_num_capability_type_v01), pbm_ad_num_capability_type_data_v01},
  {sizeof(pbm_email_capability_type_v01), pbm_email_capability_type_data_v01},
  {sizeof(pbm_gas_capability_type_v01), pbm_gas_capability_type_data_v01},
  {sizeof(pbm_aas_capability_type_v01), pbm_aas_capability_type_data_v01},
  {sizeof(pbm_access_info_type_v01), pbm_access_info_type_data_v01},
  {sizeof(pbm_capability_basic_info_array_elem_type_v01), pbm_capability_basic_info_array_elem_type_data_v01},
  {sizeof(pbm_capability_basic_info_array_type_v01), pbm_capability_basic_info_array_type_data_v01},
  {sizeof(pbm_group_capability_with_session_type_v01), pbm_group_capability_with_session_type_data_v01},
  {sizeof(pbm_ad_num_capability_with_session_type_v01), pbm_ad_num_capability_with_session_type_data_v01},
  {sizeof(pbm_email_capability_with_session_type_v01), pbm_email_capability_with_session_type_data_v01},
  {sizeof(pbm_second_name_capability_with_session_type_v01), pbm_second_name_capability_with_session_type_data_v01},
  {sizeof(pbm_hidden_records_capability_with_session_type_v01), pbm_hidden_records_capability_with_session_type_data_v01},
  {sizeof(pbm_gas_capability_with_session_type_v01), pbm_gas_capability_with_session_type_data_v01},
  {sizeof(pbm_aas_capability_with_session_type_v01), pbm_aas_capability_with_session_type_data_v01},
  {sizeof(pbm_record_info_type_v01), pbm_record_info_type_data_v01},
  {sizeof(pbm_record_information_type_v01), pbm_record_information_type_data_v01},
  {sizeof(pbm_ad_num_info_type_v01), pbm_ad_num_info_type_data_v01},
  {sizeof(pbm_email_info_type_v01), pbm_email_info_type_data_v01},
  {sizeof(pbm_num_info_type_v01), pbm_num_info_type_data_v01},
  {sizeof(pbm_email_info_ext_type_v01), pbm_email_info_ext_type_data_v01},
  {sizeof(pbm_base_record_info_type_v01), pbm_base_record_info_type_data_v01},
  {sizeof(pbm_record_id_type_v01), pbm_record_id_type_data_v01},
  {sizeof(pbm_record_update_info_type_v01), pbm_record_update_info_type_data_v01},
  {sizeof(pbm_refresh_info_type_v01), pbm_refresh_info_type_data_v01},
  {sizeof(pbm_emer_num_type_v01), pbm_emer_num_type_data_v01},
  {sizeof(pbm_ecc_emer_num_type_v01), pbm_ecc_emer_num_type_data_v01},
  {sizeof(pbm_card_emer_num_type_v01), pbm_card_emer_num_type_data_v01},
  {sizeof(pbm_network_emer_num_type_v01), pbm_network_emer_num_type_data_v01},
  {sizeof(pbm_phonebook_ready_info_type_v01), pbm_phonebook_ready_info_type_data_v01},
  {sizeof(pbm_usim_field_capability_array_elem_type_v01), pbm_usim_field_capability_array_elem_type_data_v01},
  {sizeof(pbm_usim_field_capability_array_type_v01), pbm_usim_field_capability_array_type_data_v01},
  {sizeof(pbm_usim_field_capabilities_type_v01), pbm_usim_field_capabilities_type_data_v01},
  {sizeof(pbm_record_instance_type_v01), pbm_record_instance_type_data_v01},
  {sizeof(pbm_basic_record_data_type_v01), pbm_basic_record_data_type_data_v01},
  {sizeof(pbm_sname_info_type_v01), pbm_sname_info_type_data_v01},
  {sizeof(pbm_ad_num_record_info_type_v01), pbm_ad_num_record_info_type_data_v01},
  {sizeof(pbm_grp_id_record_info_type_v01), pbm_grp_id_record_info_type_data_v01},
  {sizeof(pbm_email_record_info_type_v01), pbm_email_record_info_type_data_v01},
  {sizeof(pbm_hidden_record_info_type_v01), pbm_hidden_record_info_type_data_v01},
  {sizeof(pbm_record_instance_ext_type_v01), pbm_record_instance_ext_type_data_v01},
  {sizeof(pbm_basic_record_data_ext_type_v01), pbm_basic_record_data_ext_type_data_v01},
  {sizeof(pbm_sname_info_ext_type_v01), pbm_sname_info_ext_type_data_v01},
  {sizeof(pbm_email_record_info_ext_type_v01), pbm_email_record_info_ext_type_data_v01},
  {sizeof(pbm_groups_instance_type_v01), pbm_groups_instance_type_data_v01},
  {sizeof(pbm_groups_data_type_v01), pbm_groups_data_type_data_v01},
  {sizeof(pbm_grp_info_type_v01), pbm_grp_info_type_data_v01},
  {sizeof(pbm_grp_id_type_v01), pbm_grp_id_type_data_v01},
  {sizeof(pbm_phonebook_state_type_v01), pbm_phonebook_state_type_data_v01},
  {sizeof(pbm_hidden_status_type_v01), pbm_hidden_status_type_data_v01},
  {sizeof(pbm_aas_instance_type_v01), pbm_aas_instance_type_data_v01},
  {sizeof(pbm_aas_data_type_v01), pbm_aas_data_type_data_v01},
  {sizeof(pbm_aas_info_type_v01), pbm_aas_info_type_data_v01},
  {sizeof(pbm_aas_id_type_v01), pbm_aas_id_type_data_v01},
  {sizeof(pbm_aas_update_info_type_v01), pbm_aas_update_info_type_data_v01},
  {sizeof(pbm_gas_update_info_type_v01), pbm_gas_update_info_type_data_v01},
  {sizeof(pbm_record_instance_undecoded_type_v01), pbm_record_instance_undecoded_type_data_v01},
  {sizeof(pbm_basic_record_data_undecoded_type_v01), pbm_basic_record_data_undecoded_type_data_v01},
  {sizeof(pbm_sname_info_undecoded_type_v01), pbm_sname_info_undecoded_type_data_v01},
  {sizeof(pbm_email_info_undecoded_type_v01), pbm_email_info_undecoded_type_data_v01},
  {sizeof(pbm_email_record_info_undecoded_type_v01), pbm_email_record_info_undecoded_type_data_v01},
  {sizeof(pbm_adn_caps_info_v01), pbm_adn_caps_info_data_v01},
  {sizeof(pbm_record_capability_ext_type_v01), pbm_record_capability_ext_type_data_v01},
  {sizeof(pbm_usim_capability_ext_type_v01), pbm_usim_capability_ext_type_data_v01},
  {sizeof(pbm_pb_set_cap_info_type_v01), pbm_pb_set_cap_info_type_data_v01},
  {sizeof(pbm_pbr_cap_info_type_v01), pbm_pbr_cap_info_type_data_v01},
  {sizeof(pbm_pb_set_cap_info_with_session_type_v01), pbm_pb_set_cap_info_with_session_type_data_v01},
  {sizeof(pbm_mcc_emer_num_type_v01), pbm_mcc_emer_num_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry pbm_message_table_v01[] = {
  {sizeof(pbm_indication_register_req_msg_v01), pbm_indication_register_req_msg_data_v01},
  {sizeof(pbm_indication_register_resp_msg_v01), pbm_indication_register_resp_msg_data_v01},
  {sizeof(pbm_get_pb_capabilities_req_msg_v01), pbm_get_pb_capabilities_req_msg_data_v01},
  {sizeof(pbm_get_pb_capabilities_resp_msg_v01), pbm_get_pb_capabilities_resp_msg_data_v01},
  {sizeof(pbm_get_all_pb_capabilities_req_msg_v01), 0},
  {sizeof(pbm_get_all_pb_capabilities_resp_msg_v01), pbm_get_all_pb_capabilities_resp_msg_data_v01},
  {sizeof(pbm_read_pbset_caps_req_msg_v01), pbm_read_pbset_caps_req_msg_data_v01},
  {sizeof(pbm_read_pbset_caps_resp_msg_v01), pbm_read_pbset_caps_resp_msg_data_v01},
  {sizeof(pbm_read_records_req_msg_v01), pbm_read_records_req_msg_data_v01},
  {sizeof(pbm_read_records_resp_msg_v01), pbm_read_records_resp_msg_data_v01},
  {sizeof(pbm_read_records_ext_req_msg_v01), pbm_read_records_ext_req_msg_data_v01},
  {sizeof(pbm_read_records_ext_resp_msg_v01), pbm_read_records_ext_resp_msg_data_v01},
  {sizeof(pbm_write_record_req_msg_v01), pbm_write_record_req_msg_data_v01},
  {sizeof(pbm_write_record_resp_msg_v01), pbm_write_record_resp_msg_data_v01},
  {sizeof(pbm_write_record_ext_req_msg_v01), pbm_write_record_ext_req_msg_data_v01},
  {sizeof(pbm_write_record_ext_resp_msg_v01), pbm_write_record_ext_resp_msg_data_v01},
  {sizeof(pbm_delete_record_req_msg_v01), pbm_delete_record_req_msg_data_v01},
  {sizeof(pbm_delete_record_resp_msg_v01), pbm_delete_record_resp_msg_data_v01},
  {sizeof(pbm_delete_all_pb_records_req_msg_v01), pbm_delete_all_pb_records_req_msg_data_v01},
  {sizeof(pbm_delete_all_pb_records_resp_msg_v01), pbm_delete_all_pb_records_resp_msg_data_v01},
  {sizeof(pbm_search_records_req_msg_v01), pbm_search_records_req_msg_data_v01},
  {sizeof(pbm_search_records_resp_msg_v01), pbm_search_records_resp_msg_data_v01},
  {sizeof(pbm_search_records_ext_req_msg_v01), pbm_search_records_ext_req_msg_data_v01},
  {sizeof(pbm_search_records_ext_resp_msg_v01), pbm_search_records_ext_resp_msg_data_v01},
  {sizeof(pbm_record_update_ind_msg_v01), pbm_record_update_ind_msg_data_v01},
  {sizeof(pbm_refresh_ind_msg_v01), pbm_refresh_ind_msg_data_v01},
  {sizeof(pbm_pb_ready_ind_msg_v01), pbm_pb_ready_ind_msg_data_v01},
  {sizeof(pbm_emergency_list_ind_msg_v01), pbm_emergency_list_ind_msg_data_v01},
  {sizeof(pbm_all_pb_init_done_ind_msg_v01), pbm_all_pb_init_done_ind_msg_data_v01},
  {sizeof(pbm_pbset_cap_read_ind_msg_v01), pbm_pbset_cap_read_ind_msg_data_v01},
  {sizeof(pbm_record_read_ind_msg_v01), pbm_record_read_ind_msg_data_v01},
  {sizeof(pbm_record_read_ext_ind_msg_v01), pbm_record_read_ext_ind_msg_data_v01},
  {sizeof(pbm_get_emergency_list_req_msg_v01), 0},
  {sizeof(pbm_get_emergency_list_resp_msg_v01), pbm_get_emergency_list_resp_msg_data_v01},
  {sizeof(pbm_get_all_groups_req_msg_v01), 0},
  {sizeof(pbm_get_all_groups_resp_msg_v01), pbm_get_all_groups_resp_msg_data_v01},
  {sizeof(pbm_set_group_info_req_msg_v01), pbm_set_group_info_req_msg_data_v01},
  {sizeof(pbm_set_group_info_resp_msg_v01), pbm_set_group_info_resp_msg_data_v01},
  {sizeof(pbm_get_pb_state_req_msg_v01), pbm_get_pb_state_req_msg_data_v01},
  {sizeof(pbm_get_pb_state_resp_msg_v01), pbm_get_pb_state_resp_msg_data_v01},
  {sizeof(pbm_read_all_hidden_records_req_msg_v01), pbm_read_all_hidden_records_req_msg_data_v01},
  {sizeof(pbm_read_all_hidden_records_resp_msg_v01), pbm_read_all_hidden_records_resp_msg_data_v01},
  {sizeof(pbm_read_all_hidden_records_ext_req_msg_v01), pbm_read_all_hidden_records_ext_req_msg_data_v01},
  {sizeof(pbm_read_all_hidden_records_ext_resp_msg_v01), pbm_read_all_hidden_records_ext_resp_msg_data_v01},
  {sizeof(pbm_hidden_record_status_ind_msg_v01), pbm_hidden_record_status_ind_msg_data_v01},
  {sizeof(pbm_get_next_empty_record_id_req_msg_v01), pbm_get_next_empty_record_id_req_msg_data_v01},
  {sizeof(pbm_get_next_empty_record_id_resp_msg_v01), pbm_get_next_empty_record_id_resp_msg_data_v01},
  {sizeof(pbm_get_next_non_empty_record_id_req_msg_v01), pbm_get_next_non_empty_record_id_req_msg_data_v01},
  {sizeof(pbm_get_next_non_empty_record_id_resp_msg_v01), pbm_get_next_non_empty_record_id_resp_msg_data_v01},
  {sizeof(pbm_get_all_aas_req_msg_v01), 0},
  {sizeof(pbm_get_all_aas_resp_msg_v01), pbm_get_all_aas_resp_msg_data_v01},
  {sizeof(pbm_set_aas_req_msg_v01), pbm_set_aas_req_msg_data_v01},
  {sizeof(pbm_set_aas_resp_msg_v01), pbm_set_aas_resp_msg_data_v01},
  {sizeof(pbm_aas_update_ind_msg_v01), pbm_aas_update_ind_msg_data_v01},
  {sizeof(pbm_gas_update_ind_msg_v01), pbm_gas_update_ind_msg_data_v01},
  {sizeof(pbm_bind_subscription_req_msg_v01), pbm_bind_subscription_req_msg_data_v01},
  {sizeof(pbm_bind_subscription_resp_msg_v01), pbm_bind_subscription_resp_msg_data_v01},
  {sizeof(pbm_get_subscription_binding_req_msg_v01), 0},
  {sizeof(pbm_get_subscription_binding_resp_msg_v01), pbm_get_subscription_binding_resp_msg_data_v01},
  {sizeof(pbm_sim_init_done_ind_msg_v01), pbm_sim_init_done_ind_msg_data_v01},
  {sizeof(pbm_read_records_ext_undecoded_req_msg_v01), pbm_read_records_ext_undecoded_req_msg_data_v01},
  {sizeof(pbm_read_records_ext_undecoded_resp_msg_v01), pbm_read_records_ext_undecoded_resp_msg_data_v01},
  {sizeof(pbm_record_read_ext_undecoded_ind_msg_v01), pbm_record_read_ext_undecoded_ind_msg_data_v01},
  {sizeof(pbm_set_configuration_req_msg_v01), pbm_set_configuration_req_msg_data_v01},
  {sizeof(pbm_set_configuration_resp_msg_v01), pbm_set_configuration_resp_msg_data_v01},
  {sizeof(pbm_get_configuration_req_msg_v01), pbm_get_configuration_req_msg_data_v01},
  {sizeof(pbm_get_configuration_resp_msg_v01), pbm_get_configuration_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object pbm_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *pbm_qmi_idl_type_table_object_referenced_tables_v01[] =
{&pbm_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object pbm_qmi_idl_type_table_object_v01 = {
  sizeof(pbm_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(pbm_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  pbm_type_table_v01,
  pbm_message_table_v01,
  pbm_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry pbm_service_command_messages_v01[] = {
  {QMI_PBM_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 0), 7},
  {QMI_PBM_GET_PB_CAPABILITIES_REQ_V01, QMI_IDL_TYPE16(0, 2), 6},
  {QMI_PBM_GET_ALL_PB_CAPABILITIES_REQ_V01, QMI_IDL_TYPE16(0, 4), 0},
  {QMI_PBM_READ_RECORDS_REQ_V01, QMI_IDL_TYPE16(0, 8), 10},
  {QMI_PBM_WRITE_RECORD_REQ_V01, QMI_IDL_TYPE16(0, 12), 1224},
  {QMI_PBM_DELETE_RECORD_REQ_V01, QMI_IDL_TYPE16(0, 16), 8},
  {QMI_PBM_DELETE_ALL_PB_RECORDS_REQ_V01, QMI_IDL_TYPE16(0, 18), 6},
  {QMI_PBM_SEARCH_RECORDS_REQ_V01, QMI_IDL_TYPE16(0, 20), 295},
  {QMI_PBM_GET_EMERGENCY_LIST_REQ_V01, QMI_IDL_TYPE16(0, 32), 0},
  {QMI_PBM_GET_ALL_GROUPS_REQ_V01, QMI_IDL_TYPE16(0, 34), 0},
  {QMI_PBM_SET_GROUP_INFO_REQ_V01, QMI_IDL_TYPE16(0, 36), 248},
  {QMI_PBM_GET_PB_STATE_REQ_V01, QMI_IDL_TYPE16(0, 38), 6},
  {QMI_PBM_READ_ALL_HIDDEN_RECORDS_REQ_V01, QMI_IDL_TYPE16(0, 40), 4},
  {QMI_PBM_GET_NEXT_EMPTY_RECORD_ID_REQ_V01, QMI_IDL_TYPE16(0, 45), 8},
  {QMI_PBM_GET_NEXT_NON_EMPTY_RECORD_ID_REQ_V01, QMI_IDL_TYPE16(0, 47), 8},
  {QMI_PBM_GET_ALL_AAS_REQ_V01, QMI_IDL_TYPE16(0, 49), 0},
  {QMI_PBM_SET_AAS_REQ_V01, QMI_IDL_TYPE16(0, 51), 248},
  {QMI_PBM_BIND_SUBSCRIPTION_REQ_V01, QMI_IDL_TYPE16(0, 55), 4},
  {QMI_PBM_GET_SUBSCRIPTION_BINDING_REQ_V01, QMI_IDL_TYPE16(0, 57), 0},
  {QMI_PBM_READ_PBSET_CAPS_REQ_V01, QMI_IDL_TYPE16(0, 6), 14},
  {QMI_PBM_READ_RECORDS_EXT_REQ_V01, QMI_IDL_TYPE16(0, 10), 10},
  {QMI_PBM_WRITE_RECORD_EXT_REQ_V01, QMI_IDL_TYPE16(0, 14), 2306},
  {QMI_PBM_SEARCH_RECORDS_EXT_REQ_V01, QMI_IDL_TYPE16(0, 22), 564},
  {QMI_PBM_READ_ALL_HIDDEN_RECORDS_EXT_REQ_V01, QMI_IDL_TYPE16(0, 42), 4},
  {QMI_PBM_READ_RECORDS_EXT_UNDECODED_REQ_V01, QMI_IDL_TYPE16(0, 60), 10},
  {QMI_PBM_SET_CONFIGURATION_REQ_V01, QMI_IDL_TYPE16(0, 63), 11},
  {QMI_PBM_GET_CONFIGURATION_REQ_V01, QMI_IDL_TYPE16(0, 65), 11}
};

static const qmi_idl_service_message_table_entry pbm_service_response_messages_v01[] = {
  {QMI_PBM_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 1), 14},
  {QMI_GET_PB_CAPABILITIES_RESP_V01, QMI_IDL_TYPE16(0, 3), 378},
  {QMI_PBM_GET_ALL_PB_CAPABILITIES_RESP_V01, QMI_IDL_TYPE16(0, 5), 2377},
  {QMI_PBM_READ_RECORDS_RESP_V01, QMI_IDL_TYPE16(0, 9), 12},
  {QMI_PBM_WRITE_RECORD_RESP_V01, QMI_IDL_TYPE16(0, 13), 12},
  {QMI_PBM_DELETE_RECORD_RESP_V01, QMI_IDL_TYPE16(0, 17), 12},
  {QMI_PBM_DELETE_ALL_PB_RECORDS_RESP_V01, QMI_IDL_TYPE16(0, 19), 7},
  {QMI_PBM_SEARCH_RECORDS_RESP_V01, QMI_IDL_TYPE16(0, 21), 2012},
  {QMI_PBM_GET_EMERGENCY_LIST_RESP_V01, QMI_IDL_TYPE16(0, 33), 14445},
  {QMI_PBM_GET_ALL_GROUPS_RESP_V01, QMI_IDL_TYPE16(0, 35), 58343},
  {QMI_PBM_SET_GROUP_INFO_RESP_V01, QMI_IDL_TYPE16(0, 37), 12},
  {QMI_PBM_GET_PB_STATE_RESP_V01, QMI_IDL_TYPE16(0, 39), 14},
  {QMI_PBM_READ_ALL_HIDDEN_RECORDS_RESP_V01, QMI_IDL_TYPE16(0, 41), 12},
  {QMI_PBM_GET_NEXT_EMPTY_RECORD_ID_RESP_V01, QMI_IDL_TYPE16(0, 46), 12},
  {QMI_PBM_GET_NEXT_NON_EMPTY_RECORD_ID_RESP_V01, QMI_IDL_TYPE16(0, 48), 12},
  {QMI_PBM_GET_ALL_AAS_RESP_V01, QMI_IDL_TYPE16(0, 50), 58343},
  {QMI_PBM_SET_AAS_RESP_V01, QMI_IDL_TYPE16(0, 52), 12},
  {QMI_PBM_BIND_SUBSCRIPTION_RESP_V01, QMI_IDL_TYPE16(0, 56), 7},
  {QMI_PBM_GET_SUBSCRIPTION_BINDING_RESP_V01, QMI_IDL_TYPE16(0, 58), 11},
  {QMI_PBM_READ_PBSET_CAPS_RESP_V01, QMI_IDL_TYPE16(0, 7), 12},
  {QMI_PBM_READ_RECORDS_EXT_RESP_V01, QMI_IDL_TYPE16(0, 11), 12},
  {QMI_PBM_WRITE_RECORD_EXT_RESP_V01, QMI_IDL_TYPE16(0, 15), 12},
  {QMI_PBM_SEARCH_RECORDS_EXT_RESP_V01, QMI_IDL_TYPE16(0, 23), 2012},
  {QMI_PBM_READ_ALL_HIDDEN_RECORDS_EXT_RESP_V01, QMI_IDL_TYPE16(0, 43), 12},
  {QMI_PBM_READ_RECORDS_EXT_UNDECODED_RESP_V01, QMI_IDL_TYPE16(0, 61), 12},
  {QMI_PBM_SET_CONFIGURATION_RESP_V01, QMI_IDL_TYPE16(0, 64), 7},
  {QMI_PBM_GET_CONFIGURATION_RESP_V01, QMI_IDL_TYPE16(0, 66), 11}
};

static const qmi_idl_service_message_table_entry pbm_service_indication_messages_v01[] = {
  {QMI_PBM_RECORD_READ_IND_V01, QMI_IDL_TYPE16(0, 30), 12159},
  {QMI_PBM_RECORD_UPDATE_IND_V01, QMI_IDL_TYPE16(0, 24), 9},
  {QMI_PBM_REFRESH_IND_V01, QMI_IDL_TYPE16(0, 25), 7},
  {QMI_PBM_PB_READY_IND_V01, QMI_IDL_TYPE16(0, 26), 6},
  {QMI_PBM_EMERGENCY_LIST_IND_V01, QMI_IDL_TYPE16(0, 27), 14438},
  {QMI_PBM_ALL_PB_INIT_DONE_IND_V01, QMI_IDL_TYPE16(0, 28), 22},
  {QMI_PBM_HIDDEN_RECORD_STATUS_IND_V01, QMI_IDL_TYPE16(0, 44), 5},
  {QMI_PBM_AAS_UPDATE_IND_V01, QMI_IDL_TYPE16(0, 53), 762},
  {QMI_PBM_GAS_UPDATE_IND_V01, QMI_IDL_TYPE16(0, 54), 762},
  {QMI_PBM_PBSET_CAP_READ_IND_V01, QMI_IDL_TYPE16(0, 29), 2835},
  {QMI_PBM_RECORD_READ_EXT_IND_V01, QMI_IDL_TYPE16(0, 31), 22919},
  {QMI_PBM_SIM_INIT_DONE_IND_V01, QMI_IDL_TYPE16(0, 59), 7},
  {QMI_PBM_RECORD_READ_EXT_UNDECODED_IND_V01, QMI_IDL_TYPE16(0, 62), 12719}
};

/*Service Object*/
struct qmi_idl_service_object pbm_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x0C,
  58343,
  { sizeof(pbm_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(pbm_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(pbm_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { pbm_service_command_messages_v01, pbm_service_response_messages_v01, pbm_service_indication_messages_v01},
  &pbm_qmi_idl_type_table_object_v01,
  0x13,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type pbm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( PBM_V01_IDL_MAJOR_VERS != idl_maj_version || PBM_V01_IDL_MINOR_VERS != idl_min_version
       || PBM_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&pbm_qmi_idl_service_object_v01;
}

