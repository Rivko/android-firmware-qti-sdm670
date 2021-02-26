#include "smd_lite.h"


int32 smdl_init
(
  uint32 reserved
)
{
  return SMD_STATUS_NOT_INIT;
}


int32 smdl_term
(
  uint32 reserved
)
{
  return SMD_STATUS_NOT_INIT;
}

smdl_handle_type smdl_open
(
  const char       *name,
  smd_channel_type  edge,
  uint32            flags,
  uint32            fsize,
  smdl_callback_t   notify,
  void             *cb_data
)
{
  return NULL;
}

int32 smdl_open_ex
(
  smdl_open_params_type * params
)
{
  return SMD_STATUS_NOT_INIT;
}

int32 smdl_close
(
  smdl_handle_type port
)
{
  return SMD_STATUS_NOT_INIT;
}

int32 smdl_rx_peek
(
  smdl_handle_type port
)
{
  return SMD_STATUS_NOT_INIT;
}

int32 smdl_rx_peek_ext
(
  smdl_handle_type port,
  uint32          *pkt_len,
  uint32          *byte_avail
)
{
  return SMD_STATUS_NOT_INIT;
}

int32 smdl_tx_peek
(
  smdl_handle_type port
)
{
  return SMD_STATUS_NOT_INIT;
}

int32 smdl_read
(
  smdl_handle_type port,
  uint32           len,
  void            *buffer,
  uint32           flags
)
{
  return SMD_STATUS_NOT_INIT;
}

int32 smdl_write
(
  smdl_handle_type port,
  uint32           len,
  const void      *buffer,
  uint32           flags
)
{
  return SMD_STATUS_NOT_INIT;
}

int32 smdl_readv
(
  smdl_handle_type port,
  smdl_iovec_type *iovec,
  uint32           flags
)
{
  return SMD_STATUS_NOT_INIT;
}

int32 smdl_writev
(
  smdl_handle_type       port,
  const smdl_iovec_type *iovec,
  uint32                 flags
)
{
  return SMD_STATUS_NOT_INIT;
}

int32 smdl_sig_set
(
  smdl_handle_type port,
  smdl_sig_type    sig,
  boolean          value
)
{
  return SMD_STATUS_NOT_INIT;
}

int32 smdl_sig_get
(
  smdl_handle_type port,
  smdl_sig_type    sig
)
{
  return SMD_STATUS_NOT_INIT;
}

boolean smd_is_port_open
(
  const char       *name,
  smd_channel_type  channel_type
)
{
  return FALSE;
}








