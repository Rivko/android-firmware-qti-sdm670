/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
QCRIL_LEGACY_EVENT(HOOK_BASE                                 , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_NV_READ                              , qcril_other_request_oem_hook_nv_read                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_NV_WRITE                             , qcril_other_request_oem_hook_nv_write                  , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_DATA_GO_DORMANT                      , qcril_other_request_oem_hook_go_dormant                , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_ME_DEPERSONALIZATION                 , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_TUNE_AWAY                        , qcril_qmi_nas_dsds_request_set_tune_away               , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_TUNE_AWAY                        , qcril_qmi_nas_dsds_request_get_tune_away               , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_PAGING_PRIORITY                  , qcril_qmi_nas_dsds_request_set_paging_priority         , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_PAGING_PRIORITY                  , qcril_qmi_nas_dsds_request_get_paging_priority         , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_NEIGHBORING_CELLS_INFO           , qcril_other_request_oem_hook_neighboring_cells_info    , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_CDMA_SUB_SRC_WITH_SPC            , qcril_qmi_nas_set_subscription_source_with_spc         , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_DEFAULT_VOICE_SUB                , qcril_qmi_nas_dsds_request_set_default_voice_sub       , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_LOCAL_CALL_HOLD                  , qcril_qmi_voice_request_set_local_call_hold            , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_CDMA_AVOID_CUR_NWK                   , qcril_qmi_nas_cdma_avoid_system                        , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_CDMA_CLEAR_AVOIDANCE_LIST            , qcril_qmi_nas_cdma_clear_avoid_list                    , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_CDMA_GET_AVOIDANCE_LIST              , qcril_qmi_nas_get_cdma_avoid_system_list               , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_BUILTIN_PLMN_LIST                , qcril_qmi_nas_set_builtin_plmn_list                    , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_PERFORM_INCREMENTAL_NW_SCAN          , qcril_qmi_nas_perform_incremental_network_scan         , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_CANCEL_QUERY_AVAILABLE_NETWORK       , qcril_qmi_nas_cancel_query_available_networks          , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_ENABLE_ENGINEER_MODE                 , qcril_qmi_nas_enable_engineer_mode                     , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_REJECT_INCOMING_CALL_CAUSE_21        , qcril_qmi_voice_oem_hook_reject_incoming_call_cause_21 , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_MODEM_TEST_MODE                  , qcril_qmi_pdc_set_modem_test_mode                      , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_QUERY_MODEM_TEST_MODE                , qcril_qmi_pdc_query_modem_test_mode                    , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_AVAILABLE_CONFIGS                , qcril_qmi_pdc_get_available_configs                    , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_CSG_ID                           , qcril_qmi_nas_get_csg_id_handler                       , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_RAT_RAC                          , qcril_qmi_nas_get_rat_rac_handler                      , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_ATEL_UI_STATUS                   , qcril_qmi_nas_oem_hook_set_atel_ui_status_to_modem     , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_PREFERRED_NETWORK_ACQ_ORDER      , qcril_qmi_nas_request_set_preferred_network_acq_order  , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_PREFERRED_NETWORK_ACQ_ORDER      , qcril_qmi_nas_request_get_preferred_network_acq_order  , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_CURRENT_SETUP_CALLS              , qcril_qmi_voice_get_current_setup_calls                , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_REQUEST_SETUP_ANSWER                 , qcril_qmi_voice_request_setup_answer                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_CLEANUP_LOADED_CONFIGS               , qcril_qmi_pdc_cleanup_loaded_configs                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SEL_CONFIG                           , qcril_qmi_pdc_select_configs                           , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_META_INFO                        , qcril_qmi_pdc_get_meta_info_of_config                  , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_MODEM_CAPABILITY                 , qcril_qmi_nas_get_modem_capability                     , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UPDATE_SUB_BINDING                   , qcril_qmi_nas_update_sub_binding                       , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_DATA_SUBSCRIPTION                , qcril_qmi_nas_dsds_request_set_data_subscription       , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_PREFERRED_NETWORK_BAND_PREF      , qcril_qmi_nas_request_set_preferred_network_band_pref  , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_PREFERRED_NETWORK_BAND_PREF      , qcril_qmi_nas_request_get_preferred_network_band_pref  , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_IS_DATA_ENABLED                  , qcril_qmi_nas_set_is_data_enabled                      , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_IS_DATA_ROAMING_ENABLED          , qcril_qmi_nas_set_is_data_roaming_enabled              , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_APN_INFO                         , qcril_qmi_nas_set_apn_info                             , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_LTE_TUNE_AWAY                    , qcril_qmi_nas_dsds_request_set_lte_tune_away           , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_DEACTIVATE_CONFIGS                   , qcril_qmi_pdc_deactivate_configs                       , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#ifndef QMI_RIL_UTF
QCRIL_LEGACY_EVENT(HOOK_GET_QC_VERSION_OF_FILE               , qcril_qmi_pdc_get_qc_version_of_file                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#endif
QCRIL_LEGACY_EVENT(HOOK_VALIDATE_CONFIG                      , qcril_qmi_pdc_validate_config                          , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#ifndef QMI_RIL_UTF
QCRIL_LEGACY_EVENT(HOOK_GET_QC_VERSION_OF_CONFIGID           , qcril_qmi_pdc_get_qc_version_of_configid               , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_OEM_VERSION_OF_FILE              , qcril_qmi_pdc_get_oem_version_of_file                  , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#endif
QCRIL_LEGACY_EVENT(HOOK_GET_OEM_VERSION_OF_CONFIGID          , qcril_qmi_pdc_get_oem_version_of_configid              , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_ACTIVATE_CONFIGS                     , qcril_qmi_pdc_activate_configs                         , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_REQ_GENERIC                          , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_PROTOBUF_MSG                         , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_LTE_DIRECT_DISC_REQ                  , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_LTE_DIRECT_DISC                , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_CSG_PERFORM_NW_SCAN                  , qcril_qmi_nas_csg_handle_oem_hook_perform_network_scan , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_CSG_SET_SYS_SEL_PREF                 , qcril_qmi_nas_csg_handle_oem_hook_set_sys_selection    , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_CSG_GET_SYS_INFO                     , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_SAR_REV_KEY                      , qcril_qmi_nas2_get_sar_rev_key                         , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_TRANSMIT_POWER                   , qcril_qmi_nas2_set_max_transmit_power                  , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_DATA_CALL_PROFILE                , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UICC_VOLTAGE_STATUS_REQ              , qcril_uim_request_oem_hook_voltage_status              , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UICC_POWER_REQ, qcril_uim_request_oem_hook_uicc_power, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_PERSONALIZATION_REACTIVATE_REQ, qcril_uim_request_oem_hook_perso_reactivate, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_PERSONALIZATION_STATUS_REQ, qcril_uim_request_oem_hook_perso_status, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_ENTER_DEPERSONALIZATION_REQ, qcril_uim_request_oem_hook_enter_perso_key, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SLOT_GET_CARD_STATE_REQ, qcril_uim_request_oem_hook_get_sim_status, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SLOT_GET_ATR_REQ, qcril_uim_request_oem_hook_get_atr, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SLOT_SEND_APDU_REQ, qcril_uim_request_oem_hook_send_apdu, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SWITCH_SLOT_REQ, qcril_uim_request_oem_hook_switch_slot, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_PHYSICAL_SLOT_MAPPING_REQ, qcril_uim_request_oem_hook_get_physical_slot_mapping, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SLOT_CARD_POWER_REQ, qcril_uim_request_oem_hook_slot_card_power, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SLOT_SET_APDU_BEHAVIOR_REQ, qcril_uim_request_oem_hook_set_apdu_behavior, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ, qcril_uim_request_oem_hook_get_terminal_capability, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ, qcril_uim_request_oem_hook_set_terminal_capability, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_SLOTS_STATUS_REQ, qcril_uim_request_oem_hook_get_slots_status, &QCRIL_GRP_OEM_HOOK, QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_EXTENDED_DBM_INTL              , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_CDMA_BURST_DTMF                , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_CDMA_CONT_DTMF_START           , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_CDMA_CONT_DTMF_STOP            , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_CALL_EVT_PROGRESS_INFO_IND     , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_NSS_RELEASE                    , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_NEIGHBOR_CELL_INFO_RCVD              , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_EUTRA_STATUS                   , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_LTE_COEX                       , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_WMS_READY                      , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_VOICE_SYSTEM_ID                , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_INCREMENTAL_NW_SCAN_IND        , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_ENGINEER_MODE                  , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_SS_ERROR_CODE                  , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_PDC_CONFIG                     , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_AUDIO_STATE_CHANGED            , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_SIM_REFRESH                    , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_PDC_CLEAR_CONFIGS              , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_WWAN_AVAILABLE                 , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_MODEM_CAPABILITY               , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_UICC_VOLTAGE_STATUS            , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_PDC_VALIDATE_CONFIGS           , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_PDC_VALIDATE_DUMPED            , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_PDC_LIST_CONFIG                , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_SUB_PROVISION_STATUS           , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_SIMLOCK_TEMP_UNLOCK_EXPIRED    , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_CARD_STATE_CHANGE_IND          , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_REMOTE_SIM_STATUS_IND          , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_SLOT_STATUS_CHANGE_IND         , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_CSG_SYS_INFO_IND               , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_CSG_NETWORK_REJ_IND            , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_CSG_ID_CHANGE_IND              , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_RAT_RAC_CHANGE_IND             , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_MAX_DATA_CHANGE_IND            , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_PCSCF_ADDR                     , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_OPERATOR_RESERVED_PCO          , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_GENERIC                        , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_OEM_SOCKET_CONNECTED           , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_VT_BASE                              , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_VT_DIAL_CALL                         , qcril_qmi_ims_vt_dial_call                             , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_VT_END_CALL                          , qcril_qmi_ims_vt_end_call                              , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_VT_ANSWER_CALL                       , qcril_qmi_ims_vt_answer_call                           , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_VT_GET_CALL_INFO                     , qcril_qmi_ims_vt_get_call_info                         , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_VT_UNSOL_CALL_STATUS_IND             , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_PRESENCE_BASE                        , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_ENABLER_STATE_REQ                , qcril_qmi_ims_presence_enabler_state_req               , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_SEND_PUBLISH_REQ                 , qcril_qmi_ims_presence_send_publish_req                , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_SEND_PUBLISH_XML_REQ             , qcril_qmi_ims_presence_send_publish_xml_req            , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_SEND_UNPUBLISH_REQ               , qcril_qmi_ims_presence_send_unpublish_req              , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_SEND_SUBSCRIBE_REQ               , qcril_qmi_ims_presence_send_subscribe_req              , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_SEND_SUBSCRIBE_XML_REQ           , qcril_qmi_ims_presence_send_subscribe_xml_req          , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_SEND_UNSUBSCRIBE_REQ             , qcril_qmi_ims_presence_send_unsubscribe_req            , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_SET_NOTIFY_FMT_REQ_V01           , qcril_qmi_ims_presence_set_notify_fmt_req              , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_GET_NOTIFY_FMT_REQ_V01           , qcril_qmi_ims_presence_get_notify_fmt_req              , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_SET_EVENT_REPORT_REQ_V01         , qcril_qmi_ims_presence_set_event_report_req            , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_GET_EVENT_REPORT_REQ_V01         , qcril_qmi_ims_presence_get_event_report_req            , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_PUBLISH_TRIGGER_IND_V01          , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_NOTIFY_XML_IND_V01               , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_NOTIFY_IND_V01                   , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_IMS_ENABLER_STATUS_IND               , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_SUB_PROVISION_PREFERENCE_REQ     , qcril_qmi_prov_get_sub_prov_pref_req_handler           , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_SUB_PROVISION_PREFERENCE_REQ     , qcril_qmi_prov_set_sub_prov_pref_req_handler           , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_SIM_ICCID_REQ                    , qcril_qmi_prov_get_sim_iccid_req_handler               , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_MAX_DATA_ALLOWED_REQ             , qcril_qmi_nas_get_max_data_handler                     , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ, qcril_qmi_nas_get_l_plus_l_feature_support_status    , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_BASE                           , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_ENABLE                         , qcril_qmi_nas_embms_requst_enable    , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_DISABLE                        , qcril_qmi_nas_embms_requst_disable   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
// DATA REFACTOR TODO: Enable after data module integration is complete
#ifndef QMI_RIL_UTF
QCRIL_LEGACY_EVENT(HOOK_EMBMS_ACTIVATE_TMGI                  , qcril_data_embms_activate_tmgi       , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_DEACTIVATE_TMGI                , qcril_data_embms_deactivate_tmgi    , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#endif
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_RSSI_IND                 , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#ifndef QMI_RIL_UTF
QCRIL_LEGACY_EVENT(HOOK_EMBMS_GET_AVAILABLE_TMGI             , qcril_data_embms_get_available_tmgi  , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_GET_ACTIVE_TMGI                , qcril_data_embms_get_active_tmgi            , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#endif
QCRIL_LEGACY_EVENT(HOOK_EMBMS_ENABLE_RSSI                    , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_DISABLE_RSSI                   , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_GET_COVERAGE_STATE             , qcril_qmi_nas_embms_request_get_coverage_state         , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_GET_RSSI                       , qcril_qmi_nas_embms_get_rssi                           , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_GET_EMBMS_SVC_STATE            , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_SVC_STATE                , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_ACTIVE_TMGI              , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_COVERAGE                 , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_AVAILABLE_TMGI           , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_OSS_WARNING              , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#ifndef QMI_RIL_UTF
QCRIL_LEGACY_EVENT(HOOK_EMBMS_ACTIVATE_DEACTIVATE_TMGI       , qcril_data_embms_activate_deactivate_tmgi, &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#endif
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_CELL_INFO_CHANGED        , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_RADIO_STATE_CHANGED      , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_SAI_LIST                 , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_SET_SNTP_TIME                  , qcril_qmi_nas_embms_set_sntp_time                      , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_GET_SIB16_COVERAGE             , qcril_qmi_nas_embms_get_sib16_coverage                 , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_SIB16_COVERAGE           , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_GET_UTC_TIME                   , qcril_qmi_nas_embms_get_utc_time                       , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_GET_ACTIVE_LOG_PACKET_IDS      , qcril_qmi_nas_embms_get_active_log_packet_ids          , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_DELIVER_LOG_PACKET             , qcril_qmi_nas_embms_deliver_log_packet                 , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_GET_E911_STATE                 , qcril_qmi_nas_embms_get_e911_state                     , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_E911_STATE_CHANGED       , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#ifndef QMI_RIL_UTF
QCRIL_LEGACY_EVENT(HOOK_EMBMS_CONTENT_DESC_UPDATE            , qcril_data_embms_content_desc_update        , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#endif
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_CONTENT_DESC_CONTROL     , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_GET_SIB_PLMN                   , qcril_qmi_nas_embms_get_sib_plmn                       , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_EMBMS_STATUS             , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_GET_EMBMS_STATUS               , qcril_qmi_nas_embms_get_embms_status                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_EMBMS_UNSOL_GET_INTERESTED_TMGI_LIST , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#ifndef QMI_RIL_UTF
QCRIL_LEGACY_EVENT(HOOK_EMBMS_SEND_INTERESTED_TMGI_LIST      , qcril_data_embms_send_interested_list       , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
#endif
QCRIL_LEGACY_EVENT(HOOK_EMBMS_MAX                            , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_RFPE_BASE                            , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_RFM_SCENARIO_REQ                 , qcril_qmi_nas_set_rfm_scenario_req                     , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_RFM_SCENARIO_REQ                 , qcril_qmi_nas_get_rfm_scenario_req                     , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_PROVISIONED_TABLE_REVISION_REQ   , qcril_qmi_nas_get_provisioned_table_revision_req       , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_SET_LOCAL_CALL_HOLD_v01              , qcril_qmi_voice_request_set_local_call_hold            , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_GET_ADN_RECORD                       , qcril_qmi_pbm_get_adn_record_hndlr                     , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UPDATE_ADN_RECORD                    , qcril_qmi_pbm_update_adn_record_hndlr                  , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_ADN_INIT_DONE                  , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_ADN_RECORDS_IND                , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_UNSOL_SPEECH_CODEC_INFO              , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
QCRIL_LEGACY_EVENT(HOOK_MAX                                  , NULL                                                   , &QCRIL_GRP_OEM_HOOK , QMI_RIL_ALLOWED_IN_ALL_STATES);
