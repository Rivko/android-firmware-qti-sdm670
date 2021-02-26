#!/usr/bin/python
# -*- coding: UTF-8 -*-
def remove_json_comments(source_json_file,dest_json_file):
  dest_json = ""
  source_file_handle = open(source_json_file,"r")
#  dest_file_handle = open(dest_json_file,"w")
  for line in source_file_handle:
    if line.lstrip().startswith("#"):
        dest_json += "\n"
        continue
    temp=line.split()
    if len(temp)==0:
        dest_json += "\n"
        continue
    else:
        dest_json += line.split("#")[0].strip("\n")
        dest_json += "\n"
#  dest_file_handle.write(dest_json)
  source_file_handle.close()
#  dest_file_handle.close()
#  print dest_json
  return dest_json
