#!/usr/bin/env python


#                          EDIT HISTORY FOR MODULE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
# 09/16/15   ck      Added -s --sum
# 10/02/14   ck      Added -i --inplace-repair
# 10/02/14   ck      Added -t --test-repair
# 10/02/14   ck      Added -p --p4-checkout
# 10/02/14   ck      Added -m --modification-date option
# 09/24/14   ck      Added -e --edit-history option
# 09/24/14   ck      Added -c --continue option
# 09/12/14   ck      Initial release


from optparse import OptionParser
from subprocess import call
import shutil
import os
import sys
import locale
import re
import time
import datetime


# Following list is a collection of file extensions or file names with
# extensions (if applicable) that are to have their copyright information
# evaluated.
file_extensions = [".c", ".h", ".C", ".H"]


##############################################################################
# main
##############################################################################
def main():
  error_count = 0  

  parser = OptionParser()

  parser.add_option("-f", "--file",
    		    action="store", type="string", dest="file_name",
		    help="Single file path.")

  parser.add_option("-d", "--directory",
                    action="store", type="string", dest="base_directory",
		    help="Base directory name.")

  parser.add_option("-r", "--recursive",
                    action="store_true", dest="recursive",
		    help="Recursively search starting from base directory.")

  parser.add_option("-c", "--continue",
                    action="store_true", dest="continue_after_error",
		    help="Continue executing after a failure.")

  parser.add_option("-e", "--edit-history",
                    action="store_true", dest="edit_history",
		    help="Edit history must exist in file or it's an error.")

  parser.add_option("-m", "--modification-date",
                    action="store", dest="file_date", default="01/01/1900",
		    help="Only evaluate files that were modified on or " + \
                          "after date given.  Date format is MM/DD/YYYY.")

  parser.add_option("-p", "--p4-checkout",
                    action="store_true", dest="p4_checkout",
		    help="If option is given to repair a file, attempt to" + \
                         " use P4 to checkout the file out before modifying.")

  parser.add_option("-i", "--inplace-repair",
                    action="store_true", dest="repair",
		    help="Repair the copyright on original file.")

  parser.add_option("-t", "--test-repair",
                    action="store_true", dest="test_repair",
		    help="Test the repair of an incorrect copyright on " + \
                         "a copy of the file named: <original_name>_repair.")

  parser.add_option("-s", "--sum",
                    action="store_true", dest="sum_feature",
		    help="Sums up all of the offending files and returns count.")


  (options, args) = parser.parse_args()


  # Convert date string into datetime
  try:
    file_modification_minimum = time.strptime(options.file_date, "%m/%d/%Y")
  except:
    parser.error("File modification date supplied is invalid or not in proper format.")
    exit()


  # Ensure repair and test_repair are mutually exclusive.
  if options.repair and options.test_repair:
    parser.error("Inplace repair and test repair are mutually exclusive." + \
                 "  Please select only one.")


  # File or base directory must be supplied
  if not options.file_name and not options.base_directory:
    parser.error("A single file name or base directory must be supplied.")


  # If a single file was given then ensure it exists and then evaluate it.
  if options.file_name:
    if not os.path.isfile(options.file_name):
      parser.error("Single file name given does not exist.")

    if not evaluate_copyright(options.file_name,
                              options.edit_history,
                              options.sum_feature):

      # If file had error and option was given to repair the file then do it
      if options.repair or options.test_repair:
        repair_copyright(options.file_name,
                         options.test_repair,
                         options.p4_checkout,
                         options.edit_history)


  # If a base directory was given then search that directory for files
  # with extensions matching the file_extensions array.  Evaluate those
  # files.  If the recursive flag is set then scan all directories under
  # the base directory.  Ensure the directory exists first.
  if options.base_directory:
    if not os.path.isdir(options.base_directory):
      parser.error("Base directory name given does not exist:  " + \
                   options.base_directory)

    if not options.recursive:
      for file_name in os.listdir(options.base_directory):

        # Ensure file suffix matches and entry from file_extensions
        if any(file_name.endswith(ext) for ext in file_extensions):

          # Ensure file modification date is newer or equal to default or one supplied
          if os.path.getmtime(os.path.join(options.base_directory, file_name)) <= \
             file_modification_minimum:

            # Evaluate copyright
            if not evaluate_copyright(os.path.join(options.base_directory,file_name),
                                      options.edit_history,
                                      options.sum_feature):

              # Increase error count as error was determined
              error_count += 1

              # If file had error and option was given to repair the file then do it
              if options.repair or options.test_repair:
                repair_copyright(os.path.join(options.base_directory,file_name),
                                 options.test_repair,
                                 options.p4_checkout,
                                 options.edit_history)

              # if option was not given to continue after an error then exit
              if not options.continue_after_error and not options.sum_feature:
                break
    else:
      for root, directories, files in os.walk(options.base_directory):
        for file_name in files:

          # Ensure file suffix matches and entry from file_extensions
          if any(file_name.endswith(ext) for ext in file_extensions):

            # Ensure file modification date is newer or equal to default or one supplied
            if os.path.getmtime(os.path.join(root, file_name)) <= file_modification_minimum:
  
              # Evaluate copyright
              if not evaluate_copyright(os.path.join(root, file_name),
                                        options.edit_history,
                                        options.sum_feature):
      
                # Increase error count as error was determined
                error_count += 1

                # If file had error and option was given to repair the file then do it
                if options.repair or options.test_repair:
                  if not repair_copyright(os.path.join(options.base_directory,file_name),
                                          options.test_repair,
                                          options.p4_checkout,
                                          options.edit_history):
                    print "\nERROR: Unable to repair copyright for file: " + \
                          os.path.join(options.base_directory, file_name)

                # if option was not given to continue after an error then exit
                if not options.continue_after_error and not options.sum_feature:
                  break


  # Report the total number of errors if sum is enabled
  if options.sum_feature:
    print "Total errors: " + str(error_count)


  return


###############################################################################
# p4_checkout
###############################################################################
def p4_checkout(file_path):

  # Attempt to checkout the file with p4 edit <file_path>
  if call(["p4", "edit", file_path]):
    return False

  return True


###############################################################################
# repair_copyright
###############################################################################
def repair_copyright(file_path,
                     test_only,
                     checkout_file,
                     edit_history):

  print "\nREPAIR: Attempting to repair file: " + file_path

  # If checkout_file is true then attempt to checkout file using P4
  if checkout_file:
    if not p4_checkout(file_path):
      print "\nERROR: Unable to checkout file: " + file_path + " via P4"
      return False

  
  # Create a copy of the original file.  Repairs will be done on it.
  # <Original Name>_repair.<Original Extension>
  repair_file_name = create_test_file(file_path)
  if not repair_file_name:
    return False


  # Get the line number that contains the copyright
  line_number = copyright_line_number(file_path)
  if line_number == -1:
    "\nERROR: Unable to locate copyright line in: " + file_path
    return False

  
  # Get the modification years
  try:
    modification_years = extract_modification_years(file_path)
    list.sort(modification_years)
  except:
    print "\nERROR: An error occured while extracting the modification years " + \
      "for file: " + file_path
    return False


  # Ensure at least one modified year was found if edit_history option is
  # being enforced.  If not then only current year needs to be in copyright.
  if len(modification_years) == 0:
    if edit_history:
      print "\nERROR: Modification years could not be determined for file: " + \
        file_path
      return False
    else:
      # No modification years were found via edit history and edit history
      # option was not set.  So insert current year into modified_years.
      modification_years.append(int(datetime.datetime.now().year))


  # Create the new copyright with at least first, and possibly only, year.
  new_copyright_line = "Copyright " + str(modification_years[0])

  # Loop through remaining years adding to copyright.
  sequential_executing = False
  for index in range(1,len(modification_years)):
    # Determine if a comma is needed (Non sequential years) or
    # if hypen is needed.  (Sequential years).
    if modification_years[index] != modification_years[index - 1] + 1:
      # Non sequential
      # If sequential execution is underway then close it with previous year.
      if sequential_executing:
        sequential_executing = False
        new_copyright_line = new_copyright_line + str(modification_years[index - 1])

      new_copyright_line = new_copyright_line + "," + str(modification_years[index])
    else:
      # Sequential
      # If sequential execution is underway do nothing if not then add hyphen.
      if not sequential_executing:
        sequential_executing = True
        new_copyright_line = new_copyright_line + "-"

  # After looping through all years sequential execution could still be enabled.
  # In this case close it with the last year of the modification_years list
  if sequential_executing:
    sequential_executing = False
    new_copyright_line = \
      new_copyright_line + str(modification_years[len(modification_years) - 1])
  

  # Append rest of copyright
  new_copyright_line = new_copyright_line + " Qualcomm Technologies Incorporated.  All Rights Reserved."


  # Write the original file out to the repair file replacing the copyright.
  current_line = 0
  try:
    repair_fp = open(repair_file_name,'w+')
    with open(file_path) as file:
      for line in file:
        current_line = current_line + 1
        if current_line == line_number:
          repair_fp.write(new_copyright_line)
        else:
          repair_fp.write(line)

    repair_fp.close()
  except:
    print "\nERROR: An error occured while writing from original file: " + \
      file_path + " to repair file: " + repair_file_name
    return False


  # If test_only flag is not set then move the repair file over the original.
  if not test_only:
    try:
      shutil.move(repair_file_name,
                  file_path)
    except:
      print "\nERROR: An error occured while moving repair file: " + \
        repair_file_name + " over original file: " + file_path
      return False

  return True


###############################################################################
# create_test_file
###############################################################################
def create_test_file(file_path):
  # File name could have extension or not.  Look for "." after the last
  # directory seperator.  If exists then use, if not then file does not
  # have an extension.
  file_ext_position = ""


  try:
    last_dir_seperator = file_path.rfind(os.sep)
  except:
    # If exception occurs it just means seperator could not be found.  Use 0.
    last_dir_seperator = 0

  try:
    file_ext_position = \
      file_path.rfind(".",
                      last_dir_seperator)
  except:
    #if exception occurs then it means file does not have an extension
    pass

    
  if file_ext_position:
    file_name = file_path[:file_ext_position]
    file_ext = file_path[file_ext_position:]
  else:
    file_name = file_path
    file_ext = ""


  new_file_path = file_name + "_repair" + file_ext
  try:
    shutil.copyfile(file_path,
                    new_file_path)
  except:
    print "\nERROR: Unable to make a copy of file: " + file_path
    return ""


  return new_file_path


###############################################################################
# evaluate_copyright
###############################################################################
def evaluate_copyright(file_path,
                       edit_history,
                       silent):

  # Extract the modification years of the file based on edit log.
  # Sort modified_years so it is accending
  try:
    modified_years = extract_modification_years(file_path)
    list.sort(modified_years)
  except:
    print "\nERROR: An error occured while extracting the modification " + \
      "years for file: " + file_path
    return False
  

  # Ensure at least one modified year was found if edit_history option is
  # being enforced.  If not then only current year needs to be in copyright.
  if len(modified_years) == 0:
    if edit_history:
      if not silent:
        print "\nERROR: Modification years could not be determined for file: " + \
          file_path
      return False
    else:
      # No modification years were found via edit history and edit history
      # option was not set.  So insert current year into modified_years.
      modified_years.append(int(datetime.datetime.now().year))

  
  # Extract the copyright years from the file.
  # Sort copyright_years so it is accending
  try:
    copyright_years = extract_copyright_years(file_path)
    list.sort(copyright_years)
  except:
    print "\nERROR: An error occured while extracting the copyright years " + \
      "for file: " + file_path
    return False

  
  # Ensure at least one copyright year was found
  if len(copyright_years) == 0:
    if not silent:
      print "\nERROR: Copyright years could not be determined for file: " + \
        file_path
    return False


  # Check each year in modified_years against copyright_years to make sure
  # there is a valid entry
  for modified_year in modified_years:
    if not modified_year in copyright_years:
      if not silent:
        print "\nERROR: Missing copyright coverage for modification year " + \
          str(modified_year) + " in file: " + file_path
      return False


  # Ensure "Qualcomm Technologies Inc" is in copyright line
  try:
    if not validate_corporate_name(file_path):
      if not silent:
        print "\nERROR: Missing \"Qualcomm Technologies Inc\" in copyright line " + \
          "in file: " + file_path
      return False
  except:
    print "\nERROR: An error occured while extracting the copyright years " + \
      "for file: " + file_path
    return False
    

  return True


###############################################################################
# validate_corporate_name
###############################################################################
def validate_corporate_name(file_path):
  # A valid corporate name check is:
  # Line begins with "Copyright" in any case
  # Same line contains "Qualcomm Technologies Inc" in any case
  
  # Create regular expressions looking for proper corporate name
  reg_ex_corporate_name = re.compile("^.*copyright\s+.*qualcomm\s+.*technologies.*inc")
  
  
  # Open file and evaluate line by line looking for proper corporate name
  # in Qualcomm copyright.  Standardize by making the line all lowercase
  # before evaluating
  corporate_name_found = False
  with open(file_path) as file:
    for line in file:
      # Convert line to all lowercase
      line = line.lower()

      # Evaluate line
      matches = reg_ex_corporate_name.match(line)
      if matches:
        corporate_name_found = True
        break


  return corporate_name_found


###############################################################################
# copyright_line_number
###############################################################################
def copyright_line_number(file_path):
  # A minimal valid copyright to extract years is:
  # Line begins with "Copyright" in any case
  # Four digit years seperated by commas and/or hyphens
  # Contains "Qualcomm" in any case

  current_line = 0;

  # Create the regular expressions used later, all lowercase
  reg_ex_copyright = re.compile("^.*copyright(.*)qualcomm.*")


  # Open file and evaluate line by line looking for Qualcomm copyright
  # Standardize by making the line all lowercase before evaluating
  with open(file_path) as file:
    for line in file:
      # Update line number
      current_line = current_line + 1

      # Convert line to all lowercase
      line = line.lower()

      # Evaluate line
      matches = reg_ex_copyright.match(line)
      if matches:
        return current_line

  return -1


###############################################################################
# extract_copyright_years
###############################################################################
def extract_copyright_years(file_path):

  reg_ex_years = re.compile(r"[^\d,\,,-]")
  copyright_years = [] 
  raw_years_string = ""
  current_line = 0


  # Get the file line number that has the copyright
  line_number = copyright_line_number(file_path)
  if line_number == -1:
    return copyright_years


  # Open the file and get the line.  Convert to lowercase.
  with open(file_path) as file:
    for line in file:
      # Update line number
      current_line = current_line + 1

      if current_line == line_number:
        raw_years_string = line.lower()


  # Parse raw_years_string extracting single years (comma) and sequential
  # years (hyphen)

  # Strip all spaces
  raw_years_string = raw_years_string.replace(" ", "")

  # Strip any character that is not a number, comma, or hyphen
  raw_years_string = reg_ex_years.sub("", raw_years_string)


  # Parse all sequential years (Hyphen) and remove from raw_years_string
  while "-" in raw_years_string:
    year_range = raw_years_string[raw_years_string.index("-") - 4: \
                                  raw_years_string.index("-") + 5]

    raw_years_string = raw_years_string[:raw_years_string.index("-") - 4] + \
                       raw_years_string[raw_years_string.index("-") + 5:]

    for year in range(int(year_range[:4]), int(year_range[5:]) + 1):
      copyright_years.append(year)


  # Parse all single years (Comma) and remove from raw_years_string
  while "," in raw_years_string:
    # Check for a comma as the first character of a string.  If so then discard
    # it as it is a seperator between a year range and another year range
    # or single year.  A valid comma will be after a year.
    if raw_years_string[0] == ',':
      raw_years_string = raw_years_string[1:]
      continue

    copyright_years.append(int(raw_years_string[raw_years_string.index(",") - 4: \
                                                raw_years_string.index(",")]))
 
    raw_years_string = raw_years_string[:raw_years_string.index(",") - 4] + \
                       raw_years_string[raw_years_string.index(",") + 1:]


  # One last year could be left after all parsing
  if len(raw_years_string) == 4:
    copyright_years.append(int(raw_years_string))

  return copyright_years


###############################################################################
# extract_modification_years
###############################################################################
def extract_modification_years(file_path):
  # The following are valid entries in the "EDIT HISTORY" of a file:
  # 1) MM/DD/YY   <Space(s)>   Author    <Space(s)>   Notes 
  # 2) MM/DD/YYYY <Space(s)>   Author    <Space(s)>   Notes
  # 3) YYYY/DD/MM <Space(s)>   Author    <Space(s)>   Notes
  # 4) MM-DD-YY   <Space(s)>   Author    <Space(s)>   Notes 
  # 5) MM-DD-YYYY <Space(s)>   Author    <Space(s)>   Notes
  # 6) YYYY-DD-MM <Space(s)>   Author    <Space(s)>   Notes
  
  modified_years = []

  # Create the regular expressions
  # Covers entries 1 & 4
  reg_ex_1_4 = re.compile("^\s*([0-9]{2})[/-]([0-9]{2})[/-]([0-9]{2})\s+(\S*)\s+(.*)$")
  reg_ex_2_5 = re.compile("^\s*([0-9]{2})[/-]([0-9]{2})[/-]([0-9]{4})\s+(\S*)\s+(.*)$")
  reg_ex_3_6 = re.compile("^\s*([0-9]{4})[/-]([0-9]{2})[/-]([0-9]{2})\s+(\S*)\s+(.*)$")


  # Open file and evaluate line by line.  When match is found extract year.
  year = ""
  with open(file_path) as file:
    for line in file:

      # Evaluate 1_4
      matches = reg_ex_1_4.match(line)
      if matches:
        # Extract characters 7 and 8 (Group 3) and convert to full 4 digit year
        # Anything >= 90 will be 19xx, anything < will be 20xx
        if int(matches.group(3)) >= 90:
          year = int(matches.group(3)) + 1900
        else:
          year = int(matches.group(3)) + 2000

        # If year is not already in modified_years then add it
        if not year in modified_years:
          modified_years.append(year)


      # Evaluate 2_5
      matches = reg_ex_2_5.match(line)
      if matches:
        # Extract characters 7, 8, 9, and 10 (Group 3)
        if not int(matches.group(3)) in modified_years:
          modified_years.append(int(matches.group(3)))


      # Evaluate 3_6
      matches = reg_ex_3_6.match(line)
      if matches:
        # Extract characters 1, 2, 3, and 4 (Group 1)
        if not int(matches.group(1)) in modified_years:
          modified_years.append(int(matches.group(1)))


  return modified_years


main()
