#!/usr/bin/env perl
#=================================================================================================
# Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#=================================================================================================

use strict;
use warnings;
use POSIX qw(tzset);
use File::Spec;
use File::Basename;

no strict 'subs';

sub readChange($)
{
    my ($sha1) = @_;

    my $commitId = '';
    my $change   = `git log $sha1 -n1`;

    # 'check' for git working
    if ($change=~/Author/)
    {
        if ($change=~/Change-Id: (\w+)/)
        {
            $commitId = $1;
        }
        elsif ($change=~/Merge: \w+ (\w+)/)
        {
            $change = `git log $1 -n 1`;

            if ($change=~/Change-Id: (\w+)/)
            {
                $commitId = $1;
            }
        }
    }

    return ($commitId);
}

# Means of finding the sha1 used by git without needing to run git, which isnt always in the windows environment
sub findGitDirectory()
{
    my @parts = split(/[\\\/]/, File::Spec->rel2abs(dirname(__FILE__)));
    my $currentDirectory;
    # Iterate backwards through directories
    while ($currentDirectory = pop @parts)
    {
        my $path = join('/', @parts)."/$currentDirectory/.git";

        # if .git exists
        if (-d $path)
        {
            chdir "$path";
            # Open HEAD file which tracks current head
            if (open(HEAD, "<$path/HEAD"))
            {
                my $ref = readline(HEAD);
                close HEAD;

                my $sha1;

                # Extract ref, which is essentially branch or disconnected head state
                if ($ref=~/ref: (.*)/)
                {
                    my $ref = "$path/$1";
                    # Read ref, which has only the sha1 in it
                    if (open(REF, "<$ref"))
                    {
                        $sha1 = readline(REF);
                        chomp $sha1;
                        close REF;

                        my ($commitId) = readChange($sha1);

                        return ($sha1, $path, $commitId);
                    }
                    else
                    {
                        print "Failed to open reference $ref\n";
                        last;
                    }
                }
                elsif ($ref=~/^\w+$/)
                {
                    $sha1 = $ref;
                    chomp $sha1;

                    my ($commitId) = readChange($sha1);

                    return ($sha1, $path, $commitId);
                }
            }
            else
            {
                print "Failed to open $path/HEAD\n";
                last;
            }
        }
    }
    return ('','','');
}

my $headerFile = File::Spec->rel2abs($ARGV[0]); # c:\git\blah\boo\foo.h
my $headerFileBase = basename($headerFile); # Just 'foo.h'

# make the include guards
my $headerFileGuard = uc($headerFileBase);
$headerFileGuard =~ s/[\. ]/_/;

my ($sha1,$path,$commitId) = findGitDirectory();

$path=~s/\.git$//g;

my $oldsha1;
my $oldcommitId;

if (-e $headerFile)
{
    if (open (HEADER, "<$headerFile"))
    {
        my $line;
        while ($line = <HEADER>)
        {
            if ($line=~/CAMX_SHA1\s+"(\w+)"/)
            {
                $oldsha1 = $1;
            }
            if ($line=~/CAMX_COMMITID\s+"(\w+)"/)
            {
                $oldcommitId = $1;
            }
        }
        close HEADER;
    }
}

if (!$oldsha1 || !$oldcommitId || ($sha1 ne $oldsha1) || ($commitId ne $oldcommitId))
{
    open (HEADER, ">$headerFile") or die "Could not open $headerFile : $!";
    $ENV{TZ} = 'Asia/Shanghai';
    tzset;
    my ($sec, $min, $hour, $mday, $mon, $year) = localtime();
    $mon += 1;
    $year += 1900;
    my $datetime = "$mon/$mday/$year $hour:$min:$sec";
    my $hostname = `hostname -A `;
    chomp($hostname);
    my $ipt = `/sbin/ip addr |grep "inet " | awk -F"\ " '{print \$2}' |tail -n1`;
    print $ipt;
    chomp($ipt);

    print HEADER <<"HERE";
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \@file  $headerFileBase
/// \@brief Compile-time generated version info
///         This file is generated automatically by version.pl/.exe. Do not edit.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef $headerFileGuard
#define $headerFileGuard

#define CAMTEST_SHA1      "$sha1"
#define CAMTEST_COMMITID  "$commitId"
#define CAMTEST_BUILD_TS  "$datetime"
#define CAMTESTHOSTNAME   "$hostname"
#define CAMBUILD_IP       "$ipt"

#endif /* $headerFileGuard */
HERE
    close HEADER;
}
