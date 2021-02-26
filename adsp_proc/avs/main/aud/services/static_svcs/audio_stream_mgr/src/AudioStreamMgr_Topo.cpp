/**
@file AudStreamMgr_Topo.cpp

@brief This file contains the implementation of utility functions for AudioStreamMgr.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_Topo.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/01/13   rb      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Util.h"

static const ASM_GraphNodeIndex DecNode = ASM_NODE_DEC;
static const ASM_GraphNodeIndex EncNode = ASM_NODE_ENC;
static const ASM_GraphNodeIndex PPNode = ASM_NODE_PP;
static const ASM_GraphNodeIndex SwitchNode = ASM_NODE_SWITCH;
static const ASM_GraphNodeIndex RxMtxNode = ASM_NODE_RX_MTX;
static const ASM_GraphNodeIndex TxMtxNode = ASM_NODE_TX_MTX;
static const ASM_GraphNodeIndex RxStRtrNode = ASM_NODE_RX_ST_RTR;
static const ASM_GraphNodeIndex TxStRtrNode = ASM_NODE_TX_ST_RTR;
//static const ASM_GraphNodeIndex CombinerNode = ASM_NODE_COMBINER;
//static const ASM_GraphNodeIndex SplitterNode = ASM_NODE_SPLITTER;
static const ASM_GraphNodeIndex RxGenNode = ASM_NODE_RX_GENERIC;
static const ASM_GraphNodeIndex TxGenNode = ASM_NODE_TX_GENERIC;
static const ASM_GraphNodeIndex INVD = ASM_NODE_MAX; //invalid

/*
 * StrmSvcs is used for connections and for create/destroy
 *
 * note that this array is indexed by ASM_TopoId
 *
 * pause2 svcs are ordered from downstream to upstream.
 * run svcs are ordered from upstream to downstream.
 * order of suspend or flush doesn't matter as streams are in pause already.
 *
 * If a node is going to present in more than 1 stream of a session, then it mustbe mentioned in each stream.
 * */
static const ASM_topo_t ASM_TOPO_TABLE[ASM_TOPO_MAX] = \
      {
   {//0
         ASM_TOPO_NT_MODE,\
         /*StrmSvcs*/               {{   DecNode,      PPNode,       EncNode,      INVD,       INVD,        INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  DecNode,      PPNode,       EncNode,      INVD,       INVD,        INVD}}, \
         /*SuspendSvcs*/            {{   DecNode,      PPNode,       EncNode,      INVD,       INVD,        INVD}}, \
         /*Pause1Svcs*/             {{   PPNode,       INVD,         INVD,         INVD,       INVD,        INVD}}, \
         /*Pause2Svcs*/             {{   DecNode,      PPNode,       INVD,         INVD,       INVD,        INVD}}, \
         /*RunSvcs*/                {{   DecNode,      PPNode,       EncNode,      INVD,       INVD,        INVD}}} \
   },

   {//1
         ASM_TOPO_RX_REGULAR, \
         /*StrmSvcs*/               {{   DecNode,      PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  DecNode,      PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   DecNode,      PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   PPNode,       INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   DecNode,      PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   DecNode,      RxMtxNode,    PPNode,       INVD,        INVD,       INVD}}} \
   },

   {//2
         ASM_TOPO_RX_ULL, \
         /*StrmSvcs*/               {{   DecNode,      RxGenNode,    INVD,         INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  DecNode,      INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   DecNode,      INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   INVD,         INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   DecNode,      INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   DecNode,      INVD,         INVD,         INVD,        INVD,       INVD}}} \
   },

   {//3
         ASM_TOPO_RX_GAPLESS, \
         /*StrmSvcs*/               {{   DecNode,      SwitchNode,   PPNode,       RxMtxNode,   INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  DecNode,      SwitchNode,   PPNode,       RxMtxNode,   INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   DecNode,      PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   PPNode,       INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   DecNode,      PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   DecNode,      RxMtxNode,    PPNode,       INVD,        INVD,       INVD}}} \
   },

   {//4
         ASM_TOPO_TX_REGULAR, \
         /*StrmSvcs*/               {{   TxMtxNode,    PPNode,       EncNode,      INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  TxMtxNode,    PPNode,       EncNode,      INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   TxMtxNode,    PPNode,       EncNode,      INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   PPNode,       INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   PPNode,       TxMtxNode,    INVD,         INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   TxMtxNode,    PPNode,       EncNode,      INVD,        INVD,       INVD}}} \
   },

   {//5
         ASM_TOPO_PCM2PCM_LOOPBACK, \
         /*StrmSvcs*/               {{   TxMtxNode,    PPNode,       RxMtxNode,    INVD,        INVD ,      INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  TxMtxNode,    PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   TxMtxNode,    PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   PPNode,       INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   TxMtxNode,    PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   TxMtxNode,    PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}} \
   },

   {//6
         ASM_TOPO_PCM2COMPR_LOOPBACK, \
         /*StrmSvcs*/               {{   TxMtxNode,    PPNode,       EncNode,      RxStRtrNode, INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  TxMtxNode,    PPNode,       EncNode,      RxStRtrNode, INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   TxMtxNode,    PPNode,       EncNode,      RxStRtrNode, INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   PPNode,       INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   PPNode,       TxMtxNode,    RxStRtrNode,  INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   TxMtxNode,    PPNode,       EncNode,      RxStRtrNode, INVD,       INVD}}} \
   },

   {//7
         ASM_TOPO_RX_COMPR, \
         /*StrmSvcs*/               {{   DecNode,      RxStRtrNode,  INVD,         INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  DecNode,      RxStRtrNode,  INVD,         INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   DecNode,      RxStRtrNode,  INVD,         INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   INVD,         INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   DecNode,      RxStRtrNode,  INVD,         INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   RxStRtrNode,  DecNode,      INVD,         INVD,        INVD,       INVD}}} \
   },

   {//8
         ASM_TOPO_TX_COMPR, \
         /*StrmSvcs*/               {{   TxStRtrNode,  EncNode,      INVD,         INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  TxStRtrNode,  EncNode,      INVD,         INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   INVD,         INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   INVD,         INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   INVD,         INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   TxStRtrNode,  EncNode,      INVD,         INVD,        INVD,       INVD}}} \
   },

   {//9
         ASM_TOPO_RX_PULL, \
         /*StrmSvcs*/               {{   DecNode,      PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  DecNode,      PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   DecNode,      PPNode,       RxMtxNode,    INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   PPNode,       INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   PPNode,       RxMtxNode,    DecNode,      INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   DecNode,      RxMtxNode,    PPNode,       INVD,        INVD,       INVD}}} \
   },

   {//10
         ASM_TOPO_TX_PUSH, \
         /*StrmSvcs*/               {{   TxMtxNode,    PPNode,       EncNode,      INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  TxMtxNode,    PPNode,       EncNode,      INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   EncNode,      PPNode,       TxMtxNode,    INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   PPNode,       INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   TxMtxNode,    PPNode,       EncNode,      INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   TxMtxNode,    PPNode,       EncNode,      INVD,        INVD,       INVD}}} \
   },

   {//11
         ASM_TOPO_COMPR2COMPR_GEN_LOOPBACK, \
         /*StrmSvcs*/               {{   TxStRtrNode,  DecNode,      PPNode,       EncNode,     RxStRtrNode,INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  TxStRtrNode,  DecNode,      PPNode,       EncNode,     RxStRtrNode,INVD}}, \
         /*SuspendSvcs*/            {{   TxStRtrNode,  DecNode,      PPNode,       EncNode,     RxStRtrNode,INVD}}, \
         /*Pause1Svcs*/             {{   PPNode,       INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   RxStRtrNode,  PPNode,       DecNode,      INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   TxStRtrNode,  DecNode,      PPNode,       EncNode,     RxStRtrNode,INVD}}} \
   },

   {//12
         ASM_TOPO_COMPR2COMPR_CONV_LOOPBACK, \
         /*StrmSvcs*/               {{   TxStRtrNode,  DecNode,      RxStRtrNode,  INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  TxStRtrNode,  DecNode,      RxStRtrNode,  INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   TxStRtrNode,  DecNode,      RxStRtrNode,  INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   INVD,         INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   RxStRtrNode,  DecNode,      INVD,         INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   TxStRtrNode,  DecNode,      RxStRtrNode,  INVD,        INVD,       INVD}}} \
   },

   {//13
         ASM_TOPO_COMPR2COMPR_DIRECT_LOOPBACK, \
         /*StrmSvcs*/               {{   TxStRtrNode,  RxStRtrNode,  INVD,         INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  TxStRtrNode,  RxStRtrNode,  INVD,         INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   TxStRtrNode,  RxStRtrNode,  INVD,         INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   INVD,         INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   RxStRtrNode,  INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   TxStRtrNode,  RxStRtrNode,  INVD,         INVD,        INVD,       INVD}}} \
   },

   {//14
         ASM_TOPO_COMPR2PCM_LOOPBACK, \
         /*StrmSvcs*/               {{   TxStRtrNode,  DecNode,      PPNode,       RxMtxNode,   INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  TxStRtrNode,  DecNode,      PPNode,       RxMtxNode,   INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   TxStRtrNode,  DecNode,      PPNode,       RxMtxNode,   INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   PPNode,       INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   RxMtxNode,    PPNode,       DecNode,      INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   TxStRtrNode,  DecNode,      PPNode,       RxMtxNode,   INVD,       INVD}}} \
   },

   {//15
         ASM_TOPO_RX_PULL_ULL, \
         /*StrmSvcs*/               {{   DecNode,      RxGenNode,    INVD,         INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  DecNode,      INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   DecNode,      INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   INVD,         INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   DecNode,      INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   DecNode,      INVD,         INVD,         INVD,        INVD,       INVD}}} \
   },

   {//16
         ASM_TOPO_TX_LLNP, \
         /*StrmSvcs*/               {{   TxGenNode,    EncNode,      INVD,         INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  TxGenNode,    EncNode,      INVD,         INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   EncNode,      INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   INVD,         INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   EncNode,      INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   EncNode,      INVD,         INVD,         INVD,        INVD,       INVD}}} \
   },

   {//17
         ASM_TOPO_TX_PUSH_LLNP, \
         /*StrmSvcs*/               {{   TxGenNode,    EncNode,      INVD,         INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  TxGenNode,    EncNode,      INVD,         INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   EncNode,      INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   INVD,         INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   EncNode,      INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   EncNode,      INVD,         INVD,         INVD,        INVD,       INVD}}} \
   },

   {//18
         ASM_TOPO_RX_ULLPP, \
         /*StrmSvcs*/               {{   DecNode,      PPNode,       RxGenNode,    INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  DecNode,      PPNode,       INVD,         INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   DecNode,      PPNode,       INVD,         INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   PPNode,       INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   DecNode,      PPNode,       INVD,         INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   DecNode,      PPNode,       INVD,         INVD,        INVD,       INVD}}} \
   },

   {//19
         ASM_TOPO_RX_PULL_ULLPP, \
         /*StrmSvcs*/               {{   DecNode,      PPNode,       RxGenNode,    INVD,        INVD,       INVD}}, \
         /*FlushAndQueryDelaySvcs*/ {{{  DecNode,      PPNode,       INVD,         INVD,        INVD,       INVD}}, \
         /*SuspendSvcs*/            {{   DecNode,      PPNode,       INVD,         INVD,        INVD,       INVD}}, \
         /*Pause1Svcs*/             {{   PPNode,       INVD,         INVD,         INVD,        INVD,       INVD}}, \
         /*Pause2Svcs*/             {{   DecNode,      PPNode,       INVD,         INVD,        INVD,       INVD}}, \
         /*RunSvcs*/                {{   DecNode,      PPNode,       INVD,         INVD,        INVD,       INVD}}} \
   },

};

ADSPResult ASM_init_stream_topo(ASM_TopoId topoId, AudioStreamMgr_StreamStateType *s)
{
   if (topoId >= ASM_TOPO_MAX )
   {
      return ADSP_EFAILED;
   }

   //sanity check for manual errors.
   if ( topoId != ASM_TOPO_TABLE[topoId].TopoId )
   {
      return ADSP_EFAILED;
   }

   s->pTopo = &ASM_TOPO_TABLE[topoId];

   return ADSP_EOK;
}

ASM_TopoId ASM_GetRxTopoId(uint16_t src, uint16_t sink)
{
   ASM_TopoId asm_topo = ASM_INVALID_TOPO;

   if ( (ASM_SOURCE_CLIENT == src) && (ASM_SINK_MATRIX_STRTR == sink) )
   {
      asm_topo = ASM_TOPO_RX_REGULAR;
   }
   else
   {
      asm_topo = ASM_INVALID_TOPO;
   }
   return asm_topo;
}

/**
ASM_TOPO_PCM2PCM_LOOPBACK,
ASM_TOPO_PCM2COMPR_LOOPBACK,
ASM_TOPO_COMPR2COMPR_GEN_LOOPBACK,
ASM_TOPO_COMPR2COMPR_CONV_LOOPBACK,
ASM_TOPO_COMPR2COMPR_DIRECT_LOOPBACK,
ASM_TOPO_COMPR2PCM_LOOPBACK,

    a. PCM to PCM: Tx matrix -> POPP -> Rx matrix
    b. PCM to compressed: Tx matrix -> POPP -> Encoder -> Rx stream router
    c. Compressed to compressed: Tx Stream router -> Decoder -> POPP -> Encoder -> Rx stream router
       (selected if source and sink non-PCM formats are different & converter mode is ASM_CONVERTER_MODE_NONE. OR converter mode is ASM_POST_PROCESS_CONVERTER_MODE)
    d. Compressed to compressed: Tx Stream router -> Decoder converter -> Rx stream router. (selected based on converter mode == ASM_TRANS_LOOPBACK_CONV_MODE_USE_CONVERTERS)
    e. Compressed to compressed:Tx Stream router -> Rx stream router (direct pass through).(selected if source and sink non-PCM formats are same and converter mode is ASM_CONVERTER_MODE_NONE.)
    f. Compressed to PCM: Tx stream router -> Decoder -> POPP -> Rx matrix.
    g. MIMO decoder.
 */

ASM_TopoId ASM_GetLoopbackTopoIdAndMode(uint16_t src, uint16_t sink, uint32_t src_fmt, uint32_t sink_fmt, uint32_t dec_conv_mode, asm_session_mode_t *mode)
{
   ASM_TopoId asm_topo = ASM_INVALID_TOPO;
   bool_t is_src_pcm = ASM_IsPcmFormat(src_fmt);
   bool_t is_sink_pcm = ASM_IsPcmFormat(sink_fmt);
   bool_t is_src_eq_sink = (src_fmt == sink_fmt);

   switch (src)
   {
   case ASM_SOURCE_MATRIX_STRTR:
   {
      switch (sink)
      {
      case ASM_SINK_MATRIX_STRTR:
      {
         if (is_src_pcm && is_sink_pcm)
         {
            asm_topo = ASM_TOPO_PCM2PCM_LOOPBACK;
            *mode = ASM_LOOPBACK_PCM_IN_PCM_OUT_MODE;
         }
         else if (!is_src_pcm && is_sink_pcm)
         {
            asm_topo = ASM_TOPO_COMPR2PCM_LOOPBACK;
            *mode = ASM_LOOPBACK_COMPRESSED_IN_PCM_OUT_MODE;
         }
         else if  (is_src_pcm && !is_sink_pcm)
         {
            asm_topo = ASM_TOPO_PCM2COMPR_LOOPBACK;
            *mode = ASM_LOOPBACK_PCM_IN_COMPRESSED_OUT_MODE;
         }
         else
         {
            switch (dec_conv_mode)
            {
            case ASM_CONVERTER_MODE_NONE:
            {
               if (is_src_eq_sink)
               {
                  asm_topo = ASM_TOPO_COMPR2COMPR_DIRECT_LOOPBACK;
                  *mode = ASM_LOOPBACK_COMPRESSED_IN_COMPRESSED_OUT_MODE;
               }
               else
               {
                  asm_topo = ASM_TOPO_COMPR2COMPR_GEN_LOOPBACK;
                  *mode = ASM_LOOPBACK_COMPRESSED_IN_COMPRESSED_OUT_MODE;
               }
               break;
            }
            case ASM_POST_PROCESS_CONVERTER_MODE:
            {
               asm_topo = ASM_TOPO_COMPR2COMPR_GEN_LOOPBACK;
               *mode = ASM_LOOPBACK_COMPRESSED_IN_COMPRESSED_OUT_MODE;
               break;
            }
            default:
            {
               asm_topo = ASM_TOPO_COMPR2COMPR_CONV_LOOPBACK;
               *mode = ASM_LOOPBACK_COMPRESSED_IN_COMPRESSED_OUT_MODE;
               break;
            }
            }
         }
         break;
      }
      default:
      {
         asm_topo = ASM_INVALID_TOPO;
         break;
      }
      }
      break;
   }
   case ASM_SOURCE_CLIENT:
   default:
   {
      asm_topo = ASM_INVALID_TOPO;
   }
   }
   return asm_topo;
}

