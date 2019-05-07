/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_fm_var_ext.h
Creator: andyliu			Date: 2009-07-18
Description: extern fm variable for "*.c"
Others:
Version: V0.1
History:
	V0.1	2009-07-18		andyliu
			1. build this module
********************************************************************************/
#ifndef DRV_FM_VAR_EXT
#define DRV_FM_VAR_EXT


extern uc8  xdata uc_fm_mode;
extern uc8  xdata uc_radio_status;
extern uc8  xdata uc_fm_high_injection_flg;
extern uc8  xdata uc_fm_ch_total_num;
extern uc8  xdata uc_fm_cur_ch_idx;
extern uc8  xdata uc_fm_valid_station_flg;
extern uc8  xdata uc_fm_valid_cnt;
extern uc8  xdata uc_fm_signal_strength;
extern uc8  xdata uc_fm_ifc_er_cnt;
extern uc8  xdata uc_fm_signal_er_cnt;
extern uc8  xdata uc_fm_signal_th_acc;
extern uc8  xdata uc_fm_signal_th_acc_max;
extern uc8  xdata uc_fm_signal_strength_val;
extern uc8  xdata uc_fm_accumulative_val;

extern uc8  xdata uc_fm_band;
extern uc8  xdata uc_fm_band_bk;

extern uc8  xdata uc_fm_write_bytes[];
extern uc8  xdata uc_fm_read_bytes[];
extern ui16 xdata ui_fm_preset_buf[];

extern ui16 xdata ui_seek_value;
extern ui16 xdata ui_envelop_value;
extern ui16 xdata ui_fm_freq_limit_low;
extern ui16 xdata ui_fm_freq_limit_high;
extern ui16 xdata ui_fm_first_valid_freq;
extern ui16 xdata ui_fm_cur_freq_bk;
extern ui16 xdata ui_fm_cur_freq;




#endif