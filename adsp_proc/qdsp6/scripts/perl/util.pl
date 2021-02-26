

$StoragePath="\\\\balor\\vocoder_appdsp4\\users\\mingfung\\1825_v1\\qdsp6\\WorkDirectory";

@Arr_DSP_CLOCK=("600","245","384");
%MAP_AXI=('245'=>'61','384'=>'128','600'=>'128');
@Arr_TYPE_VOCODER=("AMR_NB","EVRC","EFR","FR","HR","V13K","4GV_NB");#"AMR_WB"
@Arr_TYPE_VOI_PLAYBACK=("AMR_NB","V13K","EVRC");#,"EVRC");#,"4GV","EVRC","EFR","FR","HR","V13K");
@Arr_TYPE_VOI_RECORD=("AMR_NB","EVRC","EFR","FR","HR","V13K","4GV_NB");#,"4GV","EVRC","EFR","FR","HR","V13K");
@Arr_TYPE_AUDIOCODER=("1EAAC_P","1MP3","EAAC_P_MP3");
@Arr_TYPE_MDMTEST=("PCM","AAC","AMR");
%CTF_AUDIOCODER=('1EAAC_P'=>'CAudioAacLpaTest','1MP3'=>'CAudioMp3LpaTest','EAAC_P_MP3'=>'CAudioPlaybackTest');
@Arr_TYPE_VDOCODER_DEC=("H264");
%CTF_VIDEO_DECODER=('VC1'=>'VC1MTDecoderTest', 'H264'=>'CH264DecoderTest','MPEG4'=>'CMPEG4DecoderTest');
@Arr_TYPE_VDO_CLIP_DEC=("QCIF","QVGA","VGA","720P");
@Arr_TYPE_VDOCODER_ENC=("MPEG4");
%CTF_VIDEO_ENCODER=('MPEG4'=>'MPEG4EncQ6TeTest');
@Arr_TYPE_VDO_CLIP_ENC=("VGA","720P");

%POrder=('Release'=>'0','Index'=>'1','MM requirement number'=>'2','DSP clock'=>'3','AXI clock'=>'4','Voice call'=>'5','Voice Playback'=>'6','Voice Recording'=>'7','Audio Coder1'=>'8','Audio Coder2'=>'9','Video decoder'=>'10','Decoder clip type'=>'11','Video encoder'=>'12','Encoder clip type'=>'13','MIDI synth'=>'14','Total MCPS'=>'15','TOTAL_MCPS_OVHD'=>'16','Total MPPS'=>'17','TOTAL_MPPS_OVHD'=>'18','APPs MPPS'=>'19','APP_MPPS_OVHD'=>'20','RTOS QUBE MCPS'=>'21','RTOS QUBE MCPS OVHD'=>'22','OMM CS LIB MCPS'=>'23','OMM_CS_LIB_MCPS_OVHD'=>'24','AXI Bandwidth from Q6'=>'25','BW_OVHD'=>'26','CACHE OVHD'=>'27','SYSTEM OVHD'=>'28','Apps MPPS CHECK'=>'29','Log'=>'30','RBP'=>'31','TLP'=>'32','All function'=>'33','Thread function'=>'34','BUS Data'=>'35');
@Arr_TYPE_CTF=("VOICE CALL","VOICE PLAYBACK","VOICE RECORDING","AUDIO PLAYBACK","VIDEO VGA","VIDEO WVGA");#VIDEO 720P

$CLIP_NAME_DEC{"VC1"}{"QVGA"} = "SML0014.rcv";
$CLIP_NAME_DEC{"VC1"}{"VGA"} = "VC1_VGA_1Mbps_30fps_clip.rcv";
$CLIP_NAME_DEC{"VC1"}{"WVGA"} = "VC1_WVGA_2Mbps_30fps_clip.rcv";
$CLIP_NAME_DEC{"VC1"}{"720P"} = "VC1_Prog_720p.vc1";
$CLIP_NAME_DEC{"H264"}{"QCIF"} = "BA2_Sony_F.264";
$CLIP_NAME_DEC{"H264"}{"QVGA"} = "h264_qvga_bp_384k.h264";
$CLIP_NAME_DEC{"H264"}{"VGA"} = "Qtc48_videoOnly.264";
$CLIP_NAME_DEC{"H264"}{"WVGA"} = "Qtc50_videoOnly.264";
$CLIP_NAME_DEC{"H264"}{"720P"} = "hd_videoh264_10M30fps_Bframe_0_149.264";
$CLIP_NAME_DEC{"MPEG4"}{"QVGA"} = "Qtc62_butt_MPEG4_qvga_30fps_384kbps_90s.m4v";
$CLIP_NAME_DEC{"MPEG4"}{"VGA"} = "XtreamDVrev2_MPEG4_vga_30fps_2mbps_2min.m4v";
$CLIP_NAME_DEC{"MPEG4"}{"WVGA"} = "butt_MPEG4_wvga_30fps_3mbps.m4v";
$CLIP_NAME_DEC{"MPEG4"}{"720P"} = "QCIF_Butterfly720p.m4v";
$CLIP_NAME_ENC{"MPEG4"}{"QVGA"} = "football_qvga.yuv";
$CLIP_NAME_ENC{"MPEG4"}{"VGA"} = "row_vga.yuv";
$CLIP_NAME_ENC{"MPEG4"}{"WVGA"} = "racecar_wvga.yuv";
$CLIP_NAME_ENC{"MPEG4"}{"720P"} = "parkrun_720p_10f.yuv";


sub Parse_config
{

   my @arg = @_;
   $cfg_file ="./qdsp6/scripts/cfg/".$arg[0];
   
   #print "\n\ncfg_file: $cfg_file\n";
   open cfg_file or die "failed to open $cfg_file :$!";
   
   while(<cfg_file>)
   {


      @line_ary = split('=',$_);
      #===========flow control===============
      if ($line_ary[0] =~ /FLOW_CTL_RUN_BUILD/){
         @line_ary1 = split(';',$line_ary[1]);
         $FLOW_CTL_RUN_BUILD = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FLOW_CTL_RUN_CREATE_IMAGE_DATABASE/){
         @line_ary1 = split(';',$line_ary[1]);
         $FLOW_CTL_RUN_CREATE_IMAGE_DATABASE = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FLOW_CTL_RUN_SIMULATION/){
         @line_ary1 = split(';',$line_ary[1]);
         $FLOW_CTL_RUN_SIMULATION = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FLOW_CTL_RUN_DTLP/){
         @line_ary1 = split(';',$line_ary[1]);
         $FLOW_CTL_RUN_DTLP = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FLOW_CTL_GEN_RBP/){
         @line_ary1 = split(';',$line_ary[1]);
         $FLOW_CTL_GEN_RBP = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FLOW_CTL_GEN_TLP/){
         @line_ary1 = split(';',$line_ary[1]);
         $FLOW_CTL_GEN_TLP = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FLOW_CTL_GEN_CALL_GRAPH/){
         @line_ary1 = split(';',$line_ary[1]);
         $FLOW_CTL_GEN_CALL_GRAPH = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FLOW_CTL_GEN_BUS_BANDWITDH/){
         @line_ary1 = split(';',$line_ary[1]);
         $FLOW_CTL_GEN_BUS_BANDWITDH = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FLOW_CTL_GEN_BUS_TRACE/){
         @line_ary1 = split(';',$line_ary[1]);
         $FLOW_CTL_GEN_BUS_TRACE = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FLOW_CTL_GEN_CACHE_TRACE/){
         @line_ary1 = split(';',$line_ary[1]);
         $FLOW_CTL_GEN_CACHE_TRACE = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FLOW_CTL_CLEAN_TEMP/){
         @line_ary1 = split(';',$line_ary[1]);
         $FLOW_CTL_CLEAN_TEMP = $line_ary1[0];
      }
      #==================Instance_type================
      if ($line_ary[0] =~ /INSTANCE_VOICE_CALL/){
         @line_ary1 = split(';',$line_ary[1]);
         $INSTANCE_VOICE_CALL = $line_ary1[0];
      }

      if ($line_ary[0] =~ /TYPE_VOCODER/){
         @line_ary1 = split(';',$line_ary[1]);
         $TYPE_VOCODER = $line_ary1[0];
      }
      if ($line_ary[0] =~ /INSTANCE_AUDIO/){
         @line_ary1 = split(';',$line_ary[1]);
         $INSTANCE_AUDIO = $line_ary1[0];
      }
      if ($line_ary[0] =~ /TYPE_AUDIOCODER/){
         @line_ary1 = split(';',$line_ary[1]);
         $TYPE_AUDIOCODER = $line_ary1[0];
      }
      if ($line_ary[0] =~ /INSTANCE_VIDEO_DEC/){
         @line_ary1 = split(';',$line_ary[1]);
         $INSTANCE_VIDEO_DEC = $line_ary1[0];
      }
      if ($line_ary[0] =~ /TYPE_VDOCODER_DEC/){
         @line_ary1 = split(';',$line_ary[1]);
         $TYPE_VDOCODER_DEC = $line_ary1[0];
      }
      if ($line_ary[0] =~ /TYPE_VDO_CLIP_DEC/){
         @line_ary1 = split(';',$line_ary[1]);
         $TYPE_VDO_CLIP_DEC = $line_ary1[0];
      }
      if ($line_ary[0] =~ /INSTANCE_VIDEO_ENC/){
         @line_ary1 = split(';',$line_ary[1]);
         $INSTANCE_VIDEO_ENC = $line_ary1[0];
      }
      if ($line_ary[0] =~ /TYPE_VDOCODER_ENC/){
         @line_ary1 = split(';',$line_ary[1]);
         $TYPE_VDOCODER_ENC = $line_ary1[0];
      }
      if ($line_ary[0] =~ /TYPE_VDO_CLIP_ENC/){
         @line_ary1 = split(';',$line_ary[1]);
         $TYPE_VDO_CLIP_ENC = $line_ary1[0];
      }
      if ($line_ary[0] =~ /INSTANCE_VOICE_PLAYBACK/){
         @line_ary1 = split(';',$line_ary[1]);
         $INSTANCE_VOICE_PLAYBACK = $line_ary1[0];
      }
      if ($line_ary[0] =~ /TYPE_VOI_PLAYBACK/){
         @line_ary1 = split(';',$line_ary[1]);
         $TYPE_VOI_PLAYBACK = $line_ary1[0];
      }
      if ($line_ary[0] =~ /INSTANCE_VOICE_RECORDING/){
         @line_ary1 = split(';',$line_ary[1]);
         $INSTANCE_VOICE_RECORDING = $line_ary1[0];
      }
      if ($line_ary[0] =~ /TYPE_VOI_RECORD/){
         @line_ary1 = split(';',$line_ary[1]);
         $TYPE_VOI_RECORD = $line_ary1[0];
      }
      if ($line_ary[0] =~ /INSTANCE_MIDI_SYNTH/){
         @line_ary1 = split(';',$line_ary[1]);
         $INSTANCE_MIDI_SYNTH = $line_ary1[0];
      }
      if ($line_ary[0] =~ /INSTANCE_MVS/){
         @line_ary1 = split(';',$line_ary[1]);
         $INSTANCE_MVS = $line_ary1[0];
      }
      if ($line_ary[0] =~ /TYPE_MVSTEST/){
         @line_ary1 = split(';',$line_ary[1]);
         $TYPE_MVSTEST = $line_ary1[0];
      }
      if ($line_ary[0] =~ /INSTANCE_MDM/){
         @line_ary1 = split(';',$line_ary[1]);
         $INSTANCE_MDM = $line_ary1[0];
      }
      if ($line_ary[0] =~ /TYPE_MDMTEST/){
         @line_ary1 = split(';',$line_ary[1]);
         $TYPE_MDMTEST = $line_ary1[0];
      }
      if ($line_ary[0] =~ /Q6_CORE_STARTPC_TIME/){
         @line_ary1 = split(';',$line_ary[1]);
         $Q6_CORE_STARTPC_TIME = $line_ary1[0];
      }
      if ($line_ary[0] =~ /Q6_CORE_STEADY_PERIOD/){
         @line_ary1 = split(';',$line_ary[1]);
         $Q6_CORE_STEADY_PERIOD = $line_ary1[0];
      }
      #===========q6_core======================
      if ($line_ary[0] =~ /Q6_CORE_STARTPC_TIME/){
         @line_ary1 = split(';',$line_ary[1]);
         $Q6_CORE_STARTPC_TIME = $line_ary1[0];
      }
      if ($line_ary[0] =~ /Q6_CORE_STEADY_PERIOD/){
         @line_ary1 = split(';',$line_ary[1]);
         $Q6_CORE_STEADY_PERIOD = $line_ary1[0];
      }
      if ($line_ary[0] =~ /Q6_CORE_HEAP_SIZE/){
         @line_ary1 = split(';',$line_ary[1]);
         $Q6_CORE_HEAP_SIZE = $line_ary1[0];
      }
      if ($line_ary[0] =~ /Q6_CORE_BASELINE_VER/){
         @line_ary1 = split(';',$line_ary[1]);
         $Q6_CORE_BASELINE_VER = $line_ary1[0];
      }
      if ($line_ary[0] =~ /Q6_CORE_FIRST_BREAKPOINT/){
         @line_ary1 = split(';',$line_ary[1]);
         $Q6_CORE_FIRST_BREAKPOINT = $line_ary1[0];
      }
      if ($line_ary[0] =~ /Q6_CORE_SECOND_BREAKPOINT/){
         @line_ary1 = split(';',$line_ary[1]);
         $Q6_CORE_SECOND_BREAKPOINT = $line_ary1[0];
      }
      if ($line_ary[0] =~ /Q6_CORE_THIRD_BREAKPOINT/){
         @line_ary1 = split(';',$line_ary[1]);
         $Q6_CORE_THIRD_BREAKPOINT = $line_ary1[0];
      }
      
      #=============directory Path=======================
      if ($line_ary[0] =~ /REL_DIR_BOOTIMG/){
         @line_ary1 = split(';',$line_ary[1]);
         $REL_DIR_BOOTIMG = $line_ary1[0];
      }
      if ($line_ary[0] =~ /REL_DIR_L4KERNEL/){
         @line_ary1 = split(';',$line_ary[1]);
         $REL_DIR_L4KERNEL = $line_ary1[0];
      }
      if ($line_ary[0] =~ /REL_DIR_CS_STATIC/){
         @line_ary1 = split(';',$line_ary[1]);
         $REL_DIR_CS_STATIC = $line_ary1[0];
      }
      if ($line_ary[0] =~ /REL_DIR_IG_SERVER/){
         @line_ary1 = split(';',$line_ary[1]);
         $REL_DIR_IG_SERVER = $line_ary1[0];
      }
      if ($line_ary[0] =~ /REL_DIR_GMON/){
         @line_ary1 = split(';',$line_ary[1]);
         $REL_DIR_GMON = $line_ary1[0];
      }
      if ($line_ary[0] =~ /REL_DIR_STACK_HEAP_DUMP/){
         @line_ary1 = split(';',$line_ary[1]);
         $REL_DIR_STACK_HEAP_DUMP = $line_ary1[0];
      }
      if ($line_ary[0] =~ /REL_DIR_SPT_OBJ/){
         @line_ary1 = split(';',$line_ary[1]);
         $REL_DIR_SPT_OBJ = $line_ary1[0];
      }
      if ($line_ary[0] =~ /REL_DIR_SPT_INPUT/){
         @line_ary1 = split(';',$line_ary[1]);
         $REL_DIR_SPT_INPUT = $line_ary1[0];
      }
      
      if ($line_ary[0] =~ /REL_DIR_SPT_OUTPUT/){
         @line_ary1 = split(';',$line_ary[1]);
         $REL_DIR_SPT_OUTPUT = $line_ary1[0];
      }
      if ($line_ary[0] =~ /REL_DIR_SPT_GPROF_FLAT_PROFILE/){
         @line_ary1 = split(';',$line_ary[1]);
         $REL_DIR_SPT_GPROF_FLAT_PROFILE = $line_ary1[0];
      }
      if ($line_ary[0] =~ /REL_DIR_SPT_GPROF_CALL_GRAPH/){
         @line_ary1 = split(';',$line_ary[1]);
         $REL_DIR_SPT_GPROF_CALL_GRAPH = $line_ary1[0];
      }
      #===========ABS directory========================
      if ($line_ary[0] =~ /ABS_DIR_MAP_TXT/){
         @line_ary1 = split(';',$line_ary[1]);
         $ABS_DIR_MAP_TXT = $line_ary1[0];
      }
      if ($line_ary[0] =~ /ABS_DIR_SPT_INPUT/){
         @line_ary1 = split(';',$line_ary[1]);
         $ABS_DIR_SPT_INPUT = $line_ary1[0];
      }
      if ($line_ary[0] =~ /ABS_DIR_SPT_PERL/){
         @line_ary1 = split(';',$line_ary[1]);
         $ABS_DIR_SPT_PERL = $line_ary1[0];
      }
      if ($line_ary[0] =~ /ABS_DIR_SPT_TCL/){
         @line_ary1 = split(';',$line_ary[1]);
         $ABS_DIR_SPT_TCL = $line_ary1[0];
      }
      if ($line_ary[0] =~ /ABS_DIR_SPT_SHELL/){
         @line_ary1 = split(';',$line_ary[1]);
         $ABS_DIR_SPT_SHELL = $line_ary1[0];
      }
      #======Output_filename========
      if ($line_ary[0] =~ /FILE_NAME_IMAGE_DATABASE/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_IMAGE_DATABASE = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_MERGE_FUNCTION_MIPS/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_MERGE_FUNCTION_MIPS = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_RBP/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_RBP = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_TBP/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_TBP = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_BUS_STATE/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_BUS_STATE = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_CACHE_DATA/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_CACHE_DATA = $line_ary1[0];
      }

      if ($line_ary[0] =~ /FILE_NAME_IMAGE_TEMP_PARSE/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_IMAGE_TEMP_PARSE = $line_ary1[0];
      }

      if ($line_ary[0] =~ /FILE_NAME_TCL/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_TCL = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_GMON_LIST/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_GMON_LIST = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_REC_TOP_FUNC_NUM/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_REC_TOP_FUNC_NUM = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_TOP_FUNCLIST_OF_THREAD/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_TOP_FUNCLIST_OF_THREAD = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_THREAD_INFO/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_THREAD_INFO = $line_ary1[0];
      }
    
      if ($line_ary[0] =~ /FILE_NAME_STACK_HEAP_TCL/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_STACK_HEAP_TCL = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_STACK_HEAP_TEMPLATE/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_STACK_HEAP_TEMPLATE = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_STACK_HEAP_USAGE/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_STACK_HEAP_USAGE = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_IMAGE_SIZE_OUTPUT/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_IMAGE_SIZE_OUTPUT = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_BUS_DATA/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_BUS_DATA = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_BUS_LOG/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_BUS_LOG = $line_ary1[0];
      }
      if ($line_ary[0] =~ /FILE_NAME_DEMANGLED_IMAGE_DATABASE/){
         @line_ary1 = split(';',$line_ary[1]);
         $FILE_NAME_DEMANGLED_IMAGE_DATABASE = $line_ary1[0];
      }
   
   }

   &Create_scenario_dir;
   #$sim_bus_penalty= int((64*($SIM_PROC_DSP_CLOCK/6))/$SIM_PROC_AXI_CLOCK);
   $WORKING_DIRECTORY = "./qdsp6/WorkDirectory/$scenario_dir";
   #print "\nWORKING_DIRECTORY=$WORKING_DIRECTORY\n";

   $ABS_DIR_SPT_OUTPUT="$WORKING_DIRECTORY/$REL_DIR_SPT_OUTPUT";
   #print "REL_DIR_SPT_OUTPUT=$REL_DIR_SPT_OUTPUT\n";
   #print "ABS_DIR_SPT_OUTPUT=$ABS_DIR_SPT_OUTPUT\n";

   $ABS_DIR_SPT_OBJ="$WORKING_DIRECTORY/$REL_DIR_SPT_OBJ";
   #print "REL_DIR_SPT_OBJ=$REL_DIR_SPT_OBJ\n";
   #print "ABS_DIR_SPT_OBJ=$ABS_DIR_SPT_OBJ\n";

   $ABS_DIR_STATS_TXT="$WORKING_DIRECTORY/";
   #print "REL_DIR_STATS_TXT=$REL_DIR_STATS_TXT\n";
   #print "ABS_DIR_STATS_TXT=$ABS_DIR_STATS_TXT\n";

   $ABS_DIR_BOOTIMG="$WORKING_DIRECTORY/$REL_DIR_BOOTIMG";
   #print "REL_DIR_BOOTIMG = $REL_DIR_BOOTIMG\n";
   #print "ABS_DIR_BOOTIMG = $ABS_DIR_BOOTIMG\n";

   $ABS_DIR_L4KERNEL="$WORKING_DIRECTORY/$REL_DIR_L4KERNEL";
   #print "REL_DIR_L4KERNEL =$REL_DIR_L4KERNEL\n";
   #print "ABS_DIR_L4KERNEL =$ABS_DIR_L4KERNEL\n";

   $ABS_DIR_CS_STATIC="$WORKING_DIRECTORY/$REL_DIR_CS_STATIC";
   #print "REL_DIR_CS_STATIC =$REL_DIR_CS_STATIC\n";
   #print "ABS_DIR_CS_STATIC =$ABS_DIR_CS_STATIC\n";

   $ABS_DIR_IG_SERVER="$WORKING_DIRECTORY/$REL_DIR_IG_SERVER";
   #print "REL_DIR_IG_SERVER =$REL_DIR_IG_SERVER\n";
   #print "ABS_DIR_IG_SERVER =$ABS_DIR_IG_SERVER\n";
   $ABS_DIR_GMON_FILE="$WORKING_DIRECTORY/$REL_DIR_GMON";


   $ABS_DIR_STACK_HEAP_DUMP="$WORKING_DIRECTORY/$REL_DIR_STACK_HEAP_DUMP";
   #print "REL_DIR_STACK_HEAP_DUMP =$REL_DIR_STACK_HEAP_DUMP\n";
   #print "ABS_DIR_STACK_HEAP_DUMP =$ABS_DIR_STACK_HEAP_DUMP\n";

   $ABS_DIR_STATS_TXT="$WORKING_DIRECTORY/$REL_DIR_STATS_TXT";
   #print "REL_DIR_STATS_TXT =$REL_DIR_STATS_TXT\n";
   #print "ABS_DIR_STATS_TXT =$ABS_DIR_STATS_TXT\n";

   $ABS_DIR_SPT_GPROF_FLAT_PROFILE="$WORKING_DIRECTORY/$REL_DIR_SPT_GPROF_FLAT_PROFILE";
   #print "REL_DIR_SPT_GPROF =$REL_DIR_SPT_GPROF\n";
   #print "ABS_DIR_SPT_GPROF =$ABS_DIR_SPT_GPROF\n";
   $ABS_DIR_SPT_GPROF_CALL_GRAPH="$WORKING_DIRECTORY/$REL_DIR_SPT_GPROF_CALL_GRAPH";

   $FILE_NAME_IMAGE_DATABASE = $Q6_CORE_BASELINE_VER."_".$scenario_dir."_".$FILE_NAME_IMAGE_DATABASE;
   $FILE_NAME_IMAGE_SIZE_OUTPUT = $Q6_CORE_BASELINE_VER."_".$scenario_dir."_".$FILE_NAME_IMAGE_SIZE_OUTPUT;
   $FILE_NAME_REGION_DESC =$FILE_NAME_REGION_DESC;
   $FILE_NAME_RBP= $Q6_CORE_BASELINE_VER."_".$scenario_dir."_".$FILE_NAME_RBP;
   $FILE_NAME_TBP = $Q6_CORE_BASELINE_VER."_".$scenario_dir."_".$FILE_NAME_TBP;
   $FILE_NAME_MERGE_FUNCTION_MIPS = $Q6_CORE_BASELINE_VER."_".$scenario_dir."_".$FILE_NAME_MERGE_FUNCTION_MIPS;
   $FILE_NAME_TOP_FUNCLIST_OF_THREAD = $Q6_CORE_BASELINE_VER."_".$scenario_dir."_".$FILE_NAME_TOP_FUNCLIST_OF_THREAD;
   $FILE_NAME_BUS_STATE = $Q6_CORE_BASELINE_VER."_".$scenario_dir."_".$FILE_NAME_BUS_STATE;
   $FILE_NAME_STACK_HEAP_USAGE=$Q6_CORE_BASELINE_VER."_".$scenario_dir."_".$FILE_NAME_STACK_HEAP_USAGE;
   $FILE_NAME_BUS_DATA=$Q6_CORE_BASELINE_VER."_".$scenario_dir."_".$FILE_NAME_BUS_DATA;
   $FILE_NAME_BUS_LOG=$Q6_CORE_BASELINE_VER."_".$scenario_dir."_".$FILE_NAME_BUS_LOG;
}


sub Create_scenario_dir
{
   $scenario_dir="D".$SIM_PROC_DSP_CLOCK."_A".$SIM_PROC_AXI_CLOCK;#"C".$SIM_PROC_DSP_CLOCK;

   if($INSTANCE_VOICE_CALL)
   {
      if($scenario_dir ne ""){
             $scenario_dir = $scenario_dir."_VOI_".lc($TYPE_VOCODER);       
      }else{
             $scenario_dir ="VOI_".lc($TYPE_VOCODER);         
      }
   }
   if($INSTANCE_VOICE_RECORDING)
   {
      if($scenario_dir ne ""){
         $scenario_dir = $scenario_dir."_REC_".lc($TYPE_VOI_RECORD);
      }else{
         $scenario_dir ="REC_".lc($TYPE_VOI_RECORD);
      }      
   }
   if($INSTANCE_VOICE_PLAYBACK)
   {
      if($scenario_dir ne ""){
         $scenario_dir = $scenario_dir."_PB_".lc($TYPE_VOI_PLAYBACK);
      }else{
         $scenario_dir ="PB_".lc($TYPE_VOI_PLAYBACK);
      }      
   }
   if($INSTANCE_AUDIO)
   {
      if($scenario_dir ne ""){
             $scenario_dir = $scenario_dir."_ADO_".lc($TYPE_AUDIOCODER);
      }else{
             $scenario_dir ="ADO_".lc($TYPE_AUDIOCODER);
      }      
   }

 # $instance=$INSTANCE_VIDEO_DEC;
 # print "ins:$instance\n";
 #  while($instance)
 #  {
   if($INSTANCE_VIDEO_DEC){
   
      if($scenario_dir ne ""){
            $scenario_dir = $scenario_dir."_VDO_DEC_".lc($TYPE_VDOCODER_DEC)."_".lc($TYPE_VDO_CLIP_DEC);
      }else{
            $scenario_dir = "VDO_DEC_".lc($TYPE_VDOCODER_DEC)."_".lc($TYPE_VDO_CLIP_DEC);
      }    
   }
 #    $instance--;
 #    print "ins:$instance\n";
 #  }
   if($INSTANCE_VIDEO_ENC)
   {
      if($scenario_dir ne ""){
            $scenario_dir = $scenario_dir."_VDO_ENC_".lc($TYPE_VDOCODER_ENC)."_".lc($TYPE_VDO_CLIP_ENC);
      }else{
            $scenario_dir = "VDO_ENC_".lc($TYPE_VDOCODER_ENC)."_".lc($TYPE_VDO_CLIP_ENC);
      }      
     
   }
   if($INSTANCE_MIDI_SYNTH){
      if($scenario_dir ne ""){
            $scenario_dir = $scenario_dir."_MIDI";
      }else{
            $scenario_dir = "MIDI";
      }  
   }

   if($INSTANCE_MDM)
   {
      if($scenario_dir ne ""){
             $scenario_dir = $scenario_dir."_MDM_".lc($TYPE_MDMTEST);
      }else{
             $scenario_dir ="MDM_".lc($TYPE_MDMTEST);
      }      
   }
   if($INSTANCE_MVS)
   {
      if($scenario_dir ne ""){
             $scenario_dir = $scenario_dir."_MVS_".lc($TYPE_MVSTEST);
      }else{
             $scenario_dir ="MVS_".lc($TYPE_MVSTEST);
      }      
   }
#   print "scenario =$scenario_dir\n";
   $scenario_dir;
}

@stk_dsp_clk;
@stk_axi_clk;
@stk_voi_instance;
@stk_voi_coder;
@stk_mdm_instance;
@stk_mvs_instance;
@stk_pb_instance;
@stk_pb_coder;
@stk_rec_instance;
@stk_rec_coder;
@stk_ado_instance;
@stk_ado_coder;
@stk_vdo_dec_instance;
@stk_vdo_enc_instance;
@stk_vdo_coder_dec;
@stk_vdo_coder_enc;
@stk_vdo_clip_dec;
@stk_vdo_clip_enc;
@stk_midi;
$stk_idx=0;
sub resetScenario {
   $SIM_PROC_DSP_CLOCK=600;
   $SIM_PROC_AXI_CLOCK=128;
   $INSTANCE_VOICE_CALL=0;
   $INSTANCE_MDM=0;
   $INSTANCE_MVS=0;
   $INSTANCE_VOICE_PLAYBACK=0;
   $INSTANCE_VOICE_RECORDING=0;
   $INSTANCE_AUDIO=0;
   $INSTANCE_VIDEO_DEC=0;
   $INSTANCE_VIDEO_ENC=0;
   $INSTANCE_MIDI_SYNTH=0;
   $INSTANCE_JPEG_ENC=0;
   
}
sub pushScenario {
#	print "\n\npushing\n\n";
	push(@stk_dsp_clk,$SIM_PROC_DSP_CLOCK);
#	print "dsp : $SIM_PROC_DSP_CLOCK\n";
	push(@stk_axi_clk,$SIM_PROC_AXI_CLOCK);
#	print "axi: $SIM_PROC_AXI_CLOCK\n";
	push(@stk_voi_instance,$INSTANCE_VOICE_CALL);
#	print "voi: $INSTANCE_VOICE_CALL, ";
	push(@stk_voi_coder,$TYPE_VOCODER);
#	print "voi coder: $TYPE_VOCODER\n";
        push(@stk_mdm_instance,$INSTANCE_MDM);
#	print "mdm instance: $INSTANCE_MDM\n";
        push(@stk_mvs_instance,$INSTANCE_MVS);
#	print "mvs instance: $INSTANCE_MVS\n";
	push(@stk_pb_instance,$INSTANCE_VOICE_PLAYBACK);
#	print "pb: $INSTANCE_VOICE_PLAYBACK,";
	push(@stk_pb_coder,$TYPE_VOI_PLAYBACK);
#	print "pb coder: $TYPE_VOI_PLAYBACK\n";
	push(@stk_rec_instance,$INSTANCE_VOICE_RECORDING);
#	print "rec : $INSTANCE_VOICE_RECORDING,";
	push(@stk_rec_coder,$TYPE_VOI_RECORD);
#	print "rec coder: $TYPE_VOI_RECORD\n";
	push(@stk_ado_instance,$INSTANCE_AUDIO);
#	print "ado : $INSTANCE_AUDIO,";
	push(@stk_ado_coder,$TYPE_AUDIOCODER);
#	print "ado coder: $TYPE_AUDIOCODER\n";
        
	push(@stk_vdo_enc_instance,$INSTANCE_VIDEO_ENC);
#	print "vdo : $INSTANCE_VIDEO_ENC, ";
        push(@stk_vdo_dec_instance,$INSTANCE_VIDEO_DEC);
#	print "vdo : $INSTANCE_VIDEO_DEC, ";
        push(@stk_vdo_coder_dec,$TYPE_VDOCODER_DEC);
#       print "vdo decoder: $TYPE_VDOCODER_DEC, ";
        push(@stk_vdo_coder_enc,$TYPE_VDOCODER_ENC);
#       print "vdo encoder: $TYPE_VDOCODER_ENC, ";	
	push(@stk_vdo_clip_dec,$TYPE_VDO_CLIP_DEC);
#	print "vdo clip dec: $TYPE_VDO_CLIP_DEC\n";
        push(@stk_vdo_clip_enc,$TYPE_VDO_CLIP_ENC);
#	print "vdo clip enc: $TYPE_VDO_CLIP_ENC\n";
        push(@stk_midi,$INSTANCE_MIDI_SYNTH);
	$stk_idx++;
	
}
sub popScenario {
   if($stk_idx==0){
      print "stack empty\n";
      return 0;
   }
   $stk_idx--;
   $SIM_PROC_DSP_CLOCK=pop(@stk_dsp_clk);
   $SIM_PROC_AXI_CLOCK=pop(@stk_axi_clk);

   $INSTANCE_VOICE_CALL=pop(@stk_voi_instance);
   $TYPE_VOCODER=pop(@stk_voi_coder);

   $INSTANCE_MDM=pop(@stk_mdm_instance);
   $INSTANCE_MVS=pop(@stk_mvs_instance);
   $INSTANCE_VOICE_PLAYBACK=pop(@stk_pb_instance);
   $TYPE_VOI_PLAYBACK=pop(@stk_pb_coder);

   $INSTANCE_VOICE_RECORDING=pop(@stk_rec_instance);
   $TYPE_VOI_RECORD=pop(@stk_rec_coder);

   $INSTANCE_AUDIO=pop(@stk_ado_instance);
   $TYPE_AUDIOCODER=pop(@stk_ado_coder);

   $INSTANCE_VIDEO_DEC=pop(@stk_vdo_dec_instance);
   $TYPE_VDOCODER_DEC=pop(@stk_vdo_coder_dec);
   $TYPE_VDO_CLIP_DEC=pop(@stk_vdo_clip_dec);

   $INSTANCE_VIDEO_ENC=pop(@stk_vdo_enc_instance);
   $TYPE_VDOCODER_ENC=pop(@stk_vdo_coder_enc);
   $TYPE_VDO_CLIP_ENC=pop(@stk_vdo_clip_enc);

   $INSTANCE_MIDI_SYNTH=pop(@stk_midi);
   
        $index_key=&Create_scenario_dir;
#	print "\n\npoping\n\n";
#	print "dsp : $SIM_PROC_DSP_CLOCK\n";
#	print "axi: $SIM_PROC_AXI_CLOCK\n";
#	print "voi: $INSTANCE_VOICE_CALL, ";
#	print "voi coder: $TYPE_VOCODER\n";
#	print "pb: $INSTANCE_VOICE_PLAYBACK, ";
#	print "pb coder: $TYPE_VOI_PLAYBACK\n";
#	print "rec : $INSTANCE_VOICE_RECORDING, ";
#	print "rec coder: $TYPE_VOI_RECORD\n";
#	print "ado : $INSTANCE_AUDIO, ";
#	print "ado coder: $TYPE_AUDIOCODER\n";
#	print "vdo dec: $INSTANCE_VIDEO_DEC, ";
#	print "vdo decoder: $TYPE_VDOCODER_DEC, ";
#	print "vdo clip dec: $TYPE_VDO_CLIP_DEC\n";
#	print "vdo enc: $INSTANCE_VIDEO_ENC, ";
#	print "vdo encoder: $TYPE_VDOCODER_ENC, ";
#	print "vdo clip enc: $TYPE_VDO_CLIP_ENC\n";

   return 1;
}
sub getRequireNum {
   $idx=0;
   if($INSTANCE_VIDEO_DEC){
      $idx+=1;
   }
   if($INSTANCE_AUDIO){
      if($TYPE_AUDIOCODER eq "1EAAC_P"){
         $idx+=2;
      }
      if($TYPE_AUDIOCODER eq "1MP3"){
         $idx+=4;
      }
      if($TYPE_AUDIOCODER eq "EAAC_P_MP3"){
         $idx+=6;
      }
   }
   if($INSTANCE_VOICE_PLAYBACK){
      $idx+=8;
   }
   if($INSTANCE_VOICE_RECORDING){
      $idx+=16;
   }
   if($INSTANCE_VOICE_CALL){
      $idx+=32;
   }
   if($INSTANCE_VIDEO_ENC){
      $idx+=64;
   }
   if($INSTANCE_MIDI_SYNTH){

      $idx+=128;
   }
   return $MMReqNum{"$idx"};
}
sub get_cfg_list {
   @list;
   my @arg = @_;
   %cfg_group_list;
   $cfg_list_handle = "./qdsp6/scripts/cfg/".$arg[0];
   open cfg_list_handle or die "cant open $cfg_list_handle ";
   while(<cfg_list_handle>){
      $gmon_new = $_;
      chop;
      if(!exists($cfg_group_list{$gmon_new})){
         $cfg_group_list{$gmon_new}=1;
         push(@list,$_);
      }
     
      
   }
   @list;
}
sub generate_shell_script {
     my @arg = @_;
     $baseline =$arg[0];
     $cfg=$arg[1];
     $shellHandle='';
     $shellHandle="> $ABS_DIR_SPT_SHELL/".$scenario_dir.".csh";
     open shellHandle or die "cant open $shellHandle";
     print shellHandle "#!/bin/tcsh\n";
     if($INSTANCE_VIDEO_DEC){
        if($TYPE_VDOCODER_DEC eq "H264"){
           print shellHandle "source qdsp6/scripts/shell/cs_617.cshrc\n";
           print shellHandle "source qdsp6/scripts/shell/vdec_csim_617.cshrc\n";
        }
        if($TYPE_VDOCODER_DEC eq "VC1"){
           print shellHandle "source qdsp6/scripts/shell/vdec_csim.cshrc\n";
        }

     } 
     if($INSTANCE_VIDEO_ENC && $INSTANCE_VIDEO_DEC  ){
        print shellHandle "source qdsp6/scripts/shell/set_qdsp6_env_encdec\n";
     }elsif($INSTANCE_VIDEO_ENC){ 
        print shellHandle "source qdsp6/scripts/shell/set_qdsp6_env_enc\n";
     }else{
        print shellHandle "source qdsp6/scripts/shell/set_qdsp6_env\n";
     }

     print shellHandle "setenv DRM_PROJECT 71642\n";
     print shellHandle "perl qdsp6/scripts/perl/System_Profile.pl $cfg\n";
     close shellHandle;
}

1;
