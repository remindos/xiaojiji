/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_fm_var_ext.h
Creator: zj.zong				Date: 2009-05-25
Description: extern fm variable for "*.c"
Others:
Version: V0.1
History:
	V0.1	2009-05-25		zj.zong
			1. build this module
********************************************************************************/
#ifndef FM_VAR_EXT
#define FM_VAR_EXT

extern bit b_fm_set_volume;
extern bit b_fm_volume_up;
extern bit b_fm_search_flg;
extern bit b_fm_stereo_flg;
extern bit b_headphone_plug_flg;

extern uc8  xdata uc_fm_volume_level;


extern uc8  xdata uc_fm_menu_ch_idx;
extern uc8  xdata uc_fm_cur_ch_idx_bk;
extern ui16 xdata ui_fm_menu_dis_freq;
extern ui16 xdata ui_fm_dis_freq;

extern uc8  xdata uc_menu_cur_idx;
extern uc8  xdata uc_menu_cur_idx_bk;
extern uc8  xdata uc_menu_total_num;

extern uc8  xdata uc_menu_exit_cnt;
extern uc8  xdata uc_fm_volume_cnt;

extern uc8  xdata uc_headphone_plug_cnt;
extern uc8  xdata uc_headphone_unplug_cnt;
extern uc8  xdata uc_menu_top_idx;




/*******************************************************************************
Dont Modify These Variable
the xdata address is mapping fm driver
********************************************************************************/
extern uc8  xdata uc_radio_status;
extern uc8  xdata uc_fm_mode;
extern uc8  xdata uc_fm_cur_ch_idx;
extern uc8  xdata uc_fm_ch_total_num;
extern uc8  xdata uc_fm_signal_strength_val;
extern uc8  xdata uc_fm_accumulative_val;
extern uc8  xdata uc_fm_band;
extern uc8  xdata uc_fm_band_bk;
extern ui16 xdata ui_fm_freq_limit_low;
extern ui16 xdata ui_fm_freq_limit_high;
extern ui16 xdata ui_fm_preset_buf[];
extern ui16 xdata ui_fm_cur_freq;

#endif