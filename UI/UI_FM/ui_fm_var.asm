/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: fm_var.asm
Creator: zj.zong				Date: 2009-05-25
Description: define variable be used in fm mode
Others: 
Version: V0.1
History:
	V0.1	2009-05-25		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_FM_VAR_DEF


PUBLIC	b_fm_set_volume
PUBLIC	b_fm_volume_up
PUBLIC	b_fm_search_flg
PUBLIC	b_fm_stereo_flg
PUBLIC	b_headphone_plug_flg

PUBLIC	uc_fm_volume_level
PUBLIC	uc_fm_menu_ch_idx
PUBLIC	uc_fm_cur_ch_idx_bk
PUBLIC	ui_fm_menu_dis_freq
PUBLIC	ui_fm_dis_freq
PUBLIC	uc_menu_cur_idx
PUBLIC	uc_menu_cur_idx_bk
PUBLIC	uc_menu_total_num
PUBLIC	uc_menu_exit_cnt
PUBLIC	uc_fm_volume_cnt
PUBLIC	uc_headphone_plug_cnt
PUBLIC	uc_headphone_unplug_cnt
PUBLIC	uc_menu_top_idx



	BSEG	AT		28H.5
b_fm_set_volume:		DBIT	1
b_fm_volume_up:			DBIT	1
b_fm_search_flg:		DBIT	1
b_fm_stereo_flg:		DBIT	1
b_headphone_plug_flg:	DBIT	1


	XSEG	AT	7200H

uc_fm_volume_level:		DS		1
uc_fm_menu_ch_idx:		DS		1
uc_fm_cur_ch_idx_bk:	DS		1
uc_menu_cur_idx:		DS		1
uc_menu_cur_idx_bk:		DS		1
uc_menu_total_num:		DS		1
uc_menu_exit_cnt:		DS		1
uc_fm_volume_cnt:		DS		1
ui_fm_menu_dis_freq:	DS		2
ui_fm_dis_freq:			DS		2
uc_headphone_plug_cnt:	DS		1
uc_headphone_unplug_cnt:DS		1
uc_menu_top_idx:		DS		1






/*******************************************************************************
Dont Modify These Variable
the xdata address is mapping fm driver
********************************************************************************/

PUBLIC	uc_radio_status
PUBLIC	uc_fm_mode
PUBLIC	ui_fm_cur_freq
PUBLIC	uc_fm_cur_ch_idx
PUBLIC	uc_fm_ch_total_num
PUBLIC	uc_fm_signal_strength_val
PUBLIC	uc_fm_accumulative_val
PUBLIC	ui_fm_freq_limit_low
PUBLIC	ui_fm_freq_limit_high
PUBLIC	uc_fm_band
PUBLIC	uc_fm_band_bk
PUBLIC	ui_fm_preset_buf

	XSEG	AT	7250H
uc_radio_status:			DS		1
uc_fm_mode:					DS		1
ui_fm_cur_freq:				DS		2
uc_fm_cur_ch_idx:			DS		1
uc_fm_ch_total_num:			DS		1
uc_fm_signal_strength_val:	DS		1
uc_fm_accumulative_val:		DS		1
ui_fm_freq_limit_low:		DS		2
ui_fm_freq_limit_high:		DS		2
ui_fm_preset_buf:			DS		30*2
uc_fm_band:					DS		1
uc_fm_band_bk:				DS		1
uc_fm_drv_other_var:		DS		45	  
#endif
	END