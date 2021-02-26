# Usage: DSP_relocate.pl 0x12340000 where 0x12340000 is the physical address of start of aDSP image

$destAddr =hex($ARGV[0]);
$srcAddr =0xc0000000;
$diffAddr = $srcAddr-$destAddr; 
$HEX_src=sprintf("0x%x",$srcAddr);
$HEX_dest=sprintf("0x%x",$destAddr);
$HEX_diff=sprintf("0x%x",$diffAddr);
print "Relocate DSP image to $HEX_dest\n";
system("cp build/ms/M8974AAAAAAAAQ1234_v.elf build/ms/M8974AAAAAAAAQ1234.elf");   
system("qdsp6-objcopy --change-start -$diffAddr --change-section-lma .start-$diffAddr --change-section-lma .init-$HEX_diff --change-section-lma .lpa_audio_text_page1-$HEX_diff --change-section-lma .lpa_audio_aac_page-$HEX_diff --change-section-lma .lpa_audio_mp3_page-$HEX_diff --change-section-lma .text-$HEX_diff --change-section-lma .fini-$HEX_diff --change-section-lma .rodata-$HEX_diff --change-section-lma .data-$HEX_diff --change-section-lma .eh_frame-$HEX_diff --change-section-lma .ctors-$HEX_diff --change-section-lma .dtors-$HEX_diff --change-section-lma .bss-$HEX_diff --change-section-lma .sdata-$HEX_diff --change-section-lma .sbss-$HEX_diff --change-section-lma .gcc_except_table-$diffAddr  build/ms/M8974AAAAAAAAQ1234.elf");
system("qdsp6-strip -s build/ms/M8974AAAAAAAAQ1234.elf -o build/ms/dsp1.mbn");
