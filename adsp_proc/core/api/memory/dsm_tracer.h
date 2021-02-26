/*===========================================================================
                           START OF TRACER VALUES
===========================================================================*/
typedef enum
{
  DSM_FREE              = 0,     /* item in free queue                     */
  DSM_ALLOCATED         = 0xFFFF,/* item has been alloced by new_buffer()  */

  /*-------------------------------------------------------------------------
    Over-the-air-RX-to-serial-TX direction values
  -------------------------------------------------------------------------*/
  DSM_RXC_RLP           = 1,    /* RX task RLP RX processing               */
  DSM_RLP_POST_RX_Q     = 3,    /* Queued from RLP post-RX to PPP RX queue */
  DSM_UM_PPP_ASY_IN     = 5,    /* Um 'ppp_asy_in' processing              */
  DSM_PPP_INCOMING      = 7,    /* PPP packet processing                   */
  DSM_TCP_RCVQ          = 9,    /* Application data queued to TCP RX queue */
  /*-------------------------------------------------------------------------
    Serial-RX-to-over-the-air-TX direction values                           
  -------------------------------------------------------------------------*/
  DSM_SCKT_TO_PS        = 2,    /* SOCKET to PS task                       */
  DSM_TCP_SNDQ          = 4,    /* TCP Send Queue                          */
  DSM_PPP_TX_WM         = 6,    /* PPP TX watermark                        */
  DSM_PS_TO_RLP_WM      = 8,    /* PS to RLP TX watermark                  */
  DSM_RLP_TX            = 10,   /* RLP TX processing                       */

  DSM_DS_FROM_PS_Q      = 11,   /* Application/command Queued to DS task   */
  DSM_CREATED_IN_ASY_IN = 12,   /* dsm item allocated in ppp_asy_in()      */
  DSM_OUT_OF_ASY_IN_EOP = 13,   /* item sent out of asy in FINDING_EOP     */
  DSM_RM_PPP_ASY_IN     = 14,   /* Um 'ppp_asy_in' processing              */
  DSM_OUT_OF_ASY_IN_EOP_HEAD = 15,/* head item out of asy_in FINDING_EOP   */
  DSM_UDP_RCVQ          = 16,   /* UDP receive queue                       */
  DSM_IP_SND_PROC       = 17,   /* IP_SEND processing                      */
  DSM_RLP_RESEQ         = 18,   /* RLP Re-sequencing queue                 */
  DSM_TCP_RESEQ         = 19,   /* TCP Re-sequencing queue                 */
  DSM_ICMP_RCVQ         = 20,   /* ICMP receive queue                      */
  DSM_IP_FRAG_LIST      = 21,   /* IP fragmentation list                   */
  DSM_UM_HDR_AN_AUTH_PPP_ASY_IN = 22, /* HDR AN 'ppp_asy_in' processing    */
  DSM_PS_META_INFO      = 23,   /* tracer for item used for ps meta info   */
  DSM_PS_EVENT_BUF      = 24,   /* for items used as event callback buffer */

  /*-------------------------------------------------------------------------
    Items created while using the hardware framer/deframer 
  -------------------------------------------------------------------------*/
  DSM_CREATED_IN_PPP_HW_FRAMER = 25,/* Created to get data from the framer */

  /*-------------------------------------------------------------------------
   PPP authentication values
  -------------------------------------------------------------------------*/
  DSM_CHAP_CHAL         = 26,
  DSM_CHAP_RESP         = 27
} dsm_tracer_enum_type;


/*===========================================================================
                            END OF TRACER VALUES
===========================================================================*/
