use ASIA_Main;
use English qw(-no_match_vars);
use File::Basename qw(&basename &dirname);
#use TextUtils qw(&forwardslash &backslash);
use strict;
use AsiaTimeUtils;
use Storable qw(&store &retrieve);
use Clone qw(&clone);

use Mortar::Audio::include;

use Mortar::Common::Tools::AdbUtils qw(:ok);
use Mortar::Common::Tools::ArchiveUtils qw(:ok);
use Mortar::Common::Tools::SystemUtils qw(:ok);
use Mortar::Common::Tools::QpstUtils;
use Mortar::Common::Tools::SpiderBoard;
use Mortar::Common::Tools::PwrUSB;

use Mortar::QDSP::ImageUtils;
use Mortar::QDSP::TestFrameworkUtils qw(:ok);

use Time::HiRes qw(&sleep);
use Win32::Process::Info;

BEGIN {
   CreateCmdLineVar('$test_master', ['f', '-e']);
   CreateCmdLineVar('$test_master_path', ['f', '-d']);
   CreateCmdLineVar('@test_master_list', ['a', 1, undef, ['f', '-e']]);
   CreateCmdLineVar('$sort_method', ['si', 'area', 'level', 'feature', 'random'], 'level');

   CreateCmdLineVar('$ArchiveRoot', ['f', '-d']);

   CreateCmdLineVar('$build_contents_xml_file', ['f', '-e']);
   CreateCmdLineVar('$meta_build_path', ['f', '-d']);
   CreateCmdLineVar('$rpm_build_path', ['f', '-d']);
   CreateCmdLineVar('$apps_build_path', ['f', '-d']);

   CreateCmdLineVar('$dsp_build_path', ['f', '-d']);
   CreateCmdLineVar('$enable_trust_zone', ['s', $TRUE, $FALSE], $TRUE);
   CreateCmdLineVar('$firmware_type', ['v']);

   CreateCmdLineVar('$test_content_root', ['f', '-d']);
   CreateCmdLineVar('$ion_driver_file', ['f', '-e']);
   CreateCmdLineVar('$framework_source_path', ['f','-d']);
   CreateCmdLineVar('$framework_path', ['v']);

   CreateCmdLineVar('$use_pwr_usb_for_reset', ['s', $TRUE, $FALSE]);
   CreateCmdLineVar('$dut_power_outlet_id', ['x', '/^outlet[1-3]$/i'], 'outlet2');
   CreateCmdLineVar('$usb_hub_power_outlet_id', ['x', '/^outlet[1-3]$/i'], 'outlet3');

   CreateCmdLineVar('$use_spider_board_for_reset', ['s', $TRUE, $FALSE]);
   CreateCmdLineVar('$spider_board_uuid', ['x', '/^[0-9a-f\-]+$/i']);

   CreateCmdLineVar('$verbose', ['s', $TRUE, $FALSE], $FALSE);
   CreateCmdLineVar('$debugObjects', ['s', $TRUE, $FALSE], $FALSE);

   # used for nv backup
   CreateCmdLineVar('$PhoneResID', ['x', '/^APS\.\d+$/i'], 'aps.1');
   CreateCmdLineVar('$ComPort', ['or', ['u'], ['x', '/^(COM\d+|FINDSINGLE)$/i']]);

   # Extended Message 2.0 settings, see APS PG for more info
   #  (i.e. at command prompt, type 'doc aps')
   CreateCmdLineVar('%ExtMsgRtHash', ['h'], (
      '8500' => 0x1F, # QDSP6 messages LOW through FATAL, bits 1-4
   ));

   # optional Extended Message 2.0 settings, to be executed w/o error handling.
   CreateCmdLineVar('%optExtMsgRtHash', ['h']);

   CreateCmdLineVar('%additionalExtMsgRtHash', ['h']);

   CreateCmdLineVar('$f3MessageLvl', ['s',
      $MSG_LVL_LOW, $MSG_LVL_MED, $MSG_LVL_HIGH,
      $MSG_LVL_FATAL, $MSG_LVL_ERROR, $MSG_LVL_NONE,
   ], undef);

   CreateCmdLineVar('@log_masks', ['a', 1, undef, ['v']], (
      # apr log codes
      0x14d0 .. 0x14d2,
   ));

   CreateCmdLineVar('@additional_log_masks', ['a', 0, undef, ['d']]);
   CreateCmdLineVar('$qxdm_config_file', ['f', '-r']);

   # exlude specific test set numbers, even if other criteria are met
   CreateCmdLineVar('@exclude_test_sets', ['a', 0, undef, ['v']]);

   # include only the following test set numbers, if other criteria are also met
   CreateCmdLineVar('@include_test_sets', ['a', 0, undef, ['v']]);

   # exclude all test sets with thes test_type values
   CreateCmdLineVar('@exclude_test_types', ['a', 0, undef, ['v']]);

   # include only test sets with these test_type values
   CreateCmdLineVar('@include_test_types', ['a', 0, undef, ['v']]);

   # include only test sets that exclusively feature included test_type values
   CreateCmdLineVar('$enable_exclusive_test_type', ['s', $TRUE, $FALSE], $FALSE);

   CreateCmdLineVar('$reset_on_test_fail', ['s', $TRUE, $FALSE], $TRUE);
   CreateCmdLineVar('$reset_on_suite_init', ['s', $TRUE, $FALSE], $FALSE);

   CreateCmdLineVar('$get_ebi_memory_dump_on_fail', ['s', $TRUE, $FALSE], $FALSE);
   CreateCmdLineVar('$get_ebi_memory_dump_on_timeout', ['s', $TRUE, $FALSE], $TRUE);
   CreateCmdLineVar('$get_ebi_memory_dump_on_abort', ['s', $TRUE, $FALSE], $TRUE);
   CreateCmdLineVar('$get_smi_memory_dump_on_fail', ['s', $TRUE, $FALSE]);

   CreateCmdLineVar('$enable_qpst_memory_download', ['s', $TRUE, $FALSE]);

   CreateCmdLineVar('$enable_dsp_load', ['s', $TRUE, $FALSE], $TRUE);
   CreateCmdLineVar('$enable_framework_sync', ['s', $TRUE, $FALSE], $TRUE);
   CreateCmdLineVar('$daisy_chain_mode', ['si', 'DC_Dynamic', 'DC_DAP']);

   CreateCmdLineVar('$enable_phone_logging', ['s', $TRUE, $FALSE], $TRUE);
   CreateCmdLineVar('$enable_phone_pcm_logging', ['s', $TRUE, $FALSE]);
   CreateCmdLineVar('$enable_extended_pcm_logging', ['s', $TRUE, $FALSE], $FALSE);
   CreateCmdLineVar('$enable_phone_log_parsing', ['s', $TRUE, $FALSE], $TRUE);
   CreateCmdLineVar('$enable_phone_log_per_iteration', ['s', $TRUE, $FALSE], $FALSE);

   CreateCmdLineVar('$enable_adb_logging', ['s', $TRUE, $FALSE], $TRUE);
   CreateCmdLineVar('$enable_adb_cleanup', ['s', $TRUE, $FALSE], $TRUE);
   CreateCmdLineVar('$enable_adb_debug_logging', ['s', $TRUE, $FALSE], $FALSE);

   CreateCmdLineVar('$restart_qpst_server', ['s', $TRUE, $FALSE], $FALSE);
   CreateCmdLineVar('$shutdown_qpst_server', ['s', $TRUE, $FALSE], $FALSE);
   CreateCmdLineVar('$test_framework_priority', ['r', -20, 20]);
   CreateCmdLineVar('$enable_ipc_router_debug', ['s', $TRUE, $FALSE], $FALSE);

   CreateCmdLineVar('$max_test_retries', ['r', 0, undef, 'inc=1'], 1);
   CreateCmdLineVar('$default_test_set_timeout', ['r', 10, undef], 180);
   CreateCmdLineVar('$default_test_iteration_timeout', ['r', 10, undef], 180);
   CreateCmdLineVar('$run_test_iterations_separately', ['s', $TRUE, $FALSE], $FALSE);
   CreateCmdLineVar('$repeat_until_elapsed_time', ['r', 0, undef, 'inc=1']);
   CreateCmdLineVar('$require_tunnel_mode_analysis', ['s', $TRUE, $FALSE], $TRUE);
   CreateCmdLineVar('$xml_result_path', ['f', '-d']);
   CreateCmdLineVar('$txt_result_summary_file', ['v'], 'test_summary.txt');

   CreateCmdLineVar('$sleep_between_tests', ['r', 0, undef], 0);
   CreateCmdLineVar('$disable_usb_between_tests', ['s', $TRUE, $FALSE], $FALSE);
   CreateCmdLineVar('$put_screen_to_sleep_between_tests', ['s', $TRUE, $FALSE], $FALSE);
   CreateCmdLineVar('$wakeup_screen_before_test', ['s', $TRUE, $FALSE], $TRUE);

   CreateAjeVar('$dsp_build', ['v'], 'unknown');
   CreateAjeVar('$product_line', ['v'], 'unknown');
   CreateAjeVar('$hw_platform', ['v'], 'unknown');

   CreateCmdLineVar('$test_scope', ['v']);
   CreateCmdLineVar('$apt_level', ['r', 1, 4, 'inc=1']);
   CreateCmdLineVar('$test_category', ['v']);

   CreateCmdLineVar('%pc_playback_device_table', ['h']);
   CreateCmdLineVar('%pc_record_device_table', ['h']);

   $RequiredAsiaServers{'MM_TEST:MORTAR'}       = '1.0.034';
   $RequiredAsiaServers{'MM_TEST:MORTAR_AUDIO'} = '1.0.026';
   $RequiredAsiaServers{'MM_TEST:MORTAR_QDSP'}  = '1.0.018';
   $RequiredAsiaServers{'MM_TEST:MORTAR_VOICE'} = '1.0.005';
}

my @pcm_log_masks = (
   # pcm logging
   0x13b0,
);

$CleanupHandler = 'cleanup';
$TestCaseLog = './perl_stdout.txt';

my $summary = {
   'test_sets' => {
      'total'     => 0,
      'mem_leaks' => 0,
   },
   'tests' => {
      'total' => 0,
      'pass'  => 0,
      'fail'  => 0,
   },
   'fwk' => {
      'test_launch_fail' => 0,
      'cosim_reset_fail' => 0,
      'mem_leaks'        => 0,
      'invalid_adsp_msg' => 0,
   },
};

my $master_test_list = [];
my $test_result_list = [];

ASIA_Initialize();

if (not defined $test_master and not @test_master_list and not $test_master_path) {
   ErrorMsg(
      'unable to proceed, neither $test_master, $test_master_path nor @test_master_list is defined',
      'specify one and only one',
   );
   ExitTestCase($FAIL);
}

if (defined $test_master and @test_master_list) {
   ErrorMsg(
      'unable to proceed, both $test_master and @test_master_list are defined',
      'specify one and only one',
   );
   ExitTestCase($FAIL);
}

if (defined $test_master and defined $test_master_path) {
   ErrorMsg(
      'unable to proceed, both $test_master and $test_master_path are defined',
      'specify one and only one',
   );
   ExitTestCase($FAIL);
}

if (defined $test_master_path and @test_master_list) {
   ErrorMsg(
      'unable to proceed, both $test_master_path and @test_master_list are defined',
      'specify one and only one',
   );
   ExitTestCase($FAIL);
}

if (defined $test_master) {
   @test_master_list = ($test_master);
} elsif (defined $test_master_path) {
   $test_master_path = backslash($test_master_path);
   $test_master_path .= '\\' if $test_master_path !~ m/\\$/i;

   my ($ret_code, $dir_output) = dir_cmd({
      'args' => backslash($test_master_path) . '*.txt',
      'switches' => '/A:-D /B',
   });

   @test_master_list = split("\n", $dir_output);
   foreach my $test_master (@test_master_list) {
      chomp($test_master);
      $test_master = $test_master_path . $test_master;
   }
}

$sort_method = lc($sort_method);

push(@pcm_log_masks, 0x13b1, 0x14ee, 0x152e .. 0x1536) if $enable_extended_pcm_logging;

my @orig_addtl_log_masks = @additional_log_masks;

# asia resource handles
our (
   $phone,
);

my $build_map;

# set the debugging level for all system utils function calls
Mortar::Common::Tools::SystemUtils::set_verbose($verbose);
Mortar::QDSP::TestFrameworkUtils::set_verbose($verbose);
Mortar::Common::Tools::AdbUtils::set_verbose($verbose);

Mortar::Common::Tools::AdbUtils::start_adb_cleanup_process() if $enable_adb_cleanup;
Mortar::Common::Tools::AdbUtils::start_adb_server();

my $adb_device_online = $FALSE;
my $adb_device_list = Mortar::Common::Tools::AdbUtils::get_adb_device_list();
if (defined $adb_device_list and ref($adb_device_list) eq 'ARRAY' and @$adb_device_list == 1) {
   my $dev_status = Mortar::Common::Tools::AdbUtils::get_adb_device_status($adb_device_list->[0]);
   $adb_device_online = $TRUE if $dev_status eq 'device';
}

my $spider_board;
my $spider_board_args = {'debug' => $verbose};
if (defined $spider_board_uuid) {
   $spider_board_args->{'uuid'} = $spider_board_uuid;
}

eval { $spider_board = Mortar::Common::Tools::SpiderBoard->new($spider_board_args); };

if (defined $spider_board and not defined $use_spider_board_for_reset) {
   if (not defined $use_pwr_usb_for_reset) {
      $use_pwr_usb_for_reset = $FALSE;
      $use_spider_board_for_reset = $TRUE;
   } elsif ($use_pwr_usb_for_reset eq $FALSE) {
      $use_spider_board_for_reset = $TRUE;
   }
} elsif (defined $use_pwr_usb_for_reset and $use_pwr_usb_for_reset eq $TRUE) {
   PrintMsg('\'spider board\' detected, ignoring $use_pwr_usb_for_reset');
   $use_pwr_usb_for_reset = $FALSE;
}
$use_spider_board_for_reset = _default($use_spider_board_for_reset, $FALSE);

my $pwr_usb_device_online = Mortar::Common::Tools::PwrUSB::check_if_pwr_usb_is_online();
$use_pwr_usb_for_reset = _default($use_pwr_usb_for_reset, (not $use_spider_board_for_reset and $pwr_usb_device_online));

if ($adb_device_online) {
   my $cmd_spec = {'timeout' => 60, 'verbose' => $verbose};
   my $ret_code = Mortar::Common::Tools::AdbUtils::adb_reboot($cmd_spec) if $reset_on_suite_init;
} elsif ($use_spider_board_for_reset) {
   run_spider_board_reset($FALSE);
} elsif ($pwr_usb_device_online) {
   Mortar::Common::Tools::PwrUSB::set_outlet_state($usb_hub_power_outlet_id, 1);
   run_pwrusb_reset($FALSE);
} elsif ($use_pwr_usb_for_reset) {
   ErrorMsg('unable to use PowerUSB device for reset, no device is detected on this computer');
   ExitTestCase($FAILED);
}

my $qpst_mem_dload_default = $get_ebi_memory_dump_on_fail || $get_ebi_memory_dump_on_abort || $get_ebi_memory_dump_on_timeout;
$enable_qpst_memory_download = _default(
   $enable_qpst_memory_download,
   $qpst_mem_dload_default,
);

my $user_err_handling = $ErrorHandling;

my @cleanup_files        = ();
my @archives_to_rename   = ();
my @dlf_logs_to_parse    = ();
my @voc_files_to_analyze = ();

my $run_result;
my $run_archive;
my $attempt_archive;

our $daw_obj; # audio playback & recording
my $play_dma_input_group = $FALSE;
my $rec_dma_output_group = $FALSE;

my $chmod_done = $FALSE;

if (not defined $meta_build_path and not defined $apps_build_path) {
   my $err_msg = 'please define at least one build, $meta_build_path or ';
   $err_msg .= '$apps_build_path for tracking purposes';

   ErrorMsg($err_msg);
   ExitTestCase($FAILED);
}

if (not defined $meta_build_path and not defined $dsp_build_path) {
   my $err_msg = 'please define at least one build, $meta_build_path or ';
   $err_msg .= '$dsp_build_path for tracking purposes';

   ErrorMsg($err_msg);
   ExitTestCase($FAILED);
}

if (defined $meta_build_path) {
   $meta_build_path = _default($meta_build_path, $apps_build_path);
   $meta_build_path = forwardslash($meta_build_path);
   $meta_build_path =~ s/\/$//i;
   $build_contents_xml_file = _default($build_contents_xml_file, $meta_build_path . '/contents.xml')
      if -e $meta_build_path . '/contents.xml';
}

if (defined $build_contents_xml_file and -e $build_contents_xml_file) {
   $build_map = parse_build_contents_xml($build_contents_xml_file);

   if (defined $build_map) {
      DumperMsg(
         'meta build contains the following images by default:',
         $build_map,
      );

      if (exists $build_map->{'apps'}) {
         $apps_build_path = _default($apps_build_path, $build_map->{'apps'});
      }

      if (exists $build_map->{'lpass'}) {
         $dsp_build_path = _default($dsp_build_path, $build_map->{'lpass'});
      }

      if (exists $build_map->{'adsp'}) {
         $dsp_build_path = _default($dsp_build_path, $build_map->{'adsp'});
      }

      if (exists $build_map->{'rpm'}) {
         $rpm_build_path = _default($rpm_build_path, $build_map->{'rpm'});
      }
   }
}

if (not defined $dsp_build_path) {
   ErrorMsg(
      'unable to determine $dsp_build_path from the meta build, unable ' .
      'to proceed with testing',
   );
   ExitTestCase($FAILED);
} else {
   $dsp_build_path = forwardslash($dsp_build_path);
   $dsp_build_path =~ s/\/$//i;
}

if (defined $rpm_build_path) {
   $rpm_build_path = forwardslash($rpm_build_path);
   $rpm_build_path =~ s/\/$//i;
}

$dsp_build = basename($dsp_build_path) if lc($dsp_build) eq 'unknown';
if (lc($product_line) eq 'unknown' or lc($hw_platform) eq 'unknown') {
   my ($build_info, $err_desc) = AsiaBuildUtils::GetBuildInfo($dsp_build);
   DumperMsg('build info', $build_info, $err_desc);

   if (defined $build_info and ref($build_info) eq 'HASH') {
      my $records;
      my $build_record_list;
      my $build_record;

      if (exists $build_info->{'Records'}) {
         $records = $build_info->{'Records'};
      }

      if (defined $records and exists $records->{'BuildRecordList'}) {
         $build_record_list = $records->{'BuildRecordList'};
      }

      $build_record_list = _default($build_record_list, []);
      if (ref($build_record_list) ne 'ARRAY') {
         $build_record_list = [$build_record_list];
      }

      my $build_date;
      my $build_location;
      my $build_status;
      my $build_type;

      foreach my $build_record_entry (@$build_record_list) {
         next if not defined $build_record_entry;
         next if ref($build_record_entry) ne 'HASH';
         next if not exists $build_record_entry->{'BuildRecord'};
         
         $build_record = $build_record_entry->{'BuildRecord'};

         next if not defined $build_record;
         next if ref($build_record_entry) ne 'HASH';
         next if not exists $build_record->{'Build'};
         next if lc($build_record->{'Build'}) ne lc($dsp_build);

         $build_date = $build_record->{'BuildDate'};
         $build_location = $build_record->{'Location'};
         $build_status = $build_record->{'Status'};
         $build_type = $build_record->{'Type'};

         $hw_platform = _default($hw_platform, $build_record->{'HWPlatform'});
         $hw_platform = $build_record->{'HWPlatform'}
            if lc($hw_platform) eq 'unknown';
         $hw_platform = _default($hw_platform, 'unknown');

         $product_line = _default(
            $product_line, $build_record->{'ProductLine'},
         );
         $product_line = $build_record->{'ProductLine'}
            if lc($product_line) eq 'unknown';
         $product_line = _default($product_line, 'unknown');
      }

      # display results
   }
}

# need to add PL and target info to the default
my $content_root_default = 'c:\perforce\qctp401\aDSPTest\Audio\adsptest\obj\android'
   if -d 'c:\perforce\qctp401\aDSPTest\Audio\adsptest\obj\android';
$content_root_default = 'c:\perforce\qctp401\aDSPTest\obj\android'
   if -d 'c:\perforce\qctp401\aDSPTest\obj\android';

$test_content_root = _default($test_content_root, $content_root_default);
ErrorMsg(
   'unable to determine $test_content_root path, please provide as a ' .
   'command-line override',
) if not defined $test_content_root;

$test_content_root = forwardslash($test_content_root);
$test_content_root =~ s/\/$//i;

$framework_path = _default($framework_path, '/data/adsp-test');
$framework_path = forwardslash($framework_path);
$framework_path =~ s/\/$//i;

my $fwk_exe_file;
my $remote_test_master = $framework_path . '/TestMaster.txt';
my $curr_apt_level;
my $curr_test_category;
my $curr_test_scope;

my $ARCHIVE;
my $LOG_MASK_CONFIG;
my $APS_IS_LOGGING = $FALSE;

my $DEFAULT_ARCHIVE_ROOT = 'e:/logs' if -d 'e:/logs';
$DEFAULT_ARCHIVE_ROOT = 'c:/logs' if not defined $DEFAULT_ARCHIVE_ROOT;

my $total_test_time = 0;
my $total_crash_count = 0;

my @argvCopy = @ARGV;

{
   if (@exclude_test_sets and @include_test_sets) {
      ErrorMsg(
         'command-line variables \'@include_test_sets\' and ' .
         '\'@exclude_test_sets\' were both specified, but can only be used ' .
         'exclusively',
      );
      ExitTestCase($FAIL);
   }

   if (@exclude_test_types and $enable_exclusive_test_type) {
      ErrorMsg(
         'command-line variables \'@exclude_test_types\' cannot be used if ' .
         '\'$enable_exclusive_test_type\' is $TRUE',
      );
      ExitTestCase($FAIL);
   }

   if (not @include_test_types and $enable_exclusive_test_type) {
      ErrorMsg(
         'command-line variable \'@include_test_types\' is required if ' .
         '\'$enable_exclusive_test_type\' is $TRUE',
      );
      ExitTestCase($FAIL);
   }

   my ($exit_code, $cwd) = system_cmd('cd');
   $cwd = forwardslash($cwd);
   $cwd .= '/' if $cwd !~ m/\/$/i;

   # TODO: check rcl ports

   my $master_spec = {};
   if (@include_test_types) {
      $master_spec->{'test_types'}->{'include'} = \@include_test_types;
      $master_spec->{'test_types'}->{'exclusive'} = $enable_exclusive_test_type;
   }

   if (@exclude_test_types) {
      $master_spec->{'test_types'}->{'exclude'} = \@exclude_test_types;
   }

   if (@include_test_sets) {
      $master_spec->{'test_sets'}->{'include'} = \@include_test_sets;
   }

   if (@exclude_test_sets) {
      $master_spec->{'test_sets'}->{'exclude'} = \@exclude_test_sets;
   }

   if (defined $apt_level) {
      $master_spec->{'level'} = $apt_level;
   }

   if (defined $test_category) {
      $master_spec->{'category'} = $test_category;
   }

   my $master_params;
   my $master_product_line;
   my $master_hw_platform;

   foreach $test_master (@test_master_list) {
      my $test_list;
      $master_spec->{'file'} = $test_master;
      ($test_list, $master_params) = read_test_master_file($master_spec);
      push(@$master_test_list, @$test_list);
   }

   $sort_method = lc($sort_method);
   $master_test_list = Mortar::QDSP::TestFrameworkUtils::sort_test_sets_by_area($master_test_list)
      if $sort_method eq 'area';
   $master_test_list = Mortar::QDSP::TestFrameworkUtils::sort_test_sets_by_feature($master_test_list)
      if $sort_method eq 'feature';
   $master_test_list = Mortar::QDSP::TestFrameworkUtils::sort_test_sets_by_level($master_test_list)
      if $sort_method eq 'level';
   $master_test_list = Mortar::QDSP::TestFrameworkUtils::randomize_test_set_order($master_test_list)
      if $sort_method eq 'random';

   $master_product_line = $master_params->{'product_line'}
      if defined $master_params and defined $master_params->{'product_line'};
   $master_hw_platform = $master_params->{'hw_platform'}
      if defined $master_params and defined $master_params->{'hw_platform'};

   $product_line = $master_product_line
      if not defined $product_line or lc($product_line) eq 'unknown';

   $hw_platform = $master_hw_platform
         if not defined $hw_platform or lc($hw_platform) eq 'unknown';

   my $curr_err_handling = $ErrorHandling;
   $ErrorHandling = $ERR_LOG;
   process_test_set_configurations($master_test_list) if defined $master_test_list;
   $ErrorHandling = $curr_err_handling;

   $master_test_list = _default($master_test_list, []);
   if (not @$master_test_list) {
      ErrorMsg(
         'TestMaster not defined, contains no tests, or the filters exclude all test sets',
      );
      ExitTestCase($SKIPPED);
   }

   # reboots the phone
   Mortar::QDSP::ImageUtils::load_android_adsp_image({
      'path' => $dsp_build_path,
      'verbose' => $verbose,
      'hashed' => $enable_trust_zone,
   }) if $enable_dsp_load;

   my $framework_args = {
      'source' => _default($framework_source_path,$test_content_root),
      'verbose' => $verbose,
   };

   if (not defined $ion_driver_file) {
      my @driver_files = ('avs_test_ker.ko', 'adsptest_fwk.ko', 'ion_driver.ko');
      my @sub_dirs = (
         'LINUX/android/vendor/qcom/proprietary/mm-audio/adsp-test/',
         'LINUX/android/out/target/product/' . $hw_platform . '/system/lib/modules/',
         'ion_driver/'
      );

      my @root_dirs = ();
      push(@root_dirs, $apps_build_path) if defined $apps_build_path and -d $apps_build_path;
      push(@root_dirs, $test_content_root) if defined $test_content_root and -d $test_content_root;

      ROOTDIR: foreach my $root_dir (@root_dirs) {
         $root_dir = forwardslash($root_dir);
         $root_dir .= '/' if $root_dir !~ m/\/$/i;

         SUBDIR: foreach my $subdir (@sub_dirs) {
            DRIVER: foreach my $driver_file (@driver_files) {
               my $file_path_and_name = $root_dir . $subdir . $driver_file;
               if (-e $file_path_and_name) {
                  $ion_driver_file = $file_path_and_name;
                  last ROOTDIR;
               }
            }
         }
      }
   }

   if (defined $ion_driver_file and -e $ion_driver_file) {
      $framework_args->{'ion_driver'} = $ion_driver_file;
   }

   Mortar::QDSP::TestFrameworkUtils::load_android_adsp_test_framework($framework_args)
      if $enable_framework_sync;

   # prevent a memory leak on ANDROID
   run_afe_init_test();

   if ($enable_phone_logging) {
      # for aps connectivity
      Mortar::Common::Tools::QpstUtils::stop_qpst_configuration() if $restart_qpst_server;
      Mortar::Common::Tools::QpstUtils::start_qpst_configuration();
   }

   $curr_err_handling = $ErrorHandling;

   RUN: foreach my $test_set (@$master_test_list) {
      if ($put_screen_to_sleep_between_tests eq $TRUE) {
         Mortar::Common::Tools::AdbUtils::press_power_key() if Mortar::Common::Tools::AdbUtils::is_display_on();
      }

      if ($disable_usb_between_tests) {
         Mortar::Common::Tools::AdbUtils::stop_adb_server();
         Mortar::Common::Tools::PwrUSB::set_outlet_state($usb_hub_power_outlet_id, 0) if $use_pwr_usb_for_reset;
         $spider_board->set_peripheral_power(0) if $use_spider_board_for_reset;
         sleep(5);
      }

      sleep($sleep_between_tests) if $sleep_between_tests > 0;

      if ($disable_usb_between_tests) {
         Mortar::Common::Tools::PwrUSB::set_outlet_state($usb_hub_power_outlet_id, 1) if $use_pwr_usb_for_reset;
         $spider_board->set_peripheral_power(1) if $use_spider_board_for_reset;
         sleep(5);
         Mortar::Common::Tools::AdbUtils::start_adb_server();
      }

      $curr_apt_level = _default($apt_level, $test_set->{'test_level'});
      $curr_test_category = _default($test_category, $test_set->{'test_category'});
      $curr_test_scope = _default($test_scope, $test_set->{'test_scope'});

      generate_archive_root($test_set) if not exists $test_set->{'archive'};
      generate_archive_root($test_set) if not defined $test_set->{'archive'};

      create_archive($test_set->{'archive'});
      $run_archive = ARCHIVE();

      $daw_obj->cleanup() if defined $daw_obj;
      $daw_obj = undef;

      close_system_cmd_log() if is_system_cmd_log_open();
      create_system_cmd_log($run_archive . 'system_cmd_log.txt');

      my $stability_test = $FALSE;
      $stability_test = $TRUE
         if defined $curr_test_category and $curr_test_category =~ m/stability/i;
      $stability_test = $TRUE
         if defined $curr_test_scope and $curr_test_scope =~ m/stability/i;

      my $tunnel_mode = _default($test_set->{'tunnel_mode'}, $FALSE);

      # setup audio playback
      if (exists $test_set->{'dma_input'}) {
         my $dma_input_table = $test_set->{'dma_input'};
         foreach my $device (keys(%$dma_input_table)) {
            my $dev_info = $dma_input_table->{$device};

            if ($dev_info->{'enabled'}) {
               $ErrorHandling = $ERR_LOG;

               # create radio link player, if not exists
               $daw_obj = Mortar::Audio::DAW->new({'debug' => $verbose})
                  if not defined $daw_obj;

               my $curr_err_handling = $ErrorHandling;
               $ErrorHandling = $ERR_NONE;
               my $player_list = $daw_obj->list_player_instances('dma_input');
               $ErrorHandling = $curr_err_handling;

               my $player_exists = $FALSE;
               if (defined $player_list and @$player_list) {
                  $player_exists = $TRUE if join(', ', @$player_list) =~ m/${device}/i;
               }

               # create dma input player, if not exists
               my $pc_device = _default($pc_playback_device_table{$device}, -1);
               $daw_obj->create_player_instance({
                  'group' => 'dma_input',
                  'name' => $device,
                  'output_device' => $pc_device,
               }) if not $player_exists;

               # convert raw to wav
               my $pb_file = $run_archive . 'input_files/' . $device . '_input.wav';
               system_cmd(['mkdir', '"' . dirname($pb_file) . '"'])
                  if not -d dirname($pb_file);

               if ($dev_info->{'file'} !~ m/\.wav$/i) {
                  Mortar::Audio::WAV::convert_raw_to_wav({
                     'output' => $pb_file,
                     'input' => $dev_info->{'file'},
                     'remove_raw' => $FALSE,
                     'details' => {
                        'samplerate' => _default($dev_info->{'sample_rate'}, 48000),
                        'channels' => _default($dev_info->{'num_channels'}, 2),
                        'bits_sample' => _default($dev_info->{'bits_per_sample'}, 16),
                     },
                  });
               } else {
                  system_cmd(['cp', '-f', '"' . $dev_info->{'file'} . '"', $pb_file])
               }

               # pre-load player
               $daw_obj->pre_load_player({
                  'group' => 'dma_input',
                  'name' => $device,
                  'file' => $pb_file,
               });
               $play_dma_input_group = $TRUE;

               $ErrorHandling = $curr_err_handling;
            }
         }
      }

      if (exists $test_set->{'dma_output'} and not $stability_test) {
         my $dma_output_table = $test_set->{'dma_output'};
         foreach my $device (keys(%$dma_output_table)) {
            my $dev_info = $dma_output_table->{$device};
            if ($dev_info->{'enabled'}) {
               $ErrorHandling = $ERR_LOG;

               # create radio link player, if not exists
               $daw_obj = Mortar::Audio::DAW->new({'debug' => $verbose})
                  if not defined $daw_obj;

               my $curr_err_handling = $ErrorHandling;
               $ErrorHandling = $ERR_NONE;
               my $recorder_list = $daw_obj->list_recorder_instances('dma_output');
               $ErrorHandling = $curr_err_handling;

               my $recorder_exists = $FALSE;
               if (defined $recorder_list and @$recorder_list) {
                  $recorder_exists = $TRUE if join(', ', @$recorder_list) =~ m/${device}/i;
               }

               # create dma input recorder, if not exists
               my $pc_device = _default($pc_record_device_table{$device}, -1);
               my $recorder_spec = {
                  'group' => 'dma_output',
                  'name' => $device,
                  'input_device' => $pc_device,
               };

               if (defined $dev_info->{'sample_rate'}) {
                  $recorder_spec->{'sample_rate'} = $dev_info->{'sample_rate'};
               }

               if (defined $dev_info->{'num_channels'}) {
                  $recorder_spec->{'channels'} = $dev_info->{'num_channels'};
               }

               $daw_obj->create_recorder_instance($recorder_spec)
                  if not $recorder_exists;

               $rec_dma_output_group = $TRUE;

               $ErrorHandling = $curr_err_handling;
            }
         }
      }

      my $attempt_result;
      my $max_attempts = $max_test_retries + 1;
      ATTEMPT: for (my $attempt = 1; $attempt <= $max_attempts; $attempt++) {
         # create archive
         create_archive($run_archive, 'attempt') if $max_attempts > 1;
         $attempt_archive = ARCHIVE();
   
         $ErrorHandling = $ERR_LOG;

         my $local_test_set = clone($test_set);

         my $total_test_loops = 1;
         if ($run_test_iterations_separately) {
            $total_test_loops = $test_set->{'iterations'};
            $local_test_set->{'iterations'} = 1;
         }
   
         my $test_set_file = $run_archive . 'input_files/TestMaster.txt';
         system_cmd(['mkdir', '"' . dirname($test_set_file) . '"'])
            if not -d dirname($test_set_file);

         if (!-e $test_set_file) {
            my $test_set_spec = {
               'test_sets' => $local_test_set,
               'output_file' => $test_set_file,
               'debug' => $verbose,
            };

            my $ret_code = write_test_set($test_set_spec);
            next RUN if $ret_code ne $NO_ERROR;

            $ret_code = Mortar::Common::Tools::SystemUtils::dos2unix({
               'input' => $test_set_file,
               'output' => $test_set_file,
               'overwrite' => $TRUE,
            });
         }
   
         my $test_master_push_cmd = 'push ' . $test_set_file;
         $test_master_push_cmd .= ' ' . $remote_test_master;

         $local_test_set->{'pre_test'} = _default($local_test_set->{'pre_test'}, []);
         $local_test_set->{'post_test'} = _default($local_test_set->{'post_test'}, []);

         my @initial_pre_test = @{$local_test_set->{'pre_test'}};
         my @initial_post_test = @{$local_test_set->{'post_test'}};

         LOOP: for (my $test_loop = 1; $test_loop <= $total_test_loops; $test_loop++) {
            if (defined $repeat_until_elapsed_time) {
               last RUN if $repeat_until_elapsed_time < $total_test_time;
            }

            if ($total_test_loops > 1) {
               create_archive($attempt_archive, 'iter');
            }

            if ($test_loop > 1) {
               # don't re-upload all the files
               $local_test_set->{'pre_test'} = [];
               foreach my $pre_test_cmd (@initial_pre_test) {
                  if ($pre_test_cmd !~ m/^push/i) {
                     push(@{$local_test_set->{'pre_test'}}, $pre_test_cmd);
                  }
               }
            }

            if ($test_loop < $total_test_loops) {
               # don't delete the input files
               $local_test_set->{'post_test'} = [];
               foreach my $post_test_cmd (@initial_post_test) {
                  if ($post_test_cmd !~ m/^shell rm/i) {
                     push(@{$local_test_set->{'post_test'}}, $post_test_cmd);
                  }
               }
            }

            unshift(@{$local_test_set->{'pre_test'}}, $test_master_push_cmd);

            if (defined $local_test_set->{'archive_input_files'}) {
               my $file_list = $local_test_set->{'archive_input_files'};
               $file_list = [$file_list] if not ref($file_list);
               $file_list = [$file_list] if ref($file_list) ne 'ARRAY';

               foreach my $input_file_to_archive (@$file_list) {
                  my $input_file_base_name = basename($input_file_to_archive);
                  my $archived_input_file = $run_archive . 'input_files/' . $input_file_base_name;
                  
                  system_cmd([
                     'mkdir', '"' . $run_archive . 'input_files"',
                  ]) if not -d $run_archive . 'input_files';

                  Mortar::Common::Tools::SystemUtils::dos2unix({
                     'input' => $input_file_to_archive,
                     'output' => $archived_input_file,
                     'overwrite' => $FALSE,
                  }) if not -e $archived_input_file and -T $input_file_to_archive;

                  system_cmd([
                     'cp', '-f',
                     '"' . $input_file_to_archive . '"',
                     '"' . $archived_input_file . '"',
                  ]) if not -e $archived_input_file and -B $input_file_to_archive;

                  foreach my $cmd_spec (@{$local_test_set->{'pre_test'}}) {
                     $cmd_spec =~ s/push $input_file_to_archive/push $archived_input_file/i;
                  }
               }
            }

            if ($local_test_set->{'test_category'} eq 'voice') {
               # determine the vocoder scenario, update as needed

            }

            my $start_time = time();
            # run test
            my $test_result = run_test_set($local_test_set);
            my $end_time = time();
            $ErrorHandling = $curr_err_handling;

            if (defined $local_test_set->{'debug'}->{'elapsed_time'}) {
               $total_test_time = $total_test_time + $local_test_set->{'debug'}->{'elapsed_time'};
            } else {
               $total_test_time = $total_test_time + ($end_time - $start_time);
            }

            if ($local_test_set->{'result'} eq 'pass' and $tunnel_mode == $TRUE) {
               $local_test_set->{'result'} = 'pending' if not $stability_test and $require_tunnel_mode_analysis;
            }

            $local_test_set->{'result'} = 'pass (retest)'
               if $local_test_set->{'result'} eq 'pass' and $attempt > 1;

            store $local_test_set, ARCHIVE() . 'test_result.dat' if defined $local_test_set->{'archive'};

            if ($test_loop < $total_test_loops) {
               next LOOP if $test_set->{'result'} =~ m/^(pass|pending)/i;
            }

            $attempt_result = $local_test_set->{'result'};
            push(@$test_result_list, $local_test_set);

            if ($total_test_loops > 1) {
               move_archive($attempt_archive, $attempt_result) if -d $attempt_archive;
               push(@archives_to_rename,
                  {'path' => $attempt_archive, 'result' => $attempt_result},
               ) if -d $attempt_archive;
            }

            $run_result = $attempt_result;
            last ATTEMPT if $attempt_result =~ m/^(pass|pending)/i;
      
            $ErrorHandling = $ERR_LOG;
            $total_crash_count = $total_crash_count + 1 if $attempt_result eq 'timeout';
            $total_crash_count = $total_crash_count + 1 if $attempt_result eq 'crash';

            my $get_ebi_dump = $FALSE;
            $get_ebi_dump = $TRUE if $get_ebi_memory_dump_on_fail and $attempt_result eq 'fail';
            $get_ebi_dump = $TRUE if $get_ebi_memory_dump_on_timeout and $attempt_result eq 'timeout';
            $get_ebi_dump = $TRUE if $get_ebi_memory_dump_on_timeout and $attempt_result eq 'crash';
            $get_ebi_dump = $TRUE if $get_ebi_memory_dump_on_abort and $attempt_result eq 'aborted';

            if ($get_ebi_dump) {
               # will set DL mode if the target is still up
               get_qpst_memory_dump();

               # if memory download has occurred, reset is required
               if (not $reset_on_test_fail) {
                  run_target_reset() if $attempt < $max_attempts;
                  run_target_reset($FALSE) if $attempt >= $max_attempts;
               }
            }

            $ErrorHandling = $user_err_handling;

            my $reset_target = $FALSE;
            $reset_target = $TRUE
               if $attempt_result eq 'fail' and $reset_on_test_fail;
      
            $reset_target = $TRUE if $attempt_result eq 'init_fail';
            $reset_target = $TRUE if $attempt_result eq 'timeout';
            $reset_target = $TRUE if $attempt_result eq 'crash';
            $reset_target = $TRUE if $attempt_result eq 'aborted';
      
            run_target_reset() if $reset_target and $attempt < $max_attempts;
            run_target_reset($FALSE) if $reset_target and $attempt >= $max_attempts;

            run_afe_init_test();

            next ATTEMPT;
         }
      }

      $run_result = 'fail' if $attempt_result eq 'init_fail';
      $run_result = 'fail' if $attempt_result eq 'timeout';
      $run_result = 'fail' if $attempt_result eq 'aborted';
      $run_result = 'fail' if $attempt_result eq 'crash';

      move_archive($run_archive, $run_result) if -d $run_archive;
      push(@archives_to_rename,
         {'path' => $run_archive, 'result' => $run_result},
      ); # push this one, so in cleanup we copy the PERL log
   }

   if (defined $repeat_until_elapsed_time) {
      goto RUN if $repeat_until_elapsed_time > $total_test_time;
   }

   $curr_err_handling = $ErrorHandling;
   $ErrorHandling = $ERR_LOG;

   PARSE: foreach my $log_file (@dlf_logs_to_parse) {
      if (-e $log_file) {
         my $pcm_file_hash = Mortar::Audio::Tools::LogUtils::extract_audio_log_packets({
            'input' => $log_file,
            'output_type' => 'hash',
            'echo' => $verbose,
            #'concatenate' => $TRUE,
            #'timing' => $TRUE,
         });

         my $apr_file_hash = Mortar::Audio::Tools::LogUtils::extract_audio_log_packets({
            'input' => $log_file,
            'output_type' => 'hash',
            'apr' => $TRUE,
            'echo' => $verbose,
         });
      }
   }

   $ErrorHandling = $curr_err_handling;
}

ExitTestCase();

sub run_test_set {
   # need to convert to ANDROID
   my $test_set = shift;

   my $manage_archive = $TRUE if defined $test_set->{'archive'};
   $manage_archive = _default($manage_archive, $FALSE);

   store $test_set, ARCHIVE() . 'test_set.dat' if $manage_archive;

   if (not defined $fwk_exe_file) {
      my @ls_fwk_exe = adb_cmd({
         'args' => 'shell ls ' . $framework_path . '/AdspTestFramework*',
         'timeout' => 60,
      });

      foreach my $ls_line (@ls_fwk_exe) {
         next if not defined $ls_line;

         $ls_line =~ s/\x0d//i;

         next if not length($ls_line);
         next if $ls_line !~ m/AdspTestFramework/i;
         next if $ls_line =~ m/\.so[a-z0-9]*$/i;
         next if $ls_line =~ m/no such file/i;
         next if $ls_line =~ m/not found/i;

         if (defined $fwk_exe_file) {
            ErrorMsg('multiple AdspTestFramework executables found on target');
            ExitTestCase($ERROR_ABORT);
         }

         $fwk_exe_file = $ls_line;
         chomp($fwk_exe_file);
      }

      if (not defined $fwk_exe_file) {
         ErrorMsg('AdspTestFramework executable not found on target');
         ExitTestCase($ERROR_ABORT);
      }
   }

   if (not $chmod_done) {
      my $adb_cmd_spec = {'timeout' => 60};

      $adb_cmd_spec->{'args'} = 'shell chmod 777 ' . $framework_path . '/*.so';
      adb_cmd($adb_cmd_spec);

      $adb_cmd_spec->{'args'} = 'shell chmod 777 ' . $fwk_exe_file;
      adb_cmd($adb_cmd_spec);

      $chmod_done = $TRUE;
   }

   my $make_dir_list = _default($test_set->{'mkdir'}, []);
   my $pre_test_cmds = _default($test_set->{'pre_test'}, []);
   my $post_test_cmds = _default($test_set->{'post_test'}, []);

   my $ret_code = $NO_ERROR;
   my $attempt_archive = ARCHIVE();

   # make sure server has started
   Mortar::Common::Tools::AdbUtils::start_adb_server();
   wait_for_adb_device();

   start_adb_logging({
      'main' =>  $attempt_archive . 'adb_main.txt',
      'kernel' => $attempt_archive . 'adb_kernel.txt',
      'top' => $attempt_archive . 'adb_top.txt',
   }) if $manage_archive and $enable_adb_logging;

   if ($enable_adb_debug_logging) {
      my $adb_cmd_spec = {'timeout' => 60};

      $adb_cmd_spec->{'args'} = 'shell mount -t debugfs debugfs /sys/kernel/debug';
      adb_cmd($adb_cmd_spec);

      $adb_cmd_spec->{'args'} = 'shell echo -n "file soc-pcm.c +p" > /sys/kernel/debug/dynamic_debug/control';
      adb_cmd($adb_cmd_spec);

      $adb_cmd_spec->{'args'} = 'shell echo -n "file msm-dai-q6-v2.c +p" > /sys/kernel/debug/dynamic_debug/control';
      adb_cmd($adb_cmd_spec);

      $adb_cmd_spec->{'args'} = 'shell echo -n "file q6afe.c +p" > /sys/kernel/debug/dynamic_debug/control';
      adb_cmd($adb_cmd_spec);
   }

   foreach my $dest_dir (@$make_dir_list) {
      $ret_code = Mortar::QDSP::TestFrameworkUtils::make_android_remote_dir($dest_dir);
      return $ret_code if $ret_code ne $NO_ERROR;
   }

   foreach my $cmd_spec (@$pre_test_cmds) {
      $ret_code = adb_cmd({'args' => $cmd_spec, 'timeout' => 60});
      return $ret_code if $ret_code ne $NO_ERROR;
   }

   $ret_code = start_test($test_set);

   if ($ret_code ne $NO_ERROR) {
      my $result = 'init_fail';
      $test_set->{'result'} = $result;

      move_archive($attempt_archive, $result) if -d $attempt_archive and $manage_archive;
      push(@archives_to_rename, {'path' => $attempt_archive, 'result' => $result})
         if -d $attempt_archive and $manage_archive;

      return $ret_code;
   }

   $ret_code = wait_for_test_end($test_set);
   my $rslt = $test_set->{'result'};

   if ($ret_code ne $NO_ERROR) {
      move_archive($attempt_archive, $rslt) if -d $attempt_archive and $manage_archive;
      return $ret_code;
   }

   foreach my $cmd_spec (@$post_test_cmds) {
      my $local_cmd_spec = $cmd_spec;
      if ($local_cmd_spec =~ m/^pull/i) {
         my @cmd_spec_words = split(/ /, $local_cmd_spec);
         $local_cmd_spec .= ' ' . ARCHIVE() if (scalar(@cmd_spec_words) < 3);
         $local_cmd_spec = forwardslash($local_cmd_spec);

         # adb pull does not like a trailing / on the end
         $local_cmd_spec =~ s/\/$//i;

         if (exists $test_set->{'media_data'}) {
            my $media_data = $test_set->{'media_data'};
            if (defined $media_data and ref($media_data) eq 'ARRAY') {
               ENTRY: foreach my $media_entry (@$media_data) {
                  if (defined $media_entry and ref($media_entry) eq 'HASH') {
                     next ENTRY if not exists $media_entry->{'file'};

                     my $file = $media_entry->{'file'};
                     if ($local_cmd_spec =~ m/ [^\s]*${file}/i) {
                        #print "found $media_entry->{'file'} in the media data\n";
                     }
                  }
               }
            }
         }
      }

      Mortar::Common::Tools::AdbUtils::adb_cmd({
         'args' => $local_cmd_spec,
         'timeout' => 60,
      });
   }

   move_archive($attempt_archive, $rslt) if -d $attempt_archive and $manage_archive;
   push(@archives_to_rename, {'path' => $attempt_archive, 'result' => $rslt})
      if -d $attempt_archive and $manage_archive;

   return $ret_code;
}

sub start_test {
   my $test_set = shift;

   my $cat_files = [];
   if (defined $test_set and ref($test_set) eq 'HASH') {
      $cat_files = _default($test_set->{'cat'}, $cat_files);
   }
   
   if ($enable_phone_logging and defined $test_set->{'archive'}) {
      allocate_phone() if not defined $phone;

      my $category = $test_set->{'test_category'};

      my $pcm_logging_enabled = _default(
         $enable_phone_pcm_logging,
         (defined $category and $category =~ /stability/i) ? $FALSE : $TRUE,
      );

      @additional_log_masks = (@orig_addtl_log_masks);
      push(@additional_log_masks, @pcm_log_masks) if $pcm_logging_enabled;
      configure_phone_logging() if defined $phone;
   }

   if ($enable_phone_log_per_iteration) {
      create_archive($attempt_archive, 'iter')
         if not $run_test_iterations_separately;
   }

   my $test_set_name = $test_set->{'test_set_number'};
   my $test_area = $test_set->{'test_category'};
   my $test_scope = $test_set->{'test_scope'};
   
   $test_set_name = lc($test_set_name);
   $test_set_name =~ s/${test_area}//ig if defined $test_area;

   $test_set_name = lc($test_scope) . '_' . $test_set_name if defined $test_scope;
   $test_set_name = lc($test_area) . '_' . $test_set_name if defined $test_area;
   $test_set_name =~ s/__/_/ig;
   $test_set_name =~ s/\\/_/ig;

   my $phone_log_file = ARCHIVE() . $test_set_name . '.dlf';
   start_aps_logging($phone_log_file) if defined $phone and defined $test_set->{'archive'};

   my $curr_err_handling = $ErrorHandling;
   $ErrorHandling = $ERR_LOG;
   adb_cmd({'args' => 'shell sync; insmod /data/adsp-test/' . basename($ion_driver_file)});
   adb_cmd({'args' => 'shell "echo 255 > /sys/module/ipc_router/parameters/debug_mask"'}) if $enable_ipc_router_debug;
   adb_cmd({'args' => 'shell "echo 255 > /sys/module/ipc_router_smd_xprt/parameters/debug_mask"'}) if $enable_ipc_router_debug;
   $ErrorHandling = $curr_err_handling;

   my $command = 'adb shell stop media; cd ' . dirname($fwk_exe_file) . '; ';
   $command .= 'chmod 755 /data/busybox/*; /data/busybox/busybox --install; export PATH=$PATH:/data/busybox; '
      if defined $test_framework_priority;
   $command .= 'cat ' . $remote_test_master . '; ';
   foreach my $cat_file (@$cat_files) {
      $command .= 'cat ' . $cat_file . '; ';
   }
   $command .= 'nice -n ' . $test_framework_priority . ' ' if defined $test_framework_priority;
   $command .= './' . basename($fwk_exe_file) . ' ' . $remote_test_master;
   
   PrintMsg('starting test set with the following command:');
   print $command . "\n\n";

   Mortar::Common::Tools::AdbUtils::start_adb_timer(90);
   my $ret_code = open(FWK, $command . ' |');
   Mortar::Common::Tools::AdbUtils::stop_adb_timer();

   return $NO_ERROR;
}

sub wait_for_test_end {
   my $test_set = shift;
   my $test_end_timeout = shift;

   if (not defined $test_end_timeout) {
      if (exists $test_set->{'timeout'}) {
         $test_end_timeout = $test_set->{'timeout'};
      }
      $test_end_timeout = _default($test_end_timeout, $default_test_set_timeout);
   }

   my $iterations = $test_set->{'iterations'};
   $test_set->{'completed_iterations'} = 0;

   my $iteration_duration = _default($test_set->{'duration'}, $default_test_iteration_timeout);
   my $iteration_start_time;
   my $iteration_timeout;
   my $current_iteration = 0;

   my $test_start_timeout = 30;
   my $polling_interval = 10;

   my $test_start_pattern = 'ENTERING ADSP TEST FRAMEWORK';
   my $test_end_pattern = 'LEAVING ADSP TEST FRAMEWORK';

   my $summary_start_pattern = 'ADSP TEST SUMMARY BEGIN';
   my $summary_end_pattern = 'ADSP TEST SUMMARY END';

   my $ion_driver_message_pattern = '^ION';
   my $fwk_error_message_pattern = '\(error\)';
   my $other_error_message_pattern = 'error';

   my $test_launcher_pattern = '\(High\).*TEST LAUNCHER';
   my $test_init_pass_pattern = 'apr_init is completed';
   my $test_init_fail_pattern = 'apr_init failed';
   my $test_fwk_version_pattern = '\(High\).*VERSION: ';
   my $test_abort_pattern = 'Segmentation fault';

   my $pattern_list = [
      $test_start_pattern,
      $test_end_pattern,
      $summary_start_pattern,
      $summary_end_pattern,
      $test_init_pass_pattern,
      $test_init_fail_pattern,
      $test_fwk_version_pattern,
      $test_abort_pattern,
      $test_launcher_pattern,
      $fwk_error_message_pattern,
      '\(High\).*WARNING',
      '\(High\).*\*',
      $other_error_message_pattern,
      $ion_driver_message_pattern,
   ];

   my $pattern = join('|', @$pattern_list);
   $pattern = '(' . $pattern . ')';

   my $test_start_detected = $FALSE;
   my $test_end_detected = $FALSE;
   my $test_abort_detected = $FALSE;
   my $test_init_fail_detected = $FALSE;
   my $test_summary_active = $FALSE;

   $test_set->{'debug'}->{'abort'} = 'false';
   $test_set->{'debug'}->{'timeout'} = 'false';
   $test_set->{'debug'}->{'crash'} = 'false';
   $test_set->{'debug'}->{'mem_leaks'} = 0;
   $test_set->{'debug'}->{'failures'} = 0;

   my $this_test_result = 'skipped';

   my $start_time = time();
   # for all iterations combined, does not renew
   my $deadline = $start_time + $test_start_timeout;

   # but if the iteration duration is known, use a different deadline
   if (defined $iterations and defined $iteration_duration) {
      # allow 10s extra buffer
      $iteration_timeout = $iteration_duration + 20;
   }

   my $is_playing = $FALSE;
   my $is_recording = $FALSE;

   my $tests_launched = 0;
   my $tests_stopped = 0;

   my @active_output_devices = ();
   if ($rec_dma_output_group) {
      $test_set->{'dma_output'} = _default($test_set->{'dma_output'}, {});
      my @device_list = keys(%{$test_set->{'dma_output'}});
      foreach my $out_device (@device_list) {
         push(@active_output_devices, $out_device)
            if $test_set->{'dma_output'}->{$out_device}->{'enabled'};
      }
   }

   my $current_time = time();

   my $status;
   my $fwk_out_log = $attempt_archive . 'adsp_test_output.txt';
   if (defined $test_set->{'archive'}) {
      my $status = open(LOG, '>>', $fwk_out_log);
      return AsiaFail(
         $FAILED, $FATALERR, __PACKAGE__ . '::wait_for_test_end -> ' .
         '\'open\' command failed for file \'' . backslash($fwk_out_log) .
         '\', ' . $!, $FAILED,
      ) if not $status;
   }
   
   while ($current_time <= $deadline) {
      #print 'start of loop, get time' . "\n";
      $current_time = time();

      my $timer_value = $deadline - $current_time;
      $timer_value = $timer_value + 15;
      last if $timer_value <= 0;

      Mortar::Common::Tools::AdbUtils::set_verbose($FALSE);
      Mortar::Common::Tools::AdbUtils::stop_adb_timer();
      Mortar::Common::Tools::AdbUtils::start_adb_timer($timer_value);
      Mortar::Common::Tools::AdbUtils::set_verbose($verbose);

      my $line = <FWK>;

      next if not defined $line;

      $line =~ s/\x0d//ig;
      chomp($line);

      #print 'print to test fwk log' . "\n";
      print LOG $line . "\n" if defined $test_set->{'archive'};
      #print 'print complete' . "\n";

      next if not length($line);

      if (defined $line) {
         my $match = ($line =~ m/${pattern}/i ? $TRUE : $FALSE);
         $match = $TRUE if $test_summary_active;
         next if not $match;

         print '                : ' . $line . "\n";

         if ($line =~ m/${test_start_pattern}/i) {
            $test_start_detected = $TRUE;
            $start_time = time();
            $deadline = $start_time + $test_start_timeout;
            next;
         }

         if ($line =~ m/${test_end_pattern}/i) {
            $test_end_detected = $TRUE;
            sleep(2.5);
            last;
         }

         if ($line =~ m/${test_abort_pattern}/i) {
            $test_abort_detected = $TRUE;
            $test_set->{'debug'}->{'abort'} = 'true';
            last;
         }

         if ($line =~ m/${test_init_pass_pattern}/i) {
            $test_init_fail_detected = $FALSE;
            $test_set->{'debug'}->{'apr_init'} = 'success';
            next;
         }

         if ($line =~ m/${test_init_fail_pattern}/i) {
            $test_init_fail_detected = $TRUE;
            $test_set->{'debug'}->{'apr_init'} = 'failed';
            last;
         }

         if ($line =~ m/${test_launcher_pattern}/i) {
            if ($line =~ m/TEST LAUNCHER SUCCESS.*iter (\d+)/i) {
               $current_iteration = $1;

               if (defined $daw_obj) {
                  if ($play_dma_input_group and not $is_playing) {
                     #print 'play daw obj' . "\n";
                     $daw_obj->play({'group' => 'dma_input'});
                     $is_playing = $TRUE;
                     #print 'daw obj is playing' . "\n";
                  }

                  if ($rec_dma_output_group and not $is_recording) {
                     my $recorder_list = [];
                     foreach my $out_device (@active_output_devices) {
                        my $recorder_spec = {'group' => 'dma_output'};
                        $recorder_spec->{'name'} = $out_device;
                        $recorder_spec->{'file'} = ARCHIVE() . $out_device;
                        $recorder_spec->{'file'} .= '_iter' . sprintf('%3d', $current_iteration)
                           if $iterations > 1;
                        $recorder_spec->{'file'} .= '_output.wav';

                        push(@$recorder_list, $recorder_spec);
                     }

                     #print 'record daw obj' . "\n";
                     $daw_obj->record($recorder_list);
                     $is_recording = $TRUE;
                     #print 'daw obj is recording' . "\n";
                  }
               }

               PrintMsg('start of test iteration ' . $current_iteration . ' detected');

               $deadline = time() + $test_end_timeout
                  if $current_iteration == 0;

               $deadline = time() + $iteration_timeout
                  if defined $iteration_timeout;

               $tests_launched = $tests_launched + 1;
               $tests_stopped = 0;
            } elsif ($line =~ m/TEST LAUNCHER, JOIN SUCCESSFUL.*iter (\d+)/i) {
               # iteration is over, allow 15s for the next iteration to start
               if ($is_playing or $is_recording) {
                  #print 'stop daw obj' . "\n";
                  $daw_obj->stop_all() if defined $daw_obj;
                  $is_playing = $FALSE;
                  $is_recording = $FALSE;
                  #print 'daw obj stopped' . "\n";
               }

               PrintMsg('end of test iteration ' . $current_iteration . ' detected');
               $deadline = time() + 15 if defined $iteration_timeout;

               $tests_stopped = $tests_stopped + 1;

               if ($tests_stopped == $tests_launched) {
                  $tests_launched = 0;
                  $test_set->{'completed_iterations'} = $test_set->{'completed_iterations'} + 1;

                  if ($iterations > $current_iteration + 1 and $enable_phone_log_per_iteration) {
                     create_archive($attempt_archive, 'iter')
                        if not $run_test_iterations_separately;

                     my $phone_log_file = ARCHIVE() . 'phone_log.dlf';
                     start_aps_logging($phone_log_file) if defined $phone;
                  }
               }
            }

            next;
         }

         if ($line =~ m/${summary_start_pattern}/i) {
            $test_summary_active = $TRUE;
            next;
         }

         if ($line =~ m/${summary_end_pattern}/i) {
            $test_summary_active = $FALSE;
            next;
         }

         if ($line =~ m/${ion_driver_message_pattern}/i) {
            if ($line =~ m/success/i) {
               # only need to set the success message once,
               # don't want to override a fail msg with the next success
               if (not exists $test_set->{'debug'}->{'ion_alloc'}) {
                  $test_set->{'debug'}->{'ion_alloc'} = 'success';               
               } elsif (not defined $test_set->{'debug'}->{'ion_alloc'}) {
                  $test_set->{'debug'}->{'ion_alloc'} = 'success';
               }
            } else {
               $test_set->{'debug'}->{'ion_alloc'} = 'failed';
            }
            next;
         }

         if ($line =~ m/${fwk_error_message_pattern}/i) {
            if (not exists $test_set->{'debug'}->{'fwk_error'}) {
               $test_set->{'debug'}->{'fwk_error'} = $line;               
            } elsif (not defined $test_set->{'debug'}->{'fwk_error'}) {
               $test_set->{'debug'}->{'fwk_error'} = $line;
            }
         }

         if ($line =~ m/${other_error_message_pattern}/i) {
            # in case some message with the pattern 'error'
            # precedes a test framework error, report that error
            # as well.
            #
            # if a framework error is first, this debug info is
            # not required.
            if (not exists $test_set->{'debug'}->{'fwk_error'}) {
               $test_set->{'debug'}->{'other_error'} = $line;               
            } elsif (not defined $test_set->{'debug'}->{'fwk_error'}) {
               $test_set->{'debug'}->{'other_error'} = $line;
            }
         }

         if ($test_summary_active) {
            # process test summary
            if ($line =~ m/TOTAL NUMBER OF TEST SETS\s+=\s+(\d+)/i) {
               my $total = $1;
               $summary->{'test_sets'}->{'total'} += $total;
               next;
            }
   
            if ($line =~ m/TOTAL TEST SETS WITH MEMLEAKS\s+=\s+(\d+)/i) {
               my $leaks = $1;
               $summary->{'test_sets'}->{'mem_leaks'} += $leaks;
               $test_set->{'debug'}->{'mem_leaks'} += $leaks;

               if ($leaks > 0) {
                  $this_test_result = 'fail';
                  downgrade_test_case_result($FAIL,
                     'detected ' . $leaks . ' test set memory leak(s)',
                  );
               }

               next;
            }
   
            if ($line =~ m/TOTAL NUMBER OF INDIVIDUAL TESTS\s+=\s+(\d+)/i) {
               my $tests = $1;
               $summary->{'tests'}->{'total'} += $tests;
               next;
            }
   
            if ($line =~ m/TOTAL INDIVIDUAL TEST PASS\s+=\s+(\d+)/i) {
               my $pass = $1;
               $summary->{'tests'}->{'pass'} += $pass;
               
               if ($pass > 0) {
                  $this_test_result = 'pass' if $this_test_result eq 'skipped'
               };
               next;
            }
   
            if ($line =~ m/TOTAL INDIVIDUAL TEST FAIL\s+=\s+(\d+)/i) {
               my $fail = $1;
               $summary->{'tests'}->{'fail'} += $fail;
               $test_set->{'debug'}->{'failures'} = $fail;

               if ($fail > 0) {
                  $this_test_result = 'fail';
                  downgrade_test_case_result($FAIL,
                     'detected ' . $fail . ' failed test(s)',
                  );
               }

               next;
            }

            if ($line =~ m/TOTAL INVALID ADSP MESSAGES\s+=\s+(\d+)/i) {
               my $invalid = $1;
               $summary->{'fwk'}->{'invalid_adsp_msg'} += $invalid;

               if ($invalid > 0) {
                  $this_test_result = 'fail';
                  downgrade_test_case_result($FAIL,
                     'detected ' . $invalid . ' invalid adsp message(s)',
                  );
               }

               next;
            }

            if ($line =~ m/TOTAL TEST SETS WITH FWK MEMLEAKS\s+=\s+(\d+)/i) {
               my $leaks = $1;
               $summary->{'fwk'}->{'mem_leaks'} += $leaks;
               $test_set->{'debug'}->{'mem_leaks'} += $leaks;

               if ($leaks > 0) {
                  $this_test_result = 'fail';
                  downgrade_test_case_result($FAIL, 
                     'detected ' . $leaks . ' fwk memory leak(s)',
                  );
               }

               next;
            }

            if ($line =~ m/TOTAL INDIVIDUAL TEST LAUNCH FAIL\s+=\s+(\d+)/i) {
               my $launch_fail = $1;
               $summary->{'fwk'}->{'test_launch_fail'} += $launch_fail;
               $test_set->{'debug'}->{'launch_failed'} += $launch_fail;

               if ($launch_fail > 0) {
                  $this_test_result = 'fail';
                  downgrade_test_case_result($FAIL,
                     'detected ' . $launch_fail . ' test launch failure(s)',
                  );
               }

               next;
            }

            if ($line =~ m/TOTAL COSIM RESET FAIL\s+=\s+(\d+)/i) {
               my $reset_fail = $1;
               $summary->{'fwk'}->{'cosim_reset_fail'} += $reset_fail;

               if ($reset_fail > 0) {
                  $this_test_result = 'fail';
                  downgrade_test_case_result($FAIL,
                     'detected ' . $reset_fail . ' test launch failure(s)',
                  );
               }

               next;
            }
         }
      }
   }

   $current_time = time();
   my $elapsed_time = $current_time - $start_time;
   PrintMsg('wait_for_test_end: while loop finished, elapsed time = ' . $elapsed_time);
   $test_set->{'debug'}->{'elapsed_time'} = $elapsed_time;
   $test_set->{'debug'}->{'completed_iterations'} = $elapsed_time;

   if ($is_playing or $is_recording) {
      #print 'stop daw obj' . "\n";
      $daw_obj->stop_all() if defined $daw_obj;
      $is_playing = $FALSE;
      $is_recording = $FALSE;
      #print 'daw obj stopped' . "\n";
   }

   print '   -> close test fwk file handle' . "\n";
   close(FWK);

   print '   -> close test fwk file log' . "\n";
   close(LOG);

   print '   -> close complete, stop adb timer' . "\n";

   Mortar::Common::Tools::AdbUtils::stop_adb_timer();

   close_system_cmd_log() if is_system_cmd_log_open();
   stop_aps_logging() if $APS_IS_LOGGING;
   stop_adb_logging();

   sleep(2);

   if (not $test_end_detected) {
      $this_test_result = 'timeout';
      $test_set->{'debug'}->{'timeout'} = 'true';

      if (has_phone_crashed()) {
         $this_test_result = 'crash';
         $test_set->{'debug'}->{'crash'} = 'true';
      }
   }
   $this_test_result = 'init_fail' if $test_init_fail_detected;
   $this_test_result = 'aborted' if $test_abort_detected;

   if (defined $test_set) {
      PrintMsg('test set result: ' . $this_test_result);
      DumperMsg('test set debug info', $test_set->{'debug'}) if $this_test_result ne 'pass';

      $test_set->{'result'} = $this_test_result;
      $test_set->{'elapsed_time'} = $elapsed_time;
   }

   return AsiaFail(
      $FAILED, $FATALERR, 'wait_for_test_end -> test initialization ' .
      'failed', $FAILED,
   ) if $test_init_fail_detected;

   return AsiaFail(
      $FAILED, $FATALERR, 'wait_for_test_end -> test abort detected ' .
      'during iteration number ' . $current_iteration,
      $FAILED,
   ) if $test_abort_detected;

   return AsiaFail(
      $TIMEOUT_ERROR, $FATALERR, 'wait_for_test_end -> failed to detect ' .
      'test start message within ' . $test_start_timeout . ' seconds',
      $TIMEOUT_ERROR,
   ) if not $test_start_detected;

   if (not $test_end_detected) {
      my $timeout_msg = 
         'test end message within ' . $test_end_timeout . ' seconds';

      $timeout_msg = 
         'test iteration end message within ' . $iteration_timeout . ' seconds'
         if defined $iteration_timeout;

      return AsiaFail(
         $TIMEOUT_ERROR, $FATALERR, 'wait_for_test_end -> failed to detect ' .
         $timeout_msg . ' during iteration number ' . $current_iteration,
         $TIMEOUT_ERROR,
      );
   }

   return $NO_ERROR;
}

sub cleanup {
   $ErrorHandling = $ERR_NONE;

   Mortar::Common::Tools::QpstUtils::stop_qpst_configuration() if $shutdown_qpst_server;

   stop_aps_logging() if defined $phone and $APS_IS_LOGGING;

   $ErrorHandling = $ERR_LOG;

   if ($enable_adb_cleanup) {
      adb_cmd({'args' => 'shell start media'});
      Mortar::Common::Tools::AdbUtils::stop_adb_server();
      Mortar::Common::Tools::AdbUtils::stop_adb_cleanup_process();
   }

   DumperMsg('framework execution summary: ', $summary);
   Mortar::QDSP::TestFrameworkUtils::display_test_summary($test_result_list, $master_test_list);
   Mortar::QDSP::TestFrameworkUtils::create_xml_result_file($test_result_list, $xml_result_path);
   Mortar::QDSP::TestFrameworkUtils::create_text_summary_file('test_summary.txt', $test_result_list, $master_test_list);

   my ($hours, $min, $sec, $min_in_sec);
   $sec = $total_test_time % 60;
   $min = ($total_test_time - $sec) / 60;
   $hours = int($min / 60);
   $min = $min % 60;

   print "\n" . '   -> total test time      : ' . sprintf('%d:%.2d:%.2d', $hours, $min, $sec) . "\n";
   print '   -> total crashes        : ' . $total_crash_count . "\n\n";

   $daw_obj->cleanup() if defined $daw_obj;
   $daw_obj = undef;

   foreach my $rm_file (@cleanup_files) {
      system_cmd(['rm', '-f', '"' . $rm_file . '"'])
         if defined $rm_file and -e $rm_file;
   }

   my @run_folders = ();
   foreach my $spec (@archives_to_rename) {
      next if not defined $spec;
      next if not ref($spec);
      next if ref($spec) ne 'HASH';

      my $source = $spec->{'path'};
      $source = forwardslash($source);
      $source =~ s/\/$//i;

      my $rslt = $spec->{'result'};
      move_archive($source, $rslt) if -d $source;

      if (basename($source) =~ m/^run/i) {
         my $new_archive = $source . '_' . $rslt;
         push(@run_folders, $new_archive) if -d $new_archive;
         push(@run_folders, $source) if -d $source;
      }
   }

   foreach my $run_folder (@run_folders) {
      next if not defined $run_folder;
      next if not -d $run_folder;

      if (defined $TestCaseLog) {
         $run_folder = forwardslash($run_folder);
         $run_folder =~ s/\/$//i;

         my $log_base_name = basename($TestCaseLog);
         my ($exit_code, $cmd_out) = system_cmd([
            'cp', '-f',
            '"' . $TestCaseLog . '"',
            '"' . $run_folder . '/perl_stdout.txt"',
         ]);
      }
   }

   return $NO_ERROR;
}

sub move_archive {
   my ($source, $result) = @_;

   $source = forwardslash($source);
   $source =~ s/\/$//i;

   my $new_archive = $source . '_' . $result;
   my ($exit_code, $cmd_out) = system_cmd(
      ['mv', '-f', '"' . $source . '"', '"' . $new_archive . '"'],
   );
   return $FAILED if $exit_code ne 0;

   for (my $i = 0; $i <= @dlf_logs_to_parse; $i++) {
      my $dlf_file = $dlf_logs_to_parse[$i];
      $dlf_file = forwardslash($dlf_file);

      if ($dlf_file =~ s/${source}/$new_archive/ig) {
         $dlf_logs_to_parse[$i] = $dlf_file;
      }
   }

   my $archive = ARCHIVE();
   if ($archive =~ s/${source}/$new_archive/ig) {
       ARCHIVE($archive);
   }
   
   return $NO_ERROR;
}

sub has_phone_crashed {
   my $com_port = shift;
   $com_port = get_phone_com_port() if defined $phone;

   if (defined $com_port) {
      my $phone_ready = Mortar::Common::Tools::QpstUtils::is_phone_in_ready_state($com_port);
      $com_port = undef if not defined $phone_ready or not $phone_ready;
   }
   $com_port = wait_for_qpst_com_port() if not defined $com_port;

   my $dl_mode = Mortar::Common::Tools::QpstUtils::is_phone_in_download_mode($com_port);
   my $dling_mode = $FALSE;
   
   if (not $dl_mode) {
      $dling_mode = Mortar::Common::Tools::QpstUtils::is_phone_downloading_memory_dumps($com_port);
   }

   if ($dl_mode or $dling_mode) {
      return $TRUE;
   }

   return $FALSE;
}

sub get_qpst_memory_dump {
   my $com_port = shift;
   $com_port = get_phone_com_port() if defined $phone;

   if (defined $com_port) {
      my $phone_ready = Mortar::Common::Tools::QpstUtils::is_phone_in_ready_state($com_port);
      $com_port = undef if not defined $phone_ready or not $phone_ready;
   }
   $com_port = wait_for_qpst_com_port() if not defined $com_port;

   my $dl_mode = Mortar::Common::Tools::QpstUtils::is_phone_in_download_mode($com_port);
   my $dling_mode = $FALSE;
   
   if (not $dl_mode) {
      $dling_mode = Mortar::Common::Tools::QpstUtils::is_phone_downloading_memory_dumps($com_port);
   }

   if (not $dl_mode and not $dling_mode) {
      PrintMsg('forcing target to download mode');
      Mortar::Common::Tools::QpstUtils::reset_com_port({
         'com_port' => $com_port,
         'download_mode' => $TRUE,
      });

      sleep(5);

      $com_port = wait_for_qpst_com_port();

      $dl_mode = Mortar::Common::Tools::QpstUtils::is_phone_in_download_mode($com_port);
      $dling_mode = $FALSE;

      if (not $dl_mode) {
         $dling_mode = Mortar::Common::Tools::QpstUtils::is_phone_downloading_memory_dumps($com_port);
      }
   }

   return AsiaFail(
      $FAILED, $FATALERR, 'get_qpst_memory_dump -> ' . 
      'unable to collect QPST memory dump, com port ' . $com_port .
      ' not in download mode', $FAILED,
   ) if not $dl_mode and not $dling_mode;
         
   my $current_archive = ARCHIVE();
   my $restore_slash = $FALSE;
   $restore_slash = $TRUE if $attempt_archive =~ s/\/$//i;

   if ($current_archive =~ m/${attempt_archive}(_[a-z]+)/i) {
      $attempt_archive .= $1;
   }
   $attempt_archive .= '/' if $restore_slash;

   if (not $dling_mode) {
      my $mem_dump_file = $attempt_archive . 'mem_dump_' . $com_port;
      PrintMsg('attempting qpst memory debug download. this may take a few minutes.');
      return Mortar::Common::Tools::QpstUtils::get_memory_dump({
         'com_port'          => $com_port,
         'absolute_location' => $mem_dump_file,
         'debug'             => $verbose,
      });
   } else {
      PrintMsg('crash detected, QPST is downloading the memory dumps. waiting for this to complete');
      my $wait_result = wait_for_qpst_download_to_complete({'com_port' => $com_port});
      PrintMsg('QPST download complete, saving the logs');
      system_cmd({'cmd_args' => 'md ' . backslash($attempt_archive) . 'Port_' . $com_port});
      system_cmd({'cmd_args' => 'mv c:/ProgramData/Qualcomm/QPST/Sahara/Port_' . $com_port . '/* ' . $attempt_archive . 'Port_' . $com_port})
         if -d 'C:/ProgramData/Qualcomm/QPST/Sahara/Port_' . $com_port;
      system_cmd({'cmd_args' => 'mv "C:/Documents and Settings/All Users/Application Data/Qualcomm/QPST/Sahara/Port_' . $com_port . '/*"' . $attempt_archive . 'Port_' . $com_port})
         if -d 'C:/Documents and Settings/All Users/Application Data/Qualcomm/QPST/Sahara/Port_' . $com_port;

   }
}

sub process_test_set_configurations {
   my $master_test_list = shift;
   $master_test_list = _default($master_test_list, []);

   my $mobile_os_type = shift;
   $mobile_os_type = _default($mobile_os_type, 'qnx');
   $mobile_os_type = lc($mobile_os_type);

   my $input_test_list = [@$master_test_list]; # clone
   splice(@$master_test_list, 0);

   my $test_sets_with_errors = [];
   SET: foreach my $test_set (@$input_test_list) {
      my $problem_list = [];
      my $config_spec;

      # before the test
      my $dir_to_make_on_phone = [];
      my $files_to_copy_to_phone = [];
      my $files_to_cat_on_phone = [];
      my $test_input_files_to_copy_to_archive = [];

      # after the test
      my $files_to_copy_from_phone = [];
      my $folders_to_copy_from_phone = [];
      my $files_to_cleanup_from_phone = [];

      my $copy_spec = {};

      my @config_files = ();

      $curr_apt_level = $test_set->{'test_level'};
      $curr_test_category = $test_set->{'test_category'};
      $curr_test_scope = $test_set->{'test_scope'};

      generate_archive_root($test_set);

      ENTRY: foreach my $test_entry (@{$test_set->{'entries'}}) {
         my $test_type = $test_entry->{'test_type'};

         if (exists $test_entry->{'config_file'}) {
            my $config_file_value = $test_entry->{'config_file'};

            if (defined $config_file_value) {
               my $config_type = get_config_type($test_type);
               my $config_file = $config_file_value;
               if (!-e $config_file) {
                  # resolve the relative path
                  $config_file = $test_content_root . '/' . $config_file;

                  if (!-e $config_file) {
                     my $reason = 'unable to find test config file, not found in:' . "\n" .
                        $test_entry->{'config_file'} . "\n" . $config_file;
                     push(@$problem_list, {
                        'test_type' => $test_type,
                        'config_file' => $config_file,
                        'reason' => $reason,
                     });
                     next ENTRY;
                  }

                  CFG: foreach my $verified_cfg_file (@config_files) {
                     # CFG has already bee verified
                     next ENTRY if lc(forwardslash($config_file)) eq lc(forwardslash($verified_cfg_file));
                  }
               }

               my $curr_err_handling = $ErrorHandling;
               $ErrorHandling = $ERR_NONE;

               $config_spec = parse_config_file($config_file, $config_type);
               if (not defined $config_spec and GetAsiaLastError() ne $NO_ERROR) {
                  push(@$problem_list, {
                     'test_type' => $test_type,
                     'config_file' => $config_file,
                     'reason' => GetAsiaErrorDescription(),
                  });
               }
               $test_set->{'cfg_params'}->{$config_file} = $config_spec;

               $ErrorHandling = $curr_err_handling;
               push(@config_files, $config_file);

               # process CFG_PATH if exists
               if (exists $config_spec->{'cfg_path'} and defined $config_spec->{'cfg_path'}) {
                  my $config_type = 'dma';
                  my $dma_config_value = $config_spec->{'cfg_path'};
                  my $dma_config_file = $dma_config_value;
                  if (!-e $dma_config_file) {
                     # resolve the relative path
                     $dma_config_file = $test_content_root . '/' . $dma_config_file;

                     if (!-e $dma_config_file) {
                        my $reason = 'unable to find test config file, not found in:' . "\n" .
                           $config_spec->{'cfg_path'} . "\n" . $dma_config_file;

                        push(@$problem_list, {
                           'test_type' => $test_type,
                           'config_file' => $dma_config_file,
                           'reason' => $reason,
                        });
                        next ENTRY;
                     }
                  }

                  $copy_spec = {'destination' => $dma_config_value};
                  $copy_spec->{'source'} = $dma_config_value if -e $dma_config_value;
                  $copy_spec->{'source'} = $dma_config_file if -e $dma_config_file;

                  push(@$files_to_copy_to_phone, $copy_spec);
                  push(@$files_to_cleanup_from_phone, $copy_spec->{'destination'});
                  push(@$test_input_files_to_copy_to_archive, $dma_config_file);

                  my $dma_config_spec = parse_config_file($dma_config_file, $config_type);
                  if (not defined $dma_config_spec and GetAsiaLastError() ne $NO_ERROR) {
                     push(@$problem_list, {
                        'test_type' => $test_type,
                        'config_file' => $dma_config_file,
                        'reason' => GetAsiaErrorDescription(),
                     });
                  }
                  $config_spec->{'dma_cfg_params'} = $dma_config_spec;
               }

               $copy_spec = {'destination' => $config_file_value};
               
               $copy_spec->{'source'} = $config_file_value if -e $config_file_value;
               $copy_spec->{'source'} = $config_file if -e $config_file;

               push(@$files_to_copy_to_phone, $copy_spec);
               push(@$test_input_files_to_copy_to_archive, $copy_spec->{'source'});
               push(@$files_to_cleanup_from_phone, $copy_spec->{'destination'});
               push(@$files_to_cat_on_phone, $copy_spec->{'destination'});

               my $input_file_value;
               my $input_file;

               PARAM: foreach my $config_param (keys(%$config_spec)) {
                  if ($config_param =~ m/input.*file/i) {
                     my $param_value = $config_spec->{$config_param};
                     $param_value = [$param_value] if not ref($param_value);

                     INPUT: foreach $input_file_value (@$param_value) {
                        $input_file_value = forwardslash($input_file_value);
                        $input_file = $test_content_root;
                        $input_file .= '/' . $input_file_value;

                        if (!-e $input_file_value and !-e $input_file) {
                           my $reason = 'test input file does not exist:' . "\n" .
                              $input_file_value . "\n" . $input_file;
                           push(@$problem_list, {
                              'test_type' => $test_type,
                              'config_file' => $config_file,
                              'reason' => $reason,
                           });
                           next INPUT;
                        }

                        $copy_spec = {'destination' => $input_file_value};
                        $copy_spec->{'source'} = $input_file_value if -e $input_file_value;
                        $copy_spec->{'source'} = $input_file if -e $input_file;

                        push(@$files_to_copy_to_phone, $copy_spec);

                        push(@$test_input_files_to_copy_to_archive, $copy_spec->{'source'});
                        push(@$files_to_cleanup_from_phone, $copy_spec->{'destination'});

                        next INPUT;
                     }
                  }

                  if ($config_param =~ m/ref.*file/i) {
                     next PARAM if $config_param =~ m/offset/i;

                     #next if $test_set->{'tunnel_mode'};

                     if (exists $config_spec->{'test_pass_criterion'}) {
                        next if $config_spec->{'test_pass_criterion'};
                     }

                     my $param_value = $config_spec->{$config_param};
                     $param_value = [$param_value] if not ref($param_value);

                     REF: foreach my $reference_file_value (@$param_value) {
                        $reference_file_value = forwardslash($reference_file_value);
                        my $reference_file = $test_content_root;
                        $reference_file .= '/' . $reference_file_value;

                        if (!-e $reference_file_value and !-e $reference_file) {
                           my $reason = 'test reference file does not exist:' . "\n" .
                              $reference_file_value . "\n" . $reference_file;
                           push(@$problem_list, {
                              'test_type' => $test_type,
                              'config_file' => $config_file,
                              'reason' => $reason,
                           });
                           next REF;
                        }

                        $copy_spec = {'destination' => $reference_file_value};
                        $copy_spec->{'source'} = $reference_file_value if -e $reference_file_value;
                        $copy_spec->{'source'} = $reference_file if -e $reference_file;

                        push(@$files_to_copy_to_phone, $copy_spec);
                        push(@$test_input_files_to_copy_to_archive, $copy_spec->{'source'});
                        push(@$files_to_cleanup_from_phone, $copy_spec->{'destination'});

                        next REF;
                     }
                  }

                  if ($config_param =~ m/license.*file/i) {
                     my $param_value = $config_spec->{$config_param};
                     $param_value = [$param_value] if not ref($param_value);

                     LICENSE: foreach my $license_file_value (@$param_value) {
                        $license_file_value = forwardslash($license_file_value);
                        my $license_file = $test_content_root;
                        $license_file .= '/' . $license_file_value;

                        if (!-e $license_file_value and !-e $license_file) {
                           my $reason = 'test license file does not exist:' . "\n" .
                              $license_file_value . "\n" . $license_file;
                           push(@$problem_list, {
                              'test_type' => $test_type,
                              'config_file' => $config_file,
                              'reason' => $reason,
                           });
                           next LICENSE;
                        }

                        $copy_spec = {'destination' => $license_file_value};
                        $copy_spec->{'source'} = $license_file_value if -e $license_file_value;
                        $copy_spec->{'source'} = $license_file if -e $license_file;

                        push(@$files_to_copy_to_phone, $copy_spec);

                        push(@$test_input_files_to_copy_to_archive, $copy_spec->{'source'});
                        push(@$files_to_cleanup_from_phone, $copy_spec->{'destination'});

                        next LICENSE;
                     }
                  }

                  if ($config_param =~ m/cfg.*file/i) {
                     next if $config_param =~ m/use.*from/i;

                     my $param_value = $config_spec->{$config_param};
                     $param_value = [$param_value] if not ref($param_value);

                     CFG_FILE: foreach my $cfg_file_value (@$param_value) {
                        $cfg_file_value = forwardslash($cfg_file_value);
                        my $cfg_file = $test_content_root;
                        $cfg_file .= '/' . $cfg_file_value;

                        if (!-e $cfg_file_value and !-e $cfg_file) {
                           my $reason = 'test ' . $config_param . ' does not exist:' . "\n" .
                              $cfg_file_value . "\n" . $cfg_file;
                           push(@$problem_list, {
                              'test_type' => $test_type,
                              'config_file' => $config_file,
                              'reason' => $reason,
                           });
                           next CFG_FILE;
                        }

                        $copy_spec = {'destination' => $cfg_file_value};
                        $copy_spec->{'source'} = $cfg_file_value if -e $cfg_file_value;
                        $copy_spec->{'source'} = $cfg_file if -e $cfg_file;

                        push(@$files_to_copy_to_phone, $copy_spec);

                        push(@$test_input_files_to_copy_to_archive, $copy_spec->{'source'});
                        push(@$files_to_cleanup_from_phone, $copy_spec->{'destination'});

                        my $cfg_param_spec = parse_config_file($copy_spec->{'source'}, 'seq');
                        if (not defined $cfg_param_spec and GetAsiaLastError() ne $NO_ERROR) {
                           push(@$problem_list, {
                              'test_type' => $test_type,
                              'config_file' => $copy_spec->{'source'},
                              'reason' => GetAsiaErrorDescription(),
                           });
                        }
                        $config_spec->{$config_param} = $cfg_param_spec;
                        $test_set->{'cfg_params'}->{$copy_spec->{'source'}} = $cfg_param_spec;
                        next CFG_FILE;
                     }
                  }

                  if ($config_param =~ m/output.*file/i) {
                     my $param_value = $config_spec->{$config_param};
                     $param_value = [$param_value] if not ref($param_value);

                     OUTPUT: foreach my $output_file_value (@$param_value) {
                        $output_file_value = forwardslash($output_file_value);
                        $copy_spec = {'source' => $output_file_value};

                        push(@$files_to_copy_from_phone, $copy_spec);
                        push(@$files_to_cleanup_from_phone, $output_file_value);
                        push(@$dir_to_make_on_phone, dirname($output_file_value));

                        next OUTPUT;
                     }
                  }

                  if ($config_param =~ m/diff.*file/i) {
                     my $diff_file_value = $config_spec->{$config_param};
                     $copy_spec = {'source' => $diff_file_value};

                     push(@$files_to_copy_from_phone, $copy_spec);
                     push(@$files_to_cleanup_from_phone, $diff_file_value);
                     push(@$dir_to_make_on_phone, dirname($diff_file_value));

                     next PARAM;
                  }

                  if ($config_param =~ m/log.*folder/i) {
                     # still working on this
                     my $log_folder_value = $config_spec->{$config_param};
                     $log_folder_value = forwardslash($log_folder_value);
                     $copy_spec = {'source' => $log_folder_value};

                     push(@$folders_to_copy_from_phone, $copy_spec);
                     push(@$dir_to_make_on_phone, $log_folder_value);

                     next PARAM;
                  }

                  if ($config_param =~ m/num_([rt]x)_sessions/i) {
                     # device config param file
                     my $direction = $1;
                     $direction = lc($direction);

                     my $num_sessions = $config_spec->{$config_param};
                     $test_set->{$config_param} = $config_spec->{$config_param};

                     next PARAM;
                  }

                  if (exists $test_set->{'num_rx_sessions'}) {
                     if ($config_param =~ m/rx_session(\d+)_([^\s]+)/i) {
                        my $session_id = $1;
                        my $session_param = $2;
                     }
                  } elsif (exists $test_set->{'num_tx_sessions'}) {
                     if ($config_param =~ m/tx_session(\d+)_([^\s]+)/i) {
                        my $session_id = $1;
                        my $session_param = $2;
                     }
                  }

                  if ($config_param =~ m/dma_cfg_params/i) {
                     my $param_value = $config_spec->{$config_param};

                     DMA: foreach my $dma_cfg_key (keys(%$param_value)) {
                        next if $dma_cfg_key !~ m/([\S_]+)_in(put)?.*file/i;

                        my $device = $1;
                        $device =~ s/_//ig;
                        
                        my $dma_cfg_value = $param_value->{$dma_cfg_key};
                        $dma_cfg_value = forwardslash($dma_cfg_value);

                        # NOTE: afe test vectors are not stored in the same 
                        # test content location, rather in the DSP build location
                        my $dma_cfg_file = $test_content_root;
                        $dma_cfg_file .= '/' . $dma_cfg_value;

                        if (!-e $dma_cfg_value and !-e $dma_cfg_file) {
                           my $reason = 'test dma input file does not exist for ' . $device .
                              ' device:' . "\n" . $dma_cfg_value . "\n" . $dma_cfg_file;
                           push(@$problem_list, {
                              'test_type' => $test_type,
                              'config_file' => $config_file,
                              'reason' => $reason,
                           });
                           next DMA;
                        }

                        $test_set->{'dma_input'}->{$device}->{'file'} = $dma_cfg_value
                           if -e $dma_cfg_value;
                        $test_set->{'dma_input'}->{$device}->{'file'} = $dma_cfg_file
                           if -e $dma_cfg_file;
                        $test_set->{'dma_input'}->{$device}->{'enabled'} = $FALSE;

                        next DMA;
                     }
                  }

                  if ($config_param =~ m/duration/i) {
                     $test_set->{'duration'} = $config_spec->{$config_param}
                        if not defined $test_set->{'duration'};
                     $test_set->{'duration'} = $config_spec->{$config_param}
                        if $config_spec->{$config_param} > $test_set->{'duration'};
                  }

                  if ($config_param =~ m/media_data/i) {
                     $test_set->{$config_param} = $config_spec->{$config_param};
                  }
               }

               if (exists $test_set->{'dma_input'} and defined $test_set->{'dma_input'}) {
                  if (exists $config_spec->{'command_sequence'}) {
                     extract_dma_input_configuration($config_spec->{'command_sequence'}, $test_set);
                  }
               }
            }
         }

         $test_set->{'tunnel_mode'} = _default($test_set->{'tunnel_mode'}, $FALSE);
      }

      if (@$problem_list) {
         $test_set->{'result'} = 'cfg error';
         push(@$test_sets_with_errors, {
            'test_set_number' => $test_set->{'test_set_number'},
            'problem_list' => $problem_list,
         });
         next SET;
      }

      if (@$test_input_files_to_copy_to_archive) {
         $test_set->{'archive_input_files'} = $test_input_files_to_copy_to_archive;
      }

      $test_set->{'pre_test'} = _default($test_set->{'pre_test'}, []);
      $test_set->{'post_test'} = _default($test_set->{'post_test'}, []);

      foreach my $file_spec (@{$files_to_copy_to_phone}) {
         my $source = $file_spec->{'source'};
         my $destination = _default(
            $file_spec->{'destination'},
            $framework_path,
         );

         # add framework root to relative paths
         $destination = $framework_path . '/' . $destination
            if $destination !~ m/^\//i;

         my $adb_cmd = 'push ' . $source . ' ' . $destination;
         push(@{$test_set->{'pre_test'}}, $adb_cmd);
      }

      my $pre_test_cleanup = [];
      foreach my $file_spec (@{$files_to_copy_from_phone}) {
         # probably need to give the ARCHIVE destination at run-time
         my $source = $file_spec->{'source'};
         my $destination = $file_spec->{'destination'};

         $source = forwardslash($source);
         # add framework root to relative paths
         $source = $framework_path . '/' . $source
            if $source !~ m/^\//i;

         my $adb_cmd = 'pull ' . $source;
         $adb_cmd .= ' ' . $destination if defined $destination;

         push(@{$test_set->{'post_test'}}, $adb_cmd);
         unshift(@{$pre_test_cleanup}, $source);
      }

      foreach my $rm_file (@{$pre_test_cleanup}) {
         unshift(@{$test_set->{'pre_test'}}, 'shell rm ' . $rm_file);
      }

      foreach my $file_spec (@{$folders_to_copy_from_phone}) {
         # probably need to give the ARCHIVE destination at run-time
         my $source = $file_spec->{'source'};
         my $destination = $file_spec->{'destination'};

         $source = forwardslash($source);
         $source =~ s/\*$//ig; # pull command cannot have a wildcard
         $source .= '/' if $source !~ m/\/$/i;

         # add framework root to relative paths
         $source = $framework_path . '/' . $source
            if $source !~ m/^\//i;

         my $adb_cmd = 'pull ' . $source;
         $adb_cmd .= ' ' . $destination if defined $destination;

         push(@{$test_set->{'post_test'}}, $adb_cmd);
         # rm command requires the wildcard
         push(@{$test_set->{'post_test'}}, 'shell rm ' . $source . '*');
         unshift(@{$pre_test_cleanup}, $source);
      }

      foreach my $rm_file (@{$files_to_cleanup_from_phone}) {
         # add framework root to relative paths
         $rm_file = $framework_path . '/' . $rm_file
            if $rm_file !~ m/^\//i;
         push(@{$test_set->{'post_test'}}, 'shell rm ' . $rm_file);
      }

      foreach my $cat_file (@{$files_to_cat_on_phone}) {
         $cat_file = forwardslash($cat_file);
         # add framework root to relative paths
         $cat_file = $framework_path . '/' . $cat_file
            if $cat_file !~ m/^\//i;

         $test_set->{'cat'} = _default($test_set->{'cat'}, []);
         push(@{$test_set->{'cat'}}, $cat_file);
      }

      foreach my $remote_dir (@{$dir_to_make_on_phone}) {
         $remote_dir = forwardslash($remote_dir);
         # add framework root to relative paths
         $remote_dir = $framework_path . '/' . $remote_dir
            if $remote_dir !~ m/^\//i;

         $test_set->{'mkdir'} = _default($test_set->{'mkdir'}, []);
         push(@{$test_set->{'mkdir'}}, $remote_dir);
      }

      if (exists $test_set->{'duration'} and defined $test_set->{'duration'}) {
         my $timeout = $test_set->{'duration'};
         $timeout = $timeout * 2;
         $test_set->{'iterations'} = _default($test_set->{'iterations'}, 1);
         $timeout = $timeout * $test_set->{'iterations'};
         $timeout = $timeout + 60 if $timeout < 180;
         $test_set->{'timeout'} = _default($test_set->{'timeout'}, $timeout);
      }

      $test_set->{'result'} = 'not run';
      DumperMsg('test_set', $test_set) if $verbose;
      push(@$master_test_list, $test_set);
   }

   if (@$test_sets_with_errors) {
      DumperMsg('one or more problem(s) exist with the selected tests:', $test_sets_with_errors);
      return AsiaFail(
         $VALUE_ERROR, $FATALERR, 'process_test_set_configurations -> ' .
         'unable to execute selected test cases, one or more problems exist ' .
         'with the test entries listed above', $VALUE_ERROR,
      );
   }

   return $NO_ERROR;
}

sub generate_archive_root {
   my $test_set = shift;

   my @module_list = ();
   my $entries = _default($test_set->{'entries'}, []);
   $entries = [$entries] if ref($entries) ne 'ARRAY';

   foreach my $entry (@$entries) {
      next if not exists $entry->{'test_type'};
      next if not defined $entry->{'test_type'};

      my $type = $entry->{'test_type'};
      $type =~ s/_//i;

      push(@module_list, $type);
   }

   my $test_number = $test_set->{'test_set_number'};
   my $category = $test_set->{'test_category'};

   if ($test_number !~ m/^\d+$/) {
      $test_number =~ s/feature_?//i;
      $test_number =~ s/${category}_?//i unless $test_number =~ m/^${category}_?\d+$/i;
      $test_number =~ s/^_+//i;
   }

   my $test_label = 'test_set_' . $test_number if $test_number =~ m/^\d+$/i;
   $test_label = $test_number if $test_number !~ m/^\d+$/i;
   $test_label .= '_' . join('_', flatten_module_list(@module_list))
      if @module_list and $test_number =~ m/^\d+$/i;

   my $build = $::dsp_build;
   my $prod_line = $::product_line;
   my $hw = $::hw_platform;

   $curr_test_scope = $test_set->{'test_scope'} if exists $test_set->{'test_scope'};
   $curr_test_scope = _default($::test_scope, $curr_test_scope);

   my $meta_build_id = basename($meta_build_path) if defined $meta_build_path;
   if (defined $meta_build_id) {
      $meta_build_id =~ s/.*[^\d]{3,}//i;
   }

   my $archiveRoot = _default($ArchiveRoot, $DEFAULT_ARCHIVE_ROOT);
   $archiveRoot = lc $archiveRoot;
   $archiveRoot = forwardslash($archiveRoot);
   $archiveRoot .= '/' if $archiveRoot !~ m/\/$/i;

   $archiveRoot .= $prod_line . '/' if $prod_line and $prod_line !~ m/^unknown$/i;
   $archiveRoot .= 'pl_unknown/' if not $prod_line or $prod_line =~ m/^unknown$/i;

   $archiveRoot .= $hw . '/' if $hw and $hw !~ m/^unknown$/i;
   $archiveRoot .= 'hw_unknown/' if not $hw or $hw =~ m/^unknown$/i;

   $archiveRoot .= $curr_test_scope . '/' if defined $curr_test_scope;
   $archiveRoot .= 'scope_unknown/' if not defined $curr_test_scope;

   if (lc($curr_test_scope) eq 'approval' or lc($curr_test_scope) eq 'stability') {
      $archiveRoot .= $build . '/' if $build and $build !~ m/^unknown$/i;
      $archiveRoot .= 'build_unknown/' if not $build or $build =~ m/^unknown$/i;
   }

   $archiveRoot .= $meta_build_id . '/' if defined $meta_build_id and lc($curr_test_scope) eq 'stability';

   $archiveRoot .= (defined $curr_test_category)
      ? $curr_test_category . '/'
      : 'uncategorized/';

   $curr_apt_level = _default($::apt_level, $curr_apt_level);
   $archiveRoot .= 'level' . $curr_apt_level . '/'
      if defined $curr_apt_level and lc($curr_test_scope) !~ m/(approval|stability)/i;

   $archiveRoot .= $test_label . '/';

   if (lc($curr_test_scope) ne 'approval' and lc($curr_test_scope) ne 'stability') {
      $archiveRoot .= $build . '/' if $build and $build !~ m/^unknown$/i;
      $archiveRoot .= 'build_unknown/' if not $build or $build =~ m/^unknown$/i;
   }
   
   $archiveRoot .= $meta_build_id . '/' if defined $meta_build_id and lc($curr_test_scope) ne 'stability';

   $archiveRoot .= 'auto/';

   # defer to see if this is useful
   #$archiveRoot .= 'atm/' if defined $::ATM_JobRunID;
   #$archiveRoot .= 'Job-' . $::ATM_JobRunID . '/' if defined $::ATM_JobRunID;

   $test_set->{'archive'} = $archiveRoot;

   return $NO_ERROR;
}

sub create_archive {
   my $archiveRoot = shift;
   my $instance_label = _default(shift, 'run');

   my $archive_spec = {
      'fields' => [],
      'use_instance' => $TRUE,
      'root' => $archiveRoot,
      'instance_label' => $instance_label,
   };
   my $archive = create_new_archive($archive_spec);

   $archive .= '/' if ($archive !~ m(/$));

   my $main_archive = (defined $ARCHIVE)
      ? $archiveRoot
      : $archive;

   PrintMsg('archive is ');
   print('   ' . backslash($archive) . "\n\n");

   $ARCHIVE = $archive;

   return $NO_ERROR;
}

sub ARCHIVE {
   my $arg_list = ['path'];
   my $arg_tls_list  = [ ['f', '-d'] ];
   return undef if not InitUserFunction(\@_, $arg_list, 0, $arg_tls_list);

   my $path = shift;
   $ARCHIVE = _default($path, $ARCHIVE);
   return undef if not defined $ARCHIVE;

   $ARCHIVE = forwardslash($ARCHIVE);
   $ARCHIVE .= '/' if ($ARCHIVE !~ m|/$|);
   return $ARCHIVE;
}

sub downgrade_test_case_result {
   my ($new_result, $reason) = @_;

   my $current_result = $::TestCaseResult;

   my $curr_result_string = "'FAIL'" if $current_result eq $FAIL;
   $curr_result_string = "'INCONCLUSIVE'" if $current_result eq $INCONCLUSIVE;
   $curr_result_string = "'PENDING'" if $current_result eq $PENDING;
   $curr_result_string = "'PASS'" if $current_result eq $PASS;
   $curr_result_string = _default($curr_result_string, "'other'");

   my $new_result_string = "'FAIL'" if $new_result eq $FAIL;
   $new_result_string = "'INCONCLUSIVE'" if $new_result eq $INCONCLUSIVE;
   $new_result_string = "'PENDING'" if $new_result eq $PENDING;
   $new_result_string = "'PASS'" if $new_result eq $PASS;
   $new_result_string = _default($new_result_string, "'other'");

   my $debug = $verbose;

   my $update_required = $FALSE;
   if ($new_result eq $FAIL) {
      $update_required = $TRUE if $current_result ne $FAIL;
   } elsif ($new_result eq $INCONCLUSIVE and $current_result ne $FAIL) {
      $update_required = $TRUE if $current_result ne $INCONCLUSIVE;
   } elsif ($new_result eq $PENDING and $current_result eq $PASS) {
      $update_required = $TRUE;
   }

   $reason = undef if defined $reason and length($reason) == 0;

   my $status_msg = 'downgrade_test_case_result: ';
   if ($update_required) {
      $status_msg .= 'updated test case result from ' . $curr_result_string;
      $status_msg .= ' to ' . $new_result_string;
      UpdateTestCaseStatus($new_result);
   } else {
      $status_msg .= 'test case result not updated to '  . $new_result_string;
      $status_msg .= ', was ' . $curr_result_string;
   }

   if ($debug or $update_required) {
      my @status_msg_args = ($status_msg);
      push(@status_msg_args, $reason) if defined $reason;
      StatusMsg(@status_msg_args);
   }

   return $NO_ERROR;
}

sub allocate_phone {
   my $com_port = shift;
   $com_port = _default($com_port, $ComPort);

   my $aps_args = {'persistence' => $PERSISTENT_NEW};
   $aps_args->{'comport'} = $com_port if defined $com_port;
   $aps_args->{'noabort'} = $TRUE;

   my $curr_err_handling = $ErrorHandling;
   $ErrorHandling = $ERR_NONE;

   PrintMsg('checking for active qpst com port');
   $com_port = wait_for_qpst_com_port({
      'timeout' => 30,
      'error_ok' => $TRUE,
   });

   if (defined $com_port) {
      $aps_args->{'comport'} = _default($aps_args->{'comport'}, $com_port);
      GetResources('phone', $PhoneResID, $aps_args);
   }

   $ErrorHandling = $curr_err_handling;

   return;
}

sub configure_phone_logging {
   # read .dmc log file if specified
   my $dmc_file_result;
   my $req_log_codes;
   my $req_log_msgs;

   if (defined $qxdm_config_file and defined $phone) {
      PrintMsg(
         __PACKAGE__ . '::configure_phone_logging -> ' .
         'extracting log codes and message levels from dmc file: \'' .
         $qxdm_config_file . '\''
      );

      $dmc_file_result = $phone->ExtractFromDMCFile($qxdm_config_file);
      if (defined $dmc_file_result and ref($dmc_file_result) eq 'HASH') {
         $req_log_codes = $dmc_file_result->{'logCodes'}
            if exists $dmc_file_result->{'logCodes'};
         $req_log_msgs = $dmc_file_result->{'msgLevels'}
            if exists $dmc_file_result->{'msgLevels'};
      }
   }

   $req_log_codes = _default(
      $req_log_codes,
      [@::log_masks, @::additional_log_masks],
   );

   $req_log_msgs = _default($req_log_msgs, \%::ExtMsgRtHash);

   # first - debug messages (f3)
   # Message 2.0, for select subsystems
   # Disable all message masks
   my $err_code = $phone->SetExtMsgRtMasks(0x0);
   return $err_code if ($err_code ne $NO_ERROR);

   # Enable required message masks
   $err_code = $phone->SetExtMsgRtMasks($req_log_msgs);
   return $err_code if ($err_code ne $NO_ERROR);

   DumperMsg(
      __PACKAGE__ . '::set_phone_log_config -> set required ' .
      'extended run-time msg masks', $req_log_msgs,
   ) if $verbose;

   my $return_list = [];
   foreach my $sub_sys (keys %::optExtMsgRtHash) {
      my $rt_hash = {$sub_sys => $::optExtMsgRtHash{$sub_sys}};
      # Enable optional message masks

      my $rtn = $phone->_CallFuncWithErrNone('SetExtMsgRtMasks', $rt_hash);
      push(
         @$return_list,
         'subsys ' . $sub_sys . ', value ' . $rt_hash->{$sub_sys} .
         ', returned ' . $rtn,
      );
   }

   foreach my $sub_sys (keys %::additionalExtMsgRtHash) {
      my $rt_hash = {$sub_sys => $::additionalExtMsgRtHash{$sub_sys}};
      # Enable optional message masks

      my $rtn = $phone->_CallFuncWithErrNone('SetExtMsgRtMasks', $rt_hash);
      push(
         @$return_list,
         'subsys ' . $sub_sys . ', value ' . $rt_hash->{$sub_sys} .
         ', returned ' . $rtn,
      );
   }

   DumperMsg(
      __PACKAGE__ . '::set_phone_log_config -> set optional '.
         'extended run-time msg masks', $return_list,
   ) if $verbose and @$return_list;

   # second - DMSS events
   $phone->LogEvents($TRUE);
   PrintMsg(
      __PACKAGE__ . '::set_phone_log_config -> enabled event logging',
   ) if $verbose;

   # third - log items
   my $setMasks = $phone->SetSubSystemLogMask($req_log_codes);
   return $setMasks if ($setMasks ne $NO_ERROR);

   if ($verbose) {
      if (@$req_log_codes) {
         StatusMsg(__PACKAGE__ . '::set_phone_log_config -> set log masks');
         print '                       [' . "\n";
         foreach my $code (@$req_log_codes) {
            print sprintf(' ' x 26 . '0x%.4x', $code) . "\n";
         }
         print '                       ]' . "\n\n";
      }
   }

   return $NO_ERROR;
}

sub start_aps_logging {
   my $log_file = shift;

   PrintMsg('start aps logging: ');
   print '   ' . backslash($log_file) . "\n\n";

   my @log_args = ($log_file, $FALSE, $TRUE);
   my $ret_code;

   if ($APS_IS_LOGGING == $TRUE) {
      $ret_code = $phone->_CallFuncWithErrNone('NextLogCreate', @log_args);
   }

   if (not defined $ret_code or $ret_code ne $NO_ERROR) {
      $ret_code = $phone->LogCreate(@log_args);
   }

   if ($ret_code eq $NO_ERROR) {
      $APS_IS_LOGGING = $TRUE;

      my $stability_test = $FALSE;
      $stability_test = $TRUE
         if defined $curr_test_category and $curr_test_category =~ m/stability/i;
      $stability_test = $TRUE
         if defined $curr_test_scope and $curr_test_scope =~ m/stability/i;

      push(@dlf_logs_to_parse, $log_file)
         if $enable_phone_log_parsing and not $stability_test;
   }

   return $ret_code;
}

sub stop_aps_logging {
   PrintMsg('stop aps logging');
   $APS_IS_LOGGING = $FALSE;
   my $ret_code = $phone->LogClose();
   return $ret_code;
}

sub run_pwrusb_reset {
   Mortar::Common::Tools::AdbUtils::stop_adb_server();

   PrintMsg('using \'PowerUSB\' to power cycle the target');
   Mortar::Common::Tools::PwrUSB::set_outlet_state($dut_power_outlet_id, 0);

   sleep(5);

   Mortar::Common::Tools::PwrUSB::set_outlet_state($usb_hub_power_outlet_id, 0);

   sleep(10);

   Mortar::Common::Tools::PwrUSB::set_outlet_state($usb_hub_power_outlet_id, 1);

   sleep(5);

   Mortar::Common::Tools::PwrUSB::set_outlet_state($dut_power_outlet_id, 1);

   # allow time for device to power up
   sleep(30);

   Mortar::Common::Tools::AdbUtils::start_adb_server();
   wait_for_adb_device();

   sleep(5);

   return $NO_ERROR;
}

sub run_spider_board_reset {
   Mortar::Common::Tools::AdbUtils::stop_adb_server();

   PrintMsg('using \'spider board\' to power cycle the target');

   if (-e 'c:/pkg/tools/CItestTools/spider/spider.py') {
      # CI uses a different config for device power
      my $com_port = $spider_board->get_com_port();
      Mortar::Common::Tools::SystemUtils::system_cmd(
         'c:/pkg/tools/CItestTools/spider/spider.py -p ' .  $com_port . 
         '-s TTL1ON:USBNONE:TTL4OFF:PWR1OFF:DLY10:PWR1ON:TTL1OFF:USBHOST:DLY15',
      );
   } else {
      $spider_board->set_device_power(0);

      sleep(5);

      $spider_board->set_peripheral_power(0);

      sleep(10);

      $spider_board->set_peripheral_power(1);

      sleep(5);

      $spider_board->set_device_power(1);
   }

   # allow time for device to power up
   sleep(30);

   Mortar::Common::Tools::AdbUtils::start_adb_server();
   wait_for_adb_device();

   sleep(5);

   return $NO_ERROR;
}

sub run_phone_reset {
   my $com_port = wait_for_qpst_com_port();
   return AsiaFail(
      $FAILED, $FATALERR, 'run_phone_reset -> ' . 
      'unable to issue reset command, no active com port detected in QPST',
      $FAILED,
   ) if not defined $com_port;

   Mortar::Common::Tools::AdbUtils::stop_adb_server();

   my $dl_mode = Mortar::Common::Tools::QpstUtils::is_phone_in_download_mode($com_port);

   if ($dl_mode) {
      Mortar::Common::Tools::QpstUtils::reset_com_port({
         'com_port' => $com_port,
         'debug'    => $verbose,
      });
   } else {
      allocate_phone($com_port) if not defined $phone;
      $phone->Reset() if defined $phone;
   }

   sleep(5);

   Mortar::Common::Tools::AdbUtils::start_adb_server();
   return wait_for_adb_device();
}

sub get_res_id {
   my $arg_list = ['handle'];
   my $arg_tls_list  = [ ['isa', 'ASIAObject'] ];
   return undef if not InitUserFunction(\@_, $arg_list, 1, $arg_tls_list);
   my $resource = shift;
   my ($res_params, $res_info) = GetResourceInfo($resource);
   return lc($res_info->{'resid'});
}

sub wait_for_qpst_com_port {
   my ($spec) = @_;

   $spec = _default($spec, {});
   my $lc_spec = {};
   foreach my $spec_key (keys(%$spec)) {
      $lc_spec->{lc($spec_key)} = $spec->{$spec_key};
   }

   my $timeout = _default($lc_spec->{'timeout'}, 120);
   my $interval = _default($lc_spec->{'interval'}, 10);
   my $err_ok = _default($lc_spec->{'error_ok'}, $FALSE);

   my $curr_err_handling = $ErrorHandling;
   $ErrorHandling = $ERR_NONE;
   my $com_port_hash = Mortar::Common::Tools::QpstUtils::get_com_ports_in_use();

   POLL: for (my $poll_time = 0; $poll_time <= $timeout; $poll_time = $poll_time + $interval) {
      if (defined $com_port_hash and keys(%{$com_port_hash})) {
         $ErrorHandling = $curr_err_handling;
         DumperMsg('com ports', $com_port_hash) if $verbose;
         my @com_port_list = keys(%{$com_port_hash});
         return $com_port_list[0];
      }

      sleep($interval);
      $com_port_hash = Mortar::Common::Tools::QpstUtils::get_com_ports_in_use();
   }

   $ErrorHandling = $curr_err_handling;

   if (defined $com_port_hash and keys(%{$com_port_hash})) {
      DumperMsg('com ports', $com_port_hash) if $verbose;
      my @com_port_list = keys(%{$com_port_hash});
      return $com_port_list[0];
   }

   if (not $err_ok) {
      DumperMsg('wait_for_qpst_com_port specification:', $lc_spec) if $verbose;
      return AsiaFail(
         $TIMEOUT_ERROR, $FATALERR, 'wait_for_qpst_com_port -> did not find an ' .
         'active qpst com port', undef,
      );
   }

   return undef;
}

sub wait_for_qpst_download_to_complete {
   my ($spec) = @_;

   $spec = _default($spec, {});
   my $lc_spec = {};
   foreach my $spec_key (keys(%$spec)) {
      $lc_spec->{lc($spec_key)} = $spec->{$spec_key};
   }

   my $com_port = $lc_spec->{'com_port'};
   return $VALUE_ERROR if not defined $com_port;

   my $phone_ready = Mortar::Common::Tools::QpstUtils::is_phone_in_ready_state($com_port);
   return $FAILED if not defined $phone_ready or not $phone_ready;

   my $dling_mode = Mortar::Common::Tools::QpstUtils::is_phone_downloading_memory_dumps($com_port);
   
   my $timeout = _default($lc_spec->{'timeout'}, 600);
   my $interval = _default($lc_spec->{'interval'}, 10);
   my $err_ok = _default($lc_spec->{'error_ok'}, $FALSE);

   my $curr_err_handling = $ErrorHandling;
   $ErrorHandling = $ERR_NONE;

   POLL: for (my $poll_time = 0; $poll_time <= $timeout; $poll_time = $poll_time + $interval) {
      if (defined $dling_mode and not $dling_mode) {
         # check if it is ready or if it needs to be reset?
         last
      } else {
         return $FAILED if not defined $dling_mode or not $dling_mode;
      }

      sleep($interval);
      $dling_mode = Mortar::Common::Tools::QpstUtils::is_phone_downloading_memory_dumps($com_port);
   }

   $ErrorHandling = $curr_err_handling;

   return $NO_ERROR;
}

sub run_target_reset {
   my $err_code = run_pwrusb_reset(@_) if $use_pwr_usb_for_reset;
   $err_code = run_spider_board_reset(@_) if $use_spider_board_for_reset;

   if (not defined $err_code) {
      # try an adb reset?
      my $std_out;
      my $cmd_spec = {'timeout' => 60, 'verbose' => $verbose};
      ($err_code, $std_out) = Mortar::Common::Tools::AdbUtils::adb_reboot($cmd_spec);
   }

   return $err_code;
}

sub get_phone_com_port {
   return undef if not defined $phone;
   my $port_info = $phone->GetComPortInfo();

   my @params = split('/', $port_info);
   my $host = $params[0];
   my $com_port = $params[1];

   return $com_port;
}

sub parse_build_contents_xml {
   my $arg_list = ['content_file'];
   my $arg_tls_list = [['f', '-e']];

   return $VALUE_ERROR
      if not InitUserFunction(\@_, $arg_list, 1, $arg_tls_list);

   my $content_file = shift;

   my $line;
   my $build_paths = {};

   my $result = open(CONTENTS, $content_file);
   return AsiaFail(
      $FAILED, $FATALERR, 'parse_build_contents_xml -> unable to parse ' .
      'contents xml file, ' . $!, $FAILED,
   ) if not $result;

   while(not eof(CONTENTS)){
      $line = <CONTENTS>; 

      if ($line =~ m/<windows_root_path cmm_root_path_var="([A-Z_]+)_BUILD_ROOT">([A-Z0-9\.\\]+)<\/windows_root_path>/i) {
         my $cmm_type = $1;
         my $path = $2;

         if (defined $path) {
            $path = backslash($path);
            $path =~ s/\\$//i;
         }

         if (defined $cmm_type and defined $path and -d $path) {
            $cmm_type = lc($cmm_type);
            $build_paths->{$cmm_type} = backslash($path);
         }
      }
   }

   return $build_paths;
}

sub run_afe_init_test {
   my $test_set = {
      'iterations' => '1',
      'cat' => [
        '/data/adsp-test/aDSPTest/TestConfig/qdsp6/device/afe/AFE_SEQ_6_DTMF_bet.cfg',
      ],
      'archive_input_files' => [],
      'pre_test' => [
        'push //baskin/apt_multimedia/qdsp6/test_files/ADSP.BF.2.0/MSM8974/aDSPTest/TestConfig/qdsp6/device/afe/AFE_SEQ_6_DTMF_bet.cfg /data/adsp-test/aDSPTest/TestConfig/qdsp6/device/afe/AFE_SEQ_6_DTMF_bet.cfg',
        'push c:/temp/afe_init_TestMaster.txt /data/adsp-test/TestMaster.txt',
      ],
      'on_target_level' => '1',
      'result' => 'not run',
      'test_category' => 'afe',
      'test_scope' => 'regression',
      'entries' => [
        {
          'test_type' => 'DEV_SEQ',
          'config_file' => 'aDSPTest/TestConfig/qdsp6/device/afe/AFE_SEQ_6_DTMF_bet.cfg',
          'force_exit' => 0,
          'instances' => '1',
        }
      ],
      'tunnel_mode' => 1,
      'duration' => '0.2',
      'test_set_number' => 'dtmf_2',
      'post_test' => [
        'shell rm /data/adsp-test/aDSPTest/TestConfig/qdsp6/device/afe/AFE_SEQ_6_DTMF_bet.cfg',
         'shell rm /data/adsp-test/TestMaster.txt',
      ],
      'timeout' => '60.4',
      'test_level' => '1',
   };

   my $test_set_spec = {
      'test_sets' => $test_set,
      'output_file' => 'c:/temp/afe_init_TestMaster.txt',
      'overwrite' => $TRUE,
      'debug' => $verbose,
   };

   my $ret_code = write_test_set($test_set_spec);

   my $curr_aps_logging = $enable_phone_logging;
   $enable_phone_logging = $FALSE;

   my $curr_err_handling = $ErrorHandling;
   $ErrorHandling = $ERR_NONE;
   $ret_code = run_test_set($test_set);

   $enable_phone_logging = $curr_aps_logging;
   $ErrorHandling = $curr_err_handling;

   return $NO_ERROR;
}
