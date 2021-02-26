file obj/qdsp6v3_ReleaseG/bootimg.pbn
set targetargs --rtos obj/qdsp6v3_ReleaseG/osam.cfg --cosim_file obj/qdsp6v3_ReleaseG/q6ss.cfg --symfile obj/qdsp6v3_ReleaseG/bootimg.pbn  --statsfile /prj/vocoder/appdsp5/users/kevwang/stats.txt
load obj/qdsp6v3_ReleaseG/bootimg.pbn 
b qurt_root_setup
b QURTK_tlb_crash
commands
puts "TLB CRASH"
puts [info reg elr]
end
b main

