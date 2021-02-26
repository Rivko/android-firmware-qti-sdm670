/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  @file sensor_descriptor_pool.cpp
  @brief
  sensor_descriptor_pool definition.
============================================================================*/

#include "sensor_descriptor_pool.h"
#include "logging_util.h"
#include <dirent.h>

void add_to_proto_vector(FILE* &proto_file, char* file,
    vector<FileDescriptorProto>& vfdp);
void sort_proto_vector(vector<FileDescriptorProto> &vfdp,
    vector<FileDescriptorProto>& vfdp_sorted);

static usta_logging_util *log_instance;

// global descriptorpool
DescriptorPool pool;

usta_rc build_descriptor_pool()
{
  vector<FileDescriptorProto> vfdp;
  // vector containing file descriptor proto of each .proto file
  vector<FileDescriptorProto> vfdp_sorted;
  // sorted vector in order of dependency
  bool google_descriptor_proto_found = false;
  log_instance = usta_logging_util::get_instance();
  usta_rc rc = USTA_RC_SUCCESS;
  DIR* FD;
  struct dirent* in_file;
  FILE    *file_strm;
  string path("/etc/sensors/proto/");

  if (NULL == (FD = opendir (path.c_str())))
  {
    ERROR_LOG(log_instance , "Failed to open input directory\n");
    rc = USTA_RC_FAILED;
    return rc;
  }
  while ((in_file = readdir(FD)))
  {
    string file_path;
    if (!strcmp(in_file->d_name,".")||!strcmp(in_file->d_name,"..")) continue;

    file_path = path+in_file->d_name;
    file_strm = fopen(file_path.c_str(), "r");

    DEBUG_LOG(log_instance , " start Building descriptor pool for " + std::string(in_file->d_name));

    // special case for decriptor.proto
    string desc_proto("descriptor.proto");
    if (desc_proto.compare(in_file->d_name)==0)
    {
      google_descriptor_proto_found = true;
      add_to_proto_vector(file_strm, (char *)"google/protobuf/descriptor.proto", vfdp);
    }
    else
    {
      add_to_proto_vector(file_strm, in_file->d_name, vfdp);
    }
  }
  if (!google_descriptor_proto_found)
  {
    ERROR_LOG(log_instance , " Google's descriptor.proto no found");
    return USTA_RC_FAILED;
  }


  for (auto &it : vfdp)
  {
    DEBUG_LOG(log_instance , " proto file is " + it.name());
  }

  sort_proto_vector(vfdp, vfdp_sorted);

  DEBUG_LOG(log_instance , "\n sorted dependecy list are \n");

  for (auto &it : vfdp_sorted)
  {
    DEBUG_LOG(log_instance , " proto file is " + it.name());
  }

  // build descriptor pool using sorted list
  DEBUG_LOG(log_instance , "\nBUILDING DESCRIPTOR POOL\n\n");
  for (auto &it : vfdp_sorted)
  {
    const FileDescriptor* fdesc = pool.BuildFile(it);
    if (fdesc == NULL)
    {
      DEBUG_LOG(log_instance , " error in generating descriptor pool for " + it.name());
    }
  }

  return rc;

}

void add_to_proto_vector(FILE* &proto_file, char* file,
    vector<FileDescriptorProto>& vfdp)
{

  int file_desc = fileno(proto_file);
  FileInputStream proto_input_stream(file_desc);
  Tokenizer tokenizer(&proto_input_stream, NULL);
  FileDescriptorProto file_desc_proto;
  Parser parser;

  if (!parser.Parse(&tokenizer, &file_desc_proto))
  {
    DEBUG_LOG(log_instance , "Cannot parse .proto file: " + std::string(file));
  }
  fclose(proto_file);
  if (!file_desc_proto.has_name())
  {
    file_desc_proto.set_name(file);
  }
  vfdp.push_back(file_desc_proto);
}

bool dependency_present(string s, vector<FileDescriptorProto>& vfdp_sorted)
{
  bool found = false;;
  for (auto &it : vfdp_sorted)
  {
    if (it.name().compare(s) == 0) found = true;
  }

  return found;
}

void sort_proto_vector(vector<FileDescriptorProto> &vfdp,
    vector<FileDescriptorProto>& vfdp_sorted)
{
  vector<FileDescriptorProto>::iterator it_insert;
  vector<FileDescriptorProto>::iterator it;

  bool dependency_met = false;
  while (!vfdp.empty())
  {

    for (it = vfdp.begin(); it != vfdp.end(); ++it)
    {
      dependency_met = true;
      if (it->dependency_size() == 0)
      {
        it_insert = vfdp_sorted.begin();
        break;
      }

      for (int i = 0; i < it->dependency_size(); i++)
      {
        if (!(dependency_present(it->dependency().Get(i), vfdp_sorted)))
        {
          dependency_met = false;
          break;
        }
      }
      if (dependency_met)
      {
        it_insert = vfdp_sorted.end();
        break;
      }
    }

    if (dependency_met)
    {
      vfdp_sorted.insert(it_insert, *it);
      vfdp.erase(it);
    }
  }
}
