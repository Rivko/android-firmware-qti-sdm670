
$min_timestamp_addr = 0;
$binary = 0;
$current_freq_seq = undef;
%translations = (
  "SwtchIdx" => "local_switch_log_index  ",
  "GetNeeds" => "get_aop_needs           ",
  "SendVote" => "arc_send_vote           ",
  "ERR:TCS2" => "service_fal: TCS2 used  ",
  "RailChng" => "rail_change_handler     ",
  "ERR:VSEQ" => "Invalid CX Voltage Seq  ",
  "ARC CONT" => "ARC_CONTINUE_PROCESSING ",
  "PS_PreWk" => "arc_unblock_ps_prework  ",
  "RM_PreWk" => "arc_unblock_rm_prework  ",
  "VoltDone" => "Voltage Sequence Done   ",
  "Volt Rdy" => "Voltage_Dependency_Ready",
  "Run VSeq" => "Running Voltage Seq     ",
  "NO VSEQ!" => "NOT Running Voltage Seq ",
  "Unblk MC" => "UNBLOCK_BCM_MC          ",
  "UnblkSHB" => "UNBLOCK_BCM_SHUB        ",
  "Unblk_PS" => "UNBLOCK_POWERSTATE_ARC  ",
  "Procssed" => "FREQ_FSM PROCESS DONE   ",
  "StrtMemT" => "Starting Memory Test    ",
  "CmdDBTst" => "CMD_DB_MEMORY_TEST      ",
  "DBTstBAD" => "CMD_DB_MEMORY_TEST FAIL ",
  "DBTst OK" => "CMD_DB_MEMORY_TEST PASS ",
  "FreqStrt" => "Starting Frequency Seq  ",
  "Log Stop" => "Freq_exit stopping log  ",
  "FreqDone" => "Frequency Seq complete  ",
  "ERR:TCS0" => "readdata: TCS 0 in use  ",
  "ReadData" => "Flush readdata complete ",
  "ERR:TCS0" => "flush_entry: TCS0 used  ",
  "ERR:TCS1" => "flush_exit: TCS1 used   ",
  "LLC_Evnt" => "Event during LLC Flush  ",
  "MainLoop" => "MAIN LOOP PROCESSING    ",
  "ZeroTemp" => "sig_zero_temp_event     ",
  "DDR ISR " => "DDR_MGR_ISR_FUNC ISR    ",
  "PMIC CB " => "PMIC Callback           ",
  "FreqNeed" => "In Frequency_Need_Met   ",
  "DCCEnter" => "Waiting for Bandwidth   ",
  "DCC Exit" => "Memtest Bandwidth ready ",
  "BusVt On" => "Voting Bandwidth on     ",
  "BusVtOff" => "Voting Bandwidth off    ",
  "DEBUG   " => "Debug Timer Expired     ",
  "XO OFF  " => "XO Voted Off            ",
  "XO start" => "Waiting for XO complete ",
  "XO done." => "XO Vote Completed.      ",
  "Block CX" => "CX seq waiting for XO   ",
  "ERRreqid" => "Unexpected Request ID   ",
  "VoltPend" => "Waiting for Volt Req    ",
  "Volt CB " => "Voltage Request Complete",
  "SendVolt" => "Checking Outstand Votes ",
  "VotesOut" => "Voltages Summary        ",
  "DDR Vote" => "DDR Vote Sent to uNPA   ",
  "CXVoteCB" => "CX uNPA Driver Callback ",
  "MXVoteCB" => "MX uNPA Driver Callback ",
  "VAVoteCB" => "VDDA NPA Driver Callback",
  "MXRet CX" => "Block CX:MX at Retention",
  "MXSeqReq" => "Block CX:MX Sequencing  ",
  "MXDepCSC" => "Block CX:MX needs TURBO ",
  "DDRclnCB" => "DDR_MSG Driver Callback ",
);

sub relative
{
   $last = shift;
   $first = shift;
   $rel = int(($last - $first) / 19.2);
   #$units = "us";
   $units = "";
   if ($rel >= 100000)
   {
      $rel = $rel / 1000;
      if ($rel >= 1000) {
         $rel = int($rel+0.5)/1000;
         $units = " sec";
      } else {
         $rel = int($rel+0.5);
         $units = " ms";
      }
   }
   return "$rel" . "$units";
}

sub flip
{
   $str = $_[0];
   if ($str =~ m/(..)(..)/)
   {
     return $2 . $1;
   }
}
sub parse_fields
{
  $count = $_[0];
  $line = $_[1];
  #       8 2   41   41   41   41   41  341123411234121234123456781234
  #00000000: 4fff a205 4d61 696e 4c6f 6f70 0430 0000  O...MainLoop.0..
  #0000000: 90a8 5c49 4152 4320 434f 4e54 0001 0100  ..\IARC CONT..../
  if ($line =~ m/([0-9a-f]+)..([0-9a-f]{4}).([0-9a-f]{4}).([0-9a-f]{4}).([0-9a-f]{4}).([0-9a-f]{4}).([0-9a-f]{4}).([0-9a-f]{4}).([0-9a-f]{4})......(.{1,8})/i)
  {
    $log_msgs[$count]{addr}      = $1;
    $log_msgs[$count]{timestamp} = flip($3) . flip($2);
    $log_msgs[$count]{msg1}      = flip($5) . flip($4);
    $log_msgs[$count]{msg2}      = flip($7) . flip($6);
    $log_msgs[$count]{data}      = flip($9) . flip($8);
    if (defined($translations{$10}))
    {
      $log_msgs[$count]{msg_text} = $translations{$10};
    }
    else
    {
      $log_msgs[$count]{msg_text} = $10 . " " x (24-length($10));
    }
    return 1;
  }
  elsif ($line =~ m/([0-9A-F]{8})..([0-9A-F]{8}).([0-9A-F]{8}).([0-9A-F]{8}).([0-9A-F]{8}).....(.{1,8})/i)
  {
    $log_msgs[$count]{addr}      = $1;
    $log_msgs[$count]{timestamp} = $2;
    $log_msgs[$count]{msg1} =      $3;
    $log_msgs[$count]{msg2} =      $4;
    $log_msgs[$count]{data} =      $5;
    if (defined($translations{$6}))
    {
      $log_msgs[$count]{msg_text} = $translations{$6};
    }
    else
    {
      $log_msgs[$count]{msg_text} = $6 . " " x (24-length($6));
    }
    return 1;
  }
  elsif ($line =~ m/([0-9A-F]{7})..([0-9A-F]{8}).([0-9A-F]{8}).([0-9A-F]{8}).([0-9A-F]{8}).....(.{1,8})/i)
  {
    $log_msgs[$count]{addr}      = $1;
    $log_msgs[$count]{timestamp} = $2;
    $log_msgs[$count]{msg1} =      $3;
    $log_msgs[$count]{msg2} =      $4;
    $log_msgs[$count]{data} =      $5;
    if (defined($translations{$6}))
    {
      $log_msgs[$count]{msg_text} = $translations{$6};
    }
    else
    {
      $log_msgs[$count]{msg_text} = $6 . " " x (24-length($6));
    }
    return 1;
  }
  else
  {
    #print "No: [$line]\n";
  }
  return 0;
}

sub by_position()
{
  %hash_a = %{$a};
  %hash_b = %{$b};
  $addr1 = hex($hash_a{addr})/16;
  $addr1 += 100 if ($addr1 < $min_timestamp_addr);
  $addr2 = hex($hash_b{addr})/16;
  $addr2 += 100 if ($addr2 < $min_timestamp_addr);
  return $addr1 <=> $addr2;
}

%clients = (
  "00" => "FREQUENCY",
  "01" => "DEPENDENCY",
  "02" => "TEMP_EVENT",
);
%resources = (
  "00" => "MX",
  "01" => "CX",
  "02" => "VDDA",
  "03" => "PMIC",
  "04" => "DDR",
  "05" => "XO",
);
%rm_names = (
  "00" => "CX",
  "01" => "MX",
  "02" => "VDDA",
  "03" => "SSC_CX",
  "04" => "SSC_MX",
  "05" => "GFX",
  "06" => "MSS",
  "07" => "DDR_SS",
  "08" => "XO",
);

%sigs = (
  "000001" => "CX_RM Agg",
  "000002" => "MX_RM Agg",
  "000004" => "VDDA_RM Agg",
  "000008" => "DDR_RM",
  "000010" => "BCM",
  "000020" => "Bandwidth Avail",
  "000040" => "XO Complete",
  "000080" => "Updated Freq Plan",
  "000100" => "Seq 0 (Frequency) Done ",
  "000200" => "Seq 1 (Voltage) Done",
  "000400" => "TCS DONE",
  "000800" => "PMIC Vote DONE",
  "001000" => "CX Prework",
  "002000" => "MX Prework",
  "004000" => "VDDA Prework",
  "008000" => "FAL Interrupt",
  "010000" => "CX Postwork",
  "020000" => "MX Postwork",
  "040000" => "Comparator done",
  "080000" => "Zero Degree C",
  "100000" => "Poll Timer",
  "200000" => "Voltage Sequence",
);

%frequency_event = (
  "000001" => "Frequency Change",
  "000002" => "Voltage Requirements Met",
  "000004" => "DCC Complete",
  "000008" => "Frequency Sequence Complete",
  "000010" => "Flush TCS Done",
  "000020" => "Process Complete",
);

sub pending_decode_new
{
  $code = $_[0];
  if ($code eq "00") {
    return "Pending None";
  } elsif ($code eq "01") {
    return "Pending Sequence";
  } elsif ($code eq "02") {
    return "Deferred Sequence";
  } else {
    return "$code";
  }
}
sub pending_decode_old
{
  $code = $_[0];
  if ($code eq "00") {
    return "Pending None";
  } elsif ($code eq "01") {
    return "Below MOL Agg";
  } elsif ($code eq "02") {
    return "Pending Agg";
  } elsif ($code eq "03") {
    return "Pending Sequence";
  } elsif ($code eq "04") {
    return "Deferred Agg";
  } else {
    return "$code";
  }
}

sub fsm_events
{
  $data = hex($_[0]);
  $fsm_list = undef;
  foreach $key (sort keys %frequency_event)
  {
    if ((hex($key) & $data) > 0)
    {
      if (defined($fsm_list))
      {
        $fsm_list = $fsm_list . ", " . $frequency_event{$key};
      }
      else
      {
        $fsm_list = $frequency_event{$key};
      }
    }
  }
  return "$fsm_list";
}

sub expand_data($$)
{
  $msg = $_[0];
  $data = $_[1];
  if ($msg =~ m/ARC_CONTINUE_PROCESSING/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      $arc = $1;
      $seq_lvl = $2;
      $in_seqlvl = $3;
      $arc = $resources{$arc} if defined($resources{$arc});
      print "ARC $arc SequenceLvl $seq_lvl  In_sequence $in_seqlvl";
    }
  }
  elsif ($msg =~ m/arc_unblock_rm_prework/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      $arc_rm = $1;
      $current = $2;
      $seq_lvl = $3;
      $in_seqlvl = $4;
      $arc_rm = $rm_names{$arc_rm} if defined($rm_names{$arc_rm});
      print "ARC_RM $arc_rm Current $current SequenceLvl $seq_lvl  In_sequence $in_seqlvl";
    }
  }
  elsif ($msg =~ m/arc_unblock_ps_prework/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      $arc = $1;
      $arc_lvl = $4;
      $arc = $resources{$arc} if defined($resources{$arc});
      print "ARC $arc ddr_arc_lvl $arc_lvl";
    }
  }
  elsif ($msg =~ m/DDR_MGR_ISR/)
  {
    if ($data =~ m/(..)(......)/)
    {
      $int = $1;
      if (defined $sigs{$2}) {
        $sig = $sigs{$2};
      }
      else
      {
        $sig = $2;
      }

      print "Interrupt $int Signal $sig";
    }
  }
  elsif ($msg =~ m/arc_send_vote/)
  {
    if ($data =~ m/(..)(..)(....)/)
    {
      $arc = $1;
      $client = $2;
      $value = $3;
      $arc = $resources{$arc} if defined($resources{$arc});
      $client = $clients{$client} if defined($clients{$client});
      print "Resource $arc Client $client Value $value";
    }
  }
  elsif ($msg =~ m/NOT Running Voltage Seq/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      print "MX:" . pending_decode_new($1);
      print " CX:" . pending_decode_new($2);
      print " VDDA:" . pending_decode_new($3);
    }
  }
  elsif (($msg =~ m/UNBLOCK_BCM_MC/) || ($msg =~ m/UNBLOCK_BCM_SHUB/))
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      print "Current:$1, BCM:$2, Next BCM Lvl:$3";
    }
  }
  elsif ($msg =~ m/UNBLOCK_POWERSTATE_ARC/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      print "Current MC:$1, Ack Lvl:$2, ARC Lvl:$3";
    }
  }
  elsif ($msg =~ m/Running Voltage Seq/)
  {
    if ($data =~ m/(..)(..)(....)/)
    {
      $mask = $2;
      %masks = (
        "01" => "MX Only",
        "02" => "CX Only",
        "03" => "CX/MX",
        "04" => "VDDA Only",
        "05" => "MX & VDDA",
        "06" => "CX & VDDA",
        "07" => "CX/MX/VDDA"
      );
      $mask = $masks{$2} if defined($masks{$2});
      print "Mask $mask";
    }
  }
  elsif ($msg =~ m/Voltage Sequence Done/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      $arc = $1;
      $was = $2;
      $now = $3;
      $arc = $resources{$arc} if defined($resources{$arc});
      print "$arc Was $was Became $now";
    }
  }
  elsif ($msg =~ m/Starting Frequency Seq/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      $mc = $1;
      $shub = $2;
      $start_addr = $3;
      $current_freq_seq = undef;
      $current_freq_seq = "SHUB Only" if ($start_addr eq "BC");
      $current_freq_seq = "MC Only" if ($start_addr eq "C4");
      $current_freq_seq = "MC/SHUB Parallel" if ($start_addr eq "F0");
      print "MC to $mc SHUB to $shub Sequence offset $start_addr";
      print " - $current_freq_seq" if (defined($current_freq_seq));
    }
  }
  elsif ($msg =~ m/Frequency Seq complete/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      $mc = $1;
      $shub = $2;
      print "MC at $mc SHUB at $shub";
      $elapsed = hex($3) * 256 + hex($4);
      if ($elapsed > 0)
      {
        $elapsed_time = int($elapsed/19.2 + 0.5);
        print " Elapsed $elapsed_time";
      }
      print " - $current_freq_seq" if (defined($current_freq_seq));
      $current_freq_seq = undef;
      print " *****************";
    }
  }
  elsif ($msg =~ m/Flush readdata complete/)
  {
    if ($data =~ m/(........)/)
    {
      print "PCB Data $1";
    }
  }
  elsif ($msg =~ m/rail_change_handler/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      %events = (
        "00" => "Agg",
        "01" => "Pre",
        "03" => "Post",
        );

      $arc = $1;
      $event = $2;
      $aop_needs = $3;
      $desired_ol = $4;

      $arc = $resources{$arc} if defined($resources{$arc});
      $event = $events{$event} if defined($events{$event});
      print "$arc Event $event DDR_NEEDS $aop_needs Desired OL $desired_ol";
    }
  }
  elsif ($msg =~ m/Event during LLC Flush/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      $event = $1;
      $next_state = $2;
      $flush_status = $3;
      $pcb_mask = $4;
      print "Events:$event\[";
      print fsm_events($event);
      print "], Next_State:" . $2;
      print ", Flush_Status:" . $3;
      print ", PCB:" . $4;
    }
  }
  elsif ($msg =~ m/MAIN LOOP PROCESSING/)
  {
    $signals = hex($data);
    $list = undef;

    foreach $key (sort keys %sigs)
    {
      if ((hex($key) & $signals) > 0)
      {
        if (defined($list))
        {
          $list = $list . ", " . $sigs{$key};
        }
        else
        {
          $list = $sigs{$key};
        }
      }
    }
    print "$list";
  }
  elsif ($msg =~ m/PSInt/)
  {
    $elapsed = int(hex($data)/19.2);
    print "Time from event to now is $elapsed us";
  }
  elsif ($msg =~ m/Freq_exit stopping log/)
  {
    $elapsed = int(hex($data)/19.2);
    print "Logging stopped, transition took $elapsed us";
  }
  elsif (($msg =~ m/CX Callback/) ||
         ($msg =~ m/MX Callback/) ||
         ($msg =~ m/VDDA Callback/))
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      $request_id = $1 . $2;
      $pending = $3;
      $vote = $4;
      print "Request $request_id Pending $pending Vote value $vote";
    }
  }
  elsif ($msg =~ m/PMIC Callback/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      if ($4 cmp "00")
      {
         $request_id = $1 . $2;
         $pending = $3;
         $vote = $4;
         $request_id = "0x" . $request_id . " (" . hex($request_id) . ")";
      }
      else
      {
         $request_id = $1;
         $pending = $2;
         $vote = $3;
      }
      print "Request $request_id Pending $pending Vote value $vote";
    }
  }
  elsif ($msg =~ m/FREQ_FSM PROCESS DONE/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      $mc = $1;
      $shub = $2;
      $next_state = $4;
      $next_state = "IDLE" if ($next_state == 0);
      $next_state = "Voltage" if ($next_state == 2);
      print "MC going to $mc Shub going to $shub Next state $next_state";
    }
  }
  elsif ($msg =~ m/In Frequency_Need_Met/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
       $pmic_pending = $1;
       $mx_min_state = $2;
       $cx_min_state = $3;
       $vdda_min_state = $4;

       print "PMIC Pending $pmic_pending CX at $cx_min_state MX at $mx_min_state VDDA at $vdda_min_state";
    }
  }
  elsif ($msg =~ m/Waiting for Bandwidth/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      print "Bandwidth available: $1"
    }
  }
  elsif ($msg =~ m/Memtest Bandwidth ready/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      print "Bandwidth previously available: $1"
    }
  }
  elsif ($msg =~ m/Voting Bandwidth on/)
  {
    if ($data =~ m/(........)/)
    {
      if ($1 cmp "00000000")
      {
        print "Bandwidth request: $1"
      }
      else
      {
        print "Bandwidth already available $1";
      }
    }
  }
  elsif ($msg =~ m/Voting Bandwidth off/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      print "Bandwidth available: $1"
    }
  }
  elsif ($msg =~ m/Debug Timer Expired/)
  {
    if ($data =~ m/(........)/)
    {
      print "Check DDR Ulogs";
      if ($1 & 1) {
        print ": DDRAUX Sequence Running";
      }
      if ($1 & 2) {
        print ": SHRM Not Responding to commands";
      }
    }
  }
  elsif ($msg =~ m/XO Voted Off/)
  {
     # Nothing to do
  }
  elsif (($msg =~ m/Freq Voting FireForget/) ||
         ($msg =~ m/Freq Voting Resp Req/))
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      printf("Num Commands:%d CX=%X, MX=%X, VDDA=%X",
              $1, hex($2)*16,hex($3)*16,hex($4)*16);
    }
  }
  elsif ($msg =~ m/Checking Outstand Votes/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      $cx_pend = $2;
      $mx_pend = $3;
      $vdda_pend = $4;
      print "CX Pending:$cx_pend; MX Pending:$mx_pend; VDDA Pending:$vdda_pend";
    }
  }
  elsif ($msg =~ m/Checking for pending/)
  {
    if ($data =~ m/(.)(.)(..)(....)/)
    {
      $arc = $2;
      $pending = $3;
      $req_id = $4;

      $arc = "MX" if ($arc =~ m/0/);
      $arc = "CX" if ($arc =~ m/1/);
      $arc = "VDDA" if ($arc =~ m/2/);

      print "ARC:$arc Pending:$pending  Req_id:$req_id";
    }
  }
  elsif ($msg =~ m/Voltages Summary/)
  {
    #"VotesOut" => "Voltages Summary        ",
    if ($data =~ m/(.)(.)(.)(.)(.)(.)(.)(.)/)
    {
      $votes = $1;
      $resp  = $2;
      $vdda_was = $3;
      $vdda_now = $4;
      $cx_was  = $5;
      $cx_now  = $6;
      $mx_was  = $7;
      $mx_now  = $8;

      printf("%d votes %d RR. CX %d->%d, MX %d->%d, VDDA %d->%d",
             $votes, $resp, $cx_was, $cx_now, $mx_was,
             $mx_now, $vdda_was, $vdda_now);
    }
  }
  elsif ($msg =~ m/DDR Vote Sent to uNPA/)
  {
    if ($data =~ m/(.)(.......)/)
    {
      $arc = $1;
      $req_id = $2;

      $arc = "MX" if ($arc =~ m/0/);
      $arc = "CX" if ($arc =~ m/1/);
      $arc = "VDDA" if ($arc =~ m/2/);

      print "ARC:$arc; Request ID: $req_id";
    }
  }
  elsif ($msg =~ m/NPA Driver Callback/)
  {
    if ($data =~ m/(....)(..)(..)/)
    {
      $req_id = $1;
      $pending = $2;
      $vote = $3;

      $vote = hex($3)*16;
      $vote = "MIN_SVS" if ($vote == hex('30'));
      $vote = "LOW_SVS" if ($vote == hex('40'));
      $vote = "SVS"     if ($vote == hex('80'));
      $vote = "SVS_L1"  if ($vote == hex('C0'));
      $vote = "NOMINAL" if ($vote == hex('100'));
      $vote = "TURBO  " if ($vote == hex('180'));
       
      print "Request ID:$1; Pending:$pending;  Vote:$vote";
    }
  }
  elsif (($msg =~ m/Waiting for XO complete/) ||
         ($msg =~ m/XO Vote Completed/)       ||
         ($msg =~ m/CX seq waiting for XO/)   ||
         ($msg =~ m/Waiting for Volt Req/)    ||
         ($msg =~ m/Block CX:MX at Retention/)||
         ($msg =~ m/Block CX:MX Sequencing/)  ||
         ($msg =~ m/Voltage Request Complete/))
  {
    if ($data =~ m/(........)/)
    {
       print "Request ID: $1";
    }
  }
  elsif ($msg =~ m/Block CX:MX needs TURBO/)
  {
    #"MXDepCSC" => "Block CX:MX needs TURBO ",
    if ($data =~ m/(........)/)
    {
       print "MX_ARC_LEVEL:$1";
    }
  }
  elsif ($msg =~ m/DDR_MSG Driver Callback/)
  { 
    #"DDRclnCB" => "DDR_MSG Driver Callback ",
    if ($data =~ m/(........)/)
    {
       print "Request ID: $1";
    }
  }
  elsif ($msg =~ m/Starting Memory Test   /)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      print "Next MC:$1, Next Shub:$2, Address:$3, AOP Read DDR only:$4";
    }
  }
  elsif ($msg =~ m/CMD_DB_MEMORY_TEST     /)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      print "Next MC:$1, Next Shub:$2";
    }
  }
  elsif ($msg =~ m/CMD_DB_MEMORY_TEST FAIL/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      print "Next MC:$1, Next Shub:$2";
    }
  }
  elsif ($msg =~ m/CMD_DB_MEMORY_TEST PASS/)
  {
    if ($data =~ m/(..)(..)(..)(..)/)
    {
      print "Next MC:$1, Next Shub:$2";
    }
  }
  else
  {
    print "...";
  }
}

if (!defined $data)
{
  $file = shift;
  if (-d $file) {
    @trials = qw(MSGRAM6.BIN aop_msg6_0xc360000--0xc3603ff.lst);
    foreach $trial (@trials) {
      print "Trying $trial\n";
      $try = $file . "/" . $trial;
      if (-f $try)
      {
        $file = $try;
        last;
      }
    }
  }
  print "Trying to open $file\n";
  open LOGFILE, "< $file" || die "Cannot open $file: $!";
  print "Logfile $file opened\n";
  if (($file =~ m/.bin$/i) || ($file =~ m/.lst$/i))
  {
    $total = 0;
    binmode LOGFILE;
    $binary = 1;
    while (read(LOGFILE, $bindata, 16) == 16)
    {
      ($word1, $word2, $word3, $word4) = unpack("I I I I", $bindata);
      $ascii = unpack("x4 a8", $bindata);
      $line = sprintf ("%08.8X: %08.8X %08.8X %08.8X %08.8X     %s    ", $total, $word1, $word2, $word3, $word4, $ascii);
      $total = $total + 16;
      push(@lines, $line);
    }
    print "Total read: $total\n";
  }
  else 
  {
    local $/;
    while (<LOGFILE>) {
      $data = $data . $_;
    }
    @lines = split /\n/, $data;
  }
  close LOGFILE;
}
else
{
  @lines = split /\n/, $data;
}
@log_msgs = ();
$count = 0;
$ts_hi_used = 0;
$ts_lo_used = 0;
foreach $line (@lines)
{
  if (parse_fields($count, $line))
  {
    $hex_ts = hex($log_msgs[$count]{timestamp});
    $ts_hi_used = 1 if (($hex_ts & 0xF0000000) == 0xF0000000);
    $ts_lo_used = 1 if (($hex_ts & 0xF0000000) == 0x00000000);
    $count++;
  }
}

$wrap_ts = 1 if ($ts_hi_used && $ts_lo_used);
$last_timestamp = hex("1FFFFFFFF");

for $index (0 .. $#log_msgs)
{
  $hex_ts = hex($log_msgs[$index]{timestamp});
  $hex_ts += hex(100000000) if ($wrap_ts && (($hex_ts & 0xC0000000) == 0x00000000));

  if ($hex_ts < $last_timestamp)
  {
    $min_timestamp_addr = hex($log_msgs[$index]{addr})/16;
    print "Min Timestamp is $log_msgs[$index]{timestamp} at index $index (last $last_timestamp)\n";
    $last_timestamp = $hex_ts;
  }
}

@log_msgs = sort by_position @log_msgs;
$log_count = 1+$#log_msgs;
print "Log msgs have $log_count\n";
$time_offset = 0;
$last_ts = 0;

foreach $hash_ref (@log_msgs)
{
  %line_hash = %{$hash_ref};
  $time = hex($line_hash{timestamp});
  $delta = $time - $last_ts;
  if ($time_offset <= 0)
  {
    $time_offset = $time;
  }
  if ($delta > 192000)
  {
    printf("-------------------:------------------------------------------------------------------------\n");
    $rel_time = relative($time, $last_ts);
    $time_offset = $time;
  }
  else
  {
    $rel_time = relative($time, $time_offset);
  }
  $last_ts = $time;
  printf("%8s %10s: %s %08s ", $line_hash{timestamp}, $rel_time,
         $line_hash{msg_text}, $line_hash{data});
  expand_data($line_hash{msg_text}, $line_hash{data});
  print "\n";
}
