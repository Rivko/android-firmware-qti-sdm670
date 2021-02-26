/** vi: tw=128 ts=3 sw=3 et
@file gen_cb_ctxt.h
@brief This file contains the API details for the Protection Domain Monitor, API 2.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/api/gen_cb_ctxt.h#2 $
$DateTime: 2018/02/28 21:43:44 $
$Change: 15572026 $
$Author: pwbldsvc $
===============================================================================*/

typedef void (*gen_cb_ctxt_cb_t)( void * arg );

void gen_cb_ctxt_invoke_cb( gen_cb_ctxt_cb_t cb, void * arg );

boolean gen_cb_ctxt_special_context( gen_cb_ctxt_cb_t cb, void * arg );
