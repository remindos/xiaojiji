/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_sys_var.asm
Creator: zj.zong			Date: 2009-06-03
Description: define variable be used in system mode
Others: 
Version: V0.1
History:
	V0.1	2009-06-03		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_SYS_VAR_DEF


PUBLIC ul_disk_encrypt_capacity
PUBLIC ul_disk_nomal_capacity
PUBLIC uc_disk_wr_state

PUBLIC	uc_menu_cur_idx_bk
PUBLIC 	uc_disk_state
PUBLIC	uc_menu_cur_idx
PUBLIC	uc_menu_top_idx

PUBLIC	uc_menu_total_num
PUBLIC	uc_menu_exit_cnt
PUBLIC	uc_disk_number
PUBLIC	uc_ebook_play_mode
PUBLIC	uc_ebook_play_gap
PUBLIC	uc_language_total_num
PUBLIC	uc_mp3_eq_mode
PUBLIC	uc_mp3_repeat_mode

PUBLIC	uc_mp3_replay_time
PUBLIC	uc_mp3_replay_gap
PUBLIC	uc_mp3_replay_mode
PUBLIC	uc_mp3_volume_level
PUBLIC	uc_rec_quality_level
PUBLIC	uc_voice_repeat_mode
PUBLIC	uc_voice_volume_level
PUBLIC	uc_fm_band
PUBLIC	uc_fm_volume_level
PUBLIC	uc_sys_ver_buf
PUBLIC	uc_sys_date_buf
PUBLIC	uc_amt_onoff_flg

DSEG		AT		14H
ul_disk_encrypt_capacity:	DS		4
ul_disk_nomal_capacity:		DS		4
uc_disk_wr_state:			DS		1
uc_menu_cur_idx:			DS		1
uc_menu_top_idx:			DS		1


	XSEG	AT		7200H

uc_menu_cur_idx_bk:		DS		1
uc_disk_state:			DS		1

uc_menu_total_num:		DS		1
uc_menu_exit_cnt:		DS		1
uc_disk_number:			DS		1
uc_ebook_play_mode:		DS		1
uc_ebook_play_gap:		DS		1
uc_language_total_num:	DS		1
uc_mp3_eq_mode:			DS		1
uc_mp3_repeat_mode:		DS		1
uc_mp3_replay_time:		DS		1
uc_mp3_replay_gap:		DS		1
uc_mp3_volume_level:	DS		1
uc_rec_quality_level:	DS		1
uc_voice_repeat_mode:	DS		1
uc_voice_volume_level:	DS		1
uc_fm_band:				DS		1
uc_fm_volume_level:		DS		1
uc_sys_ver_buf:			DS		10
uc_sys_date_buf:		DS		10
uc_mp3_replay_mode:		DS		1
uc_amt_onoff_flg:		DS		1

#endif
	END