#pragma once
/*
*
* Copyright @ 2014-2018 InvenSense Inc.  All rights reserved.
*
* INVENSENSE, INC. ("LICENSOR") SOFTWARE LICENSE AGREEMENT ("Agreement")
* BY DOWNLOADING, INSTALLING, COPYING OR OTHERWISE USING THE ENCLOSED SOFTWARE AND OTHER AVAILABLE MATERIALS ("LICENSED
* MATERIALS"), YOU ("LICENSEE") AGREE TO BE BOUND BY THE FOLLOWING TERMS AND CONDITIONS OF THIS AGREEMENT.  IF LICENSEE DOES NOT
* AGREE TO THE TERMS AND CONDITION OF THIS AGREEMENT, THEN DO NOT DOWNLOAD, INSTALL, COPY OR OTHERWISE USE THE ENCLOSED SOFTWARE
* AND MATERIALS.
*
* The Licensed Materials may include open source and proprietary software in both source code ("Source Code") and object code
* ("Object Code") versions, documentation and other materials.  Except as expressly indicated herein, all terms and conditions of
* this Agreement apply to all of the Licensed Materials.  In the event that any file contained within the Licensed Materials
* incorporates a different software license, such other license shall apply to such file.
* Except as expressly set forth herein, LICENSOR owns all of the Licensed Materials and makes them available to Licensee only
* under the terms and conditions set forth in this Agreement.
*
* 1. License:  Subject to the terms of this Agreement, LICENSOR hereby grants to Licensee a royalty-free, non-exclusive license to
* possess and to use the Licensed Materials in a secure location in accordance with the applicable terms herein.  Licensee may
* make back-up copies and install and use multiple copies of the Licensed Materials on a shared computer or concurrently on
* different computers, solely for Licensee's use within Licensee's Enterprise. "Enterprise" shall mean individual use by Licensee
* or any legal entity (such as a corporation or university) and the subsidiaries it owns by more than 50 percent.  The following
* terms apply to the specified type of Licensed Material:
*
* 1.1 Source Code:  Licensee shall have the right to use, copy, modify and prepare and have prepared derivative works of the
* Source Code for internal development and testing purposes only.  Licensee shall own any modification it makes directly to the
* Source Code that optimizes quality or performance with Licensee's product ("Modification(s)") subject to LICENSOR's ownership of
* the underlying Source Code and all intellectual property rights therein.
*
* 1.2 Object Code:  Licensee may distribute the Object Code (in object code form only) and compiled Source Code (in object code
* form only) with Modifications solely for execution or operation with applicable LICENSOR products for which Source Code and
* Object Code was designed and as incorporated in Licensee's software products. Licensee agrees not to disassemble, decompile or
* reverse engineer the Object Code.
*
* 2. Notices:  Licensee agrees to comply with and reproduce any and all copyright and other attribution notices/instructions of
* LICENSOR as included in the Licensed Materials in connection with Licensee's distribution rights pursuant to the Agreement.
*
* 3. Subcontractors:  Licensee may engage subcontractors to exercise Licensee's rights hereunder. Licensee is responsible to
* ensure that Licensee subcontractors comply with the terms and conditions of this Agreement.  Any act by a subcontractor that
* would be a breach of this Agreement by Licensee if Licensee performed the act will be deemed a breach of this Agreement by
* Licensee.
*
* 4. License Grant Back: Licensee hereby grants to LICENSOR and its Affiliates an exclusive, worldwide, irrevocable, perpetual,
* sublicenseable (through multiple tiers of sublicensees), royalty-free and fully paid-up right and license to the Modification(s)
* (in object code form) created by or on behalf of Licensee so that LICENSOR may copy, modify, create derivatives works thereof,
* to use, have used, import, make, have made, sell, offer to sell, sublicense (through multiple tiers of sublicensees), distribute
* (through multiple tiers of distributors) such derivative work(s) on a stand-alone basis or as incorporated into the Licensed
* Materials or other related technologies.  For the sake of clarity, LICENSOR is not prohibited or otherwise restricted from
* independently developing new features or functionality with respect to the Licensed Materials.
*
* 5. No Other License: No rights or licenses with respect to any proprietary information or patent, copyright, trade secret or
* other intellectual property right owned or controlled by LICENSOR are granted by LICENSOR to Licensee under this Agreement,
* expressly or by implication, except as expressly provided in this Agreement.
* 6. Intellectual Property Ownership: Except as expressly licensed to Licensee under this Agreement, LICENSOR reserves all right,
* title and interest, including but not limited to all intellectual property rights, in and to the Licensed Materials and any
* derivative work(s) made thereto.
*
* 7. Term of Agreement:  This Agreement is effective until (i) automatically terminated if Licensee fails to comply with any of
* the terms and conditions of this Agreement; or (ii) terminated by LICENSOR.  LICENSOR may terminate this Agreement (and with it,
* all of Licensee's right to the Licensed Materials) immediately upon written notice (which may include email) to Licensee, with
* or without cause; provided however, that sublicenses of Derivatives, to the extent validly granted to its customers prior to
* termination of this Agreement, shall survive such termination.
*
* 8. Confidential Information. "Confidential Information" means (i) the Licensed Materials; (ii) the structure, sequence and
* organization of the Licensed Materials and the concepts, methods of operations and ideas disclosed therein; and (iii) any trade
* secrets of LICENSOR or its affiliates or its or their suppliers relating to the Licensed Materials. Licensee will not disclose
* any Confidential Information to any third party (except subcontractors, as permitted herein) or use Confidential Information
* except as expressly permitted in this Agreement.  Licensee agrees to take all reasonable measures to protect Confidential
* Information and prevent its unauthorized disclosure, including measures at least as stringent as those measures Licensee takes
* to protect Licensee's own most sensitive confidential information.  Licensee agrees to restrict access to Confidential
* Information to Licensee employees and subcontractors who are under obligations to protect Confidential Information in accordance
* with this Agreement and who have a "need to know" the Confidential Information to exercise Licensee license rights in this
* Agreement.  All Confidential Information, and any documents and other tangible objects containing or representing Confidential
* Information, and all copies of Confidential Information, are and will remain the exclusive property of LICENSOR.
*
* 9. Defensive Suspension: If Licensee commences or participates in any legal proceeding against LICENSOR, then LICENSOR may, in
* its sole discretion, suspend or terminate all license grants and any other rights provided under this Agreement during the
* pendency of such legal proceedings.
*
* 10. No Support:  LICENSOR has no obligation to support or to continue providing or updating any of the Licensed Materials.
*
* 11. No Warranty:  THE LICENSED MATERIALS PROVIDED BY LICENSOR TO LICENSEE HEREUNDER ARE PROVIDED "AS IS."  LICENSOR DISCLAIMS
* ALL WARRANTIES, EXPRESS, IMPLIED OR STATUTORY, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*
* 12. Limitation of Liability: LICENSOR SHALL NOT BE LIABLE TO LICENSEE, LICENSEE'S CUSTOMERS, OR ANY OTHER PERSON OR ENTITY
* CLAIMING THROUGH OR UNDER LICENSEE FOR ANY LOSS OF PROFITS, INCOME, SAVINGS, OR ANY OTHER CONSEQUENTIAL, INCIDENTAL, SPECIAL,
* PUNITIVE, DIRECT OR INDIRECT DAMAGES (WHETHER IN AN ACTION IN CONTRACT, TORT OR BASED ON A WARRANTY), EVEN IF LICENSOR HAS BEEN
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF THE ESSENTIAL PURPOSE
* OF ANY LIMITED REMEDY.  IN NO EVENT SHALL LICENSOR'S AGGREGATE LIABILITY TO LICENSEE OR ANY OTHER PERSON OR ENTITY CLAIMING
* THROUGH OR UNDER LICENSEE EXCEED THE AMOUNT OF MONEY ACTUALLY PAID BY LICENSEE TO LICENSOR FOR THE LICENSED MATERIALS.
*
* 13. Applicable Law and Jurisdiction: This Agreement shall be deemed to have been made in, and shall be construed pursuant to,
* the laws of the State of California. The state and/or federal courts residing in Santa Clara County, California shall have
* exclusive jurisdiction over any dispute or claim arising out of this Agreement. The United Nations Convention on Contracts for
* the International Sale of Goods is specifically disclaimed.
*
* 14. Feedback: Licensee may, but is not obligated to, provide to LICENSOR any suggestions, comments and feedback regarding the
* Licensed Materials that are delivered by LICENSOR to Licensee under this Agreement (collectively, "Licensee Feedback").
* LICENSOR may use and include any Licensee Feedback that Licensee voluntarily provides to improve the Licensed Materials or other
* related LICENSOR technologies.  Accordingly, if Licensee provides Licensee Feedback, Licensee grants LICENSOR and its licensees
* a perpetual, irrevocable, worldwide, royalty-free, fully paid-up license grant to freely use, have used, sell, modify,
* reproduce, transmit, license, sublicense (through multiple tiers of sublicensees), distribute (through multiple tiers of
* distributors), and otherwise commercialize the Licensee Feedback in the Licensed Materials or other related technologies.
*
* 15. RESTRICTED RIGHTS NOTICE: Licensed Materials has been developed entirely at private expense and is commercial computer
* software provided with RESTRICTED RIGHTS. Use, duplication or disclosure by the U.S. Government or a U.S. Government
* subcontractor is subject to the restrictions set forth in the license agreement under which Licensed Materials was obtained
* pursuant to DFARS 227.7202-3(a) or as set forth in subparagraphs (c)(1) and (2) of the Commercial Computer Software - Restricted
* Rights clause at FAR 52.227-19, as applicable.
*
* 16. Miscellaneous: If any provision of this Agreement is inconsistent with, or cannot be fully enforced under, the law, such
* provision will be construed as limited to the extent necessary to be consistent with and fully enforceable under the law. This
* Agreement is the final, complete and exclusive agreement between the parties relating to the subject matter hereof, and
* supersedes all prior or contemporaneous understandings and agreements relating to such subject matter, whether oral or written.
* This Agreement is solely between LICENSOR and Licensee.  There are no third party beneficiaries, express or implied, to this
* Agreement. This Agreement may only be modified in writing signed by an authorized officer of LICENSOR.  Licensee agrees that it
* will not ship, transfer or export the Licensed Materials into any country, or use the Licensed Materials in any manner,
* prohibited by the United States Bureau of Industry and Security or any export laws, restrictions or regulations. This Agreement,
* and Licensee's rights and obligations herein, may not be assigned, subcontracted, delegated, or otherwise transferred by
* Licensee without LICENSOR's prior written consent, and any attempted assignment, subcontract, delegation, or transfer in
* violation of the foregoing will be null and void.   The terms of this Agreement shall be binding upon assignees.
*
*
*/

#define ICM4X6XX_DRIVER_VERSION 0x0127

/**
 * EDIT HISTORY FOR FILE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 *
 * when         version    who              what
 * --------     --------   ----------       ---------------------------------
 * 09/11/19     0x0127     TDK              Increased slew rate to accommodate lower clock rate
 * 05/08/19     0x0125     TDK              Add INT2 and HKD8150 suports
 * 05/08/19     0x0125     TDK              Turning factor for interval calculate algo
 * 05/08/19     0x0125     TDK              Fix some mistakes during code review
 * 05/08/19     0x0125     TDK              Fix stack overflow issue in non-DAE, caused by recursively flush fifo in reconfig hw
 * 02/13/19     0x0119     TDK              Add RTC mode support
 * 02/12/19     0x0119     TDK              Remove CP25 operation
 * 01/27/19     0x0118     TDK              Add freefall support
 * 01/17/19     0x0117     TDK              use wakeup sdc rate to cal fifo watermark
 * 01/13/19     0x0116     TDK              Fix incorrect report rate for FLUSH_ONLY clients, when there are some un-flush_only client
 *                                          already exists
 * 12/24/18     0x0115     TDK              remove reset first sample logic for flush_only in case of sample gap in multi-client test
 * 12/21/18     0x0115     TDK              optimize odr changing missing sample logic
 * 12/20/18     0x0115     TDK              stop dae temperature stream before starting it
 * 12/12/18     0x0114     TDK              add sampling missing checking, when ODR changed
 * 12/07/18     0x0113     TDK              add dual sensor support
 * 12/05/18     0x0113     TDK              flush fifo when dae_watermark and flush_period changed
 * 12/03/18     0x0111     TDK              add fifo_mode check for streaming mode setting
 * 11/26/18     0x0110     TDK              add function to track all request
 * 11/26/18     0x0110     TDK              add customer factory test for xiaomi
 * 11/26/18     0x0110     TDK              change source code directory name from hexagon to src
 * 11/21/18     0x0110     TDK              fix incorrect power mode used on event gated accel
 * 11/09/18     0x0110     TDK              Program the new FIFO configuration if config step is not idle
 * 11/08/18     0x0110     TDK              Optimize sample invertal calculation
 * 11/07/18     0x0110     TDK              Do not publish samples, when factory calibration is on.
 * 11/06/18     0x0110     TDK              Check odr changed(odr_changed) or not even config step is not idle
 * 11/05/18     0x0110     TDK              Optimize sensor state raw package method to avoid log package missing
 *                                          - use samll piece memory instead of max log size
 *                                          - submit this package quickly for quickly free this memory
 * 11/02/18     0x0110     TDK              Always use 16 bytes format for Havana on DAE-Disabled platform
 * 11/01/18     0x0110     TDK              Add sensor temperature raw data log package
 * 10/28/18     0x0110     TDK              Add A/G reported samples for debug
 * 10/25/18     0x0109     TDK              Add 40607 support
 * 10/25/18     0x0109     TDK              Fix high resolution bit setting
 * 10/24/18     0x0109     TDK              checking FIFO format when DAE is starting streaming, but responce
 *                                          is not deliver to driver
 * 10/19/18     0x0108     TDK              add factory calibration log package missing workaround, it seems
 *                                          SNS_CAL_EVENT and SNS_PHYSICAL_SENSOR_TEST_EVENT log package will
 *                                          be missing, if we send MSG to DAE after publish these two events.
 * 10/19/18     0x0108     TDK              add sanity check in SDC sensor driver in case of devide zero
 * 10/19/18     0x0108     TDK              change gyro start-up to 60ms when odr is 500Hz, that is also enough
 *                                          for data settling
 * 10/17/18     0x0108     TDK              don't send SNS_CAL_EVENT when receiving SNS_CAL_MSGID_SNS_CAL_RESET
 * 10/16/18     0x0108     TDK              send phy config event first, if there is another MD client request
 *                                          on md already enabled
 * 10/15/18     0x0108     TDK              limit flush_period equal to or greater than batch_period
 * 10/14/18     0x0108     TDK              if flush is already in process, ignore current flush request
 * 10/14/18     0x0108     TDK              send phy config event, if config step is not idle
 * 10/14/18     0x0108     TDK              use flush_hw instead of stop_dae_steaming, it seems frequent call
 *                                          stop_dae_steaming will cause dae no response
 * 10/13/18     0x0108     TDK              fix device watermark big than dae watermark issue when ODR is not
 *                                          pubished ODR
 * 10/13/18     0x0108     TDK              inialize both log_A/G_state_raw_info in case of mixed package to avoid crash
 * 10/12/18     0x0108     TDK              limit the last sample TS to DAE timestamp to avoid some negative interval
 * 10/11/18     0x0108     TDK              use fifo_header to cal package count in case of some mixed package
 * 10/10/18     0x0108     TDK              only change odr_changed variable in CONFIG_IDLE to avoid odr
 *                                          changing failed
 * 10/10/18     0x0108     TDK              stop temerature DAE steam before hardware self-test to avoid
 *                                          temperature glitch value
 * 10/10/18     0x0108     TDK              change the first sample TS calculator logic after odr changing to avoid
 *                                          some negative interval
 * 10/10/18     0x0108     TDK              disable tagging FSYNC flag to fix temperature resolution issue
 * 10/09/18     0x0108     TDK              disable watermark interrupt in flush_hw, resume it after flush_hw response
 * 10/09/18     0x0108     TDK              stop temperature DAE streaming sepratedly, when A/G is still streaming
 * 10/09/18     0x0108     TDK              Use DAE timestamp_type to judge the data from flush operation or
 *                                          normal interrupt.
 * 10/09/18     0x0108     TDK              Set accel odr to 0.0f in payload of DAE MSG DAE_SET_STREAMING_CONFIG,
 *                                          when there is no non-gate accel client
 * 10/08/18     0x0108     TDK              Add sns_icm4x6xx_ver.h file
 *
 **/
