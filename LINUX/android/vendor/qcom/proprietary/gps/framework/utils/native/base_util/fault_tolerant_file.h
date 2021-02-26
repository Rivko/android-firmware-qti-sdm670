/*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*

 Digest related routines

 GENERAL DESCRIPTION
 This component implements digest for consitency checks

 Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =======================================================================*/

#ifndef __QC_LOC_FW_FAULT_TOLERANT_FILE_H__
#define __QC_LOC_FW_FAULT_TOLERANT_FILE_H__

#include <base_util/log.h>
#include <base_util/list.h>
#include <base_util/time_routines.h>
#include <openssl/evp.h>

#define TEMPORARY_FILE_EXTN ".tmp"
#define MAX_FILE_NAME_SIZE 2048
// MAX_ALLOWED_FILE_NAME_SIZE=MAX_FILE_NAME_SIZE-sizeof(TEMPORARY_FILE_EXTN)
#define MAX_ALLOWED_FILE_NAME_SIZE 2044

namespace qc_loc_fw
{

int getFileNamesInFolder(const char *const in_folder_full_path,
                         List<const char *> *const in_match_extn_list,
                         List<const char *> *const out_name_list);

class FaultTolerantFile
{
public:
  FaultTolerantFile(const char *full_path_file_name_with_extn);
  virtual ~FaultTolerantFile();
  int ftf_read_and_load_to_buffer(unsigned char **buf_ptr,
                                  unsigned int& length,
                                  Timestamp *last_modified_time = NULL);
  int ftf_save_from_buffer(unsigned char *buf, const size_t length);
  // make this static util routine
  static int calculate_digest_evp(unsigned char *buf,
                                  int buf_size,
                                  unsigned char *digest);

private:
  enum FILE_STATUS
  {
    FS_EXISTS = 0,
    FS_NOT_EXIST,
    FS_OPEN_NOT_ALLOWED,
    FS_GOOD
  };

  char m_full_path_file_name_with_extn[MAX_ALLOWED_FILE_NAME_SIZE];
  char m_full_path_tmp_file_name_with_extn[MAX_FILE_NAME_SIZE];

  bool check_if_content_is_good(unsigned char *buf, int length);

  int get_file_last_modified_time(const char *const file_name,
                                  time_t& modified_time);
  int file_write(const char *const file_name, unsigned char *buf,
                 unsigned int length);
  void file_read_and_validate(const char *const file_name,
                              FILE_STATUS& out_file_status,
                              unsigned int& out_file_length,
                              Timestamp& out_file_last_modified_time,
                              unsigned char *& out_file_content);
  void file_remove(const char *const file_name, FILE_STATUS& status);
  void delete_older_file(const char *const file_1_name,
                         FILE_STATUS& file_1_status,
                         const char *const file_2_name,
                         FILE_STATUS& file_2_status);

  void read_from_opened_file(FILE *file, unsigned char **buf,
                             unsigned int& length);
};

} // namespace qc_loc_fw

#endif //#ifndef __QC_LOC_FW_FAULT_TOLERANT_FILE_H__
