# 1 "/local/mnt/workspace/CRMBuilds/AOP.HO.1.1.C1-00025-SDM710AAAAANAZO-1_20200113_014802/b/aop_proc/core/kernel/rex/target/670/aop.scl"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 324 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/AOP.HO.1.1.C1-00025-SDM710AAAAANAZO-1_20200113_014802/b/aop_proc/core/kernel/rex/target/670/aop.scl" 2
# 46 "/local/mnt/workspace/CRMBuilds/AOP.HO.1.1.C1-00025-SDM710AAAAANAZO-1_20200113_014802/b/aop_proc/core/kernel/rex/target/670/aop.scl"
ENTRY(Reset_Handler)

PHDRS
{
  AOPSS_CODE PT_LOAD;
  AOPSS_DATA PT_LOAD;
  AOPSS_RINIT PT_LOAD;
  AOPSS_PMIC PT_LOAD;
}

SECTIONS
{






  CODE_RAM 0x0 :
  {
    __aop_coderam_base = .;

    KEEP(*\startup.o(.text))
    *(InRoot$$Sections .text .text*)
    *(clock_data_sec)
  } : AOPSS_CODE

  PM_CRAM_RECLAIM_POOL:
  {
    __reclaim_cram_pool_base = .;
    *(pm_cram_reclaim_pool)
    __reclaim_cram_end = .;
  } : AOPSS_CODE


  ASSERT(. <= ((0x0 + 0x18000) - (0x600 + 0x458) - 0x2000), "AOPSS_CODE oversized");

  ARM_LIB_HEAP ((0x0 + 0x18000) - (0x600 + 0x458) - 0x2000) (NOLOAD) :
  {
    __aop_heap_base = .;

    . = . + 0x2000;

    __aop_heap_limit = .;
  }


  AOP_ULOG (0x0 + 0x18000) - (0x600 + 0x458) (NOLOAD):
  {
 __aop_log_base = .;
 . = . + 0x600;
 __aop_log_limit = .;
  } : AOPSS_CODE

  DDR_ULOG (0x0 + 0x18000) - (0x600 + 0x458) + 0x600 (NOLOAD):
  {
 __ddr_log_base = .;
 . = . + 0x458;
 __ddr_log_limit = .;
  } : AOPSS_CODE


  ASSERT(. <= (0x0 + 0x18000), "AOPSS_CODE oversized");







  DATA_RAM 0xE0000 :
  {
    KEEP(*\oem_uuid.o(.data*))
    KEEP(*\qc_version.o(.data*))
    KEEP(*\oem_version.o(.data*))
   __aop_image_id_end = .;
  } : AOPSS_DATA

  AOPSS_DATA :
  {
    KEEP(*\datetime.o (.data.gBuild*))
    *(.data .data*)
    *(.bss .bss*)
    *(.rodata .rodata*)
  } : AOPSS_DATA

  PM_DRAM_RECLAIM_POOL:
  {
    __reclaim_dram_pool_base = .;
    *(pm_dram_reclaim_pool)
    __reclaim_dram_end = .;
  } : AOPSS_DATA

  DRAM_SLEEP_CODE_POOL:
  {
    *(sleep_code_sec)
  } : AOPSS_DATA

  TASK_STACKS (NOLOAD) :
  {
    *(task_stacks)
  }
  __aop_dram_size_left = ((0xE0000 + 0x8000) - 0x300 - 32) - .;
  PRINT(. = __aop_dram_size_left);
  ASSERT(. < ((0xE0000 + 0x8000) - 0x300 - 32), "AOPSS_DATA oversized");

  ARM_LIB_STACK ((0xE0000 + 0x8000) - 0x300 - 32) (NOLOAD) :
  {
    . = . + 32;

    __aop_stack_base = .;

    . = . + 0x300;

    __aop_stack_limit = .;
  }




  RINIT_DATA (0x85FC0000) :
  {
    *(rinit_data_sec)
 *(pm_ddr_reclaim_pool)
    __rinit_data_end = .;
  } : AOPSS_RINIT

  ASSERT (. < (0x85FD0000), "RINIT DATA oversized");
# 184 "/local/mnt/workspace/CRMBuilds/AOP.HO.1.1.C1-00025-SDM710AAAAANAZO-1_20200113_014802/b/aop_proc/core/kernel/rex/target/670/aop.scl"
  ASSERT (. < (0x85FE0000), "PMIC DATA oversized");

  /DISCARD/ :
  {
    * (.ARM.extab*)
    * (.ARM.exid*)
    * (.got)
  }

}
