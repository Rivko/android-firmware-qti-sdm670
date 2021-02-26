    .section .qskernel_ranges_vstart,"a",@progbits
    .equ qskernel_ranges_vstart, .
    .section .qskernel_ranges_vend,"a",@progbits
    .equ qskernel_ranges_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_ranges_vstart
    .int qskernel_ranges_vend
    .int 0xB0000000
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_island_vstart,"a",@progbits
    .equ qskernel_island_vstart, .
    .section .qskernel_island_vend,"a",@progbits
    .equ qskernel_island_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_island_vstart
    .int qskernel_island_vend
    .int 0xB0000000
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_zi_island_vstart,"a",@nobits
    .equ qskernel_zi_island_vstart, .
    .section .qskernel_zi_island_vend,"a",@nobits
    .equ qskernel_zi_island_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_zi_island_vstart
    .int qskernel_zi_island_vend
    .int 0x20000001
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_eip_vstart,"a",@progbits
    .equ qskernel_eip_vstart, .
    .section .qskernel_eip_vend,"a",@progbits
    .equ qskernel_eip_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_eip_vstart
    .int qskernel_eip_vend
    .int 0xB0000000
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_exports_vstart,"a",@progbits
    .equ qskernel_exports_vstart, .
    .section .qskernel_exports_vend,"a",@progbits
    .equ qskernel_exports_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_exports_vstart
    .int qskernel_exports_vend
    .int 0xB0000000
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_eip_build_vstart,"a",@progbits
    .equ qskernel_eip_build_vstart, .
    .section .qskernel_eip_build_vend,"a",@progbits
    .equ qskernel_eip_build_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_eip_build_vstart
    .int qskernel_eip_build_vend
    .int 0xB0000000
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_coldboot_vstart,"a",@progbits
    .equ qskernel_coldboot_vstart, .
    .section .qskernel_coldboot_vend,"a",@progbits
    .equ qskernel_coldboot_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_coldboot_vstart
    .int qskernel_coldboot_vend
    .int 0xB0000000
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_zi_coldboot_vstart,"a",@nobits
    .equ qskernel_zi_coldboot_vstart, .
    .section .qskernel_zi_coldboot_vend,"a",@nobits
    .equ qskernel_zi_coldboot_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_zi_coldboot_vstart
    .int qskernel_zi_coldboot_vend
    .int 0x20000001
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_tcm_vstart,"a",@progbits
    .equ qskernel_tcm_vstart, .
    .section .qskernel_tcm_vend,"a",@progbits
    .equ qskernel_tcm_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_tcm_vstart
    .int qskernel_tcm_vend
    .int 0xB0000000
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_zi_tcm_vstart,"a",@nobits
    .equ qskernel_zi_tcm_vstart, .
    .section .qskernel_zi_tcm_vend,"a",@nobits
    .equ qskernel_zi_tcm_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_zi_tcm_vstart
    .int qskernel_zi_tcm_vend
    .int 0x20000001
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_zipages_main_vstart,"a",@nobits
    .equ qskernel_zipages_main_vstart, .
    .section .qskernel_zipages_main_vend,"a",@nobits
    .equ qskernel_zipages_main_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_zipages_main_vstart
    .int qskernel_zipages_main_vend
    .int 0x20000001
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_zipages_island_vstart,"a",@nobits
    .equ qskernel_zipages_island_vstart, .
    .section .qskernel_zipages_island_vend,"a",@nobits
    .equ qskernel_zipages_island_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_zipages_island_vstart
    .int qskernel_zipages_island_vend
    .int 0x20000001
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_zipages_tcm_vstart,"a",@nobits
    .equ qskernel_zipages_tcm_vstart, .
    .section .qskernel_zipages_tcm_vend,"a",@nobits
    .equ qskernel_zipages_tcm_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_zipages_tcm_vstart
    .int qskernel_zipages_tcm_vend
    .int 0x20000001
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_zipages_coldboot_vstart,"a",@nobits
    .equ qskernel_zipages_coldboot_vstart, .
    .section .qskernel_zipages_coldboot_vend,"a",@nobits
    .equ qskernel_zipages_coldboot_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_zipages_coldboot_vstart
    .int qskernel_zipages_coldboot_vend
    .int 0x20000001
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_main_vstart,"a",@progbits
    .equ qskernel_main_vstart, .
    .section .qskernel_main_vend,"a",@progbits
    .equ qskernel_main_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_main_vstart
    .int qskernel_main_vend
    .int 0xB0000000
    .int 0
    .quad 0
    .quad 0
    .section .qskernel_zi_main_vstart,"a",@nobits
    .equ qskernel_zi_main_vstart, .
    .section .qskernel_zi_main_vend,"a",@nobits
    .equ qskernel_zi_main_vend, .
    .section .qskernel_ranges,"a",@progbits
    .align 8
    .int qskernel_zi_main_vstart
    .int qskernel_zi_main_vend
    .int 0x20000001
    .int 0
    .quad 0
    .quad 0
