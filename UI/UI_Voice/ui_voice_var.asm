/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_voice_var.asm
Creator: zj.zong					Date: 2009-05-20
Description: define variable be used in voice mode
Others: 
Version: V0.1
History:
	V0.1	2009-05-20		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_VOICE_VAR_DEF

/*****************************************/
#ifndef VOICE_VAR
#define VOICE_VAR

PUBLIC	b_mpeg_init_ok
PUBLIC	b_dac_mute_flg
PUBLIC	b_voice_get_next_file
PUBLIC	b_voice_set_volume
PUBLIC	b_voice_dec_reset_flg
PUBLIC	b_voice_check_time_flg						;0:ui_voice_cur_time, 1:ui_voice_total_time
PUBLIC  b_voice_file_end
PUBLIC  b_voice_file_err
PUBLIC  b_voice_folder_exist
PUBLIC  b_voice_dis_ab_flicker
PUBLIC  b_headphone_plug_flg
PUBLIC  b_voice_startplay_flg

PUBLIC	uc_voice_fill_dbuf_flg

PUBLIC	uc_mpeg_state
PUBLIC	uc_voice_repeat_mode
PUBLIC	uc_voice_volume_level
PUBLIC	uc_voice_status
PUBLIC	uc_voice_status_bk
PUBLIC	uc_menu_cur_idx
PUBLIC	uc_menu_cur_idx_bk
PUBLIC	uc_menu_total_num
PUBLIC	uc_headphone_plug_cnt
PUBLIC	uc_headphone_unplug_cnt
PUBLIC	uc_mpegstate_cnt
PUBLIC	uc_filldata_cnt
PUBLIC	uc_voice_startplay_cnt

PUBLIC	uc_voice_time_cnt
PUBLIC	ui_voice_cur_time
PUBLIC	ui_voice_total_time
PUBLIC	ui_voice_replay_a_time
PUBLIC	ui_voice_replay_b_time
PUBLIC	ul_voice_replay_a_byte
PUBLIC	ul_voice_byterate
PUBLIC	ul_voice_wavdata_len
PUBLIC	ul_voice_samplerate
PUBLIC	ui_voice_audioformat
PUBLIC	uc_daci_sr
PUBLIC	uc_voice_replay_ab_flg
PUBLIC	uc_voice_ab_fliker_cnt
PUBLIC	uc_menu_top_idx
PUBLIC	b_fb_nofile_flg

PUBLIC	uc_menu_exit_cnt
PUBLIC	uc_refresh_voice_cnt
PUBLIC	uc_refresh_pic_num

PUBLIC	ui_voice_temp_var
PUBLIC	ui_voice_wav_idx
PUBLIC	ui_fb_total_num
PUBLIC	ui_voice_random_idx_bk

PUBLIC	uc_headphone_short_time
PUBLIC  uc_dynamicload_idx

PUBLIC	uc_fs_store_cluster1
PUBLIC	uc_fs_store_cluster2
PUBLIC	uc_fs_store_length1
PUBLIC	uc_fs_store_length2



	BSEG	AT		28H.5
b_mpeg_init_ok:				DBIT	1
b_dac_mute_flg:				DBIT	1
b_voice_get_next_file:		DBIT	1
b_voice_set_volume:			DBIT	1
b_voice_dis_ab_flicker:		DBIT	1
b_voice_dec_reset_flg:		DBIT	1
b_voice_file_end:			DBIT	1
b_voice_file_err:			DBIT	1
b_voice_check_time_flg:		DBIT	1
b_voice_folder_exist:		DBIT	1
b_headphone_plug_flg:		DBIT	1
b_voice_startplay_flg:		DBIT	1
b_fb_nofile_flg:		         DBIT    1


	DSEG	AT		14H
ui_voice_cur_time:			DS		2
ui_voice_total_time:		DS		2
uc_mpeg_state:				DS		1
uc_daci_sr:					DS		1
uc_voice_status:			DS		1
uc_voice_fill_dbuf_flg:		DS		1
ui_voice_wav_idx:			DS		2
ui_fb_total_num:			DS		2

 	DSEG	AT		2CH
uc_menu_cur_idx:			DS		1
uc_menu_total_num:			DS		1
uc_voice_volume_level:		DS		1

	XSEG	AT		7200H


uc_voice_repeat_mode:		DS		1
uc_voice_status_bk:			DS		1
uc_menu_cur_idx_bk:			DS		1
uc_menu_exit_cnt:			DS		1
uc_voice_time_cnt:			DS		1
ui_voice_replay_a_time:		DS		2
ui_voice_replay_b_time:		DS		2
ul_voice_replay_a_byte:		DS		4
ul_voice_byterate:			DS		4
ul_voice_wavdata_len:		DS		4
ul_voice_samplerate:		DS		4
ui_voice_audioformat:		DS		2
uc_voice_replay_ab_flg:		DS		1
uc_voice_ab_fliker_cnt:		DS		1
uc_refresh_pic_num:			DS		1
uc_menu_top_idx:			DS		1
uc_headphone_plug_cnt:		DS		1
uc_headphone_unplug_cnt:	DS		1
uc_mpegstate_cnt:			DS		1
uc_filldata_cnt:			DS		1
uc_voice_startplay_cnt:		DS   	1

uc_refresh_voice_cnt:		DS		1
ui_voice_temp_var:			DS		2
ui_voice_random_idx_bk:		DS		2
uc_headphone_short_time:	DS		1
uc_dynamicload_idx:			DS		1
uc_fs_store_length1:		DS		1
uc_fs_store_length2:		DS		1
uc_fs_store_cluster1:		DS		1
uc_fs_store_cluster2:		DS		1

#endif
#endif
	END
