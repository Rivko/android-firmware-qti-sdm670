echo "usage $0 <address in hex with 0x"
nn=$1
echo "Relocate DSP image to 0x${nn##*x}"
nloc=16#${nn##*x}
main=16#c0000000
(( diff=$main-$nloc ))
cp -f ./obj/qdsp6v4_ReleaseG/vbootimg.pbn ./obj/qdsp6v4_ReleaseG/bootimg.pbn
qdsp6-objcopy --change-start -$diff --change-section-lma .start-$diff --change-section-lma .init-$diff --change-section-lma .lpa_audio_text_page1-$diff --change-section-lma .text-$diff --change-section-lma .fini-$diff --change-section-lma .rodata-$diff --change-section-lma .data-$diff --change-section-lma .eh_frame-$diff --change-section-lma .ctors-$diff --change-section-lma .dtors-$diff --change-section-lma .bss-$diff --change-section-lma .sdata-$diff --change-section-lma .sbss-$diff --change-section-lma .gcc_except_table-$diff ./obj/qdsp6v4_ReleaseG/bootimg.pbn
qdsp6-strip -s ./obj/qdsp6v4_ReleaseG/bootimg.pbn -o ./obj/qdsp6v4_ReleaseG/dsp1.mbn
