#ifndef HAL_SPDIF_H_
#define HAL_SPDIF_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "typedef_drivers.h" 

typedef enum
{
    LINEAR = 0,
    NON_LINEAR = 1,
} spdif_datatype_t;

typedef enum
{
    SPDIF_NO_INT = 0,
    SPDIF_BLOCK_DONE_INT = (1 << 0),
    SPDIF_UNDERFLOW_INT = (1 << 1),
    SPDIF_IDLE_INT = (1 << 2),
    SPDIF_PAUSE_INT = (1 << 3),
    SPDIF_DROP_LATE_BLOCK_INT = (1 << 4),
    SPDIF_TS_DIFF_INT = (1 << 5),
    SPDIF_MC_ERR_INT = (1 << 6),
    SPDIF_ALL_INT = 0x7f,
} SPDIF_INT_t;


typedef enum
{
   SPDIF_INT_MUX_Q6_EN_APPS_DIS = 0,
   SPDIF_INT_MUX_Q6_DIS_APPS_EN = 1,
} spdif_int_mux_t;

typedef enum
{
   SPDIF_CH_STS_REG = 0,
   SPDIF_CH1_STS_A_CH2_STS_B = 1,
   SPDIF_CH1_AND_CH2_STS_A = 2,
   SPDIF_CH1_AND_CH2_STS_B = 3,
} spdif_ch_sts_en_t;

typedef enum
{
   SPDIF_USR_DATA_UNUSED = 0,
   SPDIF_CH1_USR_A_CH2_USR_B = 1,
   SPDIF_CH1_AND_CH2_USR_A = 2,
   SPDIF_CH1_AND_CH2_USR_B = 3,
} spdif_usr_en_t;

typedef enum
{
   SPDIF_BIT_WIDTH_16 = 0,
   SPDIF_BIT_WIDTH_24 = 8,
} spdif_bitwidth_t;

typedef enum
{
   SPDIF_FIFO_WATER_MARK_0 = 0,
   SPDIF_FIFO_WATER_MARK_1,
   SPDIF_FIFO_WATER_MARK_2,
   SPDIF_FIFO_WATER_MARK_3,
   SPDIF_FIFO_WATER_MARK_4,
   SPDIF_FIFO_WATER_MARK_5,
   SPDIF_FIFO_WATER_MARK_6,
   SPDIF_FIFO_WATER_MARK_7,
   SPDIF_FIFO_WATER_MARK_8, 
   SPDIF_FIFO_WATER_MARK_9,
   SPDIF_FIFO_WATER_MARK_10,
   SPDIF_FIFO_WATER_MARK_11,
   SPDIF_FIFO_WATER_MARK_12,
   SPDIF_FIFO_WATER_MARK_13,
   SPDIF_FIFO_WATER_MARK_14,
   SPDIF_FIFO_WATER_MARK_15, 
   SPDIF_FIFO_WATER_MARK_16,
} spdif_water_mark_t;

/********************************/
/***** Register types ***********/
/********************************/

typedef struct
{
    uint32 frame_size;
    boolean drop_late_frames;
    boolean en_inh_wtts;
} spdif_tx_cfg_t;

typedef struct
{
    spdif_ch_sts_en_t  ch_sts_e;
    spdif_usr_en_t  usr_data_e;
    boolean preamble_polarity;
    boolean parity_e;
} spdif_port_sel_t;

typedef struct
{
    spdif_bitwidth_t bit_width;
    boolean mono;
    boolean empty_fifo_repeat_mute;
    boolean soft_mute;
    boolean soft_mute_after_eof;
    boolean mono_sel;
} spdif_tx_port_cfg_t;

typedef struct
{
    boolean word_byte_swap;
    boolean dword_word_swap;
    spdif_water_mark_t   watermark;
    boolean fifo_pre_buf_en;
    boolean fifo_jump_rd_ptr;
    boolean left_justified;
} spdif_fifo_cntl_t;


typedef struct
{
    uint32 ch_sts_buf0;
    uint32 ch_sts_buf1;
    uint32 ch_sts_buf2;
    uint32 ch_sts_buf3;
    uint32 ch_sts_buf4;
    uint32 ch_sts_buf5;
} spdif_ch_sts_buf_t;
typedef struct
{
    uint32 usr_bits_buf0;
    uint32 usr_bits_buf1;
    uint32 usr_bits_buf2;
    uint32 usr_bits_buf3;
    uint32 usr_bits_buf4;
    uint32 usr_bits_buf5;
} spidf_usr_bits_buf_t;


typedef struct
{
    boolean crc_en;
    boolean crc_clr;
    uint32 crc_blk_cnt;
    uint32 mcd_word_cnt;
} spdif_crc_debug_t;

typedef struct
{
    boolean tx_block_done_int_e;
    boolean fifo_uflow_int_e;
    boolean idle_int_e;
    boolean pause_int_e;
    boolean drop_late_blk_int_e;
    boolean ts_diff_int_e;
    boolean mc_err_int_e;
    spdif_int_mux_t int_mask;
} spdif_int_cnt_t;


void hal_spdif_init(uint32 version, uint32 phy_base, uint32 virt_base, uint32 reset_phy_base, uint32 reset_virt_base);

void hal_spdif_soft_reset(void);

void hal_spdif_write_tx_cfg(spdif_tx_cfg_t tx_cfg);

void hal_spdif_write_tx_portsel(spdif_port_sel_t tx_port_sel);

void hal_spdif_write_tx_portcfg(spdif_tx_port_cfg_t tx_port_cfg);
//****************************************************************//

void hal_spdif_write_tx_fifocntl(spdif_fifo_cntl_t tx_fifo_cntl);

void hal_spdif_read_tx_release_level(uint32 *fill_level_release);

//****************************************************************//

void hal_spdif_write_tx_int_cntl(spdif_int_cnt_t tx_int_cntl);

//****************************************************************//

void hal_spdif_write_tx_nonlinear_bypass(boolean en);

void hal_spdif_write_tx_subbuf_fifo(uint32 * sub_buf_pntr, uint32 fsa, uint32 wttsFlag);

void hal_spdif_write_timestamp(uint64 timestamp);


//****************************************************************//

void hal_spdif_write_tx_chsts_a(spdif_ch_sts_buf_t tx_ch_sts);

void hal_spdif_write_tx_chsts_b(spdif_ch_sts_buf_t tx_ch_sts);

void hal_spdif_write_tx_usrbits_a(spidf_usr_bits_buf_t user_data);


void hal_spdif_write_tx_usrbits_b(spidf_usr_bits_buf_t user_data);


void hal_spdif_tx_enable(void);

void hal_spdif_tx_disable(void);

void hal_spdif_get_tsdiff(uint64 *tsdiff);

//****************************************************************//
void hal_spdif_write_tx_crc(spdif_crc_debug_t crc_inf);

void hal_spdif_tx_clear_crc(void);


void hal_spdif_read_tx_crc_mc(uint32 *crc);


void hal_spdif_crc_ready(uint32 *crc_ready);


void hal_spdif_read_tx_crc_blk(uint32 * crc_blk);

void hal_spdif_read_tx_stats(uint32 * nFrm, uint32 * nBlk);

//****************************************************************//

void hal_spdif_get_int_sta(uint32 *p_int_status);


void hal_spdif_write_int_sta_ack(uint32 val);

void hal_spdif_tx_pause(boolean pause);

void hal_spdif_tx_clear_all(void);

void hal_spdif_tx_abort(void);

void hal_spdif_tx_mute(boolean en);


#if defined(__cplusplus)
}
#endif

#endif /* HAL_SPDIF_H_ */
