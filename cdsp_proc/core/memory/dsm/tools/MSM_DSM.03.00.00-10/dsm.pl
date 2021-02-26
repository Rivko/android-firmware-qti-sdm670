# *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
#
#                                dsm.pl
#
# GENERAL DESCRIPTION
#   A script to parse the output received from running the dsmX.cmm file. 
#
#   Usage: perl dsm.pl dsm_memory_pool_dump.txt <parsed_output_file>.txt
#
#
# Copyright(c) 1999 - 2009 by QUALCOMM, Incorporated. All Rights Reserved.
# *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====
#
#                       EDIT HISTORY FOR FILE
#
#   This section contains comments describing changes made to this file.
#   Notice that changes are listed in reverse chronological order.
#
# $Header: //source/qcom/qct/modem/mcs/dsm/rel/1h09/tools/MSM_DSM.03.00.00-10/dsm.pl#1
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------  
# 05/04/09   ag      Merged from CVS.
# 
# ---------------------------------------------------------------------------

use Data::Dumper;

my %poolname = ( 0 => "SMALL",
		 1 => "LARGE",
		 2 => "DUP",
		 3 => "DIAG");

# This is how to format various keys on output.
my %fkeys = ("POOL" => "%1d",
             "POOLNAME" => "%-5s",
             "ADDR" => "0x%08X",
             "PKT_PTR" => "0x%08X",
             "DUP_PTR" => "0x%08X",
             "LINK" => "0x%08X",
             "REFERENCES" => "%2d",
             "USED" => "%4d",
             "SIZE" => "%4d",
             "DATA_PTR" => "0x%08X",
             "KIND" => "%3d",
             "PRIORITY" => "%3d",
             "APP_FIELD" => "0x%08X",
             "APP_PTR" => "0x%08X",
             "TRACER" => "%4X",
             "FILE" => "%-20s",
             "LINE" => "%-5d");

# This is the order of fields in the input file.
my @ikeys = ("POOL",
             "ADDR",
             "PKT_PTR",
             "DUP_PTR",
             "LINK",
             "REFERENCES",
             "USED",
             "SIZE",
             "DATA_PTR",
             "KIND",
             "PRIORITY",
             "APP_FIELD",
             "APP_PTR",
             "TRACER",
             "FILE",
             "LINE");

# This is the order of fields in the output file.
my @okeys = ("POOL",
             "ADDR",
             "PKT_PTR",
             "DUP_PTR",
             "LINK",
             "REFERENCES",
             "USED",
             "SIZE",
             "DATA_PTR",
#             "KIND",
#             "PRIORITY",
#             "APP_FIELD",
#             "APP_PTR",
             "TRACER",
             "FILE",
             "LINE");

sub get_record {
	my %record;
	my $line;
	my $key;
	my @rest;
LINE:	while (<>) {
		chomp;
		$line = $_;
		if ($line =~ /^#/) {
			next LINE;
		} elsif ($line =~ /^POOL_ID,/) {
			next LINE;
		} elsif ($line =~ /^................/){
			my @tkeys = @ikeys;
			%record = map { shift(@tkeys) => $_ } split(",",$line);
			return \%record;
		} 
	}
	return undef;
}
sub print_record {
	my $record = shift;
	my $first = 0;
	foreach $key (@okeys) {
		if($first == 0) {
			$first = 1;
		} else { 
			print ", ";
		}

		if(defined $$record{$key}) {
			printf($fkeys{$key}, $$record{$key});
		}

	}
	print "\n";
}

sub normalize_record {
        my $record = shift;
        my $first = 0;
        foreach $key (keys %fkeys) {
		next if(!defined($$record{$key})); 
                if($$record{$key} =~ /^\s*0x([0-9a-fA-F]+)/) {
			my $val = $1;
			$$record{$key} = hex($val); 
                } elsif ($$record{$key} =~ /^\s*(.*)$/) {
                        my $val = $1;
                        $$record{$key} = $val;
		}
        }
	return $record;
}
sub augment_record {
        my $record = shift;
	$$record{"POOLNAME"} = $poolname{$$record{"POOL"}};
	return $record;
}


# Read in and normalize the datafile
my %item;
while (defined ($record = get_record())) {

        normalize_record($record);
        augment_record($record);
	if(defined($item{$$record{"ADDR"}})) {
		# If this fires the same record shows up in the input twice
		printf("#ERROR: DUPLICATE RECORD: 0x%08X\n", $$record{"ADDR"});
	} else {
        	$item{$$record{"ADDR"}}	= normalize_record($record);
	}
}

# Build a list of item heads: copy the item hash then remove every item
# from that hash that is refered by an element of the item hash.
sub get_packets {
	my $itemref = shift;
	my %pkt = %$itemref; 
	foreach $key (keys %$itemref) {
		my $ptr = $$itemref{$key}->{"PKT_PTR"};
		if(defined($pkt{$ptr})) {
			delete($pkt{$ptr});
		} elsif ($ptr != 0) {
			#if this fires there is a good chance that
			#either memory is corrupted, or 2 dsm items
			#prepend a common chain.  Either thing is bad.
			printf("#ERROR: ITEM ADDRESS NOT FOUND: 0x%08X\n",$ptr);
		}
	}
	return \%pkt;
}

sub get_allocated {
        my $itemref = shift;
        my %pkt = %$itemref;
        foreach $key (keys %$itemref) {
		if($pkt{$key}->{"REFERENCES"} == 0) {
			delete($pkt{$key});
		}
	}	
        return \%pkt;
}

sub get_valid {
        my $itemref = shift;
        my %pkt = %$itemref;
        foreach $key (keys %$itemref) {
                if($pkt{$key}->{"DATA_PTR"} == 0) {
                        delete($pkt{$key});
                }
        }
        return \%pkt;
}


#print the header
print "#";
my $first = 0;
foreach $key (@okeys) {
	if($first == 0) {
		$first = 1;
	} else {
		print ",";
	}
	print $key;
}
print "\n";

# Get the list of packet head pointers and traverse each chain.
#foreach $key (keys(%{get_valid(get_packets(\%item))})) {
foreach $key (keys(%{get_allocated(get_valid(get_packets(\%item)))})) {
	my $pkt_ptr = $key;
	my $pkt_len = 0;
        my $pkt_cnt = 0;
	while(defined($item{$pkt_ptr})) {
		$pkt_cnt ++;
		$pkt_len += $item{$pkt_ptr}->{"USED"};
	
		if($item{$pkt_ptr}->{"USED"} > $item{$pkt_ptr}->{"SIZE"}) {
			printf( "#ERROR: USED GREATER THAN SIZE: 0x%08X\n",$pkt_ptr);
		}

	    	print_record($item{$pkt_ptr});

		if(defined($item{$item{$pkt_ptr}->{"DUP_PTR"}})) {
			print "#DUP ";
			print_record($item{$item{$pkt_ptr}->{"DUP_PTR"}});
		}

  		$pkt_ptr = $item{$pkt_ptr}->{"PKT_PTR"};
	}
	printf("#Packet 0x%08X:Items $pkt_cnt:Bytes $pkt_len\n",$key);
}

