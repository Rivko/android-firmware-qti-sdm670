#!/usr/bin/env python

# -------------------------------------------------------------------------------
#                          EDIT HISTORY FOR MODULE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
# 07/10/15   vk      Initial version
# --------   ---     -----------------------------------------------------------

import os
import sys
import re
import string

source_file = sys.argv[1]

fp = open(source_file)

while 1:
  line = fp.readline()
  if not line:
    break
  #print line
  line = line.strip()

  if len(line) == 0:
    continue
  
  line=line.replace("\\", "/")

  print "-Xpreprocessor ", line
