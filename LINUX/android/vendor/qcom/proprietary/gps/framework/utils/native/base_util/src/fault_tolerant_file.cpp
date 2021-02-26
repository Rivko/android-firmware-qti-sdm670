/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 Time related routines

 GENERAL DESCRIPTION
 This component provides utilities used for digest for consitency checks

 Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 =============================================================================*/
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#include <base_util/fault_tolerant_file.h>
#include <base_util/log.h>
#include <base_util/vector.h>
#include <base_util/string_routines.h>

namespace qc_loc_fw
{
static const char *const TAG = "FTFile";

// Function name: getFileNamesInFolder
// input:         in_folder_full_path: folder name in which search need
//                                     to be done
//                in_match_extn_list: file extentions that need to be
//                                    matched. set NULL if no preference
//                out_name_list(out param): names of files
// output:        0 if success
// Description:   This function will return the names of files inside a
//                inside a folder
int getFileNamesInFolder(const char *const in_folder_full_path,
                         List<const char *> *const in_match_extn_list,
                         List<const char *> *const out_name_list)
{
  int result = 1;
  do
  {
    if (NULL == in_folder_full_path)
    {
      result = -1;
      break;
    }
    if (NULL == out_name_list)
    {
      result = -2;
      break;
    }

    DIR *pDir = opendir(in_folder_full_path);
    if (NULL == pDir)
    {
      result = -3;
      break;
    }

    size_t path_length = strlen(in_folder_full_path);

    char *full_path = 0;
    dirent *pDirEntry = 0;
    result = 100;
    while ((pDirEntry = readdir(pDir)) != NULL)
    {
      // note length of name is not part of dirent according to POSIX
      const size_t name_length = strlen(pDirEntry->d_name);

      if (NULL != in_match_extn_list && in_match_extn_list->getSize() > 0)
      {

        bool if_of_interested_extn = false;
        for (List<const char *>::Iterator it = in_match_extn_list->begin();
             it != in_match_extn_list->end(); ++it)
        {
          const size_t ext_length = strlen(*it);
          if (0 == strncmp(pDirEntry->d_name + (name_length - ext_length),
                           *it, ext_length))
          {
            if_of_interested_extn = true;
            break;
          }

        }

        if (false == if_of_interested_extn)
        {
          // skip irrelevant file
          continue;
        }
      }

      full_path = new(std::nothrow) char[path_length + name_length + 2];
      if (NULL == full_path)
      {
        result = -101;
        break;
      }

      memcpy(full_path, in_folder_full_path, path_length);
      memcpy(full_path + path_length + 1, pDirEntry->d_name, name_length);
      full_path[path_length] = '/';
      full_path[path_length + name_length + 1] = 0;

      log_info(TAG, "%s:found file name [%s]", __FUNCTION__, full_path);


      if (0 != out_name_list->add(full_path))
      {
        result = -102;
        break;
      }
      // full_path is now owned by the list
      full_path = 0;
    }

    delete[] full_path;
    full_path = 0;
    (void)closedir(pDir);

    if (100 != result)
    {
      break;
    }
    result = 0;
  } while (0);

  if (0 != result)
  {
    log_error(TAG, "%s: result: %d", __FUNCTION__, result);
  }
  return result;
}

// Function name: calculate_digest_evp
// input:         buf: buffere on which digest need to be calculated
//                buf_size: buffer size
//                digest(out param): pointer to hold the calculated digest
// output:        0 if success
// Description:   This function will be called to calculate fixed
//                size digest depneding on buffer
int FaultTolerantFile::calculate_digest_evp(unsigned char *buf,
                                            int buf_size,
                                            unsigned char *digest)
{
  int result = -1;
  int rc = 0;
  EVP_MD_CTX *pctx = NULL;

  do
  {
    pctx = EVP_MD_CTX_create();
    if (NULL == pctx)
    {
      log_error(TAG, "%s: ctx allocation failed", __FUNCTION__);
      break;
    }

    rc = EVP_DigestInit_ex(pctx, EVP_sha1(), NULL);
    if (rc != 1)
    {
      log_error(TAG, "%s: EVP_DigestInit_ex failed", __FUNCTION__);
      break;
    }

    rc = EVP_DigestUpdate(pctx, buf, buf_size);
    if (rc != 1)
    {
      log_error(TAG, "%s: EVP_DigestUpdate failed", __FUNCTION__);
      break;
    }

    memset(digest, 0, EVP_MAX_MD_SIZE);
    rc = EVP_DigestFinal_ex(pctx, digest, NULL);
    if (rc != 1)
    {
      log_error(TAG, "%s: EVP_DigestFinal_ex failed", __FUNCTION__);
      break;
    }

    result = 0;
  } while (0);

  if (NULL != pctx)
  {
    EVP_MD_CTX_destroy(pctx);
  }

  return result;
}

// Function name: FaultTolerantFile
// input:         full_path_file_name_with_extn: full path name of file
// Description:   set file name of file. the object created shall be
//                inited before using
FaultTolerantFile::FaultTolerantFile(const char *full_path_file_name_with_extn)
{
  do
  {
    if (NULL == full_path_file_name_with_extn)
    {
      log_error(TAG, "FaultTolerantFile: NULL File Name");
      break;
    }
    memset(this->m_full_path_file_name_with_extn, '0',
           sizeof(this->m_full_path_file_name_with_extn));
    size_t copysize = strlcpy(this->m_full_path_file_name_with_extn,
                           full_path_file_name_with_extn,
                           sizeof(this->m_full_path_file_name_with_extn));
    if(copysize >= sizeof(this->m_full_path_file_name_with_extn))
    {
      log_error(TAG, "FaultTolerantFile filename input exceeds "
                     "maximum allowed filename size[%d]. ignoring",
                (int)sizeof(this->m_full_path_file_name_with_extn));
      this->m_full_path_file_name_with_extn[0] = '\0';
      break;
    }

    char full_path_buf[MAX_FILE_NAME_SIZE] = {0};
    memset(this->m_full_path_tmp_file_name_with_extn, '0',
           sizeof(this->m_full_path_tmp_file_name_with_extn));
    // we are ignoring return values as we are aware that
    // original file name is of correct size and the extn
    // we add is always same
    (void)snprintf(full_path_buf, sizeof(full_path_buf), "%s%s",
                   this->m_full_path_file_name_with_extn,
                   TEMPORARY_FILE_EXTN);
    (void) strlcpy(this->m_full_path_tmp_file_name_with_extn,
                   full_path_buf,
                   sizeof(this->m_full_path_tmp_file_name_with_extn));
  } while(0);
}

FaultTolerantFile::~FaultTolerantFile()
{
}

// Function name: file_read_and_validate
// input:         file_name: full path name of file
//                out_file_status(out param): status of file
//                out_file_length(out param): length of file
//                out_file_content(out param): content of file
//                out_file_last_modified_time(out param): file modification time
// Description:   read the file. and see if file is valid. set file status
void FaultTolerantFile::file_read_and_validate(const char *const file_name,
                                               FILE_STATUS& out_file_status,
                                               unsigned int& out_file_length,
                                               Timestamp& out_file_last_modified_time,
                                               unsigned char* &out_file_content)
{
  FILE* file_ptr;
  unsigned char* all_file_data = NULL;
  unsigned int all_file_length = 0;
  do
  {
    out_file_length = 0;
    file_ptr = fopen(file_name, "r");
    if(file_ptr == NULL)
    {
      if (errno != ENOENT)
      {
        log_error(TAG, "%s: file %s open failed, errno = %d\n",
                  __FUNCTION__, file_name, errno);
        out_file_status = FS_OPEN_NOT_ALLOWED;
      }
      else
      {
        log_debug(TAG, "%s: file %s not exist",
                  __FUNCTION__, file_name);
        out_file_status = FS_NOT_EXIST;
      }
      break;
    }
    out_file_status = FS_EXISTS;
    errno = 0;
    int file_desc = fileno(file_ptr);
    struct stat st;
    if (0 == errno && 0 == fstat(file_desc, &st))
    {
      // we use 'modify time' in the hope that it's available on all file systems
      timespec mtimespec;
      mtimespec.tv_nsec = 0;
      mtimespec.tv_sec  = st.st_mtime;
      Timestamp mtimestamp(CLOCK_REALTIME, mtimespec);
      out_file_last_modified_time = mtimestamp;
    }
    read_from_opened_file(file_ptr, &all_file_data, all_file_length);
    if (all_file_length > 0)
    {
      if(check_if_content_is_good(all_file_data, all_file_length))
      {
        out_file_length = all_file_length - EVP_MAX_MD_SIZE;
        log_debug(TAG, "%s, allocated size %d", __FUNCTION__,
                  out_file_length);

        out_file_content = new unsigned char[out_file_length];
        if (out_file_content == NULL)
        {
          log_error (TAG, "%s, Memory allocation failure", __FUNCTION__);
          out_file_status = FS_NOT_EXIST;
          break;
        }
        memcpy(out_file_content, all_file_data + EVP_MAX_MD_SIZE,
               out_file_length);
        out_file_status = FS_GOOD;
        break;
      }
      log_error (TAG, "%s, bad file %s", __FUNCTION__, file_name);
      fclose(file_ptr);
      file_ptr =NULL;
      int rc = remove (file_name);
      if(rc != 0)
      {
        log_error (TAG, "%s, failed to remove bad file %s error = %d\n",
                   __FUNCTION__, file_name, errno);
        // even if we couldn't remove
        // the file when we open it in write mode contents will be
        // truncated
      }
    }
    out_file_status = FS_NOT_EXIST;
  } while(0);
  if (NULL != file_ptr)
  {
    fclose(file_ptr);
    file_ptr =NULL;
  }
  if (NULL != all_file_data)
  {
    delete[] all_file_data;
  }
}

// Function name: file_remove
// input:         file_name: full path name of file
//                status(in and out param): status of file
// Description:   remove the file.  set file status
void FaultTolerantFile::file_remove(const char *const file_name,
                                    FILE_STATUS &status)
{
  int rc = remove (file_name);
  if(rc != 0)
  {
    log_error (TAG, "%s, failed to remove file %s error = %d\n",
               __FUNCTION__, file_name, errno);
  }
  status = FS_NOT_EXIST;
}

// Function name: delete_older_file
// input:         file_1_name, file_2_name: full path name of file
//                file_1_status, file_2_status(in and out param): status of file
// Description:   remove the file whose last modification time is older
void FaultTolerantFile::delete_older_file(const char *const file_1_name,
                                          FILE_STATUS &file_1_status,
                                          const char* const file_2_name,
                                          FILE_STATUS &file_2_status)
{
  time_t file_1_updated_time;
  time_t file_2_updated_time;

  int rc_1 = get_file_last_modified_time( file_1_name, file_1_updated_time);
  int rc_2 = get_file_last_modified_time( file_2_name, file_2_updated_time);

  if (0 != rc_1)
  {
    // we are not able to obtain last modified time of
    // file 1. Lets delete it
    file_remove(file_1_name, file_1_status);
  }
  else if (0 != rc_2)
  {
    // we are not able to obtain last modified tile of
    // file 2. Lets delete it
    file_remove(file_2_name, file_2_status);
  }
  else // rc_1 && rc_2 == 0
  {
    // find the latest file
    if ((file_1_updated_time - file_2_updated_time) > 0)
    {
      // delete file 2
      file_remove(file_2_name, file_2_status);
    }
    else
    {
      // delete file 1
      file_remove(file_1_name, file_1_status);
    }
  }
}

// Function name: file_write
// input:         file_name: full path name of file
//                buf: buffer to be written to file
//                length: length of buffer
// Description:   write the file to buffer
int FaultTolerantFile::file_write(const char *const file_name,
                                  unsigned char* buf, unsigned int length)
{
  FILE * file = NULL;
  int result = 0;

  do
  {
    // compute the digest
    unsigned char digest[EVP_MAX_MD_SIZE];
    int res = calculate_digest_evp ((unsigned char*) buf, length, digest);
    if (res != 0)
    {
      log_error (TAG, "%s: failed to calculate EVP digest for %s",
                 __FUNCTION__, file_name);
      result = -1;
      break;
    }
    // Open the file, if the file already exists, the file content will be replaced
    // with new content
    file = fopen(file_name, "w");
    if(file == NULL)
    {
      log_error(TAG, "%s: fopen failed, for %s errno = %d",
                __FUNCTION__, file_name, errno);
      result = -2;
      break;
    }

    // Write digest data into the file
    int rc =  fwrite(digest, 1, EVP_MAX_MD_SIZE, file);
    if(rc != (int) EVP_MAX_MD_SIZE)
    {
      log_error(TAG, "%s: failed to write digest for %s, errno = "
                     "%d", __FUNCTION__, file_name, errno);
      result = -4;
      break;
    }

    // go to end of file
    if(0 != fseek (file, 0, SEEK_END))
    {
      log_error(TAG, "%s: fseek failed for %s, errno = %d",
                __FUNCTION__, file_name, errno);
      result = -3;
      break;
    }

    // Write CS data into the file
    rc =  fwrite(buf, 1, length, file);
    if(rc != (int) length)
    {
      log_error(TAG, "%s: failed to write data in %s, errno = "
                     "%d", __FUNCTION__, file_name, errno);
      result = -5;
      break;
    }
    else
    {
      log_debug(TAG, "%s: success", __FUNCTION__);
    }
  } while (0);

  if(file != NULL)
  {
    (void) fclose(file);
    file = NULL;
  }
  return result;
}

// Function name: ftf_save_from_buffer
// Input:         buf: buffer to be written to file
//                length: length of buffer
// Description:   write the file to buffer.
//       1st file is written to a temp file.
//       only if writing to temp file is success and have not corrupted
//       content, we will write to original file. if writing to original
//       file is success and have not corrupted content, we will delete
//       temp file
int FaultTolerantFile::ftf_save_from_buffer(unsigned char *buf,
                                            const size_t length)
{
  FILE_STATUS original_file_status = FS_NOT_EXIST;
  FILE_STATUS temp_file_status = FS_NOT_EXIST;
  unsigned char* original_buf = NULL;
  unsigned int original_length = 0;
  Timestamp original_last_modified_time(false);
  unsigned char* temp_buf = NULL;
  unsigned int temp_length  = 0;
  Timestamp temp_last_modified_time(false);
  int result = -1;

  do
  {
    if (buf == NULL)
    {
      log_error(TAG, "%s: buf null", __FUNCTION__);
      result = -2;
      break;
    }
    if (m_full_path_file_name_with_extn[0] == '\0')
    {
      log_error(TAG, "%s: file name null",  __FUNCTION__);
      result = -3;
      break;
    }
    log_verbose(TAG, "%s: writing buffer of length %d", __FUNCTION__,
                (int)length);
    if(0 != file_write(m_full_path_tmp_file_name_with_extn, buf, length))
    {
      file_remove(m_full_path_tmp_file_name_with_extn, temp_file_status);
      log_error(TAG, "%s: saving to temp file failed",  __FUNCTION__);
      result = -4;
      break;
    }
    else
    {
      log_verbose(TAG, "%s: save to temp file succeeded",  __FUNCTION__);
    }
    file_read_and_validate(m_full_path_tmp_file_name_with_extn,
                           temp_file_status, temp_length,
                           temp_last_modified_time, temp_buf);
    if (FS_GOOD != temp_file_status)
    {
      file_remove(m_full_path_tmp_file_name_with_extn, temp_file_status);
      log_error(TAG, "%s: saving to temp file corrupted",  __FUNCTION__);
      result = -5;
      break;
    }
    if(0 != file_write(m_full_path_file_name_with_extn, temp_buf,
                       temp_length))
    {
      file_remove(m_full_path_file_name_with_extn, original_file_status);
      log_error(TAG, "%s: saving to file failed. leaving temp file intact",
                 __FUNCTION__);
      result = -6;
      break;
    }
    else
    {
      log_verbose(TAG, "%s: save to original file succeeded",
                   __FUNCTION__);
    }
    file_read_and_validate(m_full_path_file_name_with_extn,
                           original_file_status, original_length,
                           original_last_modified_time, original_buf);
    if (FS_GOOD != original_file_status)
    {
      file_remove(m_full_path_file_name_with_extn, original_file_status);
      log_error(TAG, "%s: saving to file failed. leaving temp file intact",
                 __FUNCTION__);
      result = -7;
      break;
    }
    else
    {
      file_remove(m_full_path_tmp_file_name_with_extn, temp_file_status);
    }
    result = 0;
  } while(0);
  if (NULL != original_buf)
  {
    delete[] original_buf;
  }
  if (NULL != temp_buf)
  {
    delete[] temp_buf;
  }
  return result;
}

// Function name: ftf_read_and_load_to_buffer
// Input:         buf_ptr: buffer to be read from file
//                length: length of buffer
//                last_modified_time: last modified time of file
// Output:        status, 0 if success
// Description:   read the data(without checksum) in file to buffer.
//        we can have the following possibilities
//        1 both temporary file and original file are good.
//        1.1 original file is the latest - delete temporary file
//        1.2 temporary is the latest - copy temporary file contents to
//            original file and delete temporary file
//        2 only original file is good - delete temporary file
//        3 only temporary file is good - copy temporary file contents to
//          original file and delete temporary file
int FaultTolerantFile::ftf_read_and_load_to_buffer(unsigned char **buf_ptr,
                                                   unsigned int& length,
                                                   Timestamp *last_modified_time)
{
  FILE_STATUS original_file_status = FS_EXISTS;
  FILE_STATUS temp_file_status = FS_EXISTS;
  unsigned char* original_buf = NULL;
  unsigned int original_length = 0;
  Timestamp original_last_modified_time(false);
  unsigned char* temp_buf = NULL;
  unsigned int temp_length  = 0;
  Timestamp temp_last_modified_time(false);
  int result = -1;
  do
  {
    if (buf_ptr == NULL)
    {
      log_error(TAG, "%s: buf_ptr null", __FUNCTION__);
      result = -2;
      break;
    }
    if (m_full_path_file_name_with_extn[0] == '\0')
    {
      log_error(TAG, "%s: file name null", __FUNCTION__);
      result = -3;
      break;
    }
    // Open the original file in read only mode
    file_read_and_validate(m_full_path_file_name_with_extn,
                           original_file_status, original_length,
                           original_last_modified_time, original_buf);

    // Open the temp file in read only mode
    file_read_and_validate(m_full_path_tmp_file_name_with_extn,
                           temp_file_status, temp_length,
                           temp_last_modified_time, temp_buf);

    bool should_return_with_null_data = false;
    switch (original_file_status)
    {
      case FS_GOOD:
         switch (temp_file_status)
         {
            case FS_GOOD:
              // both files are good. lets use the latest one.
              delete_older_file(m_full_path_file_name_with_extn,
                                original_file_status,
                                m_full_path_tmp_file_name_with_extn,
                                temp_file_status);

              // after this either original or temporary file will be there
              // or both file wont be there
              if ((FS_NOT_EXIST == original_file_status) &&
                  (FS_NOT_EXIST == temp_file_status))
              {
                should_return_with_null_data = true;
              }
              else if (FS_NOT_EXIST == temp_file_status)
              {
                // only original file is there read it
                // data with checksum is there in original_buf
              }
              else if (FS_NOT_EXIST == original_file_status)
              {
                // only temp file is there

                // save temp file data into original file.
                if (NULL != original_buf)
                {
                  delete[] original_buf;
                  original_buf = NULL;
                  original_length = 0;
                }
                if(0 != file_write(m_full_path_file_name_with_extn, temp_buf,
                                   temp_length))
                {
                  should_return_with_null_data = true;
                }
                else
                {

                  // check if write is good
                  file_read_and_validate(m_full_path_file_name_with_extn,
                                         original_file_status, original_length,
                                         original_last_modified_time, original_buf);

                  // can not do anything if this write was a failure.
                  // lets start FDAL from scratch
                  if (FS_GOOD != original_file_status)
                  {
                    should_return_with_null_data = true;
                  }
                  // delete temp file
                  file_remove(m_full_path_tmp_file_name_with_extn,
                              temp_file_status);
                }
              }
              else
              {
                // both file still exist? something wrong here.
                // lets anyway read original file
              }
              break;
            case FS_EXISTS:
              // original file good. temp file bad. delete temp file.
              file_remove(m_full_path_tmp_file_name_with_extn,
                          temp_file_status);
              // lets read original file
              break;
            case FS_OPEN_NOT_ALLOWED:
              // original file good. temp file not able to open.
              // delete temp file. lets read original file
              file_remove(m_full_path_tmp_file_name_with_extn,
                          temp_file_status);
            default: //case FS_NOT_EXIST:
              // original file is good. temp file not exist.
              // lets read original file
              break;
         }
         break;
       case FS_EXISTS:
         // original file is bad - delete it
         file_remove(m_full_path_file_name_with_extn,
                     original_file_status);
         switch (temp_file_status)
         {
            case FS_GOOD:
              // temp file is good. copy it to original file and
              // read original file
              // save temp file data into original file.
              if (NULL != original_buf)
              {
                delete[] original_buf;
                original_buf = NULL;
                original_length = 0;
              }

              if(0 != file_write(m_full_path_file_name_with_extn, temp_buf,
                                   temp_length))
              {
                should_return_with_null_data = true;
              }
              else
              {

                // check if write is good
                file_read_and_validate(m_full_path_file_name_with_extn,
                                       original_file_status, original_length,
                                       original_last_modified_time,
                                       original_buf);

                // can not do anything if this write was a failure.
                // lets start FDAL from scratch
                if (FS_GOOD != original_file_status)
                {
                  should_return_with_null_data = true;
                }
                // delete temp file
                file_remove(m_full_path_tmp_file_name_with_extn,
                            temp_file_status);
              }
              break;
            case FS_EXISTS:
              // both files are bad. delete temp file.
              // we can return at this point
              file_remove(m_full_path_tmp_file_name_with_extn,
                          temp_file_status);
              should_return_with_null_data = true;
              break;
            case FS_OPEN_NOT_ALLOWED:
              // original file is bad for sure. we don't know about temp
              // file. lets leave temp file here and return null data
              // what if on next load/boot we succeed in opening this
              // file
              should_return_with_null_data = true;
              break;
            default: //case FS_NOT_EXIST:
              // both files are not there.
              // we can return at this point
              should_return_with_null_data = true;
              break;
         }
         break;
       case FS_OPEN_NOT_ALLOWED:
         switch (temp_file_status)
         {
            case FS_GOOD:
              // temp file is good for sure. we don't know about original
              // file. lets delete original file
              // delete original file
              file_remove(m_full_path_file_name_with_extn,
                          original_file_status);
              // copy temp file to
              // original file and read original file
              if (NULL != original_buf)
              {
                delete[] original_buf;
                original_buf = NULL;
                original_length = 0;
              }

              if(0 != file_write(m_full_path_file_name_with_extn, temp_buf,
                                   temp_length))
              {
                should_return_with_null_data = true;
              }
              else
              {

                // check if write is good
                file_read_and_validate(m_full_path_file_name_with_extn,
                                       original_file_status, original_length,
                                       original_last_modified_time,
                                       original_buf);

                // can not do anything if this write was a failure.
                // lets start FDAL from scratch
                if (FS_GOOD != original_file_status)
                {
                  should_return_with_null_data = true;
                }
                // delete temp file
                file_remove(m_full_path_tmp_file_name_with_extn,
                            temp_file_status);
              }
              break;
            case FS_EXISTS:
              // temp file is bad for sure. we don't know about original
              // file. lets delete temp file and leave original file here
              // and return null data. what if on next load/boot we succeed
              // in opening this file
              file_remove(m_full_path_tmp_file_name_with_extn,
                          temp_file_status);
              should_return_with_null_data = true;
              break;
            case FS_OPEN_NOT_ALLOWED:
              // we don't know about both files. lets leave both files
              // and return null data
              should_return_with_null_data = true;
              break;
            default: //case FS_NOT_EXIST:
              // we don't know about original
              // file. lets leave original file here
              // and return null data. what if on next load/boot we succeed
              // in opening this file
              should_return_with_null_data = true;
              break;
         }
         break;
       default: //case FS_NOT_EXIST:
         switch (temp_file_status)
         {
            case FS_GOOD:
              // temp file is good. copy it to original file and
              // read original file
              if (NULL != original_buf)
              {
                delete[] original_buf;
                original_buf = NULL;
                original_length = 0;
              }

              if(0 != file_write(m_full_path_file_name_with_extn, temp_buf,
                                   temp_length))
              {
                should_return_with_null_data = true;
              }
              else
              {

                // check if write is good
                file_read_and_validate(m_full_path_file_name_with_extn,
                                       original_file_status, original_length,
                                       original_last_modified_time,
                                       original_buf);

                // can not do anything if this write was a failure.
                // lets start FDAL from scratch
                if (FS_GOOD != original_file_status)
                {
                  should_return_with_null_data = true;
                }
                // delete temp file
                file_remove(m_full_path_tmp_file_name_with_extn,
                            temp_file_status);
              }
              break;
            case FS_EXISTS:
              // both files are bad. delete temp file.
              // we can return at this point
              file_remove(m_full_path_tmp_file_name_with_extn,
                          temp_file_status);
              should_return_with_null_data = true;
              break;
            case FS_OPEN_NOT_ALLOWED:
              // original file is not there. we don't know about temp
              // file. lets leave temp file here and return null data
              // what if on next load/boot we succeed in opening this
              // file
              should_return_with_null_data = true;
              break;
            default: //case FS_NOT_EXIST:
              // both files are not there. lets return
              should_return_with_null_data = true;
              break;
         }
         break;
    }

    // After this temporary file would be deleted.
    // we can remove the temp_buf
    if (NULL != temp_buf)
    {
      delete[] temp_buf;
      temp_buf = NULL;
      temp_length = 0;
    }

    if (!should_return_with_null_data)
    {
      // we have checksum in the original_buf which
      // needs to be removed
      length = original_length;
      *buf_ptr = original_buf;
      if (NULL != last_modified_time)
      {
        *last_modified_time = original_last_modified_time;
      }
    }
    else
    {
      if (NULL != original_buf)
      {
        delete[] original_buf;
        original_buf = NULL;
        original_length = 0;
      }
      if (*buf_ptr != NULL)
      {
        *buf_ptr = NULL;
      }
      length = 0;
    }
    result = 0;
  }while(0);

  return result;
}

// Function name: check_if_content_is_good
// Input:         buf: buffer to validate (data + checksum)
//                length: length of buffer
// Output:        status, 0 if success
// Description:   check if the file is corrupted by comparing
//                existing checksum with newly generated checksum
bool FaultTolerantFile::check_if_content_is_good(unsigned char *buf, int length)
{
  unsigned char *file_data = (unsigned char *)buf;
  unsigned char *acual_data = file_data + EVP_MAX_MD_SIZE;
  bool is_good = true;

  unsigned char evp_digest [EVP_MAX_MD_SIZE];
  memset (evp_digest, 0, EVP_MAX_MD_SIZE);
  int dig_res = calculate_digest_evp(acual_data,
                                     length - EVP_MAX_MD_SIZE,
                                     evp_digest);
  if (dig_res != 0)
  {
    log_error (TAG, "%s: failed to calculate evp digest", __FUNCTION__);
    is_good = false;
  }

  if (memcmp (evp_digest, file_data, EVP_MAX_MD_SIZE) == 0)
  {
  }
  else
  {
    log_error (TAG, "%s: digest does not match ", __FUNCTION__);
    is_good = false;
  }
  return is_good;
}

// Function name: get_file_last_modified_time
// Input:         file_name: full path file name
//                modified_time(out param): last modified time of file
// Output:        status, 0 if success
// Description:   get last modified time of file
int FaultTolerantFile::get_file_last_modified_time(
  const char *const file_name, time_t& modified_time)
{
  int result = 0;
  do
  {
    struct stat st;
    if (0 != stat(file_name, &st))
    {
      result = -1;
      break;
    }
    // we use 'modify time' in the hope that it's available on all file systems
    modified_time = st.st_mtime;
  } while (0);
  return result;
}

// Function name: read_from_opened_file
// Input:         file: file pointer
//                buf(out param): file contents
//                length(out param): buffer length
// Description:   read from where file pointer points to a buffer
void FaultTolerantFile::read_from_opened_file(FILE *file, unsigned char **buf,
                                               unsigned int &length)
{
  unsigned char* buf_tmp = NULL;
  int result = -1;

  do
  {
    // Get the file size by seek and tell
    result = fseek (file, 0, SEEK_END);
    if(result != 0)
    {
      log_error(TAG, "%s, fseek failed, errno = %d", __FUNCTION__, errno);
      break;
    }

    int file_size = ftell (file);
    if(file_size < 0)
    {
      log_error(TAG, "%s, ftell failed, errno = %d", __FUNCTION__, errno);
      break;
    }

    if (file_size == 0)
    {
      log_error(TAG, "%s, file size is 0, ignore", __FUNCTION__);
      break;
    }

    // set length to file_size
    length = file_size;

    // Buffer to be freed by caller
    buf_tmp = new unsigned char[length];
    if (buf_tmp == NULL)
    {
      log_error(TAG, "%s, allocating memory failed", __FUNCTION__);
      break;
    }

    // Now reset file offset to beginning and read out the file content
    result = fseek (file, 0, SEEK_SET);
    if (result != 0)
    {
      log_error(TAG, "%s, fseek failed, errno = %d\n", __FUNCTION__,
                errno);
      break;
    }

    size_t read_len = fread (buf_tmp, 1, length, file);
    if (read_len != length)
    {
      log_error(TAG, "%s, fread size does not match, "
                     "read %d bytes, expected %d bytes \n", __FUNCTION__,
                (int)read_len, length);
      break;
    }

    *buf = buf_tmp;
    result = 0;

    result = 0;
  } while (0);

  if (result != 0)
  {
    if (buf_tmp != NULL)
    {
      delete[] buf_tmp;
      buf_tmp = NULL;
      length = 0;
    }

    if (buf != NULL)
    {
      buf = NULL;
      length = 0;
    }
  }
}

} // namespace qc_loc_fw

