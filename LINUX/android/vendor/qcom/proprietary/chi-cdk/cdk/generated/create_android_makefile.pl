# Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

use strict;
use warnings;

open (my $makefile, '>', "build/android/Android.mk" ) or die $!;

my $include_files = "LOCAL_INC_FILES :=      \\\n";
my $source_files  = "LOCAL_SRC_FILES :=      \\\n";

my $chromatix_include_files = "LOCAL_INC_FILES +=      \\\n";
my $chromatix_source_files  = "LOCAL_SRC_FILES +=      \\\n";

my @dir_list = ("g_parser", "g_sensor", "g_topology", "g_facedetection");
my @chromatix_dir_list = ("g_chromatix");

foreach my $current_dir (@dir_list) {
    opendir (DIR, $current_dir) || die "Can't open autogen code directory $current_dir: $!";
    my @current_dir = readdir(DIR);
    close DIR;
    foreach(@current_dir) {
       if (-f $current_dir . "/" . $_ ) {
          my $this_file = $_;
          if ($this_file =~ /\.h$/i) {
             $include_files = $include_files."    ".$current_dir."/".$this_file."    \\\n";
          }
          if ($this_file =~ /\.cpp$/i) {
             $source_files = $source_files."    ".$current_dir."/".$this_file."    \\\n";
          }
       }
    }
}

# Remove the last '\'
$include_files =~ s/    \\$//;
$source_files  =~ s/    \\$//;

foreach my $current_dir (@chromatix_dir_list) {
    opendir (DIR, $current_dir) || die "Can't open autogen code directory $current_dir: $!";
    my @current_dir = readdir(DIR);
    close DIR;
    foreach(@current_dir) {
       if (-f $current_dir . "/" . $_ ) {
          my $this_file = $_;
          if ($this_file =~ /\.h$/i) {
             $chromatix_include_files = $chromatix_include_files."    ".$current_dir."/".$this_file."    \\\n";
          }
          if ($this_file =~ /\.cpp$/i) {
             $chromatix_source_files = $chromatix_source_files."    ".$current_dir."/".$this_file."    \\\n";
          }
       }
    }
}

# Remove the last '\'
$chromatix_include_files =~ s/    \\$//;
$chromatix_source_files  =~ s/    \\$//;

my $makefile_header =   "ifeq (\$(CAMX_PATH),)\n" .
                        "LOCAL_PATH := \$(abspath \$(call my-dir)/../..)\n" .
                        "CAMX_PATH := \$(abspath \$(LOCAL_PATH)/../..)\n" .
                        "else\n" .
                        "LOCAL_PATH := \$(CAMX_CDK_PATH)/generated\n" .
                        "endif\n" .
                        "\n" .
                        "include \$(CLEAR_VARS)\n" .
                        "\n" .
                        "# Get definitions common to the CAMX project here\n" .
                        "include $(CAMX_VENDOR_PATH)/common.mk\n" .
                        "\n";

my $makefile_footer =   "# Put here any libraries that should be linked by CAMX projects\n" .
                        "LOCAL_C_LIBS := \$(CAMX_C_LIBS)\n" .
                        "\n" .
                        "# Paths to included headers\n" .
                        "LOCAL_C_INCLUDES := \$(CAMX_C_INCLUDES)\n" .
                        "LOCAL_C_INCLUDES += \$(CAMX_CDK_PATH)/generated/g_parser\n" .
                        "LOCAL_C_INCLUDES += \$(CAMX_CDK_PATH)/generated/g_topology\n" .
                        "\n" .
                        "# Compiler flags\n" .
                        "LOCAL_CFLAGS := \$(CAMX_CFLAGS)  \\\n" .
                        "    -Wno-uninitialized          \\\n" .
                        "    -Wno-unused-parameter       \\\n" .
                        "    -Wno-unused-variable\n" .
                        "LOCAL_CPPFLAGS := \$(CAMX_CPPFLAGS)\n" .
                        "\n" .
                        "# Binary name\n" .
                        "LOCAL_MODULE := libcamxgenerated\n" .
                        "\n" .
                        "include \$(CAMX_BUILD_STATIC_LIBRARY)\n";

print $makefile $makefile_header;
print $makefile $include_files;
print $makefile "\n";
print $makefile "ifneq (\$(IQSETTING),OEM1)\n";
print $makefile $chromatix_include_files;
print $makefile "endif\n";
print $makefile "\n";
print $makefile $source_files;
print $makefile "\n";
print $makefile "ifneq (\$(IQSETTING),OEM1)\n";
print $makefile $chromatix_source_files;
print $makefile "endif\n";
print $makefile "\n";
print $makefile $makefile_footer;
