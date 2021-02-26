/**
 * @file boot_gpt_partition_id.c
 * @brief
 * Source file contains the GUID for sbl2,sbl3,tz,rpm and appsbl
 *
 */

/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/22/17   rohik   Added GUID for log_dump partition 
06/22/17   ds      coldplug changes
05/30/17   vk      Add MISC image GUIDs
10/14/16   kpa     Added GUID for XBLConfig image partition
07/19/16   kpa     Added GUID for SHRM and AOP image partitions
10/15/15   wg      Added GUID for ABL.fv and ABL.fv backup partitions
09/15/15   as      Added GUID for Boot Shared Variables
07/10/15   kpa     Added GUID for sti_partition_id
06/05/15   as      Added GUID for BERT partition
05/19/15   as      Added GUID for Recovery RPM partition
05/05/15   kpa     Added GUID for APDP partition
03/02/15   kpa     Added GUID for QSEE Dev config image partition.
01/23/15   ck      Corrected QSEE recovery partition GUID
10/30/14   ck      Added recovery partitions and GUID for ACPI partition
10/14/14   kpa     Added GUID for PMIC partition
03/21/14   ck      Added GUID for SEC partition
10/17/13   ck      Split TZ into QSEE and QHEE and created GUID's for them.
03/19/13   dh      Added GUID for ram dump parition
12/05/12   jz      Added GUID for CDT partition
07/20/12   dh      Add GUID for ddr training parameter partition
03/22/12   dh      Add GUID for wdog debug image
04/05/11   plc     Add GUID info for SBL1 
03/24/11   dh      Initial creation 

============================================================================
Copyright 2011-2017 by Qualcomm Technologies Incorporated.  All Rights Reserved.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================
                             INCLUDE FILES
===========================================================================*/
#include "boot_comdef.h"
#include "coldplug_api.h"

/*define GUID for SBL1*/
/*This is the GUID that PBL expects to identify and load the first SBL*/
struct coldplug_guid sbl1_partition_id = 
      /*{DEA0BA2C-CBDD-4805-B4F9-F428251C3E98}*/
      { 0xDEA0BA2C, 0xCBDD, 0x4805, { 0xB4, 0xF9, 0xF4, 0x28, 0x25, 0x1C, 0x3E, 0x98 } };

/*define GUID for SBL2*/
struct coldplug_guid sbl2_partition_id = 
      /*{8C6B52AD-8A9E-4398-AD09-AE916E53AE2D}*/
      { 0x8C6B52AD, 0x8A9E, 0x4398, { 0xAD, 0x09, 0xAE, 0x91, 0x6E, 0x53, 0xAE, 0x2D } };

/*define GUID for SBL3*/       
struct coldplug_guid sbl3_partition_id = 
      /*{05E044DF-92F1-4325-B69E-374A82E97D6E}*/
      { 0x05E044DF, 0x92F1, 0x4325, { 0xB6, 0x9E, 0x37, 0x4A, 0x82, 0xE9, 0x7D, 0x6E } };
      
/*define GUID for APPSBL*/      
struct coldplug_guid appsbl_partition_id = 
      /*{400FFDCD-22E0-47E7-9A23-F16ED9382388}*/
      { 0x400FFDCD, 0x22E0, 0x47E7, { 0x9A, 0x23, 0xF1, 0x6E, 0xD9, 0x38, 0x23, 0x88 } };
      
/*define GUID for QSEE*/
struct coldplug_guid qsee_partition_id = 
      /*{A053AA7F-40B8-4B1C-BA08-2F68AC71A4F4}*/
      { 0xA053AA7F, 0x40B8, 0x4B1C, { 0xBA, 0x08, 0x2F, 0x68, 0xAC, 0x71, 0xA4, 0xF4 } };

/*define GUID for QHEE*/
struct coldplug_guid qhee_partition_id = 
      /*{E1A6A689-0C8D-4CC6-B4E8-55A4320FBD8A}*/
      { 0xE1A6A689, 0x0C8D, 0x4CC6, { 0xB4, 0xE8, 0x55, 0xA4, 0x32, 0x0F, 0xBD, 0x8A } };
 
/*define GUID for RPM*/      
struct coldplug_guid rpm_partition_id = 
      /*{098DF793-D712-413D-9D4E-89D711772228}*/
      { 0x098DF793, 0xD712, 0x413D, { 0x9D, 0x4E, 0x89, 0xD7, 0x11, 0x77, 0x22, 0x28 } };
      
/*define GUID for wdog debug image*/      
struct coldplug_guid wdt_partition_id = 
      /*{D4E0D938-B7FA-48C1-9D21-BC5ED5C4B203}*/
      { 0xD4E0D938, 0xB7FA, 0x48C1, { 0x9D, 0x21, 0xBC, 0x5E, 0xD5, 0xC4, 0xB2, 0x03 } };
      
/*define GUID for DDR params partiton*/      
struct coldplug_guid ddr_params_partition_id =
      /*{20A0C19C-286A-42FA-9CE7-F64C3226A794}*/
      { 0x20A0C19C, 0x286A, 0x42FA, { 0x9C, 0xE7, 0xF6, 0x4C, 0x32, 0x26, 0xA7, 0x94 } };
      
/*define GUID for CDT partition*/
struct coldplug_guid cdt_partition_id =
      /*{a19f205f-ccd8-4b6d-8f1e-2d9bc24cffb1}*/
      { 0xA19F205F, 0xCCD8, 0x4B6D, { 0x8F, 0x1E, 0x2D, 0x9B, 0xC2, 0x4C, 0xFF, 0xB1 } };

/*define GUID for Ram Dump partition*/
struct coldplug_guid ram_dump_partition_id =
      /*{66C9B323-F7FC-48B6-BF96-6F32E335A428}*/
      { 0x66C9B323, 0xF7FC, 0x48B6, { 0xBF, 0x96, 0x6F, 0x32, 0xE3, 0x35, 0xA4, 0x28 } };

/*define GUID for Log Dump partition*/	  
struct coldplug_guid log_dump_partition_id =
	      /*{5AF80809-AABB-4943-9168-CDFC38742598 } */  
      { 0x5AF80809, 0xAABB, 0x4943, { 0x91, 0x68, 0xCD, 0xFC, 0x38, 0x74, 0x25, 0x98 } }; 	  

/*define GUID for SEC partition*/
struct coldplug_guid sec_partition_id =
      /*{303E6AC3-AF15-4C54-9E9B-D9A8FBECF401}*/
      { 0x303E6AC3, 0xAF15, 0x4C54, { 0x9E, 0x9B, 0xD9, 0xA8, 0xFB, 0xEC, 0xF4, 0x01 } };

/*define GUID for PMIC config data partition*/
struct coldplug_guid pmic_partition_id =
      /*{C00EEF24-7709-43D6-9799-DD2B411E7A3C}*/
      { 0xC00EEF24, 0x7709, 0x43D6, { 0x97, 0x99, 0xDD, 0x2B, 0x41, 0x1E, 0x7A, 0x3C } };

/*define GUID for TZ enumeration image partition*/
struct coldplug_guid qsee_devcfg_image_partition_id =
      /*{F65D4B16-343D-4E25-AAFC-BE99B6556A6D}*/
      { 0xF65D4B16, 0x343D, 0x4E25, { 0xAA, 0xFC, 0xBE, 0x99, 0xB6, 0x55, 0x6A, 0x6D } };            
      
/*define GUID for APDP config data partition*/
struct coldplug_guid apdp_partition_id =
      /*{E6E98DA2-E22A-4D12-AB33-169E7DEAA507}*/
      { 0xE6E98DA2, 0xE22A, 0x4D12, { 0xAB, 0x33, 0x16, 0x9E, 0x7D, 0xEA, 0xA5, 0x07 } };
      
/*define GUID for ACPI table partition*/
struct coldplug_guid acpi_partition_id =
      /*{2A21D307-A03B-484C-A151-09A0D97716A8}*/
      { 0x2A21D307, 0xA03B, 0x484C, { 0xA1, 0x51, 0x09, 0xA0, 0xD9, 0x77, 0x16, 0xA8 } };

/*define GUID for BERT partition*/
struct coldplug_guid bert_partition_id =
      /*{0E444367-F7DB-46B5-9BE7-403996654F5C}*/
      { 0x0E444367, 0xF7DB, 0x46B5, { 0x9B, 0xE7, 0x40, 0x39, 0x96, 0x65, 0x4F, 0x5C } };

/*define GUID for STI image partition*/
struct coldplug_guid sti_partition_id =
      /*{AA9A5C4C-4F1F-7D3A-014A-22BD33BF7191}*/
      { 0xAA9A5C4C, 0x4F1F, 0x7D3A, { 0x01, 0x4A, 0x22, 0xBD, 0x33, 0xBF, 0x71, 0x91 } };

/*define GUID for boot shared variables partition*/
struct coldplug_guid boot_shared_variables_partition_id =
      /*{3E8954C9-F738-4ED6-8AD1-229E60E9B6C8}*/
      { 0x3E8954C9, 0xF738, 0x4ED6, { 0x8A, 0xD1, 0x22, 0x9E, 0x60, 0xE9, 0xB6, 0xC8 } };

/*define GUID for ABL image partition*/
struct coldplug_guid abl_partition_id =
      /*{BD6928A1-4CE0-A038-4F3A-1495E3EDDFFB}*/
      { 0xBD6928A1, 0x4CE0, 0xA038, { 0x4F, 0x3A, 0x14, 0x95, 0xE3, 0xED, 0xDF, 0xFB } };

/*define GUID for SHRM image partition*/
struct coldplug_guid shrm_partition_id =
      /*{7B8C0463-4A7A-030E-3C06-2686B6FE5299}*/
      { 0x7B8C0463, 0x4A7A, 0x030E, { 0x3C, 0x06, 0x26, 0x86, 0xB6, 0xFE, 0x52, 0x99 } };

/*define GUID for AOP image partition*/
struct coldplug_guid aop_partition_id =
      /*{D69E90A5-4CAB-0071-F6DF-AB977F141A7F}*/
      { 0xD69E90A5, 0x4CAB, 0x0071, { 0xF6, 0xDF, 0xAB, 0x97, 0x7F, 0x14, 0x1A, 0x7F } };      
      
/*define GUID for XBLConfig image partition*/
struct coldplug_guid XBLConfig_partition_id =
      /*{5A325AE4-4276-B66D-0ADD-3494DF27706A}*/
      { 0x5A325AE4, 0x4276, 0xB66D, { 0x0A, 0xDD, 0x34, 0x94, 0xDF, 0x27, 0x70, 0x6A } };     
     
/*define GUID for multi image (MISC OEM) partition*/
struct coldplug_guid multi_image_partition_id = 
      /*{E126A436-757E-42D0-8D19-0F362F7A62B8}*/ 
      { 0xE126A436, 0x757E, 0x42D0, { 0x8D, 0x19, 0x0F, 0x36, 0x2F, 0x7A, 0x62, 0xB8 } };

/*define GUID for multi image (MISC QTI) partition*/
struct coldplug_guid multi_image_qti_partition_id = 
      /*{846C6F05-EB46-4C0A-A1A3-3648EF3F9D0E}*/ 
      { 0x846C6F05, 0xEB46, 0x4C0A, { 0xA1, 0xA3, 0x36, 0x48, 0xEF, 0x3F, 0x9D, 0x0E } };     

/*define GUID for logfs image partition*/
struct coldplug_guid logfs_partition_id =
      /*{BC0330EB-3410-4951-A617-03898DBE3372}*/
      { 0xBC0330EB, 0x3410, 0x4951, { 0xA6, 0x17, 0x03, 0x89, 0x8D, 0xBE, 0x33, 0x72 } };
       
/* Recovery Partitions */
/* define GUID for SBL1 RECOVERY */
struct coldplug_guid sbl1_recovery_partition_id = 
  /*{7A3DF1A3-A31A-454D-BD78-DF259ED486BE}*/
  { 0x7A3DF1A3, 0xA31A, 0x454D, { 0xBD, 0x78, 0xDF, 0x25, 0x9E, 0xD4, 0x86, 0xBE } };

/* define GUID for QSEE RECOVERY */
struct coldplug_guid qsee_recovery_partition_id = 
  /*{C832EA16-8B0D-4398-A67B-EBB30EF98E7E}*/
  { 0xC832EA16, 0x8B0D, 0x4398, { 0xA6, 0x7B, 0xEB, 0xB3, 0x0E, 0xF9, 0x8E, 0x7E } };

/* define GUID for APPSBL RECOVERY */      
struct coldplug_guid appsbl_recovery_partition_id = 
  /*{9F234B5B-0EFB-4313-8E4C-0AF1F605536B}*/
  { 0x9F234B5B, 0x0EFB, 0x4313, { 0x8E, 0x4C, 0x0A, 0xF1, 0xF6, 0x05, 0x53, 0x6B } };

/*define GUID for PMIC config data RECOVERY partition*/
struct coldplug_guid pmic_recovery_partition_id =
      /*{D9BD7CD9-B1BA-4F3B-A6CE-0E348A1116E9}*/
      { 0xD9BD7CD9, 0xB1BA, 0x4F3B, { 0xA6, 0xCE, 0x0E, 0x34, 0x8A, 0x11, 0x16, 0xE9 } };

/*define GUID for RPM RECOVERY partition*/
struct coldplug_guid rpm_recovery_partition_id =
      /*{B7804414-8E65-4A1D-93FD-9D9BF5621306}*/
      { 0xB7804414, 0x8E65, 0x4A1D, { 0x93, 0xFD, 0x9D, 0x9B, 0xF5, 0x62, 0x13, 0x06 } };
	  
	  /*define GUID for ABL RECOVERY partition*/
struct coldplug_guid abl_recovery_partition_id =
      /*{DC6278E6-0799-4616-BD72-28D60D15921C}*/
      { 0xDC6278E6, 0x0799, 0x4616, { 0xBD, 0x72, 0x28, 0xD6, 0x0D, 0x15, 0x92, 0x1C } };
      
/*define GUID for SHRM image partition*/
struct coldplug_guid shrm_recovery_partition_id =
      /*{39FD6C00-49EB-6BD1-6899-2FB849DD4F75}*/
      { 0x39FD6C00, 0x49EB, 0x6BD1, { 0x68, 0x99, 0x2F, 0xB8, 0x49, 0xDD, 0x4F, 0x75 } };
      
/*define GUID for AOP image partition*/
struct coldplug_guid aop_recovery_partition_id =
      /*{B8B27C4C-4B5B-8AB2-502F-A792B590A896}*/
      { 0xB8B27C4C, 0x4B5B, 0x8AB2, { 0x50, 0x2F, 0xA7, 0x92, 0xB5, 0x90, 0xA8, 0x96 } };        


      
