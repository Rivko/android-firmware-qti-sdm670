Audio Dynamic Logging Documentation
------------------------------------
------------------------------------

Audio dynamic logging is enhancement in audio code to support run time logging
in audio modules.

audio_dynamic_log.xml
---------------------------
audio_dynamic_log.xml contains information about logging level for each module
and files under it. This xml file is present under /data/vendor/misc/audio.
Refer comments in xml for more details on order of files to be maintained
under each module and log modes supported.

audio_log_utils
---------------------------
audio_log_utils exposes API to parse audio_dynamic_log xml.
Each module utilizes log_utils APIs to parse the xml and obtain
logging level for each file under that module.

Steps to follow:
---------------------------
This needs to be done once after flashing new images -
$adb root
$adb remount

Usage -
$adb pull /data/vendor/misc/audio/audio_dynamic_log.xml <to_local_folder>
/* edit the xml file to either enable logs in all files under a module or
   enable/disable logging in a file under a module
 */
examples:
   ex1 (To enable logs for all files under a module) -
   <module name="hal" log_mode="MOD">

   ex2 (to enable log for particular file under a module) -
   <module name="hal" log_mode="FILE">
       <file name="audio_hw.c" log_enable="true"/>

   ex3 (to disable & enable logs for different files under a module)-
   <module name="hal" log_mode="FILE">
       <file name="audio_hw.c" log_enable="false"/>
       <file name="audio_hw.c" log_enable="true"/>

/* After modification, push the xml file back to same location. */
$adb push <from_local_folder> /data/vendor/misc/audio/

/* Now the required logs can be colleted based on the changes in the
   audio_dynamic_log.xml file.*/
/* Also one needs to restart the audioserver for the changes to be effected. */
