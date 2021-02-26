#  ===========================================================================
#
#  Copyright (c) 2015 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================

from __future__ import print_function

class PrettyPrint(object):
  def __init__(self):
    self._logfile = ""
    self._tab_count = 0
    self._fill = " "
    self._format = "<"
    self._frame = "#"
    self._line_width = 80
    self._tab = 4
  
  def set_log_output(self, logfile):
    self._logfile = logfile
    
  def set_fill(self, fill):
    self._tab_count = fill
  
  def set_format(self, format):
    self._tab_count = format
  
  def set_frame(self, frame):
    self._tab_count = frame
  
  def set_line_width(self, line_width):
    self._tab_count = line_width
  
  def set_min_tab(self, tab_count):
    self._tab_count = tab_count
  
  def set_tab_size(self, tab_size):
    self._tab = tab_size
  
  def p(self, string, fill=" ", format="<", frame="#", line_width=80, tab=4, display=True):
    if fill == None:
      fill = self._fill
    if format == None:
      format = self._format
    if frame == None:
      frame = self._frame
    if line_width == None:
      line_width = self._line_wdith
    if tab == None:
      tab = self._tab
    
    format = {
        format   : format[0],
        "left"   : "<",
        "right"  : ">",
        "center" : "^",
    }[format]
    str_arr = string.split('\n')
    if (self._logfile != ""):
      of = open(self._logfile, 'a')
    for str in str_arr:
      new_string = "\t"*self._tab_count+str
      f_string = "{0}{{0:{1}{2}{3}}}{0}".format(frame,fill,format,line_width-2*len(frame))
      if (display):
        print(f_string.format(new_string.expandtabs(tab)))
      if (self._logfile != ""):
        print(f_string.format(new_string.expandtabs(tab)), file=of)
    
    of.close()

